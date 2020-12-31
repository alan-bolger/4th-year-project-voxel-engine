#ifndef CUBE_H
#define CUBE_H

#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Cube
{
public:
    Cube(const glm::vec3 &t_center, const glm::vec3 &t_dimensions, const glm::vec3 &t_surfaceColour,
        const float &t_reflection = 0.0f, const float &t_transparency = 0.0f, const glm::vec3 &t_emissionColour = glm::vec3(0));
    ~Cube();
    bool intersectCube(const glm::vec3 &t_rayorig, const glm::vec3 &t_raydir, const glm::vec3 &t_min, const glm::vec3 &t_max, float &t_0, float &t_1, const std::vector<Cube> &t_cubes) const;

    glm::vec3 center;
    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 surfaceColour;
    glm::vec3 emissionColour;
    float transparency;
    float reflection;
};

#endif // !CUBE_H
