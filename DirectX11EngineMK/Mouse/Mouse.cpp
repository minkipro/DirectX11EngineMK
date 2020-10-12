#include "Mouse.h"

void Mouse::OnLeftPressed(int x, int y)
{
	_leftIsDown = true;
	_eventBuffer.push(MouseEvent(MouseEvent::EventType::LPress, x, y));
}

void Mouse::OnLeftReleased(int x, int y)
{
	_leftIsDown = false;
	_eventBuffer.push(MouseEvent(MouseEvent::EventType::LRelease, x, y));
}

void Mouse::OnRightPressed(int x, int y)
{
	_rightIsDown = true;
	_eventBuffer.push(MouseEvent(MouseEvent::EventType::RPress, x, y));
}

void Mouse::OnRightReleased(int x, int y)
{
	_rightIsDown = false;
	_eventBuffer.push(MouseEvent(MouseEvent::EventType::RRelease, x, y));
}

void Mouse::OnMiddlePressed(int x, int y)
{
	_mbuttonDown = true;
	_eventBuffer.push(MouseEvent(MouseEvent::EventType::MPress, x, y));
}

void Mouse::OnMiddleReleased(int x, int y)
{
	_mbuttonDown = false;
	_eventBuffer.push(MouseEvent(MouseEvent::EventType::MRelease, x, y));
}

void Mouse::OnWheelup(int x, int y)
{
	_eventBuffer.push(MouseEvent(MouseEvent::EventType::WheelUp, x, y));
}

void Mouse::OnWheelDown(int x, int y)
{
	_eventBuffer.push(MouseEvent(MouseEvent::EventType::WheelDown, x, y));
}

void Mouse::OnMouseMove(int x, int y)
{
	_x = x;
	_y = y;
	_eventBuffer.push(MouseEvent(MouseEvent::EventType::Move, x, y));
}

void Mouse::OnMouseMoveRaw(int x, int y)
{
	_eventBuffer.push(MouseEvent(MouseEvent::EventType::RAW_MOVE, x, y));
}

bool Mouse::IsLeftDown()
{
	return _leftIsDown;
}

bool Mouse::IsMiddleDown()
{
	return _mbuttonDown;
}

bool Mouse::IsRightDown()
{
	return _rightIsDown;
}

int Mouse::GetPosX()
{
	return _x;
}

int Mouse::GetPosY()
{
	return _y;
}

MousePoint Mouse::GetPos()
{
	return { _x, _y };
}

bool Mouse::EventBufferIsEmpty()
{
	return _eventBuffer.empty();
}

MouseEvent Mouse::ReadEvent()
{
	if (_eventBuffer.empty())
	{
		return MouseEvent();
	}
	else
	{
		MouseEvent e = _eventBuffer.front();
		_eventBuffer.pop();
		return e;
	}
}
