#pragma once

// C++ Libraries
#include <string>
#include <string_view>

#include <fstream>
#include <sstream>
#include <iomanip>

#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <cmath>
#include <cstdlib>
#include <ctime>

#include <algorithm>
#include <memory>
#include <optional>
#include <utility>

#include <future>
#include <thread>

#include <filesystem>

// GLFW
#include "GLFW/glfw3.h"

// Dear ImGui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

// Portable File Dialogs
#include "portable_file_dialogs/portable_file_dialogs.h"

// RapidJSON
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

// Mesh Stats Viewer
#include "Macros/Assert.h"
#include "Macros/Log.h"

// Namespaces
namespace fs = std::filesystem;
namespace json = rapidjson;