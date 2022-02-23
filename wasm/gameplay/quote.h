
#pragma once
#include "quote_platform.cpp"

/*

QUOTE_INTERNAL
    0 - Build for public release
    1 - Build for developer only

QUOTE_SLOW
    0 - No slow code allowed!
    1 - Slow code! (assertions etc)
*/


// Global functions
platform_print* print;

void InitGlobalFunctions(game_memory* GameMemory)
{
    print = GameMemory->PlatformPrint;
}


inline game_controller_input* GetController(game_input* Input, int ControllerIndex)
{
    Assert(ControllerIndex > 0);
    Assert(ControllerIndex < ArrayCount(Input->Controllers));

    game_controller_input* Result = &Input->Controllers[ControllerIndex];
    return Result;
}

struct MemoryArena
{
    uint32 size;
    uint8* base;
    memory_index used;
};

void InitializeArena(MemoryArena* arena, memory_index Size, uint8* Base)
{
    arena->size = Size;
    arena->base = Base;
    arena->used = 0;
}

#define PushBytes(arena, Count) (char*)PushSize_(arena, Count)
#define PushStruct(arena, type) (type*)PushSize_(arena, sizeof(type)) // pushes 1x of a struct and returns pointer to start of it.
#define PushArray(arena, Count, type) (type*)PushSize_(arena, (Count)*sizeof(type)) // pushes Nx of a struct and returns pointer to start of it.
void* PushSize_(MemoryArena* arena, memory_index size)
{
    Assert((arena->used + size) <= arena->size); // Assert that we won't overrun memory

    void* Result = arena->base + arena->used;
    arena->used += size;

    return Result;
}

struct Sound
{
    int sampleCount;
    Sample* samples;
};

struct SoundChannel
{
    int currentSample;
    Sound* sound;
    float volume;
    float speed;
    float soundDelta; // Temporarly value to make speed change work.
    bool loop;
    bool playing;
};

struct Image
{
    int width;
    int height;
    uint32* ImageData;
};

#include "math.cpp"

enum RenderCommandType
{
    RenderCommandType_Sphere,
    RenderCommandType_Box,
    RenderCommandType_Skybox,
    RenderCommandType_Clear,
    RenderCommandType_Effect
};

struct EffectData
{
    float spawnRate;
    bool burst;
    float spawnRadius;
    float drag;
    float3 minSpeed;
    float3 maxSpeed;
    float minLifetime;
    float maxLifetime;
    float startSize;
    float endSize;
    float gravity;
    float startTime; // Time at which the effect started, can be used for manipulating time.
};


inline EffectData ctor_EffectData(float spawnRate,
    bool burst,
    float spawnRadius,
    float drag,
    float3 minSpeed,
    float3 maxSpeed,
    float minLifetime,
    float maxLifetime,
    float startSize,
    float endSize,
    float gravity/*,
    float startTime*/)
{
    EffectData result;
    result.spawnRate = spawnRate;
    result.burst = burst;
    result.spawnRadius = spawnRadius;
    result.drag = drag;
    result.minSpeed = minSpeed;
    result.maxSpeed = maxSpeed;
    result.minLifetime = minLifetime;
    result.maxLifetime = maxLifetime;
    result.startSize = startSize;
    result.endSize = endSize;
    result.gravity = gravity;
    //result.startTime = startTime;
    return result;
}


struct RenderCommand
{
    RenderCommandType Type;
    transform t;
    float3 color;
    Image* texture1;
    Image* texture2;
    Image* cubemapTexture[6];
    int ShaderIndex;
    bool selected;
    bool hovered;

    EffectData effect;
};

struct EngineSaveState
{
    bool Initialized;
    transform Camera;
    bool GameViewEnabled;
    bool ProfilingEnabled;
    bool ThreadedRendering;
    bool ThreadedRendering_Dynamic;
    bool Visualize_ThreadedRendering;
    int ScreenThreads;

    // Editor
    bool GizmoWorldSpace;
};


enum EntityType
{
    EntityType_Sphere,
    EntityType_Box,
    EntityType_Effect,
    EntityType_Spawn,
    EntityType_Win,
    EntityType_Checkpoint,
    EntityType_Health,
    EntityType_Time,
    EntityType_Max
};
char EntityTypeNames[EntityType_Max][100] = {
    "Type: Sphere",
    "Type: Box",
    "Type: Effect",
    "Type: Spawn",
    "Type: Win",
    "Type: Checkpoint",
    "Type: Health",
    "Type: Time",
};
enum SurfaceType
{
    SurfaceType_Wood,
    SurfaceType_Stone,
    SurfaceType_Metal,
    SurfaceType_Max
};
char SurfaceTypeNames[SurfaceType_Max][100] = {

    "Surface Type: Wood",
    "Surface Type: Stone",
    "Surface Type: Metal"
};

// Fat entity with everything on it.
typedef struct
{
    bool initialized;

    transform t;

    // 3D Object
    bool mesh;
    EntityType type;
    Image* texture1;
    Image* texture2;
    float3 Color;
    int ShaderIndex;

    // Physics
    bool physics;
    bool kinematic;
    float mass;
    float3 velocity; // Linear velocity through space
    float3 angularVelocityAxis; // Axis around which we are rotating
    float angularVelocity; // Speed of that rotation
    float friction;
    float drag;
    bool grounded;
    float3 deIntersectVector; // Vector used for de-intersecting the physics object this frame. larger vector = stronger force.
    void* TouchingEntity; // entity this physics object is touching

    EffectData effect;

    int group;

    SurfaceType surfaceType;

    // Win trigger

    // Spawn point

} Entity;

enum EditorState
{
    EditorState_Editing,
    EditorState_Settings,
    EditorState_Sound,
    EditorState_Playing
};

bool Valid(Image* image)
{
    return image != 0 && image->width > 0 && image->height > 0 && image->ImageData != 0;
}
bool Valid(Image image)
{
    // TODO: (memory safety), Maybe loop through all iamges to make sure the imagedata ptr is valid?
    return image.width > 0 && image.height > 0 && image.ImageData != 0;
}

bool Valid(Entity* e)
{
    return e && e->initialized;
}
bool Valid(Entity e)
{
    return e.initialized;
}
bool ValidPhysics(Entity* e)
{
    return e && e->initialized && e->physics;
}

struct string_data
{
    float* Pos;
    float* Vel;
    float* OldPos;
    float* OldVel;
    int PixelsPerSample;


    float Pos1[200];
    float Vel1[200];
    float Pos2[200];
    float Vel2[200];

    float speed;
    float damping;
    float timescale;
    int Segments;
    float FTScale;

    float Magnitudes[100];
    float soundDelta;
};

static const int TimeSampleCount = 50;
struct ProfilingData
{
    char functionNames[1000][50]; // 100 strings, 50 long
    int64 EnterTime[1000][TimeSampleCount];
    int64 ExitTime[1000][TimeSampleCount];
    int Depth[1000];

    int64 TimeStack[100][TimeSampleCount];

    int frameNumber;
    int currentDepth;
    int index;
    game_memory* Memory;
    bool threading;
};

void ProfilerBeingSample(ProfilingData* data)
{
    if (data->threading)
        return;

    data->TimeStack[data->currentDepth][data->frameNumber] = data->Memory->PlatformTime();
    data->currentDepth++;
}

void ProfilerEndSample(ProfilingData* data, const char* name)
{
    if (data->threading)
        return;

    data->EnterTime[data->index][data->frameNumber] = data->TimeStack[data->currentDepth-1][data->frameNumber];
    data->ExitTime[data->index][data->frameNumber] = data->Memory->PlatformTime();
    for (int i = 0; i < 50; i++)
    {
        data->functionNames[data->index][i] = name[i];
    }
    data->Depth[data->index] = data->currentDepth;
    data->currentDepth--;
    data->index++;
}

struct EngineState
{
    // Image stuff
    MemoryArena textureArena;
    int CurrentImage;
    char ImageNames[50][100]; // 500 kb
    Image Images[50];
    Image* SkyFaces[6];
    Image* fontSpritesheet;

    transform Camera;

    // Sound stuff
    MemoryArena soundArena;
    int CurrentSound;
    char SoundNames[50][100]; // 500 kb
    Sound Sounds[50];

    platform_read_image_into_memory* PlatformReadImageIntoMemory;
    platform_read_sound_into_memory* PlatformReadSoundIntoMemory;
    platform_print* PlatformPrint;

    bool playing;
    int CurrentLevel;

    float GameTime;

    bool Grabbing;
    transform TestSphere;

    bool UseFiltering;

    float deltas[30];
    int deltasIndex;

    float FPS[30];
    int FPSIndex;

    float deltas2[200];
    int deltas2Index;

    Entity Entities[1000]; // Max 1000 entities?
    

    RenderCommand RenderCommands[1000];
    int NextRenderCommand;

    // Threading
    int RenderThreadTimes[100]; // Time it took to draw
    float NormalizedRenderThreadTimes[100];
    float RenderThreadSize[100];
    float RenderThreadStart[100];
    float RenderThreadEnd[100];

    bool Bouncy;
    
    // UI
    int SliderID;
    int PickedSliderID;
    float SliderTestValue;
    bool MouseIsOverUI;

    // Editor
    int MoveGizmoAxisID;
    Entity* HoveredEntity;
    Entity* SelectedEntities[100];
    int GizmoState;


    // sound
    string_data stringData;
    int CurrentSoundSample;
    int ChosenSound;
    int ChosenChannel;
    float ChosenSoundVolume;
    bool ChosenSoundLoop;

    SoundChannel soundChannels[4];
    float MasterVolume = 1.0f;

    EditorState editorState;
    EditorState LasteditorState;


    // can be compiled out for the game itself
    int UndoSteps = 0;
    int UndoIndex = 0;
    bool OperationHappened; // true if some operation happened

    Entity EntityUndoBuffer[10][1000]; // Max 1000 entities?
    char SaveBuffer[Kilobytes(128)]; // block of data for saving the game to.

    ProfilingData profilingData;

    float2 a;
    float2 b;
    float2 c;
    float2 d;

    float PhysicsTimeAccumulator;

    bool renderTest1;
    bool renderTest2;
};

