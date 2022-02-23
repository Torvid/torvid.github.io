#define WASM 1
//#include "quote.h"


#include "quote.cpp"

extern "C" void PlatformPrint(const char* n);
extern "C" int print_num(int n);
extern "C" int64 PlatformTime();
//int64 PlatformTime()
//{
//    return 0;
//}

// Needed for the compiler to be happy
//__attribute__((export_name("memcpy")))
//void* memcpy(void* dst, const void* src, unsigned long size)
//{
//    for (int i = 0; i < size; i++)
//    {
//        ((char*)dst)[i] = ((char*)src)[i];
//    }
//    return dst;
//}
//__attribute__((export_name("memset")))
//void* memset(void* dst, int c, unsigned long size)
//{
//    for (int i = 0; i < size; i++)
//    {
//        ((char*)dst)[i] = c;
//    }
//    return dst;
//}

__attribute__((export_name("add")))
int add(int a, int b)
{
  return a + b;
}

extern unsigned char* __data_end;
extern unsigned char* __heap_base;

extern "C" bool32 LoadFileIntoCanvas(char* Destination, const char* File);


uint32* PlatformReadImageIntoMemory(void* Destination, const char* File)
{
    LoadFileIntoCanvas((char*)Destination, File);
    return 0;
}

#define VK_SPACE 0x20
#define VK_BACK 0x08
#define VK_RETURN 0x0D
#define VK_ESCAPE 0x1B

#define VK_UP 0x26
#define VK_DOWN 0x28
#define VK_LEFT 0x25
#define VK_RIGHT 0x27

#define VK_SHIFT 0x10
#define VK_CONTROL 0x11
#define VK_MENU 0x12

game_input Input = {};

// Keyboard

void KeyChanged(int VKCode, bool IsDown)
{
    if (IsValidIndex(VKCode - 65, ArrayCount(Input.Letters)))
        Input.Letters[VKCode - 65].Held = IsDown;

    if (IsValidIndex(VKCode - 48, ArrayCount(Input.Numbers)))
        Input.Numbers[VKCode - 48].Held = IsDown;

    if (VKCode == VK_SPACE)     Input.Space = IsDown;
    if (VKCode == VK_BACK)      Input.Backspace = IsDown;
    if (VKCode == VK_RETURN)    Input.Enter = IsDown;
    if (VKCode == VK_ESCAPE)    Input.Escape = IsDown;

    if (VKCode == VK_UP)        Input.Up = IsDown;
    if (VKCode == VK_DOWN)      Input.Down = IsDown;
    if (VKCode == VK_LEFT)      Input.Left = IsDown;
    if (VKCode == VK_RIGHT)     Input.Right = IsDown;

    if (VKCode == VK_SHIFT)     Input.Shift = IsDown;
    if (VKCode == VK_CONTROL)   Input.Ctrl = IsDown;
    if (VKCode == VK_MENU)      Input.Alt = IsDown;
}

__attribute__((export_name("KeyDown")))
void KeyDown(int keyCode)
{
    KeyChanged(keyCode, true);
}

__attribute__((export_name("KeyUp")))
void KeyUp(int keyCode)
{
    KeyChanged(keyCode, false);
}

// Mouse
__attribute__((export_name("MouseMove")))
void MouseMove(int MouseX, int MouseY)
{
    Input.MouseX = MouseX;
    Input.MouseY = MouseY;
}

__attribute__((export_name("MouseScroll")))
void MouseScroll(int MouseZ)
{
    Input.MouseZ += MouseZ;
}

__attribute__((export_name("MouseDown")))
void MouseDown(int which)
{
    if(which == 0)
        Input.MouseLeft = true;
    else if (which == 2)
        Input.MouseRight = true;
    else
        Input.MouseMiddle = true;
}

__attribute__((export_name("MouseUp")))
void MouseUp(int which)
{
    if (which == 0)
        Input.MouseLeft = false;
    else if (which == 2)
        Input.MouseRight = false;
    else
        Input.MouseMiddle = false;
}

__attribute__((export_name("UpdateGamepad")))
void UpdateGamepad(int i, 
    int X, int Y, int A, int B,
    int Up, int Down, int Left, int Right,
    int LeftBumper, int RightBumper, int LeftTrigger, int RightTrigger,
    int Start, int Back,
    float LeftStickX, float LeftStickY,
    float RightStickX, float RightStickY)
{
    InputUpdateButton(&Input.Controllers[i].StateA, A);
    InputUpdateButton(&Input.Controllers[i].StateB, B);
    InputUpdateButton(&Input.Controllers[i].StateX, X);
    InputUpdateButton(&Input.Controllers[i].StateY, Y);

    InputUpdateButton(&Input.Controllers[i].StateUp, Up);
    InputUpdateButton(&Input.Controllers[i].StateDown, Down);
    InputUpdateButton(&Input.Controllers[i].StateLeft, Left);
    InputUpdateButton(&Input.Controllers[i].StateRight, Right);

    InputUpdateFloat(&Input.Controllers[i].StateLeftStickX, LeftStickX);
    InputUpdateFloat(&Input.Controllers[i].StateLeftStickY, LeftStickY);
    InputUpdateFloat(&Input.Controllers[i].StateRightStickX, RightStickX);
    InputUpdateFloat(&Input.Controllers[i].StateRightStickY, RightStickY);

    InputUpdateButton(&Input.Controllers[i].StateLeftBumper, LeftBumper);
    InputUpdateButton(&Input.Controllers[i].StateRightBumper, RightBumper);
    InputUpdateButton(&Input.Controllers[i].StateLeftTrigger, LeftTrigger);
    InputUpdateButton(&Input.Controllers[i].StateRightTrigger, RightTrigger);

    InputUpdateButton(&Input.Controllers[i].StateStart, Start);
    InputUpdateButton(&Input.Controllers[i].StateBack, Back);
}

game_memory* GameMemory;
const int GameMemory_Size = Megabytes(12);

__attribute__((export_name("GetHeapStart")))
int GetHeapStart()
{
    return (int)(&__heap_base);
}
__attribute__((export_name("GetBackbufferPlacement")))
int GetBackbufferPlacement()
{
    char* Memory = (char*)(&__heap_base);
    char* Backbuffer = Memory + GameMemory_Size;
    return (int)(Backbuffer);
}
__attribute__((export_name("GetEndOfMemory")))
int GetEndOfMemory(int width, int height)
{
    char* Memory = (char*)(&__heap_base);
    char* Backbuffer = Memory + GameMemory_Size;
    char* end_of_memory = Backbuffer + (width * height * 4);
    return (int)(end_of_memory);
}

float LastTime;

__attribute__((export_name("Update")))
void Update(int width, int height, float time)
{
    char wat = 100;
    float DeltaTime = time - LastTime;
    LastTime = time;

    char* Memory = (char*)(&__heap_base);
    
    char* Backbuffer = Memory + GameMemory_Size;

    char* end_of_memory = Backbuffer + (width * height * 4);
    
    // Blitting
    char SlowTime = time * 64;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int i = y*width+x;
            char* pixel = (Backbuffer + i*4);
            //pixel[0] = (((float)x) / 1920.0f) * 256;
            //pixel[1] = (((float)y) / 1080.0f) * 256;
            pixel[0] = x+ SlowTime;
            pixel[1] = y;
            pixel[2] = SlowTime;
            pixel[3] = (char)255;
        }
    }
    
    Input.DeltaTime = DeltaTime;
    InputUpdateInt(&Input.StateMouseX, Input.MouseX);
    InputUpdateInt(&Input.StateMouseY, Input.MouseY);
    InputUpdateInt(&Input.StateMouseZ, Input.MouseZ);
    InputUpdateButton(&Input.StateMouseLeft, Input.MouseLeft);
    InputUpdateButton(&Input.StateMouseRight, Input.MouseRight);
    InputUpdateButton(&Input.StateMouseMiddle, Input.MouseMiddle);

    InputUpdateButton(&Input.StateSpace, Input.Space);
    InputUpdateButton(&Input.StateBackspace, Input.Backspace);
    InputUpdateButton(&Input.StateEnter, Input.Enter);
    InputUpdateButton(&Input.StateEscape, Input.Escape);

    InputUpdateButton(&Input.StateLeft, Input.Left);
    InputUpdateButton(&Input.StateRight, Input.Right);
    InputUpdateButton(&Input.StateUp, Input.Up);
    InputUpdateButton(&Input.StateDown, Input.Down);

    InputUpdateButton(&Input.StateShift, Input.Shift);
    InputUpdateButton(&Input.StateCtrl, Input.Ctrl);
    InputUpdateButton(&Input.StateAlt, Input.Alt);

    for (int i = 0; i < 26; i++)
    {
        InputUpdateButton(&Input.Letters[i], Input.Letters[i].Held);
    }

    for (int i = 0; i < 10; i++)
    {
        InputUpdateButton(&Input.Numbers[i], Input.Numbers[i].Held);
    }

    //for (int i = 0; i < sizeof(game_input); i++)
    //{
    //    ((char*)&Input)[i] = 0;
    //}
    
    //game_memory* GameMemory = (game_memory*)(Memory); // Don't clear it.
    GameMemory->PermanentStorageSize = Megabytes(1);
    GameMemory->TransientStorageSize = Megabytes(3);
    uint64 TotalSize = GameMemory->PermanentStorageSize + GameMemory->TransientStorageSize;
    GameMemory->PermanentStorage = ((uint8*)Memory+1024);
    GameMemory->TransientStorage = (uint8*)GameMemory->PermanentStorage + GameMemory->PermanentStorageSize;
    GameMemory->PlatformReadImageIntoMemory = &PlatformReadImageIntoMemory;
    GameMemory->PlatformPrint = &PlatformPrint;
    GameMemory->PlatformTime = &PlatformTime;

    game_offscreen_buffer Buffer;
    Buffer.Memory = Backbuffer;
    Buffer.Width = width;
    Buffer.Height = height;
    Buffer.BytesPerPixel = 4;
    Buffer.Pitch = width * 4;
    Buffer.Layout = ChannelLayout_ABGR;

    GameUpdateAndRender(0, GameMemory, &Input, &Buffer, 0);
    
    //for (int i = 0; i < 20000; i++)
    //{
    //    ((char*)Buffer.Memory)[i + Kilobytes(2800)] = (char)128;
    //}
    
    //print("This is text which was passed from C. :eyes:");
}

__attribute__((export_name("MakeArray")))
int MakeArray(int* array)
{
    return 0;
}