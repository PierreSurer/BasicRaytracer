#include "ParseObj.hpp"
#include <fstream>
#include <sstream>
#include <regex>

Mesh parseObj(const std::string &filename)
{
  std::vector<glm::dvec3> verts;
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
      std::smatch match;
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