#pragma once
#ifdef _WIN32
#include <windows.h>
#endif

enum class Axis {
    Horizontal,
    Vertical,
    LookHorizontal,
    LookVertical
};

class Input {
public:
    static bool isKeyPressed(int key) {
#ifdef _WIN32
        return (GetAsyncKeyState(key) & 0x8000) != 0;
#endif
    }

    static float GetAxis(Axis axis) {
        if (axis == Axis::Horizontal) {
            return (isKeyPressed('D') ? 1.0f : 0.0f) - (isKeyPressed('A') ? 1.0f : 0.0f);
        } else if (axis == Axis::Vertical) {
            return (isKeyPressed('W') ? 1.0f : 0.0f) - (isKeyPressed('S') ? 1.0f : 0.0f);
        } else if (axis == Axis::LookHorizontal) {
            return (isKeyPressed(VK_RIGHT) ? 1.0f : 0.0f) - (isKeyPressed(VK_LEFT) ? 1.0f : 0.0f);
        } else if (axis == Axis::LookVertical) {
            return (isKeyPressed(VK_UP) ? 1.0f : 0.0f) - (isKeyPressed(VK_DOWN) ? 1.0f : 0.0f);
        }
        return 0.0f;
    }
};
