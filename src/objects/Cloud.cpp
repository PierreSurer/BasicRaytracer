#include "Cloud.hpp"

#include <random>
#include <glm/gtx/norm.hpp>

const int STEP_SIZE = 1;

Cloud::Cloud(glm::dvec3 position, glm::dvec3 scale)
 : position(position), scale(scale) 
{
    // dim 0
    const double sphere_size = 3.0;
    std::default_random_engine generator;
    std::normal_distribution<double> posDistribution(0.0, 0.2);

    std::vector<glm::dvec3> particles;
    for (int i = 0; i < 2; i++) {
        particles.push_back(glm::dvec3(posDistribution(generator) * scale.x * 0.5, posDistribution(generator) * scale.y * 0.5, posDistribution(generator) * scale.z * 0.5));
    }

    for(int i = 0; i < TEXTURE_SIZE; i++)
        for(int j = 0; j < TEXTURE_SIZE; j++)
            for(int k = 0; k < TEXTURE_SIZE; k++) {
                double minDst = std::numeric_limits<double>::infinity();
                glm::dvec3 it = scale / (TEXTURE_SIZE + 1.0);
                for(int a = 0; a < particles.size(); a++) {
                    glm::dvec3 pos = -scale * 0.5 + it / 2.0 + glm::dvec3(it.x * i, it.y * j, it.z * k);
                    double dst = glm::distance(pos, particles[a]) / sphere_size;
                    minDst = std::min(minDst, dst);
                }
                minDst = std::clamp(minDst, 0.0, 1.0);
                textures[0][i][j][k] = (uint8_t)(minDst * 255.0);
            }

}

glm::dvec4 Cloud::traverse(Ray const& ray, double tMaximum) {

    auto first = position - scale * 0.5;
    auto second = position + scale * 0.5;

    // test regular intersection
    glm::dvec3 tMin = (first - ray.O) / ray.D;
    glm::dvec3 tMax = (second - ray.O) / ray.D;
    glm::dvec3 t1 = min(tMin, tMax);
    glm::dvec3 t2 = max(tMin, tMax);
    double tNear = compMax(t1);
    double tFar = compMin(t2);
    tFar = std::min(tFar, tMaximum);
    // No intersection
    if(tNear > tFar || tFar < 0.0) return glm::dvec4(0.0);

    // Scattering in RGB, transmission in A


    // Current distance along ray
    float t = 0;
    double density = 1.0;

    while(t < tFar)
    {

        // Current ray position
        glm::dvec3 rayPos = ray.at(t) - position;

        // Evaluate our signed distance field at the current ray position
        double sdf = -signedDistance(rayPos) / 255.0;

        //std::cout<<sdf<<std::endl;

        // Only evaluate the cloud color if we're inside the volume
        if (sdf < 0)
        {
            double transmittance = exp(sdf * STEP_SIZE * 0.01);   

            // Integrate scattering
            glm::dvec4 integScatt = glm::dvec4(1.0) * transmittance;
            density *= transmittance;
            

            // Opaque check
            if (density < 0.003)
            {
                density = 0.0;
                break;
            }
        }

        // March forward; step size depends on if we're inside the volume or not
        t += sdf < 0 ? STEP_SIZE : glm::max(sdf, (double)STEP_SIZE);

    }

    return glm::dvec4(1.0, 1.0, 1.0, 1.0 - density);

}

//https://iquilezles.org/articles/distfunctions/
double Cloud::signedDistance(glm::dvec3 pos) {
    glm::ivec3 position = ((pos + scale * 0.5) * 33.0) / scale;
    return noise[position.x][position.y][position.z];
    
}