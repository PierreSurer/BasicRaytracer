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
static void operator >> (const YAML::Node& node, glm::dvec3& v)
{
    assert(node.size() == 3);
    node[0] >> v.x;
    node[1] >> v.y;
    node[2] >> v.z;
}

static glm::dvec3 parseVector(const YAML::Node& node)
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

std::unique_ptr<Material> Scene::parseMaterial(const YAML::Node& node) const
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

std::unique_ptr<Object> Scene::parseObject(const YAML::Node& node) const
{
    std::unique_ptr<Object> returnObject;
    std::string objectType;
    node["type"] >> objectType;

    auto material = parseMaterial(node["material"]);

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
        std::string path;
        glm::dvec3 loc(0.0), rot(0.0), sca(1.0);
        node["file"] >> path;
        if (node.FindValue("position")) node["position"] >> loc;
        if (node.FindValue("rotation")) node["rotation"] >> rot;
        if (node.FindValue("scale")) node["scale"] >> sca;
        rot = radians(rot);
        glm::dmat4 mat = glm::translate(glm::dmat4(1.0), loc)
                       * glm::eulerAngleYXZ(rot.y, rot.x, rot.z)
                       * glm::scale(glm::dmat4(1.0), sca);
        path = assetsDir / path;
        Mesh mesh = parseObj(path);
        mesh.transform(mat);
        mesh.optimize();
        returnObject = std::make_unique<Mesh>(std::move(mesh));
    }

    if (returnObject) {
        returnObject->material = std::move(material);
    }

    return returnObject;
}

std::unique_ptr<Light> Scene::parseLight(const YAML::Node& node) const
{
    glm::dvec3 position;
    Color color;
    node["position"] >> position;
    node["color"] >> color;
    return std::make_unique<Light>(position, color);
}


TraceParameters Scene::parseParameters(const YAML::Node& node) const
{
    TraceParameters res;

    if (node.FindValue("Shadows")) node["Shadows"] >> res.shadows;
    if (node.FindValue("MaxRecursionDepth")) node["MaxRecursionDepth"] >> res.maxBounces;
    if (node.FindValue("RecursionThreshold")) node["RecursionThreshold"] >> res.reflectionTheshold;

    if (node.FindValue("Shadows")) node["Shadows"] >> res.shadows;
    if (node.FindValue("MaxRecursionDepth")) node["MaxRecursionDepth"] >> res.maxBounces;
    if (node.FindValue("RecursionThreshold")) node["RecursionThreshold"] >> res.reflectionTheshold;
    if (node.FindValue("SuperSampling")) {
        const YAML::Node& msaa = node["SuperSampling"];
        msaa["factor"] >> res.superSampling;
    } 
    if (node.FindValue("RenderMode")) {
        if(node["RenderMode"] == "zbuffer") res.mode = RenderMode::DEPTH;
        if(node["RenderMode"] == "normal") res.mode = RenderMode::NORMAL;
        if(node["RenderMode"] == "phong") res.mode = RenderMode::PHONG;
    }

    return res;
}

Camera Scene::parseCamera(const YAML::Node& node) const
{
    Camera camera;
    glm::dvec3 eye, up, target;

    eye = parseVector(node["eye"]);
    up = parseVector(node["up"]);
    target = parseVector(node["center"]);

    camera.setPosition(eye);
    camera.lookAt(eye, target, normalize(up));
    if (node.FindValue("fov")) node["fov"] >> camera.fov;
    
    return camera;
}

/*
* Read a scene from file
*/

bool Scene::readScene(const std::string& inputFilename)
{
    // extract assets directory name from inputFilename
    // TODO: this is quick and unsafe
    assetsDir = inputFilename.substr(0, inputFilename.find_last_of("/\\") + 1); //TODO Mathis will fix this :)))))))

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

            // Rad scene parameters
            params = parseParameters(doc);

            // Read scene camera
            const YAML::Node& cam = doc["Camera"];
            camera = parseCamera(cam);
            
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

    std::cout << "YAML parsing results: " << objects.size() << " objects read - " ;
    return true;
}