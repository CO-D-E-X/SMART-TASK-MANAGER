cmake_minimum_required(VERSION 3.20)
project(SmartToDoList VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable compiler warnings
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include(FetchContent)

# 1. Fetch GLFW
message(STATUS "Fetching GLFW...")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE
)

# 2. Fetch Dear ImGui
message(STATUS "Fetching Dear ImGui...")
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.8
    GIT_SHALLOW TRUE
)

# 3. Fetch nlohmann/json for reliable modern C++ JSON persistence
message(STATUS "Fetching nlohmann/json...")
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glfw imgui nlohmann_json)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Build ImGui static library with GLFW and OpenGL3 backends
add_library(imgui_lib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui_lib PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui_lib PUBLIC
    glfw
    OpenGL::GL
)

# Core Application Sources
set(APP_SOURCES
    src/Model/Task.cpp
    src/Model/TaskManager.cpp
    src/Model/Persistence.cpp
    src/View/Theme.cpp
    src/View/TaskTableView.cpp
    src/View/ControlPanelView.cpp
    src/View/HistoryTableView.cpp
    src/View/ModalsView.cpp
    src/View/StatusBarView.cpp
    src/Controller/AppController.cpp
)

# Main Executable
add_executable(smart_todo
    src/main.cpp
    ${APP_SOURCES}
)

target_include_directories(smart_todo PRIVATE
    include
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(smart_todo PRIVATE
    imgui_lib
    glfw
    OpenGL::GL
    nlohmann_json::nlohmann_json
)

# Test Suite Executable (Verifies Model, Urgency Score, Comparator, and Persistence)
add_executable(test_domain
    tests/test_domain.cpp
    src/Model/Task.cpp
    src/Model/TaskManager.cpp
    src/Model/Persistence.cpp
)

target_include_directories(test_domain PRIVATE
    include
)

target_link_libraries(test_domain PRIVATE
    nlohmann_json::nlohmann_json
)
