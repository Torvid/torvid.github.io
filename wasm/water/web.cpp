#define WASM 1
//#include "quote.h"

#include "quote.cpp"
//#include <pthread.h>

extern "C" void web_LoadPersistentData(const char* Data, int Size);
extern "C" void web_SavePersistentData(const char* Data, int Size);
extern "C" void web_PlatformPrint(const char* n);
//extern "C" int print_num(int n);
extern "C" double web_PlatformTime();
int64 web_PlatformTime_local()
{
    return (int64)web_PlatformTime();
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
extern "C" int strlen(const char* checkString)
{
    int i = 0;
    while (checkString[i] != 0)
    {
        i++;
    }
    return i;
}

__attribute__((export_name("add")))
int add(int a, int b)
{
  return a + b;
}

extern unsigned char* __data_end;
extern unsigned char* __heap_base;

extern "C" void web_PlatformReadImageIntoMemory(void* Destination, const char* File);
extern "C" void web_PlatformReadSoundIntoMemory(void* Destination, const char* File);


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
    GameInput* input                       = ((GameInput*)                (Memory + Kilobytes(1)));

    if (IsValidIndex(VKCode - 65, ArrayCapacity(input->Letters)))
        input->Letters[VKCode - 65].Held = IsDown;

    if (IsValidIndex(VKCode - 48, ArrayCapacity(input->Numbers)))
        input->Numbers[VKCode - 48].Held = IsDown;

    if (VKCode == VK_SPACE)     input->Space = IsDown;
    if (VKCode == VK_BACK)      input->Backspace = IsDown;
    if (VKCode == VK_RETURN)    input->Enter = IsDown;
    if (VKCode == VK_ESCAPE)    input->Escape = IsDown;
    if (VKCode == VK_DELETE)    input->Delete = IsDown;

    if (VKCode == VK_UP)        input->Up = IsDown;
    if (VKCode == VK_DOWN)      input->Down = IsDown;
    if (VKCode == VK_LEFT)      input->Left = IsDown;
    if (VKCode == VK_RIGHT)     input->Right = IsDown;

    if (VKCode == VK_SHIFT)     input->Shift = IsDown;
    if (VKCode == VK_CONTROL)   input->Ctrl = IsDown;
    if (VKCode == VK_MENU)      input->Alt = IsDown;
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
    GameInput* input                       = ((GameInput*)                (Memory + Kilobytes(1)));
    input->MouseX = MouseX;
    input->MouseY = MouseY;
}

__attribute__((export_name("MouseScroll")))
void MouseScroll(int MouseZ)
{
    char* Memory = (char*)(&__heap_base);
    GameInput* input                       = ((GameInput*)                (Memory + Kilobytes(1)));
    input->MouseZ += MouseZ;
}

__attribute__((export_name("MouseDown")))
void MouseDown(int which)
{
    char* Memory = (char*)(&__heap_base);
    GameInput* input                       = ((GameInput*)                (Memory + Kilobytes(1)));
    if(which == 0)
        input->MouseLeft = true;
    else if (which == 2)
        input->MouseRight = true;
    else
        input->MouseMiddle = true;
}

__attribute__((export_name("MouseUp")))
void MouseUp(int which)
{
    char* Memory = (char*)(&__heap_base);
    GameInput* input                       = ((GameInput*)                (Memory + Kilobytes(1)));
    if (which == 0)
        input->MouseLeft = false;
    else if (which == 2)
        input->MouseRight = false;
    else
        input->MouseMiddle = false;
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
    GameInput* input                       = ((GameInput*)                (Memory + Kilobytes(1)));
    InputUpdateButton(&input->Controllers[i].StateA, A);
    InputUpdateButton(&input->Controllers[i].StateB, B);
    InputUpdateButton(&input->Controllers[i].StateX, X);
    InputUpdateButton(&input->Controllers[i].StateY, Y);

    InputUpdateButton(&input->Controllers[i].StateUp, Up);
    InputUpdateButton(&input->Controllers[i].StateDown, Down);
    InputUpdateButton(&input->Controllers[i].StateLeft, Left);
    InputUpdateButton(&input->Controllers[i].StateRight, Right);

    InputUpdateFloat(&input->Controllers[i].StateLeftStickX, LeftStickX);
    InputUpdateFloat(&input->Controllers[i].StateLeftStickY, LeftStickY);
    InputUpdateFloat(&input->Controllers[i].StateRightStickX, RightStickX);
    InputUpdateFloat(&input->Controllers[i].StateRightStickY, RightStickY);

    InputUpdateButton(&input->Controllers[i].StateLeftBumper, LeftBumper);
    InputUpdateButton(&input->Controllers[i].StateRightBumper, RightBumper);
    InputUpdateButton(&input->Controllers[i].StateLeftTrigger, LeftTrigger);
    InputUpdateButton(&input->Controllers[i].StateRightTrigger, RightTrigger);

    InputUpdateButton(&input->Controllers[i].StateStart, Start);
    InputUpdateButton(&input->Controllers[i].StateBack, Back);
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

//void PrintHello(int thread, game_memory* Memory, game_offscreen_buffer* videoBuffer, game_sound_output_buffer* SoundBuffer)
//{
//    PlatformPrint("Hello from a different thread!");
//    print_num(thread);
//}

void web_PlatformThreadCall(ThreadCallType function, int ThreadIndex)
{
    //PlatformPrint("omg3");
    char* Memory = (char*)(&__heap_base);
    GameInput* Input                       = ((GameInput*)                (Memory + Kilobytes(1)));
    VideoBuffer* Buffer           = ((VideoBuffer*)     (Memory + Kilobytes(2)));
    game_sound_output_buffer* SoundBuffer   = ((game_sound_output_buffer*)  (Memory + Kilobytes(3)));
    GameMemory* gameMemory                 = ((GameMemory*)               (Memory + Kilobytes(4)));
    int* ThreadFunctions                    = ((int*)                       (Memory + Kilobytes(5)));
    
    //print_num(ThreadIndex);

    ThreadFunctions[ThreadIndex] = (int)function;
    gameMemory->threadsExecuting[ThreadIndex] = true;
}

//int count = 0;
// Called every 1ms by every worker thread.
__attribute__((export_name("WebThreadCall")))
void WebThreadCall(int thread)
{
    char* Memory = (char*)(&__heap_base);
    GameInput* input                       = ((GameInput*)                (Memory + Kilobytes(1)));
    VideoBuffer* buffer           = ((VideoBuffer*)     (Memory + Kilobytes(2)));
    game_sound_output_buffer* soundBuffer   = ((game_sound_output_buffer*)  (Memory + Kilobytes(3)));
    GameMemory* gameMemory                 = ((GameMemory*)               (Memory + Kilobytes(4)));
    int* ThreadFunctions                    = ((int*)                       (Memory + Kilobytes(5)));

    if (gameMemory->threadsExecuting[thread])
    {
        //count++;
        //if (count < 100) // Skip the first 100 calls, waiting for it to stabilize.
        //{
        //    gameMemory->threadsExecuting[thread] = false;
        //    return;
        //}

        ThreadCallType* LOL = (ThreadCallType*)ThreadFunctions[thread];
        LOL(thread, gameMemory, buffer, soundBuffer);
        gameMemory->threadsExecuting[thread] = false;
    }
}


struct read_file_result
{
    uint32 ContentsSize;
    void* Contents;
};

//const char* Filename, read_file_result memory
bool web_PlatformWriteFile(const char* Filename, void* Contents, uint32 ContentsSize)
{
    web_PlatformPrint("File write not suported.");
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
    float deltaTime = time - LastTime;
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
            char* pixel = (Backbuffer + i * 4);
            //pixel[0] = (((float)x) / 1920.0f) * 256;
            //pixel[1] = (((float)y) / 1080.0f) * 256;
            //pixel[0] = x+ SlowTime;
            //pixel[1] = y;
            //pixel[2] = SlowTime;
            pixel[3] = (char)255;
        }
    }
    
    GameInput* input                       = ((GameInput*)                (Memory + Kilobytes(1)));
    VideoBuffer* buffer           = ((VideoBuffer*)     (Memory + Kilobytes(2)));
    game_sound_output_buffer* soundBuffer   = ((game_sound_output_buffer*)  (Memory + Kilobytes(3)));
    GameMemory* gameMemory                 = ((GameMemory*)               (Memory + Kilobytes(4)));
    int* ThreadFunctions                    = ((int*)                       (Memory + Kilobytes(5)));

    input->deltaTime = deltaTime;
    InputUpdateInt(&input->StateMouseX, input->MouseX);
    InputUpdateInt(&input->StateMouseY, input->MouseY);
    InputUpdateInt(&input->StateMouseZ, input->MouseZ);
    InputUpdateButton(&input->StateMouseLeft, input->MouseLeft);
    InputUpdateButton(&input->StateMouseRight, input->MouseRight);
    InputUpdateButton(&input->StateMouseMiddle, input->MouseMiddle);

    InputUpdateButton(&input->StateSpace, input->Space);
    InputUpdateButton(&input->StateBackspace, input->Backspace);
    InputUpdateButton(&input->StateEnter, input->Enter);
    InputUpdateButton(&input->StateEscape, input->Escape);

    InputUpdateButton(&input->StateLeft, input->Left);
    InputUpdateButton(&input->StateRight, input->Right);
    InputUpdateButton(&input->StateUp, input->Up);
    InputUpdateButton(&input->StateDown, input->Down);

    InputUpdateButton(&input->StateShift, input->Shift);
    InputUpdateButton(&input->StateCtrl, input->Ctrl);
    InputUpdateButton(&input->StateAlt, input->Alt);

    for (int i = 0; i < 26; i++)
    {
        InputUpdateButton(&input->Letters[i], input->Letters[i].Held);
    }

    for (int i = 0; i < 10; i++)
    {
        InputUpdateButton(&input->Numbers[i], input->Numbers[i].Held);
    }


    gameMemory->permanentStorageSize = Kilobytes(1);
    gameMemory->transientStorageSize = Megabytes(16);
    uint64 TotalSize = gameMemory->permanentStorageSize + gameMemory->transientStorageSize;
    gameMemory->permanentStorage = ((uint8*)(Memory + Kilobytes(16)));
    gameMemory->transientStorage = (uint8*)gameMemory->permanentStorage + gameMemory->permanentStorageSize;
    gameMemory->platformReadImageIntoMemory = &web_PlatformReadImageIntoMemory;
    gameMemory->platformReadSoundIntoMemory = &web_PlatformReadSoundIntoMemory;
    gameMemory->platformPrint = &web_PlatformPrint;
    gameMemory->platformTime = &web_PlatformTime_local;
    gameMemory->platformThreadCall = &web_PlatformThreadCall;
    gameMemory->platformWriteFile = &web_PlatformWriteFile;
    //GameMemory->videoBuffer = &Buffer;
    //GameMemory->SoundBuffer = &SoundBuffer;

    //PlatformThreadCall((&PrintHello), 5);
    //Buffer = {};
    buffer->memory = Backbuffer;
    buffer->Width = width;
    buffer->Height = height;
    buffer->BytesPerPixel = 4;
    buffer->Pitch = width * 4;
    buffer->Layout = ChannelLayout_ABGR;

    //SoundBuffer = {};
    soundBuffer->SamplesPerSecond = 44100;
    soundBuffer->SampleCount = sampleCount;
    soundBuffer->Samples = (Sample*)SoundBufferMemory;

    web_LoadPersistentData((char*)gameMemory->permanentStorage, gameMemory->permanentStorageSize);

    //print_num((int)GameMemory->transientStorage);
    gameUpdateAndRender(gameMemory, input, buffer, soundBuffer);

    web_SavePersistentData((char*)gameMemory->permanentStorage, gameMemory->permanentStorageSize);

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