#pragma once
#include <queue>
#include <cstdlib>

class MouseServer;

class MouseEvent
{
public:
	enum Type
	{
		LPress,
		LRelease,
		RPress,
		RRelease,
		WheelUp,
		WheelDown,
		Move,
		Invalid
	};
private:
	const Type type;
	const float x;
	const float y;
public:
	MouseEvent( Type type,float x,float y )
		:
		type( type ),
		x( x ),
		y( y )
	{}
	bool IsValid() const
	{
		return type != Invalid;
	}
	Type GetType() const
	{
		return type;
	}
	float GetX() const
	{
		return x;
	}
	float GetY() const
	{
		return y;
	}
};

class MouseClient
{
public:
	MouseClient( MouseServer& server );
	float GetMouseX() const;
	float GetMouseY() const;
	bool LeftIsPressed() const;
	bool RightIsPressed() const;
	bool IsInWindow() const;
	MouseEvent ReadMouse();
	bool MouseEmpty() const;
private:
	MouseServer& server;
};

class MouseServer
{
	friend MouseClient;
public:
	MouseServer();
	void OnMouseMove( float x,float y );
	void OnMouseLeave();
	void OnMouseEnter();
	void OnLeftPressed();
	void OnLeftReleased();
	void OnRightPressed();
	void OnRightReleased();
	void OnWheelUp( float x,float y );
	void OnWheelDown( float x,float y );
	bool IsInWindow() const;
private:
	bool isInWindow;
	bool leftIsPressed;
	bool rightIsPressed;
	float x, y;
	static const int bufferSize = 4;
	std::queue<MouseEvent> buffer;
};
