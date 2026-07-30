#include "BasicProjectileSystem.h"
#include <EntityManager.h>
#include "BasicProjectile.h"
#include <BloodTime.h>
#include <WindowUtils.h>

using namespace Bloodforge;

void BasicProjectileSystem::OnUpdate()
{
	EntityManager& entityManager = EntityManager::GetInstance();

	EntityQueryResult<BasicProjectile, TransformComponent> result = entityManager.QueryEntities<BasicProjectile, TransformComponent>();
	for (EntityView<BasicProjectile, TransformComponent>& view : result.EntityViews)
	{
		TransformComponent& transform = view.GetComponent<TransformComponent>();
		BasicProjectile& projectile = view.GetComponent<BasicProjectile>();

		// Check out of bounds (basic for now, will include rotation and such later)
		float windowWidth = WindowUtils::GetWindowSize().X;
		float windowHeight = WindowUtils::GetWindowSize().Y;
		Vector2 newPos = transform.GetLocalPosition();

		if (newPos.X >= windowWidth || newPos.X < 0.0f || newPos.Y >= windowHeight || newPos.Y <= 0)
		{
			entityManager.DestroyEntity(view.EntityId);
		}
		

		newPos += transform.GetForwardVector() * projectile.Speed * BloodTime::GetInstance().DeltaTime;
		transform.SetLocalPosition(newPos);
	}
}