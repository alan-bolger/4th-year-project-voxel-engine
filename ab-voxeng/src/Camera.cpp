#include "Camera.h"

/// <summary>
/// Constructor for the Camera class.
/// </summary>
/// <param name="t_controller">A XboxOneController class object.</param>
ab::Camera::Camera(XboxOneController &t_controller) : m_controller(t_controller)
{
	m_eye = glm::vec3(0.0f, 5.0f, 10.0f);
	m_projectionMatrix = glm::perspective(45.0f, 16.0f / 9.0f, 1.0f, 1000.0f);
}

/// <summary>
/// Destructor for the Camera class.
/// </summary>
ab::Camera::~Camera()
{
	
}

/// <summary>
/// This function updates a 4x4 matrix with the current view.
/// </summary>
/// <param name="t_eye">This is the position of the center of the camera.</param>
/// <param name="t_pitch">Pitch is used to look up and down and is in degrees.</param>
/// <param name="t_yaw">Yaw is used to rotate left and right and is in degrees.</param>
void ab::Camera::camera(glm::vec3 t_eye, double t_pitch, double t_yaw)
{
	double f_cosPitch = cos(glm::radians(t_pitch));
	double f_sinPitch = sin(glm::radians(t_pitch));
	double f_cosYaw = cos(glm::radians(t_yaw));
	double f_sinYaw = sin(glm::radians(t_yaw));

	glm::vec3 f_xaxis = { f_cosYaw, 0, -f_sinYaw };
	glm::vec3 f_yaxis = { f_sinYaw * f_sinPitch, f_cosPitch, f_cosYaw * f_sinPitch };
	glm::vec3 f_zaxis = { f_sinYaw * f_cosPitch, -f_sinPitch, f_cosPitch * f_cosYaw };

	// Create a 4x4 view matrix from the right, up, forward and eye position vectors
	m_viewMatrix =
	{
		glm::vec4(f_xaxis.x,            f_yaxis.x,            f_zaxis.x,      0),
		glm::vec4(f_xaxis.y,            f_yaxis.y,            f_zaxis.y,      0),
		glm::vec4(f_xaxis.z,            f_yaxis.z,            f_zaxis.z,      0),
		glm::vec4(-dot(f_xaxis, m_eye), -dot(f_yaxis, m_eye), -dot(f_zaxis, m_eye), 1)
	};
}

/// <summary>
/// Get the current view.
/// </summary>
/// <returns>A 4x4 view matrix.</returns>
glm::mat4 ab::Camera::getView()
{
	return m_viewMatrix;
}

/// <summary>
/// Get the current projection.
/// </summary>
/// <returns>A 4x4 projection matrix.</returns>
glm::mat4 ab::Camera::getProjection()
{
	return m_projectionMatrix;
}

/// <summary>
/// Get the current camera direction.
/// </summary>
/// <returns>A directional vec3.</returns>
glm::vec3 ab::Camera::getDirection()
{
	return glm::vec3(m_direction.x, m_direction.y, m_direction.z);
}

/// <summary>
/// Get the current camera center position.
/// </summary>
/// <returns>The position of the camera.</returns>
glm::vec3 ab::Camera::getEye()
{
	return m_eye;
}

/// <summary>
/// Set position of camera.
/// </summary>
/// <param name="t_position"></param>
void ab::Camera::setEye(glm::vec3 t_position)
{
	m_eye = t_position;
}

/// <summary>
/// Compute the world direction vector.
/// This function is used for ray tracing.
/// </summary>
/// <param name="t_x">The X coordinate within [-1..1].</param>
/// <param name="t_y">The Y coordinate within [-1..1].</param>
/// <param name="t_result">This final result of the calculation.</param>
void ab::Camera::getEyeRay(float t_x, float t_y, glm::vec3 &t_result)
{
	glm::vec4 f_temp_1 = glm::vec4(t_x, t_y, 0.0f, 1.0f);
	m_inverseViewMatrix = m_projectionMatrix * m_viewMatrix;
	m_inverseViewMatrix = glm::inverse(m_inverseViewMatrix);
	f_temp_1 = m_inverseViewMatrix * f_temp_1;
	f_temp_1 = f_temp_1 * (1.0f / f_temp_1.w);
	glm::vec3 f_temp_2 = glm::vec3(f_temp_1.x, f_temp_1.y, f_temp_1.z);
	t_result = f_temp_2 - m_eye;
}

/// <summary>
/// Cast a ray from the mouse position.
/// This function is used for mouse picking.
/// </summary>
/// <param name="t_x">The mouse X position</param>
/// <param name="t_y"></param>
/// <returns></returns>
glm::vec3 ab::Camera::getRayFromMousePos(float t_x, float t_y)
{
	glm::vec2 ray_nds = glm::vec2(t_x, t_y);
	glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0f, 1.0f);
	glm::mat4 invProjMat = glm::inverse(getProjection());
	glm::vec4 eyeCoords = invProjMat * ray_clip;
	eyeCoords = glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
	glm::mat4 invViewMat = glm::inverse(getView());
	glm::vec4 rayWorld = invViewMat * eyeCoords;
	glm::vec3 rayDirection = glm::normalize(glm::vec3(rayWorld));

	return rayDirection;
}

/// <summary>
/// Update the camera and the controls.
/// </summary>
/// <param name="t_deltaTime">The current delta time.</param>
void ab::Camera::update(double t_deltaTime)
{
	// Move forward/backward
	if (m_controller.m_currentState.LeftThumbStick.y < -40.0f)
	{
		glm::vec3 f_tempDirection(m_direction.x, 0.0f, m_direction.z);
		glm::normalize(f_tempDirection);
		m_eye -= f_tempDirection * static_cast<float>(t_deltaTime * m_speed);
	}
	else if (m_controller.m_currentState.LeftThumbStick.y > 40.0f)
	{
		glm::vec3 f_tempDirection(m_direction.x, 0.0f, m_direction.z);
		glm::normalize(f_tempDirection);
		m_eye += f_tempDirection * static_cast<float>(t_deltaTime * m_speed);
	}

	// Strafe
	//if (m_controller.m_currentState.LeftThumbStick.x > 40.0f)
	//{
	//	glm::vec3 f_tempDirection(m_directionStrafe.x, m_directionStrafe.y, m_directionStrafe.z);
	//	glm::normalize(f_tempDirection);
	//	m_eye += f_tempDirection * static_cast<float>(t_deltaTime * m_speed);
	//}
	//else if (m_controller.m_currentState.LeftThumbStick.x < -40.0f)
	//{
	//	glm::vec3 f_tempDirection(m_directionStrafe.x, m_directionStrafe.y, m_directionStrafe.z);
	//	glm::normalize(f_tempDirection);
	//	m_eye -= f_tempDirection * static_cast<float>(t_deltaTime * m_speed);
	//}

	// Vertically up/down
	if (m_controller.m_currentState.RTrigger > 50.0f)
	{
		glm::vec3 f_tempDirection(m_directionVerticalStrafe.x, m_directionVerticalStrafe.y, m_directionVerticalStrafe.z);
		glm::normalize(f_tempDirection);
		m_eye += f_tempDirection * static_cast<float>(t_deltaTime * m_speed);
	}
	else if (m_controller.m_currentState.LTrigger > 50.0f)
	{
		glm::vec3 f_tempDirection(m_directionVerticalStrafe.x, m_directionVerticalStrafe.y, m_directionVerticalStrafe.z);
		glm::normalize(f_tempDirection);
		m_eye -= f_tempDirection * static_cast<float>(t_deltaTime * m_speed);
	}

	// Turn/look left and right
	if (m_controller.m_currentState.RightThumbStick.x < -40.0f)
	{
		m_yaw += m_turnSpeed * t_deltaTime;
		m_rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-m_turnSpeed * (float)t_deltaTime), glm::vec3(0.f, 1.f, 0.f));
		m_direction = m_direction * m_rotationMatrix;
		//m_directionStrafe = m_directionStrafe * m_rotationMatrix;

		if (m_yaw >= 360.0)
		{
			m_yaw = 0.0;
			m_direction = glm::vec4(0.f, 0.f, 1.f, 0.f);
			m_directionStrafe = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
		}
	}
	else if (m_controller.m_currentState.RightThumbStick.x > 40.0f)
	{
		m_yaw -= m_turnSpeed * t_deltaTime;
		m_rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(m_turnSpeed * (float)t_deltaTime), glm::vec3(0.f, 1.f, 0.f));
		m_direction = m_direction * m_rotationMatrix;
		//m_directionStrafe = m_directionStrafe * m_rotationMatrix;

		if (m_yaw <= -360.0)
		{
			m_yaw = 0.0;
			m_direction = glm::vec4(0.f, 0.f, 1.f, 0.f);
		}
	}

	// Look up and down
	if (m_controller.m_currentState.RightThumbStick.y < -40.0f)
	{
		m_pitch += (float)m_pitchSpeed * (float)t_deltaTime;
		m_rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(m_pitchSpeed * (float)t_deltaTime), glm::vec3(1.f, 0.f, 0.f));
		m_direction = m_direction * m_rotationMatrix;
	}
	else if (m_controller.m_currentState.RightThumbStick.y > 40.0f)
	{
		m_pitch -= (float)m_pitchSpeed * (float)t_deltaTime;
		m_rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-m_pitchSpeed * (float)t_deltaTime), glm::vec3(1.f, 0.f, 0.f));
		m_direction = m_direction * m_rotationMatrix;
	}

	// Update view matrix
	camera(m_eye, m_pitch, m_yaw);
}