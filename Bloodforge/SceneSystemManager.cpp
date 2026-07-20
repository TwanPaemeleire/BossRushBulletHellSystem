#include "pch.h"
#include "SceneSystemManager.h"
#include "System.h"
#include "EntityManager.h"
#include "SceneManagingDataComponent.h"
#include "SceneDataComponent.h"
#include "InputHandler.h"

#include "AudioSourceSystem.h"
#include "CollisionSystem.h"
#include "FunctionInvokeSystem.h"
#include "SpriteAnimatorSystem.h"
#include "SteamSupportSystem.h"
#include "TextSystem.h"
#include "SpriteTextRendererSystem.h"
#include "ButtonSystem.h"
#include "CustomCoroutineSystem.h"
#include "CameraSystem.h"

namespace Bloodforge
{
	SceneSystemManager::SceneSystemManager()
	{
		TryRegisterSystem<AudioSourceSystem>();
		TryRegisterSystem<CollisionSystem>();
		TryRegisterSystem<FunctionInvokeSystem>();
		TryRegisterSystem<SpriteAnimatorSystem>();
		TryRegisterSystem<SteamSupportSystem>();
		TryRegisterSystem<TextSystem>();
		TryRegisterSystem<SpriteTextRendererSystem>();
		TryRegisterSystem<ButtonSystem>();
		TryRegisterSystem<CustomCoroutineSystem>();
		TryRegisterSystem<CameraSystem>();
	}

	void SceneSystemManager::Cleanup()
	{
		for (const std::unique_ptr<System>& system : m_RegisteredSystems)
		{
			system->OnCleanup();
		}
		m_RegisteredSystems.clear();
		m_SystemMap.clear();
	}

	void SceneSystemManager::Start()
	{
		for (const std::unique_ptr<System>& system : m_RegisteredSystems)
		{
			if (!system->IsEnabled()) continue;
			system->OnStart();
		}
	}

	void SceneSystemManager::Update()
	{
		if (ShouldSwitchScenes())
		{
			SetCurrentScene();
		}

		for (const std::unique_ptr<System>& system : m_RegisteredSystems)
		{
			if (!system->IsEnabled()) continue;
			system->OnUpdate();
		}
	}

	void SceneSystemManager::FixedUpdate()
	{
		for (const std::unique_ptr<System>& system : m_RegisteredSystems)
		{
			if (!system->IsEnabled()) continue;
			system->OnFixedUpdate();
		}
	}

	void SceneSystemManager::LateUpdate()
	{
		EntityManager::GetInstance().DestroyMarkedForDestructionEntities();
		for (const std::unique_ptr<System>& system : m_RegisteredSystems)
		{
			if (!system->IsEnabled()) continue;
			system->OnLateUpdate();
		}
		EntityManager::GetInstance().DestroyMarkedForDestructionEntities();
	}

	void SceneSystemManager::Render() const
	{
		for (const std::unique_ptr<System>& system : m_RegisteredSystems)
		{
			if (!system->IsEnabled()) continue;
			system->OnRender();
		}
	}

	void SceneSystemManager::RenderUI()
	{
		for (const std::unique_ptr<System>& system : m_RegisteredSystems)
		{
			if (!system->IsEnabled()) continue;
			system->OnRenderUI();
		}
	}

	void SceneSystemManager::DisableAllSystems()
	{
		for (const std::unique_ptr<System>& system : m_RegisteredSystems)
		{
			system->Disable();
		}
	}

	void SceneSystemManager::EnableAllSystems()
	{
		for (const std::unique_ptr<System>& system : m_RegisteredSystems)
		{
			system->Enable();
		}
	}

	void SceneSystemManager::SetCurrentScene()
	{
		EntityManager& entityManager = EntityManager::GetInstance();

		SceneManagingDataComponent& sceneManagingDataComp = entityManager.GetFirstEntityWithComponents<SceneManagingDataComponent>()->GetComponent<SceneManagingDataComponent>();
		SceneDataComponent* sceneDataComp = entityManager.GetComponent<SceneDataComponent>(entityManager.GetEntity(sceneManagingDataComp.SceneToLoadDataEntityId));
		entityManager.DestroyEntitiesOnSceneSwitch();
		int previousSceneDataEntityId = sceneManagingDataComp.CurrentSceneDataEntityId;
		sceneManagingDataComp.CurrentSceneDataEntityId = sceneManagingDataComp.SceneToLoadDataEntityId;
		sceneManagingDataComp.ShouldLoadScene = false;
		sceneManagingDataComp.SceneToLoadDataEntityId = -1;
		sceneDataComp->LoadFunction();
		sceneManagingDataComp = entityManager.GetFirstEntityWithComponents<SceneManagingDataComponent>()->GetComponent<SceneManagingDataComponent>(); // Need to get again, because reference might be invalid after scene load function
		sceneManagingDataComp.OnSceneSwitch.Invoke(previousSceneDataEntityId, sceneManagingDataComp.CurrentSceneDataEntityId);
		for (const std::unique_ptr<System>& system : m_RegisteredSystems)
		{
			system->OnSceneSwitch();
		}
		Start();
	}

	bool SceneSystemManager::ShouldSwitchScenes()
	{
		std::optional<EntityView<SceneManagingDataComponent>> view =EntityManager::GetInstance().GetFirstEntityWithComponents<SceneManagingDataComponent>();
		if (!view.has_value()) return false;
		return view->GetComponent<SceneManagingDataComponent>().ShouldLoadScene;
	}
}