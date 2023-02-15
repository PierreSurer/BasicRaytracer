#pragma once

#include "objects/Light.hpp"
#include "objects/Object.hpp"
#include "Material.hpp"

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <yaml/yaml.h>

struct SceneParameters {
    glm::dvec3 eye;  // the camera's eye position (looking at the origin)
                     // feel free to add other parameters as you go.
};

class Scene
{
public:
    bool readScene(const std::string& inputFilename);

    SceneParameters parameters;
    std::vector<std::unique_ptr<Object>> objects;
    std::vector<std::unique_ptr<Light>> lights;

private:
    // Couple of private functions for parsing YAML nodes
    std::unique_ptr<Material> parseMaterial(const YAML::Node& node) const;
    std::unique_ptr<Object> parseObject(const YAML::Node& node) const;
    std::unique_ptr<Light> parseLight(const YAML::Node& node) const;
};