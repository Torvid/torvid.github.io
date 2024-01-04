#pragma once

// Platform layer needs to implement everything specified in this file for the game to function.

// structs are CapitalCase
// functions are CapitalCase
// macro functions are CapitalCase
// function pointer typedef variables are snake_case
// variables camelCase
// macro variables SCREAMING_SNAKE_CASE

#define null 0

#define true 1
#define false 0

#define GetMacro(_0, _1, _2, NAME, ...) NAME
#define Assert(...) GetMacro(_0, ##__VA_ARGS__, Assert2, Assert1, Assert0)(__VA_ARGS__)

#if _WIN32
	#define Assert1(expression)      if(!(expression)) { __debugbreak(); }
	#define Assert2(expression, msg) if(!(expression)) { __debugbreak(); }
#else
	#define Assert1(expression)      if(!(expression)) { 1 / 0; }
	#define Assert2(expression, msg) if(!(expression)) { 1 / 0; }
#endif

typedef signed char         int8;
typedef signed short        int16;
typedef signed int          int32;
typedef signed long long    int64;

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long long  uint64;

typedef int32 bool;

#define Kilobytes(x) ((x) * (1024LL))
#define Megabytes(x) ((Kilobytes(x)) * (1024LL))
#define Gigabytes(x) ((Megabytes(x)) * (1024LL))
#define Terabytes(x) ((Gigabytes(x)) * (1024LL))

typedef struct
{
	int sizeX;
	int sizeY;
	uint32* data; // RGBA32
} Image;

typedef struct 
{
    bool held; // Button is being held down
    bool down; // Button changed from down to up
    bool up; // Button changed from up to down
    bool lastHeld;
} GameButtonState;

typedef void PlatformRequestImageRead(const char* path, Image* image);
typedef void PlatformBreak();
typedef int64 PlatformTime();
typedef void PlatformPrint(const char* string);


#define UNION_BUTTON(name) union { GameButtonState State##name; struct { bool name; bool name##Up; bool name##Down; }; };
void UpdateButton(GameButtonState* State, bool held)
{
	State->held = held;
	State->down = false;
	State->up = false;
	if(State->lastHeld != State->held)
	{
		State->lastHeld = State->held;
		if(State->held)
		{
			State->up = true;
		}
		else
		{
			State->down = true;
		}
	}
}
typedef struct
{
	// set by engine first time you call update(). 
	// Read it and draw it to the screen once per frame.
	uint8* videoBuffer;

	// size of the screen
	int videoSizeX;
	int videoSizeY;
	
	// pointer to memory for the game to use. 100MB should be plenty.
	void* memory;
	
	uint32* lastImageDataLocation;

	PlatformRequestImageRead* RequestImageRead;
	
	// function that gets the "current time" for profiling.
	PlatformTime* time;
	
	// function that takes a string and prints it to the console. (if one exists)
	PlatformPrint* Print;
	
	// the position of the mouse cursor.
	int mouseX;
	int mouseY;
	int mouseZ;
	
	// whether the mouse button is down or not
	UNION_BUTTON(mouseLeft)
    UNION_BUTTON(mouseRight)
    UNION_BUTTON(mouseMiddle)
	
    union
    {
        GameButtonState letters[26];
        struct
        {
			UNION_BUTTON(a)
			UNION_BUTTON(b)
			UNION_BUTTON(c)
			UNION_BUTTON(d)
			UNION_BUTTON(e)
			UNION_BUTTON(f)
			UNION_BUTTON(g)
			UNION_BUTTON(h)
			UNION_BUTTON(i)
			UNION_BUTTON(j)
			UNION_BUTTON(k)
			UNION_BUTTON(l)
			UNION_BUTTON(m)
			UNION_BUTTON(n)
			UNION_BUTTON(o)
			UNION_BUTTON(p)
			UNION_BUTTON(q)
			UNION_BUTTON(r)
			UNION_BUTTON(s)
			UNION_BUTTON(t)
			UNION_BUTTON(u)
			UNION_BUTTON(v)
			UNION_BUTTON(w)
			UNION_BUTTON(x)
			UNION_BUTTON(y)
			UNION_BUTTON(z)
		};
	};
	
    union
    {
        GameButtonState numbers[10];
        struct
        {
			UNION_BUTTON(d0)
			UNION_BUTTON(d1)
			UNION_BUTTON(d2)
			UNION_BUTTON(d3)
			UNION_BUTTON(d4)
			UNION_BUTTON(d5)
			UNION_BUTTON(d6)
			UNION_BUTTON(d7)
			UNION_BUTTON(d8)
			UNION_BUTTON(d9)
		};
	};

    UNION_BUTTON(space)
    UNION_BUTTON(backspace)
    UNION_BUTTON(enter)
    UNION_BUTTON(escape)
    UNION_BUTTON(delete)
    UNION_BUTTON(left)
    UNION_BUTTON(right)
    UNION_BUTTON(up)
    UNION_BUTTON(down)
    UNION_BUTTON(shift)
    UNION_BUTTON(ctrl)
    UNION_BUTTON(alt)

	// time in seconds since the last frame.
	float deltaTime;
} Platform;

typedef void GameUpdateAndRender(Platform* platform);
