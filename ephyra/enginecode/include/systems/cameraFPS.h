/** \file cameraFPS.h */
#pragma once
#include "camera.h"

namespace Ephyra
{

	class FirstPersonCamera : public Camera {

	public:
		FirstPersonCamera(glm::vec3 pos = glm::vec3(0.0f, 1.0f, 10.0f));

		glm::mat4 getViewMatrix() override;
		glm::mat4 getProjectionMatrix() override;
		void update(float dt) override;
		void attachWindow(std::shared_ptr<Window>& W, std::shared_ptr<InputPoller>& handler) override;
		bool isRunning() override { return m_running; };

		void Movement(bool e) override { eMovement = e; };

		glm::vec3 cameraShake() override;

	private:

		bool eMovement = true;

		float m_speed;       // camera speed
		float m_sensitivity; // mouse sensitivity
		float m_pitchContrain; // don't allow pitch to go above/below 90 otherwise screen flips

		glm::vec3 momentum;

		bool m_running;
		std::shared_ptr<Window> m_window;
		std::shared_ptr<InputPoller> m_handler;

		void updateCameraVectors();
		void zoom(float offsetY);
		void look(float offsetX, float offsetY);  // mouse scroll



	};
}

