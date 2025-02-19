#pragma once

#include "Math/Vector3.h"

class Window;
class Mesh;
struct Notification;

class Application
{
public:
	static void Start(const int argc, const char* const* const argv);

public:
	Application();

	void Run();

private:
	void Init();

	void DisplayMainMenuBar();
	void HandleShortcuts();

	void DisplayMeshDataSection();
	void DisplaySubdivideMeshSection();
	void DisplayIsPointInsideMeshSection();

	void DisplayNoMeshLoadedScreen();

	void DisplayNotifications() const;
	void AddNotification(Notification&& notification);

	void AssignMesh(Mesh&& mesh);

	void OpenMeshFile();
	void SaveMeshToFile();

private:
	std::unique_ptr<Window> m_Window;
	std::unique_ptr<Mesh> m_Mesh;

	std::vector<Notification> m_Notifications;

	std::string m_VerticesText;
	std::string m_TrianglesText;
	std::string m_SmoothVertexNormalsText;

	bool m_IsCheckButtonClicked;
	bool m_IsPointInsideMesh;
	Vector3f m_Point;
};