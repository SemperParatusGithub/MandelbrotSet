#include "Application.h"

#include <assert.h>
#include <sstream>
#include <time.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <stb_image_write.h>


Application *Application::s_Instance = nullptr;

Application::Application()
{
	assert(s_Instance == nullptr);
	s_Instance = this;

	assert(glfwInit());

	// Request an OpenGL 3.3 Core Profile context. Forward-compat is
	// required on macOS, harmless on Windows/Linux.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

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
    m_JuliaSetShader.Load("Shaders/JuliaSet.glsl");

    ImGuiUtil::CreateContext();

#ifdef _WIN32
    HWND window = GetConsoleWindow();
#ifdef _DEBUG
    ShowWindow(window, 1);
#else
    ShowWindow(window, 0);
#endif
#endif
}

Application::~Application()
{
    ImGuiUtil::DestroyContext();
	glfwTerminate();
}

void Application::Run()
{
    glfwMaximizeWindow(m_Window);

    while (!glfwWindowShouldClose(m_Window))
    {
        ImGuiUtil::BeginNewFrame();

        glfwSwapBuffers(m_Window);
        glfwPollEvents();

        ProcessKeyboardInput(ImGui::GetIO().DeltaTime);

	    glClearColor(0.7f, 0.7f, 0.7f, 0.7f);
	    glClear(GL_COLOR_BUFFER_BIT);

        auto &shader = currentItem == 0 ? m_MandelbrotShader : m_JuliaSetShader;

        const dvec2 viewport = GetFramebufferSize();

        shader.Bind();
        shader.SetInt("u_MaxIterations", m_MaxIterations);
        shader.SetFloat2("u_ScreenSize", { (float) viewport.x, (float) viewport.y });
        shader.SetFloat ("u_Zoom",       (float) m_ZoomLevel);
        shader.SetFloat2("u_Offset",     { (float) m_CameraPosition.x, (float) m_CameraPosition.y });
        shader.SetFloat4("u_Color",      m_Color);
        if (currentItem == 1)   // Julia Set
        {
            shader.SetFloat("u_RealComponent", m_RealComponent);
            shader.SetFloat("u_ImaginaryComponent", m_ImaginaryComponent);
        }
        RenderFullscreenQuad();

        m_BlockMouseEvents = ImGui::GetIO().WantCaptureMouse;

        ImGui::Begin("Settings");
        ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);

        ImGui::SetNextItemWidth(-1.0f);
        ImGui::Combo("##Current Fractal", &currentItem, items);

        ImGui::Text("Max Iterations");
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::SliderInt("##maxIterations", &m_MaxIterations, 0, 500);

        ImGui::Text("Color");
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::ColorEdit4("##color", &m_Color.x);

        if (currentItem == 1)
        {
            ImGui::Spacing();
            ImGui::Text("RealComponent");
            ImGui::SetNextItemWidth(-1.0f);
            ImGui::SliderFloat("##RealComponent", &m_RealComponent, 0.0f, 1.0f);

            ImGui::Text("ImaginaryComponent");
            ImGui::SetNextItemWidth(-1.0f);
            ImGui::SliderFloat("##ImaginaryComponent", &m_ImaginaryComponent, 0.0f, 1.0f);
        }

        ImGui::Spacing();
        if (ImGui::Button("Take Screenshot"))
            TakeScreenShot();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextDisabled("WASD / drag : pan");
        ImGui::TextDisabled("Scroll      : zoom");
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

void Application::ProcessKeyboardInput(double dt)
{
    // Skip when an ImGui widget owns the keyboard (text input, slider in
    // keyboard-nav mode, etc.) so typing doesn't pan the view.
    if (ImGui::GetIO().WantCaptureKeyboard)
        return;

    // MovementSpeed is interpreted as "screens per second" of pan, so the
    // perceived speed stays constant at every zoom level.
    const dvec2 viewport = GetFramebufferSize();
    const double pan = MovementSpeed * dt / m_ZoomLevel;

    // m_CameraPosition is subtracted from the per-pixel world coord in the
    // shader, so to move the *view* in a direction we move m_CameraPosition
    // the opposite way.
    if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
        m_CameraPosition.y -= pan * viewport.y;
    if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
        m_CameraPosition.y += pan * viewport.y;
    if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
        m_CameraPosition.x += pan * viewport.x;
    if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
        m_CameraPosition.x -= pan * viewport.x;
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
dvec2 Application::GetFramebufferSize()
{
    // gl_FragCoord and glViewport operate in framebuffer pixels, so anything
    // touching the GL pipeline (u_ScreenSize, glReadPixels, pan-speed scaling)
    // must use this rather than glfwGetWindowSize -- on HiDPI displays they
    // differ by the DPI scale factor.
    int width, height;
    glfwGetFramebufferSize(m_Window, &width, &height);

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

        // glCreateBuffers / glCreateVertexArrays are DSA (GL 4.5+) and unavailable
        // on a 3.3 core context, so use the legacy bind-to-edit form.
        glGenVertexArrays(1, &VertexArray);
        glBindVertexArray(VertexArray);

        glGenBuffers(1, &VertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &IndexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    }

    glBindVertexArray(VertexArray);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Application::TakeScreenShot()
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    const dvec2 fb = GetFramebufferSize();
    std::size_t width  = (std::size_t) fb.x;
    std::size_t height = (std::size_t) fb.y;

    char *data = new char[width * height * 3];

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

    time_t theTime = time(nullptr);
    struct tm *aTime = localtime(&theTime);

    std::stringstream name;

    name << "Screenshot_";
    name << aTime->tm_year + 1900 << "-" << aTime->tm_mon + 1 << "-" << aTime->tm_mday << "_";
    name << aTime->tm_hour << "-" << aTime->tm_min << "-" << aTime->tm_sec;
    name << ".png";

    assert(stbi_write_png(name.str().c_str(), width, height, 3, data, 0));
    delete[] data;

    LOG_INFO("Saved Screenshot: %s", name.str().c_str());
}

Application *Application::Instance()
{
	assert(s_Instance);
	return s_Instance;
}