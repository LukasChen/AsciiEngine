#define _USE_MATH_DEFINES

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <array>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <string_view>
#include <fstream>
#include <sstream>
#include "math3d.h"
#include "ecs.h"
#include "model.h"
#include "camera.h"
#include "renderer.h"
#include "gameObject.h"
#include "scene.h"
#include "registry.h"

#ifdef _WIN32
#include "window_polyfill.h"
#endif

struct SinComponent {
    float startY;
};

Registry registry;

const int WIDTH = 80;
const int HEIGHT = 40;
const float fov = 60.0f;

bool parseVec3(std::istringstream& ss, Vec3& out) {
    char comma1, comma2;
    ss >> out.x >> comma1 >> out.y >> comma2 >> out.z;
    return comma1 == ',' && comma2 == ',';
}

Entity loadSceneFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open scene file: " << filename << std::endl;
        throw std::runtime_error("Failed to open scene file");
    }
    std::cout << "Loading scene from file: " << filename << std::endl;

    std::string line;
    std::vector<GameObject> sceneObjects;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string modelFilename;
        Transform transform;
        if (!(iss >> modelFilename)) {
            std::cerr << "Invalid line in scene file: " << line << std::endl;
            continue;
        }

        if (!parseVec3(iss, transform.position)) {
            std::cerr << "Invalid position format in line: " << line << std::endl;
            continue;
        }

        if (!parseVec3(iss, transform.rotation)) {
            std::cerr << "Invalid rotation format in line: " << line << std::endl;
            continue;
        }
        
        try {
            Entity entity = registry.create();
            registry.get<Transform>().addComponent(entity, std::move(transform));
            registry.get<Model>().emplaceComponent(entity, modelFilename);

            // Model model(modelFilename);
            // std::cout << "Loaded model: " << modelFilename << " with position " << transform.position << "\n";
            // sceneObjects.emplace_back(std::move(model), std::move(transform));
        } catch (const std::exception& e) {
            std::cerr << "Error loading model from file: " << modelFilename << ". Error: " << e.what() << std::endl;
        }

    }
    return registry.getEntiityCount();
}

void spinObjects(ComponentArr<Transform>& transforms, float angle) {
    for (auto& [entity, transformIndex] : transforms.entityToIndex) {
        auto& transform = transforms.data[transformIndex];
        transform.rotation.y = angle;
        transform.rotation.x = angle * 0.5f;
        transform.rotation.z = angle * 0.25f;
    }
}

void updateCamera(Camera& cam) {
    if (GetAsyncKeyState(VK_UP) & 0x8000) {
        cam.rotation.x -= 0.05f;
    }
    if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
        cam.rotation.x += 0.05f;
    }
    if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
        cam.rotation.y -= 0.05f;
    }
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        cam.rotation.y += 0.05f;
    }

    if (GetAsyncKeyState('W') & 0x8000) {
        cam.position += gmath::normalize(cam.forward()) * 0.1f;
    }
    if (GetAsyncKeyState('S') & 0x8000) {
        cam.position += gmath::normalize(cam.forward()) * -0.1f;
    }
    if (GetAsyncKeyState('A') & 0x8000) {
        cam.position += gmath::normalize(cam.right()) * -0.1f;
    }
    if (GetAsyncKeyState('D') & 0x8000) {
        cam.position += gmath::normalize(cam.right()) * 0.1f;
    }
    if (GetAsyncKeyState('E') & 0x8000) {
        cam.position.y += 0.01f;
    }
    if (GetAsyncKeyState('Q') & 0x8000) {
        cam.position.y -= 0.01f;
    }
}

void SinAnimSystem(ComponentArr<SinComponent>& sinComponents, ComponentArr<Transform>& transforms, float time) {
    for (auto& [entity, sinIndex] : sinComponents.entityToIndex) {
        auto& sinComp = sinComponents.data[sinIndex];
        auto* transform = transforms.tryGet(entity);
        if (transform) {
            transform->position.y = sinComp.startY + std::sin(time * 5) * 0.5f;
        }
    }
}


int main() {
#ifdef _WIN32
    setupWindowCursor();
#endif

    std::cout << "\033[2J"; // Clear screen

    Entity count = loadSceneFile("scene.txt");
    // float startY = scene.transforms.getComponent(1)->position.y;
    // SinComponent sinComp{startY};
    // scene.sinComponents.addComponent(1, sinComp);

    std::cout << "Loaded " << count << " objects into the scene.\n";

    // std::cout << "Loading model..." << std::endl; // flush immediately

    float angle = 0.0f;
    float time = 0.0f;

    Camera cam({0, 0, -1.0f});

    Renderer renderer(WIDTH, HEIGHT, fov);

    auto test = registry.view<Transform, Model>();

    while (true) {
        renderer.clearBuffer();
        updateCamera(cam);
        auto t0 = std::chrono::high_resolution_clock::now();
        renderer.render(cam, test);
        spinObjects(registry.get<Transform>(), angle);
        // SinAnimSystem(registry.getComponentArray<SinComponent>(), registry.getComponentArray<Transform>(), time);

        auto t1 = std::chrono::high_resolution_clock::now();
        float ms = std::chrono::duration<float, std::milli>(t1 - t0).count();
        std::cout << "Frame: " << ms << " ms\n";
        angle += 0.05f;
        // std::cout << debugLog.size() << "\n";
        // for (size_t i = 0; i < debugLog.size(); ++i) {
        //     std::cout << debugLog[i] << "\n";
        // }
        // std::cout << "Revolutions: " << revolution << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        time += 0.016f;
    }
    std::cout << std::endl;
}
