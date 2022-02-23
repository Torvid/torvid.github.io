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
    if (!Memory->Initialized)
    {
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
        //gameState->SkyFaces[0] = LoadImage(Memory, gameState, "Sky_C_Left.tga", 512, 512);
        //gameState->SkyFaces[1] = LoadImage(Memory, gameState, "Sky_C_Right.tga", 512, 512);
        //gameState->SkyFaces[2] = LoadImage(Memory, gameState, "Sky_C_Front.tga", 512, 512); //
        //gameState->SkyFaces[3] = LoadImage(Memory, gameState, "Sky_C_Back.tga", 512, 512); //
        //gameState->SkyFaces[4] = LoadImage(Memory, gameState, "Sky_C_Top.tga", 512, 512); //
        //gameState->SkyFaces[5] = LoadImage(Memory, gameState, "Sky_C_Down.tga", 512, 512); //

        gameState->RayStartX = 100;
        gameState->RayStartY = 100;

        gameState->RayDirX = 100;
        gameState->RayDirY = 100;
        gameState->UseFiltering = true;
        gameState->Camera = transform(float3(0, -5, 0), float3(0, 1, 0), float3(0, 0, 1), float3(1, 1, 1));
        gameState->GameViewEnabled = true;

        gameState->SpherePos = float2(450, 300);
        gameState->SphereRight = float2(1, 0);
        gameState->SphereUp = float2(0,1);

    }
    gameState->GameTime += Input->DeltaTime;
    int uiOffset = 0;
    
    if (Input->GDown)
        gameState->GameViewEnabled = !gameState->GameViewEnabled;

    if (Input->MouseLeft && !gameState->RayStartGrabbed && !gameState->RayDirGrabbed)
    {
        rotate(&gameState->Camera, float3(0, 0, 1), -Input->MouseXDelta * 0.005f);
        rotate(&gameState->Camera, gameState->Camera.right, -Input->MouseYDelta * 0.005f);
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
    float speed = 2;
    if (Input->Shift)
        speed = 4;
    gameState->Camera.position += Movement * Input->DeltaTime * speed;

    float3 BoxForward3D = float3(1, 0, 0); 
    float3 BoxUp3D = float3(0, 0, 1);
    float3 BoxScale = float3(1, 1, 1);
    float3 BoxScale2 = float3(1, 1, 2);
    
    float3 RotatingBoxForward = float3(cos(gameState->GameTime), sin(gameState->GameTime), 0);

    int64 time_start = Memory->PlatformTime();
    DrawRectangle(VideoBuffer, 0, 0, 1920 / 2, 1080 / 2, 0.5, 0, 0);
    transform Boxes[6] = {};
    Boxes[0] = transform(float3(8, 0, 1.1),     BoxForward3D, BoxUp3D, BoxScale);
    Boxes[1] = transform(float3(0, 0, 2.2),     BoxForward3D, BoxUp3D, BoxScale);
    Boxes[2] = transform(float3(1.5, 0, 0),     BoxForward3D, BoxUp3D, BoxScale2);
    Boxes[3] = transform(float3(1.1, 0, 1.1),   BoxForward3D, BoxUp3D, BoxScale);
    Boxes[4] = transform(float3(0, 0, 0),       BoxForward3D, BoxUp3D, BoxScale);
    Boxes[5] = transform(float3(0, 0, 0.4),     BoxForward3D, BoxUp3D, BoxScale);
    
    float SphereRadius = 50;
    float2 BoxPos[4] = { float2(500, 500), float2(650, 200), float2(250, 400), float2(900, 400) };
    float2 BoxSize[4] = { float2(750, 50), float2(400, 5), float2(550, 60), float2(50, 200) };
    float2 BoxUp[4] = { normalize(float2(0.2, 2.5)), normalize(float2(0, 0.5)), normalize(float2(-0.2, 0.5)),normalize(float2(-0.0, 0.5)) };
    float2 BoxRight[4] = {};
    for (int i = 0; i < 4; i++)
    {
        BoxRight[i] = float2(-BoxUp[i].y, BoxUp[i].x);
        DrawBox(VideoBuffer, BoxPos[i], BoxSize[i], BoxUp[i], BoxRight[i]);
        DrawLine(VideoBuffer, BoxPos[i], BoxPos[i] + BoxUp[i] * 25);
        DrawLine(VideoBuffer, BoxPos[i], BoxPos[i] + BoxRight[i] * 25);
    }

    DrawCircle(VideoBuffer, gameState->SpherePos, SphereRadius);
    DrawLine(VideoBuffer, gameState->SpherePos, gameState->SpherePos + gameState->SphereUp    * SphereRadius);
    DrawLine(VideoBuffer, gameState->SpherePos, gameState->SpherePos + gameState->SphereRight * SphereRadius);

    // bouncyness
    // friction
    // drag

    gameState->SphereVel += float2(0, 1); // add gravity
    gameState->SpherePos += gameState->SphereVel;

    float2 DeIntersectPosition = gameState->SpherePos;
    float2 LastDeIntersectPosition = gameState->SpherePos;
    float2 DeintersectVector = float2(0, 0);
    for (int j = 0; j < 100; j++)
    {
        for (int i = 0; i < 4; i++)
        {
            float2 DeIntersect = CircleBoxDeIntersection(BoxPos[i], BoxSize[i], BoxUp[i], BoxRight[i], gameState->SpherePos, SphereRadius);
            DeintersectVector += DeIntersect;
            DeIntersectPosition += DeIntersect;
            DrawLine(VideoBuffer, LastDeIntersectPosition, DeIntersectPosition);
            LastDeIntersectPosition = DeIntersectPosition;
            if (length(DeIntersect) > 0)
            {
                float MoveDistance = distance(gameState->LastSpherePos, gameState->SpherePos);
                gameState->SpherePos += DeIntersect;
            }
        }
    }

    float bouncyness = 0;
    if (Input->FDown)
        gameState->Bouncy = !gameState->Bouncy;
    bouncyness  = gameState->Bouncy ? 1 : 0;
    if (length(DeintersectVector) > 0)
    {
        // When something hits something, angular velocity and velocity try to balance out. 
        // If it's spinning a lot, it starts moves
        // If it's moving a lot, it starts spinning
        
        float2 normal = normalize(DeintersectVector);
        float2 tangent = float2(-normal.y, normal.x);
        gameState->SphereVel  *= 0.5f;
        gameState->AngularVel *= 0.5f;
        gameState->AngularVel += dot(tangent, gameState->SphereVel) / 100;
        gameState->SphereVel += tangent * gameState->AngularVel * 50;

        //gameState->SphereVel *= 0.70f;
        //gameState->AngularVel *= 0.70f;
        //gameState->AngularVel += dot(tangent, gameState->SphereVel) / 200;
        //gameState->SphereVel += tangent * gameState->AngularVel * 25;

        float2 BounceVel = reflect(gameState->SphereVel, normal);
        gameState->SphereVel = lerp(gameState->SphereVel, BounceVel, bouncyness);
    }

    gameState->SphereUp = rotate(gameState->SphereUp, gameState->AngularVel);
    gameState->SphereRight = rotate(gameState->SphereRight, gameState->AngularVel);

    gameState->LastSpherePos = gameState->SpherePos;

    if (Input->MouseLeft)
    {
        gameState->SpherePos = float2(Input->MouseX, Input->MouseY);
        gameState->SphereVel = float2(0, 0);
    }
    gameState->AngularVel += Y * 0.01;
    gameState->SphereVel += float2(Z, 0);


    bool spheres[6] = { 0, 0, 0, 0, 0, 1 };
    //DrawSkybox(gameState, VideoBuffer, &gameState->Camera);
    for (int i = 0; i < 6; i++)
    {
        //DrawObject(gameState, VideoBuffer, &gameState->Camera, &Boxes[i], spheres[i]);
    }

    int64 time_end = Memory->PlatformTime();
    // testing
#if 0
    DrawRectangle(VideoBuffer, 0, 0, 960, 540, 0.5, 0.0, 0.0);
    DrawRectangle(VideoBuffer, 50, 50, 60, 60, 0.0, 0.5, 0.0);
    DrawRectangle(VideoBuffer, 120, 40, 10, 80, 0.0, 0.5, 0.0);
    DrawRectangle(VideoBuffer, 140, 30, 10, 100, 0.0, 0.5, 0.0);
    DrawImage(VideoBuffer, gameState->fontSpritesheet, 100, 100);
    for (int i = 0; i < gameState->fontSpritesheet->width* gameState->fontSpritesheet->height; i++)
    {
        ((uint32*)VideoBuffer->Memory)[i+300000] = gameState->fontSpritesheet->ImageData[i];
    }
#endif
    
    float FPS = (float)(1.0f / Input->DeltaTime);
    float Delta = (float)(time_end - time_start) / 10000000;
    float accumulatedDelta = GetSmoothedValue(gameState->deltas, &gameState->deltasIndex, Delta, 30);
    float SmoothFPS = GetSmoothedValue(gameState->FPS, &gameState->FPSIndex, FPS, 30);

    DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "Controls", 0);
    DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "    Space to reset.", 0);
    DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "    G to toggle debug view.", 0);
    //DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "    WASD to move.", 0);
    //DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "    Left mouse to look around.", 0);
    DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "    LMB place circle.", 0);
    DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "    WASD add rotation speed.", 0);
    DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "    F toggle bouncyness.", gameState->Bouncy);
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