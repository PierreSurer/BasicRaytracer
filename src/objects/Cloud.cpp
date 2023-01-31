#include "Cloud.hpp"

#include <random>
#include <glm/gtx/norm.hpp>

const int RAY_MAX_STEPS = 1000;
const int STEP_SIZE = 1;
Cloud::Cloud(glm::dvec3 position, glm::dvec3 scale)
 : position(position), scale(scale) 
{

}

double Cloud::densityFunction(double sdf) {
    return glm::max(-sdf, 0.0);
}


glm::dvec4 Cloud::traverse(Ray const& ray) {
    glm::dvec4 intScattTrans = glm::dvec4(0.0, 0.0, 0.0, 1.0);

    auto first = position - scale * 0.5;
    auto second = position + scale * 0.5;

    // test regular intersection
    glm::dvec3 tMin = (first - ray.O) / ray.D;
    glm::dvec3 tMax = (second - ray.O) / ray.D;
    glm::dvec3 t1 = min(tMin, tMax);
    glm::dvec3 t2 = max(tMin, tMax);
    double tNear = compMax(t1);
    double tFar = compMin(t2);
    // No intersection
    if(tNear > tFar || tFar < 0.0) return intScattTrans;

    // Scattering in RGB, transmission in A


    // Current distance along ray
    float t = 0;

    for (int i = 0; i < RAY_MAX_STEPS && t < tFar; i++)

    {

        // Current ray position
        glm::dvec3 rayPos = ray.at(t) - position;

        // Evaluate our signed distance field at the current ray position
        double sdf = signedDistance(rayPos);

        // Only evaluate the cloud color if we're inside the volume
        if (sdf < 0)
        {
            double extinction = densityFunction(sdf);
            double transmittance = exp(extinction * STEP_SIZE);   

            // Get the luminance for the current ray position
            double luminance = 0.2126 * rayPos.r + 0.7152 * rayPos.g + 0.0722 * rayPos.b;

            // Integrate scattering
            double integScatt = luminance - luminance * transmittance;
            intScattTrans.x += integScatt * intScattTrans.a;
            intScattTrans.y += integScatt * intScattTrans.a;
            intScattTrans.z += integScatt * intScattTrans.a;
            intScattTrans.a *= transmittance;

            // Opaque check
            if (intScattTrans.a < 0.003)
            {
                intScattTrans.a = 0.0;
                break;
            }
        }

        // March forward; step size depends on if we're inside the volume or not
        t += sdf < 0 ? STEP_SIZE : glm::max(sdf, (double)STEP_SIZE);

    }

    return glm::dvec4(intScattTrans.x, intScattTrans.y, intScattTrans.z, 1 - intScattTrans.a);

}

//https://iquilezles.org/articles/distfunctions/
double Cloud::signedDistance(glm::dvec3 pos) {
    glm::dvec3 q = abs(pos) - scale * 0.5;
    return length(glm::max(q, 0.0)) + glm::min(glm::max(q.x, glm::max(q.y, q.z)), 0.0);
}