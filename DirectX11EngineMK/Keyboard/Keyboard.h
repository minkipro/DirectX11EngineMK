#pragma once
#include "KeyboardEvent.h"
#include <queue>
using namespace std;
class Keyboard
{
public:
	Keyboard();
	bool KeyIsPressed(const unsigned char keycode);
	bool KeyBufferIsEmpty();
	bool CharBufferIsEmpty();
	KeyboardEvent ReadKey();
	unsigned char ReadChar();
	void OnKeyPressed(const unsigned char key);
	void OnKeyReleased(const unsigned char key);
	void OnChar(const unsigned char key);
	void EnableAutoRepeatKeys();
	void DisableAutoRepeatKeys();
	void EnableAutoRepeatChars();
	void DisableAutoRepeatChars();
	bool IsKeysAutoRepeat();
	bool IsCharsAutoRepeat();

private:
	bool _autoRepeatKeys = false;
	bool _autoRepeatChars = false;
	bool _keyStates[256] = { 0, };
	queue<KeyboardEvent> _keyBuffer;
	queue<unsigned char> _charBuffer;
};