#pragma once
#define _USE_MATH_DEFINES
#include <array>
#include <string>
#include <string_view>
#include <cmath>
#include "ecs.h"
#include "math3d.h"
#include "model.h"
#include "camera.h"
#include "gameObject.h"
#include "view.h"


class Renderer {
public:
    Renderer(int width, int height, float fov);
    ~Renderer();

    void render(const Camera& cam, View<Transform, Model>& view);
    void clearBuffer();
private:
    int m_width;
    int m_height;
    float m_fov;
    char* m_screen;
    float* m_screenBrightness; // Store brightness for ANSI color output
    float* m_zBuffer;
    std::string m_frameBuffer; // For building output with ANSI colors

    const std::string_view m_shades = " .:-=+*#%@";

    // ANSI greyscale color codes for shading (from black to white)
    static constexpr std::array<std::string_view, 10> m_shadeColors = {
        "\033[38;5;232m", // black
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

    Vec2 project(Vec3 v);
    void drawPixel(int x, int y, char c);
    void drawTriangle(Vec3 a, Vec3 b, Vec3 c, float brightness);
    char getShade(float intensity);
    std::string getShadeWithColor(float intensity);
};