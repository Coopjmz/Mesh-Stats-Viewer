#include "pch.h"
#include "Utils/FileUtils.h"

namespace utils
{
	std::optional<std::string> ReadFile(const fs::path& filepath)
	{
		std::ifstream file(filepath.c_str());
		if (!file.is_open()) return {};

		return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	}

	bool WriteFile(const fs::path& filepath, const std::string& data)
	{
		std::ofstream file(filepath.c_str());
		if (!file.is_open()) return false;

		file << data;
		return true;
	}

	std::optional<fs::path> OpenFileDialog(
		const std::string& title /* = "Open" */,
		const fs::path& defaultPath /* = "" */,
		const std::vector<std::string>& filters /* = { "All Files (*.*)", "*" } */
	)
	{
		const auto filepaths = pfd::open_file(title, defaultPath.string(), filters).result();
		if (filepaths.empty()) return {};

		return filepaths.front();
	}

	std::optional<fs::path> SaveAsFileDialog(
		const std::string& title /* = "Save As" */,
		const fs::path& defaultPath /* = "" */,
		const std::vector<std::string>& filters /* = { "All Files (*.*)", "*" } */,
		const bool confirmOverwrite /* = false */
	)
	{
		const auto options = confirmOverwrite ? pfd::opt::force_overwrite : pfd::opt::none;
		fs::path filepath = pfd::save_file(title, defaultPath.string(), filters, options).result();
		if (filepath.empty()) return {};

		if (filters.size() == 2)
		{
			const auto& filter = filters.back();
			if (filter.size() >= 2)
			{
				size_t count = filter.find(' ');
				count = count != std::string::npos ? count : filter.size();

				const std::string_view extention(filter.c_str() + 1, count - 1);
				if (filepath.extension() != extention)
					filepath += extention;
			}
			
		}

		return filepath;
	}
}