#pragma once
#include "Component.h"
#include "Vector2.h"
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"

namespace Bloodforge
{
	struct TransformComponent : Component<TransformComponent>
	{
	public:
		// Local setters
		void SetLocalPosition(const Vector2& localPos);
		void SetLocalPosition(float x, float y);
		void SetLocalRotation(float degrees);
		void Rotate(float deltaDegrees);
		void SetLocalScale(const Vector2& scale);

		// Local getters
		const Vector2& GetLocalPosition() { return m_LocalPosition; }
		float GetLocalRotation() { return m_LocalRotationDeg; }
		const Vector2& GetLocalScale() { return m_LocalScale; }

		// World getters
		const glm::mat4& GetWorldMatrix();
		const glm::mat4& GetLocalMatrix();
		Vector2 GetWorldPosition();
		float GetWorldRotation();
		Vector2 GetWorldScale();

		Vector2 GetForwardVector();

		void SetParent(int parentEntityId, bool keepWorldTransform = true);
		int GetParentEntityId() const { return m_ParentEntityId; }
		const std::vector<int>& GetChildEntityIds() const { return m_ChildEntityIds; }
		void SetTransformDirty();

	private:
		void RecalculateMatrix();
		glm::mat4 BuildLocalMatrix() const;

		void AddChild(int childEntityId);
		void RemoveChild(int childEntityId);
		bool IsChild(int childEntityId);

		int m_ParentEntityId = -1;
		std::vector<int> m_ChildEntityIds;

		bool m_IsDirty{ true };

		Vector2 m_LocalPosition{ 0.f, 0.f };
		float m_LocalRotationDeg{ 0.f };
		Vector2 m_LocalScale{ 1.f, 1.f };

		Vector2 m_WorldPosition;
		glm::mat4 m_LocalMatrix{ 1.f };
		glm::mat4 m_WorldMatrix{ 1.f };
	};
}