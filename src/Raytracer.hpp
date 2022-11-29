//
//  Framework for a raytracer
//  File: raytracer.h
//
//  Created for the Computer Science course "Introduction Computer Graphics"
//  taught at the University of Groningen by Tobias Isenberg.
//
//  Author: Maarten Everts
//
//  This framework is inspired by and uses code of the raytracer framework of 
//  Bert Freudenberg that can be found at
//  http://isgwww.cs.uni-magdeburg.de/graphik/lehre/cg2/projekt/rtprojekt.html 
//

#pragma once

#include <iostream>
#include <string>
#include "objects/Light.hpp"
#include "Scene.hpp"
#include "yaml/yaml.h"

class Raytracer {
private:
    std::string assetsDir;
    
    // Couple of private functions for parsing YAML nodes
    std::unique_ptr<Material> parseMaterial(const YAML::Node& node);
    std::unique_ptr<Object> parseObject(const YAML::Node& node);
    std::unique_ptr<Light> parseLight(const YAML::Node& node);

public:
    Raytracer()
    { }

    std::unique_ptr<Scene> scene;

    bool readScene(const std::string& inputFilename);
    void renderToFile(const std::string& outputFilename);
};
