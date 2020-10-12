#pragma once
#include "MouseEvent.h"
#include <queue>

using namespace std;
class Mouse
{
public:
	void OnLeftPressed(int x, int y);
	void OnLeftReleased(int x, int y);
	void OnRightPressed(int x, int y);
	void OnRightReleased(int x, int y);
	void OnMiddlePressed(int x, int y);
	void OnMiddleReleased(int x, int y);
	void OnWheelup(int x, int y);
	void OnWheelDown(int x, int y);
	void OnMouseMove(int x, int y);
	void OnMouseMoveRaw(int x, int y);

	bool IsLeftDown();
	bool IsMiddleDown();
	bool IsRightDown();

	int GetPosX();
	int GetPosY();
	MousePoint GetPos();
	
	bool EventBufferIsEmpty();
	MouseEvent ReadEvent();

private:
	queue<MouseEvent> _eventBuffer;
	bool _leftIsDown = false;
	bool _rightIsDown = false;
	bool _mbuttonDown = false;
	int _x = 0;
	int _y = 0;
};