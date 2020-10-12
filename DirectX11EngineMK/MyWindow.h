#pragma once

#include "ErrorLogger.h"
class WindowContainer;
class MyWindow
{
public:
	~MyWindow();
	bool Initialize(WindowContainer* windowContainer, HINSTANCE hInstance, string window_title, string window_class, int width, int height);
	bool ProcessMessage();
	HWND GetHWND() const;

private:
	HWND _hwnd = NULL;
	HINSTANCE _hInstance = NULL;

	string _windowTitle = "";
	string _windowClassName = "";
	
	wstring _wWindowTitle = L"";
	wstring _wWindowClassName = L"";

	int _width = 0;
	int _height = 0;
};