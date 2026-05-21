#pragma once
#define _USE_MATH_DEFINES
#include <array>
#include <string>
#include <string_view>
#include <cmath>
#include <utility>
#include "ecs.h"
#include "math3d.h"
#include "model.h"
#include "gameObject.h"
#include "view.h"
#include "components_type.h"


class Renderer {
public:
    Renderer(int width, int height, float fov, Entity cam);
    ~Renderer();

    void render(View<Transform, Model, Material>& view, Registry& registry);
    void clearBuffer();
    std::vector<std::string> renderLog;
private:
    int m_width;
    int m_height;
    float m_fov;
    char* m_screen;
    Color* m_screenBrightness; // Store brightness for ANSI color output
    float* m_zBuffer;
    std::string m_frameBuffer; // For building output with ANSI colors

    const std::string_view m_shades = ".:-=+*#%@";

    // ANSI greyscale color codes for shading (from black to white)
    static constexpr std::array<std::string_view, 9> m_shadeColors = {
        // "\033[38;5;232m", // black
        "\033[38;5;235m", // very dark grey
        "\033[38;5;237m", // dark grey
        "\033[38;5;240m", // medium-dark grey
        "\033[38;5;243m", // medium grey
        "\033[38;5;246m", // medium-light grey
        "\033[38;5;249m", // light grey
        "\033[38;5;251m", // lighter grey
        "\033[38;5;254m", // near white
        "\033[38;5;255m"  // white
    };



    float m_fovRad;
    float m_projectionScale;
    Entity m_cam;

    struct ClippedVertex {
        Vec3 viewPos;
        Vec3 worldPos;
        Vec3 normal;
    };

    struct V2F {
        Vec3 viewA;
        Vec3 viewB;
        Vec3 viewC;
        Vec3 worldA;
        Vec3 worldB;
        Vec3 worldC;
        Vec3 normalA;
        Vec3 normalB;
        Vec3 normalC;
        Color color;
        Transform& cam;
        Vec3 lightDir;
    };

    Vec2 project(Vec3 v);
    void drawPixel(int x, int y, char c);
    void drawTriangle(const V2F& v2f);
    char getShade(float intensity);
    ClippedVertex clipEdge(Vec3 viewA, Vec3 viewB, Vec3 worldA, Vec3 worldB, Vec3 normalA, Vec3 normalB);

    std::string getShadeWithColor(float intensity);
    std::string rgbToAnsi(Color color);

    inline float edge(Vec3 a, Vec3 b, Vec2i c) {
        return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    }

    inline bool sameSign(float a, float b, float c) {
        return (a >= 0 && b >= 0 && c >= 0) || (a <= 0 && b <= 0 && c <= 0);
    }
};