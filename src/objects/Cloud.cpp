#include "Cloud.hpp"

#include <random>
#include <glm/gtx/norm.hpp>
#include <omp.h>

const int STEP_SIZE = 1;

Cloud::Cloud(glm::dvec3 position, glm::dvec3 scale)
 : position(position), scale(scale) 
{
    // dim 0
    std::default_random_engine generator;
    std::normal_distribution<double> posDistribution(0.0, 0.05);
    //std:: uniform_real_distribution<double> posDistribution(-1.0, 1.0);


    std::vector<glm::dvec3> particles;
    for (int i = 0; i < 200; i++) {
        particles.push_back(glm::dvec3(posDistribution(generator) * scale.x * 0.5, posDistribution(generator) * scale.y * 0.5, posDistribution(generator) * scale.z * 0.5));
    }

    #pragma omp parallel for
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
                minDst = 1.0 - std::clamp(minDst, 0.0, 1.0);
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
    float t = std::min(0.0, tNear);
    double density = 1.0;
    double color = 1.0;
    while(t < tFar)
    {

        // Current ray position
        glm::dvec3 rayPos = ray.at(t) - position;
        std::cout<<rayPos<<std::endl;

        // Evaluate our signed distance field at the current ray position
        double sdf = -sphere_size *  signedDistance(rayPos) / 255.0;

        //std::cout<<sdf<<std::endl;
        double transmittance = exp(sdf * STEP_SIZE * 0.01);   

        // Integrate scattering
        density *= transmittance;

        Ray lightRay(ray.at(t), normalize(glm::dvec3(1.0, 1.0, 1.0)));

        glm::dvec3 tMin2 = (first - lightRay.O) / lightRay.D;
        glm::dvec3 tMax2 = (second - lightRay.O) / lightRay.D;

        double tFar2 = compMin(max(tMin2, tMax2));
        double t2 = 0.0;
        while(t2 < tFar2) {
            glm::dvec3 rayPos2 = lightRay.at(t2) - position;

            double sdf2 = -sphere_size * signedDistance(rayPos2) / 255.0;

            double transmittance2 = exp(sdf2 * STEP_SIZE * 0.001); 

            color *= transmittance2;  
            if (color < 0.003)
            {
                color = 0.0;
                break;
            }
            
            t2 += STEP_SIZE;
        }
        // Opaque check
        if (density < 0.003)
        {
            density = 0.0;
            break;
        }
        

        // March forward; step size depends on if we're inside the volume or not
        t += STEP_SIZE;

    }

    return glm::dvec4(color, color, color, 1.0 - density);

}

//https://iquilezles.org/articles/distfunctions/
double Cloud::signedDistance(glm::dvec3 pos) {
    glm::ivec3 position = ((pos + scale * 0.5) * (TEXTURE_SIZE + 1.0)) / scale;
    return textures[0][position.x][position.y][position.z];
    
}