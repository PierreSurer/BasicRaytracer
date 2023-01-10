#include "Scene.hpp"
#include "Material.hpp"
#include "Image.hpp"
#include "ParseObj.hpp"

#include "objects/Box.hpp"
#include "objects/Cylinder.hpp"
#include "objects/Mesh.hpp"
#include "objects/Object.hpp"
#include "objects/Sphere.hpp"
#include "objects/Light.hpp"
#include "objects/Triangle.hpp"

#include <ctype.h>
#include <fstream>
#include <chrono>
#include <assert.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

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

Scene::Scene(){
}

Scene::~Scene() {
}

std::unique_ptr<Material> Scene::parseMaterial(const YAML::Node& node)
{
    std::unique_ptr<Material> m = std::make_unique<Material>();
    node["color"] >> m->color;
    node["ka"] >> m->ka;
    node["kd"] >> m->kd;
    node["ks"] >> m->ks;
    node["n"] >> m->n;
    if (node.FindValue("ior")) {
        node["ior"] >> m->ior;
        m->ior = std::max(m->ior, 1.0);
    }
    return m;
}

std::unique_ptr<Object> Scene::parseObject(const YAML::Node& node)
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
        glm::dvec3 pos, rot, size;
        node["position"] >> pos;
        node["rotation"] >> rot;
        node["size"] >> size;
        returnObject = std::make_unique<Box>(pos, radians(rot), size);
    }
    else if (objectType == "cylinder") {
        glm::dvec3 pos, rot;
        double height, radius;
        node["position"] >> pos;
        node["rotation"] >> rot;
        node["height"] >> height;
        node["radius"] >> radius;
        returnObject = std::make_unique<Cylinder>(pos, radians(rot), height, radius);
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
        glm::dvec3 loc(0.0), rot(0.0), sca(1.0);
        node["file"] >> fname;
        if (node.FindValue("position")) node["position"] >> loc;
        if (node.FindValue("rotation")) node["rotation"] >> rot;
        if (node.FindValue("scale")) node["scale"] >> sca;
        rot = radians(rot);
        glm::dmat4 mat = glm::translate(glm::dmat4(1.0), loc)
                       * glm::eulerAngleYXZ(rot.y, rot.x, rot.z)
                       * glm::scale(glm::dmat4(1.0), sca);
        fname = assetsDir + fname;
        Mesh mesh = parseObj(fname);
        mesh.transform(mat);
        returnObject = std::make_unique<Mesh>(std::move(mesh));
    }

    if (returnObject) {
        // read the material and attach to object
        returnObject->material = parseMaterial(node["material"]);
    }

    return returnObject;
}

std::unique_ptr<Light> Scene::parseLight(const YAML::Node& node)
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

bool Scene::readScene(const std::string& inputFilename)
{
    // extract assets directory name from inputFilename
    // TODO: this is quick and unsafe
    // assetsDir = inputFilename.substr(inputFilename.find_last_of("/\\") + 1); //TODO Mathis will fix this :)))))))

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
            const YAML::Node& cam = doc["Camera"];
            glm::dvec3 eye, up, target;

            eye = parseVector(cam["eye"]);
            up = parseVector(cam["up"]);
            target = parseVector(cam["center"]);

            camera.setPosition(eye);
            camera.lookAt(eye, target, up);
            
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
                    objects.push_back(std::move(obj));
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
                lights.push_back(parseLight(*it));
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