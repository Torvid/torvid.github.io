#define WASM 1
//#include "quote.h"


#include "quote.cpp"
//#include <pthread.h>

extern "C" void LoadPersistentData(const char* Data, int Size);
extern "C" void SavePersistentData(const char* Data, int Size);
extern "C" void PlatformPrint(const char* n);
extern "C" int print_num(int n);
extern "C" double PlatformTime();
int64 PlatformTime_local()
{
    return (int64)PlatformTime();
}

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

extern "C" void PlatformReadImageIntoMemory(void* Destination, const char* File);
extern "C" void PlatformReadSoundIntoMemory(void* Destination, const char* File);


//uint32* PlatformReadImageIntoMemory(void* Destination, const char* File)
//{
//    LoadFileIntoCanvas((char*)Destination, File);
//    return 0;
//}

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
#define VK_DELETE 0x2E

// Keyboard

void KeyChanged(int VKCode, bool IsDown)
{
    char* Memory = (char*)(&__heap_base);
    game_input* Input                       = ((game_input*)                (Memory + Kilobytes(1)));

    if (IsValidIndex(VKCode - 65, ArrayCount(Input->Letters)))
        Input->Letters[VKCode - 65].Held = IsDown;

    if (IsValidIndex(VKCode - 48, ArrayCount(Input->Numbers)))
        Input->Numbers[VKCode - 48].Held = IsDown;

    if (VKCode == VK_SPACE)     Input->Space = IsDown;
    if (VKCode == VK_BACK)      Input->Backspace = IsDown;
    if (VKCode == VK_RETURN)    Input->Enter = IsDown;
    if (VKCode == VK_ESCAPE)    Input->Escape = IsDown;
    if (VKCode == VK_DELETE)    Input->Delete = IsDown;

    if (VKCode == VK_UP)        Input->Up = IsDown;
    if (VKCode == VK_DOWN)      Input->Down = IsDown;
    if (VKCode == VK_LEFT)      Input->Left = IsDown;
    if (VKCode == VK_RIGHT)     Input->Right = IsDown;

    if (VKCode == VK_SHIFT)     Input->Shift = IsDown;
    if (VKCode == VK_CONTROL)   Input->Ctrl = IsDown;
    if (VKCode == VK_MENU)      Input->Alt = IsDown;
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
    char* Memory = (char*)(&__heap_base);
    game_input* Input                       = ((game_input*)                (Memory + Kilobytes(1)));
    Input->MouseX = MouseX;
    Input->MouseY = MouseY;
}

__attribute__((export_name("MouseScroll")))
void MouseScroll(int MouseZ)
{
    char* Memory = (char*)(&__heap_base);
    game_input* Input                       = ((game_input*)                (Memory + Kilobytes(1)));
    Input->MouseZ += MouseZ;
}

__attribute__((export_name("MouseDown")))
void MouseDown(int which)
{
    char* Memory = (char*)(&__heap_base);
    game_input* Input                       = ((game_input*)                (Memory + Kilobytes(1)));
    if(which == 0)
        Input->MouseLeft = true;
    else if (which == 2)
        Input->MouseRight = true;
    else
        Input->MouseMiddle = true;
}

__attribute__((export_name("MouseUp")))
void MouseUp(int which)
{
    char* Memory = (char*)(&__heap_base);
    game_input* Input                       = ((game_input*)                (Memory + Kilobytes(1)));
    if (which == 0)
        Input->MouseLeft = false;
    else if (which == 2)
        Input->MouseRight = false;
    else
        Input->MouseMiddle = false;
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
    char* Memory = (char*)(&__heap_base);
    game_input* Input                       = ((game_input*)                (Memory + Kilobytes(1)));
    InputUpdateButton(&Input->Controllers[i].StateA, A);
    InputUpdateButton(&Input->Controllers[i].StateB, B);
    InputUpdateButton(&Input->Controllers[i].StateX, X);
    InputUpdateButton(&Input->Controllers[i].StateY, Y);

    InputUpdateButton(&Input->Controllers[i].StateUp, Up);
    InputUpdateButton(&Input->Controllers[i].StateDown, Down);
    InputUpdateButton(&Input->Controllers[i].StateLeft, Left);
    InputUpdateButton(&Input->Controllers[i].StateRight, Right);

    InputUpdateFloat(&Input->Controllers[i].StateLeftStickX, LeftStickX);
    InputUpdateFloat(&Input->Controllers[i].StateLeftStickY, LeftStickY);
    InputUpdateFloat(&Input->Controllers[i].StateRightStickX, RightStickX);
    InputUpdateFloat(&Input->Controllers[i].StateRightStickY, RightStickY);

    InputUpdateButton(&Input->Controllers[i].StateLeftBumper, LeftBumper);
    InputUpdateButton(&Input->Controllers[i].StateRightBumper, RightBumper);
    InputUpdateButton(&Input->Controllers[i].StateLeftTrigger, LeftTrigger);
    InputUpdateButton(&Input->Controllers[i].StateRightTrigger, RightTrigger);

    InputUpdateButton(&Input->Controllers[i].StateStart, Start);
    InputUpdateButton(&Input->Controllers[i].StateBack, Back);
}


const int GameMemory_Size = Megabytes(32);

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
__attribute__((export_name("GetSoundbufferPlacement")))
int GetSoundbufferPlacement(int width, int height, int sampleRate)
{
    char* Memory = (char*)(&__heap_base);
    char* Backbuffer = Memory + GameMemory_Size;
    char* SoundBufferMemory = Backbuffer + (width * height * 4);
    return (int)(SoundBufferMemory);
}
__attribute__((export_name("GetEndOfMemory")))
int GetEndOfMemory(int width, int height, int sampleRate)
{
    char* Memory = (char*)(&__heap_base);
    char* Backbuffer = Memory + GameMemory_Size;
    char* SoundBufferMemory = Backbuffer + (width * height * 4);
    char* end_of_memory = SoundBufferMemory + sampleRate * 2;
    return (int)(end_of_memory);
}

float LastTime;

//void PrintHello(int thread, game_memory* Memory, game_offscreen_buffer* VideoBuffer, game_sound_output_buffer* SoundBuffer)
//{
//    PlatformPrint("Hello from a different thread!");
//    print_num(thread);
//}

void PlatformThreadCall(ThreadCallType function, int ThreadIndex)
{
    //PlatformPrint("omg3");
    char* Memory = (char*)(&__heap_base);
    game_input* Input                       = ((game_input*)                (Memory + Kilobytes(1)));
    game_offscreen_buffer* Buffer           = ((game_offscreen_buffer*)     (Memory + Kilobytes(2)));
    game_sound_output_buffer* SoundBuffer   = ((game_sound_output_buffer*)  (Memory + Kilobytes(3)));
    game_memory* GameMemory                 = ((game_memory*)               (Memory + Kilobytes(4)));
    int* ThreadFunctions                    = ((int*)                       (Memory + Kilobytes(5)));
    
    //print_num(ThreadIndex);

    ThreadFunctions[ThreadIndex] = (int)function;
    GameMemory->ThreadsExecuting[ThreadIndex] = true;
}

int count = 0;
// Called every 1ms by every worker thread.
__attribute__((export_name("WebThreadCall")))
void WebThreadCall(int thread)
{
    char* Memory = (char*)(&__heap_base);
    game_input* Input                       = ((game_input*)                (Memory + Kilobytes(1)));
    game_offscreen_buffer* Buffer           = ((game_offscreen_buffer*)     (Memory + Kilobytes(2)));
    game_sound_output_buffer* SoundBuffer   = ((game_sound_output_buffer*)  (Memory + Kilobytes(3)));
    game_memory* GameMemory                 = ((game_memory*)               (Memory + Kilobytes(4)));
    int* ThreadFunctions                    = ((int*)                       (Memory + Kilobytes(5)));

    if (GameMemory->ThreadsExecuting[thread])
    {
        //count++;
        //if (count < 100) // Skip the first 100 calls, waiting for it to stabilize.
        //{
        //    GameMemory->ThreadsExecuting[thread] = false;
        //    return;
        //}

        ThreadCallType* LOL = (ThreadCallType*)ThreadFunctions[thread];
        LOL(thread, GameMemory, Buffer, SoundBuffer);
        GameMemory->ThreadsExecuting[thread] = false;
    }
}

bool32 PlatformWriteFile(thread_context* Thread, const char* Filename, read_file_result Memory)
{
    PlatformPrint("File write not suported.");
    return 0;
}

__attribute__((export_name("Update")))
void Update(int width, int height, float time, int sampleCount, int sampleRate)
{
    if (width <= 0)
        return;
    if (height <= 0)
        return;

    char wat = 100;
    float DeltaTime = time - LastTime;
    LastTime = time;

    char* Memory = (char*)(&__heap_base);
    
    char* Backbuffer = Memory + GameMemory_Size;

    char* SoundBufferMemory = Backbuffer + (width * height * 4);

    char* end_of_memory = SoundBufferMemory + sampleRate;
    
    // Blitting

    //char SlowTime = time * 64;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int i = y*width+x;
            char* pixel = (Backbuffer + i*4);
            //pixel[0] = (((float)x) / 1920.0f) * 256;
            //pixel[1] = (((float)y) / 1080.0f) * 256;
            //pixel[0] = x+ SlowTime;
            //pixel[1] = y;
            //pixel[2] = SlowTime;
            pixel[3] = (char)255;
        }
    }
    
    game_input* Input                       = ((game_input*)                (Memory + Kilobytes(1)));
    game_offscreen_buffer* Buffer           = ((game_offscreen_buffer*)     (Memory + Kilobytes(2)));
    game_sound_output_buffer* SoundBuffer   = ((game_sound_output_buffer*)  (Memory + Kilobytes(3)));
    game_memory* GameMemory                 = ((game_memory*)               (Memory + Kilobytes(4)));
    int* ThreadFunctions                    = ((int*)                       (Memory + Kilobytes(5)));

    Input->DeltaTime = DeltaTime;
    InputUpdateInt(&Input->StateMouseX, Input->MouseX);
    InputUpdateInt(&Input->StateMouseY, Input->MouseY);
    InputUpdateInt(&Input->StateMouseZ, Input->MouseZ);
    InputUpdateButton(&Input->StateMouseLeft, Input->MouseLeft);
    InputUpdateButton(&Input->StateMouseRight, Input->MouseRight);
    InputUpdateButton(&Input->StateMouseMiddle, Input->MouseMiddle);

    InputUpdateButton(&Input->StateSpace, Input->Space);
    InputUpdateButton(&Input->StateBackspace, Input->Backspace);
    InputUpdateButton(&Input->StateEnter, Input->Enter);
    InputUpdateButton(&Input->StateEscape, Input->Escape);

    InputUpdateButton(&Input->StateLeft, Input->Left);
    InputUpdateButton(&Input->StateRight, Input->Right);
    InputUpdateButton(&Input->StateUp, Input->Up);
    InputUpdateButton(&Input->StateDown, Input->Down);

    InputUpdateButton(&Input->StateShift, Input->Shift);
    InputUpdateButton(&Input->StateCtrl, Input->Ctrl);
    InputUpdateButton(&Input->StateAlt, Input->Alt);

    for (int i = 0; i < 26; i++)
    {
        InputUpdateButton(&Input->Letters[i], Input->Letters[i].Held);
    }

    for (int i = 0; i < 10; i++)
    {
        InputUpdateButton(&Input->Numbers[i], Input->Numbers[i].Held);
    }


    GameMemory->PermanentStorageSize = Kilobytes(1);
    GameMemory->TransientStorageSize = Megabytes(16);
    uint64 TotalSize = GameMemory->PermanentStorageSize + GameMemory->TransientStorageSize;
    GameMemory->PermanentStorage = ((uint8*)(Memory + Kilobytes(16)));
    GameMemory->TransientStorage = (uint8*)GameMemory->PermanentStorage + GameMemory->PermanentStorageSize;
    GameMemory->PlatformReadImageIntoMemory = &PlatformReadImageIntoMemory;
    GameMemory->PlatformReadSoundIntoMemory = &PlatformReadSoundIntoMemory;
    GameMemory->PlatformPrint = &PlatformPrint;
    GameMemory->PlatformTime = &PlatformTime_local;
    GameMemory->PlatformThreadCall = &PlatformThreadCall;
    GameMemory->PlatformWriteFile = &PlatformWriteFile;
    //GameMemory->VideoBuffer = &Buffer;
    //GameMemory->SoundBuffer = &SoundBuffer;

    //PlatformThreadCall((&PrintHello), 5);
    //Buffer = {};
    Buffer->Memory = Backbuffer;
    Buffer->Width = width;
    Buffer->Height = height;
    Buffer->BytesPerPixel = 4;
    Buffer->Pitch = width * 4;
    Buffer->Layout = ChannelLayout_ABGR;

    //SoundBuffer = {};
    SoundBuffer->SamplesPerSecond = 44100;
    SoundBuffer->SampleCount = sampleCount;
    SoundBuffer->Samples = (Sample*)SoundBufferMemory;


    thread_context Thread;
    Thread.Placeholder = 2;

    LoadPersistentData((char*)GameMemory->PermanentStorage, GameMemory->PermanentStorageSize);

    //print_num((int)GameMemory->TransientStorage);
    GameUpdateAndRender(&Thread, GameMemory, Input, Buffer, SoundBuffer);

    SavePersistentData((char*)GameMemory->PermanentStorage, GameMemory->PermanentStorageSize);

    //testfunction(&Thread, GameMemory, &Input, &Buffer, &SoundBuffer);
    //print_num();
    //for (int i = 0; i < 20000; i++)
    //{
    //    ((char*)Buffer->Memory)[i + Kilobytes(2800)] = (char)128;
    //}
    
    //print("This is text which was passed from C. :eyes:");
}

__attribute__((export_name("MakeArray")))
int MakeArray(int* array)
{
    return 0;
}