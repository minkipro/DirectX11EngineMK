#include "WindowContainer.h"

MyWindow::~MyWindow()
{
	if (_hwnd != NULL)
	{
		UnregisterClass(_wWindowClassName.c_str(), _hInstance);
		DestroyWindow(_hwnd);
	}
}

LRESULT CALLBACK HandleMsgRedirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;

	default:
	{
		WindowContainer* const pWindow = reinterpret_cast<WindowContainer*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
	}
	}
}

LRESULT CALLBACK HandleMessageSetup(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_NCCREATE:
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		WindowContainer* pWindow = reinterpret_cast<WindowContainer*>(pCreate->lpCreateParams);
		if (pWindow == nullptr)
		{
			ErrorLogger::Log("Critical Error: pointer to window container is null during WM_NCCREATE.");
			exit(-1);
		}
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMsgRedirect));
		
		return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

bool MyWindow::Initialize(WindowContainer* windowContainer, HINSTANCE hInstance, string windowTitle, string windowClassName, int width, int height)
{
	_hInstance = hInstance;
	_width = width;
	_height = height;
	_windowTitle = windowTitle;
	_wWindowTitle = StringHelper::StringToWide(_windowTitle);
	_windowClassName = windowClassName;
	_wWindowClassName = StringHelper::StringToWide(_windowClassName);

	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = HandleMessageSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = _hInstance;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = _wWindowClassName.c_str();
	wc.cbSize = sizeof(WNDCLASSEX);
	RegisterClassEx(&wc);

	RECT wr;
	int centerScreenX = GetSystemMetrics(SM_CXSCREEN) / 2 - _width / 2;
	int centerScreenY = GetSystemMetrics(SM_CYSCREEN) / 2 - _height / 2;
	wr.left = centerScreenX;
	wr.top = centerScreenY;
	wr.right = wr.left+ _width;
	wr.bottom = wr.top + _height;
	AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);
	_hwnd = CreateWindowEx(0,
		_wWindowClassName.c_str(),
		_wWindowTitle.c_str(),
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		wr.left,
		wr.top,
		wr.right-wr.left,
		wr.bottom-wr.top,
		NULL,
		NULL,
		_hInstance,
		windowContainer);

	if (_hwnd == NULL)
	{
		ErrorLogger::Log(GetLastError(), "CreateWindowEx Failed for window: " + this->_windowTitle);
		return false;
	}

	ShowWindow(_hwnd, SW_SHOW);
	SetForegroundWindow(_hwnd);
	SetFocus(_hwnd);

	return true;
}

bool MyWindow::ProcessMessage()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (PeekMessage(&msg, _hwnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_NULL)
	{
		if (!IsWindow(_hwnd))
		{
			_hwnd = NULL;
			UnregisterClass(_wWindowClassName.c_str(), _hInstance);
			return false;
		}
	}

	return true;
}

HWND MyWindow::GetHWND() const
{
	return _hwnd;
}


