#include "rcktpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"

//#define IMGUI_IMPL_API
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

#include "Rocket/Core/Application.h"

//temporary
#include <GLFW/glfw3.h>
#include <glad/glad.h>

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
        colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

        // headers
        colors[ImGuiCol_Header] = ImVec4{ 0.21f, 0.235f, 0.23f, 1.0f };
        colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.35f, 0.13f, 1.0f };
        colors[ImGuiCol_HeaderActive] = ImVec4{ 0.1f, 0.125f, 0.34f, 1.0f };

        // buttons
        colors[ImGuiCol_Button] = ImVec4{ 0.3f, 0.135f, 0.53f, 1.0f };
        colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.135f, 0.23f, 1.0f };
        colors[ImGuiCol_ButtonActive] = ImVec4{ 0.2f, 0.335f, 0.33f, 1.0f };

        // frame BG
        colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.235f, 0.13f, 1.0f };
        colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.2f, 0.335f, 0.33f, 1.0f };
        colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.23f, 0.235f, 0.23f, 1.0f };

        // tabs
        colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.0505f, 0.151f, 1.0f };
        colors[ImGuiCol_TabHovered] = ImVec4{ 0.25f, 0.1305f, 0.21f, 1.0f };
        colors[ImGuiCol_TabActive] = ImVec4{ 0.25f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1585f, 0.251f, 1.0f };
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.35f, 0.1995f, 0.151f, 1.0f };

        // title
        colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.2505f, 0.251f, 1.0f };
        colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.18f, 0.2595f, 0.231f, 1.0f };
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.17f, 0.2205f, 0.291f, 1.0f };

    }




} // namespace Rocket