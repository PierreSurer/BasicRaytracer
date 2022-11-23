cmake_minimum_required (VERSION 3.24)
project(Raytracer)

file(GLOB SOURCE
  ${CMAKE_CURRENT_SOURCE_DIR}/deps/yaml/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)


set(INCLUDES
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/deps
)

add_executable(Raytracer ${SOURCE})


target_include_directories(Raytracer PUBLIC ${INCLUDES})