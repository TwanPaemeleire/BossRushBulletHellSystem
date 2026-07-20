#include "pch.h"
#include "TransformComponent.h"
#include "EntityManager.h"
#include <memory>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/matrix_decompose.hpp"

namespace Bloodforge
{
	void Bloodforge::TransformComponent::SetLocalPosition(const Vector2& localPos)
	{
		m_LocalPosition = localPos;
		SetTransformDirty();
	}

	void Bloodforge::TransformComponent::SetLocalPosition(float x, float y)
	{
		SetLocalPosition({x, y});
	}

	void TransformComponent::SetLocalRotation(float degrees)
	{
		m_LocalRotationDeg = degrees;
		SetTransformDirty();
	}

	void TransformComponent::Rotate(float deltaDegrees)
	{
		m_LocalRotationDeg += deltaDegrees;
		SetTransformDirty();
	}

	void TransformComponent::SetLocalScale(const Vector2& scale)
	{
		m_LocalScale = scale;
		SetTransformDirty();
	}

	const glm::mat4& TransformComponent::GetWorldMatrix()
	{
		if (m_IsDirty) RecalculateMatrix();
		return m_WorldMatrix;
	}

	const glm::mat4& TransformComponent::GetLocalMatrix()
	{
		if (m_IsDirty) RecalculateMatrix();
		return m_LocalMatrix;
	}

	Vector2 Bloodforge::TransformComponent::GetWorldPosition()
	{
		const glm::mat4& world = GetWorldMatrix();
		return { world[3][0], world[3][1] };
	}

	float TransformComponent::GetWorldRotation()
	{
		glm::vec3 scale;
		glm::quat orientation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;

		glm::decompose(GetWorldMatrix(), scale, orientation, translation, skew, perspective);
		float radians = glm::eulerAngles(orientation).z;
		return glm::degrees(radians);
	}

	Vector2 TransformComponent::GetWorldScale()
	{
		glm::vec3 scale;
		glm::quat orientation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;

		glm::decompose(GetWorldMatrix(), scale, orientation, translation, skew, perspective);
		return { scale.x, scale.y };
	}

	Vector2 TransformComponent::GetForwardVector()
	{
		float radians = GetWorldRotation() * Vector2::DegreesToRadians;

		return
		{
			std::cos(radians),
			std::sin(radians)
		};
	}

	void TransformComponent::SetParent(int parentEntityId, bool keepWorldTransform)
	{
		if (IsChild(parentEntityId) || parentEntityId == OwnerEntityId || m_ParentEntityId == parentEntityId) return;

		glm::mat4 oldWorld = GetWorldMatrix();

		if (m_ParentEntityId != -1) // Remove self from current parent
		{
			Entity& oldParent = EntityManager::GetInstance().GetEntity(m_ParentEntityId);
			if (auto* oldParentTransform =
				EntityManager::GetInstance().GetComponent<TransformComponent>(oldParent))
			{
				oldParentTransform->RemoveChild(OwnerEntityId);
			}
		}

		m_ParentEntityId = parentEntityId; // Assign new parent
		glm::mat4 parentWorld{ 1.0f };

		if (m_ParentEntityId != -1) // If there is a new parent add ourself to it and get its world matrix
		{
			Entity& newParent = EntityManager::GetInstance().GetEntity(m_ParentEntityId);
			TransformComponent* newParentTransform = EntityManager::GetInstance().GetComponent<TransformComponent>(newParent);
			newParentTransform->AddChild(OwnerEntityId);
			parentWorld = newParentTransform->GetWorldMatrix();
		}
		if (keepWorldTransform) // Go back from previous world matrix to local matrix so that the world transform remains the same after changing parent
		{
			glm::mat4 newLocal = glm::inverse(parentWorld) * oldWorld;

			glm::vec3 scale;
			glm::quat orientation;
			glm::vec3 translation;
			glm::vec3 skew;
			glm::vec4 perspective;

			glm::decompose(newLocal, scale, orientation, translation, skew, perspective);

			m_LocalPosition = { translation.x, translation.y };
			m_LocalScale = { scale.x, scale.y };
			m_LocalRotationDeg = glm::degrees(glm::eulerAngles(orientation).z);
		}

		SetTransformDirty();
	}

	void Bloodforge::TransformComponent::SetTransformDirty()
	{
		m_IsDirty = true;
		for (int id : m_ChildEntityIds)
		{
			Entity& child = EntityManager::GetInstance().GetEntity(id);
			TransformComponent* childTransform = EntityManager::GetInstance().GetComponent<TransformComponent>(child);
			childTransform->SetTransformDirty();
		}
	}

	void Bloodforge::TransformComponent::RecalculateMatrix()
	{
		m_LocalMatrix = BuildLocalMatrix();

		if (m_ParentEntityId == -1)
		{
			m_WorldMatrix = m_LocalMatrix;
		}
		else
		{
			Entity& parent = EntityManager::GetInstance().GetEntity(m_ParentEntityId);
			TransformComponent* parentTransform = EntityManager::GetInstance().GetComponent<TransformComponent>(parent);
			m_WorldMatrix = parentTransform->GetWorldMatrix() * m_LocalMatrix;
		}
		m_IsDirty = false;
	}

	glm::mat4 TransformComponent::BuildLocalMatrix() const
	{
		const glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(m_LocalPosition.X, m_LocalPosition.Y, 0.0f));
		const glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_LocalRotationDeg), glm::vec3(0.f, 0.f, 1.f));
		const glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(m_LocalScale.X, m_LocalScale.Y, 1.0f));

		return translation * rotation * scale;
	}

	void TransformComponent::AddChild(int childEntityId)
	{
		m_ChildEntityIds.emplace_back(childEntityId);
	}

	void TransformComponent::RemoveChild(int childEntityId)
	{
		m_ChildEntityIds.erase(std::remove(m_ChildEntityIds.begin(), m_ChildEntityIds.end(), childEntityId), m_ChildEntityIds.end());
	}

	bool TransformComponent::IsChild(int childEntityId)
	{
		for (int id : m_ChildEntityIds)
		{
			if (id == childEntityId) return true;

			Entity& child = EntityManager::GetInstance().GetEntity(id);
			TransformComponent* childTransform = EntityManager::GetInstance().GetComponent<TransformComponent>(child);
			if (childTransform->IsChild(childEntityId)) return true;
		}
		return false;
	}
}