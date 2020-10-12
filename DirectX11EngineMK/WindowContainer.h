#pragma once
#include "MyWindow.h"
#include "Keyboard/Keyboard.h"
#include "Mouse/Mouse.h"
#include "Graphics/Graphics.h"
class WindowContainer
{
public:
	WindowContainer();
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	MyWindow _wnd;
	Keyboard _keyboard;
	Mouse _mouse;
	Graphics _gfx;
};