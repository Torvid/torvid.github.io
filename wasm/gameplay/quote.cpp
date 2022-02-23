//#pragma optimize( "g", on )

#pragma once

#include "quote.h"
#include "math.cpp"
#include "random.cpp"
#include "string.cpp"
#include "drawing.cpp"
#include "physics.cpp"
#include "sound.cpp"
#include "ui.cpp"
#include "editor.cpp"

#include "game.cpp"

char getNthBit(char c, char n)
{
    unsigned char tmp = 1 << n;
    return (c & tmp) >> n;
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

//int dllTest()
//{
//    return 1337;
//}


//(thread_context* Thread, game_memory* Memory, game_input* Input, game_offscreen_buffer* VideoBuffer, game_sound_output_buffer* SoundBuffer)
#if WASM
#else
__declspec(dllexport)
#endif
extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    //int wat = exeTest();

    GlobalGameMemory = Memory;

    int64 Global_Start = Memory->PlatformTime();
    Assert(sizeof(EngineState) <= Memory->TransientStorageSize);
    int64 time_start = Memory->PlatformTime();
    //Assert((&Input->Controllers[0].Terminator - &Input->Controllers[0].Buttons[0]) == (ArrayCount(Input->Controllers[0].Buttons) - 1));
    
    InitGlobalFunctions(Memory);

    EngineState* engineState = (EngineState*)Memory->TransientStorage;
    EngineSaveState* engineSaveState = (EngineSaveState*)Memory->PermanentStorage;

    BallGameState* gameState = (BallGameState*)((uint8*)Memory->TransientStorage + sizeof(EngineState));
    BallGameSaveState* gameSaveState = (BallGameSaveState*)((uint8*)Memory->PermanentStorage + sizeof(EngineSaveState));
    engineState->profilingData.Memory = Memory;
    //ProfilerEndSample(&engineState->profilingData, "Outside");
    engineState->profilingData.index = 0;
    engineState->profilingData.currentDepth = 0;

    engineState->profilingData.frameNumber++;
    engineState->profilingData.frameNumber %= TimeSampleCount;

    ProfilerBeingSample(&engineState->profilingData);

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
        engineSaveState->Initialized = false;
        for (int i = 0; i < Memory->PermanentStorageSize; i++)
        {
            ((char*)Memory->PermanentStorage)[i] = 0;
        }
    }
    engineState->profilingData.Memory = Memory;
    engineState->profilingData.threading = engineSaveState->ThreadedRendering;

    if (!engineSaveState->Initialized)
    {
        engineSaveState->Initialized = true;
        engineSaveState->Camera = transform(float3(0, -5, 0), float3(0, 1, 0), float3(0, 0, 1), float3(1, 1, 1));
        engineState->Camera = engineSaveState->Camera;
        engineSaveState->ScreenThreads = 8;
        engineSaveState->ThreadedRendering = false;
        engineSaveState->GameViewEnabled = true;
        engineSaveState->ProfilingEnabled = false;
    }

    float SphereRadius = 0.5;

    if (!Memory->Initialized) // Happens on startup and when the user presses space
    {
        Memory->Initialized = true;

        engineState->PlatformReadImageIntoMemory = Memory->PlatformReadImageIntoMemory;
        engineState->PlatformReadSoundIntoMemory = Memory->PlatformReadSoundIntoMemory;
        engineState->PlatformPrint = Memory->PlatformPrint;

        engineState->GameTime = 0;
        InitializeArena(&engineState->textureArena, Megabytes(12), (uint8*)Memory->TransientStorage + sizeof(EngineState) + sizeof(BallGameState) + Kilobytes(100));
        InitializeArena(&engineState->soundArena,   Megabytes(12), (uint8*)Memory->TransientStorage + sizeof(EngineState) + sizeof(BallGameState) + Kilobytes(100) + Megabytes(12));
        uint64 size = (uint64)(sizeof(EngineState) + sizeof(BallGameState) + Kilobytes(100) + Megabytes(12) + Megabytes(12));
        Assert(size <= Memory->TransientStorageSize);
        
        engineState->SkyFaces[0] = LoadImage(engineState, "textures/Sky_C_Left.tga", 512, 512);
        engineState->SkyFaces[1] = LoadImage(engineState, "textures/Sky_C_Right.tga", 512, 512);
        engineState->SkyFaces[2] = LoadImage(engineState, "textures/Sky_C_Front.tga", 512, 512);
        engineState->SkyFaces[3] = LoadImage(engineState, "textures/Sky_C_Back.tga", 512, 512);
        engineState->SkyFaces[4] = LoadImage(engineState, "textures/Sky_C_Top.tga", 512, 512);
        engineState->SkyFaces[5] = LoadImage(engineState, "textures/Sky_C_Down.tga", 512, 512);

        engineState->fontSpritesheet = LoadImage(engineState, "font.tga", 192, 52);
        engineState->UseFiltering = true;

        engineState->MoveGizmoAxisID = -1;
        engineState->PickedSliderID = -1;

        LoadLevel(engineState, engineState->CurrentLevel);

        engineState->stringData.Pos = (float*)&(engineState->stringData.Pos1);
        engineState->stringData.Vel = (float*)&(engineState->stringData.Vel1);
        engineState->stringData.OldPos = (float*)&(engineState->stringData.Pos2);
        engineState->stringData.OldVel = (float*)&(engineState->stringData.Vel2);

        engineState->stringData.speed = 0.4f;
        engineState->stringData.damping = 0.001f;
        engineState->stringData.timescale = 1.0f;
        engineState->stringData.Segments = 60;
        engineState->stringData.FTScale = 0.5f;
        engineState->stringData.PixelsPerSample = 10;
        engineState->ChosenSoundLoop = true;
        engineState->MasterVolume = 1.0f;
        for (int i = 0; i < ArrayCount(engineState->soundChannels); i++)
        {
            StopSound(engineState, i);
        }
        float2 s = float2(VideoBuffer->Width, VideoBuffer->Height);
        engineState->a = float2(0.1f, 0.1f)*s;
        engineState->b = float2(0.25f, 0.15f)*s;
        engineState->c = float2(0.12f, 0.22f)*s;
        engineState->d = float2(0.21f, 0.21f)*s;
        engineState->PhysicsTimeAccumulator = 0;
        GameLoad(engineState, gameState, gameSaveState);
    }
    engineState->SliderID = 0;
    engineState->GameTime += Input->DeltaTime;

    if (!Input->Ctrl && !Input->Shift && Input->GDown)
    {
        engineSaveState->GameViewEnabled = !engineSaveState->GameViewEnabled;
        for (int i = 0; i < ArrayCount(engineState->SelectedEntities); i++)
        {
            engineState->SelectedEntities[i] = 0;
        }
    }
    if (!Input->Ctrl && !Input->Shift && Input->PDown)
    {
        engineSaveState->ProfilingEnabled = !engineSaveState->ProfilingEnabled;
    }

    //float2 size = float2(1, 1);
    //float2 mousePos = float2(Input->MouseX, Input->MouseY) / size;
    //if (distance(mousePos, engineState->a) < 10 && Input->MouseLeft) { engineState->a = mousePos; }
    //if (distance(mousePos, engineState->b) < 10 && Input->MouseLeft) { engineState->b = mousePos; }
    //if (distance(mousePos, engineState->c) < 10 && Input->MouseLeft) { engineState->c = mousePos; }
    //if (distance(mousePos, engineState->d) < 10 && Input->MouseLeft) { engineState->d = mousePos; }
    //float2 a = engineState->a;
    //float2 b = engineState->b;
    //float2 c = engineState->c;
    //float2 d = engineState->d;
    //
    //
    //for (int y = 0; y < VideoBuffer->Height; y++)
    //{
    //    for (int x = 0; x < VideoBuffer->Width; x++)
    //    {
    //        //float2 pos = float2(x, y);
    //        //float2 d1 = pos;
    //        //float3 res = RasterizeSquare(pos, a, b, c, d);
    //        //
    //        ////float2 flippedvector = float2((a - c).y, -(a - c).x);
    //        ////float o = dot(flippedvector, (b - a));
    //        ////float u = dot(pos - a, flippedvector) / o + 1;
    //        //
    //        //float2 uflippedvector = float2((a - c).y, -(a - c).x);
    //        //float uo = dot(uflippedvector, (b - a));
    //        //float u = dot(pos - a, uflippedvector) / uo;
    //        //float2 vflippedvector = float2((a - b).y, -(a - b).x);
    //        //float vo = dot(vflippedvector, (c - a));
    //        //float v = dot(pos - a, vflippedvector) / vo;
    //        //
    //        //bool hit = max(max((u + v), 1 - u), 1 - v) < 1;
    //        //
    //        //float q = clamp01(hit) * 0.5;
    //        //float3 color = float3(res.x, res.y, 0);
    //
    //        ((uint32*)VideoBuffer->Memory)[y * VideoBuffer->Width + x] = ToColor(float3(0, 0, 0));
    //    }
    //}
    //
    //int resX = min(distance((a + c) * 0.5, (b + d) * 0.5), 500);
    //int resY = min(distance((a + b) * 0.5, (c + d) * 0.5), 500);
    //for (int x = 0; x < resX; x++)
    //{
    //    for (int y = 0; y < resY; y++)
    //    {
    //        float tx = ((float)x / (float)resX);
    //        float ty = ((float)y / (float)resY);
    //        float2 pos = lerp(lerp(a, b, tx), lerp(c, d, tx), ty);
    //        *GetPixel(VideoBuffer, pos.x, pos.y) = ToColor(float3(tx, ty, 0));
    //    }
    //}
    //
    //DrawPoint(VideoBuffer, a);
    //DrawPoint(VideoBuffer, b);
    //DrawPoint(VideoBuffer, c);
    //DrawPoint(VideoBuffer, d);
    //
    //font_draw(engineState->fontSpritesheet, VideoBuffer, a.x, a.y, "a");
    //font_draw(engineState->fontSpritesheet, VideoBuffer, b.x, b.y, "b");
    //font_draw(engineState->fontSpritesheet, VideoBuffer, c.x, c.y, "c");
    //font_draw(engineState->fontSpritesheet, VideoBuffer, d.x, d.y, "d");
    //return;


    // Started playing
    if (engineState->LasteditorState != engineState->editorState)
    {
        if (engineState->editorState == EditorState_Playing)
        {
            GameStart(engineState, gameState, gameSaveState);
            StopAllSounds(engineState);
        }
        else if(engineState->editorState == EditorState_Editing)
        {
            LoadLevel(engineState, 0);
            ExitToMainMenu(engineState, gameState);
            StopAllSounds(engineState);
        }
    }

    if (engineState->editorState != EditorState_Editing && gameState->menuState == MenuState_Playing)
    {
        GameUpdate(engineState, VideoBuffer, Input, gameState, gameSaveState);
        //engineState->Camera = transform(float3(0, 0, 0), float3(0, 1, 0), float3(0, 0, 1), float3(1, 1, 1));
    }
    else
    {
        if (Input->MouseRight) //  && !engineState->Grabbing
        {
            UpdateEditorCamera(engineState, engineSaveState, Input);
        }
        engineState->Camera = engineSaveState->Camera;
    }

    engineState->NextRenderCommand = 0;
    QueueDrawSkyBox(engineState, engineState->SkyFaces);
    for (int i = 0; i < ArrayCount(engineState->Entities); i++)
    {
        Entity* e = &engineState->Entities[i];
        if (!Valid(e))
            continue;
        if (e->t.position.z < -5)
            e->t.position.z = -5;
    }

    for (int i = 0; i < ArrayCount(engineState->Entities); i++)
    {
        Entity* e = &engineState->Entities[i];
        if (!Valid(e))
            continue;

        //if (!e->mesh)
        //    continue;

        bool selected = false;
        for (int i = 0; i < ArrayCount(engineState->SelectedEntities); i++)
        {
            Entity* selectedEntity = engineState->SelectedEntities[i];
            if (e == selectedEntity)
            {
                selected = true;
                break;
            }
        }

        bool Hovered = e == engineState->HoveredEntity;
        if(e->type == EntityType_Sphere && e->mesh)
            QueueDrawSphere(engineState, e->t, e->texture1, e->texture2, e->ShaderIndex, selected, Hovered);
        if (e->type == EntityType_Box && e->mesh)
            QueueDrawBox(engineState, e->t, e->texture1, e->texture2, e->ShaderIndex, selected, Hovered);
        if (e->type == EntityType_Effect)
            QueueDrawEffect(engineState, e->t, e->texture1, e->texture2, e->ShaderIndex, e->effect, e->Color);

        if (!engineSaveState->GameViewEnabled)
        {
            transform t = e->t;
            t.scale = float3(1,1,1)*0.5f;
            if (e->type == EntityType_Spawn)
                QueueDrawSphere(engineState, t, e->texture1, e->texture2, e->ShaderIndex, selected, Hovered);
            if (e->type == EntityType_Win)
                QueueDrawSphere(engineState, t, e->texture1, e->texture2, e->ShaderIndex, selected, Hovered);
            if (e->type == EntityType_Checkpoint)
                QueueDrawSphere(engineState, t, e->texture1, e->texture2, e->ShaderIndex, selected, Hovered);
            if (e->type == EntityType_Health)
                QueueDrawSphere(engineState, t, e->texture1, e->texture2, e->ShaderIndex, selected, Hovered);
            if (e->type == EntityType_Time)
                QueueDrawSphere(engineState, t, e->texture1, e->texture2, e->ShaderIndex, selected, Hovered);
        }
    }


    ProfilerBeingSample(&engineState->profilingData);
    // Draws the queued up commands to the screen.
    DrawScene(Memory, engineState, engineSaveState, VideoBuffer);

    ProfilerEndSample(&engineState->profilingData, "DrawScene");



    //float height = 200;
    //for (int i = 0; i < 1000; i++)
    //{
    //    float time = mod(engineState->GameTime, 4);
    //    float x = ((float)i - 250) / 25.0f;
    //    //DrawPoint(VideoBuffer, float2(i, pow(((float)i - 250) / 25.0f, time) * 25.0f + height));
    //    //DrawPoint(VideoBuffer, float2(i, game_sqrtf(x) * 25.0f + height));
    //    DrawPoint(VideoBuffer, float2(i, game_sqrtf(x) * 25.0f + height));
    //    //DrawPoint(VideoBuffer, float2(i, ((float)i - 250) + height));
    //}
    //DrawLine(VideoBuffer, float2(0, height), float2(1000, height));




    ProfilerBeingSample(&engineState->profilingData);
    // Update sound system
    ClearSoundBuffer(SoundBuffer);
    for (int i = 0; i < ArrayCount(engineState->soundChannels); i++)
    {
        SoundChannel* channel = &engineState->soundChannels[i];
        PlaySoundUpdate(SoundBuffer, channel);
    }

    SetSoundBufferVolume(SoundBuffer, engineState->MasterVolume);
    ProfilerEndSample(&engineState->profilingData, "Sound");

    ProfilerBeingSample(&engineState->profilingData);
    if (gameState->menuState == MenuState_Playing)
    {
        // Substepping such that physics runs at 60 fps
        engineState->PhysicsTimeAccumulator += Input->DeltaTime;

        while (engineState->PhysicsTimeAccumulator > 0)
        {
            engineState->PhysicsTimeAccumulator -= 1.0f / 60.0f;
            UpdatePhysics(Memory, engineState, engineSaveState, VideoBuffer);
        }
    }
    ProfilerEndSample(&engineState->profilingData, "Physics");

    ProfilerBeingSample(&engineState->profilingData);

    if (engineState->editorState == EditorState_Playing)
    {
        GameUpdateUI(engineState, VideoBuffer, Input, gameState, gameSaveState, engineSaveState);
    }

    int64 time_end = Memory->PlatformTime();

    float2 UIPos = float2(4, 0);

    float FPS = (float)(1.0f / Input->DeltaTime);
    float Delta = (float)(time_end - time_start) / 10000000;
    float accumulatedDelta = GetSmoothedValue(engineState->deltas, &engineState->deltasIndex, Delta, 30);
    float SmoothFPS = GetSmoothedValue(engineState->FPS, &engineState->FPSIndex, FPS, 30);

    Text(engineState, VideoBuffer, &UIPos, "Controls");
    Text(engineState, VideoBuffer, &UIPos, "  G to toggle editor.");
    Text(engineState, VideoBuffer, &UIPos, "  P to toggle profiling.");

    if (engineState->editorState != engineState->LasteditorState)
    {
        engineState->LasteditorState = engineState->editorState;
        for (int i = 0; i < ArrayCount(engineState->SelectedEntities); i++)
        {
            engineState->SelectedEntities[i] = 0;
        }
    }

    if (!engineSaveState->GameViewEnabled)
    {
        //Text(engineState, VideoBuffer, &UIPos, "Controls");
        Text(engineState, VideoBuffer, &UIPos, "  Space to reset program. Ctrl + Space to hard-reset program. Right mouse + WASD to move and look.");
        if (Toggle(engineState, Input, VideoBuffer, &UIPos, engineState->editorState == EditorState_Playing, "Playing"))
            engineState->editorState = EditorState_Playing;

        if(Toggle(engineState, Input, VideoBuffer, &UIPos, engineState->editorState == EditorState_Editing, "Editing"))
            engineState->editorState = EditorState_Editing;

        if (Toggle(engineState, Input, VideoBuffer, &UIPos, engineState->editorState == EditorState_Settings, "Settings"))
            engineState->editorState = EditorState_Settings;

        if (Toggle(engineState, Input, VideoBuffer, &UIPos, engineState->editorState == EditorState_Sound, "Sound"))
            engineState->editorState = EditorState_Sound;

        switch (engineState->editorState)
        {
        case EditorState_Editing:
        {
            UpdateEditor(Memory, engineState, engineSaveState, Input, VideoBuffer, &UIPos);

        }break;
        case EditorState_Settings:
        {
        
        }break;
        case EditorState_Sound:
        {
            float2 StringSoundPlacement = float2(200, VideoBuffer->Height - 100);
            RenderStringSoundLine(VideoBuffer, SoundBuffer, Input, &engineState->stringData, StringSoundPlacement, float3(1, 1, 1));
            //StringSound(VideoBuffer, SoundBuffer, Input, &engineState->stringData, engineState->MouseIsOverUI || engineState->Grabbing, StringSoundPlacement);
            StringSound(VideoBuffer, SoundBuffer, Input, &engineState->stringData, false, StringSoundPlacement);

            // Draw sound debug UI
            DrawSoundBuffer(VideoBuffer, SoundBuffer, float3(1.0, 0.5, 0.5));

            //char names[4][100] = {
            //    "Sound: sound/Music_Theme_2_1.wav",
            //    "Sound: sound/Misc_extraball.wav",
            //    "Sound: sound/Hit_Wood_Dome.wav",
            //    "Sound: sound/Extra_Life_Blob.wav"
            //};
            float2 ChannelsMenuPos = float2(180, 10);
            for (int i = 0; i < ArrayCount(engineState->soundChannels); i++)
            {
                SoundChannel* channel = &engineState->soundChannels[i];
                Text(engineState, VideoBuffer, &ChannelsMenuPos, "Channel ", i);
                //Text(engineState, VideoBuffer, &ChannelsMenuPos, names[SoundIndex(engineState, channel->sound)]);
                //Text(engineState, VideoBuffer, &ChannelsMenuPos, "Volume: ", channel->volume);
                //Text(engineState, VideoBuffer, &ChannelsMenuPos, "Speed: ", channel->speed);
                //Text(engineState, VideoBuffer, &ChannelsMenuPos, "Loop: ", channel->loop);

                channel->speed = Slider(engineState, Input, VideoBuffer, &ChannelsMenuPos, channel->speed, 0, 20);
                Text(engineState, VideoBuffer, ChannelsMenuPos + float2(0, -26), "Speed: ");
                Text(engineState, VideoBuffer, ChannelsMenuPos + float2(200, -26), "Volume: ");
                channel->volume = Slider(engineState, Input, VideoBuffer, ChannelsMenuPos + float2(200, -24), channel->volume, 0, 2);
                DrawSound(VideoBuffer, channel->sound, &ChannelsMenuPos, float2(550, 50), channel->currentSample);
                //Text(engineState, VideoBuffer, &ChannelsMenuPos, "Speed: ", channel->speed);
                //Text(engineState, VideoBuffer, &ChannelsMenuPos, "");
            }


            float2 SoundMenuPos = float2(VideoBuffer->Width - 200, 20);
            char PlayText[100] = "Play in channel ";
            Append(PlayText, engineState->ChosenChannel);
            if (Button(engineState, Input, VideoBuffer, &SoundMenuPos, PlayText))
            {
                PlaySound(engineState, engineState->ChosenChannel, &engineState->Sounds[engineState->ChosenSound], 1.0f, engineState->ChosenSoundLoop, 1.0f);
            }
            char StopText[100] = "Stop channel ";
            Append(StopText, engineState->ChosenChannel);
            if (Button(engineState, Input, VideoBuffer, &SoundMenuPos, StopText))
            {
                StopSound(engineState, engineState->ChosenChannel);
            }
            if (Button(engineState, Input, VideoBuffer, &SoundMenuPos, "Stop All"))
            {
                StopAllSounds(engineState);
            }
            Text(engineState, VideoBuffer, &SoundMenuPos, engineState->SoundNames[engineState->ChosenSound]);
            engineState->ChosenSound = Slider(engineState, Input, VideoBuffer, &SoundMenuPos, engineState->ChosenSound, 0, engineState->CurrentSound - 1);
            Text(engineState, VideoBuffer, &SoundMenuPos, "Channel: ", (int)engineState->ChosenChannel);
            engineState->ChosenChannel = Slider(engineState, Input, VideoBuffer, &SoundMenuPos, engineState->ChosenChannel, 0, ArrayCount(engineState->soundChannels) - 1);
            engineState->ChosenSoundLoop = Toggle(engineState, Input, VideoBuffer, &SoundMenuPos, engineState->ChosenSoundLoop, "Loop");

            Text(engineState, VideoBuffer, &SoundMenuPos, "Master Volume: ", engineState->MasterVolume);
            engineState->MasterVolume = Slider(engineState, Input, VideoBuffer, &SoundMenuPos, engineState->MasterVolume, 0.0f, 2.0f);


            float2 StringUIPos = float2(20, VideoBuffer->Height - 220);
            Text(engineState, VideoBuffer, &StringUIPos, "String synth");
            Text(engineState, VideoBuffer, &StringUIPos, "    Left click to pull string.");
            Text(engineState, VideoBuffer, &StringUIPos, " ");
            Text(engineState, VideoBuffer, &StringUIPos, "Speed: ", engineState->stringData.speed);
            engineState->stringData.speed = Slider(engineState, Input, VideoBuffer, &StringUIPos, engineState->stringData.speed, 0.01f, 0.5f);

            Text(engineState, VideoBuffer, &StringUIPos, "Damping: ", engineState->stringData.damping);
            engineState->stringData.damping = Slider(engineState, Input, VideoBuffer, &StringUIPos, engineState->stringData.damping, 0.0f, 0.005f);

            Text(engineState, VideoBuffer, &StringUIPos, "Timescale: ", engineState->stringData.timescale);
            engineState->stringData.timescale = Slider(engineState, Input, VideoBuffer, &StringUIPos, engineState->stringData.timescale, 0.0f, 1.0f);

            Text(engineState, VideoBuffer, &StringUIPos, "Segments: ", engineState->stringData.Segments);
            engineState->stringData.Segments = Slider(engineState, Input, VideoBuffer, &StringUIPos, engineState->stringData.Segments, 4.0f, 200.0f);

            float2 FFTPlacement = float2(0, VideoBuffer->Height);
            RenderFFT(VideoBuffer, SoundBuffer, Input, &engineState->stringData, FFTPlacement, float3(0.5, 0.5, 1.0));

            engineState->stringData.FTScale = 10;

        }break;
        }
    }

    ProfilerEndSample(&engineState->profilingData, "UI");

    ProfilerEndSample(&engineState->profilingData, "Frame");
    
    if (engineSaveState->ProfilingEnabled)
    {
        Text(engineState, VideoBuffer, &UIPos, " ");
        Text(engineState, VideoBuffer, &UIPos, "Timers");
        Text(engineState, VideoBuffer, &UIPos, "    Update Time MS: ", Delta * 1000);
        //Text(engineState, VideoBuffer, &UIPos, "    Draw Boxes Smooth MS: ", accumulatedDelta * 1000);
        Text(engineState, VideoBuffer, &UIPos, "    FPS: ", FPS);
        //Text(engineState, VideoBuffer, &UIPos, "    Smooth FPS: ", SmoothFPS);
        Text(engineState, VideoBuffer, &UIPos, " ");


        engineState->renderTest1 = Toggle(engineState, Input, VideoBuffer, &UIPos, engineState->renderTest1, "renderTest1");
        engineState->renderTest2 = Toggle(engineState, Input, VideoBuffer, &UIPos, engineState->renderTest2, "renderTest2");

        engineSaveState->ThreadedRendering = Toggle(engineState, Input, VideoBuffer, &UIPos, engineSaveState->ThreadedRendering, "Threaded rendering");
        if (engineSaveState->ThreadedRendering)
        {
            engineSaveState->Visualize_ThreadedRendering = Toggle(engineState, Input, VideoBuffer, &UIPos, engineSaveState->Visualize_ThreadedRendering, "Visualize", float2(12, 0));
            //engineSaveState->ThreadedRendering_Dynamic = Toggle(engineState, Input, VideoBuffer, &UIPos, engineSaveState->ThreadedRendering_Dynamic, "Dynamic render regions", float2(12, 0));

            char text[100];
            for (int i = 0; i < 100; i++)
            {
                text[i] = 0;
            }
            Append(text, "Number of threads: ");
            Append(text, engineSaveState->ScreenThreads);
            engineSaveState->ScreenThreads = 8;
            if (Button(engineState, Input, VideoBuffer, &UIPos, text))
            {
                engineSaveState->ScreenThreads *= 2;
                if (engineSaveState->ScreenThreads > 32)
                {
                    engineSaveState->ScreenThreads = 4;
                }
            }
        }

        // Debugging
        DrawRectangle(VideoBuffer, float2(0, VideoBuffer->Height - 200), float2(200, 200), float3(0.0, 0.0, 0.0), 0.5);

        int64 Global_End = Memory->PlatformTime();
        float TotalTimeDelta = (float)(Global_End - Global_Start) / 10000000;
        float SmoothedTime = GetSmoothedValue(engineState->deltas2, &engineState->deltas2Index, TotalTimeDelta, 100);

        float HeightMultiplier = 5000;
        float LastHeight = 0;
        for (int i = 0; i < 100; i++)
        {
            int h = (i + engineState->deltas2Index) % 100;
            float height = VideoBuffer->Height - engineState->deltas2[h] * HeightMultiplier;
            DrawLine(VideoBuffer, float2(i * 2 - 2, LastHeight), float2(i * 2, height));
            LastHeight = height;
        }

        float fps30dt = (1.0f / 30.0f);
        float fps60dt = (1.0f / 60.0f);
        float fps30height = VideoBuffer->Height - fps30dt * HeightMultiplier;
        float fps60height = VideoBuffer->Height - fps60dt * HeightMultiplier;
        float fpscurrentheight = VideoBuffer->Height - (SmoothedTime) * HeightMultiplier;
        DrawLine(VideoBuffer, float2(0, fps30height), float2(200, fps30height));
        DrawLine(VideoBuffer, float2(0, fps60height), float2(200, fps60height));
        font_draw(engineState->fontSpritesheet, VideoBuffer, 240, fps30height, "33.33ms");
        font_draw(engineState->fontSpritesheet, VideoBuffer, 240, fps60height, "16.66ms");
        char text[100] = {};
        Append(text, TotalTimeDelta*1000);
        Append(text, "ms");
        font_draw(engineState->fontSpritesheet, VideoBuffer, 200, fpscurrentheight, text);


        int64 lowest = 0;
        int64 highest = 0;
        for (int k = 0; k < TimeSampleCount; k++)
        {
            lowest += engineState->profilingData.EnterTime[0][k];
            highest += engineState->profilingData.ExitTime[engineState->profilingData.index - 1][k];
        }
        lowest /= TimeSampleCount;
        highest /= TimeSampleCount;
        //int64 lowest = engineState->profilingData.EnterTime[0][0];
        //int64 highest = engineState->profilingData.ExitTime[engineState->profilingData.index - 1][0];

        int64 length = highest - lowest;
        float sampleCount = 400000;
        float milliseconds = (float)length / 10000.0f;
        float StepSize = 10000.0f / (float)length;
        float pos = 0;
        //for (int i = 0; i < milliseconds; i++)
        //{
        //    pos += StepSize;
        //    float t = pos * VideoBuffer->Width;
        //    DrawLine(VideoBuffer, float2(t, VideoBuffer->Height), float2(t, VideoBuffer->Height - 100));
        //}
        for (int i = 0; i < engineState->profilingData.index; i++)
        {
            int64 start = 0;
            int64 end = 0;
            for (int k = 0; k < TimeSampleCount; k++)
            {
                start += engineState->profilingData.EnterTime[i][k] - lowest;
                end += engineState->profilingData.ExitTime[i][k] - lowest;
            }
            start /= TimeSampleCount;
            end /= TimeSampleCount;
            //int64 start = engineState->profilingData.EnterTime[i][0] - lowest;
            //int64 end = engineState->profilingData.ExitTime[i][0] - lowest;
            char* name = engineState->profilingData.functionNames[i];

            //float startNormalized = (float)start / (float)length;
            //float endNormalized = (float)end / (float)length;
            float startNormalized = (float)start / sampleCount;
            float endNormalized = (float)end / sampleCount;
            startNormalized *= VideoBuffer->Width;
            endNormalized *= VideoBuffer->Width;

            float height = VideoBuffer->Height - (17 * engineState->profilingData.Depth[i]);
            DrawRectangle(VideoBuffer, float2(round(startNormalized), height) + float2(250, 0), float2(round(endNormalized - startNormalized) - 1, 16), float3(0.5, 0.5, 0.5));
            char time[100] = {};
            Append(time, name);
            Append(time, ": ");
            Append(time, (int)((end - start) / 10));
            font_draw(engineState->fontSpritesheet, VideoBuffer, startNormalized + 250, height, time);
        }
    }

    //ProfilerBeingSample(&engineState->profilingData);
    
}