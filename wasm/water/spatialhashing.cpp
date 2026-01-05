#pragma once
#include "quote.h"

struct Point
{
    float2 position;
    float2 velocity;
    uint32 geohash;
};

const int edgeLength = 20;
const int cellCount = edgeLength * edgeLength;
const int particleCount = 500;
const int cellMaxSize = 500;

struct GridCell
{
    int lastIndex;
    Point* nodes[cellMaxSize];
    bool checked;
};

struct Scene
{
    Point points[particleCount];
    GridCell cells[cellCount];
};

int hash(float2 pos)
{
    float2 posNormalized = floor((pos / 500.0f) * edgeLength);
    return posNormalized.x + posNormalized.y * edgeLength;
}

bool CellIsFull(GridCell* cell)
{
    return cell->lastIndex >= cellMaxSize;
}

struct Scene2
{
    Point points[particleCount];
    int16 links[particleCount];
    int16 cellStartingLink[cellCount];
    int16 cellLastLink[cellCount];

    //wPoint points[particleCount];
    //int16 links[particleCount];
    //int16 cellStartingLink[cellCount];
    //int16 cellLastLink[cellCount];
    //
    ArrayCreate(float2, getNearbyResult, particleCount);
    float radius;
};


void BuildAccelerationStructure(EngineState* engineState, Scene2* scene)
{
    ProfilerEndSample(&engineState->profilingData, "Build Structure");
    // build acceleration structure
    for (int i = 0; i < ArrayCapacity(scene->cellStartingLink); i++)
    {
        scene->cellStartingLink[i] = -1;
        scene->cellLastLink[i] = -1;
    }
    for (int i = 0; i < ArrayCapacity(scene->points); i++)
    {
        scene->links[i] = -1;
    }

    for (int i = 0; i < ArrayCapacity(scene->points); i++)
    {
        //int h = hash(scene->points[i].position);
        float2 posNormalized = floor((scene->points[i].position / 500.0f) * edgeLength);
        int h = posNormalized.x + posNormalized.y * edgeLength;

        if (scene->cellStartingLink[h] == -1)
        {
            scene->cellStartingLink[h] = i;
            scene->cellLastLink[h] = i;
        }
        else
        {
            scene->links[scene->cellLastLink[h]] = i;
            scene->cellLastLink[h] = i;
        }
    }

    ProfilerBeingSample(&engineState->profilingData);
}


void GetNearby(EngineState* engineState, Scene2* scene, float2 pos, float radius)
{
    ArrayClear(scene->getNearbyResult);
    // for each cell in range
    float2 start = (pos - radius) / 500.0f * edgeLength;
    float2 end = (pos + radius) / 500.0f * edgeLength + 1;
    int startX = clamp(start.x, 0, edgeLength);
    int startY = clamp(start.y, 0, edgeLength);
    int endX = clamp(end.x, 0, edgeLength);
    int endY = clamp(end.y, 0, edgeLength);

    for (int x = startX; x < endX; x++)
    {
        for (int y = startY; y < endY; y++)
        {
            for (int next = scene->cellStartingLink[x + y * edgeLength]; next != -1; next = scene->links[next])
            {
                ArrayAdd(scene->getNearbyResult, scene->points[next].position);
            }
        }
    }
}

struct NearbyData
{
    int startX;
    int startY;
    int endX;
    int endY;
    int x;
    int y;
    int next;
};


NearbyData GetNextNearbySetup(EngineState* engineState, Scene2* scene, float2 pos, float radius)
{
    NearbyData result = {};
    // for each cell in range
    float2 start = (pos - radius) / 500.0f * edgeLength;
    float2 end = (pos + radius) / 500.0f * edgeLength + 1;
    (result.startX) = clamp(start.x, 0, edgeLength);
    (result.startY) = clamp(start.y, 0, edgeLength);
    (result.endX) = clamp(end.x, 0, edgeLength);
    (result.endY) = clamp(end.y, 0, edgeLength);

    (result.x) = result.startX;
    (result.y) = result.startY;
    (result.next) = scene->cellStartingLink[(result.x) + (result.y) * edgeLength];

    return result;
}
// gets the next nearby particle
bool GetNextNearby(EngineState* engineState, Scene2* scene, float2 pos, float radius, NearbyData* data, float2* result)
{
    if ((data->x) >= (data->endX))
    {
        return false;
    }

    if ((data->y) >= (data->endY))
    {
        return false;
    }

    while ((data->next) == -1)
    {
        (data->y)++;
        if ((data->y) >= (data->endY))
        {
            (data->y) = (data->startY);
            (data->x)++;
            if ((data->x) >= (data->endX))
            {
                return false;
            }
        }
        (data->next) = scene->cellStartingLink[(data->x) + (data->y) * edgeLength];
    }

    (*result) = scene->points[(data->next)].position;
    (data->next) = scene->links[data->next];

    return true;
}

// This function locks the current thread until all other threads are done execcuting.
void SpinLockUntilAllThreadsAreDone(GameMemory* memory, int threadCount)
{
    while (true)
    {
        bool anyExecuting = false;
        for (int i = 0; i < threadCount; i++)
        {
            if (memory->threadsExecuting[i] == true)
            {
                anyExecuting = true;
            }
            else
            {
            }
        }

        // If no threads are executing, we are done.
        if (!anyExecuting)
            break;
    }
}

int simThreadCount = 16;

void UpdateVelocities(int thread, GameMemory* memory, VideoBuffer* videoBuffer, game_sound_output_buffer* SoundBuffer)
{
    //char text[100] = {};
    //Append(text, "thread: ");
    //Append(text, thread);
    //print(text);
    EngineState* engineState = (EngineState*)memory->transientStorage;
    Scene2* scene = (Scene2*)(((uint8*)(engineState + 1)));


    float speed = 0.002f;
    float radiusSquared = scene->radius * scene->radius;
    int pointsPerGroup = ArrayCapacity(scene->points) / simThreadCount;
    int start = pointsPerGroup * thread;
    int end = pointsPerGroup * (thread + 1);

    for (int i = start; i < end; i++)
    {
        float2 result;
        NearbyData data = GetNextNearbySetup(engineState, scene, scene->points[i].position, scene->radius);
        while (GetNextNearby(engineState, scene, scene->points[i].position, scene->radius, &data, &result))
        {
            float2 delta = (result - scene->points[i].position);
            float dist = dot(delta, delta);

            if (dist == 0.0)
            {
                continue;
            }

            if (dist < (radiusSquared))
            {
                float2 deltaNormalized = (delta * delta * sign(delta)) / (dist);
                //float2 deltaNormalized = delta / sqrt(dist);
                scene->points[i].velocity += (deltaNormalized * speed);
            }
        }
    }
}

uint32 PosToGeohash(float2 pos, int depth)
{
    //float2 pos = (float2(x, y) / edgeLength * 500.0f);
    //int p = 2;
    //float2 size = (float2(500 / edgeLength, 500 / edgeLength)) - p * 2;
    //char bits2[17] = "0000000000000000";
    uint32 result = 0;
    int lineHorizontal = 250;
    int lineVertical = 250;
    int splitter = 250;
    for (int i = 0; i < depth; i++)
    {
        splitter /= 2.0f;

        bool off = (pos.x > lineHorizontal);
        lineHorizontal += splitter * (off ? 1 : -1);
        if (off)
            result |= 1 << (i * 2);

        off = (pos.y > lineVertical);
        lineVertical += splitter * (off ? 1 : -1);
        if(off)
            result |= 1 << (i * 2 + 1);
    }
    return result;
}

int GetHammingDistance(uint32 a, uint32 b)
{
    uint32 q = a ^ b;
    return popcount(q); // fast has heck hamming distance
}

void spatialhashing(GameMemory* memory, EngineState* engineState, GameInput* input, VideoBuffer* videoBuffer)
{

    ProfilerBeingSample(&engineState->profilingData);


    //WaveFunctionCollapseState* game = (WaveFunctionCollapseState*)((uint8*)memory->TransientStorage + sizeof(EngineState));
    //wavefunctioncollapse(memory, engineState, game, input, videoBuffer);
    //array_test(memory);
    //return;

    float2 mousePos = float2(input->MouseX, input->MouseY);

#define simtype_linked 0
#define simtype_buckets 1
#define simtype_naiive 2
#define simtype_geohash 3
#define simtype simtype_buckets 

    DrawClear(videoBuffer, float3(0.5, 0, 0));

#if simtype == simtype_buckets || simtype == simtype_naiive || simtype_geohash
    Scene* scene = (Scene*)(((uint8*)(engineState + 1)));
#elif simtype == simtype_linked
    Scene2* scene = (Scene2*)(((uint8*)(engineState + 1)));
#endif

    if (input->SpaceDown)
        memory->initialized = false;

    if (!memory->initialized)
    {
        engineState->profilingData.zoom = 1;
        engineState->platformReadImageIntoMemory = memory->platformReadImageIntoMemory;
        engineState->platformReadSoundIntoMemory = memory->platformReadSoundIntoMemory;
        engineState->platformPrint = memory->platformPrint;
        InitializeArena(&engineState->staticAssetsArena, Megabytes(12), (uint8*)memory->transientStorage + sizeof(EngineState) + sizeof(Scene) + Kilobytes(100));
        engineState->fontSpritesheet = LoadImage(engineState, &engineState->staticAssetsArena, "font.tga", 192, 52);
        StructClear(scene);

        RandomNumberIndex = 0;
        //for (int i = 0; i < wparticleCount / 2; i++)
        //{
        //    scene->points[i].position = float2(GetNextRandomProcedual() % 200, GetNextRandomProcedual() % 200);
        //    scene->points[i].velocity = float2(3, 5)*2;
        //}
        //for (int i = wparticleCount / 2; i < wparticleCount; i++)
        //{
        //    scene->points[i].position = float2(GetNextRandomProcedual() % 200 + 300, GetNextRandomProcedual() % 200 + 300);
        //    scene->points[i].velocity = float2(-3, -5)*2;
        //} 

        for (int i = 0; i < ArrayCapacity(scene->points); i++)
        {
            scene->points[i].position = float2(GetNextRandomProcedual() % 500, GetNextRandomProcedual() % 500);
        }

        memory->initialized = true;
    }
    if (input->QDown)
    {
        for (int i = 0; i < ArrayCapacity(scene->points); i++)
        {
            scene->points[i].velocity = float2(0, 0);
        }
    }


    ProfilerBeingSample(&engineState->profilingData);
    float radius = 50;
    float speed = 0.005f;
    //if (input->ODown)
    {
        //for (int o = 0; o < 100; o++)
        {
#if simtype == simtype_linked
            ProfilerBeingSample(&engineState->profilingData);

            for (int i = 0; i < ArrayCapacity(scene->cellStartingLink); i++)
            {
                scene->cellStartingLink[i] = -1;
                scene->cellLastLink[i] = -1;
            }
            for (int i = 0; i < ArrayCapacity(scene->points); i++)
            {
                scene->links[i] = -1;
            }

            // build acceleration structure
            for (int i = 0; i < ArrayCapacity(scene->points); i++)
            {
                int h = hash(scene->points[i].position);

                if (scene->cellStartingLink[h] == -1)
                {
                    scene->cellStartingLink[h] = i;
                    scene->cellLastLink[h] = i;
                }
                else
                {
                    scene->links[scene->cellLastLink[h]] = i;
                    scene->cellLastLink[h] = i;
                }
            }

            ProfilerEndSample(&engineState->profilingData, "Build Structure");
            ProfilerBeingSample(&engineState->profilingData);
            // Do physics!
            for (int i = 0; i < ArrayCapacity(scene->points); i++)
            {
                // for each cell in range
                float2 start = (scene->points[i].position - radius) / 500.0f * edgeLength;
                float2 end = (scene->points[i].position + radius) / 500.0f * edgeLength;
                start = clamp(start, 0, edgeLength);
                end = clamp(end, 0, edgeLength);
                for (int x = start.x; x < end.x; x++)
                {
                    for (int y = start.y; y < end.y; y++)
                    {
                        int j = x + y * edgeLength;
                        int next = scene->cellStartingLink[j];
                        while (next != -1)
                        {
                            if (&scene->points[next] == &scene->points[i])
                            {
                                next = scene->links[next];
                                continue;
                            }

                            if (distance(scene->points[next].position, scene->points[i].position) < 0.001)
                            {
                                next = scene->links[next];
                                continue;
                            }

                            if (distance(scene->points[next].position, scene->points[i].position) < radius)
                            {
                                scene->points[i].velocity += normalize(scene->points[next].position - scene->points[i].position) * speed;
                            }

                            next = scene->links[next];
                        }
                    }
                }
            }
            ProfilerEndSample(&engineState->profilingData, "Simulate");

#elif simtype == simtype_buckets
            ProfilerBeingSample(&engineState->profilingData);
            // build acceleration structure
            Clear((uint8*)&scene->cells, sizeof(scene->cells));

            for (int i = 0; i < ArrayCapacity(scene->points); i++)
            {
                int h = hash(scene->points[i].position);
                GridCell* cell = &scene->cells[h];

                // If it's overflowed, check next cells until we find one that has free space
                while (CellIsFull(cell))
                {
                    h++;
                    h %= cellCount;
                    cell = &scene->cells[h];
                }

                // add this to the cell
                cell->nodes[cell->lastIndex] = &scene->points[i];
                cell->lastIndex++;
                //AddNode(scene, &scene->points[i], scene->points[i].position);
            }
            ProfilerEndSample(&engineState->profilingData, "Build Structure");
            ProfilerBeingSample(&engineState->profilingData);

            // for each point
            for (int i = 0; i < ArrayCapacity(scene->points); i++)
            {
                // Reset the checked flags
                // Array marking what cells have been checked already for this particle.
                bool checked[cellCount] = {};

                // for each cell in range
                float2 start = (scene->points[i].position - radius) / 500.0f * edgeLength;
                float2 end = (scene->points[i].position + radius) / 500.0f * edgeLength;
                start = clamp(start, 0, edgeLength);
                end = clamp(end, 0, edgeLength);
                for (int x = start.x; x < end.x; x++)
                {
                    for (int y = start.y; y < end.y; y++)
                    {
                        int j = x + y * edgeLength;

                        GridCell* cell = &scene->cells[j];
                        while (true)
                        {
                            if (checked[j])
                                break;

                            checked[j] = true;

                            for (int k = 0; k < cell->lastIndex; k++)
                            {
                                Point* point = cell->nodes[k];

                                if (point == &scene->points[i])
                                    continue;
                                if (distance(point->position, scene->points[i].position) < 0.001)
                                    continue;

                                if (distance(point->position, scene->points[i].position) < radius)
                                {
                                    //float dist = distance(point->position, scene->points[i].position);
                                    //float force = 1.0f / (dist);
                                    scene->points[i].velocity += normalize(point->position - scene->points[i].position) * speed;
                                }
                            }
                            if (CellIsFull(cell))
                                cell = &scene->cells[j++];
                            else
                                break;
                        }
                    }
                }
            }
            ProfilerEndSample(&engineState->profilingData, "Simulate");
#elif simtype == simtype_naiive
            for (int i = 0; i < ArrayCapacity(scene->points); i++)
            {
                for (int j = 0; j < ArrayCapacity(scene->points); j++)
                {
                    if (i == j)
                        continue;

                    if (distance(scene->points[i].position, scene->points[j].position) < 0.001)
                        continue;

                    if (distance(scene->points[i].position, scene->points[j].position) < radius)
                    {
                        scene->points[i].velocity += normalize(scene->points[j].position - scene->points[i].position) * speed;
                    }
                }
            }
#elif simtype == simtype_geohash
#endif
            for (int i = 0; i < ArrayCapacity(scene->points); i++)
            {
                if (input->MouseLeft)
                {
                    float2 delta = mousePos - scene->points[i].position;
                    float dist = length(delta);
                    scene->points[i].velocity += (normalize(delta) / dist) * 10.0f;
                }
                //scene->points[i].velocity *= 0.999f;
                scene->points[i].position += scene->points[i].velocity;

                if (scene->points[i].position.x < 0)
                    scene->points[i].velocity.x = (abs(scene->points[i].velocity.x) * 0.5f);
                if (scene->points[i].position.x >= 500)
                    scene->points[i].velocity.x = (-abs(scene->points[i].velocity.x) * 0.5f);

                if (scene->points[i].position.y < 0)
                    scene->points[i].velocity.y = (abs(scene->points[i].velocity.y) * 0.5f);
                if (scene->points[i].position.y >= 500)
                    scene->points[i].velocity.y = (-abs(scene->points[i].velocity.y) * 0.5f);


                scene->points[i].position = clamp(scene->points[i].position, float2(0, 0), float2(499.99f, 499.99f));
            }

        }
    }
    ProfilerEndSample(&engineState->profilingData, "Simulate");

    // Draw the grid 
    for (int x = 0; x < edgeLength; x++)
    {
        for (int y = 0; y < edgeLength; y++)
        {
            int i = x + y * edgeLength;
            int p = 2;
            float2 pos = (float2(x, y) / edgeLength * 500.0f) + p;
            float2 size = (float2(500 / edgeLength, 500 / edgeLength)) - p * 2;
            //DrawRectangle(videoBuffer, pos, size, float3(0, 0.5, 0));
        }
    }

    //for (int i = 0; i < ArrayCapacity(scene->points); i++)
    {
        float depth = 8;
        uint32 mouseHash = PosToGeohash(mousePos, depth);
        int a = 5;
        int el = 500;
        for (int x = 0; x < el; x++)
        {
            for (int y = 0; y < el; y++)
            {
                //int p = 0;
                //float2 pos = (float2(x, y) / el * 500.0f)+p;
                //float2 size = float2(1, 1);// (float2(500 / el, 500 / el)) - p * 2;
                
                uint32 result = PosToGeohash(float2(x,y), depth);
                
                int dist = GetHammingDistance(mouseHash, result);
                uint32 c = MakeBufferColor(videoBuffer, float3(0, dist / (depth*2), 0));
                SetPixel(videoBuffer, x, y, c);

                //DrawRectangle(videoBuffer, pos, size, float3(0, dist / 4.0f, 0));
                //Text(engineState, videoBuffer, pos, bits2);
            }
        }
        Text(engineState, videoBuffer, float2(0,0), "", mouseHash);
        //
    }
    for (int i = 0; i < ArrayCapacity(scene->points); i++)
    {
        DrawPoint(videoBuffer, scene->points[i].position);
        //DrawLine(videoBuffer, wire->positions[i-1], wire->positions[i]);
    }

    for (int x = 0; x < edgeLength; x++)
    {
        for (int y = 0; y < edgeLength; y++)
        {
            int i = x + y * edgeLength;
            float fullness = 0;
#if simtype == simtype_buckets
            fullness = ((float)scene->cells[i].lastIndex) / ((float)ArrayCapacity(scene->cells[i].nodes));
#endif
            int p = 2;
            float2 pos = (float2(x, y) / edgeLength * 500.0f) + p;
            float2 size = (float2(500 / edgeLength, 500 / edgeLength)) - p * 2;
            DrawRectangle(videoBuffer, pos + float2(500 / edgeLength / 2, 0), float2(4, size.y * fullness), float3(0, 0, 1));
        }
    }

    DrawCircle(videoBuffer, mousePos, radius);

    //ProfilerBeingSample(&engineState->profilingData);
    //for (int i = 0; i < simThreadCount; i++)
    //{
    //    memory->PlatformThreadCall(&UpdateVelocities, i);
    //}
    //SpinLockUntilAllThreadsAreDone(Memory, simThreadCount);
    //ProfilerEndSample(&engineState->profilingData, "Threads");



    //for (int i = 0; i < ArrayCapacity(scene->points); i++)
    //{
    //    float2 result;
    //    NearbyData data = GetNextNearbySetup(engineState, scene, scene->points[i].position, scene->radius);
    //    while (GetNextNearby(engineState, scene, scene->points[i].position, scene->radius, &data, &result))
    //    {
    //        float2 delta = (result - scene->points[i].position);
    //        float dist = dot(delta, delta);
    //
    //        if (dist == 0.0)
    //        {
    //            continue;
    //        }
    //
    //        if (dist < (radiusSquared))
    //        {
    //            float2 deltaNormalized = (delta * delta * sign(delta)) / (dist);
    //            scene->points[i].velocity += (deltaNormalized * speed);
    //        }
    //    }
    //}


    // Mouse Force
    //for (int i = 0; i < ArrayCapacity(scene->points); i++)
    //{
    //    //if (input->MouseLeft)
    //    //{
    //    //    float2 delta = mousePos - scene->points[i].position;
    //    //    float dist = length(delta);
    //    //    scene->points[i].velocity += (normalize(delta) / dist) * 50000.0f * speed;
    //    //}
    //    //scene->points[i].velocity *= 0.99f;
    //    scene->points[i].position += scene->points[i].velocity;
    //
    //    if (scene->points[i].position.x < 0)
    //        scene->points[i].velocity.x = (abs(scene->points[i].velocity.x) * 0.5f);
    //    if (scene->points[i].position.x >= 500)
    //        scene->points[i].velocity.x = (-abs(scene->points[i].velocity.x) * 0.5f);
    //
    //    if (scene->points[i].position.y < 0)
    //        scene->points[i].velocity.y = (abs(scene->points[i].velocity.y) * 0.5f);
    //    if (scene->points[i].position.y >= 500)
    //        scene->points[i].velocity.y = (-abs(scene->points[i].velocity.y) * 0.5f);
    //
    //    scene->points[i].position = clamp(scene->points[i].position, float2(0, 0), float2(499.99f, 499.99f));
    //}

    ProfilerEndSample(&engineState->profilingData, "Frame");
    ProfilerDrawFlameChart(input, engineState, videoBuffer);

}