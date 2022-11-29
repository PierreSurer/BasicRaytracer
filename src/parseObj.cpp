//
//  Framework for a raytracer
//  File: parseObj.cpp
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

#include "parseObj.h"
#include <fstream>
#include <sstream>
#include <regex>

Mesh parseObj(const std::string &filename)
{
  std::vector<Point> verts;
  std::vector<Triangle> faces;

  const std::regex face_desc("^f (\\d+).* (\\d+).* (\\d+)");

  std::ifstream file;
  file.open(filename);

  std::string line;

  while (std::getline(file, line)) {
    std::istringstream iss(line);

    if (line.size() > 0 && line[0] == '#')
      continue;

    char c;
    iss >> c;

    if (c == 'v') {
      double x, y, z;
      iss >> x >> y >> z;
      x = x * 100 + 200;
      y = y * 100 + 200;
      z = z * 100 + 200;
      verts.emplace_back(x, y, z);
    }
    else if (c == 'f') {
      smatch match;
      regex_search(line, match, face_desc);
      int i, j, k;
      i = std::stoi(match.str(1));
      j = std::stoi(match.str(2));
      k = std::stoi(match.str(3));
      faces.emplace_back(verts[i-1], verts[j-1], verts[k-1]);
    }
  }

  return Mesh(faces);
}