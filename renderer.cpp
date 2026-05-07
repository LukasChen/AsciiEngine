#include "renderer.h"
#include "window_polyfill.h"
#include <chrono>
#include <algorithm>
#include <iostream>

Renderer::Renderer(int width, int height, float fov) : m_width(width), m_height(height), m_fov(fov) {
    m_screen = new char[m_width * m_height];
    m_screenBrightness = new float[m_width * m_height];
    m_zBuffer = new float[m_width * m_height];
    
    m_fovRad = m_fov * 0.5f * M_PI / 180.0f;
    m_projectionScale = 1.0f / std::tan(m_fovRad);
}

Renderer::~Renderer() {
    delete[] m_screen;
    delete[] m_screenBrightness;
    delete[] m_zBuffer;
}

void Renderer::render(const Camera& cam, const std::pair<ComponentArr<Transform>&, ComponentArr<Model>&>& componentArrs) {
    const Vec3 lightDir = gmath::normalize({0, 0.5f, -1});

    auto t0 = std::chrono::high_resolution_clock::now();
    for (auto& [entity, transformIndex] : componentArrs.first.entityToIndex) {
        const auto* model = componentArrs.second.getComponent(entity);
        if (!model) continue;

        const auto& transform = componentArrs.first.data[transformIndex];
        
        // std::vector<Vec2i> projectedVerts;
        // projectedVerts.reserve(model.nverts());
        
        
        for (int i = 0; i < model->nfaces(); ++i) {
            std::array<Vec3, 3> screenVerts;
            std::array<Vec3, 3> projectedVerts;
            bool behindCamera = false;
            for (int j = 0; j < 3; ++j) {
                Vec3 vert = model->vert(i, j);
                // vert = gmath::rotateY(vert, angle);
                // vert = gmath::rotateX(vert, angle * 0.5f);
                // vert = gmath::rotateZ(vert, angle * 0.25f);
                
                // model -> world 
                vert = gmath::rotateY(vert, transform.rotation.y);
                vert = gmath::rotateX(vert, transform.rotation.x);
                vert = gmath::rotateZ(vert, transform.rotation.z);
                vert += transform.position;
                
                
                // world -> view
                vert -= cam.position;
                vert = gmath::rotateY(vert, -cam.rotation.y);
                vert = gmath::rotateX(vert, -cam.rotation.x);
                vert = gmath::rotateZ(vert, -cam.rotation.z);
                
                screenVerts[j] = vert;
                
                if (screenVerts[j].z <= 0.1f) {
                    behindCamera = true; break; 
                }
                // view -> projection
                auto projected = project(screenVerts[j]);
                projectedVerts[j] = {projected.x, projected.y, screenVerts[j].z};
            }
            if (behindCamera) continue;
            
            Vec3 an = screenVerts[1] - screenVerts[0];
            Vec3 bn = screenVerts[2] - screenVerts[0];
            
            Vec3 normal = gmath::normalize(gmath::cross(an, bn));
            
            // Backface cull: skip faces whose normal points away from camera (+z direction)
            if (normal.z > 0) continue;
            
            float brightness = std::max(0.0f, gmath::dot(normal, lightDir));
            // const float ambient = 0.15f;
            // brightness = ambient + (1.0f - ambient) * brightness;
            
            drawTriangle(projectedVerts[0], projectedVerts[1], projectedVerts[2], brightness);
            
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
    
    #ifdef _WIN32
    setCursorPosition(0, 0);
    #else
    std::cout << "\033[H";
    #endif
    
    int currColorIdx = -1;
    auto t2 = std::chrono::high_resolution_clock::now();
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            int idx = y * m_width + x;
            float brightness = m_screenBrightness[idx];
            int colorIdx = std::min(static_cast<int>(std::round(brightness * (m_shades.size() - 1))), static_cast<int>(m_shades.size() - 1));
            if (currColorIdx != colorIdx) {
                m_frameBuffer += m_shadeColors[colorIdx];
                currColorIdx = colorIdx;
            }
            m_frameBuffer += m_screen[idx];
            // output.append("\033[0m", 4);
        }
        m_frameBuffer += '\n';
    }
    
    m_frameBuffer += "\033[0m"; // Reset color at the end
    
    fwrite(m_frameBuffer.data(), 1, m_frameBuffer.size(), stdout);
    auto t3 = std::chrono::high_resolution_clock::now();
    
    float ms = std::chrono::duration<float, std::milli>(t1 - t0).count();
    float ms2 = std::chrono::duration<float, std::milli>(t3 - t2).count();
    std::cout << "Fragment time: " << ms << " ms\n";
    std::cout << "Buffer output time: " << ms2 << " ms\n";
    
}

void Renderer::clearBuffer() {
    std::fill_n(m_screen, m_width * m_height, ' ');
    std::fill_n(m_screenBrightness, m_width * m_height, 0.0f);
    std::fill_n(m_zBuffer, m_width * m_height, std::numeric_limits<float>::infinity());
    m_frameBuffer.clear();
    m_frameBuffer.reserve(m_width * m_height * 20); // Reserve enough space for ANSI colored output
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

void Renderer::drawTriangle(Vec3 a, Vec3 b, Vec3 c, float brightness) {
    // bounding box
    int minX = std::max(0, static_cast<int>(std::floor(std::min({a.x, b.x, c.x}))));
    int maxX = std::min(m_width - 1, static_cast<int>(std::ceil(std::max({a.x, b.x, c.x}))));
    int minY = std::max(0, static_cast<int>(std::floor(std::min({a.y, b.y, c.y}))));
    int maxY = std::min(m_height - 1, static_cast<int>(std::ceil(std::max({a.y, b.y, c.y}))));
    
    
    float area = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    if (area == 0) return; // Degenerate triangle
    
    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            float w0 = (b.x - a.x) * (y - a.y) - (b.y - a.y) * (x - a.x);
            float w1 = (c.x - b.x) * (y - b.y) - (c.y - b.y) * (x - b.x);
            float w2 = (a.x - c.x) * (y - c.y) - (a.y - c.y) * (x - c.x);
            
            if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0)) {
                // float z = (a.z * w0 + b.z * w1 + c.z * w2) / area;
                float z = (a.z + b.z + c.z) / 3.0f; // Simple average for depth
                
                if (m_zBuffer[y * m_width + x] > z) {
                    m_zBuffer[y * m_width + x] = z;
                    m_screenBrightness[y * m_width + x] = brightness;
                    drawPixel(x, y, getShade(brightness));
                }
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
