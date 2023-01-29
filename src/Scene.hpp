#pragma once

#include "objects/Light.hpp"
#include "objects/Object.hpp"
#include "objects/Cloud.hpp"
#include "Image.hpp"
#include "Camera.hpp"
#include "yaml/yaml.h"

#include <vector>
#include <filesystem>
#include <glm.hpp>

enum class RenderMode {PHONG, NORMAL, DEPTH, UV, GOOCH};

struct TraceParameters {
    bool shadows = true;
    double reflectionTheshold = 0.01;
    int maxBounces = 10;
    int superSampling = 1;
    RenderMode mode = RenderMode::PHONG;
};

struct GoochParameters {
    double b = 0.4;
    double y = 0.4;
    double alpha = 0.2;
    double beta = 0.6;
};

enum class Sky {NONE, DAY, NIGHT, DIRECTION};

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
    std::vector<std::unique_ptr<Cloud>> clouds;
    Sky sky;

    Camera camera;
    TraceParameters params;
    GoochParameters gooch;

private:
    std::filesystem::path assetsDir;

    // Couple of private functions for parsing YAML nodes
    std::unique_ptr<Material> parseMaterial(const YAML::Node& node) const;
    std::unique_ptr<Object> parseObject(const YAML::Node& node) const;
    std::unique_ptr<Cloud> parseCloud(const YAML::Node& node) const;
    std::unique_ptr<Light> parseLight(const YAML::Node& node) const;
    Camera parseCamera(const YAML::Node& node) const;
    TraceParameters parseParameters(const YAML::Node& node) const;
    GoochParameters parseGooch(const YAML::Node& node) const;
};