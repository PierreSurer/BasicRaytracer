#pragma once

#include <glm/glm.hpp>
#include <iostream>

namespace glm {
  template<typename T, length_t N>
  std::ostream& operator<<(std::ostream& os, vec<N, T> const& v) {
    // os << std::setprecision(5);
    os << "vec" << N << "(";
    for(length_t i = 0; i < N-1; i++)
      os << v[i] << ", ";
    os << v[N-1] << ")";
    return os;
  }
}


