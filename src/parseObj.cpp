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
  std::vector<Vector> verts, texCoords, normals;
  std::vector<Mesh::Triangle> faces;

  const std::regex face_desc(
    "^f (\\d+)/(\\d+)/(\\d+) (\\d+)/(\\d+)/(\\d+) (\\d+)/(\\d+)/(\\d+)"
  );

  std::ifstream file;
  file.open(filename);

  std::string line;

  while (std::getline(file, line)) {
    std::istringstream iss(line);

    if (line.size() > 0 && line[0] == '#')
      continue;

    std::string c;
    iss >> c;

    if (c == "v") {
      double x, y, z;
      iss >> x >> y >> z;
      x = x * 100 + 200;
      y = y * 100 + 200;
      z = z * 100 + 200;
      verts.emplace_back(x, y, z);
    }
    if (c == "vn") {
      double x, y, z;
      iss >> x >> y >> z;
      normals.emplace_back(x, y, z);
    }
    if (c == "vt") {
      double u, v;
      iss >> u >> v;
      texCoords.emplace_back(u, v, 0.0);
    }
    else if (c == "f") {
      smatch match;
      regex_search(line, match, face_desc);
      int p1, p2, p3, t1, t2, t3, n1, n2, n3;
      p1 = std::stoi(match.str(1));
      p2 = std::stoi(match.str(4));
      p3 = std::stoi(match.str(7));
      t1 = std::stoi(match.str(2));
      t2 = std::stoi(match.str(5));
      t3 = std::stoi(match.str(8));
      n1 = std::stoi(match.str(3));
      n2 = std::stoi(match.str(6));
      n3 = std::stoi(match.str(9));
      faces.emplace_back(
        verts[p1-1], verts[p2-1], verts[p3-1],
        normals[n1-1], normals[n2-1], normals[n3-1]
      );
    }
  }

  return Mesh(faces);
}