#include "rcktpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"

//#define IMGUI_IMPL_API
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

#include "Rocket/Core/Application.h"

//temporary
#include <GLFW/glfw3.h>
#include <glad.h>

namespace Rocket {

	ImGuiLayer::ImGuiLayer() :Layer("ImGui Layer") {

	}

	ImGuiLayer::~ImGuiLayer() {

	}

	void ImGuiLayer::onAttach() {
        RCKT_PROFILE_FUNCTION();

	    //Setup ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        //same can be done with gamepad
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        //can be done with viewport no taskbar icons
        //can be done for viewports with no merge

        io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/UbuntuCondensed-Regular.ttf", 18.0f);

        //style setup
        ImGui::StyleColorsDark();
        //can be classic

        //if viewports enabled tweak window rounding
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        setDarkThemeColors();

        Application& app = Application::get();
        GLFWwindow* window = static_cast<GLFWwindow*>(app.getWindow().getNativeWindow());

        //setup platform, renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 410");
    }

	void ImGuiLayer::onDetach() {
        RCKT_PROFILE_FUNCTION();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
	}

    void ImGuiLayer::onEvent(Event& event) {
        if (m_blockEvents) {
            ImGuiIO& io = ImGui::GetIO();
            event.handled |= event.isInCategory(EventCategoryMouse) & io.WantCaptureMouse;
            event.handled |= event.isInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
        }
    }

    void ImGuiLayer::onImGuiRender() {
        static bool show = true;
        //show window here
    }


    void ImGuiLayer::begin() {
        RCKT_PROFILE_FUNCTION();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiLayer::end() {
        RCKT_PROFILE_FUNCTION();

        ImGuiIO& io = ImGui::GetIO();
        Application& app = Application::get();
        io.DisplaySize = ImVec2((float)app.getWindow().getWidth(), (float)app.getWindow().getHeight());

        // rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void ImGuiLayer::setDarkThemeColors() {
        auto& colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_WindowBg] = ImVec4{ 0.11f, 0.105f, 0.12f, 1.0f };

        // headers
        colors[ImGuiCol_Header] = ImVec4{ 0.3f, 0.405f, 0.511f, 1.0f };
        colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.1f, 0.405f, 0.811f, 1.0f };
        colors[ImGuiCol_HeaderActive] = ImVec4{ 0.3f, 0.505f, 0.511f, 1.0f };

        // buttons
        colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.305f, 0.21f, 1.0f };
        colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.405f, 0.31f, 1.0f };
        colors[ImGuiCol_ButtonActive] = ImVec4{ 0.4f, 0.505f, 0.41f, 1.0f };

        // frame BG
        colors[ImGuiCol_FrameBg] = ImVec4{ 0.1f, 0.305f, 0.311f, 1.0f };
        colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.311f, 1.0f };
        colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.3f, 0.305f, 0.511f, 1.0f };

        // tabs
        colors[ImGuiCol_Tab] = ImVec4{ 0.4f, 0.105f, 0.451f, 1.0f };
        colors[ImGuiCol_TabHovered] = ImVec4{ 0.15f, 0.105f, 0.211f, 1.0f };
        colors[ImGuiCol_TabActive] = ImVec4{ 0.55f, 0.505f, 0.531f, 1.0f };
        colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.21f, 0.105f, 0.11f, 1.0f };
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.1f, 0.105f, 0.311f, 1.0f };

        // title
        colors[ImGuiCol_TitleBg] = ImVec4{ 0.45f, 0.3505f, 0.351f, 1.0f };
        colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.38f, 0.3595f, 0.431f, 1.0f };
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.217f, 0.2205f, 0.291f, 1.0f };

    }




} // namespace Rocket