//#pragma optimize( "g", on )

#pragma once

#include "quote.h"
#include "string.cpp"
#include "drawing.cpp"
#include "sound.cpp"
#include "StringSound.cpp"
#include "physics.cpp"

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
void MakeEntity(GameState* gameState, Entity* entity, 
    transform t, bool mesh, int type, int texture1, int texture2, // mesh
    int shader, bool physics = 0, bool kinematic = 0, float mass = 1, // physics
    EffectData effectData = {})
{
    *entity = {};
    entity->initialized = true;

    // Transform
    entity->t = t;

    // Mesh
    entity->mesh = mesh;
    entity->type = (EntityType)type;
    entity->texture1 = &gameState->Images[texture1];
    entity->texture2 = &gameState->Images[texture2];
    entity->ShaderIndex = shader;

    // Physics
    entity->physics = physics;
    entity->kinematic = kinematic;
    entity->mass = mass;

    entity->effect = effectData;

    entity->Color = float3(1, 1, 1);
    // Particle
    //entity->spawnRate = spawnRate;
    //entity->spawnRadius = spawnRadius;
    //entity->minSpeed = minSpeed;
    //entity->maxSpeed = maxSpeed;
    //entity->startSize = startSize;
    //entity->endSize = endSize;
}

void AppendArgument(int max, char* Str, float3 in)
{
    Append(Str, ", ", max);
    CoerceAppend(Str, in, max);
}
void AppendArgument(int max, char* Str, float in)
{
    Append(Str, ", ", max);
    CoerceAppend(Str, in, max);
}
void AppendArgument(int max, char* Str, int in)
{
    Append(Str, ", ", max);
    Append(Str, in, max);
}


void WriteEntity(GameState* gameState, char* target, Entity* entity, int index)
{
    int max = ArrayCount(gameState->SaveBuffer);
    Append(target, "MakeEntity", max);
    //if (entity->mesh)
    //{
    //    if (entity->type == EntityType_Box)
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
    AppendArgument(max, target, entity->mesh);
    AppendArgument(max, target, entity->type);
    AppendArgument(max, target, FindImageIndex(gameState, entity->texture1));
    AppendArgument(max, target, FindImageIndex(gameState, entity->texture2));
    AppendArgument(max, target, entity->ShaderIndex);

    // Physics
    AppendArgument(max, target, entity->physics);
    AppendArgument(max, target, entity->kinematic);
    AppendArgument(max, target, entity->mass);
    
    // Particle
    Append(target, ", ctor_EffectData(", max);
    Append(target, entity->effect.spawnRate, max);
    AppendArgument(max, target, entity->effect.burst);
    AppendArgument(max, target, entity->effect.spawnRadius);
    AppendArgument(max, target, entity->effect.drag);
    AppendArgument(max, target, entity->effect.minSpeed);
    AppendArgument(max, target, entity->effect.maxSpeed);
    AppendArgument(max, target, entity->effect.minLifetime);
    AppendArgument(max, target, entity->effect.maxLifetime);
    AppendArgument(max, target, entity->effect.startSize);
    AppendArgument(max, target, entity->effect.endSize);
    AppendArgument(max, target, entity->effect.gravity);
    Append(target, ")", max);


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
        InitializeArena(&gameState->textureArena, Megabytes(12), (uint8*)Memory->TransientStorage + sizeof(GameState) + Kilobytes(100));
        InitializeArena(&gameState->soundArena,   Megabytes(12), (uint8*)Memory->TransientStorage + sizeof(GameState) + Kilobytes(100) + Megabytes(12));
    
        //gameState->fontSpritesheet = PushStruct(&gameState->textureArena, Spritesheet);
        //font_init(gameState->fontSpritesheet, Memory);
    
        gameState->fontSpritesheet = LoadImage(Memory, gameState, "font.tga", 192, 52);
        //Apothem      = LoadImage(Memory, gameState, "TestImage.tga", 300, 420);
        //MagSqueeze   = LoadImage(Memory, gameState, "TestImage2.tga", 512, 682);
        Wuff = LoadImage(Memory, gameState, "Wuff.tga", 256, 256);

        LoadImage(Memory, gameState, "textures/Floor_Top_Border.tga", 128, 128);
        LoadImage(Memory, gameState, "textures/Floor_Top_Flat.tga", 128, 128);
        LoadImage(Memory, gameState, "textures/ExtraBall.tga", 64, 64);
        gameState->SkyFaces[0] = LoadImage(Memory, gameState, "textures/Sky_C_Left.tga", 512, 512);
        gameState->SkyFaces[1] = LoadImage(Memory, gameState, "textures/Sky_C_Right.tga", 512, 512);
        gameState->SkyFaces[2] = LoadImage(Memory, gameState, "textures/Sky_C_Front.tga", 512, 512);
        gameState->SkyFaces[3] = LoadImage(Memory, gameState, "textures/Sky_C_Back.tga", 512, 512);
        gameState->SkyFaces[4] = LoadImage(Memory, gameState, "textures/Sky_C_Top.tga", 512, 512);
        gameState->SkyFaces[5] = LoadImage(Memory, gameState, "textures/Sky_C_Down.tga", 512, 512);


        LoadSound(Memory, gameState, "sound/Music_Theme_2_1.wav", 7.326);
        LoadSound(Memory, gameState, "sound/Misc_extraball.wav", 0.908);
        LoadSound(Memory, gameState, "sound/Hit_Wood_Dome.wav", 2.049);
        LoadSound(Memory, gameState, "sound/Extra_Life_Blob.wav", 0.317);
        
        gameState->UseFiltering = true;

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
        gameState->stringData.PixelsPerSample = 10;
        gameState->ChosenSoundLoop = true;
        gameState->MasterVolume = 1.0f;
        for (int i = 0; i < ArrayCount(gameState->soundChannels); i++)
        {
            StopSound(gameState, i);
        }
    }
    gameState->SliderID = 0;
    gameState->GameTime += Input->DeltaTime;
    int uiOffset = 0;

    if (Input->GDown)
    {
        saveState->GameViewEnabled = !saveState->GameViewEnabled;
        for (int i = 0; i < ArrayCount(gameState->SelectedEntities); i++)
        {
            gameState->SelectedEntities[i] = 0;
        }
    }

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


    gameState->NextRenderCommand = 0;
    QueueDrawSkyBox(gameState, gameState->SkyFaces);
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
        if(e->type == EntityType_Sphere)
            QueueDrawSphere(gameState, e->t, e->texture1, e->texture2, e->ShaderIndex, selected, Hovered);
        if (e->type == EntityType_Box)
            QueueDrawBox(gameState, e->t, e->texture1, e->texture2, e->ShaderIndex, selected, Hovered);
        if (e->type == EntityType_Effect)
            QueueDrawEffect(gameState, e->t, e->texture1, e->texture2, e->ShaderIndex, e->effect, e->Color);

    }

    // Draws the queued up commands to the screen.
    DrawScene(Memory, gameState, saveState, VideoBuffer);


    // Update sound system
    ClearSoundBuffer(SoundBuffer);
    for (int i = 0; i < ArrayCount(gameState->soundChannels); i++)
    {
        SoundChannel* channel = &gameState->soundChannels[i];
        PlaySoundUpdate(SoundBuffer, channel);
    }


    SetSoundBufferVolume(SoundBuffer, gameState->MasterVolume);
    
    //gameState->MouseIsOverUI = false;

    UpdatePhysics(gameState, saveState, VideoBuffer);
    

    
    int64 time_end = Memory->PlatformTime();

    float2 UIPos = float2(4, 0);

    float FPS = (float)(1.0f / Input->DeltaTime);
    float Delta = (float)(time_end - time_start) / 10000000;
    float accumulatedDelta = GetSmoothedValue(gameState->deltas, &gameState->deltasIndex, Delta, 30);
    float SmoothFPS = GetSmoothedValue(gameState->FPS, &gameState->FPSIndex, FPS, 30);

    Text(gameState, VideoBuffer, &UIPos, "Controls");
    Text(gameState, VideoBuffer, &UIPos, "  G to toggle game view.");
    if (gameState->editorState != gameState->LasteditorState)
    {
        gameState->LasteditorState = gameState->editorState;
        for (int i = 0; i < ArrayCount(gameState->SelectedEntities); i++)
        {
            gameState->SelectedEntities[i] = 0;
        }
    }
    if (!saveState->GameViewEnabled)
    {
        //Text(gameState, VideoBuffer, &UIPos, "Controls");
        Text(gameState, VideoBuffer, &UIPos, "  Space to reset program.");
        Text(gameState, VideoBuffer, &UIPos, "  Ctrl + Space to reset program and wipe save state.");
        Text(gameState, VideoBuffer, &UIPos, "  Right mouse + WASD to move and look.");
        if(Toggle(gameState, Input, VideoBuffer, &UIPos, gameState->editorState == EditorState_Editing, "Editing"))
            gameState->editorState = EditorState_Editing;

        if (Toggle(gameState, Input, VideoBuffer, &UIPos, gameState->editorState == EditorState_Profiling, "Profiling"))
            gameState->editorState = EditorState_Profiling;

        if (Toggle(gameState, Input, VideoBuffer, &UIPos, gameState->editorState == EditorState_Sound, "Sound"))
            gameState->editorState = EditorState_Sound;

        switch (gameState->editorState)
        {
        case EditorState_Editing:
        {

            // EDITOR STUFF

            //Text(gameState, VideoBuffer, &UIPos, "Editor Controls");
            //Text(gameState, VideoBuffer, &UIPos, "  W Translate.");
            //Text(gameState, VideoBuffer, &UIPos, "  E Rotate.");
            //Text(gameState, VideoBuffer, &UIPos, "  R Scale.");
            //Text(gameState, VideoBuffer, &UIPos, "  Delete to delete selection.");
            //Text(gameState, VideoBuffer, &UIPos, "  Ctrl + D to duplicate selection.");
            //Text(gameState, VideoBuffer, &UIPos, "  Left click to select.");
            //Text(gameState, VideoBuffer, &UIPos, "  Shift + Left click add to selection.");
            //Text(gameState, VideoBuffer, &UIPos, "  Ctrl + Left click remove from selection.");

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
            gameState->Grabbing = gameState->PickedSliderID != -1;

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





            float2 MenuPlacement = float2(VideoBuffer->Width - 120, 5);

            //char MenuText[10] = {};
            //gameState->playing ? Append(MenuText, "Playing") : Append(MenuText, "Editing");
            //if (Button(gameState, Input, VideoBuffer, &MenuPlacement, MenuText))
            //{ 
            //    gameState->playing = !gameState->playing;
            //}

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

            char SpaceText[10] = {};
            saveState->GizmoWorldSpace ? Append(SpaceText, "Local Space") : Append(SpaceText, "World Space");
            if (Button(gameState, Input, VideoBuffer, &MenuPlacement, SpaceText))
            {
                saveState->GizmoWorldSpace = !saveState->GizmoWorldSpace;
            }

            Text(gameState, VideoBuffer, &MenuPlacement, "");
            Text(gameState, VideoBuffer, &MenuPlacement, "Entities.");
            for (int i = 0; i < ArrayCount(gameState->Entities); i++)
            {
                Entity* e = &gameState->Entities[i];

                if (!Valid(e))
                    continue;

                char LoadText[100] = "Load Level ";
                if (Button(gameState, Input, VideoBuffer, &MenuPlacement, EntityTypeNames[e->type]))
                {
                    for (int i = 0; i < ArrayCount(gameState->SelectedEntities); i++)
                    {
                        gameState->SelectedEntities[i] = 0;
                    }
                    gameState->SelectedEntities[0] = e;
                }
            }




            Text(gameState, VideoBuffer, &UIPos, " ");


            if (Button(gameState, Input, VideoBuffer, &UIPos, "Snap"))
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

            if (Button(gameState, Input, VideoBuffer, &UIPos, "Reset Selected"))
            {
                for (int i = 0; i < ArrayCount(gameState->SelectedEntities); i++)
                {
                    Entity* entity = gameState->SelectedEntities[i];
                    if (!Valid(entity))
                        continue;

                    //entity->t.scale = float3(1, 1, 1);
                    entity->t.scale = float3(0.5, 0.5, 0.5);

                    entity->t.right = float3(1, 0, 0);
                    entity->t.forward = float3(0, 1, 0);
                    entity->t.up = float3(0, 0, 1);
                    if (entity->type == EntityType_Effect)
                    {
                        entity->effect.spawnRate = 15.0f;
                        entity->effect.burst = false;
                        entity->effect.spawnRadius = 0.1f;
                        entity->effect.drag = 1.0f;
                        entity->effect.minSpeed = float3(-0.1, 0.1, 0.5f);
                        entity->effect.maxSpeed = float3(0.1, 0.1, 1.0f);
                        entity->effect.minLifetime = 1.0f;
                        entity->effect.maxLifetime = 2.0f;
                        entity->effect.startSize = 1.0f;
                        entity->effect.endSize = 2.0f;
                        entity->effect.gravity = -5.0f;
                        entity->Color = float3(1, 1, 1);
                    }
                }
            }
            if (Button(gameState, Input, VideoBuffer, &UIPos, "Play Effect"))
            {
                for (int i = 0; i < ArrayCount(gameState->SelectedEntities); i++)
                {
                    Entity* entity = gameState->SelectedEntities[0];
                    if (!Valid(entity))
                        continue;

                    entity->effect.startTime = gameState->GameTime;
                }

            }

            Text(gameState, VideoBuffer, &UIPos, " ");
            Entity* entity = gameState->SelectedEntities[0];
            if (Valid(entity))
            {
                Text(gameState, VideoBuffer, &UIPos, "Position: ", entity->t.position);
                Text(gameState, VideoBuffer, &UIPos, EntityTypeNames[entity->type]);
                entity->type = (EntityType)(int)round(Slider(gameState, Input, VideoBuffer, &UIPos, entity->type, 0, 4));

                Text(gameState, VideoBuffer, &UIPos, "Shader: ", entity->ShaderIndex);
                entity->ShaderIndex = Slider(gameState, Input, VideoBuffer, &UIPos, entity->ShaderIndex, 0, 4);

                Text(gameState, VideoBuffer, &UIPos, "Texture 1: ");
                int texture1 = FindImageIndex(gameState, entity->texture1);
                texture1 = Slider(gameState, Input, VideoBuffer, &UIPos, texture1, 0, 8);
                entity->texture1 = &gameState->Images[texture1];

                Text(gameState, VideoBuffer, &UIPos, "Texture 2: ");
                int texture2 = FindImageIndex(gameState, entity->texture2);
                texture2 = Slider(gameState, Input, VideoBuffer, &UIPos, texture2, 0, 8);
                entity->texture2 = &gameState->Images[texture2];

                if (entity->type == EntityType_Sphere || entity->type == EntityType_Box)
                {

                    //if (Button(gameState, Input, VideoBuffer, &UIPos, entity->type == EntityType_Sphere ? "Toggle Sphere" : "Toggle Box"))
                    //{
                    //    if (entity->type == EntityType_Sphere) entity->type = EntityType_Box;
                    //    else if (entity->type == EntityType_Box) entity->type = EntityType_Sphere;
                    //}

                    entity->physics = Toggle(gameState, Input, VideoBuffer, &UIPos, entity->physics, "Physics");
                    entity->kinematic = Toggle(gameState, Input, VideoBuffer, &UIPos, entity->kinematic, "Kinematic");
                    if (entity->kinematic || !entity->physics)
                    {
                        entity->angularVelocity = 0;
                        entity->velocity = float3(0, 0, 0);
                    }
                    Text(gameState, VideoBuffer, &UIPos, "Mass: ", entity->mass);
                    entity->mass = Slider(gameState, Input, VideoBuffer, &UIPos, entity->mass, 0, 4);

                }
                else if (entity->type == EntityType_Effect)
                {
                    EffectData* effect = &entity->effect;

                    float2 UIPos2 = float2(160*2, VideoBuffer->Height - 260);
                    float2 UIPos3 = float2(160, VideoBuffer->Height - 260);

                    effect->burst = Toggle(gameState, Input, VideoBuffer, &UIPos3, effect->burst, "Burst");


                    Text(gameState, VideoBuffer, &UIPos3, "spawnRate: ", effect->spawnRate);
                    effect->spawnRate = Slider(gameState, Input, VideoBuffer, &UIPos3, effect->spawnRate, 0, 25);


                    Text(gameState, VideoBuffer, &UIPos3, "spawnRadius: ", effect->spawnRadius);
                    effect->spawnRadius = Slider(gameState, Input, VideoBuffer, &UIPos3, effect->spawnRadius, 0, 4);

                    Text(gameState, VideoBuffer, &UIPos3, "drag: ", effect->drag);
                    effect->drag = Slider(gameState, Input, VideoBuffer, &UIPos3, effect->drag, 0, 4);

                    Text(gameState, VideoBuffer, &UIPos3, "gravity: ", effect->gravity);
                    effect->gravity = Slider(gameState, Input, VideoBuffer, &UIPos3, effect->gravity, -25, 0);



                    Text(gameState, VideoBuffer, &UIPos3, "startSize: ", effect->startSize);
                    effect->startSize = Slider(gameState, Input, VideoBuffer, &UIPos3, effect->startSize, 0, 4);

                    Text(gameState, VideoBuffer, &UIPos3, "endSize: ", effect->endSize);
                    effect->endSize = Slider(gameState, Input, VideoBuffer, &UIPos3, effect->endSize, 0, 4);



                    Text(gameState, VideoBuffer, &UIPos2, "minSpeed.x: ", effect->minSpeed.x);
                    effect->minSpeed.x = Slider(gameState, Input, VideoBuffer, &UIPos2, effect->minSpeed.x, -1, 1);
                    Text(gameState, VideoBuffer, &UIPos2, "minSpeed.y: ", effect->minSpeed.y);              
                    effect->minSpeed.y = Slider(gameState, Input, VideoBuffer, &UIPos2, effect->minSpeed.y, -1, 1);
                    Text(gameState, VideoBuffer, &UIPos2, "minSpeed.z: ", effect->minSpeed.z);              
                    effect->minSpeed.z = Slider(gameState, Input, VideoBuffer, &UIPos2, effect->minSpeed.z, -1, 1);


                    Text(gameState, VideoBuffer, &UIPos2, "maxSpeed.x: ", effect->maxSpeed.x);
                    effect->maxSpeed.x = Slider(gameState, Input, VideoBuffer, &UIPos2, effect->maxSpeed.x, -1, 1);
                    Text(gameState, VideoBuffer, &UIPos2, "maxSpeed.y: ", effect->maxSpeed.y);              
                    effect->maxSpeed.y = Slider(gameState, Input, VideoBuffer, &UIPos2, effect->maxSpeed.y, -1, 1);
                    Text(gameState, VideoBuffer, &UIPos2, "maxSpeed.z: ", effect->maxSpeed.z);              
                    effect->maxSpeed.z = Slider(gameState, Input, VideoBuffer, &UIPos2, effect->maxSpeed.z, -1, 1);


                    Text(gameState, VideoBuffer, &UIPos2, "minLifetime: ", effect->minLifetime);
                    effect->minLifetime = Slider(gameState, Input, VideoBuffer, &UIPos2, effect->minLifetime, 0, 4);

                    Text(gameState, VideoBuffer, &UIPos2, "maxLifetime: ", effect->maxLifetime);
                    effect->maxLifetime = Slider(gameState, Input, VideoBuffer, &UIPos2, effect->maxLifetime, 0, 4);

                }


                //Text(gameState, VideoBuffer, &UIPos, "Scale: ", entity->t.scale);
                //Text(gameState, VideoBuffer, &UIPos, "Right: ", entity->t.right);
                //Text(gameState, VideoBuffer, &UIPos, "Forward: ", entity->t.forward);
                //Text(gameState, VideoBuffer, &UIPos, "Up: ", entity->t.up);
                //Text(gameState, VideoBuffer, &UIPos, "angularVelocityAxis: ", entity->angularVelocityAxis);
                //Text(gameState, VideoBuffer, &UIPos, "angularVelocity: ", entity->angularVelocity);
                //Text(gameState, VideoBuffer, &UIPos, "velocity: ", entity->velocity);
            }

        }break;
        case EditorState_Profiling:
        {
            Text(gameState, VideoBuffer, &UIPos, " ");
            Text(gameState, VideoBuffer, &UIPos, "Timers");
            Text(gameState, VideoBuffer, &UIPos, "    Update Time MS: ", Delta * 1000);
            //Text(gameState, VideoBuffer, &UIPos, "    Draw Boxes Smooth MS: ", accumulatedDelta * 1000);
            Text(gameState, VideoBuffer, &UIPos, "    FPS: ", FPS);
            //Text(gameState, VideoBuffer, &UIPos, "    Smooth FPS: ", SmoothFPS);
            Text(gameState, VideoBuffer, &UIPos, " ");



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

        }break;
        case EditorState_Sound:
        {
            float2 StringSoundPlacement = float2(200, VideoBuffer->Height - 100);
            RenderStringSoundLine(VideoBuffer, SoundBuffer, Input, &gameState->stringData, StringSoundPlacement, float3(1, 1, 1));
            StringSound(VideoBuffer, SoundBuffer, Input, &gameState->stringData, gameState->MouseIsOverUI || gameState->Grabbing, StringSoundPlacement);

            // Draw sound debug UI
            DrawSoundBuffer(VideoBuffer, SoundBuffer, float3(1.0, 0.5, 0.5));

            char names[4][100] = {
                "Sound: sound/Music_Theme_2_1.wav",
                "Sound: sound/Misc_extraball.wav",
                "Sound: sound/Hit_Wood_Dome.wav",
                "Sound: sound/Extra_Life_Blob.wav"
            };
            float2 ChannelsMenuPos = float2(180, 10);
            for (int i = 0; i < ArrayCount(gameState->soundChannels); i++)
            {
                SoundChannel* channel = &gameState->soundChannels[i];
                Text(gameState, VideoBuffer, &ChannelsMenuPos, "Channel ", i);
                //Text(gameState, VideoBuffer, &ChannelsMenuPos, names[SoundIndex(gameState, channel->sound)]);
                //Text(gameState, VideoBuffer, &ChannelsMenuPos, "Volume: ", channel->volume);
                //Text(gameState, VideoBuffer, &ChannelsMenuPos, "Speed: ", channel->speed);
                //Text(gameState, VideoBuffer, &ChannelsMenuPos, "Loop: ", channel->loop);

                channel->speed = Slider(gameState, Input, VideoBuffer, &ChannelsMenuPos, channel->speed, 0, 2);
                Text(gameState, VideoBuffer, ChannelsMenuPos + float2(0, -26), "Speed: ");
                Text(gameState, VideoBuffer, ChannelsMenuPos + float2(200, -26), "Volume: ");
                channel->volume = Slider(gameState, Input, VideoBuffer, ChannelsMenuPos + float2(200, -24), channel->volume, 0, 2);
                DrawSound(VideoBuffer, channel->sound, &ChannelsMenuPos, float2(550, 50), channel->currentSample);
                //Text(gameState, VideoBuffer, &ChannelsMenuPos, "Speed: ", channel->speed);
                //Text(gameState, VideoBuffer, &ChannelsMenuPos, "");
            }


            float2 SoundMenuPos = float2(VideoBuffer->Width - 200, 20);
            char PlayText[100] = "Play in channel ";
            Append(PlayText, gameState->ChosenChannel);
            if (Button(gameState, Input, VideoBuffer, &SoundMenuPos, PlayText))
            {
                PlaySound(gameState, gameState->ChosenChannel, &gameState->Sounds[gameState->ChosenSound], 1.0f, gameState->ChosenSoundLoop, 1.0f);
            }
            char StopText[100] = "Stop channel ";
            Append(StopText, gameState->ChosenChannel);
            if (Button(gameState, Input, VideoBuffer, &SoundMenuPos, StopText))
            {
                StopSound(gameState, gameState->ChosenChannel);
            }
            if (Button(gameState, Input, VideoBuffer, &SoundMenuPos, "Stop All"))
            {
                for (int i = 0; i < ArrayCount(gameState->soundChannels); i++)
                {
                    StopSound(gameState, i);
                }
            }
            Text(gameState, VideoBuffer, &SoundMenuPos, names[gameState->ChosenSound]);
            gameState->ChosenSound = Slider(gameState, Input, VideoBuffer, &SoundMenuPos, gameState->ChosenSound, 0, ArrayCount(gameState->soundChannels) - 1);
            Text(gameState, VideoBuffer, &SoundMenuPos, "Channel: ", (int)gameState->ChosenChannel);
            gameState->ChosenChannel = Slider(gameState, Input, VideoBuffer, &SoundMenuPos, gameState->ChosenChannel, 0, ArrayCount(gameState->soundChannels) - 1);
            gameState->ChosenSoundLoop = Toggle(gameState, Input, VideoBuffer, &SoundMenuPos, gameState->ChosenSoundLoop, "Loop");

            Text(gameState, VideoBuffer, &SoundMenuPos, "Master Volume: ", gameState->MasterVolume);
            gameState->MasterVolume = Slider(gameState, Input, VideoBuffer, &SoundMenuPos, gameState->MasterVolume, 0.0f, 2.0f);


            float2 StringUIPos = float2(20, VideoBuffer->Height - 220);
            Text(gameState, VideoBuffer, &StringUIPos, "String synth");
            Text(gameState, VideoBuffer, &StringUIPos, "    Left click to pull string.");
            Text(gameState, VideoBuffer, &StringUIPos, " ");
            Text(gameState, VideoBuffer, &StringUIPos, "Speed: ", gameState->stringData.speed);
            gameState->stringData.speed = Slider(gameState, Input, VideoBuffer, &StringUIPos, gameState->stringData.speed, 0.01f, 0.5f);

            Text(gameState, VideoBuffer, &StringUIPos, "Damping: ", gameState->stringData.damping);
            gameState->stringData.damping = Slider(gameState, Input, VideoBuffer, &StringUIPos, gameState->stringData.damping, 0.0f, 0.005f);

            Text(gameState, VideoBuffer, &StringUIPos, "Timescale: ", gameState->stringData.timescale);
            gameState->stringData.timescale = Slider(gameState, Input, VideoBuffer, &StringUIPos, gameState->stringData.timescale, 0.0f, 1.0f);

            Text(gameState, VideoBuffer, &StringUIPos, "Segments: ", gameState->stringData.Segments);
            gameState->stringData.Segments = Slider(gameState, Input, VideoBuffer, &StringUIPos, gameState->stringData.Segments, 4.0f, 200.0f);

            float2 FFTPlacement = float2(0, VideoBuffer->Height);
            RenderFFT(VideoBuffer, SoundBuffer, Input, &gameState->stringData, FFTPlacement, float3(0.5, 0.5, 1.0));

            gameState->stringData.FTScale = 10;



        }break;
        }



    }

    //DrawMemory(VideoBuffer, (uint8*)Memory->TransientStorage, VideoBuffer->Width* VideoBuffer->Height);

}