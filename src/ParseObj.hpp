#pragma once

#include <filesystem>
#include "objects/Mesh.hpp"

Mesh parseObj(const std::filesystem::path &path);