#include "renderer.h"
#include "window_polyfill.h"
#include <chrono>
#include <algorithm>
#include <iostream>
#include <sstream>

Renderer::Renderer(int width, int height, float fov) : m_width(width), m_height(height), m_fov(fov) {
    m_screen = new char[m_width * m_height];
    m_screenBrightness = new Color[m_width * m_height];
    m_zBuffer = new float[m_width * m_height];
    
    m_fovRad = m_fov * 0.5f * M_PI / 180.0f;
    m_projectionScale = 1.0f / std::tan(m_fovRad);
}

Renderer::~Renderer() {
    delete[] m_screen;
    delete[] m_screenBrightness;
    delete[] m_zBuffer;
}

void Renderer::render(const Camera& cam, View<Transform, Model, Material>& view) {
    const Vec3 lightDir = gmath::normalize({0, 1.0f, -1});

    auto t0 = std::chrono::high_resolution_clock::now();
    for (auto [transform, model, material] : view) {
        
        // std::vector<Vec2i> projectedVerts;
        // projectedVerts.reserve(model.nverts());

        renderLog.push_back("Rendering model with rotation" + transform.rotation.toString());
        
        
        for (int i = 0; i < model.nfaces(); ++i) {
            std::array<Vec3, 3> worldVerts;
            std::array<Vec3, 3> viewVerts;
            std::array<Vec3, 3> normals;
            for (int j = 0; j < 3; ++j) {
                Vec3 vert = model.vert(i, j);
                normals[j] = model.normal(i, j);
                normals[j] = gmath::rotateY(normals[j], transform.rotation.y);
                normals[j] = gmath::rotateX(normals[j], transform.rotation.x);
                normals[j] = gmath::rotateZ(normals[j], transform.rotation.z);
                // vert = gmath::rotateY(vert, angle);
                // vert = gmath::rotateX(vert, angle * 0.5f);
                // vert = gmath::rotateZ(vert, angle * 0.25f);
                
                // model -> world 
                vert = gmath::rotateY(vert, transform.rotation.y);
                vert = gmath::rotateX(vert, transform.rotation.x);
                vert = gmath::rotateZ(vert, transform.rotation.z);
                vert += transform.position;

                worldVerts[j] = vert;
                
                // world -> view
                vert -= cam.position;
                vert = gmath::rotateY(vert, -cam.rotation.y);
                vert = gmath::rotateX(vert, -cam.rotation.x);
                vert = gmath::rotateZ(vert, -cam.rotation.z);
                
                viewVerts[j] = vert;
            }
            
            Vec3 an = worldVerts[1] - worldVerts[0];
            Vec3 bn = worldVerts[2] - worldVerts[0];
            
            Vec3 normal = gmath::normalize(gmath::cross(an, bn));
            
            Vec3 viewDir = worldVerts[0] - cam.position;
            // Backface cull: skip faces whose normal points away from camera
            // if (gmath::dot(normal, viewDir) >= 0) continue;
            
            
            std::array<Vec3, 4> clipped;
            std::array<Vec3, 4> clippedNormals;
            int clippedCount = 0;
            for (int j = 0; j < 3; ++j) {
                Vec3 p1 = viewVerts[j];
                Vec3 p2 = viewVerts[(j + 1) % 3];
                Vec3 n1 = normals[j];
                Vec3 n2 = normals[(j + 1) % 3];
                
                if (p1.z > 0.1f) {
                    clipped[clippedCount] = p1;
                    clippedNormals[clippedCount] = n1;
                    clippedCount++;
                }
                
                if ((p1.z > 0.1f && p2.z <= 0.1f) || (p1.z <= 0.1f && p2.z > 0.1f)) {
                    auto clippedData = clipEdge(p1, p2, n1, n2);
                    clipped[clippedCount] = clippedData.first;
                    clippedNormals[clippedCount] = clippedData.second;
                    clippedCount++;
                }
            }

            if (clippedCount < 3) continue;

            std::array<Vec3, 4> projectedClipped;
            for (int i = 0; i < clippedCount; ++i) {
                auto p = project(clipped[i]);
                projectedClipped[i] = {p.x, p.y, clipped[i].z};
            }
            
            for (int j = 1; j < clippedCount - 1; ++j) {
                drawTriangle(
                    projectedClipped[0], projectedClipped[j], projectedClipped[j+1],
                    clippedNormals[0], clippedNormals[j], clippedNormals[j+1],
                    material.color,
                    lightDir);
            }
            
            // auto a = project(verts[0]);
            // auto b = project(verts[1]);
            // auto c = project(verts[2]);
            
            // drawPixel(ax, ay, '*');
            // drawPixel(bx, by, '*');
            // drawPixel(cx, cy, '*');
            // drawLine(ax, ay, bx, by);
            // drawLine(bx, by, cx, cy);
            // drawLine(cx, cy, ax, ay);
            // v = rotateY(v, angle);
            // v = rotateX(v, angle * 0.5f);
            // v.z += depth;
            // Vec2i p = project(v);
            // projectedVerts.emplace_back(project(v));
            // drawPixel(p.first, p.second, '@');
        }
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    
    
    // for (auto [a, b] : cubeEdges) {
    //     drawLine(projectedVerts[a].x, projectedVerts[a].y,
    //              projectedVerts[b].x, projectedVerts[b].y, '*');
    //     drawPixel(projectedVerts[a].x, projectedVerts[a].y, '@');
    //     drawPixel(projectedVerts[b].x, projectedVerts[b].y, '@');
    // }
    
    // int currColorIdx = -1;
    auto t2 = std::chrono::high_resolution_clock::now();
    Color lastColor{-1,-1,-1}; // Keep track of the last output color to prevent ANSI flooding

    // Begin synchronized update (Kitty / VTE / xterm support DEC private mode 2026).
    // This tells the terminal not to render until we send the closing sequence,
    // eliminating partial-frame tearing completely.
    // \033[?2026h = begin sync, \033[H = cursor to top-left (atomic with frame data)
    m_frameBuffer += "\033[?2026h\033[H";

    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            int idx = y * m_width + x;
            Color brightness = m_screenBrightness[idx];
            
            // Only output ANSI escape code if the color actually changed
            if (brightness != lastColor) {
                m_frameBuffer += rgbToAnsi(brightness);
                lastColor = brightness;
            }
            
            m_frameBuffer += m_screen[idx];
        }
        // \033[K erases from cursor to end-of-line, clearing any stale color/char
        // data left over from a previous wider frame.
        m_frameBuffer += "\033[K";
        if (y < m_height - 1) {
            m_frameBuffer += '\n';
        }
    }
    
    // Reset colors, end synchronized update.
    m_frameBuffer += "\033[0m\033[?2026l";

    fwrite(m_frameBuffer.data(), 1, m_frameBuffer.size(), stdout);
    fflush(stdout);
    // for (int y = 0; y < m_height; y++) {
    //     for (int x = 0; x < m_width; x++) {
    //         int idx = y * m_width + x;
    //         std::cout << m_screen[idx];
    //     }
    //     std::cout << '\n';
    // }
    // std::cout << m_frameBuffer << std::flush; // Ensure all output is printed immediately
    auto t3 = std::chrono::high_resolution_clock::now();
    
    float ms = std::chrono::duration<float, std::milli>(t1 - t0).count();
    float ms2 = std::chrono::duration<float, std::milli>(t3 - t2).count();
    std::cout << "\nFragment time: " << ms << " ms\n";
    std::cout << "Buffer output time: " << ms2 << " ms\033[K\n";
    
}

void Renderer::clearBuffer() {
    std::fill_n(m_screen, m_width * m_height, ' ');
    std::fill_n(m_screenBrightness, m_width * m_height, Color{0,0,0});
    std::fill_n(m_zBuffer, m_width * m_height, std::numeric_limits<float>::infinity());
    m_frameBuffer.clear();
    m_frameBuffer.reserve(m_width * m_height * 20); // Reserve enough space for ANSI colored output
    renderLog.clear();

    #ifdef _WIN32
    setCursorPosition(0, 0);
    #endif
    // Cursor reset is prepended to the frameBuffer in render() so it's
    // sent atomically in one fwrite — avoids cout/fwrite stream interleaving.
}

Vec2 Renderer::project(Vec3 v) {
    float x = (v.x / v.z) * m_projectionScale;
    float y = (v.y / v.z) * m_projectionScale;
    
    float screenX = (x * 0.5f + 0.5f) * m_width;
    float screenY = (-y * 0.5f + 0.5f) * m_height;
    
    return {screenX, screenY};
}


void Renderer::drawPixel(int x, int y, char c) {
    if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
        m_screen[y * m_width + x] = c;
    }
}

void Renderer::drawTriangle(Vec3 a, Vec3 b, Vec3 c, Vec3 na, Vec3 nb, Vec3 nc, Color color, Vec3 lightDir) {
    // bounding box
    int minX = std::max(0, static_cast<int>(std::floor(std::min({a.x, b.x, c.x}))));
    int maxX = std::min(m_width - 1, static_cast<int>(std::ceil(std::max({a.x, b.x, c.x}))));
    int minY = std::max(0, static_cast<int>(std::floor(std::min({a.y, b.y, c.y}))));
    int maxY = std::min(m_height - 1, static_cast<int>(std::ceil(std::max({a.y, b.y, c.y}))));
    
    
    float area = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    if (area == 0) return; // Degenerate triangle
    
    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {

            Vec2i p{x, y};

            float w0 = edge(b, c, p);
            float w1 = edge(c, a, p);
            float w2 = edge(a, b, p);

            if (!sameSign(w0, w1, w2)) continue; // Outside the triangle

            float invArea = 1.0f / area;
            w0 *= invArea;
            w1 *= invArea;
            w2 *= invArea;

            float z = a.z * w0 + b.z * w1 + c.z * w2;

            int idx = y * m_width + x;
            
            if (m_zBuffer[idx] > z) {
                m_zBuffer[idx] = z;

                Vec3 normal = gmath::normalize(na * w0 + nb * w1 + nc * w2);

                float brightness = std::max(0.0f, gmath::dot(normal, lightDir)) + 0.25f; // Add ambient term

                Color diffuse = color * brightness;

                m_screenBrightness[idx] = diffuse;
                drawPixel(x, y, getShade(brightness));
            }
        }
    }
}

char Renderer::getShade(float intensity) {
    int index = std::min(static_cast<int>(std::round(intensity * (m_shades.size() - 1))), static_cast<int>(m_shades.size() - 1));
    return m_shades[index];
}

std::string Renderer::getShadeWithColor(float intensity) {
    int index = std::min(static_cast<int>(intensity * (m_shades.size() - 1)), static_cast<int>(m_shades.size() - 1));
    return std::string(m_shadeColors[index]) + m_shades[index] + "\033[0m";
}

std::pair<Vec3, Vec3> Renderer::clipEdge(Vec3 a, Vec3 b, Vec3 na, Vec3 nb) {
    float t = (0.1f - a.z) / (b.z - a.z);
    Vec3 p = {
        a.x + t * (b.x - a.x),
        a.y + t * (b.y - a.y),
        0.1f
    };
    Vec3 n = gmath::normalize({
        na.x + t * (nb.x - na.x),
        na.y + t * (nb.y - na.y),
        na.z + t * (nb.z - na.z)
    });
    return {p, n};
}

std::string Renderer::rgbToAnsi(Color color) {
    auto to255 = [](float c) {
        c = std::clamp(c, 0.0f, 1.0f);
        return static_cast<int>(std::round(c * 255));
    };

    // Convert RGB [0,1] to 0-255 range
    int r = to255(color.r);
    int g = to255(color.g);
    int b = to255(color.b);

    // Use standard ANSI foreground code; some strict terminals struggle with repeated \033[38;2; sequences 
    // flooding the channel per space character, leading to buffering jitters, desync, or spurious newlines.
    // Try to cache color output entirely if the same color repeats, but if we must output it:
    
    // Instead of std::to_string repeatedly, we format an ANSI escape fast:
    char buf[32];
    snprintf(buf, sizeof(buf), "\033[38;2;%d;%d;%dm", r, g, b);
    return std::string(buf);
}