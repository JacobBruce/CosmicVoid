#pragma once
#include <queue>

enum KeyCodes {
    KEY_MOVE_UP=0,
    KEY_MOVE_DOWN=1,
    KEY_MOVE_FWRD=2,
    KEY_MOVE_BACK=3,
    KEY_MOVE_LEFT=4,
    KEY_MOVE_RIGHT=5,
    KEY_MOVE_TILTL=6,
    KEY_MOVE_TILTR=7,
    KEY_TARG_LOCK=8,
    KEY_TARG_SCAN=9,
    KEY_AUTO_PILOT=10
};

class KeyEvent
{
public:
	enum EventType
	{
		Press,
		Release,
		Invalid
	};
private:
	EventType type;
	int code;
public:
	KeyEvent( EventType type,int code )
		:
	type( type ),
	code( code )
	{}
	bool IsPress() const
	{
		return type == Press;
	}
	bool IsRelease() const
	{
		return type == Release;
	}
	bool IsValid() const
	{
		return type != Invalid;
	}
	int GetCode() const
	{
		return code;
	}
};

class KeyboardServer;

class KeyboardClient
{
public:
	KeyboardClient( KeyboardServer& kServer );
	bool KeyIsPressed( int keycode ) const;
	KeyEvent ReadKey();
	KeyEvent PeekKey() const;
	bool KeyEmpty() const;
	unsigned char ReadChar();
	unsigned char PeekChar() const;
	bool CharEmpty() const;
	void FlushKeyBuffer();
	void FlushCharBuffer();
	void FlushBuffers();
private:
	KeyboardServer& server;
};

class KeyboardServer
{
	friend KeyboardClient;
public:
	KeyboardServer();
	void OnKeyPressed( int keycode );
	void OnKeyReleased( int keycode );
	void OnChar( unsigned char character );
private:
	static const int nKeys = 350;
	static const int bufferSize = 4;
	bool keystates[ nKeys ];
	std::queue<KeyEvent> keybuffer;
	std::queue<unsigned char> charbuffer;
};
