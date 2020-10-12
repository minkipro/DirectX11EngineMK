#include "Engine.h"

bool Engine::Initialize(HINSTANCE hInstance, string window_title, string window_class, int width, int height)
{
	_timer.Start();
	if (!_wnd.Initialize(this, hInstance, window_title, window_class, width, height))
		return false;

	if (!_gfx.Initialize(_wnd.GetHWND(), width, height))
		return false;
	return true;
}

bool Engine::ProcessMessage()
{
	return _wnd.ProcessMessage();
}

void Engine::Update()
{
	float dt = static_cast<float>(_timer.GetMilisecondsElapsed());
	_timer.Restart();
	while (!_keyboard.CharBufferIsEmpty())
	{
		unsigned char c = _keyboard.ReadChar();
	}

	while (!_keyboard.KeyBufferIsEmpty())
	{
		KeyboardEvent kbe = _keyboard.ReadKey();
		unsigned char c = kbe.GetKeyCode();
	}

	while (!_mouse.EventBufferIsEmpty())
	{
		MouseEvent me = _mouse.ReadEvent();
		if (_mouse.IsRightDown())
		{
			if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
			{
				_gfx._camera3D.AdjustRotation((float)me.GetPosY() * _gfx._cameraRotSpeed*0.1f, (float)me.GetPosX() * _gfx._cameraRotSpeed* 0.1f, 0);
			}
		}
	}

	//_gfx._gameObject.AdjustRotation(0.0f, 0.001f * dt, 0);

	////_gfx._sprite.AdjustPosition(0.01f * dt, 0.0f, 0.0f);
	//if (_gfx._sprite.GetPositionFloat3().x > 160.0f)
	//{
	//	_gfx._sprite.SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	//}
	float Camera3DSpeed = _gfx._camera3DSpeed;

	if (_keyboard.KeyIsPressed(VK_SHIFT))
	{
		Camera3DSpeed *= 2.0f;
	}

	if (_keyboard.KeyIsPressed('W'))
	{
		_gfx._camera3D.AdjustPosition(_gfx._camera3D.GetForwardVector() * Camera3DSpeed*dt);
	}
	if (_keyboard.KeyIsPressed('S'))
	{
		_gfx._camera3D.AdjustPosition(_gfx._camera3D.GetBackwardVector() * Camera3DSpeed * dt);
	}
	if (_keyboard.KeyIsPressed('A'))
	{
		_gfx._camera3D.AdjustPosition(_gfx._camera3D.GetLeftVector() * Camera3DSpeed * dt);
	}
	if (_keyboard.KeyIsPressed('D'))
	{
		_gfx._camera3D.AdjustPosition(_gfx._camera3D.GetRightVector() * Camera3DSpeed * dt);
	}
	if (_keyboard.KeyIsPressed('E'))
	{
		_gfx._camera3D.AdjustPosition(0.0f, Camera3DSpeed* dt, 0.0f);
	}
	if (_keyboard.KeyIsPressed('Q'))
	{
		_gfx._camera3D.AdjustPosition(0.0f, -Camera3DSpeed* dt, 0.0f);
	}
}

void Engine::RenderFrame()
{
	_gfx.RenderFrame();
}
