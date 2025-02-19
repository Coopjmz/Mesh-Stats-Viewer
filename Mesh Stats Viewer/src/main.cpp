#include "pch.h"

#include "Application/Application.h"

int main(int argc, char** argv)
{
    Application::Start(argc, argv);
    return EXIT_SUCCESS;
}

#if defined(_WIN64) && defined(RELEASE)
#include <Windows.h>
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    return main(__argc, __argv);
}
#endif