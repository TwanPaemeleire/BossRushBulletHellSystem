#pragma once
#include <string>
namespace Bloodforge
{
	class CustomCursor;
	struct Vector2;
}

namespace Bloodforge::WindowUtils
{
	void SetWindowSize(int sizeX, int sizeY);
	Vector2 GetWindowSize();
	void SetWindowAlwaysOnTop(bool alwaysOnTop);
	void SetWindowBordered(bool borderless);
	void SetWindowFullScreen(bool fullscreen);
	void SetCustomCursor(CustomCursor* customCursor);
	void SetWindowIcon(const std::string& path);
	void SetWindowTitle(const std::string& title);
	Vector2 GetWindowPosition();
	void SetWindowPosition(int posX, int posY);
}