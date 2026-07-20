#pragma once
#include<string>

struct TTF_Font;
namespace Bloodforge
{
	class Font final
	{
	public:
		explicit Font(const std::string& path, float fontSize);
		~Font();

		TTF_Font* GetFontPointer() const {return m_Font;}
		float GetFontSize() const { return m_Fontsize; }
		const std::string& GetFontPath() const { return m_FontPath; }
	private:
		TTF_Font* m_Font;
		float m_Fontsize;
		std::string m_FontPath;
	};
}