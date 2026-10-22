#include "Controller/AppController.hpp"
#include "View/Theme.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>

static void GlfwErrorCallback(int error, const char* description) {
    std::cerr << "[GLFW Error " << error << "]: " << description << std::endl;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    // 1. Setup GLFW Error Callback
    glfwSetErrorCallback(GlfwErrorCallback);

    // 2. Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return EXIT_FAILURE;
    }

    // Decide GL+GLSL versions
#if defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    // GL 3.3 Core + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    // 3. Create Window with Graphics Context
    const int initialWidth = 1200;
    const int initialHeight = 780;
    GLFWwindow* window = glfwCreateWindow(
        initialWidth,
        initialHeight,
        "Smart To-Do List - Dynamic Task Prioritization",
        nullptr,
        nullptr
    );

    if (!window) {
        // Fallback to default OpenGL profile if core 3.3 failed
        glfwDefaultWindowHints();
        window = glfwCreateWindow(
            initialWidth,
            initialHeight,
            "Smart To-Do List - Dynamic Task Prioritization",
            nullptr,
            nullptr
        );
        if (!window) {
            std::cerr << "Failed to create GLFW Window with OpenGL context." << std::endl;
            glfwTerminate();
            return EXIT_FAILURE;
        }
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable V-Sync

    // 4. Initialize Dear ImGui Context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Apply custom modern dark aesthetic theme
    ToDo::View::ApplyModernDarkTheme();

    // 5. Setup Platform/Renderer Backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // 6. Initialize App Controller
    ToDo::AppController appController("tasks.json");
    appController.Initialize();

    const ImVec4 clearColor = ImVec4(0.09f, 0.10f, 0.13f, 1.00f);

    // 7. Main Event Loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render entire application UI
        appController.RenderUI();

        // Rendering pass
        ImGui::Render();
        int displayW = 0;
        int displayH = 0;
        glfwGetFramebufferSize(window, &displayW, &displayH);
        glViewport(0, 0, displayW, displayH);
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // 8. Clean Shutdown
    appController.Shutdown();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
