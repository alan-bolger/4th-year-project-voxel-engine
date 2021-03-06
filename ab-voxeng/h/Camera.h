// ***********************************************
// * Camera.h and Camera.cpp - Alan Bolger, 2020 *
// ***********************************************

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
		glm::mat4 getView();
		glm::mat4 getProjection();
		glm::vec3 getDirection();
		glm::vec3 getEye();
		void setEye(glm::vec3 t_position);
		void getEyeRay(float t_x, float t_y, glm::vec3 &t_result);
		glm::vec3 getRayFromMousePos(float t_x, float t_y);
		void update(double t_deltaTime);

	private:
		XboxOneController &m_controller;
		glm::mat4 m_viewMatrix;
		glm::mat4 m_projectionMatrix;
		glm::mat4 m_inverseViewMatrix;
		glm::vec3 m_eye;
		glm::mat4 m_rotationMatrix;
		glm::vec4 m_direction{ 0.f, 0.f, 1.f, 0.f }; // Direction that the camera is facing
		glm::vec4 m_directionStrafe{ 1.0f, 0.f, 0.f, 0.f };
		glm::vec4 m_directionVerticalStrafe{ 0.0f, 1.f, 0.f, 0.f }; // Only used for debug purposes
		float m_speed = 0.1f;
		float m_turnSpeed = 0.2f;
		float m_pitchSpeed = 0.2f;
		double m_yaw{ 0.0 }; // In degrees
		double m_pitch{ 0.0 }; // In degrees

		void camera(glm::vec3 t_eye, double t_pitch, double t_yaw);
	};
}

#endif // !CAMERA_H