
// Win32 platform layer.
// Since it's the platform layer, using native libraries and functions here is fine.

#include "quote_platform.cpp"

typedef uint32 memory_index;

#include <windows.h>
#include <stdio.h>
#include <xinput.h>
#include <Dinput.h>
#include <dsound.h>
#include <assert.h>
#include "CRTDBG.h"
#include "debugapi.h"
#include "math.h"
#include <exception>
#include <string>
#include "immintrin.h"

void win32_PlatformPrint(const char* string)
{
    char str[100];
    char* a = str;
    strcpy(a, string);
    strcat(a, "\n");
    OutputDebugStringA(a);
}

int ends_with(const char* str, const char* suffix)
{
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    return (str_len >= suffix_len) && (!memcmp(str + str_len - suffix_len, suffix, suffix_len));
}

struct read_file_result
{
    uint32 ContentsSize;
    void* Contents;
};

read_file_result win32_PlatformReadFile(const char* Filename)
{
    read_file_result Result = {};

#if QUOTE_static
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
    Result.Contents = (uint8*)VirtualAlloc(BaseAddress, FileSize32, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
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

bool win32_PlatformReadFile(const char* filename, void* destination)
{
    read_file_result result = win32_PlatformReadFile(filename);
    for (int i = 0; i < result.ContentsSize; i++)
    {
        ((uint8*)destination)[i] = ((uint8*)result.Contents)[i];
    }
    return true;
}

void win32_PlatformFreeFileMemory(read_file_result memory)
{
    VirtualFree(memory.Contents, null, MEM_RELEASE);
}

//bool win32_PlatformWriteFile(const char* Filename, read_file_result memory)
//{
//}
bool win32_PlatformWriteFile(const char* Filename, void* Contents, uint32 ContentsSize)
{
    HANDLE FileHandle = CreateFileA(Filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if (FileHandle == INVALID_HANDLE_VALUE)// failed to open file
        return 0;

    DWORD BytesWritten = 0;
    if (!WriteFile(FileHandle, Contents, ContentsSize, &BytesWritten, 0)) // failed to write file
        return 0;

    if (BytesWritten != ContentsSize) // we didn't end up writing the entire file
        return 0;

    CloseHandle(FileHandle);

    return 1;

    //return win32_PlatformWriteFile(Filename, { ContentsSize, Contents });
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
uint32* PlatformReadImageIntoMemory_local(void* Destination, const char* File)
{
    // TODO: Make sure the image is not greyscale omfg

    read_file_result ImageFile = win32_PlatformReadFile(File);

    if (!ImageFile.Contents)
        return 0;

    TGA_HEADER* header = (TGA_HEADER*)ImageFile.Contents;
    
    int Width = header->width;
    int Height = header->height;
    bool flipUpDown = header->descriptor & 1 << 5;
    bool flipLeftRight = header->descriptor & 1 << 4;

    // game wants everything flipped upside-down
    flipUpDown = !flipUpDown;

    if (flipUpDown && flipLeftRight)
    {
        int i = 0;
        for (int y = Height - 1; y >= 0; y--)
        {
            for (int x = Width - 1; x >= 0; x--)
            {
                int j = x + y * Width;
                ((uint8*)Destination)[i * 4 + 0] = *((uint8*)header + sizeof(TGA_HEADER) + j * 4 + 0);
                ((uint8*)Destination)[i * 4 + 1] = *((uint8*)header + sizeof(TGA_HEADER) + j * 4 + 1);
                ((uint8*)Destination)[i * 4 + 2] = *((uint8*)header + sizeof(TGA_HEADER) + j * 4 + 2);
                ((uint8*)Destination)[i * 4 + 3] = *((uint8*)header + sizeof(TGA_HEADER) + j * 4 + 3);
                i++;
            }
        }
    }
    else if (flipUpDown)
    {
        int i = 0;
        for (int y = Height - 1; y >= 0; y--)
        {
            for (int x = 0; x < Width; x++)
            {
                //int i = x + y * tga->width;
                int j = x + y * Width;
                ((uint8*)Destination)[i * 4 + 0] = *((uint8*)header + sizeof(TGA_HEADER) + j * 4 + 0);
                ((uint8*)Destination)[i * 4 + 1] = *((uint8*)header + sizeof(TGA_HEADER) + j * 4 + 1);
                ((uint8*)Destination)[i * 4 + 2] = *((uint8*)header + sizeof(TGA_HEADER) + j * 4 + 2);
                ((uint8*)Destination)[i * 4 + 3] = *((uint8*)header + sizeof(TGA_HEADER) + j * 4 + 3);
                i++;
            }
        }
    }
    else if (flipLeftRight)
    {
        int i = 0;
        for (int y = 0; y < Height; y++)
        {
            for (int x = Width - 1; x >= 0; x--)
            {
                int j = x + y * Width;
                ((uint8*)Destination)[i * 4 + 0] = *((uint8*)header + sizeof(TGA_HEADER) + j * 4 + 0);
                ((uint8*)Destination)[i * 4 + 1] = *((uint8*)header + sizeof(TGA_HEADER) + j * 4 + 1);
                ((uint8*)Destination)[i * 4 + 2] = *((uint8*)header + sizeof(TGA_HEADER) + j * 4 + 2);
                ((uint8*)Destination)[i * 4 + 3] = *((uint8*)header + sizeof(TGA_HEADER) + j * 4 + 3);
                i++;
            }
        }
    }
    else
    {
        int i = 0;
        for (int y = 0; y < Height; y++)
        {
            for (int x = 0; x < Width; x++)
            {
                int j = x + y * Width;
                ((uint8*)Destination)[i * 4 + 0] = *((uint8*)header + sizeof(TGA_HEADER) + j * 4 + 0);
                ((uint8*)Destination)[i * 4 + 1] = *((uint8*)header + sizeof(TGA_HEADER) + j * 4 + 1);
                ((uint8*)Destination)[i * 4 + 2] = *((uint8*)header + sizeof(TGA_HEADER) + j * 4 + 2);
                ((uint8*)Destination)[i * 4 + 3] = *((uint8*)header + sizeof(TGA_HEADER) + j * 4 + 3);
                i++;
            }
        }
    }

    win32_PlatformFreeFileMemory(ImageFile);

    return (uint32*)Destination;
}
void win32_PlatformReadImageIntoMemory(void* Destination, const char* File)
{
    PlatformReadImageIntoMemory_local(Destination, File);
}

typedef struct WAV_HEADER
{
    // RIFF Header
    uint8 riff_header[4]; // Contains "RIFF"
    int32 wav_size; // Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
    uint8 wave_header[4]; // Contains "WAVE"

    // Format Header
    uint8 fmt_header[4]; // Contains "fmt " (includes trailing space)
    int32 fmt_chunk_size; // Should be 16 for PCM
    int16 audio_format; // Should be 1 for PCM. 3 for IEEE Float
    int16 num_channels;
    int32 sample_rate;
    int32 byte_rate; // Number of bytes per second. sample_rate * num_channels * Bytes Per Sample
    int16 sample_alignment; // num_channels * Bytes Per Sample
    int16 bit_depth; // Number of bits per sample

    // Data
    uint8 data_header[4]; // Contains "data"
    int32 data_bytes; // Number of bytes in data. Number of samples * num_channels * sample byte size
    //uint8 bytes; // Remainder of wave file is bytes
} WAV_HEADER;


// TODO: Change the game sound to be 44100 hz instead of 2205Hz.
void win32_PlatformReadSoundIntoMemory(void* Destination, const char* File)
{
    read_file_result file = win32_PlatformReadFile(File);

    if (!file.Contents)
        return;

    WAV_HEADER* header = (WAV_HEADER*)file.Contents;
    int16* data = (int16*)(((uint8*)file.Contents) + sizeof(WAV_HEADER));
    int32 sampleCount = header->data_bytes / 2; // divide by 2 because it's 16 bits per sample.

    // The game wants all sound in int16 [left, right, left, right, left, right]

    int16* src = (int16*)data;
    Sample* dest = (Sample*)Destination;


    if (header->num_channels == 1 && header->sample_rate == 22050) // sound effects
    {
        for (int i = 0; i < sampleCount; i++) // Number of mono samples
        {
            // Duplicate the sample to increase the rate to 44100 Hz
            dest[i * 2].left = src[i];
            dest[i * 2].right = src[i];
            dest[i * 2 + 1].left = src[i];
            dest[i * 2 + 1].right = src[i];
        }
    }
    else if (header->num_channels == 2 && header->sample_rate == 44100) // music
    {
        for (int i = 0; i < sampleCount / 2; i++)
        {
            dest[i].left = src[i * 2];
            dest[i].right = src[i * 2 + 1];
        }
    }
    else if (header->num_channels == 1 && header->sample_rate == 44100) // other sound effects
    {
        for (int i = 0; i < sampleCount / 2; i++)
        {
            dest[i].left = src[i];
            dest[i].right = src[i];
        }
    }
    win32_PlatformFreeFileMemory(file);
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
X_Input_Get_State* XInputGetState_ = XInputGetStateStub; // global pointer to the stub, this is the default, this pointer can then be overwritten with the real function if it's found.
#define XInputGetState XInputGetState_ // 

#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef X_INPUT_SET_STATE(X_Input_Set_State);
X_INPUT_SET_STATE(XInputSetStateStub) { return ERROR_DEVICE_NOT_CONNECTED; }
X_Input_Set_State* XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

HMODULE Win32GetXInputLibrary()
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

// Joystick stuff, because I happen to have an old Microsoft SideWinder with force feedback :)
LPDIRECTINPUT8 dinp;
LPDIRECTINPUTDEVICE8  lpdiJoystick;
LPDIRECTINPUTDEVICE8 dinJoystick;
bool triggerEffect;
ForceType triggerForceType;
HWND globalWindow;
void SetEffectLocal(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext, ForceType forceType)
{
    // Obtain an interface to the enumerated joystick.
    HRESULT hr = dinp->CreateDevice(pdidInstance->guidInstance, &dinJoystick, NULL);
    if (FAILED(hr))
        return;
    DIPROPDWORD dipdw;
    dipdw.diph.dwSize = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj = 0;
    dipdw.diph.dwHow = DIPH_DEVICE;
    dipdw.dwData = DIPROPAUTOCENTER_OFF;
    auto result = dinJoystick->SetProperty(DIPROP_AUTOCENTER, &dipdw.diph);

    dinJoystick->SetDataFormat(&c_dfDIJoystick2);

    HWND hWnd = *((HWND*)pContext);
    if (FAILED(hr = dinJoystick->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_BACKGROUND)))
        return;

    if (FAILED(hr = dinJoystick->Acquire()))
        return;

    LPDIRECTINPUTEFFECT lpdiEffect;  // receives pointer to created effect
    switch (forceType)
    {
    case ForceType_Nothing:
    {
        DIEFFECT diEffect = {};               // parameters for created effect
        DWORD    dwAxes[2] = { DIJOFS_X, DIJOFS_Y };
        LONG     lDirection[2] = { 18000, 0 };
        DICONSTANTFORCE diConstantForce;

        diConstantForce.lMagnitude = 0; // No force means the effect is off
        diEffect.dwStartDelay = 0;
        diEffect.dwSize = sizeof(DIEFFECT);
        diEffect.dwFlags = DIEFF_POLAR | DIEFF_OBJECTOFFSETS;
        diEffect.dwDuration = INFINITE;// (DWORD)(0.5 * DI_SECONDS);
        diEffect.dwSamplePeriod = 0;                 // = default 
        diEffect.dwGain = DI_FFNOMINALMAX;   // No scaling
        diEffect.dwTriggerButton = DIEB_NOTRIGGER;    // Not a button response
        diEffect.dwTriggerRepeatInterval = 0;         // Not applicable
        diEffect.cAxes = 2;
        diEffect.rgdwAxes = &dwAxes[0];
        diEffect.rglDirection = &lDirection[0];
        diEffect.lpEnvelope = NULL;
        diEffect.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
        diEffect.lpvTypeSpecificParams = &diConstantForce;

        if (FAILED(hr = dinJoystick->CreateEffect(GUID_ConstantForce, &diEffect, &lpdiEffect, NULL)))
            return;
    }break;
    case ForceType_SpringCenter:
    {
        DIEFFECT diEffect = {};               // parameters for created effect
        DWORD    dwAxes[2] = { DIJOFS_X, DIJOFS_Y };
        LONG     lDirection[2] = { 0, 0 };
        DICONDITION springForce = {};

        springForce.dwNegativeSaturation = 10000;
        springForce.dwPositiveSaturation = 10000;
        springForce.lNegativeCoefficient = 10000;
        springForce.lPositiveCoefficient = 10000;

        //diConstantForce.lMagnitude = DI_FFNOMINALMAX; // No force means the effect is off
        diEffect.dwStartDelay = 0;
        diEffect.dwSize = sizeof(DIEFFECT);
        diEffect.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
        diEffect.dwDuration = INFINITE;// (DWORD)(0.5 * DI_SECONDS);
        diEffect.dwSamplePeriod = 0;                 // = default 
        diEffect.dwGain = DI_FFNOMINALMAX;   // No scaling
        diEffect.dwTriggerButton = DIEB_NOTRIGGER;    // Not a button response
        diEffect.dwTriggerRepeatInterval = 0;         // Not applicable
        diEffect.cAxes = 2;
        diEffect.rgdwAxes = &dwAxes[0];
        diEffect.rglDirection = &lDirection[0];
        diEffect.lpEnvelope = NULL;
        diEffect.cbTypeSpecificParams = sizeof(DICONDITION);
        diEffect.lpvTypeSpecificParams = &springForce;

        if (FAILED(hr = dinJoystick->CreateEffect(GUID_Spring, &diEffect, &lpdiEffect, NULL)))
            return;
    }break;
    case ForceType_SpringAwayFromCenter:
    {
        DIEFFECT diEffect = {};               // parameters for created effect
        DWORD    dwAxes[2] = { DIJOFS_X, DIJOFS_Y };
        LONG     lDirection[2] = { 0, 0 };
        DICONDITION springForce = {};

        springForce.dwNegativeSaturation = 10000;
        springForce.dwPositiveSaturation = 10000;
        springForce.lNegativeCoefficient = -10000;
        springForce.lPositiveCoefficient = -10000;

        //diConstantForce.lMagnitude = DI_FFNOMINALMAX; // No force means the effect is off
        diEffect.dwStartDelay = 0;
        diEffect.dwSize = sizeof(DIEFFECT);
        diEffect.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
        diEffect.dwDuration = INFINITE;// (DWORD)(0.5 * DI_SECONDS);
        diEffect.dwSamplePeriod = 0;                 // = default 
        diEffect.dwGain = DI_FFNOMINALMAX;   // No scaling
        diEffect.dwTriggerButton = DIEB_NOTRIGGER;    // Not a button response
        diEffect.dwTriggerRepeatInterval = 0;         // Not applicable
        diEffect.cAxes = 2;
        diEffect.rgdwAxes = &dwAxes[0];
        diEffect.rglDirection = &lDirection[0];
        diEffect.lpEnvelope = NULL;
        diEffect.cbTypeSpecificParams = sizeof(DICONDITION);
        diEffect.lpvTypeSpecificParams = &springForce;

        if (FAILED(hr = dinJoystick->CreateEffect(GUID_Spring, &diEffect, &lpdiEffect, NULL)))
            return;
    }break;
    case ForceType_SpringForward:
    {
        DIEFFECT diEffect = {};               // parameters for created effect
        DWORD    dwAxes[2] = { DIJOFS_X, DIJOFS_Y };
        LONG     lDirection[2] = { 18000, 0 };
        DICONDITION springForce = {};

        springForce.dwNegativeSaturation = 10000;
        springForce.dwPositiveSaturation = 10000;
        springForce.lNegativeCoefficient = 10000;
        springForce.lPositiveCoefficient = 10000;
        springForce.lOffset = -10000;

        //diConstantForce.lMagnitude = DI_FFNOMINALMAX; // No force means the effect is off
        diEffect.dwStartDelay = 0;
        diEffect.dwSize = sizeof(DIEFFECT);
        diEffect.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
        diEffect.dwDuration = INFINITE;// (DWORD)(0.5 * DI_SECONDS);
        diEffect.dwSamplePeriod = 0;                 // = default 
        diEffect.dwGain = DI_FFNOMINALMAX;   // No scaling
        diEffect.dwTriggerButton = DIEB_NOTRIGGER;    // Not a button response
        diEffect.dwTriggerRepeatInterval = 0;         // Not applicable
        diEffect.cAxes = 2;
        diEffect.rgdwAxes = &dwAxes[0];
        diEffect.rglDirection = &lDirection[0];
        diEffect.lpEnvelope = NULL;
        diEffect.cbTypeSpecificParams = sizeof(DICONDITION);
        diEffect.lpvTypeSpecificParams = &springForce;

        if (FAILED(hr = dinJoystick->CreateEffect(GUID_Spring, &diEffect, &lpdiEffect, NULL)))
            return;
    }break;
    case ForceType_Sine:
    {
        DWORD rgdwAxes[2] = { DIJOFS_X, DIJOFS_Y };
        DIEFFECT g_sWheelRumbleConfig = {};

        g_sWheelRumbleConfig.dwStartDelay = 0;

        g_sWheelRumbleConfig.dwSize = sizeof(DIEFFECT);
        //g_sWheelRumbleConfig.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
        g_sWheelRumbleConfig.dwFlags = DIEFF_POLAR | DIEFF_OBJECTOFFSETS;
        g_sWheelRumbleConfig.dwDuration = INFINITE;
        g_sWheelRumbleConfig.dwSamplePeriod = 0;
        g_sWheelRumbleConfig.dwGain = DI_FFNOMINALMAX;
        g_sWheelRumbleConfig.dwTriggerButton = DIEB_NOTRIGGER;
        g_sWheelRumbleConfig.dwTriggerRepeatInterval = 0;
        g_sWheelRumbleConfig.cAxes = 2;
        g_sWheelRumbleConfig.rgdwAxes = &rgdwAxes[0];
        LONG rglDirection[2] = { 18000, 0 };
        g_sWheelRumbleConfig.rglDirection = rglDirection;

        g_sWheelRumbleConfig.lpEnvelope = 0;
        g_sWheelRumbleConfig.dwStartDelay = 0;

        DIPERIODIC pf = { 0, 0, 0, 0 };
        pf.dwMagnitude = 10000;
        //pf.dwPeriod = 2000;
        pf.lOffset = 0;
        int period = 1000;
        pf.dwPeriod = DI_SECONDS / 1000 * period;
        pf.dwPhase = 0;

        g_sWheelRumbleConfig.cbTypeSpecificParams = sizeof(DIPERIODIC);
        g_sWheelRumbleConfig.lpvTypeSpecificParams = &pf;

        if (FAILED(hr = dinJoystick->CreateEffect(GUID_Sine, &g_sWheelRumbleConfig, &lpdiEffect, nullptr)))
            return;
    }break;
    case ForceType_SineFast:
    {
        DWORD rgdwAxes[2] = { DIJOFS_X, DIJOFS_Y };
        DIEFFECT g_sWheelRumbleConfig = {};

        g_sWheelRumbleConfig.dwStartDelay = 0;

        g_sWheelRumbleConfig.dwSize = sizeof(DIEFFECT);
        //g_sWheelRumbleConfig.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
        g_sWheelRumbleConfig.dwFlags = DIEFF_POLAR | DIEFF_OBJECTOFFSETS;
        g_sWheelRumbleConfig.dwDuration = INFINITE;
        g_sWheelRumbleConfig.dwSamplePeriod = 0;
        g_sWheelRumbleConfig.dwGain = DI_FFNOMINALMAX;
        g_sWheelRumbleConfig.dwTriggerButton = DIEB_NOTRIGGER;
        g_sWheelRumbleConfig.dwTriggerRepeatInterval = 0;
        g_sWheelRumbleConfig.cAxes = 2;
        g_sWheelRumbleConfig.rgdwAxes = &rgdwAxes[0];
        LONG rglDirection[2] = { 18000, 0 };
        g_sWheelRumbleConfig.rglDirection = rglDirection;

        g_sWheelRumbleConfig.lpEnvelope = 0;
        g_sWheelRumbleConfig.dwStartDelay = 0;

        DIPERIODIC pf = { 0,0,0,0 };
        pf.dwMagnitude = 10000;
        //pf.dwPeriod = 2000;
        pf.lOffset = 0;
        int period = 100;
        pf.dwPeriod = DI_SECONDS / 1000 * period;
        pf.dwPhase = 0;

        g_sWheelRumbleConfig.cbTypeSpecificParams = sizeof(DIPERIODIC);
        g_sWheelRumbleConfig.lpvTypeSpecificParams = &pf;

        if (FAILED(hr = dinJoystick->CreateEffect(GUID_Sine, &g_sWheelRumbleConfig, &lpdiEffect, nullptr)))
            return;
    }break;
    case ForceType_ConstantForceForward:
    {
        DIEFFECT diEffect = {};               // parameters for created effect
        DWORD    dwAxes[2] = { DIJOFS_X, DIJOFS_Y };
        LONG     lDirection[2] = { 18000, 0 };
        DICONSTANTFORCE diConstantForce;

        diConstantForce.lMagnitude = DI_FFNOMINALMAX;   // Full force
        diEffect.dwStartDelay = 0;
        diEffect.dwSize = sizeof(DIEFFECT);
        diEffect.dwFlags = DIEFF_POLAR | DIEFF_OBJECTOFFSETS;
        diEffect.dwDuration = INFINITE;// (DWORD)(0.5 * DI_SECONDS);
        diEffect.dwSamplePeriod = 0;                 // = default 
        diEffect.dwGain = DI_FFNOMINALMAX;   // No scaling
        diEffect.dwTriggerButton = DIEB_NOTRIGGER;    // Not a button response
        diEffect.dwTriggerRepeatInterval = 0;         // Not applicable
        diEffect.cAxes = 2;
        diEffect.rgdwAxes = &dwAxes[0];
        diEffect.rglDirection = &lDirection[0];
        diEffect.lpEnvelope = NULL;
        diEffect.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
        diEffect.lpvTypeSpecificParams = &diConstantForce;

        if (FAILED(hr = dinJoystick->CreateEffect(GUID_ConstantForce, &diEffect, &lpdiEffect, NULL)))
            return;
    }break;
    case ForceType_Friction:
    {
        DIEFFECT diEffect = {};               // parameters for created effect
        DWORD    dwAxes[2] = { DIJOFS_X, DIJOFS_Y };
        LONG     lDirection[2] = { 0, 0 };
        DICONDITION springForce = {};

        springForce.dwNegativeSaturation = 10000;
        springForce.dwPositiveSaturation = 10000;
        springForce.lNegativeCoefficient = 10000;
        springForce.lPositiveCoefficient = 10000;

        //diConstantForce.lMagnitude = DI_FFNOMINALMAX; // No force means the effect is off
        diEffect.dwStartDelay = 0;
        diEffect.dwSize = sizeof(DIEFFECT);
        diEffect.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
        diEffect.dwDuration = INFINITE;// (DWORD)(0.5 * DI_SECONDS);
        diEffect.dwSamplePeriod = 0;                 // = default 
        diEffect.dwGain = DI_FFNOMINALMAX;   // No scaling
        diEffect.dwTriggerButton = DIEB_NOTRIGGER;    // Not a button response
        diEffect.dwTriggerRepeatInterval = 0;         // Not applicable
        diEffect.cAxes = 2;
        diEffect.rgdwAxes = &dwAxes[0];
        diEffect.rglDirection = &lDirection[0];
        diEffect.lpEnvelope = NULL;
        diEffect.cbTypeSpecificParams = sizeof(DICONDITION);
        diEffect.lpvTypeSpecificParams = &springForce;

        if (FAILED(hr = dinJoystick->CreateEffect(GUID_Friction, &diEffect, &lpdiEffect, NULL)))
            return;
    }break;
    case ForceType_Damper:
    {
        DIEFFECT diEffect = {};               // parameters for created effect
        DWORD    dwAxes[2] = { DIJOFS_X, DIJOFS_Y };
        LONG     lDirection[2] = { 0, 0 };
        DICONDITION springForce = {};

        springForce.dwNegativeSaturation = 10000;
        springForce.dwPositiveSaturation = 10000;
        springForce.lNegativeCoefficient = 10000;
        springForce.lPositiveCoefficient = 10000;

        //diConstantForce.lMagnitude = DI_FFNOMINALMAX; // No force means the effect is off
        diEffect.dwStartDelay = 0;
        diEffect.dwSize = sizeof(DIEFFECT);
        diEffect.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
        diEffect.dwDuration = INFINITE;// (DWORD)(0.5 * DI_SECONDS);
        diEffect.dwSamplePeriod = 0;                 // = default 
        diEffect.dwGain = DI_FFNOMINALMAX;   // No scaling
        diEffect.dwTriggerButton = DIEB_NOTRIGGER;    // Not a button response
        diEffect.dwTriggerRepeatInterval = 0;         // Not applicable
        diEffect.cAxes = 2;
        diEffect.rgdwAxes = &dwAxes[0];
        diEffect.rglDirection = &lDirection[0];
        diEffect.lpEnvelope = NULL;
        diEffect.cbTypeSpecificParams = sizeof(DICONDITION);
        diEffect.lpvTypeSpecificParams = &springForce;

        if (FAILED(hr = dinJoystick->CreateEffect(GUID_Damper, &diEffect, &lpdiEffect, NULL)))
            return;
    }break;
    case ForceType_Inertia:
    {
        DIEFFECT diEffect = {};               // parameters for created effect
        DWORD    dwAxes[2] = { DIJOFS_X, DIJOFS_Y };
        LONG     lDirection[2] = { 0, 0 };
        DICONDITION springForce = {};

        springForce.dwNegativeSaturation = 10000;
        springForce.dwPositiveSaturation = 10000;
        springForce.lNegativeCoefficient = 10000;
        springForce.lPositiveCoefficient = 10000;

        //diConstantForce.lMagnitude = DI_FFNOMINALMAX; // No force means the effect is off
        diEffect.dwStartDelay = 0;
        diEffect.dwSize = sizeof(DIEFFECT);
        diEffect.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
        diEffect.dwDuration = INFINITE;// (DWORD)(0.5 * DI_SECONDS);
        diEffect.dwSamplePeriod = 0;                 // = default 
        diEffect.dwGain = DI_FFNOMINALMAX;   // No scaling
        diEffect.dwTriggerButton = DIEB_NOTRIGGER;    // Not a button response
        diEffect.dwTriggerRepeatInterval = 0;         // Not applicable
        diEffect.cAxes = 2;
        diEffect.rgdwAxes = &dwAxes[0];
        diEffect.rglDirection = &lDirection[0];
        diEffect.lpEnvelope = NULL;
        diEffect.cbTypeSpecificParams = sizeof(DICONDITION);
        diEffect.lpvTypeSpecificParams = &springForce;

        if (FAILED(hr = dinJoystick->CreateEffect(GUID_Inertia, &diEffect, &lpdiEffect, NULL)))
            return;
    }break;
    }

    if (FAILED(hr = lpdiEffect->Download()))
        return;

    if (FAILED(hr = lpdiEffect->Start(INFINITE, 0)))
        return;

}
BOOL EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance2, VOID* pContext2)
{
    if (pdidInstance2->guidProduct.Data1 == 1770590 && pdidInstance2->guidProduct.Data2 == 0 && pdidInstance2->guidProduct.Data3 == 0)
    {
        SetEffectLocal(pdidInstance2, pContext2, triggerForceType);
    }
    return (DIENUM_CONTINUE);
}

void SetEffect(ForceType forceType)
{
    triggerForceType = forceType;
    dinp->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, &globalWindow, DIEDFL_ATTACHEDONLY);
}

void Win32LoadXInput(HWND Window)
{
    HMODULE XInputLibrary = Win32GetXInputLibrary();

    if (XInputLibrary)
    {
        XInputGetState = (X_Input_Get_State*)GetProcAddress(XInputLibrary, "XInputGetState");
        XInputSetState = (X_Input_Set_State*)GetProcAddress(XInputLibrary, "XInputSetState");
    }

    if (DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&dinp, 0) != DI_OK)
    {
        return;
    }
    SetEffect(ForceType_Nothing);
}

struct win32_game_code
{
    HMODULE gameCodeDLL;
    GameUpdateAndRender* gameUpdateAndRender;
};

win32_game_code Win32LoadGameCode()
{
    win32_game_code result = {};

    result.gameCodeDLL = LoadLibraryA("quote.dll");
    result.gameUpdateAndRender = 0;
    if (result.gameCodeDLL)
    {
        result.gameUpdateAndRender = (GameUpdateAndRender*)GetProcAddress(result.gameCodeDLL, "gameUpdateAndRender");
    }

    return result;
}
void Win32UnloadGameCode(win32_game_code* gameCode)
{
    if (gameCode->gameCodeDLL)
        FreeLibrary(gameCode->gameCodeDLL);

    gameCode->gameUpdateAndRender = 0;
}

struct win32_debug_time_marker
{
    DWORD PlayCursor;
    DWORD WriteCursor;
};

struct win32_offscreen_buffer // always 4 bytes per pixel BGRA
{
    BITMAPINFO Info;
    uint8* memory;
    int width;
    int height;
    int bytesPerPixel;
    int pitch; // Distance to get to the next line
};
bool globalRunning = true;
win32_offscreen_buffer globalBackbuffer;
LPDIRECTSOUNDBUFFER globalSoundBuffer;
GameInput input;
LARGE_INTEGER globalTickFrequency;
game_sound_output_buffer soundBuffer = {};
VideoBuffer buffer = {};
GameMemory gameMemory = {};
win32_game_code gameCode;
#define maxFileCount 200
int64 times[maxFileCount];
int skipReload = 0;
DWORD dwThreadIdArray[100];
HANDLE hThreadArray[100];
int threadFunctionPointers[100];

struct win32_sound_output
{
    int samplesPerSecond;
    int bytesPerSample;
    int bufferSize;
    uint32 runningSampleIndex;
    int latencySampleCount;
};

struct win32_window_dimensions
{
    int width;
    int height;
};

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(Direct_Sound_Create);


void win32_InitDSound(HWND window, int32 samplesPerSecond, int32 bufferSize)
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

    if (!SUCCEEDED(DirectSound->SetCooperativeLevel(window, DSSCL_PRIORITY))) // if this fails we're fucked
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
    WaveFormat.nSamplesPerSec = samplesPerSecond;                                                                              // (size of this)
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
    SecondaryBufferDescription.dwBufferBytes = bufferSize;
    SecondaryBufferDescription.lpwfxFormat = &WaveFormat;

    if (!SUCCEEDED(DirectSound->CreateSoundBuffer(&SecondaryBufferDescription, &globalSoundBuffer, 0)))
        return;

    // we did it!
}

void win32_ClearSoundBuffer(win32_sound_output* SoundOutput)
{
    VOID* Region1;
    DWORD Region1Size;
    VOID* Region2;
    DWORD Region2Size;

    if (!SUCCEEDED(globalSoundBuffer->Lock(0, SoundOutput->bufferSize, &Region1, &Region1Size, &Region2, &Region2Size, 0)))
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


    globalSoundBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);

}
void win32_FillSoundBuffer(win32_sound_output* SoundOutput, DWORD ByteToLock, DWORD BytesToWrite, game_sound_output_buffer* SourceBuffer)
{
    // Buffer structure
    // sample        sample        sample        sample
    // int16  int16  int16  int16  int16  int16  int16  int16 
    // [left  right] [left  right] [left  right] [left  right]
    VOID* Region1;
    DWORD Region1Size;
    VOID* Region2;
    DWORD Region2Size;

    if (!SUCCEEDED(globalSoundBuffer->Lock(ByteToLock, BytesToWrite, &Region1, &Region1Size, &Region2, &Region2Size, 0)))
        return;

    // TODO: assert that region1size and region1size is an even multiple of byestpersample

    // TODO: combine these two loops
    DWORD Region1SampleCount = Region1Size / SoundOutput->bytesPerSample;
    Sample* DestinationSample = (Sample*)Region1;
    Sample* SourceSample = (Sample*)SourceBuffer->Samples;
    for (uint64 i = 0; i < Region1SampleCount; i++)
    {
        *DestinationSample++ = *SourceSample++;
        (SoundOutput->runningSampleIndex)++;
    }

    DWORD Region2SampleCount = Region2Size / SoundOutput->bytesPerSample;
    DestinationSample = (Sample*)Region2;

    for (uint64 i = 0; i < Region2SampleCount; i++)
    {
        *DestinationSample++ = *SourceSample++; // post-increment (nice ovo)
        (SoundOutput->runningSampleIndex)++;
    }

    globalSoundBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);
}


void win32_ResizeDIBSection(win32_offscreen_buffer* Buffer, int Width, int Height) // resizes backbuffer
{
    if (Buffer->memory)
    {
        VirtualFree(Buffer->memory, null, MEM_RELEASE);
    }

    Buffer->width = Width;
    Buffer->height = Height;
    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->width;
    Buffer->Info.bmiHeader.biHeight = -Buffer->height; // negative height makes the image top down instead of bottom up.
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;

    // Windows, gimme memory. :)
    Buffer->memory = (uint8*)VirtualAlloc(0, 4 * Width * Height, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE); // Rounds up to page size on its own (4k bytes by default).
    Buffer->bytesPerPixel = 4;
    Buffer->pitch = Width * Buffer->bytesPerPixel;
    // TODO: Probably clear to black
}

void Win32DebugDrawLine(win32_offscreen_buffer* Backbuffer, int X, int Top, int Bottom, uint32 Color)
{
    uint8* Pixel = (uint8*)Backbuffer->memory + X * Backbuffer->bytesPerPixel + Top * Backbuffer->pitch;
    for (int i = Top; i < Bottom; i++)
    {
        *(uint32*)Pixel = Color;
        Pixel += Backbuffer->pitch;
    }
}

win32_window_dimensions GetWindowDimension(HWND Window)
{
    win32_window_dimensions res;
    RECT windowrect;
    GetClientRect(Window, &windowrect);
    res.width = windowrect.right - windowrect.left;
    res.height = windowrect.bottom - windowrect.top;
    return res;
}

void win32_CopyBufferToWindow(const win32_offscreen_buffer* Buffer, HDC Device, int Width, int Height)
{
    StretchDIBits(Device,
        0, 0, Width, Height,
        0, 0, Buffer->width, Buffer->height,
        Buffer->memory, &Buffer->Info, DIB_RGB_COLORS, SRCCOPY);
}


float clamp(float a, float b, float c)
{
    return min(max(a, b), c);
}
float MoveTowards(float current, float target, float distance)
{
    return current + clamp(target - current, -distance, distance);
}
float Win32ProcessStick(int value, int deadzone)
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
bool Focused = true;
LRESULT MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;
    switch ((Events)Message)
    {
    case Size:
    {
        print("WM_SIZE\n");
        win32_window_dimensions dim = GetWindowDimension(Window);
        win32_ResizeDIBSection(&globalBackbuffer, dim.width, dim.height);

        buffer.memory = globalBackbuffer.memory;
        buffer.Width = globalBackbuffer.width;
        buffer.Height = globalBackbuffer.height;
        buffer.BytesPerPixel = 4;
        buffer.Pitch = globalBackbuffer.width * 4;
        buffer.Layout = ChannelLayout_ARGB;
        if (gameCode.gameUpdateAndRender && buffer.Width > 0 && buffer.Height > 0)
            gameCode.gameUpdateAndRender(&gameMemory, &input, &buffer, &soundBuffer);

    } break;
    case Destroy:
    {
        print("WM_DESTROY\n");
        //PostQuitMessage(0);
        globalRunning = false;

    } break;
    case Close:
    {
        print("WM_CLOSE\n");
        //PostQuitMessage(0);
        globalRunning = false;
    } break;
    case Active:
    {
        bool Entered = WParam;
        //Focused = WParam;
        if (Entered)
        {
            print("Entered\n");
            ResetInput(&input);
        }
        else
        {
            print("Exited\n");
            ResetInput(&input);
        }
    } break;
    case Paint:
    {
        print("WM_PAINT\n");
        PAINTSTRUCT Paint;
        HDC DeviceContext = BeginPaint(Window, &Paint);
        win32_window_dimensions dim = GetWindowDimension(Window);
        win32_CopyBufferToWindow(&globalBackbuffer, DeviceContext, dim.width, dim.height);
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

int64 win32_PlatformTime()
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
        for (int i = 0; i < maxFileCount; i++)
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

// https://stackoverflow.com/questions/41533158/create-32-bit-color-icon-programmatically
HICON CreateIconFromBytes(HDC DC, int width, int height, uint32* bytes)
{
    if(!bytes)
        return 0;

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

typedef struct MyData {
    ThreadCallType* function;
    int ThreadIndex;
} MYDATA, * PMYDATA;
MyData threadData[100];

DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
    MyData* data = (MyData*)lpParam;
    while (true)
    {
        //gameMemory.ThreadsExecuting[data->ThreadIndex] = true;
        ThreadCallType* functionPointer = data->function;
        functionPointer(data->ThreadIndex, &gameMemory, &buffer, &soundBuffer);
        gameMemory.threadsExecuting[data->ThreadIndex] = false;
        SuspendThread(GetCurrentThread());
    }

    return 0;
}
void win32_PlatformThreadCall(ThreadCallType function, int ThreadIndex)
{
    gameMemory.threadsExecuting[ThreadIndex] = true;
    threadData[ThreadIndex].function = function;
    threadData[ThreadIndex].ThreadIndex = ThreadIndex;
    ResumeThread(hThreadArray[ThreadIndex]);

}

//float exe_sqrtf(float n) { return sqrtf(n); }
//float exe_floorf(float n) { return floorf(n); }
//float exe_ceilf(float n) { return ceilf(n); }
//float exe_sinf(float n) { return sinf(n); }
//float exe_cosf(float n) { return cosf(n); }
//float exe_atan2(float a, float b) { return atan2(a, b); }
//float exe_log10(float n) { return log10(n); }
//float exe_fabsf(float n) { return fabsf(n); }
//float exe_pow(float a, float b) { return pow(a, b); }
//float exe_fminf(float a, float b) { return fminf(a, b); }
//float exe_fmaxf(float a, float b) { return fmaxf(a, b); }
//float exe_roundf(float a) { return roundf(a); }
//float exe_exp2f(float a) { return exp2f(a); }
//float exe_acosf(float a) { return acosf(a); }
//bool  exe_isfinite(float a) { return isfinite(a); }

// entry point
int WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommandLine, int ShowCode)
{
    //int64 thing = 0;
    //while(true)
    //{
    //    int64 value = __rdtsc();
    //    if(value < thing)
    //    {
    //        printf("amonugs\n");
    //    }
    //    thing = value;
    //}
    //
    //











    int width = 1920;// / 2;
    int height = 1080;// / 2;
    const char* Title = "Thingy Win32";
    const char* IconPath = "favicon.tga";

    // Initialize game memory
#define FramesOfAudioLatency 8
#define GameUpdateHz 60
    float TargetSecondsElapsedPerFrame = 1.0f / (float)GameUpdateHz;

    int smallestPageSize = GetLargePageMinimum();
    LPVOID BaseAddress = 0;// (LPVOID)(smallestPageSize * 1024);
    gameMemory.permanentStorageSize = Megabytes(2); // Stored in memory and copied to disk now and then.
    gameMemory.transientStorageSize = Megabytes(1024); // RAM avilable for use.
    uint64 TotalSize = gameMemory.permanentStorageSize + gameMemory.transientStorageSize;

    // Set the base addresses it will allocate to so that they are the same every time. this allows for awesome debugging
    gameMemory.permanentStorage = (uint8*)VirtualAlloc(0, (size_t)TotalSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    DWORD error = GetLastError();
    gameMemory.transientStorage = (uint8*)gameMemory.permanentStorage + gameMemory.permanentStorageSize;

    gameMemory.platformThreadCall = &win32_PlatformThreadCall;
    gameMemory.platformWriteFile = &win32_PlatformWriteFile;
    gameMemory.platformReadFile = &win32_PlatformReadFile;
    gameMemory.platformReadImageIntoMemory = &win32_PlatformReadImageIntoMemory;
    gameMemory.platformReadSoundIntoMemory = &win32_PlatformReadSoundIntoMemory;
    gameMemory.platformPrint = &win32_PlatformPrint;
    gameMemory.platformSetForcefeedbackEffect = &SetEffect;
    print = &win32_PlatformPrint;
    gameMemory.platformTime = &win32_PlatformTime;
    gameMemory.threadCount = 50;

    //gameMemory.sqrtf = &sqrtf;
    //gameMemory.floorf = &floorf;
    //gameMemory.ceilf = &ceilf;
    //gameMemory.sinf = &sinf;
    //gameMemory.cosf = &cosf;
    //gameMemory.atan2 = &atan2f;
    //gameMemory.log10 = &log10f;
    //gameMemory.fabsf = &fabsf;
    //gameMemory.pow = &powf;
    //gameMemory.fminf = &fminf;
    //gameMemory.fmaxf = &fmaxf;
    //gameMemory.roundf = &roundf;
    //gameMemory.exp2f = &exp2f;
    //gameMemory.acosf = &acosf;
    //gameMemory.isfinite = &isfinite;

    // Spawn a bunch of threads and immedietly suspend them
    for (int ThreadIndex = 0; ThreadIndex < gameMemory.threadCount; ThreadIndex++)
    {
        threadData[ThreadIndex].ThreadIndex = ThreadIndex;
        hThreadArray[ThreadIndex] = CreateThread(
            NULL,                                   // default security attributes
            0,                                      // use default stack size  
            MyThreadFunction,                       // thread function name
            &threadData[ThreadIndex],               // argument to thread function 
            CREATE_SUSPENDED,                       // use default creation flags 
            &dwThreadIdArray[ThreadIndex]);         // returns the thread identifier 
    }


    //if (SaveDelay == 0)
    //{
    read_file_result data = {};
    data = win32_PlatformReadFile("savegame.bin");
    for (int i = 0; i < gameMemory.permanentStorageSize; i++)
    {
        ((char*)gameMemory.permanentStorage)[i] = 0;
    }
    for (int i = 0; i < min(data.ContentsSize, gameMemory.permanentStorageSize); i++)
    {
        ((char*)gameMemory.permanentStorage)[i] = ((char*)data.Contents)[i];
    }
    win32_PlatformFreeFileMemory(data);
    // }

    WNDCLASS WindowClass = {};
    // TODO: Check if hdredraw and vredraw are needed
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = (WNDPROC)MainWindowCallback;
    WindowClass.hInstance = GetModuleHandle(0);
    WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WindowClass.lpszClassName = "HelloWindowClass";

    // Make icon
    // Small enough to fit on the stack. (1k)
    uint8 StackImage[1024];
    uint32* image = PlatformReadImageIntoMemory_local(StackImage, IconPath);
    HICON hIcon3 = CreateIconFromBytes(CreateCompatibleDC(NULL), 16, 16, image);
    WindowClass.hIcon = hIcon3;

    if (!RegisterClass(&WindowClass))
        return 0;

    // Set windows scheduler granuality to 1ms so our sleep() can be more granular
    bool SleepIsGranular = timeBeginPeriod(1) == TIMERR_NOERROR;

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
    globalWindow = Window;

    triggerEffect = false;
    Win32LoadXInput(Window);


    LARGE_INTEGER ticks = Win32GetWallClock();
    LARGE_INTEGER LastCounter = ticks;

    // Initialize sound output
    win32_sound_output SoundOutput = {};
    //
    //SoundOutput.SamplesPerSecond = 48000;
    SoundOutput.samplesPerSecond = 44100;
    SoundOutput.bytesPerSample = sizeof(int16) * 2;
    SoundOutput.bufferSize = (SoundOutput.samplesPerSecond * SoundOutput.bytesPerSample); // 1 second long buffer
    SoundOutput.runningSampleIndex = 0;
    SoundOutput.latencySampleCount = FramesOfAudioLatency * (SoundOutput.samplesPerSecond / GameUpdateHz); // if our framerate is below 15 we'll start skipping / stuttering the sound.
    Sample* Samples = (Sample*)VirtualAlloc(0, SoundOutput.samplesPerSecond * 2 * sizeof(int16), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    win32_InitDSound(Window, SoundOutput.samplesPerSecond, SoundOutput.bufferSize);
    win32_ClearSoundBuffer(&SoundOutput);
    globalSoundBuffer->Play(0, 0, DSBPLAY_LOOPING);


    if (Samples && gameMemory.permanentStorage && gameMemory.transientStorage) {}
    else
        return 0; // failed to accloate memory. no game this time.

    DWORD LastPlayCursor = 0;
    bool SoundIsValid = false;

    int64 CycleCount = 0;
    float Time = 0;
    gameCode = Win32LoadGameCode();
    uint8 SaveDelay = 0;
    while (globalRunning)
    {
        //if (triggerEffect)
        //{
        //    triggerEffect = false;
        //    SetEffectLocal(triggerForceType);
        //}
        //PlatformThreadCall(&PrintHello, 2);
        SaveDelay++;
        // Only update input if the window is focused.

            // Deal with windows messages
        MSG Message;
        while (PeekMessage(&Message, null, null, null, PM_REMOVE))
        {
            switch (Message.message)
            {
            case WM_QUIT:
                globalRunning = false;
                break;

            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP:
            {
                if (!Focused) break;
                bool WasDown = (Message.lParam & (1 << 30)) != 0;
                bool IsDown = (Message.lParam & (1 << 31)) == 0;

                if (WasDown != IsDown)
                {
                    uint32 VKCode = (uint32)Message.wParam;
                    if (IsValidIndex(VKCode - 65, ArrayCapacity(input.Letters)))
                        input.Letters[VKCode - 65].Held = IsDown;

                    if (IsValidIndex(VKCode - 48, ArrayCapacity(input.Numbers)))
                        input.Numbers[VKCode - 48].Held = IsDown;

                    if (VKCode == VK_SPACE)     input.Space = IsDown;
                    if (VKCode == VK_BACK)      input.Backspace = IsDown;
                    if (VKCode == VK_RETURN)    input.Enter = IsDown;
                    if (VKCode == VK_ESCAPE)    input.Escape = IsDown;
                    if (VKCode == VK_DELETE)    input.Delete = IsDown;

                    if (VKCode == VK_UP)        input.Up = IsDown;
                    if (VKCode == VK_DOWN)      input.Down = IsDown;
                    if (VKCode == VK_LEFT)      input.Left = IsDown;
                    if (VKCode == VK_RIGHT)     input.Right = IsDown;

                    if (VKCode == VK_SHIFT)     input.Shift = IsDown;
                    if (VKCode == VK_CONTROL)   input.Ctrl = IsDown;
                    if (VKCode == VK_MENU)      input.Alt = IsDown;
                }
                // Alt + F4 close
                bool AltKeyIsDown = (Message.lParam & (1 << 29)) != 0;
                if (AltKeyIsDown && Message.wParam == VK_F4)
                {
                    globalRunning = false;
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
                input.MouseLeft = true;
            } break;
            case WM_LBUTTONUP:
            {
                if (!Focused) break;
                input.MouseLeft = false;
            } break;
            case WM_RBUTTONDOWN:
            {
                if (!Focused) break;
                input.MouseRight = true;
            } break;
            case WM_RBUTTONUP:
            {
                if (!Focused) break;
                input.MouseRight = false;
            } break;
            case WM_MBUTTONDOWN:
            {
                if (!Focused) break;
                input.MouseMiddle = true;
            } break;
            case WM_MBUTTONUP:
            {
                if (!Focused) break;
                input.MouseMiddle = false;
            } break;
            case WM_MOUSEWHEEL:
            {
                if (!Focused) break;
                input.MouseZ += GET_WHEEL_DELTA_WPARAM(Message.wParam) / 120;
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
            InputUpdateInt(&input.StateMouseX, TempMouseX);
            InputUpdateInt(&input.StateMouseY, TempMouseY);
            //// Pointer ballistics?
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

            InputUpdateInt(&input.StateMouseZ, input.MouseZ);

            InputUpdateButton(&input.StateMouseLeft, input.MouseLeft);
            InputUpdateButton(&input.StateMouseRight, input.MouseRight);
            InputUpdateButton(&input.StateMouseMiddle, input.MouseMiddle);

            InputUpdateButton(&input.StateSpace, input.Space);
            InputUpdateButton(&input.StateBackspace, input.Backspace);
            InputUpdateButton(&input.StateEnter, input.Enter);
            InputUpdateButton(&input.StateEscape, input.Escape);
            InputUpdateButton(&input.StateDelete, input.Delete);

            InputUpdateButton(&input.StateLeft, input.Left);
            InputUpdateButton(&input.StateRight, input.Right);
            InputUpdateButton(&input.StateUp, input.Up);
            InputUpdateButton(&input.StateDown, input.Down);

            InputUpdateButton(&input.StateShift, input.Shift);
            InputUpdateButton(&input.StateCtrl, input.Ctrl);
            InputUpdateButton(&input.StateAlt, input.Alt);

            for (int i = 0; i < 26; i++)
            {
                InputUpdateButton(&input.Letters[i], input.Letters[i].Held);
            }

            for (int i = 0; i < 10; i++)
            {
                InputUpdateButton(&input.Numbers[i], input.Numbers[i].Held);
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
                    InputUpdateFloat(&input.Controllers[i].StateLeftStickX, Win32ProcessStick(Gamepad->sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE));
                    InputUpdateFloat(&input.Controllers[i].StateLeftStickY, Win32ProcessStick(Gamepad->sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE));
                    InputUpdateFloat(&input.Controllers[i].StateRightStickX, Win32ProcessStick(Gamepad->sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE));
                    InputUpdateFloat(&input.Controllers[i].StateRightStickY, Win32ProcessStick(Gamepad->sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE));

                    InputUpdateButton(&input.Controllers[i].StateA, Gamepad->wButtons & XINPUT_GAMEPAD_A);
                    InputUpdateButton(&input.Controllers[i].StateB, Gamepad->wButtons & XINPUT_GAMEPAD_B);
                    InputUpdateButton(&input.Controllers[i].StateX, Gamepad->wButtons & XINPUT_GAMEPAD_X);
                    InputUpdateButton(&input.Controllers[i].StateY, Gamepad->wButtons & XINPUT_GAMEPAD_Y);

                    InputUpdateButton(&input.Controllers[i].StateUp, Gamepad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
                    InputUpdateButton(&input.Controllers[i].StateDown, Gamepad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                    InputUpdateButton(&input.Controllers[i].StateLeft, Gamepad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                    InputUpdateButton(&input.Controllers[i].StateRight, Gamepad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);

                    InputUpdateButton(&input.Controllers[i].StateLeftBumper, Gamepad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
                    InputUpdateButton(&input.Controllers[i].StateRightBumper, Gamepad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
                    InputUpdateButton(&input.Controllers[i].StateLeftTrigger, Gamepad->bLeftTrigger);
                    InputUpdateButton(&input.Controllers[i].StateRightTrigger, Gamepad->bRightTrigger);

                    InputUpdateButton(&input.Controllers[i].StateStart, Gamepad->wButtons & XINPUT_GAMEPAD_START);
                    InputUpdateButton(&input.Controllers[i].StateBack, Gamepad->wButtons & XINPUT_GAMEPAD_BACK);
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
            TargetCursor = (LastPlayCursor + (SoundOutput.latencySampleCount * SoundOutput.bytesPerSample)) % SoundOutput.bufferSize;

            ByteToLock = ((SoundOutput.runningSampleIndex) * SoundOutput.bytesPerSample) % SoundOutput.bufferSize; // where we are in the buffer this frame
            // we want to fill in the "wwww" region with our data so that the write cursor catches up behind the play cursor.
            // There are two possible situations, one where the write cursor is ahead of the play cursor, and one where it's behind.
            // This is how we handle those two scenarios:

            if (ByteToLock > TargetCursor)
            {
                //[wwwwwww--------wwwwwww]
                //0       ^      ^       21
                //      Play   Write

                BytesToWrite = SoundOutput.bufferSize - ByteToLock;
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
        soundBuffer.SamplesPerSecond = SoundOutput.samplesPerSecond;
        soundBuffer.SampleCount = BytesToWrite / SoundOutput.bytesPerSample;
        soundBuffer.Samples = Samples;

        buffer.memory = globalBackbuffer.memory;
        buffer.Width = globalBackbuffer.width;
        buffer.Height = globalBackbuffer.height;
        buffer.BytesPerPixel = 4;
        buffer.Pitch = globalBackbuffer.width * 4;
        buffer.Layout = ChannelLayout_ARGB;

        // Load persistent state
        if (gameCode.gameUpdateAndRender && buffer.Width > 0 && buffer.Height > 0)
            gameCode.gameUpdateAndRender(&gameMemory, &input, &buffer, &soundBuffer);


        if (SaveDelay == 0)
        {
            // Save persistent state
            //read_file_result data = {};
            //data.Contents = (uint8*)gameMemory.permanentStorage;
            //data.ContentsSize = gameMemory.permanentStorageSize;
            win32_PlatformWriteFile("savegame.bin", (uint8*)gameMemory.permanentStorage, gameMemory.permanentStorageSize);
        }

        if (FileExists("build.bat"))
        {
            bool FileChanged = ReloadFilesChanged();

            if (FileChanged)
            {
                ResetInput(&input);
                //print("Recompile");
                Win32UnloadGameCode(&gameCode);

                system("build.bat dll_only");
                //system("build.bat dll_only use_clang");
                gameCode = Win32LoadGameCode();

                ReloadFilesChanged();
            }
        }

        if (SoundIsValid)
        {
            win32_FillSoundBuffer(&SoundOutput, ByteToLock, BytesToWrite, &soundBuffer);
        }

        LARGE_INTEGER WorkCounter = Win32GetWallClock();
        float WorkSecondsElapsed = Win32GetSecondsElapsed(LastCounter, WorkCounter);

        input.deltaTime = WorkSecondsElapsed;


        LARGE_INTEGER EndCounter = Win32GetWallClock();
        WorkSecondsElapsed = Win32GetSecondsElapsed(LastCounter, Win32GetWallClock());

        LastCounter = EndCounter;

        // Copy rendered image to screen
        HDC DeviceContext = GetDC(Window);
        win32_window_dimensions dim = GetWindowDimension(Window);
        win32_CopyBufferToWindow(&globalBackbuffer, DeviceContext, dim.width, dim.height);
        ReleaseDC(Window, DeviceContext);

        DWORD PlayCursor;
        DWORD WriteCursor;
        if (SUCCEEDED(globalSoundBuffer->GetCurrentPosition(&PlayCursor, &WriteCursor)))
        {
            if (!SoundIsValid)
            {
                SoundOutput.runningSampleIndex = WriteCursor / SoundOutput.bytesPerSample;
            }
            SoundIsValid = true;
            LastPlayCursor = PlayCursor;
        }
        else
        {
            SoundIsValid = false;
        }
    }

    return 1337;
}
