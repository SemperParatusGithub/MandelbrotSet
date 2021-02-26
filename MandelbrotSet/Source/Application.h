#pragma once

#include "ImGuiUtil.h"
#include "Core.h"


struct GLFWwindow;

class Application
{
public:
	Application();
	~Application();

	void Run();

	void OnMouseScrolled(float xOffset, float yOffset);
	void OnResize(u32 width, u32 height);

	vec2 GetMousePosition();
	vec2 GetMainViewportSize();

	static Application *Instance();

private:
	static Application *s_Instance;

private:
	float m_LastFrame = 0.0f;
	vec2 m_LastMousePosition = { 0.0f, 0.0f };

	GLFWwindow *m_Window;

	friend class ImGuiUtil;
};