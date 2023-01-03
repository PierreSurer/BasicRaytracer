#pragma once

#include "objects/Light.hpp"
#include "objects/Object.hpp"
#include "Image.hpp"
#include "Camera.hpp"
#include "vulkan/WindowManager.hpp"
#include "vulkan/Device.hpp"
#include "vulkan/Renderer.hpp"
#include "yaml/yaml.h"

#include <vector>
#include <glm.hpp>

class Scene
{
public:

    Scene();
    ~Scene();

    Scene(const Scene&) = delete;
    Scene operator=(const Scene&) = delete;
    
    bool readScene(const std::string& inputFilename);

    std::vector<std::unique_ptr<Object>> objects; //TODO private?
    std::vector<std::unique_ptr<Light>> lights;

    Camera camera;
    void run();

private:
    // Couple of private functions for parsing YAML nodes
    std::unique_ptr<Material> parseMaterial(const YAML::Node& node);
    std::unique_ptr<Object> parseObject(const YAML::Node& node);
    std::unique_ptr<Light> parseLight(const YAML::Node& node);

    void updateObjects(const double& dt);

    std::string assetsDir;

    WindowManager window;
    Device device;
    Renderer renderer;
    Camera mainCamera;
    
};
