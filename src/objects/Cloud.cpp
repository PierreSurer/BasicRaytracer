#include "Cloud.hpp"

#include <random>
#include <glm/gtx/norm.hpp>
#include <omp.h>

static const glm::ivec3 offsets[] =
{
    // centre
    glm::ivec3(0,0,0),
    // front face
    glm::ivec3(0,0,1),
    glm::ivec3(-1,1,1),
    glm::ivec3(-1,0,1),
    glm::ivec3(-1,-1,1),
    glm::ivec3(0,1,1),
    glm::ivec3(0,-1,1),
    glm::ivec3(1,1,1),
    glm::ivec3(1,0,1),
    glm::ivec3(1,-1,1),
    // back face
    glm::ivec3(0,0,-1),
    glm::ivec3(-1,1,-1),
    glm::ivec3(-1,0,-1),
    glm::ivec3(-1,-1,-1),
    glm::ivec3(0,1,-1),
    glm::ivec3(0,-1,-1),
    glm::ivec3(1,1,-1),
    glm::ivec3(1,0,-1),
    glm::ivec3(1,-1,-1),
    // ring around centre
    glm::ivec3(-1,1,0),
    glm::ivec3(-1,0,0),
    glm::ivec3(-1,-1,0),
    glm::ivec3(0,1,0),
    glm::ivec3(0,-1,0),
    glm::ivec3(1,1,0),
    glm::ivec3(1,0,0),
    glm::ivec3(1,-1,0)
};

static const int grid_size[textureChannels] = {4, 8, 16};
static const int texture_size[textureChannels] = {512, 256, 128};

static const double STEP_SIZE = 0.5;

int mod(int a, int b) {
    int r = a % b;
    return r < 0 ? r + b : r;
}

Cloud::Cloud(glm::dvec3 position, glm::dvec3 scale)
 : position(position), scale(scale) 
{

    for(int texIdx = 0; texIdx < textureChannels; texIdx++ ) {
        int texSize = texture_size[texIdx];
        int gridSize = grid_size[texIdx];

        std::default_random_engine generator;
        std::uniform_real_distribution<double> posDistribution(0.0, 1.0);

        std::vector<glm::dvec3> points(gridSize * gridSize * gridSize);
        double cellSize = 1.0 / gridSize;

        #pragma omp parallel for
        for (int x = 0; x < gridSize; x++) {
            for (int y = 0; y < gridSize; y++) {
                for (int z = 0; z < gridSize; z++) {
                    double rdX = posDistribution(generator);
                    double rdY = posDistribution(generator);
                    double rdZ = posDistribution(generator);
                    glm::dvec3 randomOffset = glm::dvec3(rdX, rdY, rdZ) * cellSize;
                    glm::dvec3 cellCorner = glm::dvec3(x, y, z) * cellSize;

                    int index = x + gridSize * (y + z * gridSize);
                    points[index] = cellCorner + randomOffset;
                }
            }
        }

        textures[texIdx] = std::vector<uint8_t>(texSize * texSize * texSize);
        double pixSize = 1.0 / texSize;
        int pixPerCell = texSize / gridSize;

        #pragma omp parallel for
        for (int x = 0; x < texSize; x++) {
            for (int y = 0; y < texSize; y++) {
                for (int z = 0; z < texSize; z++) {
                    glm::dvec3 pixelPos = glm::dvec3(x, y, z) * pixSize;

                    glm::ivec3 cell(x / pixPerCell, y / pixPerCell, z / pixPerCell);
                    double minDist = std::numeric_limits<double>::max(); 
                    
                    for(int cellIdx = 0; cellIdx < 27; cellIdx++) {
                        int idxX = mod(cell.x + offsets[cellIdx].x, gridSize);
                        int idxY = mod(cell.y + offsets[cellIdx].y, gridSize);
                        int idxZ = mod(cell.z + offsets[cellIdx].z, gridSize);
                        int index = idxX + gridSize * (idxY + idxZ * gridSize);

                        minDist = std::min(minDist, glm::distance2(pixelPos, points.at(index)));

                    }
                    textures[texIdx].at(x + texSize * (y + z * texSize)) = 255 - (uint8_t)(255.0 * std::min(1.0, 10.0*sqrt(minDist)));

                }
            }
        }
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
    float t = std::max(0.0, tNear);
    double density = 1.0;
    double color = 1.0;
    while(t < tFar)
    {
        glm::dvec3 ra = ray.at(t);
        // Current ray position
        glm::dvec3 rayPos = 0.5 + ((ray.at(t) - position) / scale); //Position between 0.0 and 1.0
        //std::cout<<rayPos<<std::endl;

        // Evaluate our signed distance field at the current ray position
        double sdf = -signedDistance(rayPos) / 255.0;

        //std::cout<<sdf<<std::endl;
        double transmittance = exp(sdf * STEP_SIZE * 0.05);   

        // Integrate scattering
        density *= transmittance;

        Ray lightRay(ray.at(t), normalize(glm::dvec3(0.0, 1.0, 1.0)));

        glm::dvec3 tMin2 = (first - lightRay.O) / lightRay.D;
        glm::dvec3 tMax2 = (second - lightRay.O) / lightRay.D;

        double tFar2 = compMin(max(tMin2, tMax2));
        double t2 = 0.0;
        while(t2 < tFar2) {
            glm::dvec3 rayPos2 = 0.5 + ((lightRay.at(t2) - position) / scale);

            double sdf2 = -signedDistance(rayPos2) / 255.0;

            double transmittance2 = exp(sdf2 * STEP_SIZE * 0.001); 

            color *= transmittance2;  
            if (color < 0.003)
            {
                color = 0.0;
                break;
            }
            
            t2 += STEP_SIZE * 80;
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
    double dist = 0.0;
    for(int i = 0; i < textureChannels; i++) {
        // pos.x = fmod(pos.x + 0.165402 * i, 1.0);
        // pos.y = fmod(pos.y + 0.545132 * i, 1.0);
        // pos.z = fmod(pos.z + 0.754132 * i, 1.0);
        int texSize = texture_size[i];
        int x = std::clamp((int)(pos.x * texSize), 0, texSize - 1);
        int y = std::clamp((int)(pos.y * texSize), 0, texSize - 1);
        int z = std::clamp((int)(pos.z * texSize), 0, texSize - 1);
        dist += textures[0].at(x + texSize * (y + z * texSize));
    }
    
    return dist;
    
}