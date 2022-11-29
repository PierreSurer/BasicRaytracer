#pragma once

#include <vector>

#include "WindowManager.hpp"
#include "Device.hpp"
#include "Renderer.hpp"
#include "GameObject.hpp"

class Scene {
public:
    Scene();
    ~Scene();

    Scene(const Scene&) = delete;
    Scene operator=(const Scene&) = delete;

    void run();

private:
    void loadGameObjects();
    void updateGameObjects(const float& dt);

    Model::Builder createPlane(glm::vec2 size, int subDivisions, float concentration = 0.0f);
    
    WindowManager window;
    Device device;
    Renderer renderer;
    Camera mainCamera;

    std::vector<GameObject> gameObjects;
};