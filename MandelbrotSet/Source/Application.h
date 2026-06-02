#pragma once

#include "Core.h"
#include "ImGuiUtil.h"
#include "Shader.h"


// fp32 precision floor: pixel spacing 1/Z must stay above the smallest
// representable delta at |c| ~ 2, i.e. FLT_EPSILON * 2 ~= 2.4e-7. That gives
// Z <= ~4.2e6 as the strict ceiling; we sit slightly past it (mirroring the
// original `dvec2` value's 1.5x stretch over the fp64 floor) and accept some
// visible pixelation at maximum zoom. Switch to perturbation theory for
// deeper zoom — see README.
static const double MaxZoomLevel = 5.0e6;
static const double MinZoomLevel = 100;

static const double ZoomSpeed = 1.0f;
static const double MovementSpeed = 0.5f;

struct GLFWwindow;

class Application
{
public:
	Application();
	~Application();

	Application(const Application &) = delete;
	Application &operator=(const Application &) = delete;

	void Run();

private:
	void OnMouseScrolled(double xOffset, double yOffset);
	void OnMouseMoved(double xPosition, double yPosition);
	void OnResize(u32 width, u32 height);

	void ProcessKeyboardInput(double dt);

	dvec2 GetMousePosition();
	dvec2 GetMainViewportSize();   // logical points (for ImGui)
	dvec2 GetFramebufferSize();    // physical pixels (for GL / gl_FragCoord)

	void RenderFullscreenQuad();

	void TakeScreenShot();

private:
	static Application *s_Instance;
	static Application *Instance();

private:
	GLFWwindow *m_Window;
	Shader m_MandelbrotShader;
	Shader m_JuliaSetShader;

	dvec2 m_LastMousePosition = { 0.0, 0.0 };
	bool m_HasLastMousePosition = false;
	bool m_BlockMouseEvents = false;

	int m_MaxIterations = 100;
	// Initial zoom is in framebuffer pixels per world unit. 400 produces a
	// "fits the window" view at the default 1280x720 logical size on both
	// 1x and 2x DPI displays (since u_ScreenSize is now framebuffer-pixel
	// based, raising it from 200 keeps Retina users from getting a
	// half-zoomed default).
	double m_ZoomLevel = 400.0;
	dvec2 m_CameraPosition = { 0.0, 0.0 };
	// Alpha is unused by the shader but kept = 1 so the uniform value is
	// always sane if any future shader does sample u_Color.w.
	vec4 m_Color = { 0.5f, 1.0f, 0.7f, 1.0f };
	float m_RealComponent = 0.0f;
	float m_ImaginaryComponent = 0.0f;

	int currentItem = 0;
	const char *items = "Mandelbrot set\0Julia set";

	// Fullscreen quad GL state (created lazily on first draw, deleted in dtor).
	u32 m_QuadVAO = 0;
	u32 m_QuadVBO = 0;
	u32 m_QuadEBO = 0;

private:
	friend class ImGuiUtil;
};