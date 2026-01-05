#pragma once
// This file is shared between the game and the platform. Any code here will be avilable in both.

#define GET_MACRO(_0, _1, _2, NAME, ...) NAME
#define Assert(...) GET_MACRO(_0, ##__VA_ARGS__, Assert2, Assert1, Assert0)(__VA_ARGS__)

#define Assert1(expression)      if(!(expression)) { *(int*)0 = 0; }
#define Assert2(expression, msg) if(!(expression)) { const char* message = msg; *(int*)0 = 0; }

#define ArrayCapacity(Array) (sizeof(Array) / sizeof((Array)[0]))

typedef signed char         int8;
typedef signed short        int16;
typedef signed int          int32;
typedef signed long long    int64;

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned long long  uint64;

#define null 0

#define Kilobytes(x) ((x) * (1024LL))
#define Megabytes(x) ((Kilobytes(x)) * (1024LL))
#define Gigabytes(x) ((Megabytes(x)) * (1024LL))
#define Terabytes(x) ((Gigabytes(x)) * (1024LL))

uint32 SafeTrunchateUint64(uint64 Value)
{
    Assert(Value <= 0xFFFFFFFF);
    uint32 result = (uint32)Value;
    return result;
}

bool IsValidIndex(int index, int length)
{
    if (index >= 0 && index <= length-1)
    {
        return true;
    }
    else
    {
        return false;
    }
}


enum ChannelLayout {
    ChannelLayout_ARGB,
    ChannelLayout_ABGR,
};

struct VideoBuffer
{
    void* memory;
    int Width;
    int Height;
    int BytesPerPixel;
    int Pitch;
    ChannelLayout Layout;
};

struct Sample
{
    int16 left;
    int16 right;
};

Sample ctor_Sample(int16 left, int16 right)
{
    Sample result;
    result.left = left;
    result.right = right;
    return result;
}
#define Sample(left, right) ctor_Sample((left), (right))


struct game_sound_output_buffer
{
    int32 SamplesPerSecond;
    int32 SampleCount;
    Sample* Samples;
};

struct game_button_state
{
    bool Held; // Button is being held down
    bool Down; // Button changed from down to up
    bool Up; // Button changed from up to down
    bool LastHeld;
};

struct game_int_state
{
    int32 Value;
    int32 LastValue;
    int32 ValueDelta;
};

struct game_float_state
{
    float Value;
    float LastValue;
    float ValueDelta;
};
#define UNION_BUTTON(name) union { game_button_state State##name; struct { bool name; bool name##Up; bool name##Down; }; };
#define UNION_INT(name) union { game_int_state State##name; struct { int32 name; int32 Last##name; int32 name##Delta; }; };
#define UNION_FLOAT(name) union { game_float_state State##name; struct { float name; float Last##name; float name##Delta; }; };


struct game_controller_input
{
    UNION_FLOAT(LeftStickX)
    UNION_FLOAT(LeftStickY)
    UNION_FLOAT(RightStickX)
    UNION_FLOAT(RightStickY)

    union
    {
        game_button_state Buttons[12];
        struct
        {
            UNION_BUTTON(Left)
            UNION_BUTTON(Right)
            UNION_BUTTON(Up)
            UNION_BUTTON(Down)

            UNION_BUTTON(A)
            UNION_BUTTON(B)
            UNION_BUTTON(X)
            UNION_BUTTON(Y)

            UNION_BUTTON(LeftBumper)
            UNION_BUTTON(RightBumper)

            UNION_BUTTON(LeftTrigger)
            UNION_BUTTON(RightTrigger)

            UNION_BUTTON(Start)
            UNION_BUTTON(Back)
        };
    };
};

struct thread
{
    bool executing;
    void* function;
};

enum ForceType
{
    ForceType_Nothing,
    ForceType_SpringCenter,
    ForceType_SpringAwayFromCenter,
    ForceType_SpringForward,
    ForceType_Sine,
    ForceType_SineFast,
    ForceType_ConstantForceForward,
    ForceType_Friction,
    ForceType_Damper,
    ForceType_Inertia,
};
typedef bool PlatformWriteFile(const char* filename, void* source, uint32 length);
typedef bool PlatformReadFile(const char* filename, void* target);
typedef void PlatformReadImageIntoMemory(void* destination, const char* filename);
typedef void PlatformReadSoundIntoMemory(void* destination, const char* filename);
typedef int64 PlatformTime();
typedef void PlatformPrint(const char* string);
PlatformPrint* print;

typedef void PlatformSetForcefeedbackEffect(ForceType forceType);
struct GameMemory;

typedef void ThreadCallType(int thread, GameMemory* memory, VideoBuffer* videoBuffer, game_sound_output_buffer* soundBuffer);

typedef void PlatformThreadCall(ThreadCallType function, int threadIndex);

/*
#if WASM
#else
extern "C" __declspec(dllexport) float exe_sqrtf(float n);
extern "C" __declspec(dllexport) float exe_floorf(float n);
extern "C" __declspec(dllexport) float exe_ceilf(float n);
extern "C" __declspec(dllexport) float exe_sinf(float n);
extern "C" __declspec(dllexport) float exe_cosf(float n);
extern "C" __declspec(dllexport) float exe_atan2(float a, float b);
extern "C" __declspec(dllexport) float exe_log10(float n);
extern "C" __declspec(dllexport) float exe_fabsf(float n);
extern "C" __declspec(dllexport) float exe_pow(float a, float b);
extern "C" __declspec(dllexport) float exe_fminf(float a, float b);
extern "C" __declspec(dllexport) float exe_fmaxf(float a, float b);
extern "C" __declspec(dllexport) float exe_roundf(float a);
extern "C" __declspec(dllexport) float exe_exp2f(float a);
extern "C" __declspec(dllexport) float exe_acosf(float a);
extern "C" __declspec(dllexport) bool  exe_isfinite(float a);
#endif
typedef float platform_sqrtf(float n);
typedef float platform_floorf(float n);
typedef float platform_ceilf(float n);
typedef float platform_sinf(float n);
typedef float platform_cosf(float n);
typedef float platform_atan2(float a, float b);
typedef float platform_log10(float n);
typedef float platform_fabsf(float n);
typedef float platform_pow(float a, float b);
typedef float platform_fminf(float a, float b);
typedef float platform_fmaxf(float a, float b);
typedef float platform_roundf(float a);
typedef float platform_exp2f(float a);
typedef float platform_acosf(float a);
typedef bool  platform_isfinite(float a);
float external_sqrtf(float n);
float external_floorf(float n);
float external_ceilf(float n);
float external_sinf(float n);
float external_cosf(float n);
float external_atan2(float a, float b);
float external_log10(float n);
float external_fabsf(float n);
float external_pow(float a, float b);
float external_fminf(float a, float b);
float external_fmaxf(float a, float b);
float external_roundf(float a);
float external_exp2f(float a);
float external_acosf(float a);
bool  external_isfinite(float a);
*/

struct GameMemory
{
    bool initialized;
    uint64 permanentStorageSize;
    void*  permanentStorage; // Note: REQUIRED to be initialized to all 0s.
    uint64 transientStorageSize;
    void*  transientStorage; // Note: REQUIRED to be initialized to all 0s.

    PlatformWriteFile* platformWriteFile;
    PlatformReadFile* platformReadFile;
    PlatformReadImageIntoMemory* platformReadImageIntoMemory;
    PlatformReadSoundIntoMemory* platformReadSoundIntoMemory;
    PlatformPrint* platformPrint;
    PlatformTime* platformTime;
    PlatformSetForcefeedbackEffect* platformSetForcefeedbackEffect;

    // call a function on another thread.
    PlatformThreadCall* platformThreadCall;
    int threadCount;
    bool threadsExecuting[100]; // bool is true if the thread is currently executing.

    //platform_sqrtf* sqrtf;
    //platform_floorf* floorf;
    //platform_ceilf* ceilf;
    //platform_sinf* sinf;
    //platform_cosf* cosf;
    //platform_atan2* atan2;
    //platform_log10* log10;
    //platform_fabsf* fabsf;
    //platform_pow* pow;
    //platform_fminf* fminf;
    //platform_fmaxf* fmaxf;
    //platform_roundf* roundf;
    //platform_exp2f* exp2f;
    //platform_acosf* acosf;
    //platform_isfinite* isfinite;

};

void InputUpdateButton(game_button_state* Button, bool Held)
{
    Button->Held = Held;
    Button->Down = false;
    Button->Up = false;
    if (Button->Held != Button->LastHeld)
    {
        Button->LastHeld = Button->Held;
        if (Button->Held)
        {
            Button->Up = true;
        }
        else
        {
            Button->Down = true;
        }
    }
}

void InputUpdateInt(game_int_state* Button, int32 NewValue)
{
    Button->Value = NewValue;
    Button->ValueDelta = Button->Value - Button->LastValue;
    Button->LastValue = Button->Value;
}
void InputUpdateFloat(game_float_state* Button, float NewValue)
{
    Button->Value = NewValue;
    Button->ValueDelta = Button->Value - Button->LastValue;
    Button->LastValue = Button->Value;
}

struct GameInput
{
    UNION_INT(MouseX)
    UNION_INT(MouseY)
    UNION_INT(MouseZ)

    UNION_BUTTON(MouseLeft)
    UNION_BUTTON(MouseRight)
    UNION_BUTTON(MouseMiddle)

    union
    {
        game_button_state Letters[26];
        struct
        {
            UNION_BUTTON(A)
            UNION_BUTTON(B)
            UNION_BUTTON(C)
            UNION_BUTTON(D)
            UNION_BUTTON(E)
            UNION_BUTTON(F)
            UNION_BUTTON(G)
            UNION_BUTTON(H)
            UNION_BUTTON(I)
            UNION_BUTTON(J)
            UNION_BUTTON(K)
            UNION_BUTTON(L)
            UNION_BUTTON(M)
            UNION_BUTTON(N)
            UNION_BUTTON(O)
            UNION_BUTTON(P)
            UNION_BUTTON(Q)
            UNION_BUTTON(R)
            UNION_BUTTON(S)
            UNION_BUTTON(T)
            UNION_BUTTON(U)
            UNION_BUTTON(V)
            UNION_BUTTON(W)
            UNION_BUTTON(X)
            UNION_BUTTON(Y)
            UNION_BUTTON(Z)
        };
    };

    union
    {
        game_button_state Numbers[10];
        struct
        {
            UNION_BUTTON(D0)
            UNION_BUTTON(D1)
            UNION_BUTTON(D2)
            UNION_BUTTON(D3)
            UNION_BUTTON(D4)
            UNION_BUTTON(D5)
            UNION_BUTTON(D6)
            UNION_BUTTON(D7)
            UNION_BUTTON(D8)
            UNION_BUTTON(D9)
        };
    };

    UNION_BUTTON(Space)
    UNION_BUTTON(Backspace)
    UNION_BUTTON(Enter)
    UNION_BUTTON(Escape)
    UNION_BUTTON(Delete)

    UNION_BUTTON(Left)
    UNION_BUTTON(Right)
    UNION_BUTTON(Up)
    UNION_BUTTON(Down)

    UNION_BUTTON(Shift)
    UNION_BUTTON(Ctrl)
    UNION_BUTTON(Alt)

    float deltaTime;

    game_controller_input Controllers[4];
};

typedef void GameUpdateAndRender(GameMemory* memory, GameInput* input, VideoBuffer* videoBuffer, game_sound_output_buffer* SoundBuffer);

// Call this to set all input to nothing. useful for when the window is de-seleted.
void ResetInput(GameInput* input)
{
    //InputUpdateInt(&Input.StateMouseX, 0);
    //InputUpdateInt(&Input.StateMouseY, 0);

    InputUpdateInt(&input->StateMouseZ, false);

    InputUpdateButton(&input->StateMouseLeft, false);
    InputUpdateButton(&input->StateMouseRight, false);
    InputUpdateButton(&input->StateMouseMiddle, false);

    InputUpdateButton(&input->StateSpace, false);
    InputUpdateButton(&input->StateBackspace, false);
    InputUpdateButton(&input->StateEnter, false);
    InputUpdateButton(&input->StateEscape, false);
    InputUpdateButton(&input->StateDelete, false);

    InputUpdateButton(&input->StateLeft, false);
    InputUpdateButton(&input->StateRight, false);
    InputUpdateButton(&input->StateUp, false);
    InputUpdateButton(&input->StateDown, false);

    InputUpdateButton(&input->StateShift, false);
    InputUpdateButton(&input->StateCtrl, false);
    InputUpdateButton(&input->StateAlt, false);

    for (int i = 0; i < 26; i++)
    {
        InputUpdateButton(&input->Letters[i], false);
    }

    for (int i = 0; i < 10; i++)
    {
        InputUpdateButton(&input->Numbers[i], false);
    }


    for (int i = 0; i < 4; i++)
    {
        InputUpdateFloat(&input->Controllers[i].StateLeftStickX, 0.0f);
        InputUpdateFloat(&input->Controllers[i].StateLeftStickY, 0.0f);
        InputUpdateFloat(&input->Controllers[i].StateRightStickX, 0.0f);
        InputUpdateFloat(&input->Controllers[i].StateRightStickY, 0.0f);

        InputUpdateButton(&input->Controllers[i].StateA, false);
        InputUpdateButton(&input->Controllers[i].StateB, false);
        InputUpdateButton(&input->Controllers[i].StateX, false);
        InputUpdateButton(&input->Controllers[i].StateY, false);

        InputUpdateButton(&input->Controllers[i].StateUp, false);
        InputUpdateButton(&input->Controllers[i].StateDown, false);
        InputUpdateButton(&input->Controllers[i].StateLeft, false);
        InputUpdateButton(&input->Controllers[i].StateRight, false);

        InputUpdateButton(&input->Controllers[i].StateLeftBumper, false);
        InputUpdateButton(&input->Controllers[i].StateRightBumper, false);
        InputUpdateButton(&input->Controllers[i].StateLeftTrigger, false);
        InputUpdateButton(&input->Controllers[i].StateRightTrigger, false);

        InputUpdateButton(&input->Controllers[i].StateStart, false);
        InputUpdateButton(&input->Controllers[i].StateBack, false);
    }
}

game_controller_input* GetController(GameInput* Input, int ControllerIndex)
{
    Assert(ControllerIndex > 0);
    Assert(ControllerIndex < ArrayCapacity(Input->Controllers));

    game_controller_input* Result = &Input->Controllers[ControllerIndex];
    return Result;
}