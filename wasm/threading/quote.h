
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
    RenderCommandType_Clear
};

struct RenderCommand
{
    RenderCommandType Type;
    transform t;
    float3 Color;
    Image* Texture;
    Image* CubemapTexture[6];
};

struct SaveState
{
    bool Initialized;
    transform Camera;
    bool GameViewEnabled;
    bool ThreadedRendering;
    bool ThreadedRendering_Dynamic;
    int ScreenThreads;
};

struct GameState
{
    MemoryArena worldArena;
    MemoryArena textureArena;
    Image* fontSpritesheet;
    int CurrentImage;
    Image Images[50];
    Image* SkyFaces[6];
    bool playing;

    int step;
    float tSine;
    float tSine2;
    float GameTime;

    bool RayStartGrabbed;
    bool RayDirGrabbed;
    float RayStartX;
    float RayStartY;
    float RayDirX;
    float RayDirY;

    bool Grabbing;
    transform TestSphere;

    bool UseFiltering;

    bool PrefViewEnabled;
    float deltas[30];
    int deltasIndex;

    float FPS[30];
    int FPSIndex;

    float deltas2[200];
    int deltas2Index;

    TileMapPosition PlayerP;

    RigidBody2D Bodies2D[100];
    
    RigidBody3D Bodies3D[100];

    //transform FastDrawCubes[25];

    RenderCommand RenderCommands[100];
    int NextRenderCommand;

    // Threading
    int RenderThreadTimes[100]; // Time it took to draw
    float NormalizedRenderThreadTimes[100]; // Time it took to draw
    float RenderThreadSize[100]; // Time it took to draw
    float RenderThreadStart[100]; // Time it took to draw
    float RenderThreadEnd[100]; // Time it took to draw
    //float2 LastSpherePos;
    //float2 SpherePos;
    //float2 SphereVel;
    //float2 SphereUp;
    //float2 SphereRight;
    //float AngularVel;
    bool Bouncy;

};
