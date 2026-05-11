#define _USE_MATH_DEFINES

#include <iostream>
#include <memory>
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
#include "system.h"
#include "sinAnimSystem.h"
#include "spinSystem.h"
#include "primitive.h"
#include "components_type.h"
#include "schemaRegistry.h"

#ifdef _WIN32
#include "window_polyfill.h"
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

void setInputMode() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
}

void restoreInputMode() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= ICANON | ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

int key_w = 0, key_a = 0, key_s = 0, key_d = 0, key_q = 0, key_e = 0;
int key_up = 0, key_down = 0, key_left = 0, key_right = 0;

void pollInput() {
    char buf[256];
    int bytesRead = read(STDIN_FILENO, buf, sizeof(buf));
    
    // Decrement timers
    if (key_w > 0) key_w--;
    if (key_a > 0) key_a--;
    if (key_s > 0) key_s--;
    if (key_d > 0) key_d--;
    if (key_q > 0) key_q--;
    if (key_e > 0) key_e--;
    if (key_up > 0) key_up--;
    if (key_down > 0) key_down--;
    if (key_left > 0) key_left--;
    if (key_right > 0) key_right--;

    for (int i = 0; i < bytesRead; i++) {
        char c = buf[i];
        if (c == 'w' || c == 'W') key_w = 3;
        if (c == 's' || c == 'S') key_s = 3;
        if (c == 'a' || c == 'A') key_a = 3;
        if (c == 'd' || c == 'D') key_d = 3;
        if (c == 'q' || c == 'Q') key_q = 3;
        if (c == 'e' || c == 'E') key_e = 3;
        if (c == '\033') { // Escape sequence
            if (i + 2 < bytesRead && buf[i+1] == '[') {
                char seq = buf[i+2];
                if (seq == 'A') key_up = 3;
                if (seq == 'B') key_down = 3;
                if (seq == 'C') key_right = 3;
                if (seq == 'D') key_left = 3;
                i += 2;
            }
        }
    }
}
#endif

std::vector<std::string> sceneLog;

using loadFn = std::function<void(Registry&, Entity, std::istringstream*)>;
std::unordered_map<std::string, loadFn> componentLoaders;

template<typename T>
void readComponent(T& component, std::istringstream& ss) {
    auto& schema = SchemaRegistry::instance().get<T>();

    for (auto& field : schema) {
        sceneLog.push_back("Reading field for component: " + std::string(typeid(T).name()));
        field->read(&component, ss);
    }
}

template<typename T>
void registerComponentType(const std::string& name) {
    componentLoaders[name] = [](Registry& registry, Entity entity, std::istringstream* ss) {
        T component;
        if (ss != nullptr) {
            readComponent(component, *ss);
        }
        registry.get<T>().addComponent(entity, std::move(component));
    };
}


Registry registry;
std::vector<std::unique_ptr<BaseSystem>> systems;


template<typename T>
void BindSystem() {
    systems.push_back(std::make_unique<T>());
}

const int WIDTH = 120;
const int HEIGHT = 60;
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
        Entity entity = registry.create();

        Transform transform;
        Material material;
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

        if (!parseVec3(iss, material.color)) {
            std::cerr << "Invalid color format in line: " << line << std::endl;
            continue;
        }

        std::string componentList;
        if (iss >> componentList) {
            std::string_view comSv(componentList);
            size_t pos = 0;

            while((pos = comSv.find(';')) != std::string_view::npos) {
                std::string_view token = comSv.substr(0, pos);
                sceneLog.push_back("Was?");
                size_t colonPos = token.find(':');
                if (colonPos != std::string_view::npos) {
                    std::string compName(token.substr(0, colonPos));
                    sceneLog.push_back("Loading component: " + compName + " for entity: " + std::to_string(entity));
                    if (componentLoaders.find(compName) != componentLoaders.end()) {
                        std::istringstream compData(std::string(token.substr(colonPos + 1)));
                        componentLoaders[compName](registry, entity, &compData);
                    } else {
                        std::cerr << "Unknown component: " << compName << " in line: " << line << std::endl;
                    }

                } else {
                    std::string compName(token);
                    if (componentLoaders.find(compName) != componentLoaders.end()) {
                        componentLoaders[compName](registry, entity, nullptr);
                    } else {
                        std::cerr << "Unknown component: " << compName << " in line: " << line << std::endl;
                    }
                }
                comSv.remove_prefix(pos + 1);
            }
        }
        
        try {
            registry.get<Transform>().addComponent(entity, std::move(transform));
            registry.get<Model>().emplaceComponent(entity, modelFilename);
            registry.get<Material>().addComponent(entity, material);

            // Model model(modelFilename);
            // std::cout << "Loaded model: " << modelFilename << " with position " << transform.position << "\n";
            // sceneObjects.emplace_back(std::move(model), std::move(transform));
        } catch (const std::exception& e) {
            std::cerr << "Error loading model from file: " << modelFilename << ". Error: " << e.what() << std::endl;
        }

    }
    return registry.getEntitiyCount();
}

void updateCamera(Camera& cam) {
#ifdef _WIN32
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
#else
    pollInput();
    if (key_up) cam.rotation.x -= 0.05f;
    if (key_down) cam.rotation.x += 0.05f;
    if (key_left) cam.rotation.y -= 0.05f;
    if (key_right) cam.rotation.y += 0.05f;

    if (key_w) cam.position += gmath::normalize(cam.forward()) * 0.1f;
    if (key_s) cam.position += gmath::normalize(cam.forward()) * -0.1f;
    if (key_a) cam.position += gmath::normalize(cam.right()) * -0.1f;
    if (key_d) cam.position += gmath::normalize(cam.right()) * 0.1f;
    if (key_e) cam.position.y += 0.01f;
    if (key_q) cam.position.y -= 0.01f;
#endif
}

void startSystems() {
    for (auto& system : systems) {
        system->doStart(registry);
    }
}

void updateSystems(float deltaTime) {
    for (auto& system : systems) {
        system->doUpdate(registry, deltaTime);
    }
}

int main() {
#ifdef _WIN32
    setupWindowCursor();
#else
    setInputMode();
#endif
    // Hide cursor (don't touch terminal wrap mode; some terminals handle DECAWM differently)
    std::cout << "\033[?25l" << std::flush;
    std::cout << "\033[H\033[2J";

    registerComponentType<SinComponent>("SinAnim");

    Entity count = loadSceneFile("scene.txt");
    // float startY = scene.transforms.getComponent(1)->position.y;
    // SinComponent sinComp{startY};
    // scene.sinComponents.addComponent(1, sinComp);

    // std::cout << "Loaded " << count << " objects into the scene.\n";

    // std::cout << "Loading model..." << std::endl; // flush immediately

    float angle = 0.0f;
    float time = 0.0f;

    Camera cam({0, 0, -1.0f});

    Renderer renderer(WIDTH, HEIGHT, fov);

    // registry.get<SinComponent>().addComponent(1, SinComponent());
    auto test = registry.view<Transform, Model, Material>();

    Entity planeEntity = registry.create();
    registry.get<Transform>().addComponent(planeEntity, Transform({0, -0.5, 0}));
    registry.get<Model>().addComponent(planeEntity, Primitive::createPlane());
    registry.get<Material>().addComponent(planeEntity, Material{{0.8f, 0.8f, 0.8f}});

    BindSystem<SinAnimSystem>();
    // BindSystem<SpinSystem>();
    startSystems();

    std::cout << "\033[H\033[2J";

    std::ofstream logFile("scene_log.txt");
    for (auto& log : sceneLog) {
        logFile << log << "\n";
    }
    logFile << "Finished loading scene. Starting main loop...\n";
    logFile.close();

    while (true) {
        renderer.clearBuffer();
        updateCamera(cam);
        auto t0 = std::chrono::high_resolution_clock::now();
        updateSystems(0.016f);
        renderer.render(cam, test);
        // std::cout << "hi" << time << std::endl;
        // SinAnimSystem(registry.getComponentArray<SinComponent>(), registry.getComponentArray<Transform>(), time);

        auto t1 = std::chrono::high_resolution_clock::now();
        float ms = std::chrono::duration<float, std::milli>(t1 - t0).count();
        // Print below the render area (+3 to clear renderer's own timing lines)
        std::cout << "\033[" << (HEIGHT + 3) << ";1HFrame: " << ms << " ms\033[K\n";
        for (size_t i = 0; i < renderer.renderLog.size(); ++i) {
            std::cout << renderer.renderLog[i] << "\033[K\n";
        }
        angle += 0.05f;

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        time += 0.016f;
    }
    std::cout << "\033[?25h"; // Show cursor again before exiting
    std::cout << std::endl;
}