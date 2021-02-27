#pragma once

#include "Core.h"
#include "ImGuiUtil.h"
#include "Shader.h"


static const double MaxZoomLevel = 3.25e15;
static const double MinZoomLevel = 200;

static const double ZoomSpeed = 1.0f;
static const double MovementSpeed = 0.5f;

struct GLFWwindow;
int main(int argc, char **argv);

class Application
{
private:
	Application();
	~Application();

	void Run();

	void OnMouseScrolled(double xOffset, double yOffset);
	void OnMouseMoved(double xPosition, double yPosition);
	void OnResize(u32 width, u32 height);

	dvec2 GetMousePosition();
	dvec2 GetMainViewportSize();

	static void RenderFullscreenQuad();

	void TakeScreenShot(const std::string &filepath);

private:
	static Application *s_Instance;
	static Application *Instance();

private:
	GLFWwindow *m_Window;
	Shader m_MandelbrotShader;

	dvec2 m_LastMousePosition = { 0.0, 0.0 };
	bool m_BlockMouseEvents = false;

	int m_MaxIterations = 100;
	double m_ZoomLevel = 200.0f;
	dvec2 m_CameraPosition = { 0.0, 0.0 };
	vec4 m_Color = { 0.5f, 1.0f, 0.7f };

private:
	friend class ImGuiUtil;
	friend int ::main(int argc, char **argv);
};