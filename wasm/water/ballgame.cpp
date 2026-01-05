#pragma once
#include "quote.h"

//int addFloat(float a, float b)
//{
//    return a + b;
//}
//int addInt(int a, int b)
//{
//    return a + b;
//}
//int addInt16(uint16 a, uint16 b)
//{
//    return a + b;
//}
//
//
//#define Add(X, y) _Generic((X), float    : addFloat , \
//                                int      : addInt   , \
//                                uint16   : addInt16 , \)(X, y)

// Editor
int FindImageIndex(EngineState* engineState, Image* image)
{
    for (int i = 0; i < ArrayCapacity(engineState->images); i++)
    {
        if (engineState->images[i] == image)
        {
            return i;
        }
    }
    return -1; // not found
}

// Functions called when deserializing
Entity* MakeEntity(EngineState* engineState, Entity* entity,
    Transform t, bool mesh, int type, int texture1, int texture2, // mesh
    int shader, bool physics = 0, bool kinematic = 0, float mass = 1, // physics
    EffectData effectData = {}, int group = 0, int surfaceType = 0)
{
    *entity = {};
    entity->initialized = true;

    // Transform
    entity->transform = t;

    // Mesh
    entity->mesh = mesh;
    entity->type = (EntityType)type;
    entity->texture1 = engineState->images[texture1];
    entity->texture2 = engineState->images[texture2];
    entity->shaderIndex = shader;

    // Physics
    entity->physics = physics;
    entity->kinematic = kinematic;
    entity->mass = mass;
    entity->friction = 0.01f;
    entity->drag = 0.005f;
    entity->effect = effectData;

    entity->color = float3(1, 1, 1);
    entity->group = group;

    entity->surfaceType = (SurfaceType)surfaceType;

    // Particle
    //entity->spawnRate = spawnRate;
    //entity->spawnRadius = spawnRadius;
    //entity->minSpeed = minSpeed;
    //entity->maxSpeed = maxSpeed;
    //entity->startSize = startSize;
    //entity->endSize = endSize;
    return entity;
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


void WriteEntity(EngineState* engineState, char* target, Entity* entity, int index)
{
    int max = ArrayCapacity(engineState->saveBuffer);
    Append(target, "MakeEntity", max);
    //if (entity->mesh)
    //{
    //    if (entity->type == EntityType_Box)
    //    else
    //        Append(target, "MakeEntity_Sphere", max);
    //}
    Append(target, "(engineState, &engineState->Entities[", max);
    Append(target, index, max);
    Append(target, "], ", max);

    // Transform
    Append(target, "transform(", max);
    CoerceAppend(target, entity->transform.position, max);
    Append(target, ", ", max);
    CoerceAppend(target, entity->transform.forward, max);
    Append(target, ", ", max);
    CoerceAppend(target, entity->transform.up, max);
    Append(target, ", ", max);
    CoerceAppend(target, entity->transform.scale, max);
    Append(target, ")", max);

    // Mesh
    AppendArgument(max, target, entity->mesh);
    AppendArgument(max, target, entity->type);
    AppendArgument(max, target, FindImageIndex(engineState, entity->texture1));
    AppendArgument(max, target, FindImageIndex(engineState, entity->texture2));
    AppendArgument(max, target, entity->shaderIndex);

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

    AppendArgument(max, target, entity->group);
    AppendArgument(max, target, entity->surfaceType);

    Append(target, ");", max);
    Append(target, "\n", max);
}

void LoadLevel(EngineState* engineState, int level)
{
    for (int j = 0; j < ArrayCapacity(engineState->entities); j++)
    {
        engineState->entities[j].initialized = false;
    }
    engineState->currentLevel = level;
    if (level == 0)
    {
#include "Level0.cpp";
    }
    else if (level == 1)
    {
#include "Level1.cpp";
    }
    else if (level == 2)
    {
#include "Level2.cpp";
    }
    else if (level == 3)
    {
#include "Level3.cpp";
    }

    // Wipe the undo buffer
    //for (int i = 0; i < ArrayCapacity(engineState->EntityUndoBuffer); i++)
    //{
    //    for (int j = 0; j < ArrayCapacity(engineState->Entities); j++)
    //    {
    //        engineState->Entities[j] = engineState->Entities[j];
    //    }
    //}

}

void SaveLevel(GameMemory* memory, EngineState* engineState, int level)
{
    for (int i = 0; i < ArrayCapacity(engineState->saveBuffer); i++)
    {
        engineState->saveBuffer[i] = 0;
    }
    for (int i = 0; i < ArrayCapacity(engineState->entities); i++)
    {
        Entity* entity = &engineState->entities[i];
        if (!entity->initialized)
            continue;
        WriteEntity(engineState, engineState->saveBuffer, entity, i);
    }
    char LevelName[11] = {};
    Append(LevelName, "Level");
    Append(LevelName, level);
    Append(LevelName, ".cpp");

    memory->platformWriteFile(LevelName, &engineState->saveBuffer, StringLength(engineState->saveBuffer));
}


void UpdateEditorCamera(EngineState* engineState, EngineSaveState* engineSaveState, GameInput* Input)
{
    engineSaveState->camera = rotate(engineSaveState->camera, float3(0, 0, 1), -Input->MouseXDelta * 0.005f);
    engineSaveState->camera = rotate(engineSaveState->camera, engineSaveState->camera.right, -Input->MouseYDelta * 0.005f);

    int X = Input->W ? 1 : 0 + Input->S ? -1 : 0;
    int Y = Input->D ? 1 : 0 + Input->A ? -1 : 0;
    int Z = Input->E ? 1 : 0 + Input->Q ? -1 : 0;

    float3 Movement = float3(0, 0, 0);
    Movement = Movement + engineSaveState->camera.forward * X;
    Movement = Movement + engineSaveState->camera.right * Y;
    Movement = Movement + engineSaveState->camera.up * Z;
    if (length(Movement) > 0)
        Movement = normalize(Movement);
    float speed = 4;
    if (Input->Shift)
        speed = 16;
    engineSaveState->camera.position += Movement * Input->deltaTime * speed;

}

void ClearSelection(EngineState* engineState)
{
    for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
    {
        engineState->selectedEntities[i] = 0;
    }
}

void SelectEntity(EngineState* engineState, Entity* entity)
{
    // Find an empty slot in the list of selected entites and add our entity there.

    for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
    {
        if (engineState->selectedEntities[i] == entity)
            break;
        if (!Valid(engineState->selectedEntities[i]))
        {
            engineState->selectedEntities[i] = entity;
            
            break;
        }
    }
}

void SelectGroup(EngineState* engineState, int group)
{
    if (group == 0)
        return;

    for (int i = 0; i < ArrayCapacity(engineState->entities); i++)
    {
        Entity* e = &engineState->entities[i];

        if (Valid(e) && engineState->entities[i].group == group)
        {
            SelectEntity(engineState, e);
        }
    }
}

// find a free and avilable group
int FindFreeGroup(EngineState* engineState)
{
    // If multiple entites are selected, find an empty group slot and group them togeather.
    bool GroupOccupied[100] = {};
    int FreeGroup = 0;
    for (int i = 0; i < ArrayCapacity(engineState->entities); i++)
    {
        Entity* e = &engineState->entities[i];
        if (Valid(e))
        {
            GroupOccupied[e->group] = true;
        }
    }
    for (int i = 0; i < ArrayCapacity(GroupOccupied); i++)
    {
        if (GroupOccupied[i] == false)
        {
            FreeGroup = i;
            break;
        }
    }
    return FreeGroup;
}

void DeselectEntity(EngineState* engineState, Entity* entity)
{
    // loop through the list of selected entites, if our one is found, set it to 0.
    for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
    {
        if (engineState->selectedEntities[i] == entity)
        {
            engineState->selectedEntities[i] = 0;
            break;
        }
    }
}

void UpdateEditor(GameMemory* memory, EngineState* engineState, EngineSaveState* engineSaveState, GameInput* Input, VideoBuffer* videoBuffer, float2* UIPos)
{
    // EDITOR STUFF
    //Text(engineState, videoBuffer, &UIPos, "Editor Controls");
    //Text(engineState, videoBuffer, &UIPos, "  W Translate.");
    //Text(engineState, videoBuffer, &UIPos, "  E Rotate.");
    //Text(engineState, videoBuffer, &UIPos, "  R Scale.");
    //Text(engineState, videoBuffer, &UIPos, "  Delete to delete selection.");
    //Text(engineState, videoBuffer, &UIPos, "  Ctrl + D to duplicate selection.");
    //Text(engineState, videoBuffer, &UIPos, "  Left click to select.");
    //Text(engineState, videoBuffer, &UIPos, "  Shift + Left click add to selection.");
    //Text(engineState, videoBuffer, &UIPos, "  Ctrl + Left click remove from selection.");

    for (int i = 0; i < ArrayCapacity(engineState->entities); i++)
    {
        Entity* e = &engineState->entities[i];
        if (!Valid(e))
            continue;

        float3 ScreenPos3D = ToScreenPos(videoBuffer, e->transform.position, engineState->camera);
        if (ScreenPos3D.z < 0)
            continue;

        if (e->type <= 2)
            continue;

        float2 ScreenPos = float2(ScreenPos3D.x, (ScreenPos3D.y * 0.5) + 100);

        Text(engineState, videoBuffer, ScreenPos, EntityTypeNames[e->type]);
    }

    Entity* GizmoEntity = 0;
    for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
    {
        Entity* e = engineState->selectedEntities[i];
        if (!Valid(e))
            continue;

        GizmoEntity = e;
    }

    if (!Input->MouseRight)
    {
        if (Input->W)
            engineState->gizmoState = 0;
        if (Input->E)
            engineState->gizmoState = 1;
        if (Input->R)
            engineState->gizmoState = 2;
    }

    float3 MoveOffset = float3(0, 0, 0);
    float3 ScaleOffset = float3(0, 0, 0);
    Rotation RotationOffset;
    RotationOffset.Axis = float3(1, 0, 0);
    RotationOffset.Angle = 0;
    if (Valid(GizmoEntity))
    {
        if (engineState->gizmoState == 0)
        {
            MoveOffset = MoveGizmo(engineState, Input, videoBuffer, engineState->camera, GizmoEntity->transform, engineSaveState->gizmoWorldSpace);
        }
        else if (engineState->gizmoState == 1)
        {
            RotationOffset = RotateGizmo(engineState, Input, videoBuffer, engineState->camera, GizmoEntity->transform, engineSaveState->gizmoWorldSpace);
        }
        else if (engineState->gizmoState == 2)
        {
            ScaleOffset = ScaleGizmo(engineState, Input, videoBuffer, engineState->camera, GizmoEntity->transform);
        }
    }
    engineState->grabbing = engineState->pickedSliderID != -1;

    for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
    {
        Entity* e = engineState->selectedEntities[i];
        if (Valid(e))
        {
            e->transform.position += MoveOffset;
            e->transform.scale += ScaleOffset;
            e->transform = rotate(e->transform, RotationOffset.Axis, RotationOffset.Angle);

            if (e != GizmoEntity)
            {
                float3 RelativePos = e->transform.position - GizmoEntity->transform.position;
                RelativePos = RotateAroundAxis(RelativePos, RotationOffset.Axis, RotationOffset.Angle);
                RelativePos += GizmoEntity->transform.position;
                e->transform.position = RelativePos;
            }
        }
    }

    if (Input->Ctrl && Input->GDown)
    {
        engineState->operationHappened = true;
        if (ArrayCapacity(engineState->selectedEntities) == 1) // If a single entity is selected, clear its group
        {
            Entity* e = engineState->selectedEntities[0];
            if (Valid(e))
            {
                e->group = 0;
            }
        }
        else
        {
            int FreeGroup = FindFreeGroup(engineState);

            for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
            {
                Entity* e = engineState->selectedEntities[i];
                if (Valid(e))
                {
                    e->group = FreeGroup;
                }
            }
        }
    }
    if (Input->Shift && Input->GDown)
    {
        engineState->operationHappened = true;
        for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
        {
            Entity* e = engineState->selectedEntities[i];
            if (Valid(e))
            {
                e->group = 0;
            }
        }
    }

    if (Input->DeleteDown)
    {
        engineState->operationHappened = true;
        for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
        {
            Entity* e = engineState->selectedEntities[i];
            if (Valid(e))
            {
                *e = {};
            }
        }
    }


    if (Input->Ctrl && Input->DDown)
    {
        int FreeGroup = FindFreeGroup(engineState);
        for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
        {
            Entity* selectedEntity = engineState->selectedEntities[i];
            if (!Valid(selectedEntity))
                continue;
            
            for (int j = 0; j < ArrayCapacity(engineState->entities); j++)
            {
                Entity* entity = &engineState->entities[j];
                if (Valid(entity))
                    continue;

                *entity = *selectedEntity; // copy?
                entity->group = FreeGroup;
                break;
            }
        }
    }

    if (!engineState->grabbing)
    {
        // Find closest entity under cursor.
        float3 MouseRay = ScreenPointToRay(videoBuffer, engineState->camera, float2(Input->MouseX, Input->MouseY));
        float ClosestDistance = 99999;
        engineState->hoveredEntity = 0;
        for (int i = 0; i < ArrayCapacity(engineState->entities); i++)
        {
            Entity* e = &engineState->entities[i];
            if (!Valid(e))
                continue;

            float3 Normal;
            float3 LocalPos = WorldToLocal(e->transform, engineState->camera.position);
            float3 LocalDir = WorldToLocalVector(e->transform, MouseRay);

            float HitDepth = RayBoxIntersect(LocalPos, LocalDir, &Normal);
            if (HitDepth > 0)
            {
                if (HitDepth < ClosestDistance)
                {
                    ClosestDistance = HitDepth;
                    engineState->hoveredEntity = e;
                }
            }
        }

        if (Input->MouseLeftDown && !engineState->mouseIsOverUI && Valid(engineState->hoveredEntity))
        {
            if (Input->Shift)
            {
                SelectEntity(engineState, engineState->hoveredEntity);
                SelectGroup(engineState, engineState->hoveredEntity->group);
            }
            else if (Input->Ctrl)
            {
                DeselectEntity(engineState, engineState->hoveredEntity);
            }
            else
            {
                ClearSelection(engineState);
                SelectEntity(engineState, engineState->hoveredEntity);
                SelectGroup(engineState, engineState->hoveredEntity->group);
            }
        }
        if (Input->EscapeDown)
        {
            ClearSelection(engineState);
        }
    }

    engineState->mouseIsOverUI = false;



    float2 MenuPlacement = float2(videoBuffer->Width - 120, 5);

    for (int i = 0; i < 4; i++)
    {
        char LoadText[100] = "Load Level ";
        Append(LoadText, i);
        if (Button(engineState, Input, videoBuffer, &MenuPlacement, LoadText))
        {
            LoadLevel(engineState,i);
        }
    }

    if (Button(engineState, Input, videoBuffer, &MenuPlacement, "Save"))
    {
        SaveLevel(memory, engineState, engineState->currentLevel);
    }
    if (Input->Ctrl && Input->SDown)
    {
        SaveLevel(memory, engineState, engineState->currentLevel);
    }

    char SpaceText[10] = {};
    engineSaveState->gizmoWorldSpace ? Append(SpaceText, "Local Space") : Append(SpaceText, "World Space");
    if (Button(engineState, Input, videoBuffer, &MenuPlacement, SpaceText))
    {
        engineSaveState->gizmoWorldSpace = !engineSaveState->gizmoWorldSpace;
    }

    Text(engineState, videoBuffer, &MenuPlacement, "");
    Text(engineState, videoBuffer, &MenuPlacement, "Entities.");
    for (int i = 0; i < ArrayCapacity(engineState->entities); i++)
    {
        Entity* e = &engineState->entities[i];

        if (!Valid(e))
            continue;

        char LoadText[100] = "Load Level ";
        if (Button(engineState, Input, videoBuffer, &MenuPlacement, EntityTypeNames[e->type]))
        {
            ClearSelection(engineState);
            engineState->selectedEntities[0] = e;
        }
    }

    Text(engineState, videoBuffer, UIPos, " ");
    
    if (Button(engineState, Input, videoBuffer, UIPos, "Snap Rotation"))
    {
        engineState->operationHappened = true;
        for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
        {
            Entity* entity = engineState->selectedEntities[i];
            if (!Valid(entity))
                continue;

            for (int i = 0; i < 5; i++)
            {
                entity->transform.right.x = MoveTowards(entity->transform.right.x, 0, 0.5f);
                entity->transform.right.y = MoveTowards(entity->transform.right.y, 0, 0.5f);
                entity->transform.right.z = MoveTowards(entity->transform.right.z, 0, 0.5f);
                entity->transform.right = normalize(entity->transform.right);

                entity->transform.forward.x = MoveTowards(entity->transform.forward.x, 0, 0.5f);
                entity->transform.forward.y = MoveTowards(entity->transform.forward.y, 0, 0.5f);
                entity->transform.forward.z = MoveTowards(entity->transform.forward.z, 0, 0.5f);
                entity->transform.forward = normalize(entity->transform.forward);

                entity->transform.up.x = MoveTowards(entity->transform.up.x, 0, 0.5f);
                entity->transform.up.y = MoveTowards(entity->transform.up.y, 0, 0.5f);
                entity->transform.up.z = MoveTowards(entity->transform.up.z, 0, 0.5f);
                entity->transform.up = normalize(entity->transform.up);
            }
        }
    }
    if (Button(engineState, Input, videoBuffer, UIPos, "Snap"))
    {
        engineState->operationHappened = true;
        for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
        {
            Entity* entity = engineState->selectedEntities[i];
            if (!Valid(entity))
                continue;

            entity->transform.scale = round(entity->transform.scale);
            if (entity->transform.scale.x <= 1) entity->transform.scale.x = 1;
            if (entity->transform.scale.y <= 1) entity->transform.scale.y = 1;
            if (entity->transform.scale.z <= 1) entity->transform.scale.z = 1;

            float3 HalfScale = float3(0, 0, 0);
            HalfScale += entity->transform.right * (entity->transform.scale.x * 0.5);
            HalfScale += entity->transform.forward * (entity->transform.scale.y * 0.5);
            HalfScale += entity->transform.up * (entity->transform.scale.z * 0.5);

            entity->transform.position = round(entity->transform.position - HalfScale) + HalfScale;

            for (int i = 0; i < 5; i++)
            {
                entity->transform.right.x = MoveTowards(entity->transform.right.x, 0, 0.5f);
                entity->transform.right.y = MoveTowards(entity->transform.right.y, 0, 0.5f);
                entity->transform.right.z = MoveTowards(entity->transform.right.z, 0, 0.5f);
                entity->transform.right = normalize(entity->transform.right);

                entity->transform.forward.x = MoveTowards(entity->transform.forward.x, 0, 0.5f);
                entity->transform.forward.y = MoveTowards(entity->transform.forward.y, 0, 0.5f);
                entity->transform.forward.z = MoveTowards(entity->transform.forward.z, 0, 0.5f);
                entity->transform.forward = normalize(entity->transform.forward);

                entity->transform.up.x = MoveTowards(entity->transform.up.x, 0, 0.5f);
                entity->transform.up.y = MoveTowards(entity->transform.up.y, 0, 0.5f);
                entity->transform.up.z = MoveTowards(entity->transform.up.z, 0, 0.5f);
                entity->transform.up = normalize(entity->transform.up);
            }
        }
    }

    if (Button(engineState, Input, videoBuffer, UIPos, "Reset Selected"))
    {
        engineState->operationHappened = true;
        for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
        {
            Entity* entity = engineState->selectedEntities[i];
            if (!Valid(entity))
                continue;

            //entity->t.scale = float3(1, 1, 1);
            entity->transform.scale = float3(0.5, 0.5, 0.5);

            entity->transform.right = float3(1, 0, 0);
            entity->transform.forward = float3(0, 1, 0);
            entity->transform.up = float3(0, 0, 1);
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
                entity->color = float3(1, 1, 1);
            }
        }
    }
    if (Button(engineState, Input, videoBuffer, UIPos, "Play Effect"))
    {
        for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
        {
            Entity* entity = engineState->selectedEntities[0];
            if (!Valid(entity))
                continue;

            entity->effect.startTime = engineState->gameTime;
        }
    }

    Text(engineState, videoBuffer, UIPos, " ");
    Entity* entity = engineState->selectedEntities[0];
    if (Valid(entity))
    {
        Text(engineState, videoBuffer, UIPos, "Position: ", entity->transform.position);
        Text(engineState, videoBuffer, UIPos, "Group: ", entity->group);
        Text(engineState, videoBuffer, UIPos, EntityTypeNames[entity->type]);
        entity->type = (EntityType)(int)round(Slider(engineState, Input, videoBuffer, UIPos, entity->type, 0, ArrayCapacity(EntityTypeNames)));

        Text(engineState, videoBuffer, UIPos, ShaderNames[entity->shaderIndex]);
        int LastShaderIndex = entity->shaderIndex;
        entity->shaderIndex = Slider(engineState, Input, videoBuffer, UIPos, entity->shaderIndex, 0, ArrayCapacity(ShaderNames));
        if (LastShaderIndex != entity->shaderIndex)
        {
            for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
            {
                if (!Valid(engineState->selectedEntities[i]))
                    continue;
                engineState->selectedEntities[i]->shaderIndex = entity->shaderIndex;
            }
        }

        Text(engineState, videoBuffer, UIPos, "Texture 1: ");
        int texture1 = FindImageIndex(engineState, entity->texture1);
        int LastTexture1 = texture1;
        texture1 = Slider(engineState, Input, videoBuffer, UIPos, texture1, 0, engineState->currentImage);
        entity->texture1 = engineState->images[texture1];
        if (LastTexture1 != texture1)
        {
            for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
            {
                if (!Valid(engineState->selectedEntities[i]))
                    continue;
                engineState->selectedEntities[i]->texture1 = entity->texture1;
            }
        }

        int EntityCount = 0;
        for (int i = 0; i < ArrayCapacity(engineState->entities); i++)
        {
            if (Valid(engineState->entities[i]))
                EntityCount++;
        }

        Text(engineState, videoBuffer, UIPos, "Entity Count: ", EntityCount);

        Text(engineState, videoBuffer, UIPos, "Texture 2: ");
        int texture2 = FindImageIndex(engineState, entity->texture2);
        int LastTexture2 = texture2;
        texture2 = Slider(engineState, Input, videoBuffer, UIPos, texture2, 0, engineState->currentImage);
        entity->texture2 = engineState->images[texture2];
        if (LastTexture2 != texture2)
        {
            for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
            {
                if (!Valid(engineState->selectedEntities[i]))
                    continue;
                engineState->selectedEntities[i]->texture2 = entity->texture2;
            }
        }

        if (entity->type == EntityType_Sphere || entity->type == EntityType_Box)
        {
            Text(engineState, videoBuffer, UIPos, surfaceTypeNames[entity->surfaceType]);
            int LastShaderIndex = entity->surfaceType;
            entity->surfaceType = (SurfaceType)(int)round(Slider(engineState, Input, videoBuffer, UIPos, entity->surfaceType, 0, ArrayCapacity(surfaceTypeNames)));
            if (LastShaderIndex != entity->surfaceType)
            {
                for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
                {
                    if (!Valid(engineState->selectedEntities[i]))
                        continue;
                    engineState->selectedEntities[i]->surfaceType = entity->surfaceType;
                }
            }

            entity->physics = Toggle(engineState, Input, videoBuffer, UIPos, entity->physics, "Physics");
            entity->kinematic = Toggle(engineState, Input, videoBuffer, UIPos, entity->kinematic, "Kinematic");
            if (entity->kinematic || !entity->physics)
            {
                entity->angularVelocity = 0;
                entity->velocity = float3(0, 0, 0);
            }
            Text(engineState, videoBuffer, UIPos, "Mass: ", entity->mass);
            entity->mass = Slider(engineState, Input, videoBuffer, UIPos, entity->mass, 0, 4);

        }
        else if (entity->type == EntityType_Effect)
        {
            EffectData* effect = &entity->effect;

            float2 UIPos2 = float2(160 * 2, videoBuffer->Height - 260);
            float2 UIPos3 = float2(160, videoBuffer->Height - 260);

            effect->burst = Toggle(engineState, Input, videoBuffer, &UIPos3, effect->burst, "Burst");


            Text(engineState, videoBuffer, &UIPos3, "spawnRate: ", effect->spawnRate);
            effect->spawnRate = Slider(engineState, Input, videoBuffer, &UIPos3, effect->spawnRate, 0, 25);


            Text(engineState, videoBuffer, &UIPos3, "spawnRadius: ", effect->spawnRadius);
            effect->spawnRadius = Slider(engineState, Input, videoBuffer, &UIPos3, effect->spawnRadius, 0, 4);

            Text(engineState, videoBuffer, &UIPos3, "drag: ", effect->drag);
            effect->drag = Slider(engineState, Input, videoBuffer, &UIPos3, effect->drag, 0, 4);

            Text(engineState, videoBuffer, &UIPos3, "gravity: ", effect->gravity);
            effect->gravity = Slider(engineState, Input, videoBuffer, &UIPos3, effect->gravity, -25, 0);


            Text(engineState, videoBuffer, &UIPos3, "startSize: ", effect->startSize);
            effect->startSize = Slider(engineState, Input, videoBuffer, &UIPos3, effect->startSize, 0, 4);

            Text(engineState, videoBuffer, &UIPos3, "endSize: ", effect->endSize);
            effect->endSize = Slider(engineState, Input, videoBuffer, &UIPos3, effect->endSize, 0, 4);


            Text(engineState, videoBuffer, &UIPos2, "minSpeed.x: ", effect->minSpeed.x);
            effect->minSpeed.x = Slider(engineState, Input, videoBuffer, &UIPos2, effect->minSpeed.x, -1, 1);
            Text(engineState, videoBuffer, &UIPos2, "minSpeed.y: ", effect->minSpeed.y);
            effect->minSpeed.y = Slider(engineState, Input, videoBuffer, &UIPos2, effect->minSpeed.y, -1, 1);
            Text(engineState, videoBuffer, &UIPos2, "minSpeed.z: ", effect->minSpeed.z);
            effect->minSpeed.z = Slider(engineState, Input, videoBuffer, &UIPos2, effect->minSpeed.z, -1, 1);


            Text(engineState, videoBuffer, &UIPos2, "maxSpeed.x: ", effect->maxSpeed.x);
            effect->maxSpeed.x = Slider(engineState, Input, videoBuffer, &UIPos2, effect->maxSpeed.x, -1, 1);
            Text(engineState, videoBuffer, &UIPos2, "maxSpeed.y: ", effect->maxSpeed.y);
            effect->maxSpeed.y = Slider(engineState, Input, videoBuffer, &UIPos2, effect->maxSpeed.y, -1, 1);
            Text(engineState, videoBuffer, &UIPos2, "maxSpeed.z: ", effect->maxSpeed.z);
            effect->maxSpeed.z = Slider(engineState, Input, videoBuffer, &UIPos2, effect->maxSpeed.z, -1, 1);


            Text(engineState, videoBuffer, &UIPos2, "minLifetime: ", effect->minLifetime);
            effect->minLifetime = Slider(engineState, Input, videoBuffer, &UIPos2, effect->minLifetime, 0, 4);

            Text(engineState, videoBuffer, &UIPos2, "maxLifetime: ", effect->maxLifetime);
            effect->maxLifetime = Slider(engineState, Input, videoBuffer, &UIPos2, effect->maxLifetime, 0, 4);
        }
    }


    int UndoBufferLength = ArrayCapacity(engineState->entityUndoBuffer) - 1;

    if (engineState->operationHappened)
    {
        engineState->undoSteps++; // add another undo step
        if (engineState->undoSteps > UndoBufferLength)
            engineState->undoSteps = UndoBufferLength;

        engineState->operationHappened = false;
        engineState->undoIndex++;
        if (engineState->undoIndex < 0)
            engineState->undoIndex = UndoBufferLength;
        if (engineState->undoIndex > UndoBufferLength)
            engineState->undoIndex = 0;
        for (int i = 0; i < ArrayCapacity(engineState->entities); i++)
        {
            engineState->entityUndoBuffer[engineState->undoIndex][i] = engineState->entities[i];
        }
    }
    // Undo
    if (Input->Ctrl && Input->ZDown)
    {
        if (engineState->undoSteps > 0) // there are undo steps left
        {
            engineState->undoIndex--;
            if (engineState->undoIndex < 0)
                engineState->undoIndex = UndoBufferLength;
            if (engineState->undoIndex > UndoBufferLength)
                engineState->undoIndex = 0;

            for (int i = 0; i < ArrayCapacity(engineState->entities); i++)
            {
                engineState->entities[i] = engineState->entityUndoBuffer[engineState->undoIndex][i];
            }
        }
        engineState->undoSteps--;
        if (engineState->undoSteps <= 0)
            engineState->undoSteps = 0;
    }
}


// Game

enum MenuState
{
    MenuState_MainMenu,
    MenuState_Settings,
    MenuState_Playing,
    MenuState_Paused,
    MenuState_WinScreen,
    MenuState_LoseScreen, 
    MenuState_ChapterSelect,
    MenuState_Max,
};

char MenuStateNames[MenuState_Max][100] = {
    "MainMenu",
    "Settings",
    "Playing",
    "Paused",
    "WinScreen",
    "LoseScreen",
    "ChapterSelect"
};

struct BallGameState
{
    Entity* player;
    Entity* win;

    Entity* SpawnPoint;
    Entity* Checkpoints[10];
    Entity* CheckpointEffects[10];
    Entity* CurrentCheckpoint;
    Entity* LastCurrentCheckpoint;
    
    Image* ballTexture;
    Image* minecraft;
    Image* extraBallTexture;

    Image* soapTexture;
    Image* soapParticleTexture;
    Image* particleTimeTexture;
    Image* particleFireTexture;

    int health = 0;
    float score = 0;
    MenuState menuState;
    int CurrentLevel;

    int HealthPickupSphere;
    Entity* HealthPickupSpheres[10];
    Entity* HealthPickupSphereEffect[10];
    Entity* PickupHealthEffectEntity;

    int TimePickupSphere;
    Entity* TimePickupEditorSphere[10];
    Entity* TimePickupSpheres[10];
    Entity* TimePickupSphereEffect[10];
    Entity* TimePickupSpheresOrbiters[10][10];
    Entity* TimePickupBurstEffect;

    SurfaceType playerSurfaceType;
    SurfaceType lastPlayerSurfaceType;

    Sound* HitSoundMetal;
    Sound* HitSoundStone;
    Sound* HitSoundWood;

    Sound* RollSoundMetal;
    Sound* RollSoundStone;
    Sound* RollSoundWood;

    uint8 output[2479*2479];

};


// Persits between restarts
struct BallGameSaveState
{
    int UnlockedLevels;
    int Level1HighScore = 0;
    int Level2HighScore = 0;
    int Level3HighScore = 0;
};

Entity* GetFreeEntitySlot(EngineState* engineState)
{
    for (int j = 0; j < ArrayCapacity(engineState->entities); j++)
    {
        if (!Valid(engineState->entities[j]))
            return &engineState->entities[j];
    }

    // no entities left
    Assert(false);
    return 0;
}

Entity* MakeEntity(EngineState* engineState, float3 pos, bool sphere, bool physics, Image* texture, ShaderType shaderType, float scale = 1.0f)
{
    int texture1 = FindImageIndex(engineState, texture);
    Transform t = transform_pos(pos);
    t.scale = float3(1, 1, 1) * scale;
    return MakeEntity(engineState, GetFreeEntitySlot(engineState), t, true, EntityType_Sphere, texture1, 0, shaderType, physics, false);
}

Entity* MakeEffect(EngineState* engineState, float3 pos, Image* texture, EffectData effectData, bool additive = false)
{
    int texture1 = FindImageIndex(engineState, texture);
    Entity* result = MakeEntity(engineState, GetFreeEntitySlot(engineState), transform_pos(pos), false, EntityType_Effect, texture1, 0, additive ? ShaderType_Texture : ShaderType_Sphere, false, false, 1.0f, effectData);
    
    result->effect.startTime = engineState->gameTime;
    return result;
}

Entity* SpawnPlayer(EngineState* engineState, BallGameState* gameState)
{
    Entity* ent = MakeEntity(engineState, gameState->SpawnPoint->transform.position, true, true, gameState->ballTexture, ShaderType_Sphere);
    ent->transform.scale = float3(0.4f, 0.4f, 0.4f);
    ent->mass = 1.0;
    return ent;
}

void InitWorld(EngineState* engineState, BallGameState* gameState)
{
    int CheckpointCount = 0;
    for (int j = 0; j < ArrayCapacity(engineState->entities); j++)
    {
        if (Valid(engineState->entities[j]) && engineState->entities[j].type == EntityType_Spawn)
        {
            gameState->SpawnPoint = &engineState->entities[j];
        }
        if (Valid(engineState->entities[j]) && engineState->entities[j].type == EntityType_Checkpoint)
        {
            gameState->Checkpoints[CheckpointCount] = &engineState->entities[j];
            CheckpointCount++;
        }
    }

    for (int i = 0; i < ArrayCapacity(gameState->Checkpoints); i++)
    {
        Entity* e = gameState->Checkpoints[i];
        if (!Valid(e))
            continue;
        gameState->CheckpointEffects[i] = MakeEffect(engineState, e->transform.position, gameState->particleFireTexture, ctor_EffectData(10, false, 0.2, 1, float3(0, 0, 0.2), float3(0, 0, 0.4), 0.5f, 0.5f, 2.0f, 0.0f, 0.0), true);
    }

    gameState->CurrentCheckpoint = 0;
    
    gameState->PickupHealthEffectEntity = MakeEffect(engineState, float3(0, 0, 0), gameState->soapParticleTexture, ctor_EffectData(6, true, 0.25f, 1, float3(-1, -1, -1) * 0.25, float3(1, 1, 1) * 0.25, 0.1f, 0.5f, 0.2f, 2.0f, 0.0), true);
    gameState->TimePickupBurstEffect = MakeEffect(engineState, float3(0, 0, 0), gameState->extraBallTexture, ctor_EffectData(6, true, 0.25f, 1, float3(-1, -1, -1) * 0.25, float3(1, 1, 1) * 0.25, 0.1f, 0.2f, 0.5f, 0.0f, 0.0), false);
    gameState->HealthPickupSphere = 0;
    gameState->TimePickupSphere = 0;

    for (int i = 0; i < ArrayCapacity(engineState->entities); i++)
    {
        Entity* e = &engineState->entities[i];

        if (!Valid(e))
            continue;

        if (e->type == EntityType_Health)
        {
            gameState->HealthPickupSpheres[gameState->HealthPickupSphere] = MakeEntity(engineState, e->transform.position, true, false, gameState->soapTexture, ShaderType_Additive);
            gameState->HealthPickupSphereEffect[gameState->HealthPickupSphere] = MakeEffect(engineState, e->transform.position, gameState->extraBallTexture, ctor_EffectData(1, false, 0.01, 1, float3(0, 0, 0), float3(0, 0, 0), 2.0f, 2.0f, 1.25f, 1.25f, 0.0));
            gameState->HealthPickupSphere++;
        }
        else if (e->type == EntityType_Time)
        {
            gameState->TimePickupEditorSphere[gameState->TimePickupSphere] = e;
            gameState->TimePickupSphereEffect[gameState->TimePickupSphere] = MakeEffect(engineState, e->transform.position, gameState->extraBallTexture, ctor_EffectData(1, false, 0.01, 1, float3(0, 0, 0), float3(0, 0, 0), 2.0f, 2.0f, 1.25f, 1.25f, 0.0));
            
            gameState->TimePickupSpheres[gameState->TimePickupSphere] = MakeEntity(engineState, e->transform.position, true, false, gameState->particleTimeTexture, ShaderType_Additive, 0.5f);
            for (int j = 0; j < ArrayCapacity(gameState->TimePickupSpheres); j++)
            {
                gameState->TimePickupSpheresOrbiters[gameState->TimePickupSphere][j] = MakeEffect(engineState, e->transform.position, gameState->extraBallTexture, ctor_EffectData(1, false, 0.01, 1, float3(0, 0, 0), float3(0, 0, 0), 2.0f, 2.0f, 0.5f, 0.5f, 0.0));
            }
            
            gameState->TimePickupSphere++;
        }
    }
}

void UpdateWorld(EngineState* engineState, BallGameState* gameState)
{
    for (int i = 0; i < ArrayCapacity(gameState->HealthPickupSpheres); i++)
    {
        Entity* e = gameState->HealthPickupSpheres[i];

        if (!Valid(e))
            continue;

        e->transform.scale = 0.5f + sin(engineState->gameTime* 2.0f) * 0.05 * float3(1, 1, 1);
    }

    for (int i = 0; i < ArrayCapacity(gameState->TimePickupSpheres); i++)
    {
        Entity* e = gameState->TimePickupSpheres[i];

        if (!Valid(e))
            continue;

        for (int j = 0; j < ArrayCapacity(gameState->TimePickupSpheres); j++)
        {
            float3 a = normalize(float3(rand(float2(i, j)), rand(float2(i, j) * 2), rand(float2(i, j) * 3)));
            gameState->TimePickupSpheresOrbiters[i][j]->transform.position = e->transform.position + normalize(RotateAroundAxis(float3(0, 0, 1), a, (engineState->gameTime*4) + j)) * 0.5f;
        }
    }
}

void PickupHealth(EngineState* engineState, BallGameState* gameState, int i)
{
    gameState->PickupHealthEffectEntity->transform.position = gameState->HealthPickupSpheres[i]->transform.position;
    gameState->PickupHealthEffectEntity->effect.startTime = engineState->gameTime;

    (*gameState->HealthPickupSpheres[i]) = {};
    (*gameState->HealthPickupSphereEffect[i]) = {};

    gameState->health++;
}

void PickupTime(EngineState* engineState, BallGameState* gameState, int i)
{
    (*gameState->TimePickupSpheres[i]) = {};
    (*gameState->TimePickupSphereEffect[i]) = {};
}
void PlayLevel(EngineState* engineState, BallGameState* gameState, int Level)
{
    gameState->menuState = MenuState_Playing;
    LoadLevel(engineState, Level);
    InitWorld(engineState, gameState);
    gameState->player = SpawnPlayer(engineState, gameState);
    gameState->score = 1000;
    gameState->health = 3;
    gameState->CurrentLevel = Level;
}
void ExitToMainMenu(EngineState* engineState, BallGameState* gameState)
{
    gameState->menuState = MenuState_MainMenu;
    LoadLevel(engineState, 0);
    gameState->CurrentLevel = 0;
    gameState->player = 0;
    gameState->score = 0;
    gameState->health = 0;
}
void WinLevel(EngineState* engineState, BallGameState* gameState, BallGameSaveState* gameSaveState)
{
    gameState->menuState = MenuState_WinScreen;
    gameSaveState->UnlockedLevels++;
}

// Called once per frame
void GameUpdate(EngineState* engineState, VideoBuffer* videoBuffer, GameInput* Input, BallGameState* gameState, BallGameSaveState* gameSaveState)
{
    if (gameState->menuState == MenuState_Playing)
    {
        Transform* Camera = &engineState->camera;
        Transform* Player = &gameState->player->transform;

        float NormalizedSpeed = clamp01(length(gameState->player->velocity) / 1.5f);
        float RollingVolume = pow(NormalizedSpeed, 1.5f) * 32.0f;
        if (!gameState->player->grounded)
            RollingVolume = 0;

        Camera->position = Player->position + float3(0, 2, 5);
        Entity* HitEntity = ((Entity*)gameState->player->TouchingEntity);
        if (Valid(HitEntity))
        {
            gameState->playerSurfaceType = HitEntity->surfaceType;
        }
        float hitforce = (length(gameState->player->deIntersectVector) / Input->deltaTime) - 1.5f;
        if (hitforce > 0.0f)
        {
            if (!SoundPlaying(engineState, 1))
            {
                switch (gameState->playerSurfaceType)
                {
                case SurfaceType_Metal:
                {
                    PlaySound(engineState, 1, gameState->HitSoundMetal, 1.0f, false);
                }break;
                case SurfaceType_Stone:
                {
                    PlaySound(engineState, 1, gameState->HitSoundStone, 1.0f, false);
                }break;
                case SurfaceType_Wood:
                {
                    PlaySound(engineState, 1, gameState->HitSoundWood, 1.0f, false);
                }break;
                }
            }
        }
        if (gameState->playerSurfaceType != gameState->lastPlayerSurfaceType)
        {
            gameState->lastPlayerSurfaceType = gameState->playerSurfaceType;
            switch (gameState->playerSurfaceType)
            {
            case SurfaceType_Metal:
            {
                PlaySound(engineState, 0, gameState->RollSoundMetal, 1.0f, true);
            }break;
            case SurfaceType_Stone:
            {
                PlaySound(engineState, 0, gameState->RollSoundStone, 1.0f, true);
            }break;
            case SurfaceType_Wood:
            {
                PlaySound(engineState, 0, gameState->RollSoundWood, 1.0f, true);
            }break;
            }
        }
        SetSoundChannelVolume(engineState, 0, RollingVolume);
        SetSoundChannelVolume(engineState, 1, hitforce);

        *Camera = LookRotation(*Camera, normalize(Player->position - Camera->position), float3(0, 0, 1));

        float3 movementInput = float3(0, 0, 0);
        movementInput.x += (Input->D ? 0 :  1);
        movementInput.x += (Input->A ? 0 : -1);
        movementInput.y += (Input->W ? 0 :  1);
        movementInput.y += (Input->S ? 0 : -1);
        if(length(movementInput) > 0)
            movementInput = normalize(movementInput);

        gameState->player->velocity += movementInput * Input->deltaTime * 0.2f;

        gameState->score -= Input->deltaTime;
        if (gameState->score <= 0)
            gameState->score = 0;

        if (Player->position.z < -4)
        {
            gameState->health--;
            float3 SpawnPos = gameState->SpawnPoint->transform.position;
            if (Valid(gameState->CurrentCheckpoint))
                SpawnPos = gameState->CurrentCheckpoint->transform.position;

            gameState->player->transform.position = SpawnPos;
            gameState->player->velocity = float3(0,0,0);
            gameState->player->angularVelocity = 0;

            if (gameState->health < 0)
            {
                gameState->menuState = MenuState_LoseScreen;
            }
        }

        for (int i = 0; i < ArrayCapacity(gameState->HealthPickupSpheres); i++)
        {
            Entity* e = gameState->HealthPickupSpheres[i];

            if (!Valid(e))
                continue;
            
            if (distance(e->transform.position, gameState->player->transform.position) < 1)
            {
                PickupHealth(engineState, gameState, i);
            }
        }

        // TODO: something better than "t" for transform, find some way to namespace-smash types into eachother.
        // Some entity system, more organized way of managing entities.
        // Some entity array type.

        int CheckpointIndex = -1;
        for (int i = 0; i < ArrayCapacity(gameState->Checkpoints); i++)
        {
            if (!Valid(gameState->Checkpoints[i]))
                continue;

            if (distance(gameState->Checkpoints[i]->transform.position, gameState->player->transform.position) < 1)
            {
                gameState->CurrentCheckpoint = gameState->Checkpoints[i];
                CheckpointIndex = i;
                break;
            }
        }

        if (gameState->CurrentCheckpoint != gameState->LastCurrentCheckpoint)
        {
            if(CheckpointIndex >= 0)
                *gameState->CheckpointEffects[CheckpointIndex] = {};
        }

        if (Valid(gameState->win))
        {
            if (distance(gameState->win->transform.position, gameState->player->transform.position) < 1)
            {
                WinLevel(engineState, gameState, gameSaveState);
            }
        }

        for (int i = 0; i < ArrayCapacity(gameState->TimePickupSpheres); i++)
        {
            Entity* e = gameState->TimePickupSpheres[i];
            if (Valid(e))
            {
                if (distance(e->transform.position, gameState->player->transform.position) < 1)
                {
                    PickupTime(engineState, gameState, i);
                }
            }
            else
            {
                for (int j = 0; j < ArrayCapacity(gameState->TimePickupSpheres); j++)
                {
                    Entity* orbiter = gameState->TimePickupSpheresOrbiters[i][j];
                    if (Valid(orbiter))
                    {
                        orbiter->transform.scale.x -= Input->deltaTime * 1.0f;
                        if (orbiter->transform.scale.x > 0)
                        {
                            // + float3(0, 0, 2.0f)
                            orbiter->transform.position += (normalize(orbiter->transform.position - gameState->TimePickupEditorSphere[i]->transform.position) + float3(0,0,2)) * Input->deltaTime;
                        }
                        else
                        {
                            orbiter->transform.position = lerp(orbiter->transform.position, gameState->player->transform.position, Input->deltaTime * 1.0f);
                            if (distance(orbiter->transform.position, gameState->player->transform.position) < 0.5f)
                            {
                                (*gameState->TimePickupSpheresOrbiters[i][j]) = {};
                                gameState->score += 10.0f;
                                gameState->TimePickupBurstEffect->transform.position = gameState->TimePickupSpheresOrbiters[i][j]->transform.position;
                                gameState->TimePickupBurstEffect->effect.startTime = engineState->gameTime;
                            }
                        }
                    }
                }
            }
        }
    }
    UpdateWorld(engineState, gameState);
}

void GameUpdateUI(EngineState* engineState, VideoBuffer* videoBuffer, GameInput* Input, BallGameState* gameState, BallGameSaveState* gameSaveState, EngineSaveState* engineSaveState)
{
    if (gameState->menuState != MenuState_Playing)
    {
        DarkenBuffer(videoBuffer);
    }
    float2 a = float2(200, 100);
    Text(engineState, videoBuffer, &a, MenuStateNames[gameState->menuState]);
    Text(engineState, videoBuffer, &a, "");
    
    switch (gameState->menuState)
    {
    case MenuState_MainMenu:
    {
        Text(engineState, videoBuffer, &a, "This game is a couple-weeks-long project to learn C");
        Text(engineState, videoBuffer, &a, "and program a game from scratch (no engine, no libraries.)");
        Text(engineState, videoBuffer, &a, "All the textures, sound and game design are shamelessly");
        Text(engineState, videoBuffer, &a, "ripped from an abandonware game called Ballance.");
        Text(engineState, videoBuffer, &a, "");

        Text(engineState, videoBuffer, &a, "Sound only works on chrome, multithreading only works on windows.");
        Text(engineState, videoBuffer, &a, "win32 makes me annoyed, the web makes me infuriated.");
        Text(engineState, videoBuffer, &a, "");

        if (gameSaveState->UnlockedLevels == 0)
        {
            if (Button(engineState, Input, videoBuffer, &a, "Play"))
            {
                PlayLevel(engineState, gameState, 0);
            }
        }
        else
        {
            if (Button(engineState, Input, videoBuffer, &a, "Continue"))
            {
                gameState->menuState = MenuState_ChapterSelect;
            }
        }
        if (Button(engineState, Input, videoBuffer, &a, "Settings"))
        {
            gameState->menuState = MenuState_Settings;
        }

    }break;
    case MenuState_ChapterSelect:
    {
        if (Button(engineState, Input, videoBuffer, &a, "Play Test Level"))
        {
            PlayLevel(engineState, gameState, 0);
        }
        if (Button(engineState, Input, videoBuffer, &a, "Play Level 1"))
        {
            PlayLevel(engineState, gameState, 1);
        }
        if (Button(engineState, Input, videoBuffer, &a, gameSaveState->UnlockedLevels >= 1 ? "Play Level 2" : "Level 2 Locked"))
        {
            if (gameSaveState->UnlockedLevels >= 1)
            {
                PlayLevel(engineState, gameState, 2);
            }
        }
        if (Button(engineState, Input, videoBuffer, &a, gameSaveState->UnlockedLevels >= 2 ? "Play Level 3" : "Level 3 Locked"))
        {
            if (gameSaveState->UnlockedLevels >= 2)
            {
                PlayLevel(engineState, gameState, 3);
            }
        }
        if (Button(engineState, Input, videoBuffer, &a, "Back"))
        {
            ExitToMainMenu(engineState, gameState);
        }
    }break;
    case MenuState_Playing:
    {
        Text(engineState, videoBuffer, &a, "Health: ", gameState->health);
        Text(engineState, videoBuffer, &a, "Time: ", round(gameState->score));
        if (Input->EscapeDown)
        {
            gameState->menuState = MenuState_Paused;
        }
        if (Button(engineState, Input, videoBuffer, &a, "Win Level"))
        {
            WinLevel(engineState, gameState, gameSaveState);
        }
    }break;
    case MenuState_Paused:
    {if (Input->EscapeDown)
    {
        gameState->menuState = MenuState_Playing;
    }
    if (Button(engineState, Input, videoBuffer, &a, "Exit to Main Menu"))
    {
        ExitToMainMenu(engineState, gameState);
    }
    if (Button(engineState, Input, videoBuffer, &a, "Restart Level"))
    {
        PlayLevel(engineState, gameState, gameState->CurrentLevel);
    }
    if (Button(engineState, Input, videoBuffer, &a, "Back"))
    {
        gameState->menuState = MenuState_Playing;
    }
    }break;
    case MenuState_Settings:
    {
        engineSaveState->threadedRendering = Toggle(engineState, Input, videoBuffer, &a, engineSaveState->threadedRendering, "Threaded rendering (only on win32)");

        if (Button(engineState, Input, videoBuffer, &a, "Reset Progress"))
        {
            gameSaveState->UnlockedLevels = 0;
        }
        if (Button(engineState, Input, videoBuffer, &a, "Back"))
        {
            ExitToMainMenu(engineState, gameState);
        }
    }break;
    case MenuState_WinScreen:
    {
        Text(engineState, videoBuffer, &a, "Victory!");
        int highschore = ((int)round(gameState->score)) + gameState->health * 10;
        Text(engineState, videoBuffer, &a, "Score: ", highschore);
        if (Button(engineState, Input, videoBuffer, &a, "Continue"))
        {
            if (gameState->CurrentLevel == 0)
                gameSaveState->Level1HighScore = min(gameSaveState->Level1HighScore, highschore);
            if (gameState->CurrentLevel == 1)
                gameSaveState->Level2HighScore = min(gameSaveState->Level2HighScore, highschore);
            if (gameState->CurrentLevel == 2)
                gameSaveState->Level3HighScore = min(gameSaveState->Level3HighScore, highschore);

            ExitToMainMenu(engineState, gameState);
            gameState->menuState = MenuState_ChapterSelect;
        }

    }break;
    case MenuState_LoseScreen:
    {
        Text(engineState, videoBuffer, &a, "Game over!");
        if (Button(engineState, Input, videoBuffer, &a, "Exit to Main Menu"))
        {
            ExitToMainMenu(engineState, gameState);
        }
        if (Button(engineState, Input, videoBuffer, &a, "Replay"))
        {
            PlayLevel(engineState, gameState, gameState->CurrentLevel);
        }
    }break;
    }
    if (Valid(gameState->player))
    {
        Text(engineState, videoBuffer, &a, "wat", length(gameState->player->deIntersectVector) / Input->deltaTime);
    }
    //Text(engineState, videoBuffer, &a, "position: ", gameState->player->t.position);
    //Text(engineState, videoBuffer, &a, "velocity: ", gameState->player->velocity);
    //Text(engineState, videoBuffer, &a, "angularVelocity: ", gameState->player->angularVelocity);
    //Text(engineState, videoBuffer, &a, "angularVelocityAxis: ", gameState->player->angularVelocityAxis);
}

void ballgame(GameMemory* Memory, EngineState* engineState, EngineSaveState* engineSaveState, GameInput* input, VideoBuffer* videoBuffer, game_sound_output_buffer* SoundBuffer)
{
    BallGameState* gameState = (BallGameState*)((uint8*)Memory->transientStorage + sizeof(EngineState));
    BallGameSaveState* gameSaveState = (BallGameSaveState*)((uint8*)Memory->permanentStorage + sizeof(EngineSaveState));
    if (input->SpaceDown)
    {
        Memory->initialized = false;
        for (int i = 0; i < Memory->transientStorageSize; i++)
        {
            ((char*)Memory->transientStorage)[i] = 0;
        }
    }

    if (input->Ctrl && input->SpaceDown)
    {
        engineSaveState->initialized = false;
        for (int i = 0; i < Memory->permanentStorageSize; i++)
        {
            ((char*)Memory->permanentStorage)[i] = 0;
        }
    }
    engineState->profilingData.memory = Memory;
    engineState->profilingData.threading = engineSaveState->threadedRendering;

    if (!engineSaveState->initialized)
    {
        engineSaveState->initialized = true;
        engineSaveState->camera = transform(float3(0, -5, 0), float3(0, 1, 0), float3(0, 0, 1), float3(1, 1, 1));
        engineState->camera = engineSaveState->camera;
        engineSaveState->screenThreads = 8;
        engineSaveState->threadedRendering = false;
        engineSaveState->gameViewEnabled = true;
        engineSaveState->profilingEnabled = false;
    }

    float SphereRadius = 0.5;

    if (!Memory->initialized) // Happens on startup and when the user presses space
    {
        Memory->initialized = true;

        engineState->platformReadImageIntoMemory = Memory->platformReadImageIntoMemory;
        engineState->platformReadSoundIntoMemory = Memory->platformReadSoundIntoMemory;
        engineState->platformPrint = Memory->platformPrint;
        engineState->platformSetForcefeedbackEffect = Memory->platformSetForcefeedbackEffect;

        engineState->gameTime = 0;
        InitializeArena(&engineState->staticAssetsArena, Megabytes(12), (uint8*)Memory->transientStorage + sizeof(EngineState) + sizeof(BallGameState) + Kilobytes(100));
        //InitializeArena(&engineState->soundArena, Megabytes(12), (uint8*)Memory->transientStorage + sizeof(EngineState) + sizeof(BallGameState) + Kilobytes(100) + Megabytes(12));
        uint64 size = (uint64)(sizeof(EngineState) + sizeof(BallGameState) + Kilobytes(100) + Megabytes(12) + Megabytes(12));
        Assert(size <= Memory->transientStorageSize);
        
        //gameState->minecraft = LoadImage(engineState, &engineState->staticAssetsArena, "textures/Minecraft.tga", 2479, 2479);
        engineState->skyFaces[0] = LoadImage(engineState, &engineState->staticAssetsArena, "textures/Sky_C_Left.tga", 512, 512);
        engineState->skyFaces[1] = LoadImage(engineState, &engineState->staticAssetsArena, "textures/Sky_C_Right.tga", 512, 512);
        engineState->skyFaces[2] = LoadImage(engineState, &engineState->staticAssetsArena, "textures/Sky_C_Front.tga", 512, 512);
        engineState->skyFaces[3] = LoadImage(engineState, &engineState->staticAssetsArena, "textures/Sky_C_Back.tga", 512, 512);
        engineState->skyFaces[4] = LoadImage(engineState, &engineState->staticAssetsArena, "textures/Sky_C_Top.tga", 512, 512);
        engineState->skyFaces[5] = LoadImage(engineState, &engineState->staticAssetsArena, "textures/Sky_C_Down.tga", 512, 512);

        engineState->fontSpritesheet = LoadImage(engineState, &engineState->staticAssetsArena, "font.tga", 192, 52);
        engineState->useFiltering = true;

        engineState->moveGizmoAxisID = -1;
        engineState->pickedSliderID = -1;


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
        engineState->chosenSoundLoop = true;
        engineState->masterVolume = 1.0f;
        for (int i = 0; i < ArrayCapacity(engineState->soundChannels); i++)
        {
            StopSound(engineState, i);
        }
        float2 s = float2(videoBuffer->Width, videoBuffer->Height);

        engineState->PhysicsTimeAccumulator = 0;
        

        //gameSaveState->UnlockedLevels = 0;
        LoadImage(engineState, &engineState->staticAssetsArena, "Wuff.tga", 256, 256);
        LoadImage(engineState, &engineState->staticAssetsArena, "textures/Floor_Top_Border.tga", 128, 128);
        LoadImage(engineState, &engineState->staticAssetsArena, "textures/Floor_Top_Flat.tga", 128, 128);
        gameState->extraBallTexture = LoadImage(engineState, &engineState->staticAssetsArena, "textures/ExtraBall.tga", 64, 64);
        gameState->ballTexture = LoadImage(engineState, &engineState->staticAssetsArena, "textures/Ball_Wood.tga", 128, 128);

        gameState->soapParticleTexture = LoadImage(engineState, &engineState->staticAssetsArena, "textures/Particle_Extra_Life.tga", 64, 64);
        gameState->soapTexture = LoadImage(engineState, &engineState->staticAssetsArena, "textures/Extra_Life_Oil.tga", 128, 128);
        gameState->particleTimeTexture = LoadImage(engineState, &engineState->staticAssetsArena, "textures/Particle_Time.tga", 64, 64);
        gameState->particleFireTexture = LoadImage(engineState, &engineState->staticAssetsArena, "textures/Particle_Flames.tga", 128, 128);


        LoadSound(engineState, &engineState->staticAssetsArena, "sound/Music_Theme_2_1.wav", 7.326);
        LoadSound(engineState, &engineState->staticAssetsArena, "sound/Misc_extraball.wav", 0.908);
        LoadSound(engineState, &engineState->staticAssetsArena, "sound/Extra_Life_Blob.wav", 0.317);
        LoadSound(engineState, &engineState->staticAssetsArena, "sound/Hit_Wood_Dome.wav", 2.049);

        gameState->HitSoundMetal = LoadSound(engineState, &engineState->staticAssetsArena, "sound/Hit_Wood_Metal.wav", 0.146);
        gameState->HitSoundStone = LoadSound(engineState, &engineState->staticAssetsArena, "sound/Hit_Wood_Stone.wav", 0.99);
        gameState->HitSoundWood = LoadSound(engineState, &engineState->staticAssetsArena, "sound/Hit_Wood_Wood.wav", 0.122);

        gameState->RollSoundMetal = LoadSound(engineState, &engineState->staticAssetsArena, "sound/Roll_Wood_Metal.wav", 1.771);
        gameState->RollSoundStone = LoadSound(engineState, &engineState->staticAssetsArena, "sound/Roll_Wood_Stone.wav", 1.635);
        gameState->RollSoundWood = LoadSound(engineState, &engineState->staticAssetsArena, "sound/Roll_Wood_Wood.wav", 1.033);


        LoadLevel(engineState, engineState->currentLevel);
    }

    if (!input->Ctrl && !input->Shift && input->GDown)
    {
        engineSaveState->gameViewEnabled = !engineSaveState->gameViewEnabled;
        for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
        {
            engineState->selectedEntities[i] = 0;
        }
    }
    if (!input->Ctrl && !input->Shift && input->PDown)
    {
        engineSaveState->profilingEnabled = !engineSaveState->profilingEnabled;
    }


    // Started playing
    if (engineState->lasteditorState != engineState->editorState)
    {
        if (engineState->editorState == EditorState_Playing)
        {
            //GameStart(engineState, gameState, gameSaveState);
            StopAllSounds(engineState);
        }
        else if (engineState->editorState == EditorState_Editing)
        {
            LoadLevel(engineState, 0);
            ExitToMainMenu(engineState, gameState);
            StopAllSounds(engineState);
        }
    }

    if (engineState->editorState != EditorState_Editing && gameState->menuState == MenuState_Playing)
    {
        GameUpdate(engineState, videoBuffer, input, gameState, gameSaveState);
    }
    else
    {
        if (input->MouseRight)
        {
            UpdateEditorCamera(engineState, engineSaveState, input);
        }
        engineState->camera = engineSaveState->camera;
    }

    ArrayClear(engineState->renderCommands);

    QueueDrawSkyBox(engineState, engineState->skyFaces);
    for (int i = 0; i < ArrayCapacity(engineState->entities); i++)
    {
        Entity* entity = &engineState->entities[i];
        if (!Valid(entity))
            continue;
        if (entity->transform.position.z < -5)
            entity->transform.position.z = -5;
    }

    for (int i = 0; i < ArrayCapacity(engineState->entities); i++)
    {
        Entity* e = &engineState->entities[i];
        if (!Valid(e))
            continue;

        bool selected = false;
        for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
        {
            Entity* selectedEntity = engineState->selectedEntities[i];
            if (e == selectedEntity)
            {
                selected = true;
                break;
            }
        }

        bool Hovered = e == engineState->hoveredEntity;
        if (e->type == EntityType_Sphere && e->mesh)
            QueueDrawSphere(engineState, e->transform, e->texture1, e->texture2, e->shaderIndex, selected, Hovered);
        if (e->type == EntityType_Box && e->mesh)
            QueueDrawBox(engineState, e->transform, e->texture1, e->texture2, e->shaderIndex, selected, Hovered);
        if (e->type == EntityType_Effect)
            QueueDrawEffect(engineState, e->transform, e->texture1, e->texture2, e->shaderIndex, e->effect, e->color);

        if (!engineSaveState->gameViewEnabled)
        {
            Transform t = e->transform;
            t.scale = float3(1, 1, 1) * 0.5f;
            if (e->type == EntityType_Spawn)
                QueueDrawSphere(engineState, t, e->texture1, e->texture2, e->shaderIndex, selected, Hovered);
            if (e->type == EntityType_Win)
                QueueDrawSphere(engineState, t, e->texture1, e->texture2, e->shaderIndex, selected, Hovered);
            if (e->type == EntityType_Checkpoint)
                QueueDrawSphere(engineState, t, e->texture1, e->texture2, e->shaderIndex, selected, Hovered);
            if (e->type == EntityType_Health)
                QueueDrawSphere(engineState, t, e->texture1, e->texture2, e->shaderIndex, selected, Hovered);
            if (e->type == EntityType_Time)
                QueueDrawSphere(engineState, t, e->texture1, e->texture2, e->shaderIndex, selected, Hovered);
        }
    }


    ProfilerBeingSample(&engineState->profilingData);
    // Draws the queued up commands to the screen.
    DrawScene(Memory, engineState, engineSaveState, videoBuffer);
    //DrawLine3D(videoBuffer, engineState->camera, float3(0, 0, 0), float3(5, 4, 3), true);
    //DrawRectangle(videoBuffer, float2(0, 0), float2(videoBuffer->Width, videoBuffer->Height), float3(0, 0.5f, 0));
    ProfilerEndSample(&engineState->profilingData, "DrawScene");
    

    ProfilerBeingSample(&engineState->profilingData);
    // Update sound system
    ClearSoundBuffer(SoundBuffer);
    for (int i = 0; i < ArrayCapacity(engineState->soundChannels); i++)
    {
        SoundChannel* channel = &engineState->soundChannels[i];
        PlaySoundUpdate(SoundBuffer, channel);
    }

    SetSoundBufferVolume(SoundBuffer, engineState->masterVolume);
    ProfilerEndSample(&engineState->profilingData, "Sound");

    ProfilerBeingSample(&engineState->profilingData);
    if (gameState->menuState == MenuState_Playing)
    {
        // Substepping such that physics runs at 60 fps
        engineState->PhysicsTimeAccumulator += input->deltaTime;

        while (engineState->PhysicsTimeAccumulator > 0)
        {
            engineState->PhysicsTimeAccumulator -= 1.0f / 60.0f;
            UpdatePhysics(Memory, engineState, engineSaveState, videoBuffer);
        }
    }
    ProfilerEndSample(&engineState->profilingData, "Physics");

    ProfilerBeingSample(&engineState->profilingData);

    if (engineState->editorState == EditorState_Playing)
    {
        GameUpdateUI(engineState, videoBuffer, input, gameState, gameSaveState, engineSaveState);
    }

    int64 time_end = Memory->platformTime();

    float2 UIPos = float2(4, 0);

    Text(engineState, videoBuffer, &UIPos, "Controls");
    Text(engineState, videoBuffer, &UIPos, "  G to toggle editor.");
    Text(engineState, videoBuffer, &UIPos, "  P to toggle profiling.");

    if (engineState->editorState != engineState->lasteditorState)
    {
        engineState->lasteditorState = engineState->editorState;
        for (int i = 0; i < ArrayCapacity(engineState->selectedEntities); i++)
        {
            engineState->selectedEntities[i] = 0;
        }
    }
    
    //DrawMemoryColor(videoBuffer, (uint8*)engineState, videoBuffer->Width*500);
    if (!engineSaveState->gameViewEnabled)
    {
        Text(engineState, videoBuffer, &UIPos, "  Space to reset program. Ctrl + Space to hard-reset program. Right mouse + WASD to move and look.");
        if (Toggle(engineState, input, videoBuffer, &UIPos, engineState->editorState == EditorState_Playing, "Playing"))
            engineState->editorState = EditorState_Playing;

        if (Toggle(engineState, input, videoBuffer, &UIPos, engineState->editorState == EditorState_Editing, "Editing"))
            engineState->editorState = EditorState_Editing;

        if (Toggle(engineState, input, videoBuffer, &UIPos, engineState->editorState == EditorState_Settings, "Settings"))
            engineState->editorState = EditorState_Settings;

        if (Toggle(engineState, input, videoBuffer, &UIPos, engineState->editorState == EditorState_Sound, "Sound"))
            engineState->editorState = EditorState_Sound;

        switch (engineState->editorState)
        {
        case EditorState_Editing:
        {
            UpdateEditor(Memory, engineState, engineSaveState, input, videoBuffer, &UIPos);

        }break;
        case EditorState_Settings:
        {

        }break;
        case EditorState_Sound:
        {
            float2 StringSoundPlacement = float2(200, videoBuffer->Height - 100);
            RenderStringSoundLine(videoBuffer, SoundBuffer, input, &engineState->stringData, StringSoundPlacement, float3(1, 1, 1));
            //StringSound(videoBuffer, SoundBuffer, input, &engineState->stringData, engineState->MouseIsOverUI || engineState->Grabbing, StringSoundPlacement);
            StringSound(videoBuffer, SoundBuffer, input, &engineState->stringData, false, StringSoundPlacement);

            // Draw sound debug UI
            DrawSoundBuffer(videoBuffer, SoundBuffer, float3(1.0, 0.5, 0.5));

            //char names[4][100] = {
            //    "Sound: sound/Music_Theme_2_1.wav",
            //    "Sound: sound/Misc_extraball.wav",
            //    "Sound: sound/Hit_Wood_Dome.wav",
            //    "Sound: sound/Extra_Life_Blob.wav"
            //};
            float2 ChannelsMenuPos = float2(180, 10);
            for (int i = 0; i < ArrayCapacity(engineState->soundChannels); i++)
            {
                SoundChannel* channel = &engineState->soundChannels[i];
                Text(engineState, videoBuffer, &ChannelsMenuPos, "Channel ", i);
                //Text(engineState, videoBuffer, &ChannelsMenuPos, names[SoundIndex(engineState, channel->sound)]);
                //Text(engineState, videoBuffer, &ChannelsMenuPos, "Volume: ", channel->volume);
                //Text(engineState, videoBuffer, &ChannelsMenuPos, "Speed: ", channel->speed);
                //Text(engineState, videoBuffer, &ChannelsMenuPos, "Loop: ", channel->loop);

                channel->speed = Slider(engineState, input, videoBuffer, &ChannelsMenuPos, channel->speed, 0, 20);
                Text(engineState, videoBuffer, ChannelsMenuPos + float2(0, -26), "Speed: ");
                Text(engineState, videoBuffer, ChannelsMenuPos + float2(200, -26), "Volume: ");
                channel->volume = Slider(engineState, input, videoBuffer, ChannelsMenuPos + float2(200, -24), channel->volume, 0, 2);
                DrawSound(videoBuffer, channel->sound, &ChannelsMenuPos, float2(550, 50), channel->currentSample);
                //Text(engineState, videoBuffer, &ChannelsMenuPos, "Speed: ", channel->speed);
                //Text(engineState, videoBuffer, &ChannelsMenuPos, "");
            }

            float2 SoundMenuPos = float2(videoBuffer->Width - 200, 20);
            /*
            char PlayText[100] = "Play in channel ";
            Append(PlayText, engineState->chosenChannel);
            if (Button(engineState, input, videoBuffer, &SoundMenuPos, PlayText))
            {
                PlaySound(engineState, engineState->chosenChannel, &engineState->sounds[engineState->chosenSound], 1.0f, engineState->chosenSoundLoop, 1.0f);
            }
            char StopText[100] = "Stop channel ";
            Append(StopText, engineState->chosenChannel);
            if (Button(engineState, input, videoBuffer, &SoundMenuPos, StopText))
            {
                StopSound(engineState, engineState->chosenChannel);
            }
            if (Button(engineState, input, videoBuffer, &SoundMenuPos, "Stop All"))
            {
                StopAllSounds(engineState);
            }
            Text(engineState, videoBuffer, &SoundMenuPos, engineState->soundNames[engineState->chosenSound]);
            engineState->chosenSound = Slider(engineState, input, videoBuffer, &SoundMenuPos, engineState->chosenSound, 0, engineState->currentSound - 1);
            Text(engineState, videoBuffer, &SoundMenuPos, "Channel: ", (int)engineState->chosenChannel);
            engineState->chosenChannel = Slider(engineState, input, videoBuffer, &SoundMenuPos, engineState->chosenChannel, 0, ArrayCapacity(engineState->soundChannels) - 1);
            engineState->chosenSoundLoop = Toggle(engineState, input, videoBuffer, &SoundMenuPos, engineState->chosenSoundLoop, "Loop");
            */
            Text(engineState, videoBuffer, &SoundMenuPos, "Master Volume: ", engineState->masterVolume);
            engineState->masterVolume = Slider(engineState, input, videoBuffer, &SoundMenuPos, engineState->masterVolume, 0.0f, 2.0f);


            float2 StringUIPos = float2(20, videoBuffer->Height - 220);
            Text(engineState, videoBuffer, &StringUIPos, "String synth");
            Text(engineState, videoBuffer, &StringUIPos, "    Left click to pull string.");
            Text(engineState, videoBuffer, &StringUIPos, " ");
            Text(engineState, videoBuffer, &StringUIPos, "Speed: ", engineState->stringData.speed);
            engineState->stringData.speed = Slider(engineState, input, videoBuffer, &StringUIPos, engineState->stringData.speed, 0.01f, 0.5f);

            Text(engineState, videoBuffer, &StringUIPos, "Damping: ", engineState->stringData.damping);
            engineState->stringData.damping = Slider(engineState, input, videoBuffer, &StringUIPos, engineState->stringData.damping, 0.0f, 0.005f);

            Text(engineState, videoBuffer, &StringUIPos, "Timescale: ", engineState->stringData.timescale);
            engineState->stringData.timescale = Slider(engineState, input, videoBuffer, &StringUIPos, engineState->stringData.timescale, 0.0f, 1.0f);

            Text(engineState, videoBuffer, &StringUIPos, "Segments: ", engineState->stringData.Segments);
            engineState->stringData.Segments = Slider(engineState, input, videoBuffer, &StringUIPos, engineState->stringData.Segments, 4.0f, 200.0f);

            float2 FFTPlacement = float2(0, videoBuffer->Height);
            RenderFFT(videoBuffer, SoundBuffer, input, &engineState->stringData, FFTPlacement, float3(0.5, 0.5, 1.0));

            engineState->stringData.FTScale = 10;

        }break;
        }
    }

    ProfilerEndSample(&engineState->profilingData, "UI");

    if (engineSaveState->profilingEnabled)
    {
        Text(engineState, videoBuffer, &UIPos, " ");
        Text(engineState, videoBuffer, &UIPos, "Timers");
        Text(engineState, videoBuffer, &UIPos, "    Update Time MS: ", input->deltaTime * 1000);
        Text(engineState, videoBuffer, &UIPos, "    FPS: ", 1.0f / input->deltaTime);
        Text(engineState, videoBuffer, &UIPos, " ");
        engineSaveState->threadedRendering = Toggle(engineState, input, videoBuffer, &UIPos, engineSaveState->threadedRendering, "Threaded rendering");
        if (engineSaveState->threadedRendering)
        {
            engineSaveState->visualize_ThreadedRendering = Toggle(engineState, input, videoBuffer, &UIPos, engineSaveState->visualize_ThreadedRendering, "Visualize", float2(12, 0));
            
            char text[100];
            for (int i = 0; i < 100; i++)
            {
                text[i] = 0;
            }
            Append(text, "Number of threads: ");
            Append(text, engineSaveState->screenThreads);

            if (Button(engineState, input, videoBuffer, &UIPos, text))
            {
                engineSaveState->screenThreads *= 2;
                if (engineSaveState->screenThreads > 32)
                {
                    engineSaveState->screenThreads = 2;
                }
            }
        }

        // Debugging
        ProfilerDrawTimeChart(input, engineState, videoBuffer);
        ProfilerDrawFlameChart(input, engineState, videoBuffer);
    }
}