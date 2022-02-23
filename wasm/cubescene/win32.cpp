
/*
    TODO: This is not a final platform layer.
    - save game locations documents/appdata
    - getting a handle to executable file
    - assests
    - threating
    - raw input, support for multiple keyboads
    - Sleep/timeBeginPeriod
    - ClipCursor() so the mouse doesn't move off the window
    - Fullscreens upport
    - WM_SERCURSOR (control curso visiblilty)
    - QuerycancelAutoplay
    - WM_ACTIVEAPP (for when we are not the active aplication)
    - Blit speed improvemetns (BitBlit)
    - Hardware acceleration (OpenGL, Direct3D or BOTH??)
    - GetKeyboardLayout (keyboard language support)
*/

#include "quote_platform.cpp"

//#include <signal.h>
//#include "setjmp.h"

typedef uint32 memory_index;

#define null 0
#define internal static 
#define local_persist static // Good to be able to find these because they suck and should never be used
#define global_variable static // We'll only ever have one translation unit for the whole program so static is program-wide global


#define Pi 3.14152128f


#include <windows.h>
#include <stdio.h>
#include <xinput.h>
#include <dsound.h>

void PlatformPrint(const char* lpOutputString)
{
    char string[100];
    char* a = string;
    strcpy(a, lpOutputString);
    strcat(a, "\n");
    OutputDebugStringA(a);
}
platform_print* print = &PlatformPrint;

int ends_with(const char* str, const char* suffix)
{
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);

    return (str_len >= suffix_len) && (!memcmp(str + str_len - suffix_len, suffix, suffix_len));
}


read_file_result PlatformReadFile(thread_context* Thread, const char* Filename)
{
    read_file_result Result = {};

#if QUOTE_INTERNAL
    local_persist uint64 FileCounter = 0;
    FileCounter++;
    LPVOID BaseAddress = (LPVOID)(Terabytes(3) + Gigabytes(4 * FileCounter));
#else
    LPVOID BaseAddress = 0;
#endif

    HANDLE FileHandle = CreateFileA(Filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (FileHandle == INVALID_HANDLE_VALUE)// failed to open file
        return Result;

    LARGE_INTEGER Size;
    if (!GetFileSizeEx(FileHandle, &Size)) // failed to get size
        return Result;

    // can't load files larger than 4 gb
    uint32 FileSize32 = SafeTrunchateUint64(Size.QuadPart);
    // TODO: replace this with something else, we don't want to make lots of small VirtualAllocs. Repalce with HeapAlloc or our own arena allocator.
    Result.Contents = VirtualAlloc(BaseAddress, FileSize32, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!Result.Contents) // failed to allocate memoy
        return Result;

    DWORD BytesRead = 0;
    if (!ReadFile(FileHandle, Result.Contents, FileSize32, &BytesRead, 0)) // failed to read file
    {
        VirtualFree(Result.Contents, null, MEM_RELEASE);
        return Result;
    }

    if (BytesRead != FileSize32) // we didn't end up reading the entire file
    {
        VirtualFree(Result.Contents, null, MEM_RELEASE);
        return Result;
    }

    CloseHandle(FileHandle);
    Result.ContentsSize = FileSize32;

    return Result;
}

void PlatformFreeFileMemory(thread_context* Thread, read_file_result Memory)
{
    VirtualFree(Memory.Contents, null, MEM_RELEASE);
}

bool32 PlatformWriteFile(thread_context* Thread, char* Filename, read_file_result Memory)
{
    HANDLE FileHandle = CreateFileA(Filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if (FileHandle == INVALID_HANDLE_VALUE)// failed to open file
        return 0;

    DWORD BytesWritten = 0;
    if (!WriteFile(FileHandle, Memory.Contents, Memory.ContentsSize, &BytesWritten, 0)) // failed to write file
        return 0;

    if (BytesWritten != Memory.ContentsSize) // we didn't end up writing the entire file
        return 0;

    CloseHandle(FileHandle);

    return 1;
}

struct TGA_HEADER
{
    char imagetype;
    uint16 colourmapstart;
    uint16 colourmaplength;
    char colourmapbits;

    uint16 xstart;
    uint16 ystart;
    uint16 width;
    uint16 height;
    char bits;
    char descriptor;
};

const int targa_header_size = 18;
uint32* PlatformReadImageIntoMemory(void* Destination, const char* File)
{
    read_file_result ImageFile = PlatformReadFile(0, File);

    if (!ImageFile.Contents)
        return 0;

    TGA_HEADER* header = (TGA_HEADER*)ImageFile.Contents;

    int Width = header->width;
    int Height = header->height;

    //if (OutWidth)
    //    *OutWidth = Width;
    //if (OutHeight)
    //    *OutHeight = Height;

    // Targa images are flipped upside-down.
    for (int y = 0; y < Height; y++)
    {
        for (int x = 0; x < Width; x++)
        {
            int i1 = (x + y * Width);
            int i2 = (x + ((Height-1) - y) * Width);
            ((uint32*)Destination)[i1] = ((uint32*)(((char*)ImageFile.Contents) + targa_header_size))[i2];
        }
    }

    PlatformFreeFileMemory(0, ImageFile);

    return (uint32*)Destination;
}

bool FileExists(const char* file)
{
    WIN32_FIND_DATA FindFileData;
    HANDLE handle = FindFirstFile(file, &FindFileData);
    int found = handle != INVALID_HANDLE_VALUE;
    if (found)
    {
        //FindClose(&handle); this will crash
        FindClose(handle);
    }
    return (bool)found;
}

// wtf
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState) // external function template
typedef X_INPUT_GET_STATE(X_Input_Get_State); // a way to define a function with that template
X_INPUT_GET_STATE(XInputGetStateStub) { return ERROR_DEVICE_NOT_CONNECTED; } // define stub function
global_variable X_Input_Get_State* XInputGetState_ = XInputGetStateStub; // global pointer to the stub, this is the default, this pointer can then be overwritten with the real function if it's found.
#define XInputGetState XInputGetState_ // 

#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef X_INPUT_SET_STATE(X_Input_Set_State);
X_INPUT_SET_STATE(XInputSetStateStub) { return ERROR_DEVICE_NOT_CONNECTED; }
global_variable X_Input_Set_State* XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

internal HMODULE Win32GetXInputLibrary()
{
    HMODULE XInputLibrary = LoadLibraryA("XInput9_1_0.dll");
    if (XInputLibrary) return XInputLibrary;

    XInputLibrary = LoadLibraryA("xinput1_1.dll");
    if (XInputLibrary) return XInputLibrary;

    XInputLibrary = LoadLibraryA("xinput1_2.dll");
    if (XInputLibrary) return XInputLibrary;

    XInputLibrary = LoadLibraryA("xinput1_3.dll");
    if (XInputLibrary) return XInputLibrary;

    XInputLibrary = LoadLibraryA("xinput1_4.dll");
    if (XInputLibrary) return XInputLibrary;

    return null;
}

internal void Win32LoadXInput()
{
    HMODULE XInputLibrary = Win32GetXInputLibrary();

    if (XInputLibrary)
    {
        XInputGetState = (X_Input_Get_State*)GetProcAddress(XInputLibrary, "XInputGetState");
        XInputSetState = (X_Input_Set_State*)GetProcAddress(XInputLibrary, "XInputSetState");
    }
}


struct win32_game_code
{
    HMODULE GameCodeDLL;
    game_update_and_render* UpdateAndRender;
    bool IsValid = false;
};

internal win32_game_code Win32LoadGameCode()
{
    win32_game_code Result = {};

    Result.IsValid = false;
    Result.GameCodeDLL = LoadLibraryA("quote.dll");
    if (Result.GameCodeDLL)
    {
        Result.UpdateAndRender = (game_update_and_render*)GetProcAddress(Result.GameCodeDLL, "GameUpdateAndRender");
        Result.IsValid = Result.UpdateAndRender;
    }

    if (!Result.IsValid)
    {
        Result.UpdateAndRender = GameUpdateAndRenderStub;
    }

    return Result;
}
internal void Win32UnloadGameCode(win32_game_code* GameCode)
{
    if(GameCode->GameCodeDLL)
        FreeLibrary(GameCode->GameCodeDLL);

    GameCode->UpdateAndRender = 0;
    GameCode->IsValid = false;

}


struct win32_debug_time_marker
{
    DWORD PlayCursor;
    DWORD WriteCursor;
};


struct win32_offscreen_buffer // always 4 bytes per pixel BGRA
{
    BITMAPINFO Info;
    void* Memory;
    int Width;
    int Height;
    int BytesPerPixel;
    int Pitch; // Distance to get to the next line
};
global_variable bool32 GlobalRunning = true;
global_variable win32_offscreen_buffer GlobalBackbuffer;
global_variable LPDIRECTSOUNDBUFFER GlobalSoundBuffer;
global_variable game_input Input;
global_variable LARGE_INTEGER GlobalTickFrequency;
global_variable game_sound_output_buffer SoundBuffer = {};
global_variable game_offscreen_buffer Buffer = {};
global_variable game_memory GameMemory = {};
global_variable win32_game_code GameCode;
global_variable int64 times[100];
global_variable int SkipReload = 0;

struct win32_sound_output
{
    int SamplesPerSecond;
    int BytesPerSample;
    int BufferSize;
    uint32 RunningSampleIndex;
    int LatencySampleCount;
};

struct win32_window_dimensions
{
    int Width;
    int Height;
};

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(Direct_Sound_Create);


internal void win32_InitDSound(HWND Window, int32 SamplesPerSecond, int32 BufferSize)
{
    // Load the library
    HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");
    if (!DSoundLibrary)
        return;

    // Get a DirectSound object
    Direct_Sound_Create* DirectSoundCreate = (Direct_Sound_Create*)GetProcAddress(DSoundLibrary, "DirectSoundCreate");

    LPDIRECTSOUND DirectSound;
    if (!DirectSoundCreate || !SUCCEEDED(DirectSoundCreate(0, &DirectSound, 0)))
        return;

    if (!SUCCEEDED(DirectSound->SetCooperativeLevel(Window, DSSCL_PRIORITY))) // if this fails we're fucked
        return;

    // Create a primary buffer, this is not a real buffer of data, it's just here so we can get at the SetFormat function to tell the """sound card""" what our sound will be like.
    DSBUFFERDESC BufferDescription = {}; // clear to all 0s
    BufferDescription.dwSize = sizeof(BufferDescription);
    BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;

    LPDIRECTSOUNDBUFFER PrimaryBuffer;
    HRESULT Error = DirectSound->CreateSoundBuffer(&BufferDescription, &PrimaryBuffer, 0);
    if (!SUCCEEDED(Error))
        return;

    WAVEFORMATEX WaveFormat = {};
    WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
    WaveFormat.nChannels = 2; // stereo
    WaveFormat.nSamplesPerSec = SamplesPerSecond;                                                                              // (size of this)
    WaveFormat.wBitsPerSample = 16;                                                                                            //       V
    WaveFormat.nBlockAlign = (WaveFormat.nChannels * WaveFormat.wBitsPerSample) / 8; // size of each set of channel data in bytes. [left right] left right left right
    WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;
    WaveFormat.cbSize = 0;

    if (!SUCCEEDED(PrimaryBuffer->SetFormat(&WaveFormat)))
        return;

    // Create a secondary buffer we actually write things to
    DSBUFFERDESC SecondaryBufferDescription = {}; // clear to all 0s
    SecondaryBufferDescription.dwSize = sizeof(SecondaryBufferDescription);
    SecondaryBufferDescription.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_GETCURRENTPOSITION2;// ; // DSBCAPS_GETCURRENTPOSITION2 - use this to get accurate play head position?
    SecondaryBufferDescription.dwBufferBytes = BufferSize;
    SecondaryBufferDescription.lpwfxFormat = &WaveFormat;

    if (!SUCCEEDED(DirectSound->CreateSoundBuffer(&SecondaryBufferDescription, &GlobalSoundBuffer, 0)))
        return;

    // we did it!
}

internal void win32_ClearSoundBuffer(win32_sound_output* SoundOutput)
{
    VOID* Region1;
    DWORD Region1Size;
    VOID* Region2;
    DWORD Region2Size;

    if (!SUCCEEDED(GlobalSoundBuffer->Lock(0, SoundOutput->BufferSize, &Region1, &Region1Size, &Region2, &Region2Size, 0)))
        return;

    uint8* DestinationSample = (uint8*)Region1;
    for (uint64 i = 0; i < Region1Size; i++)
    {
        *DestinationSample++ = 0;
    }

    DestinationSample = (uint8*)Region2;
    for (uint64 i = 0; i < Region2Size; i++)
    {
        *DestinationSample++ = 0;
    }


    GlobalSoundBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);

}
internal void win32_FillSoundBuffer(win32_sound_output* SoundOutput, DWORD ByteToLock, DWORD BytesToWrite, game_sound_output_buffer* SourceBuffer)
{
    // Buffer structure
    // sample        sample        sample        sample
    // int16  int16  int16  int16  int16  int16  int16  int16 
    // [left  right] [left  right] [left  right] [left  right]
    VOID* Region1;
    DWORD Region1Size;
    VOID* Region2;
    DWORD Region2Size;


    if (!SUCCEEDED(GlobalSoundBuffer->Lock(ByteToLock, BytesToWrite, &Region1, &Region1Size, &Region2, &Region2Size, 0)))
        return;

    // TODO: assert that region1size and region1size is an even multiple of byestpersample

    // TODO: combine these two loops
    DWORD Region1SampleCount = Region1Size / SoundOutput->BytesPerSample;
    int16* DestinationSample = (int16*)Region1;
    int16* SourceSample = SourceBuffer->Samples;
    for (uint64 i = 0; i < Region1SampleCount; i++)
    {
        *DestinationSample++ = *SourceSample++;
        *DestinationSample++ = *SourceSample++;
        (SoundOutput->RunningSampleIndex)++;
    }

    DWORD Region2SampleCount = Region2Size / SoundOutput->BytesPerSample;
    DestinationSample = (int16*)Region2;

    for (uint64 i = 0; i < Region2SampleCount; i++)
    {
        *DestinationSample++ = *SourceSample++; // post-increment (nice ovo)
        *DestinationSample++ = *SourceSample++;
        (SoundOutput->RunningSampleIndex)++;
    }

    GlobalSoundBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);
}


internal void win32_ResizeDIBSection(win32_offscreen_buffer* Buffer, int Width, int Height) // resizes backbuffer
{
    if (Buffer->Memory)
    {
        VirtualFree(Buffer->Memory, null, MEM_RELEASE);
        //free(BitmapMemory);
    }

    Buffer->Width = Width;
    Buffer->Height = Height;
    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height; // negative height makes the image top down instead of bottom up.
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;

    // Windows, gimme memory. :)
    Buffer->Memory = VirtualAlloc(0, 4 * Width * Height, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE); // Rounds up to page size on its own (4k bytes by default).
    //BitmapMemory = malloc(4 * Width * Height);
    Buffer->BytesPerPixel = 4;
    Buffer->Pitch = Width * Buffer->BytesPerPixel;
    // TODO: Probably clear to black
}

void Win32DebugDrawLine(win32_offscreen_buffer* Backbuffer, int X, int Top, int Bottom, uint32 Color)
{
    uint8* Pixel = (uint8*)Backbuffer->Memory + X * Backbuffer->BytesPerPixel + Top * Backbuffer->Pitch;
    for (int i = Top; i < Bottom; i++)
    {
        *(uint32*)Pixel = Color;
        Pixel += Backbuffer->Pitch;
    }
}

internal win32_window_dimensions GetWindowDimension(HWND Window)
{
    win32_window_dimensions res;
    RECT windowrect;
    GetClientRect(Window, &windowrect);
    res.Width = windowrect.right - windowrect.left;
    res.Height = windowrect.bottom - windowrect.top;
    return res;
}

internal void win32_CopyBufferToWindow(const win32_offscreen_buffer* Buffer, HDC Device, int Width, int Height)
{
    StretchDIBits(Device,
        0, 0, Width, Height,
        0, 0, Buffer->Width, Buffer->Height,
        Buffer->Memory, &Buffer->Info, DIB_RGB_COLORS, SRCCOPY);
}


float clamp(float a, float b, float c)
{
    return min(max(a, b), c);
}
float MoveTowards(float current, float target, float distance)
{
    return current + clamp(target - current, -distance, distance);
}
internal float Win32ProcessStick(int value, int deadzone)
{
    return (float)MoveTowards(value, 0, deadzone) / (float)(((value > 0) ? 32767 : 32768) - deadzone);
}

enum Events
{
    Size = WM_SIZE,
    Destroy = WM_DESTROY,
    Close = WM_CLOSE,
    Active = WM_ACTIVATEAPP,
    Paint = WM_PAINT
};
static bool Focused = true;
internal LRESULT MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;
    switch ((Events)Message)
    {
    case Size:
    {
        print("WM_SIZE\n");
        win32_window_dimensions dim = GetWindowDimension(Window);
        win32_ResizeDIBSection(&GlobalBackbuffer, dim.Width, dim.Height);

        Buffer.Memory = GlobalBackbuffer.Memory;
        Buffer.Width = GlobalBackbuffer.Width;
        Buffer.Height = GlobalBackbuffer.Height;
        Buffer.BytesPerPixel = 4;
        Buffer.Pitch = GlobalBackbuffer.Width * 4;
        Buffer.Layout = ChannelLayout_ARGB;
        if (GameCode.UpdateAndRender && Buffer.Width > 0 && Buffer.Height > 0)
            GameCode.UpdateAndRender(0, &GameMemory, &Input, &Buffer, &SoundBuffer);

    } break;
    case Destroy:
    {
        print("WM_DESTROY\n");
        //PostQuitMessage(0);
        GlobalRunning = false;

    } break;
    case Close:
    {
        print("WM_CLOSE\n");
        //PostQuitMessage(0);
        GlobalRunning = false;
    } break;
    case Active:
    {
        bool Entered = WParam;
        //Focused = WParam;
        if (Entered)
        {
            print("Entered\n");
        }
        else
        {
            print("Exited\n");
        }
    } break;
    case Paint:
    {
        print("WM_PAINT\n");
        PAINTSTRUCT Paint;
        HDC DeviceContext = BeginPaint(Window, &Paint);
        win32_window_dimensions dim = GetWindowDimension(Window);
        win32_CopyBufferToWindow(&GlobalBackbuffer, DeviceContext, dim.Width, dim.Height);
        ReleaseDC(Window, DeviceContext);
        EndPaint(Window, &Paint);
    } break;
    default:
    {
        Result = DefWindowProcA(Window, Message, WParam, LParam);
    } break;
    }
    return Result;
}

LARGE_INTEGER Win32GetWallClock()
{
    LARGE_INTEGER Result;
    QueryPerformanceCounter(&Result);
    return Result;
}

float Win32GetSecondsElapsed(LARGE_INTEGER Start, LARGE_INTEGER End)
{
    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);
    float Result = (float)(End.QuadPart - Start.QuadPart) / (float)Frequency.QuadPart;
    return Result;
}
int64 PlatformTime()
{
    LARGE_INTEGER Frequency;
    LARGE_INTEGER Result;
    QueryPerformanceFrequency(&Frequency);
    QueryPerformanceCounter(&Result);
    return Result.QuadPart;
}

bool ReloadFilesChanged()
{
    bool FileChanged = false;
    WIN32_FIND_DATAA data;
    HANDLE hFind = FindFirstFile("*", &data);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        for (int i = 0; i < 100; i++)
        {
            //FILETIME File1;
            //FILETIME File2;
            //FILETIME File3;
            //HANDLE FileHandle = CreateFileA(data.cFileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
            //GetFileTime(FileHandle, &File1, &File2, &File3);
            //CloseHandle(FileHandle);

            int64 date = ((int64)data.ftLastWriteTime.dwHighDateTime << 32) + data.ftLastWriteTime.dwLowDateTime;

            if (ends_with(data.cFileName, ".cpp") || ends_with(data.cFileName, ".h"))
            {
                if (date != times[i])
                {
                    FileChanged = true;
                }
            }
            times[i] = date;

            bool found = FindNextFileA(hFind, &data);
            if (!found)
                break;
        }
        FindClose(hFind);
    }
    return FileChanged;
}

void sighandler(int signum)
{
    //switch (signum)
    //{
    //    case SIGSEGV:
    //    {
    //        MessageBox(0, "Segmentation Violation, Invalid access to storage attempted.", "Program crashed.", 0);
    //        break;
    //    }
    //    case SIGFPE:
    //    {
    //        MessageBox(0, "Floating-Point Exception, Erroneous arithmetic operation attempted.", "Program crashed.", 0);
    //        break;
    //    }
    //}
}

// https://stackoverflow.com/questions/41533158/create-32-bit-color-icon-programmatically
static HICON CreateIconFromBytes(HDC DC, int width, int height, uint32* bytes)
{
    HICON hIcon = NULL;

    ICONINFO iconInfo = {
        TRUE, // fIcon, set to true if this is an icon, set to false if this is a cursor
        NULL, // xHotspot, set to null for icons
        NULL, // yHotspot, set to null for icons
        NULL, // Monochrome bitmap mask, set to null initially
        NULL  // Color bitmap mask, set to null initially
    };

    uint32* rawBitmap = new uint32[width * height];

    ULONG uWidth = (ULONG)width;
    ULONG uHeight = (ULONG)height;
    uint32* bitmapPtr = rawBitmap;
    for (ULONG y = 0; y < uHeight; y++) {
        for (ULONG x = 0; x < uWidth; x++) {
            // Bytes are expected to be in RGB order (8 bits each)
            // Swap G and B bytes, so that it is in BGR order for windows
            uint32 byte = bytes[x + y * width];
            uint8 A = (byte & 0xff000000) >> 24;
            uint8 R = (byte & 0xff0000) >> 16;
            uint8 G = (byte & 0xff00) >> 8;
            uint8 B = (byte & 0xff);
            *bitmapPtr = (A << 24) | (R << 16) | (G << 8) | B;
            bitmapPtr++;
        }
    }

    iconInfo.hbmColor = CreateBitmap(width, height, 1, 32, rawBitmap);
    if (iconInfo.hbmColor) {
        iconInfo.hbmMask = CreateCompatibleBitmap(DC, width, height);
        if (iconInfo.hbmMask) {
            hIcon = CreateIconIndirect(&iconInfo);
            if (hIcon == NULL) {
                //Log::Warning("Failed to create icon.");
            }
            DeleteObject(iconInfo.hbmMask);
        }
        else {
            //Log::Warning("Failed to create color mask.");
        }
        DeleteObject(iconInfo.hbmColor);
    }
    else {
        //Log::Warning("Failed to create bitmap mask.");
    }

    delete[] rawBitmap;

    return hIcon;
}

void TrackMouse(HWND hwnd)
{
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(TRACKMOUSEEVENT);
    tme.dwFlags = TME_HOVER | TME_LEAVE; //Type of events to track & trigger.
    tme.dwHoverTime = 1; //How long the mouse has to be in the window to trigger a hover event.
    tme.hwndTrack = hwnd;
    TrackMouseEvent(&tme);
}
static bool Tracking = false;

// entry point
int WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommandLine, int ShowCode)
{

    //signal(SIGSEGV, sighandler);
    //signal(SIGFPE, sighandler);

    int width = 1920 / 2;
    int height = 1080 / 2;
    const char* Title = "Thingy Win32";
    const char* IconPath = "favicon.tga";

    // Initialize game memory
#define FramesOfAudioLatency 4
#define MonitorRefreshHz 60
#define GameUpdateHz 60
    float TargetSecondsElapsedPerFrame = 1.0f / (float)GameUpdateHz;

#if QUOTE_INTERNAL
    LPVOID BaseAddress = (LPVOID)Terabytes(2);
    win32_debug_time_marker DebugTimeMarker[GameUpdateHz / 2] = {};
    int DebugTimeMarkerIndex = 0;
#else
    LPVOID BaseAddress = 0;
#endif
    //GameMemory.PermanentStorageSize = Megabytes(64);
    //GameMemory.TransientStorageSize = Gigabytes((uint64)1);
    GameMemory.PermanentStorageSize = Megabytes(9);
    GameMemory.TransientStorageSize = Megabytes(3);
    uint64 TotalSize = GameMemory.PermanentStorageSize + GameMemory.TransientStorageSize;
    // Set the base addresses it will allocate to so that they are the same every time. this allows for awesome debugging
    GameMemory.PermanentStorage = (uint8*)VirtualAlloc(BaseAddress, (size_t)TotalSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    GameMemory.TransientStorage = (uint8*)GameMemory.PermanentStorage + GameMemory.PermanentStorageSize;

    GameMemory.PlatformReadImageIntoMemory = &PlatformReadImageIntoMemory;
    GameMemory.PlatformPrint = &PlatformPrint;
    GameMemory.PlatformTime = &PlatformTime;

    WNDCLASS WindowClass = {};
    // TODO: Check if hdredraw and vredraw are needed
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = (WNDPROC)MainWindowCallback;
    WindowClass.hInstance = GetModuleHandle(0);
    WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WindowClass.lpszClassName = "HelloWindowClass";

    // Make icon
    // Small enough to fit on the stack. (1k)
    char StackImage[1024];
    uint32* image = PlatformReadImageIntoMemory(StackImage, IconPath);
    HICON hIcon3 = CreateIconFromBytes(CreateCompatibleDC(NULL), 16, 16, image);
    WindowClass.hIcon = hIcon3;

    if (!RegisterClass(&WindowClass))
        return 0;
    
    // Set windows scheduler granuality to 1ms so our sleep() can be more granular
    bool32 SleepIsGranular = timeBeginPeriod(1) == TIMERR_NOERROR;



    DWORD Style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    RECT windowrect;
    windowrect.left = 0;
    windowrect.top = 0;
    windowrect.right = width;
    windowrect.bottom = height;
    AdjustWindowRect(&windowrect, Style, false);
    int WindowWidth = windowrect.right - windowrect.left;
    int WindowHeight = windowrect.bottom - windowrect.top;
    HWND Window = CreateWindowEx(0, WindowClass.lpszClassName, Title, Style, CW_USEDEFAULT, CW_USEDEFAULT, WindowWidth, WindowHeight, 0, 0, Instance, null);
    if (!Window)
        return 0;

    
    Win32LoadXInput();



    LARGE_INTEGER ticks = Win32GetWallClock();
    LARGE_INTEGER LastCounter = ticks;


    // Initialize sound output
    win32_sound_output SoundOutput = {};
    SoundOutput.SamplesPerSecond = 48000;
    SoundOutput.BytesPerSample = sizeof(int16) * 2;
    SoundOutput.BufferSize = (SoundOutput.SamplesPerSecond * SoundOutput.BytesPerSample);
    SoundOutput.RunningSampleIndex = 0;
    SoundOutput.LatencySampleCount = FramesOfAudioLatency * (SoundOutput.SamplesPerSecond / GameUpdateHz); // if our framerate is below 15 we'll start skipping / stuttering the sound.
    int16* Samples = (int16*)VirtualAlloc(0, 48000 * 2 * sizeof(int16), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    win32_InitDSound(Window, SoundOutput.SamplesPerSecond, SoundOutput.BufferSize);
    win32_ClearSoundBuffer(&SoundOutput);
    GlobalSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);


    if (Samples && GameMemory.PermanentStorage && GameMemory.TransientStorage) {}
    else
        return 0; // failed to accloate memory. no game this time.

    DWORD LastPlayCursor = 0;
    bool32 SoundIsValid = false;

    int64 CycleCount = 0;
    float Time = 0;
    
    GameCode = Win32LoadGameCode();

    while (GlobalRunning)
    {
        // Only update input if the window is focused.
        

            // Deal with windows messages
            MSG Message;
            while (PeekMessage(&Message, null, null, null, PM_REMOVE))
            {
                switch (Message.message)
                {
                case WM_QUIT:
                    GlobalRunning = false;
                    break;
                    
                case WM_SYSKEYDOWN:
                case WM_SYSKEYUP:
                case WM_KEYDOWN:
                case WM_KEYUP:
                {
                    if (!Focused) break;
                    bool32 WasDown = (Message.lParam & (1 << 30)) != 0;
                    bool32 IsDown = (Message.lParam & (1 << 31)) == 0;

                    if (WasDown != IsDown)
                    {
                        uint32 VKCode = (uint32)Message.wParam;
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
                    // Alt + F4 close
                    bool32 AltKeyIsDown = (Message.lParam & (1 << 29)) != 0;
                    if (AltKeyIsDown && Message.wParam == VK_F4)
                    {
                        GlobalRunning = false;
                    }

                    // Esc key close
                    if (Message.wParam == VK_ESCAPE)
                    {
                        //GlobalRunning = false;
                    }

                } break;
                case WM_LBUTTONDOWN:
                {
                    if (!Focused) break;
                    Input.MouseLeft = true;
                } break;
                case WM_LBUTTONUP:
                {
                    if (!Focused) break;
                    Input.MouseLeft = false;
                } break;
                case WM_RBUTTONDOWN:
                {
                    if (!Focused) break;
                    Input.MouseRight = true;
                } break;
                case WM_RBUTTONUP:
                {
                    if (!Focused) break;
                    Input.MouseRight = false;
                } break;
                case WM_MBUTTONDOWN:
                {
                    if (!Focused) break;
                    Input.MouseMiddle = true;
                } break;
                case WM_MBUTTONUP:
                {
                    if (!Focused) break;
                    Input.MouseMiddle = false;
                } break;
                case WM_MOUSEWHEEL:
                {
                    if (!Focused) break;
                    Input.MouseZ += GET_WHEEL_DELTA_WPARAM(Message.wParam) / 120;
                } break;
                case WM_MOUSEMOVE:
                {
                    //if (!Focused) break;
                    //if (!Tracking)
                    //{
                    //    TrackMouse(Window);
                    //    Tracking = true;
                    //}
                    //int16 xPos = (int16)(Message.lParam);
                    //int16 yPos = (int16)(Message.lParam >> 16);
                    //Input.MouseX = (int32)xPos;
                    //Input.MouseY = (int32)yPos;
                } break;
                case WM_MOUSELEAVE:
                {
                    //if (!Focused) break;
                    //Tracking = false;

                } break;
                case WM_SIZING:
                {
                } break;
                default:
                {
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }
                }
            }

            if (Focused)
            {
                RECT WindowRect;
                GetWindowRect(Window, &WindowRect);
                POINT CursorPoint;
                GetCursorPos(&CursorPoint);
                int32 TempMouseX = CursorPoint.x - WindowRect.left + windowrect.left;
                int32 TempMouseY = CursorPoint.y - WindowRect.top + windowrect.top;
                InputUpdateInt(&Input.StateMouseX, TempMouseX);
                InputUpdateInt(&Input.StateMouseY, TempMouseY);
                //// If the cursor is outside of the window.
                //if (TempMouseX < 0 || TempMouseX > width || TempMouseY < 0 || TempMouseY > height)
                //{
                //    int32 c = Input.MouseX;
                //    int32 d = Input.MouseY;
                //    int32 a = TempMouseX - Input.MouseX;
                //    int32 b = TempMouseY - Input.MouseY;
                //    if (a != 0 && b != 0)
                //    {
                //        int32 LeftHitX = 0;
                //        int32 LeftHitY = d - ((b * c) / a);
                //        int32 BottomHitX = c - ((a * d) / b);
                //        int32 BottomHitY = 0;
                //        int32 RightHitX = width;
                //        int32 RightHitY = d - ((b * (c - width)) / a);
                //        int32 TopHitX = c - ((a * (d - height)) / b);
                //        int32 TopHitY = height;
                //        
                //        int DistanceToLeft = Input.MouseX;
                //        int DistanceToBottom = Input.MouseY;
                //        int DistanceToRight = width - Input.MouseX;
                //        int DistanceToTop = height - Input.MouseY;
                //        
                //        int SmallestNumber = min(min(min(DistanceToLeft, DistanceToBottom), DistanceToRight), DistanceToTop);
                //        if (SmallestNumber == DistanceToLeft)
                //        {
                //            Input.MouseX = LeftHitX;
                //            Input.MouseY = LeftHitY;
                //        }
                //        else if (SmallestNumber == DistanceToBottom)
                //        {
                //            Input.MouseX = BottomHitX;
                //            Input.MouseY = BottomHitY;
                //        }
                //        else if (SmallestNumber == DistanceToRight)
                //        {
                //            Input.MouseX = RightHitX;
                //            Input.MouseY = RightHitY;
                //        }
                //        else if (SmallestNumber == DistanceToTop)
                //        {
                //            Input.MouseX = TopHitX;
                //            Input.MouseY = TopHitY;
                //        }
                //        InputUpdateInt(&Input.StateMouseX, Input.MouseX);
                //        InputUpdateInt(&Input.StateMouseY, Input.MouseY);
                //    }
                //}
                //else
                //{
                //    InputUpdateInt(&Input.StateMouseX, TempMouseX);
                //    InputUpdateInt(&Input.StateMouseY, TempMouseY);
                //}


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

            int MaxControllerCount = XUSER_MAX_COUNT;
            if (MaxControllerCount > 4)
                MaxControllerCount = 4;

            // Get input
            for (int i = 0; i < MaxControllerCount; i++)
            {
                XINPUT_STATE ControllerState;
                if (XInputGetState(i, &ControllerState) == ERROR_SUCCESS)
                {
                    // controller
                    XINPUT_GAMEPAD* Gamepad = &ControllerState.Gamepad;

                    InputUpdateFloat(&Input.Controllers[i].StateLeftStickX, Win32ProcessStick(Gamepad->sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE));
                    InputUpdateFloat(&Input.Controllers[i].StateLeftStickY, Win32ProcessStick(Gamepad->sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE));
                    InputUpdateFloat(&Input.Controllers[i].StateRightStickX, Win32ProcessStick(Gamepad->sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE));
                    InputUpdateFloat(&Input.Controllers[i].StateRightStickY, Win32ProcessStick(Gamepad->sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE));

                    InputUpdateButton(&Input.Controllers[i].StateA, Gamepad->wButtons & XINPUT_GAMEPAD_A);
                    InputUpdateButton(&Input.Controllers[i].StateB, Gamepad->wButtons & XINPUT_GAMEPAD_B);
                    InputUpdateButton(&Input.Controllers[i].StateX, Gamepad->wButtons & XINPUT_GAMEPAD_X);
                    InputUpdateButton(&Input.Controllers[i].StateY, Gamepad->wButtons & XINPUT_GAMEPAD_Y);

                    InputUpdateButton(&Input.Controllers[i].StateUp, Gamepad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
                    InputUpdateButton(&Input.Controllers[i].StateDown, Gamepad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                    InputUpdateButton(&Input.Controllers[i].StateLeft, Gamepad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                    InputUpdateButton(&Input.Controllers[i].StateRight, Gamepad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);

                    InputUpdateButton(&Input.Controllers[i].StateLeftBumper, Gamepad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
                    InputUpdateButton(&Input.Controllers[i].StateRightBumper, Gamepad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
                    InputUpdateButton(&Input.Controllers[i].StateLeftTrigger, Gamepad->bLeftTrigger);
                    InputUpdateButton(&Input.Controllers[i].StateRightTrigger, Gamepad->bRightTrigger);

                    InputUpdateButton(&Input.Controllers[i].StateStart, Gamepad->wButtons & XINPUT_GAMEPAD_START);
                    InputUpdateButton(&Input.Controllers[i].StateBack, Gamepad->wButtons & XINPUT_GAMEPAD_BACK);
                }
                else
                {
                    // no controller
                }
            }
        }

        DWORD ByteToLock = 0;
        DWORD TargetCursor = 0;
        DWORD BytesToWrite = 0;
        if (SoundIsValid)
        {
            TargetCursor = (LastPlayCursor + (SoundOutput.LatencySampleCount * SoundOutput.BytesPerSample)) % SoundOutput.BufferSize;

            ByteToLock = ((SoundOutput.RunningSampleIndex) * SoundOutput.BytesPerSample) % SoundOutput.BufferSize; // where we are in the buffer this frame
            // we want to fill in the "wwww" region with our data so that the write cursor catches up behind the play cursor.
            // There are two possible situations, one where the write cursor is ahead of the play cursor, and one where it's behind.
            // This is how we handle those two scenarios:

            if (ByteToLock > TargetCursor)
            {
                //[wwwwwww--------wwwwwww]
                //0       ^      ^       21
                //      Play   Write

                BytesToWrite = SoundOutput.BufferSize - ByteToLock;
                BytesToWrite += TargetCursor;
            }
            else
            {
                //[-------wwwwwww-------]
                //0       ^      ^      21
                //      Write   Play   
                BytesToWrite = TargetCursor - ByteToLock;
            }
        }


        // we pay for an extra buffer copy here to make it simpler for the game, it just needs to fill an entire buffer
        SoundBuffer.SamplesPerSecond = SoundOutput.SamplesPerSecond;
        SoundBuffer.SampleCount = BytesToWrite / SoundOutput.BytesPerSample;
        SoundBuffer.Samples = Samples;

        Buffer.Memory = GlobalBackbuffer.Memory;
        Buffer.Width = GlobalBackbuffer.Width;
        Buffer.Height = GlobalBackbuffer.Height;
        Buffer.BytesPerPixel = 4;
        Buffer.Pitch = GlobalBackbuffer.Width * 4;
        Buffer.Layout = ChannelLayout_ARGB;

        if(GameCode.UpdateAndRender && Buffer.Width > 0 && Buffer.Height > 0)
            GameCode.UpdateAndRender(0, &GameMemory, &Input, &Buffer, &SoundBuffer);

        if (FileExists("build.bat"))
        {
            bool FileChanged = ReloadFilesChanged();

            if (FileChanged)
            {
                //print("Recompile");
                Win32UnloadGameCode(&GameCode);

                system("build_clang.bat dll_only");
                //system("build.bat dll_only");
                GameCode = Win32LoadGameCode();
                ReloadFilesChanged();
            }
        }


        if (SoundIsValid)
        {
            win32_FillSoundBuffer(&SoundOutput, ByteToLock, BytesToWrite, &SoundBuffer);
        }


        LARGE_INTEGER WorkCounter = Win32GetWallClock();
        float WorkSecondsElapsed = Win32GetSecondsElapsed(LastCounter, WorkCounter);
        // Time stuff
        if (WorkSecondsElapsed < TargetSecondsElapsedPerFrame)
        {
            while (WorkSecondsElapsed < TargetSecondsElapsedPerFrame)
            {
                if (SleepIsGranular)
                {
                    DWORD SleepMS = (DWORD)((TargetSecondsElapsedPerFrame - WorkSecondsElapsed) * 1000.0f);
                    if (SleepMS > 0)
                        Sleep(SleepMS);
                }
                WorkSecondsElapsed = Win32GetSecondsElapsed(LastCounter, Win32GetWallClock());
            }
        }
        else
        {
            //print("Missed Framerate Target");
            // Missed a frame. destroy everything.
            //Assert(0);
        }

        Input.DeltaTime = WorkSecondsElapsed;

        LARGE_INTEGER EndCounter = Win32GetWallClock();
        WorkSecondsElapsed = Win32GetSecondsElapsed(LastCounter, Win32GetWallClock());

        LastCounter = EndCounter;

        // Copy rendered image to screen
        HDC DeviceContext = GetDC(Window);
        win32_window_dimensions dim = GetWindowDimension(Window);
        win32_CopyBufferToWindow(&GlobalBackbuffer, DeviceContext, dim.Width, dim.Height);
        ReleaseDC(Window, DeviceContext);

        DWORD PlayCursor;
        DWORD WriteCursor;
        if (SUCCEEDED(GlobalSoundBuffer->GetCurrentPosition(&PlayCursor, &WriteCursor)))
        {
            if (!SoundIsValid)
            {
                SoundOutput.RunningSampleIndex = WriteCursor / SoundOutput.BytesPerSample;
            }
            SoundIsValid = true;
            LastPlayCursor = PlayCursor;
        }
        else
        {
            SoundIsValid = false;
        }

    }

    //MessageBoxA(null, "Welcome back commander.", "Title", MB_OK | MB_ICONINFORMATION);
    return 1337;
}
