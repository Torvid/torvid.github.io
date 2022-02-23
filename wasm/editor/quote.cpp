//#pragma optimize( "g", on )

#pragma once

#include "quote.h"
#include "string.cpp"
#include "drawing.cpp"
#include "StringSound.cpp"

internal void GameOutputSound(GameState* GameState, game_sound_output_buffer* SoundBuffer, float ToneHz, float ToneVolume)
{
    int16* SampleOut = SoundBuffer->Samples;
    float TonePeriod = (int16)((float)SoundBuffer->SamplesPerSecond / ToneHz);
    int16 SampleValue = 0;

    for (int i = 0; i < SoundBuffer->SampleCount; i++)
    {
        GameState->tSine += 2.0f * Pi * 1.0f / (TonePeriod * 4.0f);
        if (GameState->tSine > 3.14152128f * 2.0f)
        {
            GameState->tSine = 0;
        }
        //GameState->tSine = 0;

        float SineValue = sinf(GameState->tSine);

        SampleValue = (int16)(SineValue * ToneVolume);

        *SampleOut++ = SampleValue;
        *SampleOut++ = SampleValue;
    }
}

char getNthBit(char c, char n)
{
    unsigned char tmp = 1 << n;
    return (c & tmp) >> n;
}

void DebugDrawInput(Image* fontSpritesheet, game_offscreen_buffer* VideoBuffer, game_input* Input)
{
    int ui_height = 0;

    ui_height += 12;
    ui_height += 12;
    ui_height += 12;
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "MouseX: ", Input->MouseX);
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "MouseY: ", Input->MouseY);
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "MouseZ: ", Input->MouseZ);
    ui_height += 12;

    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "MouseLeft: ", Input->MouseLeft);
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "MouseRight: ", Input->MouseRight);
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "MouseMiddle: ", Input->MouseMiddle);
    ui_height += 12;

    char Letters[100] = {};
    Append(Letters, "Letters: ");
    for (int i = 0; i < 26; i++)
    {
        char name[2] = " ";
        if (Input->Letters[i].Held)
            name[0] = (char)(i + 65);
        else
            name[0] = '-';

        Append(Letters, name);
    }
    font_draw(fontSpritesheet, VideoBuffer, 0, ui_height, Letters);
    ui_height += 12;

    char Numbers[100] = {};
    Append(Numbers, "Numbers: ");
    for (int i = 0; i < 10; i++)
    {
        char name[2] = " ";
        if (Input->Numbers[i].Held)
            name[0] = (char)(i + 48);
        else
            name[0] = '-';

        Append(Numbers, name);
    }
    font_draw(fontSpritesheet, VideoBuffer, 0, ui_height, Numbers);
    ui_height += 12;

    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Space: ", Input->Space);
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Backspace: ", Input->Backspace);
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Enter: ", Input->Enter);
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Escape: ", Input->Escape);
    //
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Left: ", Input->Left);
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Right: ", Input->Right);
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Up: ", Input->Up);
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Down: ", Input->Down);
    //
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Shift: ", Input->Shift);
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Ctrl: ", Input->Ctrl);
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Alt: ", Input->Alt);

    ui_height += 12;
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "A: ", Input->Controllers[0].A);
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "B: ", Input->Controllers[0].B);
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "X: ", Input->Controllers[0].X);
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Y: ", Input->Controllers[0].Y);
    //
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Left: ", Input->Controllers[0].Left);
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Right: ", Input->Controllers[0].Right);
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Up: ", Input->Controllers[0].Up);
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Down: ", Input->Controllers[0].Down);
    //
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "LeftStickX: ", Input->Controllers[0].LeftStickX);
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "LeftStickY: ", Input->Controllers[0].LeftStickY);
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "RightStickX: ", Input->Controllers[0].RightStickX);
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "RightStickY: ", Input->Controllers[0].RightStickY);
    //
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "LeftBumper: ", Input->Controllers[0].LeftBumper);
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "RightBumper: ", Input->Controllers[0].RightBumper);
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "LeftTrigger: ", Input->Controllers[0].LeftTrigger);
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "RightTrigger: ", Input->Controllers[0].RightTrigger);
    //
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Start: ", Input->Controllers[0].Start);
    //DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Back: ", Input->Controllers[0].Back);

    font_draw(fontSpritesheet, VideoBuffer, 140, 18, "input struct raw data:");

    int x_offset = 100;
    int y = VideoBuffer->Pitch * 1;
    for (int i = 0; i < sizeof(game_input); i++) // for every byte of the data
    {
        int i_wrapped = i % 10;
        if (i % 10 == 0)
            y += VideoBuffer->Pitch / 32;

        char byte = ((char*)Input)[i];

        int PixelIndex = (i_wrapped - x_offset) + y;

        for (int j = 0; j < 8; j++)
        {
            char bit = getNthBit(byte, j) != 0 ? 255 : 0;

            int bit_index = (((PixelIndex) * 8) + j) * 4;
            ((char*)VideoBuffer->Memory)[bit_index + 0] = bit;
            ((char*)VideoBuffer->Memory)[bit_index + 1] = bit;
            ((char*)VideoBuffer->Memory)[bit_index + 2] = bit;
            ((char*)VideoBuffer->Memory)[bit_index + 3] = 255;
        }
    }
}


float GetSmoothedValue(float data[100], int* index, float NewTime, int count)
{
    (*index)++;
    (*index) %= count;
    data[*index] = NewTime;// (float)(time_end - time_start) / 10000000;
    float accumulatedDelta = 0;
    for (int i = 0; i < count; i++)
    {
        accumulatedDelta += data[i];
    }
    accumulatedDelta /= count;
    return accumulatedDelta;
}

bool SphereBoxIntersect(transform* Sphere, transform* Box)
{
    float3 LocalPos = Sphere->position - Box->position;
    LocalPos = float3(dot(LocalPos, Box->right * Box->scale.x), dot(LocalPos, Box->forward * Box->scale.y), dot(LocalPos, Box->up * Box->scale.z));

    if (LocalPos.x < 0.5 && LocalPos.x > -0.5 &&
        LocalPos.y < 0.5 && LocalPos.y > -0.5 &&
        LocalPos.z < 0.5 && LocalPos.z > -0.5)
    {
        return true;
    }
    return false;
}

float2 CircleBoxDeIntersection(float2 BoxPos, float2 BoxSize, float2 BoxUp, float2 BoxRight, float2 SpherePos, float SphereRadius)
{
    SpherePos = BoxPos + float2(dot(SpherePos - BoxPos, BoxRight), dot(SpherePos - BoxPos, BoxUp));

    float2 InverseBoxUp = float2(BoxRight.y, BoxUp.y);
    float2 InverseBoxRight = float2(BoxRight.x, BoxUp.x);

    float2 LargeBoxSize = BoxSize + SphereRadius * 2;
    float p1 = (BoxPos.x - LargeBoxSize.x * 0.5f);
    float p2 = (BoxPos.x + LargeBoxSize.x * 0.5f);
    float p3 = (BoxPos.y - LargeBoxSize.y * 0.5f);
    float p4 = (BoxPos.y + LargeBoxSize.y * 0.5f);

    bool InBox = SpherePos.x > p1 &&
        SpherePos.x < p2 &&
        SpherePos.y > p3 &&
        SpherePos.y < p4;

    float2 deIntersectPos1 = BoxPos + sign(SpherePos - BoxPos) * LargeBoxSize * 0.5;
    if (abs((SpherePos - BoxPos) / BoxSize).x < abs((SpherePos - BoxPos) / BoxSize).y)
        deIntersectPos1.x = SpherePos.x;
    else
        deIntersectPos1.y = SpherePos.y;

    float2 a = abs(BoxPos - SpherePos) - BoxSize * 0.5;
    float q = min(a.x, a.y);
    bool incross = q < 0;

    float2 corner1 = BoxPos + BoxSize * float2(0.5, 0.5);
    float2 corner2 = BoxPos + BoxSize * float2(-0.5, 0.5);
    float2 corner3 = BoxPos + BoxSize * float2(0.5, -0.5);
    float2 corner4 = BoxPos + BoxSize * float2(-0.5, -0.5);

    bool insphere = distance(SpherePos, corner1) < SphereRadius ||
        distance(SpherePos, corner2) < SphereRadius ||
        distance(SpherePos, corner3) < SphereRadius ||
        distance(SpherePos, corner4) < SphereRadius;

    float2 deIntersectPos2 = float2(0, 0);
    if (distance(SpherePos, corner1) < SphereRadius)
        deIntersectPos2 = corner1 + normalize(SpherePos - corner1) * SphereRadius;
    else if (distance(SpherePos, corner2) < SphereRadius)
        deIntersectPos2 = corner2 + normalize(SpherePos - corner2) * SphereRadius;
    else if (distance(SpherePos, corner3) < SphereRadius)
        deIntersectPos2 = corner3 + normalize(SpherePos - corner3) * SphereRadius;
    else if (distance(SpherePos, corner4) < SphereRadius)
        deIntersectPos2 = corner4 + normalize(SpherePos - corner4) * SphereRadius;

    float2 deIntersectPos = SpherePos;
    if (incross && InBox)
        deIntersectPos = deIntersectPos1;
    else if (insphere)
        deIntersectPos = deIntersectPos2;

    float2 r = deIntersectPos - SpherePos;
    r = float2(dot(r, InverseBoxRight), dot(r, InverseBoxUp));
    return r;
}


float2 CircleCircleDeIntersection(float2 Sphere1Pos, float Sphere1Radius, float2 Sphere2Pos, float Sphere2Radius)
{
    return normalize(Sphere1Pos - Sphere2Pos) * min(length(Sphere1Pos - Sphere2Pos) - Sphere1Radius - Sphere2Radius, 0);
}

float3 SphereSphereDeIntersection(float3 Sphere1Pos, float Sphere1Radius, float3 Sphere2Pos, float Sphere2Radius)
{
    return normalize(Sphere1Pos - Sphere2Pos) * min(length(Sphere1Pos - Sphere2Pos) - Sphere1Radius - Sphere2Radius, 0);
}
float3 SphereBoxDeIntersection(transform Box, transform Sphere)
{
    float SphereRadius = max(Sphere.scale);
    transform BigBox = Box;
    BigBox.scale = Box.scale + SphereRadius * 2;

    float3 SpherePos = WorldToLocalNoScale(Box, Sphere.position);// -Box.position; // local sphere position
    // TODO: rewrite this in terms of a distance function. It'd be way faster!
    bool InsideBigBox =
        SpherePos.x < BigBox.scale.x * 0.5 &&
        SpherePos.y < BigBox.scale.y * 0.5 &&
        SpherePos.z < BigBox.scale.z * 0.5 &&
        SpherePos.x > -BigBox.scale.x * 0.5 &&
        SpherePos.y > -BigBox.scale.y * 0.5 &&
        SpherePos.z > -BigBox.scale.z * 0.5;

    float3 boxScaledGradient = abs(SpherePos) - Box.scale * 0.5;
    float InsideCombinedShape = min(boxScaledGradient);

    float InsideCombinedShape2 = max(max(
        min(boxScaledGradient.x, boxScaledGradient.y),
        min(boxScaledGradient.x, boxScaledGradient.z)),
        min(boxScaledGradient.y, boxScaledGradient.z));

    float3 DeIntersectVectorLocal = float3(0, 0, 0);
    if (InsideCombinedShape > 0) // Inside a corner
    {
        //float3 SpherePosLocal = SpherePos / (Box.scale * 0.5);
        if (SpherePos.x < 0 && SpherePos.y < 0 && SpherePos.z < 0)
            DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(-1, -1, -1), 0, SpherePos, SphereRadius);
        if (SpherePos.x > 0 && SpherePos.y < 0 && SpherePos.z < 0)
            DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(1, -1, -1), 0, SpherePos, SphereRadius);
        if (SpherePos.x < 0 && SpherePos.y > 0 && SpherePos.z < 0)
            DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(-1, 1, -1), 0, SpherePos, SphereRadius);
        if (SpherePos.x > 0 && SpherePos.y > 0 && SpherePos.z < 0)
            DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(1, 1, -1), 0, SpherePos, SphereRadius);
        if (SpherePos.x < 0 && SpherePos.y < 0 && SpherePos.z > 0)
            DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(-1, -1, 1), 0, SpherePos, SphereRadius);
        if (SpherePos.x > 0 && SpherePos.y < 0 && SpherePos.z > 0)
            DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(1, -1, 1), 0, SpherePos, SphereRadius);
        if (SpherePos.x < 0 && SpherePos.y > 0 && SpherePos.z > 0)
            DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(-1, 1, 1), 0, SpherePos, SphereRadius);
        if (SpherePos.x > 0 && SpherePos.y > 0 && SpherePos.z > 0)
            DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(1, 1, 1), 0, SpherePos, SphereRadius);
    }
    else if (InsideCombinedShape2 > 0) // Inside an edge
    {
        float3 SpherePosLocal = SpherePos / (Box.scale * 0.5);
        if (SpherePosLocal.z < 1 && SpherePosLocal.z > -1)
        {
            if (SpherePosLocal.x > 1 && SpherePosLocal.y > 1)
                DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(1, 1, 0), 0, SpherePos * float3(1, 1, 0), SphereRadius);
            if (SpherePosLocal.x < -1 && SpherePosLocal.y > 1)
                DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(-1, 1, 0), 0, SpherePos * float3(1, 1, 0), SphereRadius);
            if (SpherePosLocal.x > 1 && SpherePosLocal.y < -1)
                DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(1, -1, 0), 0, SpherePos * float3(1, 1, 0), SphereRadius);
            if (SpherePosLocal.x < -1 && SpherePosLocal.y < -1)
                DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(-1, -1, 0), 0, SpherePos * float3(1, 1, 0), SphereRadius);
        }
        else if (SpherePosLocal.y < 1 && SpherePosLocal.y > -1)
        {
            if (SpherePosLocal.x > 1 && SpherePosLocal.z > 1)
                DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(1, 0, 1), 0, SpherePos * float3(1, 0, 1), SphereRadius);
            if (SpherePosLocal.x < -1 && SpherePosLocal.z > 1)
                DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(-1, 0, 1), 0, SpherePos * float3(1, 0, 1), SphereRadius);
            if (SpherePosLocal.x > 1 && SpherePosLocal.z < -1)
                DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(1, 0, -1), 0, SpherePos * float3(1, 0, 1), SphereRadius);
            if (SpherePosLocal.x < -1 && SpherePosLocal.z < -1)
                DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(-1, 0, -1), 0, SpherePos * float3(1, 0, 1), SphereRadius);
        }
        else if (SpherePosLocal.x < 1 && SpherePosLocal.x > -1)
        {
            if (SpherePosLocal.y > 1 && SpherePosLocal.z > 1)
                DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(0, 1, 1), 0, SpherePos * float3(0, 1, 1), SphereRadius);
            if (SpherePosLocal.y < -1 && SpherePosLocal.z > 1)
                DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(0, -1, 1), 0, SpherePos * float3(0, 1, 1), SphereRadius);
            if (SpherePosLocal.y > 1 && SpherePosLocal.z < -1)
                DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(0, 1, -1), 0, SpherePos * float3(0, 1, 1), SphereRadius);
            if (SpherePosLocal.y < -1 && SpherePosLocal.z < -1)
                DeIntersectVectorLocal = SphereSphereDeIntersection((Box.scale * 0.5) * float3(0, -1, -1), 0, SpherePos * float3(0, 1, 1), SphereRadius);
        }
    }
    else if (InsideBigBox) // Inside a face
    {
        float3 SpherePosLocal = SpherePos / (Box.scale * 0.5);
        float largest = max(SpherePosLocal);
        float smallest = min(SpherePosLocal);
        DeIntersectVectorLocal = SpherePos;
        if (abs(smallest) < largest)
        {
            if (SpherePosLocal.x == largest)
                DeIntersectVectorLocal.x = BigBox.scale.x * 0.5;
            else if (SpherePosLocal.y == largest)
                DeIntersectVectorLocal.y = BigBox.scale.y * 0.5;
            else if (SpherePosLocal.z == largest)
                DeIntersectVectorLocal.z = BigBox.scale.z * 0.5;
        }
        else
        {
            if (SpherePosLocal.x == smallest)
                DeIntersectVectorLocal.x = -BigBox.scale.x * 0.5;
            else if (SpherePosLocal.y == smallest)
                DeIntersectVectorLocal.y = -BigBox.scale.y * 0.5;
            else if (SpherePosLocal.z == smallest)
                DeIntersectVectorLocal.z = -BigBox.scale.z * 0.5;
        }
        DeIntersectVectorLocal -= SpherePos;
    }
    else // Outisde of the box.
    {
    }

    return LocalToWorldVectorNoScale(Box, DeIntersectVectorLocal);
}
//my preferred datastructures are C array and void pointer



int FindImageIndex(GameState* gameState, Image* image)
{
    for (int i = 0; i < ArrayCount(gameState->Images); i++)
    {
        if (&gameState->Images[i] == image)
        {
            return i;
        }
    }
    return -1; // not found
}
//int ShaderToIndex(SurfaceShader* shader)
//{
//    if (shader == SurfaceShaderExample)
//        return 0;
//    else if (shader == SurfaceShaderSimple)
//        return 1;
//    else if (shader == SurfaceShaderTexture)
//        return 2;
//}
//SurfaceShader* IndexToShader(int index)
//{
//    if (index == 0)
//        return SurfaceShaderExample;
//    else if (index == 1)
//        return SurfaceShaderSimple;
//    else if (index == 2)
//        return SurfaceShaderTexture;
//    return SurfaceShaderExample;
//}




// Functions called when deserializing
void MakeEntity(GameState* gameState, Entity* entity, transform t, bool mesh, int type, int texture1, int texture2, int shader, bool physics = 0, bool kinematic = 0, float mass = 1)
{
    *entity = {};
    entity->initialized = true;

    // Transform
    entity->t = t;

    // Mesh
    entity->mesh = mesh;
    entity->type = (bodytype)type;
    entity->texture1 = &gameState->Images[texture1];
    entity->texture2 = &gameState->Images[texture2];
    entity->ShaderIndex = shader;

    // Physics
    entity->physics = physics;
    entity->kinematic = kinematic;
    entity->mass = mass;
}

void WriteEntity(GameState* gameState, char* target, Entity* entity, int index)
{
    int max = ArrayCount(gameState->SaveBuffer);
    Append(target, "MakeEntity", max);
    //if (entity->mesh)
    //{
    //    if (entity->type == bodytype_box)
    //    else
    //        Append(target, "MakeEntity_Sphere", max);
    //}
    Append(target, "(gameState, &gameState->Entities[", max);
    Append(target, index, max);
    Append(target, "], ", max);

    // Transform
    Append(target, "transform(", max);
    CoerceAppend(target, entity->t.position, max);
    Append(target, ", ", max);
    CoerceAppend(target, entity->t.forward, max);
    Append(target, ", ", max);
    CoerceAppend(target, entity->t.up, max);
    Append(target, ", ", max);
    CoerceAppend(target, entity->t.scale, max);
    Append(target, ")", max);

    // Mesh
    Append(target, ", ", max);
    Append(target, entity->mesh, max);
    Append(target, ", ", max);
    Append(target, entity->type, max);
    Append(target, ", ", max);
    Append(target, FindImageIndex(gameState, entity->texture1), max);
    Append(target, ", ", max);
    Append(target, FindImageIndex(gameState, entity->texture2), max);
    Append(target, ", ", max);
    Append(target, entity->ShaderIndex, max);

    // Physics
    Append(target, ", ", max);
    Append(target, entity->physics, max);
    Append(target, ", ", max);
    Append(target, entity->kinematic, max);
    Append(target, ", ", max);
    Append(target, entity->mass, max);
    
    Append(target, ");", max);
    Append(target, "\n", max);
}



void LoadLevel(GameState* gameState, SaveState* saveState, int level)
{
    for (int j = 0; j < ArrayCount(gameState->Entities); j++)
    {
        gameState->Entities[j].initialized = false;
    }
    saveState->CurrentLevel = level;
    if (level  == 0)
    {
        #include "level0.cpp";
    }
    else if (level == 1)
    {
        #include "level1.cpp";
    }
    else if (level == 2)
    {
        #include "level2.cpp";
    }
}

void SaveLevel(game_memory* Memory, GameState* gameState, int level)
{
    for (int i = 0; i < ArrayCount(gameState->SaveBuffer); i++)
    {
        gameState->SaveBuffer[i] = 0;
    }
    for (int i = 0; i < ArrayCount(gameState->Entities); i++)
    {
        Entity* entity = &gameState->Entities[i];
        if (!entity->initialized)
            continue;
        WriteEntity(gameState, gameState->SaveBuffer, entity, i);
    }
    char LevelName[11] = {};
    Append(LevelName, "Level");
    Append(LevelName, level);
    Append(LevelName, ".cpp");
    read_file_result r;
    r.Contents = &gameState->SaveBuffer;
    r.ContentsSize = StringLength(gameState->SaveBuffer);
    Memory->PlatformWriteFile(0, LevelName, r);
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

//(thread_context* Thread, game_memory* Memory, game_input* Input, game_offscreen_buffer* VideoBuffer, game_sound_output_buffer* SoundBuffer)
#if WASM
#else
__declspec(dllexport)
#endif
extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    int64 Global_Start = Memory->PlatformTime();
    Assert(sizeof(GameState) <= Memory->TransientStorageSize);
    int64 time_start = Memory->PlatformTime();
    //Assert((&Input->Controllers[0].Terminator - &Input->Controllers[0].Buttons[0]) == (ArrayCount(Input->Controllers[0].Buttons) - 1));
    
    InitGlobalFunctions(Memory);

    GameState* gameState = (GameState*)Memory->TransientStorage;
    SaveState* saveState = (SaveState*)Memory->PermanentStorage;

    //GameOutputSound(gameState, SoundBuffer, 2610, 500.0f);
    if (Input->SpaceDown)
    {
        Memory->Initialized = false;
        for (int i = 0; i < Memory->TransientStorageSize; i++)
        {
            ((char*)Memory->TransientStorage)[i] = 0;
        }
    }

    if (Input->Ctrl && Input->SpaceDown)
    {
        saveState->Initialized = false;
        for (int i = 0; i < Memory->PermanentStorageSize; i++)
        {
            ((char*)Memory->PermanentStorage)[i] = 0;
        }
    }

    if (!saveState->Initialized)
    {
        saveState->Initialized = true;
        saveState->Camera = transform(float3(0, -5, 0), float3(0, 1, 0), float3(0, 0, 1), float3(1, 1, 1));
        saveState->ScreenThreads = 8;
        //saveState->Visualize_ThreadedRendering = false;
        //saveState->GameViewEnabled = false;
        saveState->ThreadedRendering = false;
        saveState->GameViewEnabled = true;
        //saveState->ThreadedRendering_Dynamic = false;
    }

    //Image* Apothem;
    //Image* MagSqueeze;
    Image* Wuff;
    float SphereRadius = 0.5;
    //bool init = false;
    if (!Memory->Initialized) // Happens on startup and when the user presses space
    {
        //init = true;
        Memory->Initialized = true;
        gameState->GameTime = 0;
        //InitializeArena(&gameState->worldArena,     Megabytes(1), (uint8*)Memory->TransientStorage + sizeof(GameState));
        InitializeArena(&gameState->textureArena,   Megabytes(12), (uint8*)Memory->TransientStorage + sizeof(GameState) + Megabytes(1));
    
        //gameState->fontSpritesheet = PushStruct(&gameState->textureArena, Spritesheet);
        //font_init(gameState->fontSpritesheet, Memory);
    
        gameState->fontSpritesheet = LoadImage(Memory, gameState, "font.tga", 192, 52);
        //Apothem      = LoadImage(Memory, gameState, "TestImage.tga", 300, 420);
        //MagSqueeze   = LoadImage(Memory, gameState, "TestImage2.tga", 512, 682);
        Wuff = LoadImage(Memory, gameState, "Wuff.tga", 256, 256);

        LoadImage(Memory, gameState, "textures/Floor_Top_Border.tga", 128, 128);
        LoadImage(Memory, gameState, "textures/Floor_Top_Flat.tga", 128, 128);
        gameState->SkyFaces[0] = LoadImage(Memory, gameState, "textures/Sky_C_Left.tga", 512, 512);
        gameState->SkyFaces[1] = LoadImage(Memory, gameState, "textures/Sky_C_Right.tga", 512, 512);
        gameState->SkyFaces[2] = LoadImage(Memory, gameState, "textures/Sky_C_Front.tga", 512, 512);
        gameState->SkyFaces[3] = LoadImage(Memory, gameState, "textures/Sky_C_Back.tga", 512, 512);
        gameState->SkyFaces[4] = LoadImage(Memory, gameState, "textures/Sky_C_Top.tga", 512, 512);
        gameState->SkyFaces[5] = LoadImage(Memory, gameState, "textures/Sky_C_Down.tga", 512, 512);


        //MakeEntity_Box(gameState, &gameState->Entities[0], transform(float3(0, 0, 0), float3(0, 1, 0), float3(0, 0, 1), float3(1, 1, 1)), 1, 1, 0);
        //MakeEntity_Box(gameState, &gameState->Entities[1], transform(float3(0, 1, 0), float3(0, 1, 0), float3(0, 0, 1), float3(1, 1, 1)), 1, 1, 0);
        //MakeEntity_Box(gameState, &gameState->Entities[2], transform(float3(0, 3, 0), float3(0, 1, 0), float3(0, 0, 1), float3(1, 1, 1)), 1, 1, 0);
        //MakeEntity_Box(gameState, &gameState->Entities[3], transform(float3(2, 0, 0), float3(0, 1, 0), float3(0, 0, 1), float3(1, 1, 1)), 1, 1, 0);

        //gameState->RayStartX = 100;
        //gameState->RayStartY = 100;
        //
        //gameState->RayDirX = 100;
        //gameState->RayDirY = 100;
        gameState->UseFiltering = true;

        //gameState->SpherePos = float2(450, 300);
        //gameState->SphereRight = float2(1, 0);
        //gameState->SphereUp = float2(0,1);
        gameState->TestSphere = transform(float3(0, 0, 1), float3(0,1,0), float3(0,0,1), float3(SphereRadius, SphereRadius, SphereRadius));

        gameState->MoveGizmoAxisID = -1;
        gameState->PickedSliderID = -1;

        LoadLevel(gameState, saveState, saveState->CurrentLevel);

        gameState->stringData.Pos = (float*)&(gameState->stringData.Pos1);
        gameState->stringData.Vel = (float*)&(gameState->stringData.Vel1);
        gameState->stringData.OldPos = (float*)&(gameState->stringData.Pos2);
        gameState->stringData.OldVel = (float*)&(gameState->stringData.Vel2);

        gameState->stringData.speed = 0.4f;
        gameState->stringData.damping = 0.001f;
        gameState->stringData.timescale = 1.0f;
        gameState->stringData.Segments = 60;
        gameState->stringData.FTScale = 0.5f;
        //gameState->stringData.Magnitudes = {};
    }
    gameState->SliderID = 0;
    gameState->GameTime += Input->DeltaTime;
    int uiOffset = 0;

    if (Input->GDown)
        saveState->GameViewEnabled = !saveState->GameViewEnabled;

    if (Input->MouseRight && !gameState->Grabbing)
    {
        saveState->Camera = rotate(saveState->Camera, float3(0, 0, 1), -Input->MouseXDelta * 0.005f);
        saveState->Camera = rotate(saveState->Camera, saveState->Camera.right, -Input->MouseYDelta * 0.005f);

        int X = Input->W ? 1 : 0 + Input->S ? -1 : 0;
        int Y = Input->D ? 1 : 0 + Input->A ? -1 : 0;
        int Z = Input->E ? 1 : 0 + Input->Q ? -1 : 0;

        float3 Movement = float3(0, 0, 0);
        Movement = Movement + saveState->Camera.forward * X;
        Movement = Movement + saveState->Camera.right * Y;
        Movement = Movement + saveState->Camera.up * Z;
        if(length(Movement) > 0)
            Movement = normalize(Movement);
        float speed = 4;
        if (Input->Shift)
            speed = 8;
        saveState->Camera.position += Movement * Input->DeltaTime * speed;
    }

    float3 BoxForward3D = float3(1, 0, 0); 
    float3 BoxUp3D = float3(0, 0, 1);
    float3 BoxScale = float3(1, 1, 1);
    float3 BoxScale2 = float3(1, 1, 2);
    
    float3 RotatingBoxForward = float3(cos(gameState->GameTime), sin(gameState->GameTime), 0);


    //transform Boxes[6] = {};
    //Boxes[0] = transform(float3(8, 0, 1.1),     BoxForward3D, BoxUp3D, BoxScale);
    //Boxes[1] = transform(float3(0, 0, 2.2),     BoxForward3D, BoxUp3D, BoxScale);
    //Boxes[2] = transform(float3(1.5, 0, 0),     BoxForward3D, BoxUp3D, BoxScale2);
    //Boxes[3] = transform(float3(1.1, 0, 1.1),   BoxForward3D, BoxUp3D, BoxScale);
    //Boxes[4] = transform(float3(0, 0, 0),       BoxForward3D, BoxUp3D, BoxScale);
    //Boxes[5] = transform(float3(0, 0, 0.4),     BoxForward3D, BoxUp3D, BoxScale);
    //
    //bool spheres[6] = { 0, 0, 0, 0, 0, 1 };

    //if(gameState->PrefViewEnabled)
    //    DrawSkybox(gameState, VideoBuffer, &saveState->Camera);
    //else
    //    DrawRectangle(VideoBuffer, 0, 0, 1920 / 2, 1080 / 2, 0.5, 0, 0);
    //for (int i = 0; i < 6; i++)
    //{
    //    if (gameState->PrefViewEnabled)
    //        DrawObject(gameState, VideoBuffer, &saveState->Camera, &Boxes[i], spheres[i]);
    //    else
    //        DrawObjectCheap(gameState, VideoBuffer, &saveState->Camera, &Boxes[i], spheres[i]);
    //}

    
    //if (Input->MouseLeftUp)
    //{
    //    gameState->Grabbing = false;
    //}
    

    //if (Input->MouseLeft && gameState->Grabbing)
    //{
    //    float MoveSpeed = 0.01;
    //    gameState->TestSphere.position += Input->MouseXDelta * saveState->Camera.right * MoveSpeed;
    //    gameState->TestSphere.position += -Input->MouseYDelta * saveState->Camera.up * MoveSpeed;
    //}

    //transform Box = transform(float3(0, 0, 0), RotatingBoxForward, BoxUp3D, float3(1, 2, 3));
    //transform Box = transform(float3(0, 0, 0), BoxForward3D, BoxUp3D, float3(2, 3, 1));
    //Box = rotate(Box, Input->DeltaTime, 0, 0),
    //gameState->TestSphere.position += SphereBoxDeIntersection(Box, gameState->TestSphere);
    //
    //float hit = RaySphereIntersect(saveState->Camera.position - gameState->TestSphere.position, MouseRay, SphereRadius);
    //if (hit > 0)
    //{
    //    transform SphereSelect = gameState->TestSphere;
    //    SphereSelect.scale *= 1.03;
    //    DrawObjectCheap(gameState, VideoBuffer, saveState->Camera, SphereSelect, true);
    //    if (Input->MouseLeftDown)
    //    {
    //        gameState->Grabbing = true;
    //    }
    //}

    //DrawObject(gameState, VideoBuffer, saveState->Camera, gameState->TestSphere, true);
    //DrawObject(gameState, VideoBuffer, saveState->Camera, Box, false);


    //gameState->Bodies3D[0] = RigidBody3DBox(transform(float3(1.2, 4, 0), BoxForward3D, BoxUp3D, float3(3, 1, 1)), 1, true);
    //gameState->Bodies3D[1] = RigidBody3DBox(transform(float3(0, 2, 0), BoxForward3D, BoxUp3D, float3(1, 1, 1)), 1, true);
    //gameState->Bodies3D[2] = RigidBody3DBox(transform(float3(0, 4, 0), BoxForward3D, BoxUp3D, float3(1, 1, 1)), 1, true);
    //gameState->Bodies3D[2].t = rotate(gameState->Bodies3D[2].t, float3(1, 0, 0), 1);
    //gameState->Bodies3D[4] = RigidBody3DBox(transform(float3(0, 6, -3), BoxForward3D, BoxUp3D, float3(10, 10, 0.5)), 1, true);
    //gameState->Bodies3D[4].t = rotate(gameState->Bodies3D[4].t, float3(1, 0, 0), 0.25);
    
    gameState->NextRenderCommand = 0;
    QueueDrawSkyBox(gameState, gameState->SkyFaces);

    // Ready memory for drawing
    //int i = 0;
    //for (int x = 0; x < 3; x++)
    //{
    //    for (int y = 0; y < 3; y++)
    //    {
    //        //gameState->FastDrawCubes[i] = ;
    //        QueueDrawBox(gameState, transform(float3(x * 4 - 8, y * 4 - 8, -6), BoxForward3D, BoxUp3D, float3(4, 4, 1)), &gameState->Images[3]);
    //        //DrawObject(gameState, VideoBuffer, saveState->Camera, gameState->FastDrawCubes[i], false, -1);
    //
    //        i++;
    //    }
    //}
    for (int i = 0; i < ArrayCount(gameState->Entities); i++)
    {
        Entity* e = &gameState->Entities[i];
        if (!Valid(e))
            continue;
        if (e->t.position.z < -5)
            e->t.position.z = -5;
    }

    for (int i = 0; i < ArrayCount(gameState->Entities); i++)
    {
        Entity* e = &gameState->Entities[i];
        if (!Valid(e))
            continue;

        if (!e->mesh)
            continue;

        bool selected = false;
        for (int i = 0; i < ArrayCount(gameState->SelectedEntities); i++)
        {
            Entity* selectedEntity = gameState->SelectedEntities[i];
            if (e == selectedEntity)
            {
                selected = true;
                break;
            }
        }

        bool Hovered = e == gameState->HoveredEntity;
        if(e->type == bodytype_sphere)
            QueueDrawSphere(gameState, e->t, e->texture1, e->texture2, e->ShaderIndex, selected, Hovered);
        if (e->type == bodytype_box)
            QueueDrawBox(gameState, e->t, e->texture1, e->texture2, e->ShaderIndex, selected, Hovered);
    }
    // Draws the queued up commands to the screen.
    DrawScene(Memory, gameState, saveState, VideoBuffer);

    
    //DrawRectangle(VideoBuffer, float2(0, 0), float2(VideoBuffer->Width, VideoBuffer->Height), float3(0.5, 0, 0));
    //
    //int DesiredLenth = 700;
    //gameState->stringData.PixelsPerSample = DesiredLenth / gameState->stringData.Segments;
    //RenderStringSoundLine(VideoBuffer, SoundBuffer, Input, &gameState->stringData);
    //StringSound(VideoBuffer, SoundBuffer, Input, &gameState->stringData);


    if (Input->FDown)
    {
        //int i = 0;
        //for (int x = 0; x < 2; x++)
        //{
        //    for (int y = 0; y < 2; y++)
        //    {
        //        for (int z = 0; z < 3; z++)
        //        {
        //            gameState->Bodies3D[i + 40] = RigidBody3DSphere(transform(float3(
        //                x * 4.1 + 1.5 + z * 0.344,
        //                y * 4.1 + 4 - z * 0.344,
        //                z * 2.5 + 5), BoxForward3D, BoxUp3D, float3(1, 1, 1)), 1, true);
        //            i++;
        //        }
        //    }
        //}
        
        //gameState->Bodies3D[15] = RigidBody3DSphere(transform(float3(0, 4, 3), BoxForward3D, BoxUp3D, float3(1, 1, 1)), 1, true);
        //gameState->Bodies3D[16] = RigidBody3DSphere(transform(float3(0, 7, 3), BoxForward3D, BoxUp3D, float3(1, 1, 1)), 1, true);
        //gameState->Bodies3D[17] = RigidBody3DSphere(transform(float3(0, 9, 3), BoxForward3D, BoxUp3D, float3(1, 1, 1)), 1, true);
        //gameState->Bodies3D[15].t = rotate(gameState->Bodies3D[15].t, 10, 45, 75);
    }
    float2 Wat = float2(400, 200);

    for (int i = 0; i < ArrayCount(gameState->Entities); i++)
    {
        Entity* Body = &gameState->Entities[i];
        if (!ValidPhysics(Body))
            continue;

        // De-intersect
        // Currently only spheres can be dynamic
        if (Body->type != bodytype_sphere)
            continue;
        if(Body->kinematic)
            continue;
        Body->velocity += float3(0, 0, -0.005);
        Body->t.position += Body->velocity;
        if (!isfinite(Body->t.position))
            Body->t.position = float3(0, 0, 0);

        Entity* HitDynamicBody = 0;

        float3 DeintersectVector = float3(0, 0, 0);
        for (int j = 0; j < 10; j++)
        {
            for (int k = 0; k < ArrayCount(gameState->Entities); k++)
            {
                Entity* OtherBody = &gameState->Entities[k];
                if (!ValidPhysics(OtherBody))
                    continue;
                if (OtherBody == Body)
                    continue;

                float3 DeIntersect = float3(0, 0, 0);
                if (OtherBody->type == bodytype_box)
                    DeIntersect = SphereBoxDeIntersection(OtherBody->t, Body->t);
                else if (OtherBody->type == bodytype_sphere)
                    DeIntersect = SphereSphereDeIntersection(OtherBody->t.position, OtherBody->t.scale.x, Body->t.position, Body->t.scale.y);

                DeintersectVector += DeIntersect;
                //DeIntersectPosition += DeIntersect;
                //DrawLine(VideoBuffer, LastDeIntersectPosition, DeIntersectPosition);
                //LastDeIntersectPosition = DeIntersectPosition;
                if (length(DeIntersect) > 0)
                {
                    //float MoveDistance = distance(gameState->LastSpherePos, gameState->SpherePos);
                    // TODO: If the other object is also dynamic, do something else here.
                    Body->t.position += DeIntersect;
                    if (!OtherBody->kinematic)
                        HitDynamicBody = OtherBody;
                }
            }
        }

        // Add force
        float bouncyness = 0.25;// gameState->Bouncy ? 1 : 0;
        if (length(DeintersectVector) > 0)
        {
            // When something hits something, angular velocity and velocity try to balance out. 
            // If it's spinning a lot, it starts moves
            // If it's moving a lot, it starts spinning
            float3 normalizedVelocity = normalize(Body->velocity);
            float3 normal = normalize(DeintersectVector);
            if (normalizedVelocity == normal)
            {
                continue;
            }
            bool safe = true;
            float3 tangentN = cross(normalizedVelocity, normal);
            if (dot(tangentN, tangentN) == 0)
                safe = false;

            float3 StartVel = Body->velocity;
            float StartAngularVel = Body->angularVelocity;
            float3 NewVel = Body->velocity;
            float NewAngularVel = Body->angularVelocity;

            if (safe)
            {
                float3 tangent = normalize(tangentN);
                float3 binormal = normalize(cross(normal, tangent));
                DrawLine3D(VideoBuffer, saveState->Camera, Body->t.position, Body->t.position + normal);
                DrawLine3D(VideoBuffer, saveState->Camera, Body->t.position, Body->t.position + tangent);
                DrawLine3D(VideoBuffer, saveState->Camera, Body->t.position, Body->t.position + binormal);

                float3 BounceVel = reflect(NewVel, normal);
                float3 StopVel = binormal * dot(binormal, NewVel);// normal* clamp01(-dot(normal, NewVel));
                NewVel = lerp(StopVel, BounceVel, bouncyness);

                // Tangent is up, velocity is down, they cancen out to 0.
                float3 thing = lerp(Body->angularVelocityAxis, tangent, 0.5f);

                if (dot(thing, thing) == 0)
                    safe = false;

                if (safe)
                {
                    Body->angularVelocityAxis = normalize(thing); // move angular velocity a little bit to the hit angle
                    NewAngularVel += abs(dot(binormal, NewVel) / 100);
                    NewVel += binormal * NewAngularVel * 50;
                }
            }

            NewVel *= 0.5f;
            NewAngularVel *= 0.5f;

            float3 DeltaVel = (NewVel - StartVel);
            float DeltaAngularVel = (NewAngularVel - StartAngularVel);

            if (HitDynamicBody)
            {
                float MassRatio1 = Body->mass / HitDynamicBody->mass;
                float MassRatio2 = HitDynamicBody->mass / Body->mass;
            
                Body->velocity += DeltaVel * MassRatio2;
                Body->angularVelocity += DeltaAngularVel * MassRatio2;
            
                //HitDynamicBody->velocity -= DeltaVel * 0.75 * MassRatio1;
                //HitDynamicBody->angularVelocity -= DeltaAngularVel * 0.75 * MassRatio1;
                HitDynamicBody->velocity -= DeltaVel * 1 * MassRatio1;
                HitDynamicBody->angularVelocity -= DeltaAngularVel * 1 * MassRatio1;
            }
            else
            {
                Body->velocity += DeltaVel;
                Body->angularVelocity += DeltaAngularVel;
            }
        }

        Body->t = rotate(Body->t, Body->angularVelocityAxis, -Body->angularVelocity * 100);

        DrawLine3D(VideoBuffer, saveState->Camera, Body->t.position, Body->t.position + Body->velocity);
    }

    
    int64 time_end = Memory->PlatformTime();

#if 0

    if (Input->FDown)
        gameState->Bouncy = !gameState->Bouncy;

    float2 MousePos = float2(Input->MouseX, Input->MouseY);
    
    gameState->Bodies2D[0] = RigidBody2DBox(float2(500, 500), float2(750, 50), normalize(float2(0.2, 2.5)), 1, true);
    gameState->Bodies2D[1] = RigidBody2DBox(float2(650, 100), float2(400, 5), normalize(float2(0, 0.5)), 1, true);
    gameState->Bodies2D[2] = RigidBody2DBox(float2(150, 400), float2(550, 60), normalize(float2(-0.6, 0.5)), 1, true);
    gameState->Bodies2D[3] = RigidBody2DBox(float2(1000, 400), float2(50, 200), normalize(float2(-0.0, 0.5)), 1, true);

    gameState->Bodies2D[4] = RigidBody2DBox(float2(500, 490), float2(900, 100), normalize(float2(-0.0, 0.5)), 1, true);

    if (Input->MouseLeft)
    {
        //for (int i = 0; i < 50-4; i++)
        //{
        //    gameState->Bodies2D[4+i] = RigidBody2DSphere(float2(Input->MouseX+i*10, Input->MouseY), 20, false);
        //}
        gameState->Bodies2D[8] = RigidBody2DSphere(float2(Input->MouseX, Input->MouseY), 50, 2, false);
        gameState->Bodies2D[9] = RigidBody2DSphere(float2(Input->MouseX + 300, Input->MouseY), 50, 1, false);
        gameState->Bodies2D[10] = RigidBody2DSphere(float2(Input->MouseX + 400, Input->MouseY), 50, 1, false);
        gameState->Bodies2D[11] = RigidBody2DSphere(float2(Input->MouseX + 500, Input->MouseY), 50, 1, false);
        gameState->Bodies2D[12] = RigidBody2DSphere(float2(Input->MouseX + 600, Input->MouseY), 50, 1, false);
        gameState->Bodies2D[13] = RigidBody2DSphere(float2(Input->MouseX + 700, Input->MouseY), 50, 1, false);
    }
    gameState->Bodies2D[8].angularVelocity += Y * 0.01;

    for (int i = 0; i < ArrayCount(gameState->Bodies2D); i++)
    {
        RigidBody2D* Body = &gameState->Bodies2D[i];
        if (!Body->initialized)
            continue;

        // Draw
        if (Body->type == bodytype_sphere)
            DrawCircle(VideoBuffer, Body->position, Body->radius);
        else if(Body->type == bodytype_box)
            DrawBox(VideoBuffer, Body->position, Body->size, Body->up, Body->right);

        DrawLine(VideoBuffer, Body->position, Body->position + Body->up * 50);
        DrawLine(VideoBuffer, Body->position, Body->position + Body->right * 50);

        // De-intersect
        // Currently only spheres can be dynamic
        if (Body->type != bodytype_sphere)
            continue;

        Body->velocity += float2(0, 1);
        Body->position += Body->velocity;

        float2 DeIntersectPosition = Body->position;
        float2 LastDeIntersectPosition = Body->position;

        RigidBody2D* HitDynamicBody = 0;

        float2 DeintersectVector = float2(0, 0);
        for (int j = 0; j < 10; j++)
        {
            for (int k = 0; k < ArrayCount(gameState->Bodies2D); k++)
            {
                RigidBody2D* OtherBody = &gameState->Bodies2D[k];
                if (!OtherBody->initialized)
                    continue;
                if (OtherBody == Body)
                    continue;

                float2 DeIntersect = float2(0, 0);
                if(OtherBody->type == bodytype_box)
                    DeIntersect = CircleBoxDeIntersection(OtherBody->position, OtherBody->size, OtherBody->up, OtherBody->right, Body->position, Body->radius);
                else if(OtherBody->type == bodytype_sphere)
                    DeIntersect = CircleCircleDeIntersection(OtherBody->position, OtherBody->radius, Body->position, Body->radius);
        
                DeintersectVector += DeIntersect;
                DeIntersectPosition += DeIntersect;
                DrawLine(VideoBuffer, LastDeIntersectPosition, DeIntersectPosition);
                LastDeIntersectPosition = DeIntersectPosition;
                if (length(DeIntersect) > 0)
                {
                    //float MoveDistance = distance(gameState->LastSpherePos, gameState->SpherePos);
                    // TODO: If the other object is also dynamic, do something else here.
                    Body->position += DeIntersect;
                    if (!OtherBody->kinematic)
                        HitDynamicBody = OtherBody;
                }
            }
        }
        
        // Add force
        float bouncyness = gameState->Bouncy ? 1 : 0;
        if (length(DeintersectVector) > 0)
        {
            // When something hits something, angular velocity and velocity try to balance out. 
            // If it's spinning a lot, it starts moves
            // If it's moving a lot, it starts spinning
        
            float2 normal = normalize(DeintersectVector);
            float2 tangent = float2(-normal.y, normal.x);
        
            float2 StartVel = Body->velocity;
            float StartAngularVel = Body->angularVelocity;
        
            float2 NewVel = Body->velocity;
            float NewAngularVel = Body->angularVelocity;
        
            float2 BounceVel = reflect(NewVel, normal);
            float2 StopVel = tangent * dot(tangent, NewVel);
            NewVel = lerp(StopVel, BounceVel, bouncyness);

            NewVel *= 0.5f;
            NewAngularVel *= 0.5f;
            NewAngularVel += dot(tangent, NewVel) / 100;
            NewVel += tangent * NewAngularVel * 50;
        
            float2 DeltaVel = (NewVel - StartVel);
            float DeltaAngularVel = (NewAngularVel - StartAngularVel);

            if (HitDynamicBody)
            {
                float MassRatio1 = Body->mass / HitDynamicBody->mass;
                float MassRatio2 = HitDynamicBody->mass / Body->mass;

                Body->velocity += DeltaVel * MassRatio2;
                Body->angularVelocity += DeltaAngularVel * MassRatio2;

                HitDynamicBody->velocity -= DeltaVel * 0.75 * MassRatio1;
                HitDynamicBody->angularVelocity -= DeltaAngularVel * 0.75 * MassRatio1;
            }
            else
            {
                Body->velocity += DeltaVel;
                Body->angularVelocity += DeltaAngularVel;
            }
        }
        Body->up = rotate(Body->up, Body->angularVelocity);
        Body->right = rotate(Body->right, Body->angularVelocity);
    }
#endif
    float2 UIPos = float2(4, 0);

    float FPS = (float)(1.0f / Input->DeltaTime);
    float Delta = (float)(time_end - time_start) / 10000000;
    float accumulatedDelta = GetSmoothedValue(gameState->deltas, &gameState->deltasIndex, Delta, 30);
    float SmoothFPS = GetSmoothedValue(gameState->FPS, &gameState->FPSIndex, FPS, 30);

    //Text(gameState, VideoBuffer, &UIPos, "Controls");
    //Text(gameState, VideoBuffer, &UIPos, "  Left click to control string synth.");
    Text(gameState, VideoBuffer, &UIPos, "Controls");
    Text(gameState, VideoBuffer, &UIPos, "  Space to reset program.");
    Text(gameState, VideoBuffer, &UIPos, "  Shift + Space to reset program and wipe save state.");
    Text(gameState, VideoBuffer, &UIPos, "  G to toggle game view.");
    Text(gameState, VideoBuffer, &UIPos, "  Right mouse + WASD to move.");
    Text(gameState, VideoBuffer, &UIPos, "  Right mouse to look.");

    //Text(gameState, VideoBuffer, &UIPos, "    F to reset spheres.");
    if (!saveState->GameViewEnabled )
    {
        Text(gameState, VideoBuffer, &UIPos, " ");
        Text(gameState, VideoBuffer, &UIPos, "Timers");
        Text(gameState, VideoBuffer, &UIPos, "    Update Time MS: ", Delta * 1000);
        //Text(gameState, VideoBuffer, &UIPos, "    Draw Boxes Smooth MS: ", accumulatedDelta * 1000);
        Text(gameState, VideoBuffer, &UIPos, "    FPS: ", FPS);
        //Text(gameState, VideoBuffer, &UIPos, "    Smooth FPS: ", SmoothFPS);
        Text(gameState, VideoBuffer, &UIPos, " ");

        // EDITOR STUFF

        Text(gameState, VideoBuffer, &UIPos, "Editor Controls");
        Text(gameState, VideoBuffer, &UIPos, "  W Translate.");
        Text(gameState, VideoBuffer, &UIPos, "  E Rotate.");
        Text(gameState, VideoBuffer, &UIPos, "  R Scale.");
        Text(gameState, VideoBuffer, &UIPos, "  Delete to delete selection.");
        Text(gameState, VideoBuffer, &UIPos, "  Ctrl + D to duplicate selection.");
        Text(gameState, VideoBuffer, &UIPos, "  Left click to select.");
        Text(gameState, VideoBuffer, &UIPos, "  Shift + Left click add to selection.");
        Text(gameState, VideoBuffer, &UIPos, "  Ctrl + Left click remove from selection.");

        Entity* GizmoEntity = 0;
        for (int i = 0; i < ArrayCount(gameState->SelectedEntities); i++)
        {
            Entity* e = gameState->SelectedEntities[i];
            if (Valid(e))
            {
                GizmoEntity = e;
            }
        }

        if (!Input->MouseRight)
        {
            if (Input->W)
                gameState->GizmoState = 0;
            if (Input->E)
                gameState->GizmoState = 1;
            if (Input->R)
                gameState->GizmoState = 2;
        }


        float3 MoveOffset = float3(0, 0, 0);
        float3 ScaleOffset = float3(0, 0, 0);
        Rotation RotationOffset;
        RotationOffset.Axis = float3(1, 0, 0);
        RotationOffset.Angle = 0;
        if (Valid(GizmoEntity))
        {
            if (gameState->GizmoState == 0)
            {
                MoveOffset = MoveGizmo(gameState, Input, VideoBuffer, saveState->Camera, GizmoEntity->t, saveState->GizmoWorldSpace);
            }
            else if (gameState->GizmoState == 1)
            {
                RotationOffset = RotateGizmo(gameState, Input, VideoBuffer, saveState->Camera, GizmoEntity->t, saveState->GizmoWorldSpace);
            }
            else if (gameState->GizmoState == 2)
            {
                ScaleOffset = ScaleGizmo(gameState, Input, VideoBuffer, saveState->Camera, GizmoEntity->t);
            }
        }

        for (int i = 0; i < ArrayCount(gameState->SelectedEntities); i++)
        {
            Entity* e = gameState->SelectedEntities[i];
            if (Valid(e))
            {
                e->t.position += MoveOffset;
                e->t.scale += ScaleOffset;
                e->t = rotate(e->t, RotationOffset.Axis, RotationOffset.Angle);
            }
        }

        //MoveGizmo(gameState, Input, VideoBuffer, saveState->Camera,   transform(float3(1, 0, 0), float3(0, 1, 0), float3(0, 0, 1), float3(1, 1, 1)));
        //if (Valid(GizmoEntity))
        //    GizmoEntity->t = RotateGizmo(gameState, Input, VideoBuffer, saveState->Camera, GizmoEntity->t);
        //ScaleGizmo(gameState, Input, VideoBuffer, saveState->Camera,  transform(float3(3, 0, 0), float3(0, 1, 0), float3(0, 0, 1), float3(1, 1, 1)));

        if (Input->DeleteDown)
        {
            for (int i = 0; i < ArrayCount(gameState->SelectedEntities); i++)
            {
                Entity* e = gameState->SelectedEntities[i];
                if (Valid(e))
                {
                    *e = {};
                }
            }
        }
        if (Input->DDown && Input->Ctrl)
        {
            for (int i = 0; i < ArrayCount(gameState->SelectedEntities); i++)
            {
                Entity* selectedEntity = gameState->SelectedEntities[i];
                if (Valid(selectedEntity))
                {
                    for (int j = 0; j < ArrayCount(gameState->Entities); j++)
                    {
                        Entity* entity = &gameState->Entities[j];
                        if (!Valid(entity))
                        {
                            *entity = *selectedEntity; // copy?
                            break;
                        }
                    }
                }
            }
        }

        gameState->Grabbing = gameState->PickedSliderID != -1;
        if (!gameState->Grabbing)
        {
            // Find closest entity under cursor.
            float3 MouseRay = ScreenPointToRay(VideoBuffer, saveState->Camera, float2(Input->MouseX, Input->MouseY));
            float ClosestDistance = 99999;
            gameState->HoveredEntity = 0;
            for (int i = 0; i < ArrayCount(gameState->Entities); i++)
            {
                Entity* e = &gameState->Entities[i];
                if (!Valid(e))
                    continue;

                float3 Normal;
                float3 LocalPos = WorldToLocal(e->t, saveState->Camera.position);
                float3 LocalDir = WorldToLocalVector(e->t, MouseRay);

                float HitDepth = RayBoxIntersect(LocalPos, LocalDir, &Normal);
                if (HitDepth > 0)
                {
                    if (HitDepth < ClosestDistance)
                    {
                        ClosestDistance = HitDepth;
                        gameState->HoveredEntity = e;
                    }
                }
            }

            if (Input->MouseLeftDown && !gameState->MouseIsOverUI)
            {
                if (Input->Shift)
                {
                    for (int i = 0; i < ArrayCount(gameState->SelectedEntities); i++)
                    {
                        if (gameState->SelectedEntities[i] == gameState->HoveredEntity)
                            break;
                        if (!Valid(gameState->SelectedEntities[i]))
                        {
                            gameState->SelectedEntities[i] = gameState->HoveredEntity;
                            break;
                        }
                    }
                }
                else if (Input->Ctrl)
                {
                    for (int i = 0; i < ArrayCount(gameState->SelectedEntities); i++)
                    {
                        if (gameState->SelectedEntities[i] == gameState->HoveredEntity)
                        {
                            gameState->SelectedEntities[i] = 0;
                            break;
                        }
                    }
                }
                else
                {
                    for (int i = 0; i < ArrayCount(gameState->SelectedEntities); i++)
                    {
                        gameState->SelectedEntities[i] = 0;
                    }
                    gameState->SelectedEntities[0] = gameState->HoveredEntity;
                }
            }
            if (Input->EscapeDown)
            {
                for (int i = 0; i < ArrayCount(gameState->SelectedEntities); i++)
                {
                    gameState->SelectedEntities[i] = 0;
                }
            }
        }

        gameState->MouseIsOverUI = false;

        //float2 pos = float2(200, 100);
        //for (int i = 0; i < ArrayCount(gameState->Entities); i++)
        //{
        //    Entity* e = &gameState->Entities[i];
        //
        //    if (!Valid(e))
        //        continue;
        //
        //    if (Button(gameState, Input, VideoBuffer, &pos, "Entity"))
        //    {
        //        for (int i = 0; i < ArrayCount(gameState->SelectedEntities); i++)
        //        {
        //            gameState->SelectedEntities[i] = 0;
        //        }
        //        gameState->SelectedEntities[0] = e;
        //    }
        //}

        float2 MenuPlacement = float2(VideoBuffer->Width - 200, 5);

        char MenuText[10] = {};
        gameState->playing ? Append(MenuText, "Playing") : Append(MenuText, "Editing");
        if (Button(gameState, Input, VideoBuffer, &MenuPlacement, MenuText))
        {
            gameState->playing = !gameState->playing;
        }

        for (int i = 0; i < 3; i++)
        {
            char LoadText[100] = "Load Level ";
            Append(LoadText, i);
            if (Button(gameState, Input, VideoBuffer, &MenuPlacement, LoadText))
            {
                LoadLevel(gameState, saveState, i);
            }
        }

        if (Button(gameState, Input, VideoBuffer, &MenuPlacement, "Save"))
        {
            SaveLevel(Memory, gameState, saveState->CurrentLevel);
        }
        if (Input->Ctrl && Input->SDown)
        {
            SaveLevel(Memory, gameState, saveState->CurrentLevel);
        }
        
        Text(gameState, VideoBuffer, &MenuPlacement, "Editor.");
        char SpaceText[10] = {};
        saveState->GizmoWorldSpace ? Append(SpaceText, "Local Space") : Append(SpaceText, "World Space");
        if (Button(gameState, Input, VideoBuffer, &MenuPlacement, SpaceText))
        {
            saveState->GizmoWorldSpace = !saveState->GizmoWorldSpace;
        }

        if (Button(gameState, Input, VideoBuffer, &MenuPlacement, "Snap"))
        {
            for (int i = 0; i < ArrayCount(gameState->SelectedEntities); i++)
            {
                Entity* entity = gameState->SelectedEntities[i];
                if (!Valid(entity))
                    continue;

                entity->t.scale = round(entity->t.scale);
                if (entity->t.scale.x <= 1) entity->t.scale.x = 1;
                if (entity->t.scale.y <= 1) entity->t.scale.y = 1;
                if (entity->t.scale.z <= 1) entity->t.scale.z = 1;

                float3 HalfScale = float3(0, 0, 0);
                HalfScale += entity->t.right * (entity->t.scale.x * 0.5);
                HalfScale += entity->t.forward * (entity->t.scale.y * 0.5);
                HalfScale += entity->t.up * (entity->t.scale.z * 0.5);

                entity->t.position = round(entity->t.position - HalfScale) + HalfScale;
                //entity->t.position = entity->t.position - (entity->t.scale * 0.5);

                for (int i = 0; i < 5; i++)
                {
                    entity->t.right.x = MoveTowards(entity->t.right.x, 0, 0.5f);
                    entity->t.right.y = MoveTowards(entity->t.right.y, 0, 0.5f);
                    entity->t.right.z = MoveTowards(entity->t.right.z, 0, 0.5f);
                    entity->t.right = normalize(entity->t.right);

                    entity->t.forward.x = MoveTowards(entity->t.forward.x, 0, 0.5f);
                    entity->t.forward.y = MoveTowards(entity->t.forward.y, 0, 0.5f);
                    entity->t.forward.z = MoveTowards(entity->t.forward.z, 0, 0.5f);
                    entity->t.forward = normalize(entity->t.forward);

                    entity->t.up.x = MoveTowards(entity->t.up.x, 0, 0.5f);
                    entity->t.up.y = MoveTowards(entity->t.up.y, 0, 0.5f);
                    entity->t.up.z = MoveTowards(entity->t.up.z, 0, 0.5f);
                    entity->t.up = normalize(entity->t.up);
                }
            }
        }

        if (Button(gameState, Input, VideoBuffer, &MenuPlacement, "Reset Selected"))
        {
            for (int i = 0; i < ArrayCount(gameState->SelectedEntities); i++)
            {
                Entity* entity = gameState->SelectedEntities[i];
                if (!Valid(entity))
                    continue;

                entity->t.scale = float3(1, 1, 1);

                entity->t.right = float3(1, 0, 0);
                entity->t.forward = float3(0, 1, 0);
                entity->t.up = float3(0, 0, 1);
            }
        }
        Entity* entity = gameState->SelectedEntities[0];
        if (Valid(entity))
        {
            if (Button(gameState, Input, VideoBuffer, &MenuPlacement, entity->type == bodytype_sphere ? "Toggle Sphere" : "Toggle Box"))
            {
                if (entity->type == bodytype_sphere) entity->type = bodytype_box;
                else if (entity->type == bodytype_box) entity->type = bodytype_sphere;
            }

            entity->physics = Toggle(gameState, Input, VideoBuffer, &MenuPlacement, entity->physics, "Physics");
            entity->kinematic = Toggle(gameState, Input, VideoBuffer, &MenuPlacement, entity->kinematic, "Kinematic");
            if (entity->kinematic || !entity->physics)
            {
                entity->angularVelocity = 0;
                entity->velocity = float3(0, 0, 0);
            }

            Text(gameState, VideoBuffer, &MenuPlacement, "Shader: ", entity->ShaderIndex);
            entity->ShaderIndex = Slider(gameState, Input, VideoBuffer, &MenuPlacement, entity->ShaderIndex, 0, 4);

            Text(gameState, VideoBuffer, &MenuPlacement, "Mass: ", entity->mass);
            entity->mass = Slider(gameState, Input, VideoBuffer, &MenuPlacement, entity->mass, 0, 4);

            Text(gameState, VideoBuffer, &MenuPlacement, "Position: ", entity->t.position);
            Text(gameState, VideoBuffer, &MenuPlacement, "Scale: ", entity->t.scale);
            Text(gameState, VideoBuffer, &MenuPlacement, "Right: ", entity->t.right);
            Text(gameState, VideoBuffer, &MenuPlacement, "Forward: ", entity->t.forward);
            Text(gameState, VideoBuffer, &MenuPlacement, "Up: ", entity->t.up);
            Text(gameState, VideoBuffer, &MenuPlacement, "angularVelocityAxis: ", entity->angularVelocityAxis);
            Text(gameState, VideoBuffer, &MenuPlacement, "angularVelocity: ", entity->angularVelocity);
            Text(gameState, VideoBuffer, &MenuPlacement, "velocity: ", entity->velocity);
        }
        

        saveState->ThreadedRendering = Toggle(gameState, Input, VideoBuffer, &UIPos, saveState->ThreadedRendering, "Threaded rendering");
        if (saveState->ThreadedRendering)
        {
            saveState->Visualize_ThreadedRendering = Toggle(gameState, Input, VideoBuffer, &UIPos, saveState->Visualize_ThreadedRendering, "Visualize Threaded rendering", float2(12, 0));
            saveState->ThreadedRendering_Dynamic = Toggle(gameState, Input, VideoBuffer, &UIPos, saveState->ThreadedRendering_Dynamic, "Dynamic render regions", float2(12, 0));

            char text[100];
            for (int i = 0; i < 100; i++)
            {
                text[i] = 0;
            }
            Append(text, "Number of threads: ");
            Append(text, saveState->ScreenThreads);
            if (Button(gameState, Input, VideoBuffer, &UIPos, text))
            {
                saveState->ScreenThreads *= 2;
                if (saveState->ScreenThreads > 32)
                {
                    saveState->ScreenThreads = 4;
                }
            }
        }

        //Text(gameState, VideoBuffer, &UIPos, "SliderTestValue: ", gameState->SliderTestValue);
        //gameState->SliderTestValue = Slider(gameState, Input, VideoBuffer, &UIPos, gameState->SliderTestValue, -1, 3, float2(0, 0));

        // Debugging
        DrawRectangle(VideoBuffer, float2(0, VideoBuffer->Height - 200), float2(200, 200), float3(0.0, 0.0, 0.0), 0.5);

        int64 Global_End = Memory->PlatformTime();
        float TotalTimeDelta = (float)(Global_End - Global_Start) / 10000000;
        float SmoothedTime = GetSmoothedValue(gameState->deltas2, &gameState->deltas2Index, TotalTimeDelta, 100);

        float HeightMultiplier = 5000;
        float LastHeight = 0;
        for (int i = 0; i < 100; i++)
        {
            int h = (i + gameState->deltas2Index) % 100;
            float height = VideoBuffer->Height - gameState->deltas2[h] * HeightMultiplier;
            DrawLine(VideoBuffer, float2(i * 2 - 2, LastHeight), float2(i * 2, height));
            LastHeight = height;
        }

        float fps30dt = (1.0f / 30.0f);
        float fps60dt = (1.0f / 60.0f);
        float fps30height = VideoBuffer->Height - fps30dt * HeightMultiplier;
        float fps60height = VideoBuffer->Height - fps60dt * HeightMultiplier;
        float fpscurrentheight = VideoBuffer->Height - (SmoothedTime)*HeightMultiplier;
        DrawLine(VideoBuffer, float2(0, fps30height), float2(200, fps30height));
        DrawLine(VideoBuffer, float2(0, fps60height), float2(200, fps60height));
        font_draw(gameState->fontSpritesheet, VideoBuffer, 220, fps30height, "33.33 ms");
        font_draw(gameState->fontSpritesheet, VideoBuffer, 220, fps60height, "16.66 ms");

        char Space[100] = {};
        Append(Space, (int)(SmoothedTime));
        font_draw(gameState->fontSpritesheet, VideoBuffer, 202, fpscurrentheight, Space);
    }
}