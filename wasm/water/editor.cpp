#pragma once
#include "quote.h"
//
//
//int FindImageIndex(EngineState* engineState, Image* image)
//{
//    for (int i = 0; i < ArrayCapacity(engineState->images); i++)
//    {
//        if (engineState->images[i] == image)
//        {
//            return i;
//        }
//    }
//    return -1; // not found
//}
//
//// Functions called when deserializing
//Entity* MakeEntity(EngineState* engineState, Entity* entity,
//    Transform t, bool mesh, int type, int texture1, int texture2, // mesh
//    int shader, bool physics = 0, bool kinematic = 0, float mass = 1, // physics
//    EffectData effectData = {}, int group = 0, int surfaceType = 0)
//{
//    *entity = {};
//    entity->initialized = true;
//
//    // Transform
//    entity->transform = t;
//
//    // Mesh
//    entity->mesh = mesh;
//    entity->type = (EntityType)type;
//    entity->texture1 = engineState->images[texture1];
//    entity->texture2 = engineState->images[texture2];
//    entity->shaderIndex = shader;
//
//    // Physics
//    entity->physics = physics;
//    entity->kinematic = kinematic;
//    entity->mass = mass;
//    entity->friction = 0.01f;
//    entity->drag = 0.005f;
//    entity->effect = effectData;
//
//    entity->color = float3(1, 1, 1);
//    entity->group = group;
//
//    entity->surfaceType = (SurfaceType)surfaceType;
//
//    // Particle
//    //entity->spawnRate = spawnRate;
//    //entity->spawnRadius = spawnRadius;
//    //entity->minSpeed = minSpeed;
//    //entity->maxSpeed = maxSpeed;
//    //entity->startSize = startSize;
//    //entity->endSize = endSize;
//    return entity;
//}
//
//void AppendArgument(int max, char* Str, float3 in)
//{
//    Append(Str, ", ", max);
//    CoerceAppend(Str, in, max);
//}
//void AppendArgument(int max, char* Str, float in)
//{
//    Append(Str, ", ", max);
//    CoerceAppend(Str, in, max);
//}
//void AppendArgument(int max, char* Str, int in)
//{
//    Append(Str, ", ", max);
//    Append(Str, in, max);
//}
//
//
//void WriteEntity(EngineState* engineState, char* target, Entity* entity, int index)
//{
//    int max = ArrayCapacity(engineState->saveBuffer);
//    Append(target, "MakeEntity", max);
//    //if (entity->mesh)
//    //{
//    //    if (entity->type == EntityType_Box)
//    //    else
//    //        Append(target, "MakeEntity_Sphere", max);
//    //}
//    Append(target, "(engineState, &engineState->Entities[", max);
//    Append(target, index, max);
//    Append(target, "], ", max);
//
//    // Transform
//    Append(target, "transform(", max);
//    CoerceAppend(target, entity->transform.position, max);
//    Append(target, ", ", max);
//    CoerceAppend(target, entity->transform.forward, max);
//    Append(target, ", ", max);
//    CoerceAppend(target, entity->transform.up, max);
//    Append(target, ", ", max);
//    CoerceAppend(target, entity->transform.scale, max);
//    Append(target, ")", max);
//
//    // Mesh
//    AppendArgument(max, target, entity->mesh);
//    AppendArgument(max, target, entity->type);
//    AppendArgument(max, target, FindImageIndex(engineState, entity->texture1));
//    AppendArgument(max, target, FindImageIndex(engineState, entity->texture2));
//    AppendArgument(max, target, entity->shaderIndex);
//
//    // Physics
//    AppendArgument(max, target, entity->physics);
//    AppendArgument(max, target, entity->kinematic);
//    AppendArgument(max, target, entity->mass);
//
//    // Particle
//    Append(target, ", ctor_EffectData(", max);
//    Append(target, entity->effect.spawnRate, max);
//    AppendArgument(max, target, entity->effect.burst);
//    AppendArgument(max, target, entity->effect.spawnRadius);
//    AppendArgument(max, target, entity->effect.drag);
//    AppendArgument(max, target, entity->effect.minSpeed);
//    AppendArgument(max, target, entity->effect.maxSpeed);
//    AppendArgument(max, target, entity->effect.minLifetime);
//    AppendArgument(max, target, entity->effect.maxLifetime);
//    AppendArgument(max, target, entity->effect.startSize);
//    AppendArgument(max, target, entity->effect.endSize);
//    AppendArgument(max, target, entity->effect.gravity);
//    Append(target, ")", max);
//
//    AppendArgument(max, target, entity->group);
//    AppendArgument(max, target, entity->surfaceType);
//
//    Append(target, ");", max);
//    Append(target, "\n", max);
//}
//
//void LoadLevel(EngineState* engineState, int level)
//{
//    
//    for (int j = 0; j < ArrayCapacity(engineState->entities); j++)
//    {
//        engineState->entities[j].initialized = false;
//    }
//    engineState->currentLevel = level;
//    if (level == 0)
//    {
//#include "Level0.cpp";
//    }
//    else if (level == 1)
//    {
//#include "Level1.cpp";
//    }
//    else if (level == 2)
//    {
//#include "Level2.cpp";
//    }
//    else if (level == 3)
//    {
//#include "Level3.cpp";
//    }
//
//    // Wipe the undo buffer
//    //for (int i = 0; i < ArrayCapacity(engineState->EntityUndoBuffer); i++)
//    //{
//    //    for (int j = 0; j < ArrayCapacity(engineState->Entities); j++)
//    //    {
//    //        engineState->Entities[j] = engineState->Entities[j];
//    //    }
//    //}
//
//}
//
//void SaveLevel(GameMemory* memory, EngineState* engineState, int level)
//{
//    for (int i = 0; i < ArrayCapacity(engineState->saveBuffer); i++)
//    {
//        engineState->saveBuffer[i] = 0;
//    }
//    for (int i = 0; i < ArrayCapacity(engineState->entities); i++)
//    {
//        Entity* entity = &engineState->entities[i];
//        if (!entity->initialized)
//            continue;
//        WriteEntity(engineState, engineState->saveBuffer, entity, i);
//    }
//    char LevelName[11] = {};
//    Append(LevelName, "Level");
//    Append(LevelName, level);
//    Append(LevelName, ".cpp");
//
//    memory->platformWriteFile(LevelName, &engineState->saveBuffer, StringLength(engineState->saveBuffer));
//}
//
//
//void UpdateEditorCamera(EngineState* engineState, EngineSaveState* engineSaveState, GameInput* Input)
//{
//    engineSaveState->camera = rotate(engineSaveState->camera, float3(0, 0, 1), -Input->MouseXDelta * 0.005f);
//    engineSaveState->camera = rotate(engineSaveState->camera, engineSaveState->camera.right, -Input->MouseYDelta * 0.005f);
//
//    int X = Input->W ? 1 : 0 + Input->S ? -1 : 0;
//    int Y = Input->D ? 1 : 0 + Input->A ? -1 : 0;
//    int Z = Input->E ? 1 : 0 + Input->Q ? -1 : 0;
//
//    float3 Movement = float3(0, 0, 0);
//    Movement = Movement + engineSaveState->camera.forward * X;
//    Movement = Movement + engineSaveState->camera.right * Y;
//    Movement = Movement + engineSaveState->camera.up * Z;
//    if (length(Movement) > 0)
//        Movement = normalize(Movement);
//    float speed = 4;
//    if (Input->Shift)
//        speed = 16;
//    engineSaveState->camera.position += Movement * Input->DeltaTime * speed;
//
//}
//
//void ClearSelection(EngineState* engineState)
//{
//    for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
//    {
//        engineState->selectedEntities[i] = 0;
//    }
//}
//
//void SelectEntity(EngineState* engineState, Entity* entity)
//{
//    // Find an empty slot in the list of selected entites and add our entity there.
//
//    for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
//    {
//        if (engineState->selectedEntities[i] == entity)
//            break;
//        if (!Valid(engineState->selectedEntities[i]))
//        {
//            engineState->selectedEntities[i] = entity;
//            
//            break;
//        }
//    }
//}
//
//void SelectGroup(EngineState* engineState, int group)
//{
//    if (group == 0)
//        return;
//
//    for (int i = 0; i < ArrayCapacity(engineState->entities); i++)
//    {
//        Entity* e = &engineState->entities[i];
//
//        if (Valid(e) && engineState->entities[i].group == group)
//        {
//            SelectEntity(engineState, e);
//        }
//    }
//}
//
//// find a free and avilable group
//int FindFreeGroup(EngineState* engineState)
//{
//    // If multiple entites are selected, find an empty group slot and group them togeather.
//    bool GroupOccupied[100] = {};
//    int FreeGroup = 0;
//    for (int i = 0; i < ArrayCapacity(engineState->entities); i++)
//    {
//        Entity* e = &engineState->entities[i];
//        if (Valid(e))
//        {
//            GroupOccupied[e->group] = true;
//        }
//    }
//    for (int i = 0; i < ArrayCapacity(GroupOccupied); i++)
//    {
//        if (GroupOccupied[i] == false)
//        {
//            FreeGroup = i;
//            break;
//        }
//    }
//    return FreeGroup;
//}
//
//void DeselectEntity(EngineState* engineState, Entity* entity)
//{
//    // loop through the list of selected entites, if our one is found, set it to 0.
//    for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
//    {
//        if (engineState->selectedEntities[i] == entity)
//        {
//            engineState->selectedEntities[i] = 0;
//            break;
//        }
//    }
//}
//
//void UpdateEditor(GameMemory* memory, EngineState* engineState, EngineSaveState* engineSaveState, GameInput* Input, VideoBuffer* videoBuffer, float2* UIPos)
//{
//    // EDITOR STUFF
//    //Text(engineState, videoBuffer, &UIPos, "Editor Controls");
//    //Text(engineState, videoBuffer, &UIPos, "  W Translate.");
//    //Text(engineState, videoBuffer, &UIPos, "  E Rotate.");
//    //Text(engineState, videoBuffer, &UIPos, "  R Scale.");
//    //Text(engineState, videoBuffer, &UIPos, "  Delete to delete selection.");
//    //Text(engineState, videoBuffer, &UIPos, "  Ctrl + D to duplicate selection.");
//    //Text(engineState, videoBuffer, &UIPos, "  Left click to select.");
//    //Text(engineState, videoBuffer, &UIPos, "  Shift + Left click add to selection.");
//    //Text(engineState, videoBuffer, &UIPos, "  Ctrl + Left click remove from selection.");
//
//    for (int i = 0; i < ArrayCapacity(engineState->entities); i++)
//    {
//        Entity* e = &engineState->entities[i];
//        if (!Valid(e))
//            continue;
//
//        float3 ScreenPos3D = ToScreenPos(videoBuffer, e->transform.position, engineState->camera);
//        if (ScreenPos3D.z < 0)
//            continue;
//
//        if (e->type <= 2)
//            continue;
//
//        float2 ScreenPos = float2(ScreenPos3D.x, (ScreenPos3D.y * 0.5) + 100);
//
//        Text(engineState, videoBuffer, ScreenPos, EntityTypeNames[e->type]);
//    }
//
//    Entity* GizmoEntity = 0;
//    for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
//    {
//        Entity* e = engineState->selectedEntities[i];
//        if (!Valid(e))
//            continue;
//
//        GizmoEntity = e;
//    }
//
//    if (!Input->MouseRight)
//    {
//        if (Input->W)
//            engineState->gizmoState = 0;
//        if (Input->E)
//            engineState->gizmoState = 1;
//        if (Input->R)
//            engineState->gizmoState = 2;
//    }
//
//    float3 MoveOffset = float3(0, 0, 0);
//    float3 ScaleOffset = float3(0, 0, 0);
//    Rotation RotationOffset;
//    RotationOffset.Axis = float3(1, 0, 0);
//    RotationOffset.Angle = 0;
//    if (Valid(GizmoEntity))
//    {
//        if (engineState->gizmoState == 0)
//        {
//            MoveOffset = MoveGizmo(engineState, Input, videoBuffer, engineState->camera, GizmoEntity->transform, engineSaveState->gizmoWorldSpace);
//        }
//        else if (engineState->gizmoState == 1)
//        {
//            RotationOffset = RotateGizmo(engineState, Input, videoBuffer, engineState->camera, GizmoEntity->transform, engineSaveState->gizmoWorldSpace);
//        }
//        else if (engineState->gizmoState == 2)
//        {
//            ScaleOffset = ScaleGizmo(engineState, Input, videoBuffer, engineState->camera, GizmoEntity->transform);
//        }
//    }
//    engineState->grabbing = engineState->pickedSliderID != -1;
//
//    for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
//    {
//        Entity* e = engineState->selectedEntities[i];
//        if (Valid(e))
//        {
//            e->transform.position += MoveOffset;
//            e->transform.scale += ScaleOffset;
//            e->transform = rotate(e->transform, RotationOffset.Axis, RotationOffset.Angle);
//
//            if (e != GizmoEntity)
//            {
//                float3 RelativePos = e->transform.position - GizmoEntity->transform.position;
//                RelativePos = RotateAroundAxis(RelativePos, RotationOffset.Axis, RotationOffset.Angle);
//                RelativePos += GizmoEntity->transform.position;
//                e->transform.position = RelativePos;
//            }
//        }
//    }
//
//    if (Input->Ctrl && Input->GDown)
//    {
//        engineState->operationHappened = true;
//        if (ArrayCapacity(engineState->selectedEntities) == 1) // If a single entity is selected, clear its group
//        {
//            Entity* e = engineState->selectedEntities[0];
//            if (Valid(e))
//            {
//                e->group = 0;
//            }
//        }
//        else
//        {
//            int FreeGroup = FindFreeGroup(engineState);
//
//            for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
//            {
//                Entity* e = engineState->selectedEntities[i];
//                if (Valid(e))
//                {
//                    e->group = FreeGroup;
//                }
//            }
//        }
//    }
//    if (Input->Shift && Input->GDown)
//    {
//        engineState->operationHappened = true;
//        for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
//        {
//            Entity* e = engineState->selectedEntities[i];
//            if (Valid(e))
//            {
//                e->group = 0;
//            }
//        }
//    }
//
//    if (Input->DeleteDown)
//    {
//        engineState->operationHappened = true;
//        for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
//        {
//            Entity* e = engineState->selectedEntities[i];
//            if (Valid(e))
//            {
//                *e = {};
//            }
//        }
//    }
//
//
//    if (Input->Ctrl && Input->DDown)
//    {
//        int FreeGroup = FindFreeGroup(engineState);
//        for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
//        {
//            Entity* selectedEntity = engineState->selectedEntities[i];
//            if (!Valid(selectedEntity))
//                continue;
//            
//            for (int j = 0; j < ArrayCapacity(engineState->entities); j++)
//            {
//                Entity* entity = &engineState->entities[j];
//                if (Valid(entity))
//                    continue;
//
//                *entity = *selectedEntity; // copy?
//                entity->group = FreeGroup;
//                break;
//            }
//        }
//    }
//
//    if (!engineState->grabbing)
//    {
//        // Find closest entity under cursor.
//        float3 MouseRay = ScreenPointToRay(videoBuffer, engineState->camera, float2(Input->MouseX, Input->MouseY));
//        float ClosestDistance = 99999;
//        engineState->hoveredEntity = 0;
//        for (int i = 0; i < ArrayCapacity(engineState->entities); i++)
//        {
//            Entity* e = &engineState->entities[i];
//            if (!Valid(e))
//                continue;
//
//            float3 Normal;
//            float3 LocalPos = WorldToLocal(e->transform, engineState->camera.position);
//            float3 LocalDir = WorldToLocalVector(e->transform, MouseRay);
//
//            float HitDepth = RayBoxIntersect(LocalPos, LocalDir, &Normal);
//            if (HitDepth > 0)
//            {
//                if (HitDepth < ClosestDistance)
//                {
//                    ClosestDistance = HitDepth;
//                    engineState->hoveredEntity = e;
//                }
//            }
//        }
//
//        if (Input->MouseLeftDown && !engineState->mouseIsOverUI && Valid(engineState->hoveredEntity))
//        {
//            if (Input->Shift)
//            {
//                SelectEntity(engineState, engineState->hoveredEntity);
//                SelectGroup(engineState, engineState->hoveredEntity->group);
//            }
//            else if (Input->Ctrl)
//            {
//                DeselectEntity(engineState, engineState->hoveredEntity);
//            }
//            else
//            {
//                ClearSelection(engineState);
//                SelectEntity(engineState, engineState->hoveredEntity);
//                SelectGroup(engineState, engineState->hoveredEntity->group);
//            }
//        }
//        if (Input->EscapeDown)
//        {
//            ClearSelection(engineState);
//        }
//    }
//
//    engineState->mouseIsOverUI = false;
//
//
//
//    float2 MenuPlacement = float2(videoBuffer->Width - 120, 5);
//
//    for (int i = 0; i < 4; i++)
//    {
//        char LoadText[100] = "Load Level ";
//        Append(LoadText, i);
//        if (Button(engineState, Input, videoBuffer, &MenuPlacement, LoadText))
//        {
//            LoadLevel(engineState,i);
//        }
//    }
//
//    if (Button(engineState, Input, videoBuffer, &MenuPlacement, "Save"))
//    {
//        SaveLevel(memory, engineState, engineState->currentLevel);
//    }
//    if (Input->Ctrl && Input->SDown)
//    {
//        SaveLevel(memory, engineState, engineState->currentLevel);
//    }
//
//    char SpaceText[10] = {};
//    engineSaveState->gizmoWorldSpace ? Append(SpaceText, "Local Space") : Append(SpaceText, "World Space");
//    if (Button(engineState, Input, videoBuffer, &MenuPlacement, SpaceText))
//    {
//        engineSaveState->gizmoWorldSpace = !engineSaveState->gizmoWorldSpace;
//    }
//
//    Text(engineState, videoBuffer, &MenuPlacement, "");
//    Text(engineState, videoBuffer, &MenuPlacement, "Entities.");
//    for (int i = 0; i < ArrayCapacity(engineState->entities); i++)
//    {
//        Entity* e = &engineState->entities[i];
//
//        if (!Valid(e))
//            continue;
//
//        char LoadText[100] = "Load Level ";
//        if (Button(engineState, Input, videoBuffer, &MenuPlacement, EntityTypeNames[e->type]))
//        {
//            ClearSelection(engineState);
//            engineState->selectedEntities[0] = e;
//        }
//    }
//
//    Text(engineState, videoBuffer, UIPos, " ");
//    
//    if (Button(engineState, Input, videoBuffer, UIPos, "Snap Rotation"))
//    {
//        engineState->operationHappened = true;
//        for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
//        {
//            Entity* entity = engineState->selectedEntities[i];
//            if (!Valid(entity))
//                continue;
//
//            for (int i = 0; i < 5; i++)
//            {
//                entity->transform.right.x = MoveTowards(entity->transform.right.x, 0, 0.5f);
//                entity->transform.right.y = MoveTowards(entity->transform.right.y, 0, 0.5f);
//                entity->transform.right.z = MoveTowards(entity->transform.right.z, 0, 0.5f);
//                entity->transform.right = normalize(entity->transform.right);
//
//                entity->transform.forward.x = MoveTowards(entity->transform.forward.x, 0, 0.5f);
//                entity->transform.forward.y = MoveTowards(entity->transform.forward.y, 0, 0.5f);
//                entity->transform.forward.z = MoveTowards(entity->transform.forward.z, 0, 0.5f);
//                entity->transform.forward = normalize(entity->transform.forward);
//
//                entity->transform.up.x = MoveTowards(entity->transform.up.x, 0, 0.5f);
//                entity->transform.up.y = MoveTowards(entity->transform.up.y, 0, 0.5f);
//                entity->transform.up.z = MoveTowards(entity->transform.up.z, 0, 0.5f);
//                entity->transform.up = normalize(entity->transform.up);
//            }
//        }
//    }
//    if (Button(engineState, Input, videoBuffer, UIPos, "Snap"))
//    {
//        engineState->operationHappened = true;
//        for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
//        {
//            Entity* entity = engineState->selectedEntities[i];
//            if (!Valid(entity))
//                continue;
//
//            entity->transform.scale = round(entity->transform.scale);
//            if (entity->transform.scale.x <= 1) entity->transform.scale.x = 1;
//            if (entity->transform.scale.y <= 1) entity->transform.scale.y = 1;
//            if (entity->transform.scale.z <= 1) entity->transform.scale.z = 1;
//
//            float3 HalfScale = float3(0, 0, 0);
//            HalfScale += entity->transform.right * (entity->transform.scale.x * 0.5);
//            HalfScale += entity->transform.forward * (entity->transform.scale.y * 0.5);
//            HalfScale += entity->transform.up * (entity->transform.scale.z * 0.5);
//
//            entity->transform.position = round(entity->transform.position - HalfScale) + HalfScale;
//
//            for (int i = 0; i < 5; i++)
//            {
//                entity->transform.right.x = MoveTowards(entity->transform.right.x, 0, 0.5f);
//                entity->transform.right.y = MoveTowards(entity->transform.right.y, 0, 0.5f);
//                entity->transform.right.z = MoveTowards(entity->transform.right.z, 0, 0.5f);
//                entity->transform.right = normalize(entity->transform.right);
//
//                entity->transform.forward.x = MoveTowards(entity->transform.forward.x, 0, 0.5f);
//                entity->transform.forward.y = MoveTowards(entity->transform.forward.y, 0, 0.5f);
//                entity->transform.forward.z = MoveTowards(entity->transform.forward.z, 0, 0.5f);
//                entity->transform.forward = normalize(entity->transform.forward);
//
//                entity->transform.up.x = MoveTowards(entity->transform.up.x, 0, 0.5f);
//                entity->transform.up.y = MoveTowards(entity->transform.up.y, 0, 0.5f);
//                entity->transform.up.z = MoveTowards(entity->transform.up.z, 0, 0.5f);
//                entity->transform.up = normalize(entity->transform.up);
//            }
//        }
//    }
//
//    if (Button(engineState, Input, videoBuffer, UIPos, "Reset Selected"))
//    {
//        engineState->operationHappened = true;
//        for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
//        {
//            Entity* entity = engineState->selectedEntities[i];
//            if (!Valid(entity))
//                continue;
//
//            //entity->t.scale = float3(1, 1, 1);
//            entity->transform.scale = float3(0.5, 0.5, 0.5);
//
//            entity->transform.right = float3(1, 0, 0);
//            entity->transform.forward = float3(0, 1, 0);
//            entity->transform.up = float3(0, 0, 1);
//            if (entity->type == EntityType_Effect)
//            {
//                entity->effect.spawnRate = 15.0f;
//                entity->effect.burst = false;
//                entity->effect.spawnRadius = 0.1f;
//                entity->effect.drag = 1.0f;
//                entity->effect.minSpeed = float3(-0.1, 0.1, 0.5f);
//                entity->effect.maxSpeed = float3(0.1, 0.1, 1.0f);
//                entity->effect.minLifetime = 1.0f;
//                entity->effect.maxLifetime = 2.0f;
//                entity->effect.startSize = 1.0f;
//                entity->effect.endSize = 2.0f;
//                entity->effect.gravity = -5.0f;
//                entity->color = float3(1, 1, 1);
//            }
//        }
//    }
//    if (Button(engineState, Input, videoBuffer, UIPos, "Play Effect"))
//    {
//        for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
//        {
//            Entity* entity = engineState->selectedEntities[0];
//            if (!Valid(entity))
//                continue;
//
//            entity->effect.startTime = engineState->gameTime;
//        }
//    }
//
//    Text(engineState, videoBuffer, UIPos, " ");
//    Entity* entity = engineState->selectedEntities[0];
//    if (Valid(entity))
//    {
//        Text(engineState, videoBuffer, UIPos, "Position: ", entity->transform.position);
//        Text(engineState, videoBuffer, UIPos, "Group: ", entity->group);
//        Text(engineState, videoBuffer, UIPos, EntityTypeNames[entity->type]);
//        entity->type = (EntityType)(int)round(Slider(engineState, Input, videoBuffer, UIPos, entity->type, 0, ArrayCapacity(EntityTypeNames)));
//
//        Text(engineState, videoBuffer, UIPos, ShaderNames[entity->shaderIndex]);
//        int LastShaderIndex = entity->shaderIndex;
//        entity->shaderIndex = Slider(engineState, Input, videoBuffer, UIPos, entity->shaderIndex, 0, ArrayCapacity(ShaderNames));
//        if (LastShaderIndex != entity->shaderIndex)
//        {
//            for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
//            {
//                if (!Valid(engineState->selectedEntities[i]))
//                    continue;
//                engineState->selectedEntities[i]->shaderIndex = entity->shaderIndex;
//            }
//        }
//
//        Text(engineState, videoBuffer, UIPos, "Texture 1: ");
//        int texture1 = FindImageIndex(engineState, entity->texture1);
//        int LastTexture1 = texture1;
//        texture1 = Slider(engineState, Input, videoBuffer, UIPos, texture1, 0, engineState->currentImage);
//        entity->texture1 = engineState->images[texture1];
//        if (LastTexture1 != texture1)
//        {
//            for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
//            {
//                if (!Valid(engineState->selectedEntities[i]))
//                    continue;
//                engineState->selectedEntities[i]->texture1 = entity->texture1;
//            }
//        }
//
//        int EntityCount = 0;
//        for (int i = 0; i < ArrayCapacity(engineState->entities); i++)
//        {
//            if (Valid(engineState->entities[i]))
//                EntityCount++;
//        }
//
//        Text(engineState, videoBuffer, UIPos, "Entity Count: ", EntityCount);
//
//        Text(engineState, videoBuffer, UIPos, "Texture 2: ");
//        int texture2 = FindImageIndex(engineState, entity->texture2);
//        int LastTexture2 = texture2;
//        texture2 = Slider(engineState, Input, videoBuffer, UIPos, texture2, 0, engineState->currentImage);
//        entity->texture2 = engineState->images[texture2];
//        if (LastTexture2 != texture2)
//        {
//            for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
//            {
//                if (!Valid(engineState->selectedEntities[i]))
//                    continue;
//                engineState->selectedEntities[i]->texture2 = entity->texture2;
//            }
//        }
//
//        if (entity->type == EntityType_Sphere || entity->type == EntityType_Box)
//        {
//            Text(engineState, videoBuffer, UIPos, surfaceTypeNames[entity->surfaceType]);
//            int LastShaderIndex = entity->surfaceType;
//            entity->surfaceType = (SurfaceType)(int)round(Slider(engineState, Input, videoBuffer, UIPos, entity->surfaceType, 0, ArrayCapacity(surfaceTypeNames)));
//            if (LastShaderIndex != entity->surfaceType)
//            {
//                for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
//                {
//                    if (!Valid(engineState->selectedEntities[i]))
//                        continue;
//                    engineState->selectedEntities[i]->surfaceType = entity->surfaceType;
//                }
//            }
//
//            entity->physics = Toggle(engineState, Input, videoBuffer, UIPos, entity->physics, "Physics");
//            entity->kinematic = Toggle(engineState, Input, videoBuffer, UIPos, entity->kinematic, "Kinematic");
//            if (entity->kinematic || !entity->physics)
//            {
//                entity->angularVelocity = 0;
//                entity->velocity = float3(0, 0, 0);
//            }
//            Text(engineState, videoBuffer, UIPos, "Mass: ", entity->mass);
//            entity->mass = Slider(engineState, Input, videoBuffer, UIPos, entity->mass, 0, 4);
//
//        }
//        else if (entity->type == EntityType_Effect)
//        {
//            EffectData* effect = &entity->effect;
//
//            float2 UIPos2 = float2(160 * 2, videoBuffer->Height - 260);
//            float2 UIPos3 = float2(160, videoBuffer->Height - 260);
//
//            effect->burst = Toggle(engineState, Input, videoBuffer, &UIPos3, effect->burst, "Burst");
//
//
//            Text(engineState, videoBuffer, &UIPos3, "spawnRate: ", effect->spawnRate);
//            effect->spawnRate = Slider(engineState, Input, videoBuffer, &UIPos3, effect->spawnRate, 0, 25);
//
//
//            Text(engineState, videoBuffer, &UIPos3, "spawnRadius: ", effect->spawnRadius);
//            effect->spawnRadius = Slider(engineState, Input, videoBuffer, &UIPos3, effect->spawnRadius, 0, 4);
//
//            Text(engineState, videoBuffer, &UIPos3, "drag: ", effect->drag);
//            effect->drag = Slider(engineState, Input, videoBuffer, &UIPos3, effect->drag, 0, 4);
//
//            Text(engineState, videoBuffer, &UIPos3, "gravity: ", effect->gravity);
//            effect->gravity = Slider(engineState, Input, videoBuffer, &UIPos3, effect->gravity, -25, 0);
//
//
//            Text(engineState, videoBuffer, &UIPos3, "startSize: ", effect->startSize);
//            effect->startSize = Slider(engineState, Input, videoBuffer, &UIPos3, effect->startSize, 0, 4);
//
//            Text(engineState, videoBuffer, &UIPos3, "endSize: ", effect->endSize);
//            effect->endSize = Slider(engineState, Input, videoBuffer, &UIPos3, effect->endSize, 0, 4);
//
//
//            Text(engineState, videoBuffer, &UIPos2, "minSpeed.x: ", effect->minSpeed.x);
//            effect->minSpeed.x = Slider(engineState, Input, videoBuffer, &UIPos2, effect->minSpeed.x, -1, 1);
//            Text(engineState, videoBuffer, &UIPos2, "minSpeed.y: ", effect->minSpeed.y);
//            effect->minSpeed.y = Slider(engineState, Input, videoBuffer, &UIPos2, effect->minSpeed.y, -1, 1);
//            Text(engineState, videoBuffer, &UIPos2, "minSpeed.z: ", effect->minSpeed.z);
//            effect->minSpeed.z = Slider(engineState, Input, videoBuffer, &UIPos2, effect->minSpeed.z, -1, 1);
//
//
//            Text(engineState, videoBuffer, &UIPos2, "maxSpeed.x: ", effect->maxSpeed.x);
//            effect->maxSpeed.x = Slider(engineState, Input, videoBuffer, &UIPos2, effect->maxSpeed.x, -1, 1);
//            Text(engineState, videoBuffer, &UIPos2, "maxSpeed.y: ", effect->maxSpeed.y);
//            effect->maxSpeed.y = Slider(engineState, Input, videoBuffer, &UIPos2, effect->maxSpeed.y, -1, 1);
//            Text(engineState, videoBuffer, &UIPos2, "maxSpeed.z: ", effect->maxSpeed.z);
//            effect->maxSpeed.z = Slider(engineState, Input, videoBuffer, &UIPos2, effect->maxSpeed.z, -1, 1);
//
//
//            Text(engineState, videoBuffer, &UIPos2, "minLifetime: ", effect->minLifetime);
//            effect->minLifetime = Slider(engineState, Input, videoBuffer, &UIPos2, effect->minLifetime, 0, 4);
//
//            Text(engineState, videoBuffer, &UIPos2, "maxLifetime: ", effect->maxLifetime);
//            effect->maxLifetime = Slider(engineState, Input, videoBuffer, &UIPos2, effect->maxLifetime, 0, 4);
//        }
//    }
//
//
//    int UndoBufferLength = ArrayCapacity(engineState->entityUndoBuffer) - 1;
//
//    if (engineState->operationHappened)
//    {
//        engineState->undoSteps++; // add another undo step
//        if (engineState->undoSteps > UndoBufferLength)
//            engineState->undoSteps = UndoBufferLength;
//
//        engineState->operationHappened = false;
//        engineState->undoIndex++;
//        if (engineState->undoIndex < 0)
//            engineState->undoIndex = UndoBufferLength;
//        if (engineState->undoIndex > UndoBufferLength)
//            engineState->undoIndex = 0;
//        for (int i = 0; i < ArrayCapacity(engineState->entities); i++)
//        {
//            engineState->entityUndoBuffer[engineState->undoIndex][i] = engineState->entities[i];
//        }
//    }
//    // Undo
//    if (Input->Ctrl && Input->ZDown)
//    {
//        if (engineState->undoSteps > 0) // there are undo steps left
//        {
//            engineState->undoIndex--;
//            if (engineState->undoIndex < 0)
//                engineState->undoIndex = UndoBufferLength;
//            if (engineState->undoIndex > UndoBufferLength)
//                engineState->undoIndex = 0;
//
//            for (int i = 0; i < ArrayCapacity(engineState->entities); i++)
//            {
//                engineState->entities[i] = engineState->entityUndoBuffer[engineState->undoIndex][i];
//            }
//        }
//        engineState->undoSteps--;
//        if (engineState->undoSteps <= 0)
//            engineState->undoSteps = 0;
//    }
//}