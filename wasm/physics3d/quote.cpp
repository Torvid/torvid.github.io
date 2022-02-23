//#pragma optimize( "g", on )

#pragma once

#include "quote.h"
#include "string.cpp"
#include "drawing.cpp"

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


        GameState->tSine = 0;

        float SineValue = 0;// sinf(GameState->tSine);

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
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "MouseX: ", Input->MouseX);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "MouseY: ", Input->MouseY);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "MouseZ: ", Input->MouseZ);
    ui_height += 12;

    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "MouseLeft: ", Input->MouseLeft);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "MouseRight: ", Input->MouseRight);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "MouseMiddle: ", Input->MouseMiddle);
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

    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Space: ", Input->Space);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Backspace: ", Input->Backspace);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Enter: ", Input->Enter);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Escape: ", Input->Escape);

    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Left: ", Input->Left);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Right: ", Input->Right);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Up: ", Input->Up);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Down: ", Input->Down);

    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Shift: ", Input->Shift);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Ctrl: ", Input->Ctrl);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Alt: ", Input->Alt);

    ui_height += 12;
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "A: ", Input->Controllers[0].A);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "B: ", Input->Controllers[0].B);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "X: ", Input->Controllers[0].X);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Y: ", Input->Controllers[0].Y);

    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Left: ", Input->Controllers[0].Left);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Right: ", Input->Controllers[0].Right);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Up: ", Input->Controllers[0].Up);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Down: ", Input->Controllers[0].Down);

    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "LeftStickX: ", Input->Controllers[0].LeftStickX);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "LeftStickY: ", Input->Controllers[0].LeftStickY);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "RightStickX: ", Input->Controllers[0].RightStickX);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "RightStickY: ", Input->Controllers[0].RightStickY);

    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "LeftBumper: ", Input->Controllers[0].LeftBumper);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "RightBumper: ", Input->Controllers[0].RightBumper);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "LeftTrigger: ", Input->Controllers[0].LeftTrigger);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "RightTrigger: ", Input->Controllers[0].RightTrigger);

    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Start: ", Input->Controllers[0].Start);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Back: ", Input->Controllers[0].Back);

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

//(thread_context* Thread, game_memory* Memory, game_input* Input, game_offscreen_buffer* VideoBuffer, game_sound_output_buffer* SoundBuffer)
#if WASM
#else
__declspec(dllexport)
#endif
extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    int64 Global_Start = Memory->PlatformTime();
    Assert(sizeof(GameState) <= Memory->PermanentStorageSize);
    //Assert((&Input->Controllers[0].Terminator - &Input->Controllers[0].Buttons[0]) == (ArrayCount(Input->Controllers[0].Buttons) - 1));
    
    InitGlobalFunctions(Memory);

    GameState* gameState = (GameState*)Memory->PermanentStorage;


    if (Input->SpaceDown)
    {
        Memory->Initialized = false;
        for (int i = 0; i < Memory->PermanentStorageSize; i++)
        {
            ((char*)Memory->PermanentStorage)[i] = 0;
        }
        for (int i = 0; i < Memory->TransientStorageSize; i++)
        {
            ((char*)Memory->TransientStorage)[i] = 0;
        }
    }

    Image* Apothem;
    Image* MagSqueeze;
    Image* Wuff;
    float SphereRadius = 0.5;
    bool init = false;
    if (!Memory->Initialized)
    {
        init = true;
        Memory->Initialized = true;
        gameState->GameTime = 0;
    
        InitializeArena(&gameState->worldArena,     Megabytes(1), (uint8*)Memory->PermanentStorage + sizeof(GameState));
        InitializeArena(&gameState->textureArena,   Megabytes(12), (uint8*)Memory->PermanentStorage + sizeof(GameState) + Megabytes(1));
    
        //gameState->fontSpritesheet = PushStruct(&gameState->textureArena, Spritesheet);
        //font_init(gameState->fontSpritesheet, Memory);
    
        gameState->fontSpritesheet = LoadImage(Memory, gameState, "font.tga", 192, 52);
        Apothem      = LoadImage(Memory, gameState, "TestImage.tga", 300, 420);
        //MagSqueeze   = LoadImage(Memory, gameState, "TestImage2.tga", 512, 682);
        Wuff = LoadImage(Memory, gameState, "Wuff.tga", 256, 256);
        gameState->SkyFaces[0] = LoadImage(Memory, gameState, "Sky_C_Left.tga", 512, 512);
        gameState->SkyFaces[1] = LoadImage(Memory, gameState, "Sky_C_Right.tga", 512, 512);
        gameState->SkyFaces[2] = LoadImage(Memory, gameState, "Sky_C_Front.tga", 512, 512); //
        gameState->SkyFaces[3] = LoadImage(Memory, gameState, "Sky_C_Back.tga", 512, 512); //
        gameState->SkyFaces[4] = LoadImage(Memory, gameState, "Sky_C_Top.tga", 512, 512); //
        gameState->SkyFaces[5] = LoadImage(Memory, gameState, "Sky_C_Down.tga", 512, 512); //

        gameState->RayStartX = 100;
        gameState->RayStartY = 100;

        gameState->RayDirX = 100;
        gameState->RayDirY = 100;
        gameState->UseFiltering = true;
        gameState->Camera = transform(float3(0, -5, 0), float3(0, 1, 0), float3(0, 0, 1), float3(1, 1, 1));
        gameState->GameViewEnabled = true;

        //gameState->SpherePos = float2(450, 300);
        //gameState->SphereRight = float2(1, 0);
        //gameState->SphereUp = float2(0,1);
        gameState->TestSphere = transform(float3(0, 0, 1), float3(0,1,0), float3(0,0,1), float3(SphereRadius, SphereRadius, SphereRadius));

    }
    gameState->GameTime += Input->DeltaTime;
    int uiOffset = 0;

    if (Input->GDown)
        gameState->GameViewEnabled = !gameState->GameViewEnabled;
    //if (Input->FDown)
    //    gameState->PrefViewEnabled = !gameState->PrefViewEnabled;

    if ((Input->MouseLeft || Input->MouseRight) && !gameState->RayStartGrabbed && !gameState->RayDirGrabbed && !gameState->Grabbing)
    {
        gameState->Camera = rotate(gameState->Camera, float3(0, 0, 1), -Input->MouseXDelta * 0.005f);
        gameState->Camera = rotate(gameState->Camera, gameState->Camera.right, -Input->MouseYDelta * 0.005f);
    }

    int X = Input->W ? 1 : 0 + Input->S ? -1 : 0;
    int Y = Input->D ? 1 : 0 + Input->A ? -1 : 0;
    int Z = Input->E ? 1 : 0 + Input->Q ? -1 : 0;

    float3 Movement = float3(0, 0, 0);
    Movement = Movement + gameState->Camera.forward * X;
    Movement = Movement + gameState->Camera.right * Y;
    Movement = Movement + gameState->Camera.up * Z;
    if(length(Movement) > 0)
        Movement = normalize(Movement);
    float speed = 4;
    if (Input->Shift)
        speed = 8;
    gameState->Camera.position += Movement * Input->DeltaTime * speed;

    float3 BoxForward3D = float3(1, 0, 0); 
    float3 BoxUp3D = float3(0, 0, 1);
    float3 BoxScale = float3(1, 1, 1);
    float3 BoxScale2 = float3(1, 1, 2);
    
    float3 RotatingBoxForward = float3(cos(gameState->GameTime), sin(gameState->GameTime), 0);

    int64 time_start = Memory->PlatformTime();
    //transform Boxes[6] = {};
    //Boxes[0] = transform(float3(8, 0, 1.1),     BoxForward3D, BoxUp3D, BoxScale);
    //Boxes[1] = transform(float3(0, 0, 2.2),     BoxForward3D, BoxUp3D, BoxScale);
    //Boxes[2] = transform(float3(1.5, 0, 0),     BoxForward3D, BoxUp3D, BoxScale2);
    //Boxes[3] = transform(float3(1.1, 0, 1.1),   BoxForward3D, BoxUp3D, BoxScale);
    //Boxes[4] = transform(float3(0, 0, 0),       BoxForward3D, BoxUp3D, BoxScale);
    //Boxes[5] = transform(float3(0, 0, 0.4),     BoxForward3D, BoxUp3D, BoxScale);
    //
    //bool spheres[6] = { 0, 0, 0, 0, 0, 1 };
    DrawSkybox(gameState, VideoBuffer, &gameState->Camera);
    //if(gameState->PrefViewEnabled)
    //    DrawSkybox(gameState, VideoBuffer, &gameState->Camera);
    //else
    //    DrawRectangle(VideoBuffer, 0, 0, 1920 / 2, 1080 / 2, 0.5, 0, 0);
    //for (int i = 0; i < 6; i++)
    //{
    //    if (gameState->PrefViewEnabled)
    //        DrawObject(gameState, VideoBuffer, &gameState->Camera, &Boxes[i], spheres[i]);
    //    else
    //        DrawObjectCheap(gameState, VideoBuffer, &gameState->Camera, &Boxes[i], spheres[i]);
    //}

    
    //if (Input->MouseLeftUp)
    //{
    //    gameState->Grabbing = false;
    //}
    //
    //float3 MouseRay = ScreenPointToRay(VideoBuffer, gameState->Camera, float2(Input->MouseX, Input->MouseY));
    //
    //if (Input->MouseLeft && gameState->Grabbing)
    //{
    //    float MoveSpeed = 0.01;
    //    gameState->TestSphere.position += Input->MouseXDelta * gameState->Camera.right * MoveSpeed;
    //    gameState->TestSphere.position += -Input->MouseYDelta * gameState->Camera.up * MoveSpeed;
    //}

    //transform Box = transform(float3(0, 0, 0), RotatingBoxForward, BoxUp3D, float3(1, 2, 3));
    //transform Box = transform(float3(0, 0, 0), BoxForward3D, BoxUp3D, float3(2, 3, 1));
    //Box = rotate(Box, Input->DeltaTime, 0, 0),
    //gameState->TestSphere.position += SphereBoxDeIntersection(Box, gameState->TestSphere);
    //
    //float hit = RaySphereIntersect(gameState->Camera.position - gameState->TestSphere.position, MouseRay, SphereRadius);
    //if (hit > 0)
    //{
    //    transform SphereSelect = gameState->TestSphere;
    //    SphereSelect.scale *= 1.03;
    //    DrawObjectCheap(gameState, VideoBuffer, gameState->Camera, SphereSelect, true);
    //    if (Input->MouseLeftDown)
    //    {
    //        gameState->Grabbing = true;
    //    }
    //}

    //DrawObject(gameState, VideoBuffer, gameState->Camera, gameState->TestSphere, true);
    //DrawObject(gameState, VideoBuffer, gameState->Camera, Box, false);



    gameState->Bodies3D[0] = RigidBody3DBox(transform(float3(1.2, 4, 0), BoxForward3D, BoxUp3D, float3(3, 1, 1)), 1, true);
    gameState->Bodies3D[1] = RigidBody3DBox(transform(float3(0, 2, 0), BoxForward3D, BoxUp3D, float3(1, 1, 1)), 1, true);
    gameState->Bodies3D[2] = RigidBody3DBox(transform(float3(0, 4, 0), BoxForward3D, BoxUp3D, float3(1, 1, 1)), 1, true);
    gameState->Bodies3D[2].t = rotate(gameState->Bodies3D[2].t, float3(1, 0, 0), 1);
    gameState->Bodies3D[4] = RigidBody3DBox(transform(float3(0, 6, -3), BoxForward3D, BoxUp3D, float3(10, 10, 0.5)), 1, true);
    gameState->Bodies3D[4].t = rotate(gameState->Bodies3D[4].t, float3(1, 0, 0), 0.25);

    //gameState->Bodies3D[3] = RigidBody3DBox(transform(float3(0, 0, -6), BoxForward3D, BoxUp3D, float3(40, 20, 0.5)), 1, true);

    int i = 0;
    for (int x = 0; x < 5; x++)
    {
        for (int y = 0; y < 5; y++)
        {
            gameState->Bodies3D[i + 5] = RigidBody3DBox(transform(float3(
                x * 4-8,
                y * 4-8,
                -6), BoxForward3D, BoxUp3D, float3(4, 4, 1)), 1, true);
            i++;
        }
    }

    if (Input->FDown || init)
    {
        int i = 0;
        for (int x = 0; x < 2; x++)
        {
            for (int y = 0; y < 2; y++)
            {
                for (int z = 0; z < 3; z++)
                {
                    gameState->Bodies3D[i + 40] = RigidBody3DSphere(transform(float3(
                        x * 4.1 + 1.5 + z * 0.344,
                        y * 4.1 + 4 - z * 0.344,
                        z * 2.5 + 5), BoxForward3D, BoxUp3D, float3(1, 1, 1)), 1, true);
                    i++;
                }
            }
        }
        //gameState->Bodies3D[15] = RigidBody3DSphere(transform(float3(0, 4, 3), BoxForward3D, BoxUp3D, float3(1, 1, 1)), 1, true);
        //gameState->Bodies3D[16] = RigidBody3DSphere(transform(float3(0, 7, 3), BoxForward3D, BoxUp3D, float3(1, 1, 1)), 1, true);
        //gameState->Bodies3D[17] = RigidBody3DSphere(transform(float3(0, 9, 3), BoxForward3D, BoxUp3D, float3(1, 1, 1)), 1, true);
        //gameState->Bodies3D[15].t = rotate(gameState->Bodies3D[15].t, 10, 45, 75);
    }

    for (int i = 0; i < ArrayCount(gameState->Bodies3D); i++)
    {
        RigidBody3D* Body = &gameState->Bodies3D[i];
        if (!Body->initialized)
            continue;

        // Draw
        DrawObject(gameState, VideoBuffer, gameState->Camera, Body->t, Body->type == bodytype_sphere);
        //DrawObject(gameState, VideoBuffer, gameState->Camera, Body->t, Body->type == bodytype_sphere);

        // De-intersect
        // Currently only spheres can be dynamic
        if (Body->type != bodytype_sphere)
            continue;

        Body->velocity += float3(0, 0, -0.005);
        Body->t.position += Body->velocity;

        RigidBody3D* HitDynamicBody = 0;

        float3 DeintersectVector = float3(0, 0, 0);
        for (int j = 0; j < 10; j++)
        {
            for (int k = 0; k < ArrayCount(gameState->Bodies3D); k++)
            {
                RigidBody3D* OtherBody = &gameState->Bodies3D[k];
                if (!OtherBody->initialized)
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
                DrawLine3D(VideoBuffer, gameState->Camera, Body->t.position, Body->t.position + normal);
                DrawLine3D(VideoBuffer, gameState->Camera, Body->t.position, Body->t.position + tangent);
                DrawLine3D(VideoBuffer, gameState->Camera, Body->t.position, Body->t.position + binormal);

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

        DrawLine3D(VideoBuffer, gameState->Camera, Body->t.position, Body->t.position + Body->velocity);
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



    float FPS = (float)(1.0f / Input->DeltaTime);
    float Delta = (float)(time_end - time_start) / 10000000;
    float accumulatedDelta = GetSmoothedValue(gameState->deltas, &gameState->deltasIndex, Delta, 30);
    float SmoothFPS = GetSmoothedValue(gameState->FPS, &gameState->FPSIndex, FPS, 30);

    DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "Controls", 0);
    DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "    Space to reset program.", 0);
    DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "    G to toggle debug view.", 0);
    DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "    WASD to move.", 0);
    DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "    Right mouse to look around.", 0);

    DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "    F to reset spheres.", 0);
    //DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "   value: ", gameState->Camera.forward);

    if (!gameState->GameViewEnabled)
    {
        DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, " ", 0);
        DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "Timers", 0);
        DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "    Draw Boxes MS: ", Delta * 1000);
        DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "    Draw Boxes Smooth MS: ", accumulatedDelta * 1000);
        DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "    FPS: ", FPS);
        DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "    Smooth FPS: ", SmoothFPS);
    }

    /*
    float rx = gameState->RayStartX;
    float ry = gameState->RayStartY;
    float dx = gameState->RayDirX;
    float dy = gameState->RayDirY;

    // normalize
    float len = sqrt(dx * dx + dy * dy);
    dx = dx / len;
    dy = dy / len;

    float width = 150;
    float height = 60;
    float x = 300;
    float y = 300;
    DrawBox(VideoBuffer, x, y, width, height);
    float ClosestDistance = RayBoxIntersect(VideoBuffer, rx, ry, dx, dy, x, y, width, height);
    if (ClosestDistance > 0)
    {
        DrawCircle(VideoBuffer, rx + dx * ClosestDistance, ry + dy * ClosestDistance, 5);
    }

    RayCircleIntersect(VideoBuffer, rx, ry, dx, dy, 100);
    if (distance(Input->MouseX, Input->MouseY, gameState->RayStartX, gameState->RayStartY) < 10)
    {
        DrawCircle(VideoBuffer, gameState->RayStartX, gameState->RayStartY, 8);
        DrawCircle(VideoBuffer, gameState->RayStartX, gameState->RayStartY, 7.5);
        DrawCircle(VideoBuffer, gameState->RayStartX, gameState->RayStartY, 7);

        if (Input->MouseLeftDown)
        {
            gameState->RayStartGrabbed = true;
        }
    }
    else
    {
        DrawCircle(VideoBuffer, gameState->RayStartX, gameState->RayStartY, 8);
    }
    if (distance(Input->MouseX, Input->MouseY, gameState->RayStartX + gameState->RayDirX, gameState->RayStartY + gameState->RayDirY) < 10)
    {
        DrawCircle(VideoBuffer, gameState->RayStartX + gameState->RayDirX, gameState->RayStartY + gameState->RayDirY, 8);
        DrawCircle(VideoBuffer, gameState->RayStartX + gameState->RayDirX, gameState->RayStartY + gameState->RayDirY, 7.5);
        DrawCircle(VideoBuffer, gameState->RayStartX + gameState->RayDirX, gameState->RayStartY + gameState->RayDirY, 7);
        if (Input->MouseLeftDown)
        {
            gameState->RayDirGrabbed = true;
        }
    }
    else
    {
        DrawCircle(VideoBuffer, gameState->RayStartX + gameState->RayDirX, gameState->RayStartY + gameState->RayDirY, 8);
    }
    if (Input->MouseLeftUp)
    {
        gameState->RayStartGrabbed = false;
        gameState->RayDirGrabbed = false;
    }
    if (gameState->RayStartGrabbed)
    {
        gameState->RayStartX += Input->MouseXDelta;
        gameState->RayStartY += Input->MouseYDelta;
        gameState->RayDirX -= Input->MouseXDelta;
        gameState->RayDirY -= Input->MouseYDelta;
    }
    if (gameState->RayDirGrabbed)
    {
        gameState->RayDirX += Input->MouseXDelta;
        gameState->RayDirY += Input->MouseYDelta;
    }
    DrawLine(VideoBuffer, gameState->RayStartX, gameState->RayStartY, gameState->RayStartX + gameState->RayDirX * 1.25, gameState->RayStartY + gameState->RayDirY * 1.25);
    */
    
    if (!gameState->GameViewEnabled)
    {
        DrawRectangle(VideoBuffer, 0, VideoBuffer->Height - 200, 200, 200, 0.0, 0.0, 0.0, 0.5);

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