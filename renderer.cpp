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
    const Vec3 lightDir = gmath::normalize({0, -1.0f, 1});

    auto t0 = std::chrono::high_resolution_clock::now();
    for (auto [transform, model, material] : view) {
        
        // std::vector<Vec2i> projectedVerts;
        // projectedVerts.reserve(model.nverts());

        
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
                vert *= transform.scale;
                vert += transform.position;

                worldVerts[j] = vert;
                
                // world -> view
                vert -= cam.position;
                vert = gmath::rotateY(vert, -cam.rotation.y);
                vert = gmath::rotateX(vert, -cam.rotation.x);
                vert = gmath::rotateZ(vert, -cam.rotation.z);
                
                viewVerts[j] = vert;
            }
            
            std::array<Vec3, 4> clipped;
            std::array<Vec3, 4> clippedWorld;
            std::array<Vec3, 4> clippedNormals;
            int clippedCount = 0;
            for (int j = 0; j < 3; ++j) {
                Vec3 p1 = viewVerts[j];
                Vec3 p2 = viewVerts[(j + 1) % 3];
                Vec3 w1 = worldVerts[j];
                Vec3 w2 = worldVerts[(j + 1) % 3];
                Vec3 n1 = normals[j];
                Vec3 n2 = normals[(j + 1) % 3];
                
                if (p1.z > 0.1f) {
                    clipped[clippedCount] = p1;
                    clippedWorld[clippedCount] = w1;
                    clippedNormals[clippedCount] = n1;
                    clippedCount++;
                }
                
                if ((p1.z > 0.1f && p2.z <= 0.1f) || (p1.z <= 0.1f && p2.z > 0.1f)) {
                    ClippedVertex clippedData = clipEdge(p1, p2, w1, w2, n1, n2);
                    clipped[clippedCount] = clippedData.viewPos;
                    clippedWorld[clippedCount] = clippedData.worldPos;
                    clippedNormals[clippedCount] = clippedData.normal;
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
                V2F v2f = {
                    .viewA = projectedClipped[0],
                    .viewB = projectedClipped[j],
                    .viewC = projectedClipped[j+1],
                    .worldA = clippedWorld[0],
                    .worldB = clippedWorld[j],
                    .worldC = clippedWorld[j+1],
                    .normalA = clippedNormals[0],
                    .normalB = clippedNormals[j],
                    .normalC = clippedNormals[j+1],
                    .color = material.color,
                    .camPos = cam.position,
                    .lightDir = lightDir
                };
                drawTriangle(v2f);
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
    // Position cursor just below the render area so debug lines don't get overwritten each frame
    std::cout << "\033[" << (m_height + 1) << ";1HFragment time: " << ms << " ms\033[K\n";
    std::cout << "Buffer output time: " << ms2 << " ms\033[K\n";
    std::cout << "Frame Buffer size: " << m_frameBuffer.size() << " bytes\033[K\n";
    
}

void Renderer::clearBuffer() {
    std::fill_n(m_screen, m_width * m_height, ' ');
    std::fill_n(m_screenBrightness, m_width * m_height, Color{0,0,0});
    std::fill_n(m_zBuffer, m_width * m_height, std::numeric_limits<float>::infinity());
    m_frameBuffer.clear();
    m_frameBuffer.reserve(m_width * m_height * 2); // Reserve enough space for ANSI colored output
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

void Renderer::drawTriangle(const V2F& v2f) {
    // bounding box
    int minX = std::max(0, static_cast<int>(std::floor(std::min({v2f.viewA.x, v2f.viewB.x, v2f.viewC.x}))));
    int maxX = std::min(m_width - 1, static_cast<int>(std::ceil(std::max({v2f.viewA.x, v2f.viewB.x, v2f.viewC.x}))));
    int minY = std::max(0, static_cast<int>(std::floor(std::min({v2f.viewA.y, v2f.viewB.y, v2f.viewC.y}))));
    int maxY = std::min(m_height - 1, static_cast<int>(std::ceil(std::max({v2f.viewA.y, v2f.viewB.y, v2f.viewC.y}))));
    
    
    float area = (v2f.viewB.x - v2f.viewA.x) * (v2f.viewC.y - v2f.viewA.y) - (v2f.viewB.y - v2f.viewA.y) * (v2f.viewC.x - v2f.viewA.x);
    if (area == 0) return; // Degenerate triangle
    
    float invArea = 1.0f / area;

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {

            Vec2i p{x, y};

            float w0 = edge(v2f.viewB, v2f.viewC, p);
            float w1 = edge(v2f.viewC, v2f.viewA, p);
            float w2 = edge(v2f.viewA, v2f.viewB, p);

            if (!sameSign(w0, w1, w2)) continue; // Outside the triangle

            w0 *= invArea;
            w1 *= invArea;
            w2 *= invArea;

            float z = v2f.viewA.z * w0 + v2f.viewB.z * w1 + v2f.viewC.z * w2;

            int idx = y * m_width + x;
            
            if (m_zBuffer[idx] > z) {
                m_zBuffer[idx] = z;

                Vec3 fragPos = v2f.worldA * w0 + v2f.worldB * w1 + v2f.worldC * w2;
                Vec3 camDir = gmath::normalize(v2f.camPos - fragPos);

                Vec3 normal = gmath::normalize(v2f.normalA * w0 + v2f.normalB * w1 + v2f.normalC * w2);

                float ambient = 0.4f;
                float diffuse = std::max(0.0f, gmath::dot(normal, -v2f.lightDir));
                float lit = ambient + (1.0f - ambient) * diffuse;

                float shininess = 32.0f;
                float specular = 0.0f;
                if (diffuse > 0.0f) {
                    Vec3 reflected = gmath::reflect(v2f.lightDir, normal);
                    specular = std::pow(std::max(0.0f, gmath::dot(camDir, reflected)), shininess);
                }

                float shade = std::clamp(lit + specular, 0.0f, 1.0f);

                m_screenBrightness[idx] = v2f.color * lit + Color{1.0f, 1.0f, 1.0f} * specular;
                drawPixel(x, y, getShade(shade));
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

Renderer::ClippedVertex Renderer::clipEdge(Vec3 viewA, Vec3 viewB, Vec3 worldA, Vec3 worldB, Vec3 normalA, Vec3 normalB) {
    float t = (0.1f - viewA.z) / (viewB.z - viewA.z);
    Vec3 clippedView = {
        viewA.x + t * (viewB.x - viewA.x),
        viewA.y + t * (viewB.y - viewA.y),
        0.1f
    };
    Vec3 clippedWorld = {
        worldA.x + t * (worldB.x - worldA.x),
        worldA.y + t * (worldB.y - worldA.y),
        worldA.z + t * (worldB.z - worldA.z)
    };
    Vec3 clippedNormal = gmath::normalize({
        normalA.x + t * (normalB.x - normalA.x),
        normalA.y + t * (normalB.y - normalA.y),
        normalA.z + t * (normalB.z - normalA.z)
    });
    return {clippedView, clippedWorld, clippedNormal};
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