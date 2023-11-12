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

        //style setup
        ImGui::StyleColorsDark();
        //can be classic

        //if viewports enabled tweak window rounding
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

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




} // namespace Rocket