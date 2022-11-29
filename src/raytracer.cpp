#include "Raytracer.hpp"
#include "objects/Box.hpp"
#include "objects/Mesh.hpp"
#include "objects/Object.hpp"
#include "objects/Sphere.hpp"
#include "objects/Light.hpp"
#include "objects/Triangle.hpp"
#include "Material.hpp"
#include "Image.hpp"
#include "ParseObj.hpp"
#include "yaml/yaml.h"
#include <ctype.h>
#include <fstream>
#include <assert.h>

// Functions to ease reading from YAML input
void operator >> (const YAML::Node& node, glm::dvec3& v);
glm::dvec3 parseVector(const YAML::Node& node);

void operator >> (const YAML::Node& node, glm::dvec3& v)
{
    assert(node.size() == 3);
    node[0] >> v.x;
    node[1] >> v.y;
    node[2] >> v.z;
}

glm::dvec3 parseVector(const YAML::Node& node)
{
    glm::dvec3 v;
    node[0] >> v.x;
    node[1] >> v.y;
    node[2] >> v.z;
    return v;
}

std::unique_ptr<Material> Raytracer::parseMaterial(const YAML::Node& node)
{
    std::unique_ptr<Material> m = std::make_unique<Material>();
    node["color"] >> m->color;	
    node["ka"] >> m->ka;
    node["kd"] >> m->kd;
    node["ks"] >> m->ks;
    node["n"] >> m->n;
    return std::move(m);
}

std::unique_ptr<Object> Raytracer::parseObject(const YAML::Node& node)
{
    std::unique_ptr<Object> returnObject;
    std::string objectType;
    node["type"] >> objectType;

    if (objectType == "sphere") {
        glm::dvec3 pos;
        double r;
        node["position"] >> pos;
        node["radius"] >> r;
        returnObject = std::make_unique<Sphere>(pos, r);
    }
    else if (objectType == "box") {
        glm::dvec3 pos;
        double size;
        node["position"] >> pos;
        node["size"] >> size;
        returnObject = std::make_unique<Box>(pos, size);
    }
    else if (objectType == "triangle") {
        glm::dvec3 p1;
        glm::dvec3 p2;
        glm::dvec3 p3;
        node["p1"] >> p1;
        node["p2"] >> p2;
        node["p3"] >> p3;
        returnObject = std::make_unique<Triangle>(p1, p2, p3);
    }
    else if (objectType == "mesh") {
        std::string fname;
        node["file"] >> fname;
        fname = assetsDir + fname;
        returnObject = std::make_unique<Mesh>(parseObj(fname));
    }

    if (returnObject) {
        // read the material and attach to object
        returnObject->material = std::move(parseMaterial(node["material"]));
    }

    return std::move(returnObject);
}

std::unique_ptr<Light> Raytracer::parseLight(const YAML::Node& node)
{
    glm::dvec3 position;
    Color color;
    node["position"] >> position;
    node["color"] >> color;
    return std::make_unique<Light>(position, color);
}

/*
* Read a scene from file
*/

bool Raytracer::readScene(const std::string& inputFilename)
{
    // extract assets directory name from inputFilename
    // TODO: this is quick and unsafe
    assetsDir = inputFilename.substr(0, inputFilename.find_last_of("/\\") + 1);

    // Initialize a new scene
    scene = std::make_unique<Scene>();

    // Open file stream for reading and have the YAML module parse it
    std::ifstream fin(inputFilename.c_str());
    if (!fin) {
        std::cerr << "Error: unable to open " << inputFilename << " for reading." << std::endl;;
        return false;
    }
    try {
        YAML::Parser parser(fin);
        if (parser) {
            YAML::Node doc;
            parser.GetNextDocument(doc);

            // Read scene configuration options
            scene->eye = parseVector(doc["Eye"]);

            // Read and parse the scene objects
            const YAML::Node& sceneObjects = doc["Objects"];
            if (sceneObjects.GetType() != YAML::CT_SEQUENCE) {
                std::cerr << "Error: expected a sequence of objects." << std::endl;
                return false;
            }
            for(YAML::Iterator it=sceneObjects.begin(); it!=sceneObjects.end(); ++it) {
                std::unique_ptr<Object> obj = parseObject(*it);
                // Only add object if it is recognized
                if (obj) {
                    scene->objects.push_back(std::move(obj));
                } else {
                    std::cerr << "Warning: found object of unknown type, ignored." << std::endl;
                }
            }

            // Read and parse light definitions
            const YAML::Node& sceneLights = doc["Lights"];
            if (sceneObjects.GetType() != YAML::CT_SEQUENCE) {
                std::cerr << "Error: expected a sequence of lights." << std::endl;
                return false;
            }
            for(YAML::Iterator it=sceneLights.begin();it!=sceneLights.end();++it) {
                scene->lights.push_back(parseLight(*it));
            }
        }
        if (parser) {
            std::cerr << "Warning: unexpected YAML document, ignored." << std::endl;
        }
    } catch(YAML::ParserException& e) {
        std::cerr << "Error at line " << e.mark.line + 1 << ", col " << e.mark.column + 1 << ": " << e.msg << std::endl;
        return false;
    }

    std::cout << "YAML parsing results: " << scene->objects.size() << " objects read." << std::endl;
    return true;
}

void Raytracer::renderToFile(const std::string& outputFilename)
{
    Image img(400,400);
    std::cout << "Tracing..." << std::endl;
    scene->render(img);
    std::cout << "Writing image to " << outputFilename << "..." << std::endl;
    img.writePng(outputFilename.c_str());
    std::cout << "Done." << std::endl;
}
