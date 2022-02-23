
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

#include "intrinsics.cpp"
#include "quote_tile.cpp"
#include "random.cpp"
#include "font.cpp"

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

struct SaveState
{
    bool Initialized;
    transform Camera;
    bool GameViewEnabled;
    bool ThreadedRendering;
    bool ThreadedRendering_Dynamic;
    bool Visualize_ThreadedRendering;
    int ScreenThreads;
    int CurrentLevel;

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
    EntityType_Player,
    EntityType_Checkpoint,
    EntityType_Health,
    EntityType_Max
};
char EntityTypeNames[EntityType_Max][100] = {
    "Type: Sphere",
    "Type: Box",
    "Type: Effect",
    "Type: Spawn",
    "Type: Win",
    "Type: Player",
    "Type: Checkpoint",
    "Type: Health",
};

//typedef float3 SurfaceShader(Image* texture1, Image* texture2, float3 Color, float3 UV, float depth, float3 normal, float3 viewDirection, bodytype type);


#define SURFACE_SHADER(name) float3 name(game_offscreen_buffer* VideoBuffer, Image* texture1, Image* texture2, float3 Color, float3 LocalPos, float depth, float3 normal, float3 normalLocal, float3 viewDirection, EntityType type, float3 scale)
typedef SURFACE_SHADER(SurfaceShader); // template function


//typedef float3 SurfaceShader(Image* texture1, Image* texture2, float3 Color, float3 UV, float depth, float3 normal, float3 viewDirection, bodytype type)
//typedef SURFACE_SHADER(SurfaceShader); // template function

SURFACE_SHADER(SurfaceShaderExample)
{
    return float3(0.5, 0.0f, 0.0f);
}

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

    EffectData effect;
    // Particle Effect
    //float spawnRate;
    //float spawnRadius;
    //float3 minSpeed;
    //float3 maxSpeed;
    //float startSize;
    //float endSize;

    // Win trigger

    // Spawn point


} Entity;

enum EditorState
{
    EditorState_Editing,
    EditorState_Profiling,
    EditorState_Sound
};

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


struct GameState
{
    // Image stuff
    MemoryArena textureArena;
    int CurrentImage;
    Image Images[50];
    Image* SkyFaces[6];
    Image* fontSpritesheet;

    // Sound stuff
    MemoryArena soundArena;
    int CurrentSound;
    Sound Sounds[50];


    bool playing;

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

    Entity Entities[100]; // Max 100 entities?
    

    RenderCommand RenderCommands[100];
    int NextRenderCommand;

    // Threading
    int RenderThreadTimes[100]; // Time it took to draw
    float NormalizedRenderThreadTimes[100];
    float RenderThreadSize[100];
    float RenderThreadStart[100];
    float RenderThreadEnd[100];

    bool Bouncy;
    char SaveBuffer[Kilobytes(32)]; // block of data for saving the game to.
    
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
};

