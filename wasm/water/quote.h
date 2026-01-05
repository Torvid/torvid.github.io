
#pragma once
#include "quote_platform.cpp"


void InitGlobalFunctions(GameMemory* gameMemory)
{
    print = gameMemory->platformPrint;
}

#include "memory.cpp"
//#include "array.cpp"
#include "math.cpp"


struct Sound
{
    int sampleCount;
    char name[100];
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
    char name[100];
    uint32* ImageData;
};


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


 EffectData ctor_EffectData(float spawnRate,
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
    Transform t;
    float3 color;
    Image* texture1;
    Image* texture2;
    Image* cubemapTexture[6];
    int shaderIndex;
    bool selected;
    bool hovered;

    bool isPortal;
    bool isEntryPortal;

    EffectData effect;
};

struct EngineSaveState
{
    bool initialized;
    Transform camera;
    bool gameViewEnabled;
    bool profilingEnabled;
    bool threadedRendering;
    bool threadedRendering_Dynamic;
    bool visualize_ThreadedRendering;
    int screenThreads;
    bool avx2;

    // Editor
    bool gizmoWorldSpace;
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
    EntityType_Portal,
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
    "Type: Portal",
};
enum SurfaceType
{
    SurfaceType_Wood,
    SurfaceType_Stone,
    SurfaceType_Metal,
    SurfaceType_Max
};
char surfaceTypeNames[SurfaceType_Max][100] = {

    "Surface Type: Wood",
    "Surface Type: Stone",
    "Surface Type: Metal"
};

// Fat entity with everything on it.
typedef struct
{
    bool initialized;

    Transform transform;

    // 3D Object
    bool mesh;
    EntityType type;
    Image* texture1;
    Image* texture2;
    float3 color;
    int shaderIndex;

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

static const int timeSampleCount = 30;
struct ProfilingData
{
    char functionNames[1000][50]; // 100 strings, 50 long
    int64 enterTime[1000][timeSampleCount];
    int64 exitTime[1000][timeSampleCount];
    int depth[1000];

    int64 timeStack[100][timeSampleCount];

    int frameNumber;
    int currentDepth;
    int index;
    GameMemory* memory;
    bool threading;
    float zoom = 0;

    int64 Global_Start;
    int64 Global_End;
    int64 Delta;


    float deltas[30];
    int deltasIndex;

    float FPS[30];
    int FPSIndex;

    float deltas2[200];
    int deltas2Index;

};

struct EngineState
{
    MemoryArena staticAssetsArena; // Static assets that never get cleared


    // Image stuff
    //MemoryArena textureArena;
    int currentImage;
    //char imageNames[50][100]; // 500 kb
    //Image images[50];
    ArrayCreate(Image*, images, 50);
    ArrayCreate(Sound*, sounds, 50);

    Image* skyFacesData[6];
    Image* skyFaces[6];
    Image* fontSpritesheet;

    Transform camera;

    // Sound stuff
    //MemoryArena soundArena;
    int currentSound;
    //char soundNames[50][100]; // 500 kb
    //Sound sounds[50];

    PlatformReadImageIntoMemory* platformReadImageIntoMemory;
    PlatformReadSoundIntoMemory* platformReadSoundIntoMemory;
    PlatformPrint* platformPrint;
    PlatformSetForcefeedbackEffect* platformSetForcefeedbackEffect;


    bool playing;
    int currentLevel;

    float gameTime;

    bool grabbing;

    bool useFiltering;

    Entity entities[1000]; // Max 1000 entities?
    
    ArrayCreate(RenderCommand, renderCommands, 1000);


    // Threading
    int renderThreadTimes[100]; // Time it took to draw
    float normalizedRenderThreadTimes[100];
    float renderThreadSize[100];
    float renderThreadStart[100];
    float renderThreadEnd[100];

    // UI
    int sliderID;
    int pickedSliderID;
    float sliderTestValue;
    bool mouseIsOverUI;

    // Editor
    int moveGizmoAxisID;
    Entity* hoveredEntity;
    Entity* selectedEntities[100];
    int gizmoState;


    // sound
    string_data stringData;
    int currentSoundSample;
    int chosenSound;
    int chosenChannel;
    float chosenSoundVolume;
    bool chosenSoundLoop;

    SoundChannel soundChannels[4];
    float masterVolume = 1.0f;

    EditorState editorState;
    EditorState lasteditorState;


    int undoSteps = 0;
    int undoIndex = 0;
    bool operationHappened; // true if some operation happened

    Entity entityUndoBuffer[10][1000]; // Max 1000 entities?
    char saveBuffer[Kilobytes(128)]; // block of data for saving the game to.

    ProfilingData profilingData;

    float PhysicsTimeAccumulator;

    bool renderTest1;
    bool renderTest2;
};

#include "random.cpp"
#include "string.cpp"
#include "profiling.cpp"
#include "raytracing.cpp"
#include "drawing2d.cpp"
#include "drawing.cpp"
#include "physics.cpp"
#include "sound.cpp"
#include "ui.cpp"
//#include "editor.cpp"
