#pragma once
//#include "quote.h"
//
//
//enum MenuState
//{
//    MenuState_MainMenu,
//    MenuState_Settings,
//    MenuState_Playing,
//    MenuState_Paused,
//    MenuState_WinScreen,
//    MenuState_LoseScreen, 
//    MenuState_ChapterSelect,
//    MenuState_Max,
//};
//
//char MenuStateNames[MenuState_Max][100] = {
//    "MainMenu",
//    "Settings",
//    "Playing",
//    "Paused",
//    "WinScreen",
//    "LoseScreen",
//    "ChapterSelect"
//};
//
//struct BallGameState
//{
//    Entity* player;
//    Entity* win;
//
//    Entity* SpawnPoint;
//    Entity* Checkpoints[10];
//    Entity* CheckpointEffects[10];
//    Entity* CurrentCheckpoint;
//    Entity* LastCurrentCheckpoint;
//    
//    Image* ballTexture;
//    Image* minecraft;
//    Image* extraBallTexture;
//
//    Image* soapTexture;
//    Image* soapParticleTexture;
//    Image* particleTimeTexture;
//    Image* particleFireTexture;
//
//    int health = 0;
//    float score = 0;
//    MenuState menuState;
//    int CurrentLevel;
//
//    int HealthPickupSphere;
//    Entity* HealthPickupSpheres[10];
//    Entity* HealthPickupSphereEffect[10];
//    Entity* PickupHealthEffectEntity;
//
//    int TimePickupSphere;
//    Entity* TimePickupEditorSphere[10];
//    Entity* TimePickupSpheres[10];
//    Entity* TimePickupSphereEffect[10];
//    Entity* TimePickupSpheresOrbiters[10][10];
//    Entity* TimePickupBurstEffect;
//
//    SurfaceType playerSurfaceType;
//    SurfaceType lastPlayerSurfaceType;
//
//    Sound* HitSoundMetal;
//    Sound* HitSoundStone;
//    Sound* HitSoundWood;
//
//    Sound* RollSoundMetal;
//    Sound* RollSoundStone;
//    Sound* RollSoundWood;
//
//    uint8 output[2479*2479];
//
//};
//
//
//// Persits between restarts
//struct BallGameSaveState
//{
//    int UnlockedLevels;
//    int Level1HighScore = 0;
//    int Level2HighScore = 0;
//    int Level3HighScore = 0;
//};
//
//Entity* GetFreeEntitySlot(EngineState* engineState)
//{
//    for (int j = 0; j < ArrayCapacity(engineState->entities); j++)
//    {
//        if (!Valid(engineState->entities[j]))
//            return &engineState->entities[j];
//    }
//
//    // no entities left
//    Assert(false);
//    return 0;
//}
//
//Entity* MakeEntity(EngineState* engineState, float3 pos, bool sphere, bool physics, Image* texture, ShaderType shaderType, float scale = 1.0f)
//{
//    int texture1 = FindImageIndex(engineState, texture);
//    Transform t = transform_pos(pos);
//    t.scale = float3(1, 1, 1) * scale;
//    return MakeEntity(engineState, GetFreeEntitySlot(engineState), t, true, EntityType_Sphere, texture1, 0, shaderType, physics, false);
//}
//
//Entity* MakeEffect(EngineState* engineState, float3 pos, Image* texture, EffectData effectData, bool additive = false)
//{
//    int texture1 = FindImageIndex(engineState, texture);
//    Entity* result = MakeEntity(engineState, GetFreeEntitySlot(engineState), transform_pos(pos), false, EntityType_Effect, texture1, 0, additive ? ShaderType_Texture : ShaderType_Sphere, false, false, 1.0f, effectData);
//    
//    result->effect.startTime = engineState->gameTime;
//    return result;
//}
//
//Entity* SpawnPlayer(EngineState* engineState, BallGameState* gameState)
//{
//    Entity* ent = MakeEntity(engineState, gameState->SpawnPoint->transform.position, true, true, gameState->ballTexture, ShaderType_Sphere);
//    ent->transform.scale = float3(0.4f, 0.4f, 0.4f);
//    ent->mass = 1.0;
//    return ent;
//}
//
//void InitWorld(EngineState* engineState, BallGameState* gameState)
//{
//    int CheckpointCount = 0;
//    for (int j = 0; j < ArrayCapacity(engineState->entities); j++)
//    {
//        if (Valid(engineState->entities[j]) && engineState->entities[j].type == EntityType_Spawn)
//        {
//            gameState->SpawnPoint = &engineState->entities[j];
//        }
//        if (Valid(engineState->entities[j]) && engineState->entities[j].type == EntityType_Checkpoint)
//        {
//            gameState->Checkpoints[CheckpointCount] = &engineState->entities[j];
//            CheckpointCount++;
//        }
//    }
//
//    for (int i = 0; i < ArrayCapacity(gameState->Checkpoints); i++)
//    {
//        Entity* e = gameState->Checkpoints[i];
//        if (!Valid(e))
//            continue;
//        gameState->CheckpointEffects[i] = MakeEffect(engineState, e->transform.position, gameState->particleFireTexture, ctor_EffectData(10, false, 0.2, 1, float3(0, 0, 0.2), float3(0, 0, 0.4), 0.5f, 0.5f, 2.0f, 0.0f, 0.0), true);
//    }
//
//    gameState->CurrentCheckpoint = 0;
//    
//    gameState->PickupHealthEffectEntity = MakeEffect(engineState, float3(0, 0, 0), gameState->soapParticleTexture, ctor_EffectData(6, true, 0.25f, 1, float3(-1, -1, -1) * 0.25, float3(1, 1, 1) * 0.25, 0.1f, 0.5f, 0.2f, 2.0f, 0.0), true);
//    gameState->TimePickupBurstEffect = MakeEffect(engineState, float3(0, 0, 0), gameState->extraBallTexture, ctor_EffectData(6, true, 0.25f, 1, float3(-1, -1, -1) * 0.25, float3(1, 1, 1) * 0.25, 0.1f, 0.2f, 0.5f, 0.0f, 0.0), false);
//    gameState->HealthPickupSphere = 0;
//    gameState->TimePickupSphere = 0;
//
//    for (int i = 0; i < ArrayCapacity(engineState->entities); i++)
//    {
//        Entity* e = &engineState->entities[i];
//
//        if (!Valid(e))
//            continue;
//
//        if (e->type == EntityType_Health)
//        {
//            gameState->HealthPickupSpheres[gameState->HealthPickupSphere] = MakeEntity(engineState, e->transform.position, true, false, gameState->soapTexture, ShaderType_Additive);
//            gameState->HealthPickupSphereEffect[gameState->HealthPickupSphere] = MakeEffect(engineState, e->transform.position, gameState->extraBallTexture, ctor_EffectData(1, false, 0.01, 1, float3(0, 0, 0), float3(0, 0, 0), 2.0f, 2.0f, 1.25f, 1.25f, 0.0));
//            gameState->HealthPickupSphere++;
//        }
//        else if (e->type == EntityType_Time)
//        {
//            gameState->TimePickupEditorSphere[gameState->TimePickupSphere] = e;
//            gameState->TimePickupSphereEffect[gameState->TimePickupSphere] = MakeEffect(engineState, e->transform.position, gameState->extraBallTexture, ctor_EffectData(1, false, 0.01, 1, float3(0, 0, 0), float3(0, 0, 0), 2.0f, 2.0f, 1.25f, 1.25f, 0.0));
//            
//            gameState->TimePickupSpheres[gameState->TimePickupSphere] = MakeEntity(engineState, e->transform.position, true, false, gameState->particleTimeTexture, ShaderType_Additive, 0.5f);
//            for (int j = 0; j < ArrayCapacity(gameState->TimePickupSpheres); j++)
//            {
//                gameState->TimePickupSpheresOrbiters[gameState->TimePickupSphere][j] = MakeEffect(engineState, e->transform.position, gameState->extraBallTexture, ctor_EffectData(1, false, 0.01, 1, float3(0, 0, 0), float3(0, 0, 0), 2.0f, 2.0f, 0.5f, 0.5f, 0.0));
//            }
//            
//            gameState->TimePickupSphere++;
//        }
//    }
//}
//
//void UpdateWorld(EngineState* engineState, BallGameState* gameState)
//{
//    for (int i = 0; i < ArrayCapacity(gameState->HealthPickupSpheres); i++)
//    {
//        Entity* e = gameState->HealthPickupSpheres[i];
//
//        if (!Valid(e))
//            continue;
//
//        e->transform.scale = 0.5f + sin(engineState->gameTime* 2.0f) * 0.05 * float3(1, 1, 1);
//    }
//
//    for (int i = 0; i < ArrayCapacity(gameState->TimePickupSpheres); i++)
//    {
//        Entity* e = gameState->TimePickupSpheres[i];
//
//        if (!Valid(e))
//            continue;
//
//        for (int j = 0; j < ArrayCapacity(gameState->TimePickupSpheres); j++)
//        {
//            float3 a = normalize(float3(rand(float2(i, j)), rand(float2(i, j) * 2), rand(float2(i, j) * 3)));
//            gameState->TimePickupSpheresOrbiters[i][j]->transform.position = e->transform.position + normalize(RotateAroundAxis(float3(0, 0, 1), a, (engineState->gameTime*4) + j)) * 0.5f;
//        }
//    }
//}
//
//void PickupHealth(EngineState* engineState, BallGameState* gameState, int i)
//{
//    gameState->PickupHealthEffectEntity->transform.position = gameState->HealthPickupSpheres[i]->transform.position;
//    gameState->PickupHealthEffectEntity->effect.startTime = engineState->gameTime;
//
//    (*gameState->HealthPickupSpheres[i]) = {};
//    (*gameState->HealthPickupSphereEffect[i]) = {};
//
//    gameState->health++;
//}
//
//void PickupTime(EngineState* engineState, BallGameState* gameState, int i)
//{
//    (*gameState->TimePickupSpheres[i]) = {};
//    (*gameState->TimePickupSphereEffect[i]) = {};
//}
//void PlayLevel(EngineState* engineState, BallGameState* gameState, int Level)
//{
//    gameState->menuState = MenuState_Playing;
//    LoadLevel(engineState, Level);
//    InitWorld(engineState, gameState);
//    gameState->player = SpawnPlayer(engineState, gameState);
//    gameState->score = 1000;
//    gameState->health = 3;
//    gameState->CurrentLevel = Level;
//}
//void ExitToMainMenu(EngineState* engineState, BallGameState* gameState)
//{
//    gameState->menuState = MenuState_MainMenu;
//    LoadLevel(engineState, 0);
//    gameState->CurrentLevel = 0;
//    gameState->player = 0;
//    gameState->score = 0;
//    gameState->health = 0;
//}
//void WinLevel(EngineState* engineState, BallGameState* gameState, BallGameSaveState* gameSaveState)
//{
//    gameState->menuState = MenuState_WinScreen;
//    gameSaveState->UnlockedLevels++;
//}
//
//// Called once per frame
//void GameUpdate(EngineState* engineState, VideoBuffer* videoBuffer, GameInput* Input, BallGameState* gameState, BallGameSaveState* gameSaveState)
//{
//    if (gameState->menuState == MenuState_Playing)
//    {
//        Transform* Camera = &engineState->camera;
//        Transform* Player = &gameState->player->transform;
//
//        float NormalizedSpeed = clamp01(length(gameState->player->velocity) / 1.5f);
//        float RollingVolume = pow(NormalizedSpeed, 1.5f) * 32.0f;
//        if (!gameState->player->grounded)
//            RollingVolume = 0;
//
//        Camera->position = Player->position + float3(0, 2, 5);
//        Entity* HitEntity = ((Entity*)gameState->player->TouchingEntity);
//        if (Valid(HitEntity))
//        {
//            gameState->playerSurfaceType = HitEntity->surfaceType;
//        }
//        float hitforce = (length(gameState->player->deIntersectVector) / Input->deltaTime) - 1.5f;
//        if (hitforce > 0.0f)
//        {
//            if (!SoundPlaying(engineState, 1))
//            {
//                switch (gameState->playerSurfaceType)
//                {
//                case SurfaceType_Metal:
//                {
//                    PlaySound(engineState, 1, gameState->HitSoundMetal, 1.0f, false);
//                }break;
//                case SurfaceType_Stone:
//                {
//                    PlaySound(engineState, 1, gameState->HitSoundStone, 1.0f, false);
//                }break;
//                case SurfaceType_Wood:
//                {
//                    PlaySound(engineState, 1, gameState->HitSoundWood, 1.0f, false);
//                }break;
//                }
//            }
//        }
//        if (gameState->playerSurfaceType != gameState->lastPlayerSurfaceType)
//        {
//            gameState->lastPlayerSurfaceType = gameState->playerSurfaceType;
//            switch (gameState->playerSurfaceType)
//            {
//            case SurfaceType_Metal:
//            {
//                PlaySound(engineState, 0, gameState->RollSoundMetal, 1.0f, true);
//            }break;
//            case SurfaceType_Stone:
//            {
//                PlaySound(engineState, 0, gameState->RollSoundStone, 1.0f, true);
//            }break;
//            case SurfaceType_Wood:
//            {
//                PlaySound(engineState, 0, gameState->RollSoundWood, 1.0f, true);
//            }break;
//            }
//        }
//        SetSoundChannelVolume(engineState, 0, RollingVolume);
//        SetSoundChannelVolume(engineState, 1, hitforce);
//
//        *Camera = LookRotation(*Camera, normalize(Player->position - Camera->position), float3(0, 0, 1));
//
//        float3 movementInput = float3(0, 0, 0);
//        movementInput.x += (Input->D ? 0 :  1);
//        movementInput.x += (Input->A ? 0 : -1);
//        movementInput.y += (Input->W ? 0 :  1);
//        movementInput.y += (Input->S ? 0 : -1);
//        if(length(movementInput) > 0)
//            movementInput = normalize(movementInput);
//
//        gameState->player->velocity += movementInput * Input->deltaTime * 0.2f;
//
//        gameState->score -= Input->deltaTime;
//        if (gameState->score <= 0)
//            gameState->score = 0;
//
//        if (Player->position.z < -4)
//        {
//            gameState->health--;
//            float3 SpawnPos = gameState->SpawnPoint->transform.position;
//            if (Valid(gameState->CurrentCheckpoint))
//                SpawnPos = gameState->CurrentCheckpoint->transform.position;
//
//            gameState->player->transform.position = SpawnPos;
//            gameState->player->velocity = float3(0,0,0);
//            gameState->player->angularVelocity = 0;
//
//            if (gameState->health < 0)
//            {
//                gameState->menuState = MenuState_LoseScreen;
//            }
//        }
//
//        for (int i = 0; i < ArrayCapacity(gameState->HealthPickupSpheres); i++)
//        {
//            Entity* e = gameState->HealthPickupSpheres[i];
//
//            if (!Valid(e))
//                continue;
//            
//            if (distance(e->transform.position, gameState->player->transform.position) < 1)
//            {
//                PickupHealth(engineState, gameState, i);
//            }
//        }
//
//        // TODO: something better than "t" for transform, find some way to namespace-smash types into eachother.
//        // Some entity system, more organized way of managing entities.
//        // Some entity array type.
//
//        int CheckpointIndex = -1;
//        for (int i = 0; i < ArrayCapacity(gameState->Checkpoints); i++)
//        {
//            if (!Valid(gameState->Checkpoints[i]))
//                continue;
//
//            if (distance(gameState->Checkpoints[i]->transform.position, gameState->player->transform.position) < 1)
//            {
//                gameState->CurrentCheckpoint = gameState->Checkpoints[i];
//                CheckpointIndex = i;
//                break;
//            }
//        }
//
//        if (gameState->CurrentCheckpoint != gameState->LastCurrentCheckpoint)
//        {
//            if(CheckpointIndex >= 0)
//                *gameState->CheckpointEffects[CheckpointIndex] = {};
//        }
//
//        if (Valid(gameState->win))
//        {
//            if (distance(gameState->win->transform.position, gameState->player->transform.position) < 1)
//            {
//                WinLevel(engineState, gameState, gameSaveState);
//            }
//        }
//
//        for (int i = 0; i < ArrayCapacity(gameState->TimePickupSpheres); i++)
//        {
//            Entity* e = gameState->TimePickupSpheres[i];
//            if (Valid(e))
//            {
//                if (distance(e->transform.position, gameState->player->transform.position) < 1)
//                {
//                    PickupTime(engineState, gameState, i);
//                }
//            }
//            else
//            {
//                for (int j = 0; j < ArrayCapacity(gameState->TimePickupSpheres); j++)
//                {
//                    Entity* orbiter = gameState->TimePickupSpheresOrbiters[i][j];
//                    if (Valid(orbiter))
//                    {
//                        orbiter->transform.scale.x -= Input->deltaTime * 1.0f;
//                        if (orbiter->transform.scale.x > 0)
//                        {
//                            // + float3(0, 0, 2.0f)
//                            orbiter->transform.position += (normalize(orbiter->transform.position - gameState->TimePickupEditorSphere[i]->transform.position) + float3(0,0,2)) * Input->deltaTime;
//                        }
//                        else
//                        {
//                            orbiter->transform.position = lerp(orbiter->transform.position, gameState->player->transform.position, Input->deltaTime * 1.0f);
//                            if (distance(orbiter->transform.position, gameState->player->transform.position) < 0.5f)
//                            {
//                                (*gameState->TimePickupSpheresOrbiters[i][j]) = {};
//                                gameState->score += 10.0f;
//                                gameState->TimePickupBurstEffect->transform.position = gameState->TimePickupSpheresOrbiters[i][j]->transform.position;
//                                gameState->TimePickupBurstEffect->effect.startTime = engineState->gameTime;
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }
//    UpdateWorld(engineState, gameState);
//}
//
//void GameUpdateUI(EngineState* engineState, VideoBuffer* videoBuffer, GameInput* Input, BallGameState* gameState, BallGameSaveState* gameSaveState, EngineSaveState* engineSaveState)
//{
//    if (gameState->menuState != MenuState_Playing)
//    {
//        DarkenBuffer(videoBuffer);
//    }
//    float2 a = float2(200, 100);
//    Text(engineState, videoBuffer, &a, MenuStateNames[gameState->menuState]);
//    Text(engineState, videoBuffer, &a, "");
//    
//    switch (gameState->menuState)
//    {
//    case MenuState_MainMenu:
//    {
//        Text(engineState, videoBuffer, &a, "This game is a couple-weeks-long project to learn C");
//        Text(engineState, videoBuffer, &a, "and program a game from scratch (no engine, no libraries.)");
//        Text(engineState, videoBuffer, &a, "All the textures, sound and game design are shamelessly");
//        Text(engineState, videoBuffer, &a, "ripped from an abandonware game called Ballance.");
//        Text(engineState, videoBuffer, &a, "");
//
//        Text(engineState, videoBuffer, &a, "Sound only works on chrome, multithreading only works on windows.");
//        Text(engineState, videoBuffer, &a, "win32 makes me annoyed, the web makes me infuriated.");
//        Text(engineState, videoBuffer, &a, "");
//
//        if (gameSaveState->UnlockedLevels == 0)
//        {
//            if (Button(engineState, Input, videoBuffer, &a, "Play"))
//            {
//                PlayLevel(engineState, gameState, 0);
//            }
//        }
//        else
//        {
//            if (Button(engineState, Input, videoBuffer, &a, "Continue"))
//            {
//                gameState->menuState = MenuState_ChapterSelect;
//            }
//        }
//        if (Button(engineState, Input, videoBuffer, &a, "Settings"))
//        {
//            gameState->menuState = MenuState_Settings;
//        }
//
//    }break;
//    case MenuState_ChapterSelect:
//    {
//        if (Button(engineState, Input, videoBuffer, &a, "Play Test Level"))
//        {
//            PlayLevel(engineState, gameState, 0);
//        }
//        if (Button(engineState, Input, videoBuffer, &a, "Play Level 1"))
//        {
//            PlayLevel(engineState, gameState, 1);
//        }
//        if (Button(engineState, Input, videoBuffer, &a, gameSaveState->UnlockedLevels >= 1 ? "Play Level 2" : "Level 2 Locked"))
//        {
//            if (gameSaveState->UnlockedLevels >= 1)
//            {
//                PlayLevel(engineState, gameState, 2);
//            }
//        }
//        if (Button(engineState, Input, videoBuffer, &a, gameSaveState->UnlockedLevels >= 2 ? "Play Level 3" : "Level 3 Locked"))
//        {
//            if (gameSaveState->UnlockedLevels >= 2)
//            {
//                PlayLevel(engineState, gameState, 3);
//            }
//        }
//        if (Button(engineState, Input, videoBuffer, &a, "Back"))
//        {
//            ExitToMainMenu(engineState, gameState);
//        }
//    }break;
//    case MenuState_Playing:
//    {
//        Text(engineState, videoBuffer, &a, "Health: ", gameState->health);
//        Text(engineState, videoBuffer, &a, "Time: ", round(gameState->score));
//        if (Input->EscapeDown)
//        {
//            gameState->menuState = MenuState_Paused;
//        }
//        if (Button(engineState, Input, videoBuffer, &a, "Win Level"))
//        {
//            WinLevel(engineState, gameState, gameSaveState);
//        }
//    }break;
//    case MenuState_Paused:
//    {if (Input->EscapeDown)
//    {
//        gameState->menuState = MenuState_Playing;
//    }
//    if (Button(engineState, Input, videoBuffer, &a, "Exit to Main Menu"))
//    {
//        ExitToMainMenu(engineState, gameState);
//    }
//    if (Button(engineState, Input, videoBuffer, &a, "Restart Level"))
//    {
//        PlayLevel(engineState, gameState, gameState->CurrentLevel);
//    }
//    if (Button(engineState, Input, videoBuffer, &a, "Back"))
//    {
//        gameState->menuState = MenuState_Playing;
//    }
//    }break;
//    case MenuState_Settings:
//    {
//        engineSaveState->threadedRendering = Toggle(engineState, Input, videoBuffer, &a, engineSaveState->threadedRendering, "Threaded rendering (only on win32)");
//
//        if (Button(engineState, Input, videoBuffer, &a, "Reset Progress"))
//        {
//            gameSaveState->UnlockedLevels = 0;
//        }
//        if (Button(engineState, Input, videoBuffer, &a, "Back"))
//        {
//            ExitToMainMenu(engineState, gameState);
//        }
//    }break;
//    case MenuState_WinScreen:
//    {
//        Text(engineState, videoBuffer, &a, "Victory!");
//        int highschore = ((int)round(gameState->score)) + gameState->health * 10;
//        Text(engineState, videoBuffer, &a, "Score: ", highschore);
//        if (Button(engineState, Input, videoBuffer, &a, "Continue"))
//        {
//            if (gameState->CurrentLevel == 0)
//                gameSaveState->Level1HighScore = min(gameSaveState->Level1HighScore, highschore);
//            if (gameState->CurrentLevel == 1)
//                gameSaveState->Level2HighScore = min(gameSaveState->Level2HighScore, highschore);
//            if (gameState->CurrentLevel == 2)
//                gameSaveState->Level3HighScore = min(gameSaveState->Level3HighScore, highschore);
//
//            ExitToMainMenu(engineState, gameState);
//            gameState->menuState = MenuState_ChapterSelect;
//        }
//
//    }break;
//    case MenuState_LoseScreen:
//    {
//        Text(engineState, videoBuffer, &a, "Game over!");
//        if (Button(engineState, Input, videoBuffer, &a, "Exit to Main Menu"))
//        {
//            ExitToMainMenu(engineState, gameState);
//        }
//        if (Button(engineState, Input, videoBuffer, &a, "Replay"))
//        {
//            PlayLevel(engineState, gameState, gameState->CurrentLevel);
//        }
//    }break;
//    }
//    if (Valid(gameState->player))
//    {
//        Text(engineState, videoBuffer, &a, "wat", length(gameState->player->deIntersectVector) / Input->deltaTime);
//    }
//    //Text(engineState, videoBuffer, &a, "position: ", gameState->player->t.position);
//    //Text(engineState, videoBuffer, &a, "velocity: ", gameState->player->velocity);
//    //Text(engineState, videoBuffer, &a, "angularVelocity: ", gameState->player->angularVelocity);
//    //Text(engineState, videoBuffer, &a, "angularVelocityAxis: ", gameState->player->angularVelocityAxis);
//}