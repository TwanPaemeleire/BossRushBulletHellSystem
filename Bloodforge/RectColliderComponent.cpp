#include "pch.h"
#include "RectColliderComponent.h"
#include "EntityManager.h"
#include "TransformComponent.h"

namespace Bloodforge
{
	const ColliderRect& RectColliderComponent::GetRect()
	{
		if (WorldPosNeedsUpdate)
		{
			UpdateWorldSpaceRect();
		}
		return WorldSpaceRect;
	}

	void RectColliderComponent::UpdateWorldSpaceRect()
	{
		EntityManager& entityManager = EntityManager::GetInstance();
		TransformComponent* transformComponent = entityManager.GetComponent<TransformComponent>(entityManager.GetEntity(OwnerEntityId));

		const Vector2 scale = transformComponent->GetWorldScale();
		const Vector2& worldPos = transformComponent->GetWorldPosition();
		const float rotation = transformComponent->GetWorldRotation();

		const Vector2 scaledSize = Vector2::Multiply(Size, scale);
		const Vector2 scaledOffset = Vector2::Multiply(Offset, scale);

		const float radians = rotation * Vector2::DegreesToRadians;
		const float cosRot = cos(radians);
		const float sinRot = sin(radians);

		const Vector2 rotatedOffset = 
		{ 
			scaledOffset.X * cosRot - scaledOffset.Y * sinRot, 
			scaledOffset.X * sinRot + scaledOffset.Y * cosRot 
		};

		WorldSpaceRect.X = worldPos.X + rotatedOffset.X;
		WorldSpaceRect.Y = worldPos.Y + rotatedOffset.Y;
		WorldSpaceRect.Width = scaledSize.X;
		WorldSpaceRect.Height = scaledSize.Y;

		const float halfWidth = WorldSpaceRect.Width / 2.0f;
		const float halfHeight = WorldSpaceRect.Height / 2.0f;

		const Vector2 center{ WorldSpaceRect.X, WorldSpaceRect.Y };

		WorldSpaceRect.TopLeft = center + RotateRectPoint(-halfWidth, -halfHeight, cosRot, sinRot);
		WorldSpaceRect.TopRight = center + RotateRectPoint(halfWidth, -halfHeight, cosRot, sinRot);
		WorldSpaceRect.BottomRight = center + RotateRectPoint(halfWidth, halfHeight, cosRot, sinRot);
		WorldSpaceRect.BottomLeft = center + RotateRectPoint(-halfWidth, halfHeight, cosRot, sinRot);

		WorldPosNeedsUpdate = false;
	}

	Vector2 RectColliderComponent::RotateRectPoint(float x, float y, float cosRot, float sinRot)
	{
		return
		{
			x * cosRot - y * sinRot,
			x * sinRot + y * cosRot
		};
	}
}