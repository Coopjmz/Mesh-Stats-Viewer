#pragma once

class Window
{
public:
	struct Settings
	{
		// Required
		std::string Title;
		uint32_t Width;
		uint32_t Height;
		bool VSync;
		ImVec4 BackgroundColor;

		// Optional
		fs::path FontPath;
		float FontSize;

		static std::optional<Window::Settings> LoadFromFile(const fs::path& filepath);

		Settings(const std::string& title, const uint32_t width, const uint32_t height, const bool vsync,
			const ImVec4& backgroundColor, const fs::path& fontPath = {}, const float fontSize = 0.f);

		Settings(std::string&& title, const uint32_t width, const uint32_t height, const bool vsync,
			const ImVec4& backgroundColor, fs::path&& fontPath = {}, const float fontSize = 0.f);
	};

public:
	Window(const Window::Settings& settings);

	~Window();

	Window(const Window& other) = delete;
	Window& operator=(const Window& other) = delete;
	Window(Window&& other) noexcept;
	Window& operator=(Window&& other) noexcept;

	bool IsRunning() const;
	void Update() const;
	void StartFrame() const;
	void Render() const;

	GLFWwindow* GetGlfwWindow() const;
	ImGuiContext* GetImGuiContext() const;

	const std::string& GetTitle() const;
	uint32_t GetFramebufferWidth() const;
	uint32_t GetFramebufferHeight() const;
	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	bool IsVSync() const;
	const ImVec4& GetBackgroundColor() const;

	void SetTitle(const std::string& title);
	void SetTitle(std::string&& title);
	void SetWidth(const uint32_t width);
	void SetHeight(const uint32_t height);
	void Resize(const uint32_t width, const uint32_t height);
	void SetVSync(const bool enabled);
	void SetBackgroundColor(const ImVec4& backgroundColor);

private:
	void Init(const Window::Settings& settings);
	void InitGlfwWindow(const Window::Settings& settings);
	void InitImGuiContext(const Window::Settings& settings);

	void Destroy();
	void DestroyGlfwWindow();
	void DestroyImGuiContext();

private:
	struct Data
	{
		std::string Title;
		uint32_t FramebufferWidth = 0;
		uint32_t FramebufferHeight = 0;
		uint32_t WindowWidth = 0;
		uint32_t WindowHeight = 0;
		float Scale = 1.f;
		bool VSync = false;
		ImVec4 BackgroundColor;
	};

	Window::Data m_Data;

	GLFWwindow* m_GlfwWindow;
	ImGuiContext* m_ImGuiContext;
};