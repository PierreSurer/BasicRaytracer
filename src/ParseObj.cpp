#include "ParseObj.hpp"
#include "Material.hpp"
#include <fstream>
#include <sstream>
#include <regex>
#include <glm.hpp>

using namespace glm;

Mesh parseObj(const std::filesystem::path &path)
{
  std::vector<dvec3> verts, normals;
  std::vector<dvec2> texCoords;
  std::vector<Triangle> faces;
  MaterialBank mats;
  std::shared_ptr<Material> active_mat = DEFAULT_MATERIAL;

  const std::regex face_desc(
    "^f (\\d+)/(\\d+)/(\\d+) (\\d+)/(\\d+)/(\\d+) (\\d+)/(\\d+)/(\\d+)"
  );

  std::ifstream file;
  file.open(path);

  std::string line;

  while (std::getline(file, line)) {
    std::istringstream iss(line);

    if (line.size() > 0 && line[0] == '#')
      continue;

    std::string c;
    iss >> c;

    if (c == "mtllib") {
      std::filesystem::path mtl_path;
      iss >> mtl_path;
      mtl_path = path.parent_path() / mtl_path;
      MaterialBank mtl = parseMtl(mtl_path);
      mats.insert(mtl.begin(), mtl.end());
    }
    if (c == "usemtl") {
      std::string name;
      iss >> name;
      auto it = mats.find(name);
      if (it != mats.end()) {
        active_mat = it->second;
      }
    }
    else if (c == "v") {
      double x, y, z;
      iss >> x >> y >> z;
      verts.emplace_back(x, y, z);
    }
    else if (c == "vn") {
      double x, y, z;
      iss >> x >> y >> z;
      normals.emplace_back(x, y, z);
    }
    else if (c == "vt") {
      double u, v;
      iss >> u >> v;
      texCoords.emplace_back(u, v);
    }
    else if (c == "f") {
      std::smatch match;
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
      Triangle tri(
        verts[p1-1], verts[p2-1], verts[p3-1],
        normals[n1-1], normals[n2-1], normals[n3-1],
        texCoords[t1-1], texCoords[t2-1], texCoords[t3-1]
      );
      tri.material = active_mat;
      faces.push_back(std::move(tri));
    }
  }

  return Mesh(std::move(faces));
}


MaterialBank parseMtl(const std::filesystem::path &path) {
  MaterialBank res;
  Material* mat = nullptr;

  std::ifstream file;
  file.open(path);

  std::string line;

  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string c;
    iss >> c;

    if (c == "newmtl") {
      std::string name;
      iss >> name;
      auto insert = res.emplace(name, std::make_shared<Material>());
      mat = insert.first->second.get();
      mat->color = dvec3(1.0);
    }
    else if (c == "Ka") { // ambient color
      // our color ambient color is implicitly white, so we get the magnitude
      // of the mtl's Ka (which represents ambientColor * ka)
      Color col;
      iss >> col.r >> col.g >> col.b;
      mat->ka = length(col / 255.0) / sqrt(3.0);
    }
    else if (c == "Kd") { // diffuse color
      // our color diffuse color is implicitly white, so we get the magnitude
      // of the mtl's Kd (which represents diffuseColor * kd)
      Color col;
      iss >> col.r >> col.g >> col.b;
      mat->kd = length(col / 255.0) / sqrt(3.0);
    }
    else if (c == "Ks") { // specular color
      // our color specular color is implicitly white, so we get the magnitude
      // of the mtl's Ks (which represents specularColor * ks)
      Color col;
      iss >> col.r >> col.g >> col.b;
      mat->ks = length(col / 255.0) / sqrt(3.0);
    }
    else if (c == "Ns") { // specular exponent
      iss >> mat->n;
    }
    else if (c == "Ni") { // index of refraction
      iss >> mat->ior;
    }
    else if (c == "map_Kd") { // diffuse texture
      std::filesystem::path tex_path;
      iss >> tex_path;
      tex_path = path.parent_path() / tex_path;
      mat->texture = std::make_shared<Image>(tex_path.string());
      mat->kd = 1.0;
    }
  }

  return res;
}
