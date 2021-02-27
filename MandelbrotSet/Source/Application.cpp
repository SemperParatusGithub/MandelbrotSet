#include "Application.h"

#include <assert.h>

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
	m_Window = glfwCreateWindow(1280, 720, "Mandelbrot set", nullptr, nullptr);
	glfwMakeContextCurrent(m_Window);

    glfwSetScrollCallback(m_Window, [](GLFWwindow *window, double xOffset, double yOffset)
        { Instance()->OnMouseScrolled(xOffset, yOffset); });

    glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow *window, int sizeX, int sizeY)
        { Instance()->OnResize((u32) sizeX, (u32) sizeY); });

    glfwSetCursorPosCallback(m_Window, [](GLFWwindow *window, double xOffset, double yOffset)
        { Instance()->OnMouseMoved(xOffset, yOffset); });

	assert(gladLoadGLLoader((GLADloadproc) glfwGetProcAddress));
	glViewport(0, 0, 1280, 720);

    m_MandelbrotShader.Load("Shaders/Mandelbrot.glsl");

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

	    glClearColor(0.7f, 0.7f, 0.7f, 0.7f);
	    glClear(GL_COLOR_BUFFER_BIT);

        m_MandelbrotShader.Bind();
        m_MandelbrotShader.SetInt("u_MaxIterations", m_MaxIterations);
        m_MandelbrotShader.SetDouble2("u_ScreenSize", GetMainViewportSize());
        m_MandelbrotShader.SetDouble("u_Zoom", m_ZoomLevel);
        m_MandelbrotShader.SetDouble2("u_Offset", m_CameraPosition);
        m_MandelbrotShader.SetFloat4("u_Color", m_Color);
        RenderFullscreenQuad();

        m_BlockMouseEvents = ImGui::GetIO().WantCaptureMouse;

        ImGui::Begin("Hello World");
        ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
        ImGui::SliderInt("u_MaxIterations", &m_MaxIterations, 0, 500);
        ImGui::ColorEdit4("Color", &m_Color.x);
        ImGui::End();

        ImGuiUtil::EndFrame();
	}
}

void Application::OnMouseScrolled(double xOffset, double yOffset)
{
    if(!m_BlockMouseEvents)
        m_ZoomLevel += yOffset * ZoomSpeed * m_ZoomLevel / 10.0f;
  
    if (m_ZoomLevel < MinZoomLevel)
        m_ZoomLevel = MinZoomLevel;
    if (m_ZoomLevel > MaxZoomLevel)
        m_ZoomLevel = MaxZoomLevel;
}
void Application::OnMouseMoved(double xPosition, double yPosition)
{
    dvec2 offset = { m_LastMousePosition.x - xPosition,
                     yPosition - m_LastMousePosition.y };

    m_LastMousePosition = dvec2 { xPosition, yPosition };

    if (glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && 
        !m_BlockMouseEvents)
    {
        m_CameraPosition.x -= offset.x / m_ZoomLevel;
        m_CameraPosition.y -= offset.y / m_ZoomLevel;
    }
}
void Application::OnResize(u32 width, u32 height)
{
    glViewport(0, 0, width, height);
}

dvec2 Application::GetMousePosition()
{
    double xPos, yPos;
    glfwGetCursorPos(m_Window, &xPos, &yPos);

    return dvec2 { xPos, yPos };
}
dvec2 Application::GetMainViewportSize()
{
    int width, height;
    glfwGetWindowSize(m_Window, &width, &height);

    return dvec2 { (double) width, (double) height };
}

void Application::RenderFullscreenQuad()
{
    static u32 VertexArray = 0;
    static u32 VertexBuffer = 0, IndexBuffer = 0;

    // prepare pipeline
    if (VertexArray == 0)
    {
        constexpr float vertices[] = {
            -1.0f, -1.0f, 0.0f,
             1.0f, -1.0f, 0.0f,
             1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f
        };
        constexpr u32 indices[] = {
            0, 1, 2, 2, 3, 0
        };

        glCreateVertexArrays(1, &VertexArray);
        glBindVertexArray(VertexArray);

        glCreateBuffers(1, &VertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glCreateBuffers(1, &IndexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    }

    glBindVertexArray(VertexArray);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

Application *Application::Instance()
{
	assert(s_Instance);
	return s_Instance;
}