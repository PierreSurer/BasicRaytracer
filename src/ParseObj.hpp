#pragma once

#include "objects/Mesh.hpp"

#include <filesystem>
#include <unordered_map>
#include <memory>

Mesh parseObj(const std::filesystem::path &path);

using MaterialBank = std::unordered_map<std::string, std::shared_ptr<Material>>;

MaterialBank parseMtl(const std::filesystem::path &path);