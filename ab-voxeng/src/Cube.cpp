#include "Cube.h"

/// <summary>
/// Constructor for the Cube class.
/// </summary>
/// <param name="t_center">Center point.</param>
/// <param name="t_dimensions">The X, Y and Z size of the cube.</param>
/// <param name="t_surfaceColour">Colour of cube surface.</param>
/// <param name="t_reflection">[OPTIONAL] Reflection value (0 - 1).</param>
/// <param name="t_transparency">[OPTIONAL] Transparency value (0 - 1).</param>
/// <param name="t_emissionColour">[OPTIONAL] Emission colour.</param>
Cube::Cube(const glm::vec3 &t_center, const glm::vec3 &t_dimensions, const glm::vec3 &t_surfaceColour,
	const float &t_reflection, const float &t_transparency, const glm::vec3 &t_emissionColour)
	:
	center(t_center),
	min(t_center - (t_dimensions / 2.f)),
	max(t_center + (t_dimensions / 2.f)),
	surfaceColour(t_surfaceColour),
	reflection(t_reflection),
	transparency(t_transparency),
	emissionColour(t_emissionColour)	
{

}

/// <summary>
/// Destructor for the Cube class.
/// </summary>
Cube::~Cube()
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
bool Cube::intersectCube(const glm::vec3 &t_rayorig, const glm::vec3 &t_raydir, const glm::vec3 &t_min, const glm::vec3 &t_max, float &t_0, float &t_1, const std::vector<Cube> &t_cubes) const
{
    float smallest = 100;
    bool found = false;

	glm::vec3 tMin = (t_min - t_rayorig) / t_raydir;
	glm::vec3 tMax = (t_max - t_rayorig) / t_raydir;
	glm::vec3 t1 = glm::min(tMin, tMax);
	glm::vec3 t2 = glm::max(tMin, tMax);
	t_0 = glm::max(glm::max(t1.x, t1.y), t1.z);
	t_1 = glm::min(glm::min(t2.x, t2.y), t2.z);
	glm::vec2 lambda = glm::vec2(t_0, t_1);

    if (lambda.x > 0.0 && lambda.x < lambda.y && lambda.x < smallest)
    {
        smallest = lambda.x;
        found = true;
    }

    return found;
}