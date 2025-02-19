#include "pch.h"
#include "Application/Application.h"

#include "Application/Notification.h"
#include "Application/Window.h"
#include "Core/Mesh.h"
#include "Utils/FileUtils.h"

namespace
{
	constexpr const char* WINDOW_SETTINGS_PATH = R"(config\window_settings.json)";

	constexpr uint32_t TEXT_BOX_VISIBLE_ENTRIES = 4;
	constexpr uint32_t MAIN_WINDOW_HEIGHT_MULTIPLIER = TEXT_BOX_VISIBLE_ENTRIES + 10;

	constexpr ImVec4 COLOR_RED = { 1.f, 0.f, 0.f, 1.f };
	constexpr ImVec4 COLOR_GREEN = { 0.f, 1.f, 0.f, 1.f };
	constexpr ImVec4 COLOR_ORANGE = { 1.f, 0.65f, 0.f, 1.f };
	constexpr ImVec4 COLOR_YELLOW = { 1.0f, 1.0f, 0.0f, 1.0f };

	constexpr const char* OPEN_FILE_DIALOG_NAME = "Open";
	constexpr const char* OPEN_FILE_DIALOG_DEFAULT_PATH = R"(res\meshes)";

	constexpr const char* SAVE_AS_FILE_DIALOG_NAME = "Save As";
	constexpr const char* SAVE_AS_FILE_DIALOG_DEFAULT_PATH = R"(res\meshes\mesh.json)";

	const std::vector<std::string> FILE_DIALOG_FILTERS = { "JSON (*.json)", "*.json" };

	void WriteBool(const char* const name, const bool value)
	{
		ImGui::Text("%s:", name);
		ImGui::SameLine();

		if (value)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, COLOR_GREEN);
			ImGui::TextUnformatted("true");
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, COLOR_RED);
			ImGui::TextUnformatted("false");
		}

		ImGui::PopStyleColor();
	}

	void WriteUint(const char* const name, const uint32_t value)
	{
		ImGui::Text("%s:", name);
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Text, COLOR_YELLOW);
		ImGui::Text("%u", value);
		ImGui::PopStyleColor();
	}

	void WriteFloat(const char* const name, const float value)
	{
		ImGui::Text("%s:", name);
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Text, COLOR_YELLOW);
		ImGui::Text("%f", value);
		ImGui::PopStyleColor();
	}

	void AddSeparator()
	{
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
	}

	std::string ToString(const Vector3f& vector)
	{
		return std::format("({}, {}, {})", vector.x, vector.y, vector.z);
	}
}

/*static*/ void Application::Start(const int argc, const char* const* const argv)
{
	Application app;
	app.Run();
}

Application::Application()
	: m_Mesh(nullptr)
	, m_Window(nullptr)
	, m_IsCheckButtonClicked(false)
	, m_IsPointInsideMesh(false)
{
	Init();
}

void Application::Init()
{
	const auto windowSettings = Window::Settings::LoadFromFile(WINDOW_SETTINGS_PATH);
	ASSERT(windowSettings);

	LOG_INFO("Successfully loaded window settings from: \"{}\"", WINDOW_SETTINGS_PATH);
	m_Window = std::make_unique<Window>(*windowSettings);
}

void Application::Run()
{
	ASSERT(m_Window);

	while (m_Window->IsRunning())
	{
		const float framebufferWidth = static_cast<float>(m_Window->GetFramebufferWidth());
		const float framebufferHeight = static_cast<float>(m_Window->GetFramebufferHeight());

		const float mainMenuBarHeight = ImGui::GetFrameHeight();
		const float frameHeightWithSpacing = ImGui::GetFrameHeightWithSpacing();

		const ImVec2 mainWindowPos = { 0.f, mainMenuBarHeight };
		const ImVec2 mainWindowSize = { framebufferWidth, frameHeightWithSpacing * MAIN_WINDOW_HEIGHT_MULTIPLIER };
		const ImVec2 notificationsWindowPos = { 0.f, mainWindowPos.y + mainWindowSize.y };
		const ImVec2 notificationsWindowSize = { framebufferWidth, framebufferHeight - mainMenuBarHeight - mainWindowSize.y };

		m_Window->Update();
		m_Window->StartFrame();

		DisplayMainMenuBar();
		HandleShortcuts();

		{
			ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.f, 0.f, 0.f, 0.f });
			ImGui::SetNextWindowPos(mainWindowPos);
			ImGui::SetNextWindowSize(mainWindowSize);
			ImGui::Begin("Main", nullptr,
				ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar
			);

			if (m_Mesh)
			{
				DisplayMeshDataSection();
				AddSeparator();
				DisplaySubdivideMeshSection();
				AddSeparator();
				DisplayIsPointInsideMeshSection();
			}
			else
			{
				DisplayNoMeshLoadedScreen();
			}

			ImGui::End();
			ImGui::PopStyleColor();
		}

		{
			ImGui::SetNextWindowPos(notificationsWindowPos);
			ImGui::SetNextWindowSize(notificationsWindowSize);
			ImGui::Begin("Notifications", nullptr,
				ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar
			);

			DisplayNotifications();

			ImGui::End();
		}

		m_Window->Render();
	}
}

void Application::DisplayMainMenuBar()
{
	ASSERT(ImGui::BeginMainMenuBar());

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Open...", "Ctrl+O"))
			OpenMeshFile();

		if (m_Mesh)
		{
			if (ImGui::MenuItem("Save As...", "Ctrl+S"))
				SaveMeshToFile();
		}

		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();
}

void Application::HandleShortcuts()
{
	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_O, ImGuiInputFlags_RouteGlobal))
		OpenMeshFile();

	if (m_Mesh)
	{
		if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S, ImGuiInputFlags_RouteGlobal))
			SaveMeshToFile();
	}
}

void Application::DisplayMeshDataSection()
{
	ASSERT(m_Mesh);

	const auto& style = ImGui::GetStyle();
	const float windowWidth = ImGui::GetWindowSize().x;
	const float itemSpacingWidth = style.ItemSpacing.x;
	const float windowPaddingWidth = style.WindowPadding.x;
	
	const float textboxWidth = (windowWidth - 2.f * itemSpacingWidth - windowPaddingWidth) / 3.f;

	{
		const auto vertexCount = static_cast<uint32_t>(m_Mesh->GetVertices().size());
		const auto trianglesCount = static_cast<uint32_t>(m_Mesh->GetTriangles().size());
		const auto smoothVertexNormalsCount = static_cast<uint32_t>(m_Mesh->GetSmoothVertexNormals().size());

		WriteUint("Vertices", vertexCount);
		ImGui::SameLine();
		ImGui::SetCursorPosX(windowPaddingWidth + 1.f * (textboxWidth + itemSpacingWidth));
		WriteUint("Triangles", trianglesCount);
		ImGui::SameLine();
		ImGui::SetCursorPosX(windowPaddingWidth + 2.f * (textboxWidth + itemSpacingWidth));
		WriteUint("Smooth vertex normals", smoothVertexNormalsCount);
	}

	{
		const float textboxHeight = 2.f * style.FramePadding.y + ImGui::GetTextLineHeight() * TEXT_BOX_VISIBLE_ENTRIES;
		const ImVec2 textboxSize = { textboxWidth, textboxHeight };

		ImGui::InputTextMultiline("##Vertices", m_VerticesText.data(), m_VerticesText.size() + 1, textboxSize, ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		ImGui::InputTextMultiline("##Triangles", m_TrianglesText.data(), m_TrianglesText.size() + 1, textboxSize, ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		ImGui::InputTextMultiline("##SmoothVertexNormals", m_SmoothVertexNormalsText.data(), m_SmoothVertexNormalsText.size() + 1, textboxSize, ImGuiInputTextFlags_ReadOnly);
	}

	AddSeparator();

	{
		const auto& statistics = m_Mesh->GetStatistics();
		const uint32_t edgeCount = m_Mesh->GetEdgeCount();
		const bool isClosed = m_Mesh->IsClosed();

		WriteFloat("Smallest triangle area", statistics.SmallestTriangleArea);
		WriteFloat("Biggest triangle area", statistics.BiggestTriangleArea);
		WriteFloat("Average triangle area", statistics.AverageTriangleArea);
		WriteUint("Edge count", edgeCount);
		WriteBool("Is closed", isClosed);
	}
}

void Application::DisplaySubdivideMeshSection()
{
	ImGui::TextUnformatted("Generate a new mesh by subdividing each triangle into 4 smaller ones:");
	ImGui::SameLine();
	if (ImGui::Button("Generate Mesh"))
	{
		AssignMesh(m_Mesh->GenerateSubdividedMesh());
		AddNotification(Notification::Info("Generated new subdivided mesh"));
	}
}

void Application::DisplayIsPointInsideMeshSection()
{
	ImGui::TextUnformatted("Point:");
	ImGui::SameLine();
	ImGui::InputFloat3("##Point", &m_Point.x);

	ImGui::TextUnformatted("Check if point is inside mesh:");
	ImGui::SameLine();
	if (ImGui::Button("Check"))
	{
		m_IsCheckButtonClicked = true;
		m_IsPointInsideMesh = m_Mesh->IsPointInsideMesh(m_Point);
		AddNotification(Notification::Info(std::format("Checked if point {} is inside mesh", ToString(m_Point))));
	}

	if (m_IsCheckButtonClicked)
		WriteBool("Is point inside mesh", m_IsPointInsideMesh);
	else
		ImGui::TextUnformatted("Is point inside mesh:");
}

void Application::DisplayNoMeshLoadedScreen()
{
	static constexpr const char* OPEN_FILE_TEXT = "Open a mesh file to view its statistics:";
	static constexpr const char* OPEN_FILE_BUTTON_TEXT = "Open";

	const auto& style = ImGui::GetStyle();
	const auto windowSize = ImGui::GetWindowSize();
	const auto textSize = ImGui::CalcTextSize(OPEN_FILE_TEXT);
	const float buttonWidth = ImGui::CalcTextSize(OPEN_FILE_BUTTON_TEXT).x;
	const float extraWidth = 2.f * style.FramePadding.x + style.ItemSpacing.x;
	const float cursorPosX = 0.5f * (windowSize.x - textSize.x - buttonWidth - extraWidth);
	const float cursorPosY = 0.5f * (windowSize.y - textSize.y);

	ImGui::SetCursorPos({ cursorPosX, cursorPosY });
	ImGui::TextUnformatted(OPEN_FILE_TEXT);
	ImGui::SameLine();
	if (ImGui::Button(OPEN_FILE_BUTTON_TEXT))
		OpenMeshFile();
}

void Application::DisplayNotifications() const
{
	for (const auto& notification : m_Notifications)
	{
		switch (notification.Type)
		{
		case NotificationType::Info:
			ImGui::PushStyleColor(ImGuiCol_Text, COLOR_GREEN);
			break;
		case NotificationType::Warning:
			ImGui::PushStyleColor(ImGuiCol_Text, COLOR_ORANGE);
			break;
		case NotificationType::Error:
			ImGui::PushStyleColor(ImGuiCol_Text, COLOR_RED);
			break;
		}

		ImGui::TextUnformatted(notification.Message.c_str());
		ImGui::PopStyleColor();
	}
}

void Application::AddNotification(Notification&& notification)
{
	switch (notification.Type)
	{
	case NotificationType::Info:
		LOG_INFO("Notification: {}", notification.Message);
		break;
	case NotificationType::Warning:
		LOG_WARNING("Notification: {}", notification.Message);
		break;
	case NotificationType::Error:
		LOG_ERROR("Notification: {}", notification.Message);
		break;
	}

	m_Notifications.push_back(std::move(notification));
}

void Application::AssignMesh(Mesh&& mesh)
{
	static constexpr size_t STRING_INITIAL_CAPACITY_VECTOR3F = 35;
	static constexpr size_t STRING_INITIAL_CAPACITY_TRIANGLE = 25;

	m_Mesh = std::make_unique<Mesh>(std::move(mesh));

	const auto& vertices = m_Mesh->GetVertices();
	const auto& triangles = m_Mesh->GetTriangles();
	const auto& smoothVertexNormals = m_Mesh->GetSmoothVertexNormals();

	auto verticesTextFuture = std::async(std::launch::async,
		[&vertices]() -> std::string
		{
			std::string result;
			result.reserve(vertices.size() * STRING_INITIAL_CAPACITY_VECTOR3F);

			for (const auto& vertex : vertices)
			{
				result += ToString(vertex);
				result += '\n';
			}

			return result.erase(result.size() - 1); // Remove the last new line
		}
	);

	auto trianglesTextFuture = std::async(std::launch::async,
		[&triangles]() -> std::string
		{
			std::string result;
			result.reserve(triangles.size() * STRING_INITIAL_CAPACITY_TRIANGLE);

			for (const auto& triangle : triangles)
			{
				result += std::format("({}, {}, {})",
					triangle.VertexIndexes[0], triangle.VertexIndexes[1], triangle.VertexIndexes[2]);
				result += '\n';
			}

			return result.erase(result.size() - 1); // Remove the last new line
		}
	);

	auto smoothVertexNormalsTextFuture = std::async(std::launch::async,
		[&smoothVertexNormals]() -> std::string
		{
			std::string result;
			result.reserve(smoothVertexNormals.size() * STRING_INITIAL_CAPACITY_VECTOR3F);

			for (const auto& smoothVertexNormal : smoothVertexNormals)
			{
				result += ToString(smoothVertexNormal);
				result += '\n';
			}

			return result.erase(result.size() - 1); // Remove the last new line
		}
	);

	m_VerticesText = verticesTextFuture.get();
	m_TrianglesText = trianglesTextFuture.get();
	m_SmoothVertexNormalsText = smoothVertexNormalsTextFuture.get();
}

void Application::OpenMeshFile()
{
	const auto filepath = utils::OpenFileDialog(OPEN_FILE_DIALOG_NAME, OPEN_FILE_DIALOG_DEFAULT_PATH, FILE_DIALOG_FILTERS);
	if (!filepath) return;

	if (auto mesh = Mesh::LoadFromFile(*filepath))
	{
		AssignMesh(std::move(*mesh));
		AddNotification(Notification::Info(std::format("Successfully loaded mesh from: \"{}\"", filepath->string())));
	}
	else
	{
		AddNotification(Notification::Error(std::format("File does not exist or has incorrect format: \"{}\"", filepath->string())));
	}
}

void Application::SaveMeshToFile()
{
	ASSERT(m_Mesh);

	const auto filepath = utils::SaveAsFileDialog(SAVE_AS_FILE_DIALOG_NAME, SAVE_AS_FILE_DIALOG_DEFAULT_PATH, FILE_DIALOG_FILTERS);
	if (!filepath) return;

	if (Mesh::SaveToFile(*filepath, *m_Mesh))
		AddNotification(Notification::Info(std::format("Successfully saved mesh to: \"{}\"", filepath->string())));
	else
		AddNotification(Notification::Error(std::format("Could not save mesh to: \"{}\"", filepath->string())));
}