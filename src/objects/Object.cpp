#include "Object.hpp"

using namespace glm;

Ray Object::computeLocalRay(const Ray& globalRay) const
{
  dvec3 O = invModel * dvec4(globalRay.O, 1.0);
  dvec3 target = invModel * dvec4(globalRay.O + globalRay.D, 1.0);
  return Ray(O, target - O, globalRay.delay);
}


void Object::setModel(mat4 M)
{
  model = M;
  invModel = inverse(model);
  normModel = transpose(invModel);
}
