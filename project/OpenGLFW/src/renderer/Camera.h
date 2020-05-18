#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "common/commonDefine.h"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW			= -90.0f;
const float PITCH		= 0.0f;
const float SPEED		= 20.5f;
const float SENSITIVITY	= 0.05f;
const float FOV			= 45.0f;


class Camera
{
public:
	glm::vec3 pos;
	glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	// euler angles
	float yaw;
	float pitch;

	// camera options;
	float movementSpeed = SPEED;
	float mouseSensitivity = SENSITIVITY;
	float fov = FOV;

	// constructor with vectors
	Camera(glm::vec3 position = V::zero, glm::vec3 up = V::up, float yaw = YAW, float pitch = PITCH)
		: pos(position)
		, worldUp(up)
		, yaw(yaw)
		, pitch(pitch)
	{
		updateCameraVectors();
	}

	glm::mat4 getViewMatrix() {
		return glm::lookAt(pos, pos + forward, up);
	}

	// process input received from any keyboard-like input system.
	// accepts input parameter in the form of camera defined ENUM
	void processKeyboard(CameraMovement direction, float deltaTime) {
		float velocity = movementSpeed * deltaTime;

		if (direction == FORWARD) pos += forward * velocity;
		if (direction == BACKWARD) pos -= forward * velocity;
		if (direction == LEFT) pos -= right * velocity;
		if (direction == RIGHT) pos += right * velocity;
	}

	// process input received from a mouse input system.
	// Expects the offset value in both the x and y direction.
	void processMouseMovement(float xOffset, float yOffset, bool isConstrainPitch = true) {
		xOffset *= mouseSensitivity;
		yOffset *= mouseSensitivity;

		yaw += xOffset;
		pitch -= yOffset;

		if (isConstrainPitch)
		{
			if (pitch > 89.0f) pitch = 89.0f;
			if (pitch < -89.0f) pitch = -89.0f;
		}

		updateCameraVectors();
	}

	void processMouseScroll(float yOffset) {
		if (fov >= 1.0f && fov <= 45.0f)
		{
			fov -= yOffset;
		}
		else if (fov < 1.0f) fov = 1.0f;
		else if (fov > 45.0f) fov = 45.0f;
	}

private:
	// calculates the forward vector from the camera's (updated) Euler Angles
	void updateCameraVectors() {
		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		forward = glm::normalize(direction);

		// re-calculate the Right and Up vector
		right = glm::normalize(glm::cross(forward, worldUp));
		up = glm::normalize(glm::cross(right, forward));
	}
};
