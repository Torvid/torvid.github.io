#pragma once
#include "quote.h"

struct wPoint
{
    float2 position;
    float2 velocity;
};


const int wedgeLength = 50;
const int wcellCount = wedgeLength * wedgeLength;
const int wparticleCount = 500;

char WirefunToolNames[4][100] = { "None", "PlaceComponent", "PlaceWire", "DragWire" };

enum WirefunTool
{
    WirefunTool_None,
    WirefunTool_PlaceComponent,
    WirefunTool_PlaceWire,
    WirefunTool_DragWire,
    WirefunTool_Max,
};

struct Component
{
    float2 position;
};

struct WirePoint
{
    float2 position;
    float2 velocity;
};

struct Wire
{
    float2 start;
    float2 end;
    ArrayCreate(WirePoint, points, 100);
    float tension;
};

void DrawWire(VideoBuffer* videoBuffer, Wire* wire, float offset = 0)
{
    //DrawLine(videoBuffer, wire->start, wire->end);
    for (int i = 1; i < ArrayCount(wire->points); i++)
    {
        DrawBox(videoBuffer, wire->points[i].position, float2(1, 1));

        float2 tangent = normalize(wire->points[i - 1].position - wire->points[i].position);
        tangent = float2(-tangent.y, tangent.x) * offset;

        DrawLine(videoBuffer, wire->points[i - 1].position + tangent, wire->points[i].position + tangent);
    }
}

void DrawComponent(VideoBuffer* videoBuffer, Component* component)
{
    DrawBox(videoBuffer, component->position, float2(10, 10));
}

struct Scene3
{
    WirefunTool currentTool;

    ArrayCreate(Component, compontents, 10);
    ArrayCreate(Wire, wires, 10);
    Wire* activeWire;
    int activeWireIndex;
    WirePoint* selectedWirePoint;
    Wire* selectedWire;
};
void wirefun(GameMemory* memory, EngineState* engineState, GameInput* input, VideoBuffer* videoBuffer)
{
    ProfilerBeingSample(&engineState->profilingData);

    float2 mousePos = float2(input->MouseX, input->MouseY);
    Scene3* scene = (Scene3*)(((uint8*)(engineState + 1)));

    if (input->SpaceDown)
        memory->initialized = false;

    if (!memory->initialized)
    {
        engineState->profilingData.zoom = 1;
        engineState->platformReadImageIntoMemory = memory->platformReadImageIntoMemory;
        engineState->platformReadSoundIntoMemory = memory->platformReadSoundIntoMemory;
        engineState->platformPrint = memory->platformPrint;
        InitializeArena(&engineState->staticAssetsArena, Megabytes(12), (uint8*)memory->transientStorage + sizeof(EngineState) + sizeof(Scene3) + Kilobytes(100));
        engineState->fontSpritesheet = LoadImage(engineState, &engineState->staticAssetsArena, "font.tga", 192, 52);
        StructClear(scene);

        memory->initialized = true;
    }


    DrawClear(videoBuffer, float3(0.5, 0.5, 0));
    float2 ui = float2(500, 5);

    Text(engineState, videoBuffer, &ui, WirefunToolNames[scene->currentTool]);

    for (int i = 0; i < WirefunTool_Max; i++)
    {
        if (Button(engineState, input, videoBuffer, &ui, WirefunToolNames[i]))
        {
            scene->currentTool = (WirefunTool)i;
        }
    }

    for (int i = 0; i < ArrayCount(scene->wires); i++)
    {
        DrawWire(videoBuffer, &scene->wires[i]);
        Text(engineState, videoBuffer, &ui, "Wire ", i);
    }
    for (int i = 0; i < ArrayCount(scene->compontents); i++)
    {
        DrawComponent(videoBuffer, &scene->compontents[i]);
        Text(engineState, videoBuffer, &ui, "Component ", i);
    }

    

    float wireParticleDistance = 10.0f;

    if (mousePos.x < 500 && mousePos.y < 500)
    {
        // get closest component
        Component* closestComponent = 0;
        Component* hoveredComponent = 0;
        float lowestDist = 9999;
        int closestComponentIndex = -1;
        int hoveredComponentIndex = -1;

        for (int i = 0; i < ArrayCount(scene->compontents); i++)
        {
            float dist = distance(mousePos, scene->compontents[i].position);
            if (dist < lowestDist)
            {
                lowestDist = dist;
                closestComponent = &scene->compontents[i];
                closestComponentIndex = i;
            }
        }
        if (closestComponent && lowestDist < 12)
        {
            hoveredComponent = closestComponent;
            hoveredComponentIndex = closestComponentIndex;
        }
        if (hoveredComponent)
        {
            DrawBox(videoBuffer, hoveredComponent->position, float2(14, 14));
        }

        // get closest wire point


        WirePoint* closestWirePoint = 0;
        Wire* hoveredWire = 0;
        bool closestWirePointIsStartOrEnd = false;
        lowestDist = 9999;
        for (int i = 0; i < ArrayCount(scene->wires); i++)
        {
            for (int j = 0; j < ArrayCount(scene->wires[i].points); j++)
            {
                float dist = distance(mousePos, scene->wires[i].points[j].position);
                if (dist < lowestDist && dist < 12)
                {
                    lowestDist = dist;
                    closestWirePoint = &scene->wires[i].points[j];
                    hoveredWire = &scene->wires[i];
                    if (j == 0 || j == (ArrayCount(scene->wires[i].points)-1))
                        closestWirePointIsStartOrEnd = true;
                }
            }
        }
        if (closestWirePoint)
        {
            float size = 5;
            if (closestWirePointIsStartOrEnd)
                size = 10;
            DrawBox(videoBuffer, closestWirePoint->position, float2(size, size));
        }

        Wire* HighlightWire = scene->selectedWire;
        if (!HighlightWire)
            HighlightWire = hoveredWire;
        if (HighlightWire)
        {
            DrawWire(videoBuffer, HighlightWire, 1);
            //DrawWire(videoBuffer, HighlightWire, -1);
        }

        //if (scene->currentTool == WirefunTool_PlaceComponent)
        //{
        //    if (input->MouseLeftDown)
        //    {
        //        if (hoveredComponent)
        //        {
        //            ArrayRemove(scene->compontents, hoveredComponentIndex);
        //        }
        //        else
        //        {
        //            Component comp;
        //            comp.position = mousePos;
        //            if (!ArrayFull(scene->compontents))
        //                ArrayAdd(scene->compontents, comp);
        //        }
        //    }
        //    if (!ArrayFull(scene->compontents))
        //        DrawBox(videoBuffer, mousePos, float2(10, 10));
        //}
        //
        //
        //if (scene->currentTool == WirefunTool_PlaceWire)
        //{
            if (input->Shift)
            {
                // Mouse down on the hovered component
                if (input->MouseLeftDown && !ArrayFull(scene->wires))
                {
                    scene->activeWire = ArrayAddIndex(scene->wires);
                    if (hoveredComponent)
                        scene->activeWire->start = hoveredComponent->position;
                    else
                        scene->activeWire->start = mousePos;

                    WirePoint* point = ArrayAddIndex(scene->activeWire->points);
                    point->position = scene->activeWire->start;
                }
                if (input->MouseLeft && scene->activeWire)
                {
                    WirePoint* lastPoint = ArrayGetLast(scene->activeWire->points);
                    if (distance(lastPoint->position, mousePos) > wireParticleDistance && !ArrayFull(scene->activeWire->points))
                    {
                        WirePoint* point = ArrayAddIndex(scene->activeWire->points);
                        point->position = MoveTowards(lastPoint->position, mousePos, wireParticleDistance);
                    }
                    scene->activeWire->end = mousePos;
                }
                if (input->MouseLeftUp && scene->activeWire)
                {
                    if (hoveredComponent)
                    {
                        scene->activeWire->end = hoveredComponent->position;
                    }
                    else
                    {

                    }
                    if (!ArrayFull(scene->activeWire->points))
                    {
                        WirePoint* point = ArrayAddIndex(scene->activeWire->points);
                        point->position = mousePos;
                    }
                    scene->activeWire = 0;
                }
            }
            else
            {
                if (input->MouseLeftDown && closestWirePoint)
                {
                    scene->selectedWirePoint = closestWirePoint;
                    scene->selectedWire = hoveredWire;
                }
                if (input->MouseLeft && scene->selectedWirePoint)
                {
                    scene->selectedWirePoint->position = mousePos;
                }
                if (input->MouseLeftUp)
                {
                    scene->selectedWirePoint = 0;
                    scene->selectedWire = 0;
                }
            }
        //}
    }

    ProfilerBeingSample(&engineState->profilingData);

    for (int q = 0; q < 100; q++)
    {
        if (input->MouseLeft && scene->selectedWirePoint && !input->Shift)
        {
            scene->selectedWirePoint->position = mousePos;
        }

        for (int i = 0; i < ArrayCount(scene->wires); i++)
        {
            for (int j = 1; j < ArrayCount(scene->wires[i].points)-1; j++)
            {
                float2 prevPos = scene->wires[i].points[j-1].position;
                float2 currentPos = scene->wires[i].points[j].position;
                float2 nextPos = scene->wires[i].points[j+1].position;


                float2 p1 = MoveTowards(prevPos, currentPos, wireParticleDistance);
                float2 p2 = MoveTowards(nextPos, currentPos, wireParticleDistance);
                float2 targetPos = (p1 + p2) * 0.5f;
                float dist = distance(targetPos, currentPos);

                if (dist > 0.1f)
                    scene->wires[i].points[j].position = targetPos;
            }
        }
    //}
    //
    //// measure tension
    ////for (int i = 0; i < ArrayCount(scene->wires); i++)
    ////{
    ////    float expectedLength = ArrayCount(scene->wires[i].points) * wireParticleDistance;
    ////    float length = 0;
    ////    for (int j = 1; j < ArrayCount(scene->wires[i].points); j++)
    ////    {
    ////        length += distance(scene->wires[i].points[j - 1].position, scene->wires[i].points[j].position);
    ////    }
    ////    Text(engineState, videoBuffer, scene->wires[i].points[0].position, "tension: ", length- expectedLength);
    ////    scene->wires[i].tension = length - expectedLength;
    ////}
    //
    //
    //
    //// physics!
    //for (int q = 0; q < 10; q++)
    //{

    for (int i = 0; i < ArrayCount(scene->wires); i++)
    {
        for (int j = 0; j < ArrayCount(scene->wires); j++)
        {
            Wire* wireI = &scene->wires[i];
            Wire* wireJ = &scene->wires[j];
            //if (wireI == wireJ)
            //    continue;

            for (int k = 1; k < ArrayCount(wireI->points)-1; k++)
            {
                for (int l = 0; l < ArrayCount(wireJ->points); l++)
                {
                    if (&wireI->points[k] == &wireJ->points[l])
                        continue;
                    float dist = distance(wireI->points[k].position, wireJ->points[l].position);
                    float2 delta = normalize(wireI->points[k].position - wireJ->points[l].position);

                    float force = 5.0f / dist;
                    if(l == 0 || l == (ArrayCount(wireJ->points)-1))
                        force = 10.0f / dist;

                    if(force > 0.5f)
                        wireI->points[k].position += delta * force;
                }
            }
        }

    }
    }

    // add points to relieve tension
    //for (int i = 0; i < ArrayCount(scene->wires); i++)
    //{
    //    //float expectedLength = ArrayCount(scene->wires[i].points) * wireParticleDistance;
    //    //float length = 0;
    //    for (int j = 1; j < ArrayCount(scene->wires[i].points); j++)
    //    {
    //        float2 p1 = scene->wires[i].points[j - 1].position;
    //        float2 p2 = scene->wires[i].points[j].position;
    //        float dist = distance(p1, p2);
    //        if (dist > (wireParticleDistance * 2))
    //        {
    //            WirePoint newPoint = {};
    //            newPoint.velocity = float2(0, 0);
    //            newPoint.position = (p1 + p2) * 0.5f;
    //            ArrayInsert(scene->wires[i].points, newPoint, j);
    //        }
    //    }
    //    //Text(engineState, videoBuffer, scene->wires[i].points[0].position, "tension: ", length- expectedLength);
    //    //scene->wires[i].tension = length - expectedLength;
    //}

    // tension reduction
    //if (!(input->Shift && input->MouseLeft))
    //{
    //    for (int i = 0; i < ArrayCount(scene->wires); i++)
    //    {
    //        if (scene->wires[i].tension > 12)
    //        {
    //            WirePoint newPoint = scene->wires[i].points[ArrayCount(scene->wires[i].points)-1];
    //            ArrayAdd(scene->wires[i].points, newPoint);
    //            //int c = ArrayCount(scene->wires[i].points) - 1;
    //            //if (c < 2)
    //            //    continue;
    //            //ArrayRemove(scene->wires[i].points, c);
    //        }
    //    }
    //}


    ProfilerEndSample(&engineState->profilingData, "Simulate");


    ProfilerBeingSample(&engineState->profilingData);


    ProfilerEndSample(&engineState->profilingData, "Draw");
    ProfilerEndSample(&engineState->profilingData, "Frame");
    ProfilerDrawFlameChart(input, engineState, videoBuffer);

}
