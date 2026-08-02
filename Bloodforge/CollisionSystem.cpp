#include "pch.h"
#include "CollisionSystem.h"
#include "EntityManager.h"
#include "RectColliderComponent.h"
#include "TransformComponent.h"
#include "BloodRenderer.h"
#include "CircleColliderComponent.h"

namespace Bloodforge
{
	void CollisionSystem::OnLateUpdate()
	{
		std::unordered_set<uint64_t> currentFrame;
		UpdateAllCollisionRects();

		CheckRectRect(currentFrame);
		CheckCircleRect(currentFrame);
		CheckCircleCircle(currentFrame);

		CheckCollisionExits(currentFrame);

		m_LastFrameCollisions = std::move(currentFrame);
	}

	template<typename ColliderA, typename ColliderB>
	void CollisionSystem::HandleCollision(ColliderA& colliderA, ColliderB& colliderB, int firstEntityId, int secondEntityId, std::unordered_set<uint64_t>& currentFrameCollisions)
	{
		Entity& firstEntity = EntityManager::GetInstance().GetEntity(firstEntityId);
		Entity& secondEntity = EntityManager::GetInstance().GetEntity(secondEntityId);
		bool firstIgnoresSecond = colliderA.IgnoreTags.contains(secondEntity.Tag);
		bool secondIgnoresFirst = colliderB.IgnoreTags.contains(firstEntity.Tag);

		uint64_t packed = Pack(firstEntityId, secondEntityId);

		if (!m_LastFrameCollisions.contains(packed))
		{
			if (!firstIgnoresSecond) colliderA.OnCollisionEnterEvent.Invoke(firstEntityId, secondEntityId);
			if (!secondIgnoresFirst) colliderB.OnCollisionEnterEvent.Invoke(secondEntityId, firstEntityId);
		}

		if (!firstIgnoresSecond) colliderA.OnCollisionEvent.Invoke(firstEntityId, secondEntityId);
		if (!secondIgnoresFirst) colliderB.OnCollisionEvent.Invoke(secondEntityId, firstEntityId);

		Entity& updatedFirstEntity = EntityManager::GetInstance().GetEntity(firstEntityId);
		Entity& updatedSecondEntity = EntityManager::GetInstance().GetEntity(secondEntityId);
		if (!updatedFirstEntity.MarkedForDestruction && !updatedSecondEntity.MarkedForDestruction)
		{
			currentFrameCollisions.insert(packed);
		}
	}

	void CollisionSystem::CheckRectRect(std::unordered_set<uint64_t>& currentCollisions)
	{
		auto result = EntityManager::GetInstance().QueryEntities<RectColliderComponent, TransformComponent>();

		for (size_t index = 0; index < result.EntityViews.size(); ++index)
		{
			for (size_t innerIndex = index + 1; innerIndex < result.EntityViews.size(); ++innerIndex)
			{
				auto& rect1 = result.EntityViews[index].GetComponent<RectColliderComponent>();
				auto& rect2 = result.EntityViews[innerIndex].GetComponent<RectColliderComponent>();

				if (!RectRectOverlap(rect1, rect2)) continue;

				HandleCollision<Bloodforge::RectColliderComponent, Bloodforge::RectColliderComponent>(rect1, rect2, result.EntityViews[index].EntityId, result.EntityViews[innerIndex].EntityId, currentCollisions);
			}
		}
	}

	bool CollisionSystem::RectRectOverlap(RectColliderComponent& rect1, RectColliderComponent& rect2)
	{
		if (IsRecttOverlappingAABB(rect1.GetRect(), rect2.GetRect()))
		{
			TransformComponent* transform1 = EntityManager::GetInstance().GetComponent<TransformComponent>(rect1.OwnerEntityId);
			TransformComponent* transform2 = EntityManager::GetInstance().GetComponent<TransformComponent>(rect2.OwnerEntityId);
			if ((transform1->GetWorldRotation() == 0.0f && transform2->GetWorldRotation() == 0.0f) || IsRectOverlappingAxis(rect1.GetRect(), rect2.GetRect()))
			{
				return true;
			}
		}
		return false;
	}

	void CollisionSystem::OnRender()
	{
#if _DEBUG
		EntityQueryResult<RectColliderComponent, TransformComponent> result = EntityManager::GetInstance().QueryEntities<RectColliderComponent, TransformComponent>();
		for (EntityView<RectColliderComponent, TransformComponent>& view : result.EntityViews)
		{
			// Draw debug rendering
			RectColliderComponent& rect = view.GetComponent<RectColliderComponent>();
			const ColliderRect& collisionRect = rect.GetRect();
			BloodRenderer::GetInstance().DrawRectangle(collisionRect, Color(255, 255, 255, 255));
		}

		auto circleResult = EntityManager::GetInstance().QueryEntities<CircleColliderComponent, TransformComponent>();
		for (EntityView<CircleColliderComponent, TransformComponent>& view : circleResult.EntityViews)
		{
			CircleColliderComponent& circle = view.GetComponent<CircleColliderComponent>();
			TransformComponent* transform = EntityManager::GetInstance().GetComponent<TransformComponent>(circle.OwnerEntityId);
			Vector2 worldPos = transform->GetWorldPosition();
			BloodRenderer::GetInstance().DrawCircle(worldPos, circle.Radius, Color(255, 255, 255, 255));
		}
#endif
	}

	void CollisionSystem::OnCleanup()
	{
		m_LastFrameCollisions.clear();
	}

	void CollisionSystem::UpdateAllCollisionRects()
	{
		EntityQueryResult<RectColliderComponent> result = EntityManager::GetInstance().QueryEntities<RectColliderComponent>();
		for (EntityView<RectColliderComponent>& view : result.EntityViews)
		{
			view.GetComponent<RectColliderComponent>().UpdateWorldSpaceRect();
		}
	}

	bool CollisionSystem::IsRecttOverlappingAABB(const ColliderRect& rect1, const ColliderRect& rect2)
	{
		if (rect1.TopRight.X < rect2.TopLeft.X || rect1.TopLeft.X > rect2.TopRight.X) return false;
		if (rect1.BottomLeft.Y < rect2.TopLeft.Y || rect1.TopLeft.Y > rect2.BottomLeft.Y) return false;
		return true;
	}

	bool CollisionSystem::IsRectOverlappingAxis(const ColliderRect& rect1, const ColliderRect& rect2)
	{
		Vector2 axes[4]
		{
			(rect1.TopRight - rect1.TopLeft).GetPerpendicular().Normalized(),
			(rect1.BottomLeft - rect1.TopLeft).GetPerpendicular().Normalized(),
			(rect2.TopRight - rect2.TopLeft).GetPerpendicular().Normalized(),
			(rect2.BottomLeft - rect2.TopLeft).GetPerpendicular().Normalized()
		};

		for (const Vector2& axis : axes)
		{
			if (!IsOverlappingOnAxis(rect1, rect2, axis)) return false;
		}

		return true;
	}

	void CollisionSystem::CheckCircleCircle(std::unordered_set<uint64_t>& currentCollisions)
	{
		auto result = EntityManager::GetInstance().QueryEntities<CircleColliderComponent, TransformComponent>();

		for (size_t index = 0; index < result.EntityViews.size(); ++index)
		{
			for (size_t innerIndex = index + 1; innerIndex < result.EntityViews.size(); ++innerIndex)
			{
				auto& circle1 = result.EntityViews[index].GetComponent<CircleColliderComponent>();
				auto& circle2 = result.EntityViews[innerIndex].GetComponent<CircleColliderComponent>();

				if (!CircleCircleOverlap(circle1, circle2)) continue;

				HandleCollision<Bloodforge::CircleColliderComponent, Bloodforge::CircleColliderComponent>(circle1, circle2, result.EntityViews[index].EntityId, result.EntityViews[innerIndex].EntityId, currentCollisions);
			}
		}
	}

	bool CollisionSystem::CircleCircleOverlap(CircleColliderComponent & circle1, CircleColliderComponent & circle2)
	{
		TransformComponent* transform1 = EntityManager::GetInstance().GetComponent<TransformComponent>(circle1.OwnerEntityId);
		TransformComponent* transform2 = EntityManager::GetInstance().GetComponent<TransformComponent>(circle2.OwnerEntityId);
		Vector2 delta = transform1->GetWorldPosition() - transform2->GetWorldPosition();
		float radiusSum = circle1.Radius + circle2.Radius;

		return delta.SqrMagnitude() <= radiusSum * radiusSum;
	}

	void CollisionSystem::CheckCircleRect(std::unordered_set<uint64_t>& currentCollisions)
	{
		auto circleResult = EntityManager::GetInstance().QueryEntities<CircleColliderComponent, TransformComponent>();
		auto rectResult = EntityManager::GetInstance().QueryEntities<RectColliderComponent, TransformComponent>();

		for (size_t index = 0; index < circleResult.EntityViews.size(); ++index)
		{
			for (size_t innerIndex = 0; innerIndex < rectResult.EntityViews.size(); ++innerIndex)
			{
				auto& circle = circleResult.EntityViews[index].GetComponent<CircleColliderComponent>();
				auto& rect = rectResult.EntityViews[innerIndex].GetComponent<RectColliderComponent>();

				if (!CircleRectOverlap(circle, rect)) continue;

				HandleCollision<Bloodforge::CircleColliderComponent, Bloodforge::RectColliderComponent>(circle, rect, circleResult.EntityViews[index].EntityId, rectResult.EntityViews[innerIndex].EntityId, currentCollisions);
			}
		}
	}

	bool CollisionSystem::CircleRectOverlap(CircleColliderComponent & circle, RectColliderComponent & rect)
	{
		Vector2 rectCenter = (rect.GetRect().TopLeft + rect.GetRect().BottomRight) * 0.5f;
		TransformComponent* rectTransform = EntityManager::GetInstance().GetComponent<TransformComponent>(rect.OwnerEntityId);
		TransformComponent* circleTransform = EntityManager::GetInstance().GetComponent<TransformComponent>(circle.OwnerEntityId);

		Vector2 local = circleTransform->GetWorldPosition() - rectCenter;

		local.Rotate(-rectTransform->GetWorldRotation());

		float halfWidth = rect.GetRect().Width * 0.5f;
		float halfHeight = rect.GetRect().Height * 0.5f;

		Vector2 closest
		{
			std::clamp(local.X, -halfWidth, halfWidth),
			std::clamp(local.Y, -halfHeight, halfHeight)
		};

		Vector2 delta = local - closest;

		return delta.SqrMagnitude() <= circle.Radius * circle.Radius;
	}

	void CollisionSystem::CheckCollisionExits(std::unordered_set<uint64_t>& currentFrameCollisions)
	{
		EntityManager& entityManager = EntityManager::GetInstance();
		for (const auto& lastFramePair : m_LastFrameCollisions)
		{
			if(!currentFrameCollisions.contains(lastFramePair))
			{
				int first = UnpackFirst(lastFramePair);
				int second = UnpackSecond(lastFramePair);
				Entity& firstEntity = entityManager.GetEntity(first);
				Entity& secondEntity = entityManager.GetEntity(second);
				if (!firstEntity.IsAlive || !secondEntity.IsAlive) continue;

				RectColliderComponent* firstRect = entityManager.GetComponent<RectColliderComponent>(firstEntity);
				CircleColliderComponent* firstCircle = entityManager.GetComponent<CircleColliderComponent>(firstEntity);
				RectColliderComponent* secondRect = entityManager.GetComponent<RectColliderComponent>(secondEntity);
				CircleColliderComponent* secondCircle = entityManager.GetComponent<CircleColliderComponent>(secondEntity);

				if (!firstRect && !firstCircle) continue;
				if (!secondRect && !secondCircle) continue;
				bool firstIgnoresSecond = firstRect ? firstRect->IgnoreTags.contains(secondEntity.Tag) : firstCircle->IgnoreTags.contains(secondEntity.Tag);
				bool secondIgnoresFirst = secondRect ? secondRect->IgnoreTags.contains(firstEntity.Tag) : secondCircle->IgnoreTags.contains(firstEntity.Tag);
				if(!firstIgnoresSecond) firstRect ? firstRect->OnCollisionExitEvent.Invoke(first, second) : firstCircle->OnCollisionExitEvent.Invoke(first, second);
				if(!secondIgnoresFirst) secondRect ? secondRect->OnCollisionExitEvent.Invoke(second, first) : secondCircle->OnCollisionExitEvent.Invoke(second, first);
			}
		}
	}

	uint64_t CollisionSystem::Pack(int a, int b)
	{
		return (static_cast<uint64_t>(a) << 32) | b;
	}

	int CollisionSystem::UnpackFirst(uint64_t packed)
	{
		return static_cast<int>(packed >> 32);
	}

	int CollisionSystem::UnpackSecond(uint64_t packed)
	{
		return static_cast<int>(packed & 0xFFFFFFFF);
	}

	void CollisionSystem::ProjectRectOntoAxis(const ColliderRect& rect, const Vector2& axis, float& min, float& max)
	{
		const float points[4] = 
		{ 
			Vector2::Dot(rect.TopLeft, axis), 
			Vector2::Dot(rect.TopRight, axis),
			Vector2::Dot(rect.BottomRight, axis),
			Vector2::Dot(rect.BottomLeft, axis)
		};

		min = std::min(std::min(points[0], points[1]), std::min(points[2], points[3]));
		max = std::max(std::max(points[0], points[1]), std::max(points[2], points[3]));
	}

	bool CollisionSystem::IsOverlappingOnAxis(const ColliderRect& rect1, const ColliderRect& rect2, const Vector2& axis)
	{
		float minA;
		float maxA;
		float minB;
		float maxB;
		ProjectRectOntoAxis(rect1, axis, minA, maxA);
		ProjectRectOntoAxis(rect2, axis, minB, maxB);

		return !(maxA < minB || maxB < minA);
	}
}