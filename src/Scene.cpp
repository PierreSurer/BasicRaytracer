#include "Scene.hpp"

#include "Material.hpp"
#include "Image.hpp"

#include "objects/Object.hpp"
#include "objects/Sphere.hpp"
#include "objects/Light.hpp"

#include <memory>
#include <fstream>

// Functions to ease reading from YAML input
static void operator>> (const YAML::Node& node, glm::dvec3& v)
{
    assert(node.size() == 3);
    node[0] >> v.x;
    node[1] >> v.y;
    node[2] >> v.z;
}

std::unique_ptr<Material> Scene::parseMaterial(const YAML::Node& node) const
{
    std::unique_ptr<Material> m = std::make_unique<Material>();
    if (node.FindValue("color")) node["color"] >> m->color;
    if (node.FindValue("ka")) node["ka"] >> m->ka;
    if (node.FindValue("kd")) node["kd"] >> m->kd;
    if (node.FindValue("ks")) node["ks"] >> m->ks;
    if (node.FindValue("n")) node["n"] >> m->n;
    return m;
}

std::unique_ptr<Object> Scene::parseObject(const YAML::Node& node) const
{
    std::string objectType;
    node["type"] >> objectType;

    auto material = parseMaterial(node["material"]);

    if (objectType == "sphere") {
        double radius;
        glm::dvec3 position;
        node["radius"] >> radius;
        node["position"] >> position;
        return std::make_unique<Sphere>(position, radius);
    }
    else {
        std::cerr << "Error: unknown object type: " << objectType << std::endl;;
        throw std::runtime_error("unknown object type");
    }
}

std::unique_ptr<Light> Scene::parseLight(const YAML::Node& node) const
{
    glm::dvec3 position;
    Color color;
    node["position"] >> position;
    node["color"] >> color;
    return std::make_unique<Light>(position, color);
}

bool Scene::readScene(const std::string& inputFilename)
{
    // Open file stream for reading and have the YAML module parse it
    std::ifstream fin(inputFilename.c_str());
    if (!fin) {
        std::cerr << "Error: unable to open " << inputFilename << " for reading scene." << std::endl;;
        return false;
    }
    try {
        YAML::Parser parser(fin);
        if (parser) {
            YAML::Node doc;
            parser.GetNextDocument(doc);

            // Read scene configuration options
            doc["Eye"] >> parameters.eye;
            

            // Read and parse the scene objects
            const YAML::Node& sceneObjects = doc["Objects"];
            if (sceneObjects.GetType() != YAML::CT_SEQUENCE) {
                std::cerr << "Error: expected a sequence of objects." << std::endl;
                return false;
            }
            for(auto const& o : sceneObjects) {
                objects.push_back(parseObject(o));
            }

            // Read and parse light definitions
            const YAML::Node& sceneLights = doc["Lights"];
            if (sceneLights.GetType() != YAML::CT_SEQUENCE) {
                std::cerr << "Error: expected a sequence of lights." << std::endl;
                return false;
            }
            for(auto const& l : sceneLights) {
                lights.push_back(parseLight(l));
            }
        }
        if (parser) {
            std::cerr << "Warning: unexpected YAML document, ignored." << std::endl;
        }
    } catch(YAML::ParserException& e) {
        std::cerr << "Error at line " << e.mark.line + 1 << ", col " << e.mark.column + 1 << ": " << e.msg << std::endl;
        return false;
    }

    std::cout << "YAML parsing results: " << objects.size() << " objects read." << std::endl;
    return true;
}