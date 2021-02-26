#pragma once


class ImGuiUtil
{
public:
	static void CreateContext();
	static void DestroyContext();

	static void BeginNewFrame();
	static void EndFrame();
};