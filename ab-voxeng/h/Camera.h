#ifndef CAMERA_H
#define CAMERA_H

#include "glew/glew.h"
#include "glew/wglew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "XboxOneController.h"

namespace ab
{
	class Camera
	{
	public:
		Camera(XboxOneController &t_controller);
		~Camera();
		void camera(glm::vec3 t_eye, double t_pitch, double t_yaw);
		glm::mat4 getView();
		glm::vec3 getDirection();
		glm::vec3 getEye();
		void update(double t_deltaTime);

	private:
		XboxOneController &m_controller;
		glm::mat4 m_viewMatrix;
		glm::vec3 m_eye;
		glm::mat4 m_rotationMatrix;
		glm::vec4 m_direction{ 0.f, 0.f, 1.f, 0.f }; // Direction that the camera is facing
		glm::vec4 m_directionStrafe{ 1.0f, 0.f, 0.f, 0.f };
		glm::vec4 m_directionVerticalStrafe{ 0.0f, 1.f, 0.f, 0.f }; // Only used for debug purposes
		float m_speed = 0.5f;
		float m_turnSpeed = 10.0f;
		float m_pitchSpeed = 10.0f;
		double m_yaw{ 0.0 }; // In degrees
		double m_pitch{ 0.0 }; // In degrees
	};
}

#endif // !CAMERA_H