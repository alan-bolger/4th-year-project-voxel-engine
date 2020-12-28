#ifndef RAYTRACER_H
#define RAYTRACER_H

#define NOMINMAX

#include <vector>
#include <fstream>

#include "Sphere.h"
#include "Camera.h"

#define PI 3.141592653589793

namespace ab
{
	class RayTracer
	{
	public:
		RayTracer();
		~RayTracer();
		glm::vec3 trace(const glm::vec3 &t_rayorig, const glm::vec3 &t_raydir, const std::vector<Sphere> &t_spheres, const int &t_depth);
		GLuint draw(const std::vector<Sphere> &t_spheres, ab::Camera &t_camera);

	private:
		static const int MAX_RAY_DEPTH = 5;

		float mix(const float &t_a, const float &t_b, const float &t_mix);
	};
}

#endif // !RAYTRACER_H
