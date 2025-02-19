#include "pch.h"
#include "Application/Window.h"

#include "Utils/FileUtils.h"

namespace
{
	constexpr int OPENGL_VERSION_MAJOR = 4;
	constexpr int OPENGL_VERSION_MINOR = 6;
	constexpr int OPENGL_PROFILE = GLFW_OPENGL_CORE_PROFILE;
	constexpr const char* GLSL_VERSION = "#version 460 core";

	bool s_GlfwInitialized = false;

	bool InitGlfw()
	{
		if (s_GlfwInitialized)
			return true;

		glfwSetErrorCallback(
			[](const int errorCode, const char* const description)
			{
				fprintf(stderr, "[GLFW Error]\nId: %d\nDescription: %s\n", errorCode, description);
			}
		);

		s_GlfwInitialized = glfwInit() == GLFW_TRUE;
		return s_GlfwInitialized;
	}

	void ShutdownGlfw()
	{
		if (!s_GlfwInitialized)
			return;

		glfwTerminate();
		s_GlfwInitialized = false;
	}

	void ScaleImGui(const float scaleFactor)
	{
		ImGui::GetIO().FontGlobalScale *= scaleFactor;
		ImGui::GetStyle().ScaleAllSizes(scaleFactor);
	}
}

// Window::Settings
/*static*/ std::optional<Window::Settings> Window::Settings::LoadFromFile(const fs::path& filepath)
{
	const auto fileContents = utils::ReadFile(filepath);
	if (!fileContents)
	{
		LOG_ERROR("\"{}\" does not exist!", filepath.string());
		return {};
	}

	const auto hasInvalidFormat = [&filepath](const bool condition) -> bool
		{
			if (!condition)
			{
				LOG_ERROR("\"{}\" has invalid format!", filepath.string());
			}

			return !condition;
		};

	json::Document jsonDocument;
	jsonDocument.Parse(fileContents->c_str());
	if (hasInvalidFormat(
		jsonDocument.IsObject() &&
		jsonDocument.HasMember("title") &&
		jsonDocument.HasMember("size") &&
		jsonDocument.HasMember("vsync") &&
		jsonDocument.HasMember("background_color"))) return {};

	std::string title;
	{
		const auto& jsonTitle = jsonDocument["title"];
		if (hasInvalidFormat(jsonTitle.IsString())) return {};
		title = jsonTitle.GetString();
		if (hasInvalidFormat(!title.empty())) return {};
	}

	uint32_t width, height;
	{
		const auto& jsonSize = jsonDocument["size"];
		if (hasInvalidFormat(
			jsonSize.IsObject() &&
			jsonSize.HasMember("width") &&
			jsonSize.HasMember("height"))) return {};

		const auto& jsonWidth = jsonSize["width"];
		if (hasInvalidFormat(jsonWidth.IsUint())) return {};
		width = jsonWidth.GetUint();

		const auto& jsonHeight = jsonSize["height"];
		if (hasInvalidFormat(jsonHeight.IsUint())) return {};
		height = jsonHeight.GetUint();
	}

	bool vsync;
	{
		const auto& jsonVSync = jsonDocument["vsync"];
		if (hasInvalidFormat(jsonVSync.IsBool())) return {};
		vsync = jsonVSync.GetBool();
	}

	ImVec4 backroundColor;
	{
		const auto& jsonBackgroundColor = jsonDocument["background_color"];
		if (hasInvalidFormat(jsonBackgroundColor.IsArray() && jsonBackgroundColor.Size() == 4)) return {};

		if (hasInvalidFormat(jsonBackgroundColor[0].IsDouble())) return {};
		backroundColor.x = jsonBackgroundColor[0].GetFloat();

		if (hasInvalidFormat(jsonBackgroundColor[1].IsDouble())) return {};
		backroundColor.y = jsonBackgroundColor[1].GetFloat();

		if (hasInvalidFormat(jsonBackgroundColor[2].IsDouble())) return {};
		backroundColor.z = jsonBackgroundColor[2].GetFloat();

		if (hasInvalidFormat(jsonBackgroundColor[3].IsDouble())) return {};
		backroundColor.w = jsonBackgroundColor[3].GetFloat();
	}

	fs::path fontPath;
	float fontSize = 0.f;
	if (jsonDocument.HasMember("font")) // Optional
	{
		const auto& jsonFont = jsonDocument["font"];
		if (hasInvalidFormat(
			jsonFont.IsObject() &&
			jsonFont.HasMember("path") &&
			jsonFont.HasMember("size"))) return {};

		const auto& jsonFontPath = jsonFont["path"];
		if (hasInvalidFormat(jsonFontPath.IsString())) return {};
		fontPath = jsonFontPath.GetString();
		if (hasInvalidFormat(!fontPath.empty())) return {};

		const auto& jsonFontSize = jsonFont["size"];
		if (hasInvalidFormat(jsonFontSize.IsDouble())) return {};
		fontSize = jsonFontSize.GetFloat();
	}

	return Window::Settings(std::move(title), width, height, vsync,
		backroundColor, std::move(fontPath), fontSize);
}

Window::Settings::Settings(const std::string& title, const uint32_t width, const uint32_t height, const bool vsync,
	const ImVec4& backgroundColor, const fs::path& fontPath /* = {} */, const float fontSize /* = 0.f */)
	: Title(title)
	, Width(width)
	, Height(height)
	, VSync(vsync)
	, BackgroundColor(backgroundColor)
	, FontPath(fontPath)
	, FontSize(fontSize)
{
}

Window::Settings::Settings(std::string&& title, const uint32_t width, const uint32_t height, const bool vsync,
	const ImVec4& backgroundColor, fs::path&& fontPath /* = {} */, const float fontSize /* = 0.f */)
	: Title(std::move(title))
	, Width(width)
	, Height(height)
	, VSync(vsync)
	, BackgroundColor(backgroundColor)
	, FontPath(std::move(fontPath))
	, FontSize(fontSize)
{
}

// Window
Window::Window(const Window::Settings& settings)
{
	Init(settings);
}

Window::~Window()
{
	Destroy();
}

Window::Window(Window&& other) noexcept
	: m_Data(std::move(other.m_Data))
	, m_GlfwWindow(std::exchange(other.m_GlfwWindow, nullptr))
	, m_ImGuiContext(std::exchange(other.m_ImGuiContext, nullptr))
{
}

Window& Window::operator=(Window&& other) noexcept
{
	if (this == &other) return *this;

	Destroy();

	m_Data = std::move(other.m_Data);
	m_GlfwWindow = std::exchange(other.m_GlfwWindow, nullptr);
	m_ImGuiContext = std::exchange(other.m_ImGuiContext, nullptr);

	return *this;
}

void Window::Init(const Window::Settings& settings)
{
	InitGlfwWindow(settings);
	ASSERT(m_GlfwWindow);

	InitImGuiContext(settings);
	ASSERT(m_ImGuiContext);

	SetBackgroundColor(settings.BackgroundColor);
}

void Window::InitGlfwWindow(const Window::Settings& settings)
{
	ASSERT(InitGlfw());

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);
	glfwWindowHint(GLFW_OPENGL_PROFILE, OPENGL_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
	glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_TRUE);

	m_GlfwWindow = glfwCreateWindow(settings.Width, settings.Height, settings.Title.c_str(), nullptr, nullptr);
	ASSERT(m_GlfwWindow);

	m_Data.Title = settings.Title;

	glfwGetFramebufferSize(m_GlfwWindow,
		reinterpret_cast<int*>(&m_Data.FramebufferWidth),
		reinterpret_cast<int*>(&m_Data.FramebufferHeight)
	);

	glfwGetWindowSize(m_GlfwWindow,
		reinterpret_cast<int*>(&m_Data.WindowWidth),
		reinterpret_cast<int*>(&m_Data.WindowHeight)
	);

	glfwGetWindowContentScale(m_GlfwWindow,
		&m_Data.Scale,
		&m_Data.Scale
	);

	glfwMakeContextCurrent(m_GlfwWindow);
	SetVSync(settings.VSync);

	glfwSetWindowUserPointer(m_GlfwWindow, &m_Data);

	glfwSetFramebufferSizeCallback(m_GlfwWindow,
		[](GLFWwindow* const window, const int width, const int height)
		{
			auto* const data = reinterpret_cast<Window::Data*>(glfwGetWindowUserPointer(window));
			ASSERT(data);

			data->FramebufferWidth = width;
			data->FramebufferHeight = height;

			glViewport(0, 0, width, height);
		}
	);

	glfwSetWindowSizeCallback(m_GlfwWindow,
		[](GLFWwindow* const window, const int width, const int height)
		{
			auto* const data = reinterpret_cast<Window::Data*>(glfwGetWindowUserPointer(window));
			ASSERT(data);

			data->WindowWidth = width;
			data->WindowHeight = height;
		}
	);

	glfwSetWindowContentScaleCallback(m_GlfwWindow,
		[](GLFWwindow* const window, const float scaleX, const float scaleY)
		{
			auto* const data = reinterpret_cast<Window::Data*>(glfwGetWindowUserPointer(window));
			ASSERT(data);

			const float oldScale = data->Scale;
			const float newScale = scaleY;
			const float scaleFactor = newScale / oldScale;

			data->Scale = newScale;
			ScaleImGui(scaleFactor);
		}
	);
}

void Window::InitImGuiContext(const Window::Settings& settings)
{
	ASSERT(m_GlfwWindow);
	ASSERT(IMGUI_CHECKVERSION());

	m_ImGuiContext = ImGui::CreateContext();
	ImGui::StyleColorsDark();

	auto& io = ImGui::GetIO();
	io.IniFilename = nullptr;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	if (!settings.FontPath.empty())
	{
		io.Fonts->AddFontFromFileTTF(settings.FontPath.string().c_str(), settings.FontSize * m_Data.Scale);
		io.FontGlobalScale /= m_Data.Scale;
	}

	ScaleImGui(m_Data.Scale);

	ImGui_ImplGlfw_InitForOpenGL(m_GlfwWindow, true);
	ImGui_ImplOpenGL3_Init(GLSL_VERSION);
}

void Window::Destroy()
{
	if (m_ImGuiContext)
	{
		DestroyImGuiContext();
		m_ImGuiContext = nullptr;
	}

	if (m_GlfwWindow)
	{
		DestroyGlfwWindow();
		m_GlfwWindow = nullptr;
	}
}

void Window::DestroyGlfwWindow()
{
	ASSERT(m_GlfwWindow);

	glfwDestroyWindow(m_GlfwWindow);
	ShutdownGlfw();
}

void Window::DestroyImGuiContext()
{
	ASSERT(m_ImGuiContext);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

bool Window::IsRunning() const
{
	return !glfwWindowShouldClose(m_GlfwWindow);
}

void Window::Update() const
{
	glfwPollEvents();
}

void Window::StartFrame() const
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Window::Render() const
{
	ImGui::Render();
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(m_GlfwWindow);
}

GLFWwindow* Window::GetGlfwWindow() const
{
	return m_GlfwWindow;
}

ImGuiContext* Window::GetImGuiContext() const
{
	return m_ImGuiContext;
}

const std::string& Window::GetTitle() const
{
	return m_Data.Title;
}

uint32_t Window::GetWidth() const
{
	return m_Data.WindowWidth;
}

uint32_t Window::GetHeight() const
{
	return m_Data.WindowHeight;
}

uint32_t Window::GetFramebufferWidth() const
{
	return m_Data.FramebufferWidth;
}

uint32_t Window::GetFramebufferHeight() const
{
	return m_Data.FramebufferHeight;
}

bool Window::IsVSync() const
{
	return m_Data.VSync;
}

const ImVec4& Window::GetBackgroundColor() const
{
	return m_Data.BackgroundColor;
}

void Window::SetTitle(const std::string& title)
{
	glfwSetWindowTitle(m_GlfwWindow, title.c_str());
	m_Data.Title = title;
}

void Window::SetTitle(std::string&& title)
{
	glfwSetWindowTitle(m_GlfwWindow, title.c_str());
	m_Data.Title = std::move(title);
}

void Window::SetWidth(const uint32_t width)
{
	Resize(width, m_Data.WindowHeight);
}

void Window::SetHeight(const uint32_t height)
{
	Resize(m_Data.WindowWidth, height);
}

void Window::Resize(const uint32_t width, const uint32_t height)
{
	glfwSetWindowSize(m_GlfwWindow, width, height);
	// Callbacks automatically update Width, Height, FramebufferWidth and FramebufferHeight if needed
}

void Window::SetVSync(const bool enabled)
{
	glfwSwapInterval(enabled ? 1 : 0);
	m_Data.VSync = enabled;
}

void Window::SetBackgroundColor(const ImVec4& backgroundColor)
{
	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
	m_Data.BackgroundColor = backgroundColor;
}