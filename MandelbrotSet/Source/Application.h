#pragma once

#include "Core.h"
#include "ImGuiUtil.h"
#include "Shader.h"


static inline constexpr u32 maxZoomLevel = 0;
static inline constexpr u32 minZoomLevel = 200;

static inline constexpr float zoomSpeed = 1.0f;
static inline constexpr float movementSpeed = 0.5f;

struct GLFWwindow;

class Application
{
public:
	Application();
	~Application();

	void Run();

	void OnMouseScrolled(float xOffset, float yOffset);
	void OnMouseMoved(float xOffset, float yOffset);
	void OnResize(u32 width, u32 height);

	dvec2 GetMousePosition();
	dvec2 GetMainViewportSize();

	static Application *Instance();

private:
	static void RenderFullscreenQuad();

private:
	static Application *s_Instance;

private:
	GLFWwindow *m_Window;
	Shader m_MandelbrotShader;

	float m_LastFrame = 0.0f;
	dvec2 m_LastMousePosition = { 0.0, 0.0 };

	float m_ZoomLevel = 200.0f;
	dvec2 m_CameraPosition = { 0.0, 0.0 };
	int m_MaxIterations = 200;
	vec4 m_Color = { 0.5f, 1.0f, 0.7f };

	bool m_BlockMouseEvents = false;

	friend class ImGuiUtil;
};