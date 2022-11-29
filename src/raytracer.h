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
#include "triple.h"
#include "light.h"
#include "scene.h"
#include "yaml/yaml.h"

class Raytracer {
private:
    std::string assetsDir;
    
    // Couple of private functions for parsing YAML nodes
    Material* parseMaterial(const YAML::Node& node);
    Object* parseObject(const YAML::Node& node);
    Light* parseLight(const YAML::Node& node);

public:
    Raytracer()
    { }

    Scene *scene;

    bool readScene(const std::string& inputFilename);
    void renderToFile(const std::string& outputFilename);
};
