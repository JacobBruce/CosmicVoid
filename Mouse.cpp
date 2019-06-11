#include "Mouse.h"

MouseClient::MouseClient( MouseServer& server )
: server( server )
{}
float MouseClient::GetMouseX() const
{
	return server.x;
}
float MouseClient::GetMouseY() const
{
	return server.y;
}
bool MouseClient::LeftIsPressed() const
{
	return server.leftIsPressed;
}
bool MouseClient::RightIsPressed() const
{
	return server.rightIsPressed;
}
bool MouseClient::IsInWindow() const
{
	return server.isInWindow;
}
MouseEvent MouseClient::ReadMouse()
{
	if( server.buffer.size() > 0 )
	{
		MouseEvent e = server.buffer.front( );
		server.buffer.pop( );
		return e;
	}
	else
	{
		return MouseEvent( MouseEvent::Invalid,0,0 );
	}
}
bool MouseClient::MouseEmpty( ) const
{
	return server.buffer.empty( );
}


MouseServer::MouseServer()
:	isInWindow( false ),
	leftIsPressed( false ),
	rightIsPressed( false ),
	x( -1 ),
	y( -1 )
{}
void MouseServer::OnMouseMove( float x_new,float y_new )
{
	x = x_new;
	y = y_new;

	buffer.push( MouseEvent( MouseEvent::Move,x,y ) );
	if( buffer.size( ) > bufferSize )
	{
		buffer.pop( );
	}
}
void MouseServer::OnMouseLeave()
{
	isInWindow = false;
}
void MouseServer::OnMouseEnter()
{
	isInWindow = true;
}
void MouseServer::OnLeftPressed( )
{
	leftIsPressed = true;

	buffer.push( MouseEvent( MouseEvent::LPress,x,y ) );
	if( buffer.size( ) > bufferSize )
	{
		buffer.pop( );
	}
}
void MouseServer::OnLeftReleased( )
{
	leftIsPressed = false;

	buffer.push( MouseEvent( MouseEvent::LRelease,x,y ) );
	if( buffer.size( ) > bufferSize )
	{
		buffer.pop( );
	}
}
void MouseServer::OnRightPressed( )
{
	rightIsPressed = true;

	buffer.push( MouseEvent( MouseEvent::RPress,x,y ) );
	if( buffer.size( ) > bufferSize )
	{
		buffer.pop( );
	}
}
void MouseServer::OnRightReleased( )
{
	rightIsPressed = false;

	buffer.push( MouseEvent( MouseEvent::RRelease,x,y ) );
	if( buffer.size( ) > bufferSize )
	{
		buffer.pop( );
	}
}
void MouseServer::OnWheelUp( float x,float y )
{
	buffer.push( MouseEvent( MouseEvent::WheelUp,x,y ) );
	if( buffer.size( ) > bufferSize )
	{
		buffer.pop( );
	}

}
void MouseServer::OnWheelDown( float x,float y )
{
	buffer.push( MouseEvent( MouseEvent::WheelDown,x,y ) );
	if( buffer.size( ) > bufferSize )
	{
		buffer.pop( );
	}

}
bool MouseServer::IsInWindow() const
{
	return isInWindow;
}
