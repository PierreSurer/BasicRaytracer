#pragma once

#include "objects/Light.hpp"
#include "objects/Object.hpp"
#include "Image.hpp"
#include "Camera.hpp"
#include "yaml/yaml.h"

#include <vector>
#include <filesystem>
#include <glm.hpp>

enum class RenderMode {PHONG, NORMAL, DEPTH};

struct TraceParameters {
    bool shadows = true;
    double reflectionTheshold = 0.01;
    int maxBounces = 10;
    int superSampling = 1;
    RenderMode mode = RenderMode::PHONG;
};

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
    TraceParameters params;

private:
    std::filesystem::path assetsDir;

    // Couple of private functions for parsing YAML nodes
    std::unique_ptr<Material> parseMaterial(const YAML::Node& node) const;
    std::unique_ptr<Object> parseObject(const YAML::Node& node) const;
    std::unique_ptr<Light> parseLight(const YAML::Node& node) const;
    Camera parseCamera(const YAML::Node& node) const;
    TraceParameters parseParameters(const YAML::Node& node) const;
};