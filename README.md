# Mesh Stats Viewer
Software that allows the user to parse meshes from JSON format and view their different statistics.

## Installation
1. Download the repository.
2. Depending on your OS and IDE, run the corresponding script in the [scripts](https://github.com/Coopjmz/Mesh-Stats-Viewer/tree/main/scripts) folder.
3. Build and run the project.
4. If you decide to run the executable manually, you can find it in the binaries folder - `build\bin`. Copy the [config](https://github.com/Coopjmz/Mesh-Stats-Viewer/tree/main/Mesh%20Stats%20Viewer/config) and [res](https://github.com/Coopjmz/Mesh-Stats-Viewer/tree/main/Mesh%20Stats%20Viewer/res) folders and put them in the same directory as the executable.

## Usage
You can find some test meshes in JSON format here: [Mesh Stats Viewer\res\meshes](https://github.com/Coopjmz/Mesh-Stats-Viewer/tree/main/Mesh%20Stats%20Viewer/res/meshes)

You can change the window's settings by modifying [window_settings.json](https://github.com/Coopjmz/Mesh-Stats-Viewer/blob/main/Mesh%20Stats%20Viewer/config/window_settings.json)

## External Libraries
- [GLFW (v3.4)](https://github.com/glfw/glfw)
- [Dear ImGui (v1.91.8)](https://github.com/ocornut/imgui)
- [Portable File Dialogs (v0.1.0)](https://github.com/samhocevar/portable-file-dialogs)
- [RapidJSON (v1.1.0)](https://github.com/Tencent/rapidjson)

## Screenshots
- pyramid.json

![pyramid.png](https://github.com/Coopjmz/Mesh-Stats-Viewer/blob/main/screenshots/pyramid.png)

***
- teapot.json

![teapot.png](https://github.com/Coopjmz/Mesh-Stats-Viewer/blob/main/screenshots/teapot.png)

***
- lucy.json

![lucy.png](https://github.com/Coopjmz/Mesh-Stats-Viewer/blob/main/screenshots/lucy.png)

***
- lucy.json (subdivided 3 times)

![lucy_subdivided_x12.png](https://github.com/Coopjmz/Mesh-Stats-Viewer/blob/main/screenshots/lucy_subdivided_x12.png)