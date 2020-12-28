#include "Sphere.h"

/// <summary>
/// Constructor for the Sphere class.
/// </summary>
Sphere::Sphere(const glm::vec3 &t_center, const float &t_radius, const glm::vec3 &t_surfaceColour, 
	const float &t_reflection, const float &t_transparency, const glm::vec3 &t_emissionColour)
	:
    center(t_center),
	radius(t_radius),
	radius2(t_radius * t_radius),
	surfaceColour(t_surfaceColour),
	emissionColour(t_emissionColour),
	transparency(t_transparency),
	reflection(t_reflection)
{

}

/// <summary>
/// Destructor for the Sphere class.
/// </summary>
Sphere::~Sphere()
{

}

/// <summary>
/// Intersect.
/// </summary>
/// <param name="t_rayorig">The ray origin.</param>
/// <param name="t_raydir">The ray direction.</param>
/// <param name="t_0">Near value.</param>
/// <param name="t_1">Far value.</param>
/// <returns>True if an intersect is found.</returns>
bool Sphere::intersect(const glm::vec3 &t_rayorig, const glm::vec3 &t_raydir, float &t_0, float &t_1) const
{
    glm::vec3 l = center - t_rayorig;
    float tca = glm::dot(l, t_raydir);

    if (tca < 0) return false;

    float d2 = glm::dot(l, l) - tca * tca;

    if (d2 > radius2) return false;

    float thc = sqrt(radius2 - d2);
    t_0 = tca - thc;
    t_1 = tca + thc;

    return true;
}
