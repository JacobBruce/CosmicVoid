#include "Keyboard.h"

KeyboardClient::KeyboardClient( KeyboardServer& kServer )
	: server( kServer )
{}

bool KeyboardClient::KeyIsPressed( int keycode ) const
{
	return server.keystates[ keycode ];
}

KeyEvent KeyboardClient::ReadKey()
{
	if( server.keybuffer.size() > 0 )
	{
		KeyEvent e = server.keybuffer.front();
		server.keybuffer.pop();
		return e;
	}
	else
	{
		return KeyEvent( KeyEvent::Invalid,0 );
	}
}

KeyEvent KeyboardClient::PeekKey() const
{
	if( server.keybuffer.size() > 0 )
	{
		return server.keybuffer.front();
	}
	else
	{
		return KeyEvent( KeyEvent::Invalid,0 );
	}
}

bool KeyboardClient::KeyEmpty() const
{
	return server.keybuffer.empty();
}

unsigned char KeyboardClient::ReadChar()
{
	if( server.charbuffer.size() > 0 )
	{
		unsigned char charcode = server.charbuffer.front();
		server.charbuffer.pop();
		return charcode;
	}
	else
	{
		return 0;
	}
}

unsigned char KeyboardClient::PeekChar() const
{
	if( server.charbuffer.size() > 0 )
	{
		return server.charbuffer.front();
	}
	else
	{
		return 0;
	}
}

bool KeyboardClient::CharEmpty() const
{
	return server.charbuffer.empty();
}

void KeyboardClient::FlushKeyBuffer()
{
	while( !server.keybuffer.empty() )
	{
		server.keybuffer.pop();
	}
}

void KeyboardClient::FlushCharBuffer()
{
	while( !server.charbuffer.empty() )
	{
		server.charbuffer.pop();
	}
}

void KeyboardClient::FlushBuffers()
{
	FlushKeyBuffer();
	FlushCharBuffer();
}

KeyboardServer::KeyboardServer()
{
	for( int x = 0; x < nKeys; x++ )
	{
		keystates[ x ] = false;
	}
}

void KeyboardServer::OnKeyPressed( int keycode )
{
	if (keycode > 0) {
		keystates[ keycode ] = true;

		keybuffer.push( KeyEvent( KeyEvent::Press,keycode ) );
		if( keybuffer.size() > bufferSize )
		{
			keybuffer.pop();
		}
	}
}

void KeyboardServer::OnKeyReleased( int keycode )
{
	if (keycode > 0) {
		keystates[ keycode ] = false;
		keybuffer.push( KeyEvent( KeyEvent::Release,keycode ) );
		if( keybuffer.size() > bufferSize )
		{
			keybuffer.pop();
		}
	}
}

void KeyboardServer::OnChar( unsigned char character )
{
	charbuffer.push( character );
	if( charbuffer.size() > bufferSize )
	{
		charbuffer.pop();
	}
}

