#pragma once

#include "Core.h"
#include "ImGuiUtil.h"
#include "Shader.h"


struct GLFWwindow;

class Application
{
public:
	Application();
	~Application();

	void Run();

	void OnMouseScrolled(float xOffset, float yOffset);
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
	u32 m_MaxIterations = 200;
	vec4 m_Color = { 0.5f, 1.0f, 0.7f };

	friend class ImGuiUtil;
};