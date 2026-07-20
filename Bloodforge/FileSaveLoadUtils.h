#pragma once
#include <fstream>
#include "json.hpp"
#include <string>
#include <stdexcept>

namespace Bloodforge::FileSaveLoadUtils
{
	template<typename SaveFileType>
	void SaveFile(const SaveFileType& saveFile, const std::string& path)
	{
		std::ofstream outStream(path);
		if (!outStream.is_open())
		{
			throw std::runtime_error("Failed to open file for writing: " + path);
		}

		nlohmann::json j = saveFile;
		outStream << j.dump(4);
	}

	template<typename SaveFileType>
	SaveFileType LoadFile(const std::string& path)
	{
		std::ifstream inStream(path);
		if (!inStream.is_open())
		{
			throw std::runtime_error("Failed to open file for reading: " + path);
		}

		nlohmann::json parser;
		inStream >> parser;
		return parser.get<SaveFileType>();
	}
}