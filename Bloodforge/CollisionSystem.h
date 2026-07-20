#pragma once
#include "System.h"
#include <unordered_set>

namespace Bloodforge
{
	struct ColliderRect;
	struct Vector2;
	struct RectColliderComponent;
	struct TransformComponent;
	class CollisionSystem final : public System
	{
	public:
		virtual void OnLateUpdate() override;
		virtual void OnRender() override;
		virtual void OnCleanup() override;

	private:
		void UpdateAllCollisionRects();
		bool IsOverlappingAABB(const ColliderRect& rect1, const ColliderRect& rect2);
		bool IsOverlapping(const ColliderRect& rect1, const ColliderRect& rect2);

		uint64_t Pack(int a, int b);
		int UnpackFirst(uint64_t packed);
		int UnpackSecond(uint64_t packed);

		void ProjectRectOntoAxis(const ColliderRect& rect, const Vector2& axis, float& min, float& max);
		bool IsOverlappingOnAxis(const ColliderRect& rect1, const ColliderRect& rect2, const Vector2& axis);
		std::unordered_set<uint64_t> m_LastFrameCollisions;
	};
}