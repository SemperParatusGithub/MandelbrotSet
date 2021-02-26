#include "Application.h"

#include <assert.h>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>


Application *Application::s_Instance = nullptr;

Application::Application()
{
	assert(s_Instance == nullptr);
	s_Instance = this;

	assert(glfwInit());
	LOG_INFO("Creating Window: 1280, 720");
    glfwWindowHint(GLFW_SAMPLES, 8);
	m_Window = glfwCreateWindow(1280, 720, "Window", nullptr, nullptr);
	glfwMakeContextCurrent(m_Window);

    glfwSetScrollCallback(m_Window, [](GLFWwindow *window, double xOffset, double yOffset)
        { Instance()->OnMouseScrolled((float) xOffset, (float) yOffset); });
    glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow *window, int sizeX, int sizeY)
        { Instance()->OnResize((u32) sizeX, (u32) sizeY); });

	assert(gladLoadGLLoader((GLADloadproc) glfwGetProcAddress));
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, 1280, 720);

    ImGuiUtil::CreateContext();
}

Application::~Application()
{
    ImGuiUtil::DestroyContext();
	glfwTerminate();
}

void Application::Run()
{
    while (!glfwWindowShouldClose(m_Window))
    {
        ImGuiUtil::BeginNewFrame();

        glfwSwapBuffers(m_Window);
        glfwPollEvents();

        vec2 mouseOffset = { 
            GetMousePosition().x - m_LastMousePosition.x, 
            GetMousePosition().y - m_LastMousePosition.y };
        float deltaTime = (float) glfwGetTime() - m_LastFrame;

        m_LastMousePosition = GetMousePosition();
        m_LastFrame = glfwGetTime();

	    glClearColor(0.7f, 0.7f, 0.7f, 0.7f);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui::Begin("Hello World");
        ImGui::Text("Test!");
        ImGui::End();

        ImGuiUtil::EndFrame();
	}
}

void Application::OnMouseScrolled(float xOffset, float yOffset)
{
}

void Application::OnResize(u32 width, u32 height)
{
    glViewport(0, 0, width, height);
}

vec2 Application::GetMousePosition()
{
    double xPos, yPos;
    glfwGetCursorPos(m_Window, &xPos, &yPos);

    return vec2 { (float) xPos, (float) yPos };
}

vec2 Application::GetMainViewportSize()
{
    int width, height;
    glfwGetWindowSize(m_Window, &width, &height);
    return vec2 { (float) width, (float) height };
}

Application *Application::Instance()
{
	assert(s_Instance);
	return s_Instance;
}