#pragma once

namespace utils
{
	std::optional<std::string> ReadFile(const fs::path& filepath);
	bool WriteFile(const fs::path& filepath, const std::string& data);

	std::optional<fs::path> OpenFileDialog(
		const std::string& title = "Open",
		const fs::path& defaultPath = "",
		const std::vector<std::string>& filters = { "All Files (*.*)", "*" }
	);

	std::optional<fs::path> SaveAsFileDialog(
		const std::string& title = "Save As",
		const fs::path& defaultPath = "",
		const std::vector<std::string>& filters = { "All Files (*.*)", "*" },
		const bool confirmOverwrite = false
	);
}