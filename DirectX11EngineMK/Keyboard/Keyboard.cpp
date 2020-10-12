#include "Keyboard.h"

Keyboard::Keyboard()
{
	for (int i = 0; i < 256; i++)
	{
		_keyStates[i] = false;
	}
}

bool Keyboard::KeyIsPressed(const unsigned char keycode)
{
	return _keyStates[keycode];
}

bool Keyboard::KeyBufferIsEmpty()
{
	return _keyBuffer.empty();
}

bool Keyboard::CharBufferIsEmpty()
{
	return _charBuffer.empty();
}

KeyboardEvent Keyboard::ReadKey()
{
	if (_keyBuffer.empty())
	{
		return KeyboardEvent();
	}
	else
	{
		KeyboardEvent e = _keyBuffer.front();
		_keyBuffer.pop();
		return e;
	}
}

unsigned char Keyboard::ReadChar()
{
	if (_charBuffer.empty())
	{
		return 0u;
	}
	else
	{
		unsigned char c = _charBuffer.front();
		_charBuffer.pop();
		return c;
	}
}

void Keyboard::OnKeyPressed(const unsigned char key)
{
	_keyStates[key] = true;
	_keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Press, key));
}

void Keyboard::OnKeyReleased(const unsigned char key)
{
	_keyStates[key] = false;
	_keyBuffer.push(KeyboardEvent(KeyboardEvent::EventType::Release, key));
}

void Keyboard::OnChar(const unsigned char key)
{
	_charBuffer.push(key);
}

void Keyboard::EnableAutoRepeatKeys()
{
	_autoRepeatKeys = true;
}

void Keyboard::DisableAutoRepeatKeys()
{
	_autoRepeatKeys = false;
}

void Keyboard::EnableAutoRepeatChars()
{
	_autoRepeatChars = true;
}

void Keyboard::DisableAutoRepeatChars()
{
	_autoRepeatChars = false;
}

bool Keyboard::IsKeysAutoRepeat()
{
	return _autoRepeatKeys;
}

bool Keyboard::IsCharsAutoRepeat()
{
	return _autoRepeatChars;
}
