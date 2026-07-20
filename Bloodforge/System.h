#pragma once

namespace Bloodforge 
{
	class System
	{
	public:
		System() = default;
		~System();

		virtual void OnStart() {};
		virtual void OnUpdate() {};
		virtual void OnLateUpdate() {};
		virtual void OnFixedUpdate() {};
		virtual void OnRender() {};
		virtual void OnRenderUI() {};

		virtual void OnSceneSwitch() {};

		virtual void OnEnable() {};
		virtual void OnDisable() {};

		virtual void OnCleanup() {};

		bool IsEnabled() const { return m_IsEnabled; }
		void Enable();
		void Disable();
	private:
		bool m_IsEnabled = true;
	};
}