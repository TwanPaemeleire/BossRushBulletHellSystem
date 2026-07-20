#include "pch.h"
#include "Bloodforge.h"
#include <chrono>
#include <thread>
#include "BloodTime.h"
#include "BloodRenderer.h"
#include "InputHandler.h"
#include <filesystem>
#include "SceneSystemManager.h"
#include "EntityManager.h"
#include "ResourceManager.h"
#include "GlobalEventHandler.h"
#include <steam_api.h>

namespace Bloodforge
{
	void Bloodforge::Run()
	{
		auto& sceneSystemManager = SceneSystemManager::GetInstance();
		auto& renderer = BloodRenderer::GetInstance();
		auto& bloodTime = BloodTime::GetInstance();
		auto& inputHandler = InputHandler::GetInstance();
		
		auto lastTime = std::chrono::high_resolution_clock::now();
		float lag = 0.f;
		
		while (m_ShouldContinue)
		{
			const auto currentTime = std::chrono::high_resolution_clock::now();
			bloodTime.SetDeltaTime(std::chrono::duration<float>(currentTime - lastTime).count());

			lastTime = currentTime;
			lag += bloodTime.DeltaTime;
		
			inputHandler.ProcessInput();

			while (lag >= m_FixedTimeStep)
			{
				sceneSystemManager.FixedUpdate();
				lag -= m_FixedTimeStep;
			}
			sceneSystemManager.Update();
			sceneSystemManager.LateUpdate();
			renderer.Render();

			const auto sleepTime = std::chrono::milliseconds(m_MsPerFrame) - (std::chrono::high_resolution_clock::now() - currentTime);
			std::this_thread::sleep_for(sleepTime);
		}

		EntityManager::GetInstance().DestroyAllEntities();
		GlobalEventHandler::GetInstance().Cleanup();
		inputHandler.Cleanup();
		ResourceManager::GetInstance().Cleanup();
		sceneSystemManager.Cleanup();
		SteamAPI_Shutdown();
		renderer.Cleanup();
	}
	void Bloodforge::RequestStop()
	{
		m_ShouldContinue = false;
	}

	void Bloodforge::SetResourcesDirectory(const std::string& path) const
	{
		if (std::filesystem::exists(path))
		{
			std::filesystem::current_path(path);
		}
		else
		{
			throw std::runtime_error("Tried setting resources directory to a folder which doesn't exist.");
		}
	}
}