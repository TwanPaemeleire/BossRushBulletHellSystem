#pragma once
#include "System.h"
#include <unordered_set>

namespace Bloodforge
{
	struct ColliderRect;
	struct Vector2;
	struct RectColliderComponent;
	struct CircleColliderComponent;
	struct TransformComponent;
	struct Entity;
	class CollisionSystem final : public System
	{
	public:
		virtual void OnLateUpdate() override;
		virtual void OnRender() override;
		virtual void OnCleanup() override;

	private:
		template<typename ColliderA, typename ColliderB>
		void HandleCollision(ColliderA& colliderA, ColliderB& colliderB, int firstEntityId, int secondEntityId, std::unordered_set<uint64_t>& currentFrameCollisions);

		// Rects
		void CheckRectRect(std::unordered_set<uint64_t>& currentCollisions);
		bool RectRectOverlap(RectColliderComponent& rect1, RectColliderComponent& rect2);
		void UpdateAllCollisionRects();
		bool IsRecttOverlappingAABB(const ColliderRect& rect1, const ColliderRect& rect2);
		bool IsRectOverlappingAxis(const ColliderRect& rect1, const ColliderRect& rect2);
		void ProjectRectOntoAxis(const ColliderRect& rect, const Vector2& axis, float& min, float& max);
		bool IsOverlappingOnAxis(const ColliderRect& rect1, const ColliderRect& rect2, const Vector2& axis);

		// Circles
		void CheckCircleCircle(std::unordered_set<uint64_t>& currentCollisions);
		bool CircleCircleOverlap(CircleColliderComponent& circle1, CircleColliderComponent& circle2);
		void CheckCircleRect(std::unordered_set<uint64_t>& currentCollisions);
		bool CircleRectOverlap(CircleColliderComponent& circle, RectColliderComponent& rect);

		void CheckCollisionExits(std::unordered_set<uint64_t>& currentFrameCollisions);

		uint64_t Pack(int a, int b);
		int UnpackFirst(uint64_t packed);
		int UnpackSecond(uint64_t packed);

		std::unordered_set<uint64_t> m_LastFrameCollisions;
	};
}