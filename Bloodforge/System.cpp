#include "pch.h"
#include "System.h"

namespace Bloodforge
{
	System::~System() = default;

	void System::Enable()
	{
		if (m_IsEnabled) return;
		m_IsEnabled = true;
		OnEnable();
	}

	void System::Disable()
	{
		if (!m_IsEnabled) return;
		m_IsEnabled = false;
		OnDisable();
	}
}