#pragma once

#include <glm/glm.hpp>
#include <iomanip>
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
  template<typename T, length_t C, length_t R>
  std::ostream& operator<<(std::ostream& os, mat<C, R, T> const& m) {
    // os << std::setprecision(5);
    os << "mat" << C << "x" << R << "(" << std::endl;
    for(length_t r = 0; r < R; r++) {
      for(length_t c = 0; c < C; c++) {
        os << "  " << std::setw(4) << m[c][r] << ", ";
      }
      os << std::endl;
     }
    os << ")";
    return os;
  }
}


