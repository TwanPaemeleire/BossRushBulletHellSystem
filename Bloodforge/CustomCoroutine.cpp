#include "pch.h"
#include "CustomCoroutine.h"
#include "CustomCoroutineDataComponent.h"
#include "EntityManager.h"
#include "CustomCoroutinesMapHolderComponent.h"

int Bloodforge::StartCoroutine(Coroutine coroutine)
{
    EntityManager& entityManager = EntityManager::GetInstance();
    Entity& dataEntity = entityManager.CreateEntity();
    int dataEntityId = dataEntity.Id;
    CustomCoroutineDataComponent* dataComp = entityManager.AddComponent<CustomCoroutineDataComponent>(dataEntity);
    dataComp->CoroutineHandle = coroutine;

    CustomCoroutinesMapHolderComponent& mapHolderComp = entityManager.GetOrCreateFirstEntityWithComponents<CustomCoroutinesMapHolderComponent>().GetComponent<CustomCoroutinesMapHolderComponent>();
    mapHolderComp.AddressToEntityId.insert({dataComp->CoroutineHandle.handle.address(), dataEntityId});

    coroutine.handle.resume();

    return dataEntityId;
}

void Bloodforge::StopCoroutine(int id)
{
    EntityManager& entityManager = EntityManager::GetInstance();
    Entity& entity = entityManager.GetEntity(id);
    CustomCoroutineDataComponent* dataComp = entityManager.GetComponent<CustomCoroutineDataComponent>(entity);
    entityManager.DestroyEntity(entity);

    CustomCoroutinesMapHolderComponent& mapHolderComp = entityManager.GetOrCreateFirstEntityWithComponents<CustomCoroutinesMapHolderComponent>().GetComponent<CustomCoroutinesMapHolderComponent>();
    mapHolderComp.AddressToEntityId.erase(dataComp->CoroutineHandle.handle.address());
    dataComp->CoroutineHandle.handle.destroy();
    dataComp->IsWaitingForNextFrame = false;
    dataComp->WaitTimeCounter = 0.0f;
}

void Bloodforge::WaitNextFrameAwaiter::await_suspend(std::coroutine_handle<> handle) const noexcept
{
    EntityManager& entityManager = EntityManager::GetInstance();

    CustomCoroutinesMapHolderComponent& mapHolderComp = entityManager.GetOrCreateFirstEntityWithComponents<CustomCoroutinesMapHolderComponent>().GetComponent<CustomCoroutinesMapHolderComponent>();
    int id = mapHolderComp.AddressToEntityId[handle.address()];
    
    CustomCoroutineDataComponent* dataComp = entityManager.GetComponent<CustomCoroutineDataComponent>(id);
    dataComp->IsWaitingForNextFrame = true;
}

void Bloodforge::WaitForSecondsAwaiter::await_suspend(std::coroutine_handle<> handle) const noexcept
{
    EntityManager& entityManager = EntityManager::GetInstance();

    CustomCoroutinesMapHolderComponent& mapHolderComp = entityManager.GetOrCreateFirstEntityWithComponents<CustomCoroutinesMapHolderComponent>().GetComponent<CustomCoroutinesMapHolderComponent>();
    int id = mapHolderComp.AddressToEntityId[handle.address()];

    CustomCoroutineDataComponent* dataComp = entityManager.GetComponent<CustomCoroutineDataComponent>(id);
    dataComp->WaitTimeCounter = SecondsToWait;
}