#ifndef SPHERE_H
#define SPHERE_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Sphere
{
public:
    Sphere(const glm::vec3 &t_center, const float &t_radius, const glm::vec3 &t_surfaceColour, 
        const float &t_reflection = 0.0f, const float &t_transparency = 0.0f, const glm::vec3 &t_emissionColour = glm::vec3(0));
    ~Sphere();
    bool intersect(const glm::vec3 &t_rayorig, const glm::vec3 &t_raydir, float &t_0, float &t_1) const;

    glm::vec3 center;
    float radius;
    float radius2;
    glm::vec3 surfaceColour;
    glm::vec3 emissionColour;
    float transparency;
    float reflection;
};

#endif // !SPHERE_H
