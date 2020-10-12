#include "MouseEvent.h"

MouseEvent::MouseEvent()
	:
	_type(EventType::Invalid),
	_x(0),
	_y(0)
{
}

MouseEvent::MouseEvent(const EventType type, const int x, const int y)
	:
	_type(type),
	_x(x),
	_y(y)
{
}

bool MouseEvent::IsValid() const
{
	return _type != EventType::Invalid;
}

MouseEvent::EventType MouseEvent::GetType() const
{
	return _type;
}

MousePoint MouseEvent::GetPos() const
{
	return { _x, _y };
}

int MouseEvent::GetPosX() const
{
	return _x;
}

int MouseEvent::GetPosY() const
{
	return _y;
}
