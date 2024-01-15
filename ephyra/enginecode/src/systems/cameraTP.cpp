/** \ file cameraFPS*/

#include "engine_pch.h"
#include "systems/cameraTP.h"
#include <GLFW/glfw3.h>
#include "systems/perlinNoise.h"

namespace Engine
{

	ThirdPersonCamera::ThirdPersonCamera(glm::vec3 pos) : Camera(pos), m_speed(1.5f), m_sensitivity(0.075f), m_running(true)
	{
		m_zoom = 45.0;
		m_yaw = -90.0f;
		m_pitch = 0.0f;
		m_pitchContrain = 89.0f;
		m_allowLook = true;
		updateCameraVectors();
		momentum = { 0, 0, 0 };
	}

	//getters
	glm::mat4 ThirdPersonCamera::getViewMatrix()
	{
		return glm::lookAt(m_offsetPos, m_position, WORLD_UP);
	}

	glm::mat4 ThirdPersonCamera::getProjectionMatrix()
	{
		return glm::perspective(glm::radians(m_zoom), (float)m_window->getWidthf() / (float)m_window->getHeightf(), m_nearP, m_farP);
	}

	//updates
	void ThirdPersonCamera::attachWindow(std::shared_ptr<Window>& window, std::shared_ptr<InputPoller>& handler)
	{
		m_window = window;
		m_handler = handler;
	}

	glm::vec3 ThirdPersonCamera::cameraShake()
	{
		perlinOffset = perlin(shakeIncrementer);

		glm::vec3 dir = glm::cross(m_front, m_right);
		return perlinOffset * 5 * dir;
	}

	// Camera Key movement
	// Standard WSAD, could extend for any other movement you want ( eg, up, down, etc..)
	void ThirdPersonCamera::update(float dt)
	{
		float vel = m_speed * dt;
		bool mouseMove = m_handler->mouseHasMoved();

		if (m_handler->keyHasBeenPressed()) {
			if (m_handler->isKeyPressed(GLFW_KEY_LEFT_SHIFT) && (momentum.x != 0 || momentum.y != 0 || momentum.z != 0)) {
				cameraShake();
				shakeIncrementer += dt * 3;
				vel *= 1.5;
			}
			if (m_handler->isKeyPressed(GLFW_KEY_W)) {
				momentum += glm::vec3{-1,0,-1} * (-m_front) * vel;
			}
			if (m_handler->isKeyPressed(GLFW_KEY_S)) {
				momentum -= glm::vec3{ -1,0,-1 } * (-m_front) * vel;
			}
			if (m_handler->isKeyPressed(GLFW_KEY_A)) {
				m_yaw -= 60 * vel;
			}
			if (m_handler->isKeyPressed(GLFW_KEY_D)) {
				//momentum -= glm::vec3{ -1,0,-1 } * m_right * vel;
				m_yaw += 60 * vel;
			}
			if (m_handler->isKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
				momentum += glm::vec3{ 0,-1,0 } * vel;
			}
			if (m_handler->isKeyPressed(GLFW_KEY_SPACE)) {
				momentum -= glm::vec3{ 0,-1,0 } * vel;
			}
			if (m_handler->isKeyPressed(GLFW_KEY_ESCAPE)) {
				m_running = false;
			}
			if (m_handler->isKeyPressed(GLFW_KEY_TAB)) {
				m_allowLook = true;
			}
			if (m_handler->isKeyPressed(GLFW_KEY_LEFT_ALT)) {
				m_allowLook = false;
			}
		}
		if (!m_allowLook) {
			glfwSetInputMode((GLFWwindow*)m_window->getNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else {
			glfwSetInputMode((GLFWwindow*)m_window->getNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			look(m_handler->getMouseDeltaX(), m_handler->getMouseDeltaY());
			zoom(m_handler->getMouseScrollY());
		}

		m_position = m_position + momentum;

		if ((momentum.x > 0.001 || momentum.x < -0.001))
			momentum.x = momentum.x * 0.90;
		else
			momentum.x = 0;
		if ((momentum.y > 0.001 || momentum.y < -0.001))
			momentum.y = momentum.y * 0.90;
		else
			momentum.y = 0;
		if ((momentum.z > 0.001 || momentum.z < -0.001))
			momentum.z = momentum.z * 0.90;
		else
			momentum.z = 0;

		

		m_handler->endFrame();  // reset delta values
	}



	void ThirdPersonCamera::updateCameraVectors()
	{
		m_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		m_front.y = sin(glm::radians(m_pitch));
		m_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		m_front = glm::normalize(m_front);
		//re-calculate the Right and Up vector
		m_right = glm::normalize(glm::cross(m_front, WORLD_UP));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		m_up = glm::normalize(glm::cross(m_right, m_front));

		m_offsetPos = m_position - ((glm::vec3(5)*(m_front)) * (1-(perlinOffset/(float)3)));
	}


	void ThirdPersonCamera::zoom(float offsetY)
	{
		float min = 20.0f;
		float max = 45.0f;
		if (m_zoom >= min && m_zoom <= max)
			m_zoom -= offsetY;
		if (m_zoom <= min) m_zoom = min;
		if (m_zoom >= max) m_zoom = max;
	}


	// move mouse around and update pitch and yaw
	void ThirdPersonCamera::look(float x, float y)
	{

		//x *= -m_sensitivity;
		y *= m_sensitivity;

		//m_yaw += x;
		m_pitch += y;

		if (m_pitch > m_pitchContrain) m_pitch = m_pitchContrain;
		if (m_pitch < -m_pitchContrain) m_pitch = -m_pitchContrain;

		updateCameraVectors();

	}

}
