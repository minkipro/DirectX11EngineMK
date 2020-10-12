#pragma once
#include "WindowContainer.h"
#include "Timer.h"

class Engine : public WindowContainer
{
public:
	bool Initialize(HINSTANCE hInstance, string window_title, string window_class, int width, int height);
	bool ProcessMessage();

	void Update();
	void RenderFrame();

private:
	Timer _timer;
};