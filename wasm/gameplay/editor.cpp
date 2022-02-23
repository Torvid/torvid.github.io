#pragma once



int FindImageIndex(EngineState* engineState, Image* image)
{
    for (int i = 0; i < ArrayCount(engineState->Images); i++)
    {
        if (&engineState->Images[i] == image)
        {
            return i;
        }
    }
    return -1; // not found
}

// Functions called when deserializing
Entity* MakeEntity(EngineState* engineState, Entity* entity,
    transform t, bool mesh, int type, int texture1, int texture2, // mesh
    int shader, bool physics = 0, bool kinematic = 0, float mass = 1, // physics
    EffectData effectData = {}, int group = 0, int surfaceType = 0)
{
    *entity = {};
    entity->initialized = true;

    // Transform
    entity->t = t;

    // Mesh
    entity->mesh = mesh;
    entity->type = (EntityType)type;
    entity->texture1 = &engineState->Images[texture1];
    entity->texture2 = &engineState->Images[texture2];
    entity->ShaderIndex = shader;

    // Physics
    entity->physics = physics;
    entity->kinematic = kinematic;
    entity->mass = mass;
    entity->friction = 0.01f;
    entity->drag = 0.005f;
    entity->effect = effectData;

    entity->Color = float3(1, 1, 1);
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
    int max = ArrayCount(engineState->SaveBuffer);
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
    AppendArgument(max, target, FindImageIndex(engineState, entity->texture1));
    AppendArgument(max, target, FindImageIndex(engineState, entity->texture2));
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

    AppendArgument(max, target, entity->group);
    AppendArgument(max, target, entity->surfaceType);

    Append(target, ");", max);
    Append(target, "\n", max);
}

void LoadLevel(EngineState* engineState, int level)
{
    
    for (int j = 0; j < ArrayCount(engineState->Entities); j++)
    {
        engineState->Entities[j].initialized = false;
    }
    engineState->CurrentLevel = level;
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
    //for (int i = 0; i < ArrayCount(engineState->EntityUndoBuffer); i++)
    //{
    //    for (int j = 0; j < ArrayCount(engineState->Entities); j++)
    //    {
    //        engineState->Entities[j] = engineState->Entities[j];
    //    }
    //}

}

void SaveLevel(game_memory* Memory, EngineState* engineState, int level)
{
    for (int i = 0; i < ArrayCount(engineState->SaveBuffer); i++)
    {
        engineState->SaveBuffer[i] = 0;
    }
    for (int i = 0; i < ArrayCount(engineState->Entities); i++)
    {
        Entity* entity = &engineState->Entities[i];
        if (!entity->initialized)
            continue;
        WriteEntity(engineState, engineState->SaveBuffer, entity, i);
    }
    char LevelName[11] = {};
    Append(LevelName, "Level");
    Append(LevelName, level);
    Append(LevelName, ".cpp");
    read_file_result r;
    r.Contents = &engineState->SaveBuffer;
    r.ContentsSize = StringLength(engineState->SaveBuffer);
    Memory->PlatformWriteFile(0, LevelName, r);
}


void UpdateEditorCamera(EngineState* engineState, EngineSaveState* engineSaveState, game_input* Input)
{
    engineSaveState->Camera = rotate(engineSaveState->Camera, float3(0, 0, 1), -Input->MouseXDelta * 0.005f);
    engineSaveState->Camera = rotate(engineSaveState->Camera, engineSaveState->Camera.right, -Input->MouseYDelta * 0.005f);

    int X = Input->W ? 1 : 0 + Input->S ? -1 : 0;
    int Y = Input->D ? 1 : 0 + Input->A ? -1 : 0;
    int Z = Input->E ? 1 : 0 + Input->Q ? -1 : 0;

    float3 Movement = float3(0, 0, 0);
    Movement = Movement + engineSaveState->Camera.forward * X;
    Movement = Movement + engineSaveState->Camera.right * Y;
    Movement = Movement + engineSaveState->Camera.up * Z;
    if (length(Movement) > 0)
        Movement = normalize(Movement);
    float speed = 4;
    if (Input->Shift)
        speed = 16;
    engineSaveState->Camera.position += Movement * Input->DeltaTime * speed;

}

void ClearSelection(EngineState* engineState)
{
    for (int i = 0; i < ArrayCount(engineState->SelectedEntities); i++)
    {
        engineState->SelectedEntities[i] = 0;
    }
}

void SelectEntity(EngineState* engineState, Entity* entity)
{
    // Find an empty slot in the list of selected entites and add our entity there.

    for (int i = 0; i < ArrayCount(engineState->SelectedEntities); i++)
    {
        if (engineState->SelectedEntities[i] == entity)
            break;
        if (!Valid(engineState->SelectedEntities[i]))
        {
            engineState->SelectedEntities[i] = entity;
            
            break;
        }
    }
}

void SelectGroup(EngineState* engineState, int group)
{
    if (group == 0)
        return;

    for (int i = 0; i < ArrayCount(engineState->Entities); i++)
    {
        Entity* e = &engineState->Entities[i];

        if (Valid(e) && engineState->Entities[i].group == group)
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
    for (int i = 0; i < ArrayCount(engineState->Entities); i++)
    {
        Entity* e = &engineState->Entities[i];
        if (Valid(e))
        {
            GroupOccupied[e->group] = true;
        }
    }
    for (int i = 0; i < ArrayCount(GroupOccupied); i++)
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
    for (int i = 0; i < ArrayCount(engineState->SelectedEntities); i++)
    {
        if (engineState->SelectedEntities[i] == entity)
        {
            engineState->SelectedEntities[i] = 0;
            break;
        }
    }
}

void UpdateEditor(game_memory* Memory, EngineState* engineState, EngineSaveState* engineSaveState, game_input* Input, game_offscreen_buffer* VideoBuffer, float2* UIPos)
{
    // EDITOR STUFF
    //Text(engineState, VideoBuffer, &UIPos, "Editor Controls");
    //Text(engineState, VideoBuffer, &UIPos, "  W Translate.");
    //Text(engineState, VideoBuffer, &UIPos, "  E Rotate.");
    //Text(engineState, VideoBuffer, &UIPos, "  R Scale.");
    //Text(engineState, VideoBuffer, &UIPos, "  Delete to delete selection.");
    //Text(engineState, VideoBuffer, &UIPos, "  Ctrl + D to duplicate selection.");
    //Text(engineState, VideoBuffer, &UIPos, "  Left click to select.");
    //Text(engineState, VideoBuffer, &UIPos, "  Shift + Left click add to selection.");
    //Text(engineState, VideoBuffer, &UIPos, "  Ctrl + Left click remove from selection.");

    for (int i = 0; i < ArrayCount(engineState->Entities); i++)
    {
        Entity* e = &engineState->Entities[i];
        if (!Valid(e))
            continue;

        float3 ScreenPos3D = ToScreenPos(VideoBuffer, e->t.position, engineState->Camera);
        if (ScreenPos3D.z < 0)
            continue;

        if (e->type <= 2)
            continue;

        float2 ScreenPos = float2(ScreenPos3D.x, (ScreenPos3D.y * 0.5) + 100);

        Text(engineState, VideoBuffer, ScreenPos, EntityTypeNames[e->type]);
    }

    Entity* GizmoEntity = 0;
    for (int i = 0; i < ArrayCount(engineState->SelectedEntities); i++)
    {
        Entity* e = engineState->SelectedEntities[i];
        if (!Valid(e))
            continue;

        GizmoEntity = e;
    }

    if (!Input->MouseRight)
    {
        if (Input->W)
            engineState->GizmoState = 0;
        if (Input->E)
            engineState->GizmoState = 1;
        if (Input->R)
            engineState->GizmoState = 2;
    }

    float3 MoveOffset = float3(0, 0, 0);
    float3 ScaleOffset = float3(0, 0, 0);
    Rotation RotationOffset;
    RotationOffset.Axis = float3(1, 0, 0);
    RotationOffset.Angle = 0;
    if (Valid(GizmoEntity))
    {
        if (engineState->GizmoState == 0)
        {
            MoveOffset = MoveGizmo(engineState, Input, VideoBuffer, engineState->Camera, GizmoEntity->t, engineSaveState->GizmoWorldSpace);
        }
        else if (engineState->GizmoState == 1)
        {
            RotationOffset = RotateGizmo(engineState, Input, VideoBuffer, engineState->Camera, GizmoEntity->t, engineSaveState->GizmoWorldSpace);
        }
        else if (engineState->GizmoState == 2)
        {
            ScaleOffset = ScaleGizmo(engineState, Input, VideoBuffer, engineState->Camera, GizmoEntity->t);
        }
    }
    engineState->Grabbing = engineState->PickedSliderID != -1;

    for (int i = 0; i < ArrayCount(engineState->SelectedEntities); i++)
    {
        Entity* e = engineState->SelectedEntities[i];
        if (Valid(e))
        {
            e->t.position += MoveOffset;
            e->t.scale += ScaleOffset;
            e->t = rotate(e->t, RotationOffset.Axis, RotationOffset.Angle);

            if (e != GizmoEntity)
            {
                float3 RelativePos = e->t.position - GizmoEntity->t.position;
                RelativePos = RotateAroundAxis(RelativePos, RotationOffset.Axis, RotationOffset.Angle);
                RelativePos += GizmoEntity->t.position;
                e->t.position = RelativePos;
            }
        }
    }

    if (Input->Ctrl && Input->GDown)
    {
        engineState->OperationHappened = true;
        if (ArrayCount(engineState->SelectedEntities) == 1) // If a single entity is selected, clear its group
        {
            Entity* e = engineState->SelectedEntities[0];
            if (Valid(e))
            {
                e->group = 0;
            }
        }
        else
        {
            int FreeGroup = FindFreeGroup(engineState);

            for (int i = 0; i < ArrayCount(engineState->SelectedEntities); i++)
            {
                Entity* e = engineState->SelectedEntities[i];
                if (Valid(e))
                {
                    e->group = FreeGroup;
                }
            }
        }
    }
    if (Input->Shift && Input->GDown)
    {
        engineState->OperationHappened = true;
        for (int i = 0; i < ArrayCount(engineState->SelectedEntities); i++)
        {
            Entity* e = engineState->SelectedEntities[i];
            if (Valid(e))
            {
                e->group = 0;
            }
        }
    }

    if (Input->DeleteDown)
    {
        engineState->OperationHappened = true;
        for (int i = 0; i < ArrayCount(engineState->SelectedEntities); i++)
        {
            Entity* e = engineState->SelectedEntities[i];
            if (Valid(e))
            {
                *e = {};
            }
        }
    }


    if (Input->Ctrl && Input->DDown)
    {
        int FreeGroup = FindFreeGroup(engineState);
        for (int i = 0; i < ArrayCount(engineState->SelectedEntities); i++)
        {
            Entity* selectedEntity = engineState->SelectedEntities[i];
            if (!Valid(selectedEntity))
                continue;
            
            for (int j = 0; j < ArrayCount(engineState->Entities); j++)
            {
                Entity* entity = &engineState->Entities[j];
                if (Valid(entity))
                    continue;

                *entity = *selectedEntity; // copy?
                entity->group = FreeGroup;
                break;
            }
        }
    }

    if (!engineState->Grabbing)
    {
        // Find closest entity under cursor.
        float3 MouseRay = ScreenPointToRay(VideoBuffer, engineState->Camera, float2(Input->MouseX, Input->MouseY));
        float ClosestDistance = 99999;
        engineState->HoveredEntity = 0;
        for (int i = 0; i < ArrayCount(engineState->Entities); i++)
        {
            Entity* e = &engineState->Entities[i];
            if (!Valid(e))
                continue;

            float3 Normal;
            float3 LocalPos = WorldToLocal(e->t, engineState->Camera.position);
            float3 LocalDir = WorldToLocalVector(e->t, MouseRay);

            float HitDepth = RayBoxIntersect(LocalPos, LocalDir, &Normal);
            if (HitDepth > 0)
            {
                if (HitDepth < ClosestDistance)
                {
                    ClosestDistance = HitDepth;
                    engineState->HoveredEntity = e;
                }
            }
        }

        if (Input->MouseLeftDown && !engineState->MouseIsOverUI && Valid(engineState->HoveredEntity))
        {
            if (Input->Shift)
            {
                SelectEntity(engineState, engineState->HoveredEntity);
                SelectGroup(engineState, engineState->HoveredEntity->group);
            }
            else if (Input->Ctrl)
            {
                DeselectEntity(engineState, engineState->HoveredEntity);
            }
            else
            {
                ClearSelection(engineState);
                SelectEntity(engineState, engineState->HoveredEntity);
                SelectGroup(engineState, engineState->HoveredEntity->group);
            }
        }
        if (Input->EscapeDown)
        {
            ClearSelection(engineState);
        }
    }

    engineState->MouseIsOverUI = false;



    float2 MenuPlacement = float2(VideoBuffer->Width - 120, 5);

    for (int i = 0; i < 4; i++)
    {
        char LoadText[100] = "Load Level ";
        Append(LoadText, i);
        if (Button(engineState, Input, VideoBuffer, &MenuPlacement, LoadText))
        {
            LoadLevel(engineState,i);
        }
    }

    if (Button(engineState, Input, VideoBuffer, &MenuPlacement, "Save"))
    {
        SaveLevel(Memory, engineState, engineState->CurrentLevel);
    }
    if (Input->Ctrl && Input->SDown)
    {
        SaveLevel(Memory, engineState, engineState->CurrentLevel);
    }

    char SpaceText[10] = {};
    engineSaveState->GizmoWorldSpace ? Append(SpaceText, "Local Space") : Append(SpaceText, "World Space");
    if (Button(engineState, Input, VideoBuffer, &MenuPlacement, SpaceText))
    {
        engineSaveState->GizmoWorldSpace = !engineSaveState->GizmoWorldSpace;
    }

    Text(engineState, VideoBuffer, &MenuPlacement, "");
    Text(engineState, VideoBuffer, &MenuPlacement, "Entities.");
    for (int i = 0; i < ArrayCount(engineState->Entities); i++)
    {
        Entity* e = &engineState->Entities[i];

        if (!Valid(e))
            continue;

        char LoadText[100] = "Load Level ";
        if (Button(engineState, Input, VideoBuffer, &MenuPlacement, EntityTypeNames[e->type]))
        {
            ClearSelection(engineState);
            engineState->SelectedEntities[0] = e;
        }
    }

    Text(engineState, VideoBuffer, UIPos, " ");
    
    if (Button(engineState, Input, VideoBuffer, UIPos, "Snap Rotation"))
    {
        engineState->OperationHappened = true;
        for (int i = 0; i < ArrayCount(engineState->SelectedEntities); i++)
        {
            Entity* entity = engineState->SelectedEntities[i];
            if (!Valid(entity))
                continue;

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
    if (Button(engineState, Input, VideoBuffer, UIPos, "Snap"))
    {
        engineState->OperationHappened = true;
        for (int i = 0; i < ArrayCount(engineState->SelectedEntities); i++)
        {
            Entity* entity = engineState->SelectedEntities[i];
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

    if (Button(engineState, Input, VideoBuffer, UIPos, "Reset Selected"))
    {
        engineState->OperationHappened = true;
        for (int i = 0; i < ArrayCount(engineState->SelectedEntities); i++)
        {
            Entity* entity = engineState->SelectedEntities[i];
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
    if (Button(engineState, Input, VideoBuffer, UIPos, "Play Effect"))
    {
        for (int i = 0; i < ArrayCount(engineState->SelectedEntities); i++)
        {
            Entity* entity = engineState->SelectedEntities[0];
            if (!Valid(entity))
                continue;

            entity->effect.startTime = engineState->GameTime;
        }
    }

    Text(engineState, VideoBuffer, UIPos, " ");
    Entity* entity = engineState->SelectedEntities[0];
    if (Valid(entity))
    {
        Text(engineState, VideoBuffer, UIPos, "Position: ", entity->t.position);
        Text(engineState, VideoBuffer, UIPos, "Group: ", entity->group);
        Text(engineState, VideoBuffer, UIPos, EntityTypeNames[entity->type]);
        entity->type = (EntityType)(int)round(Slider(engineState, Input, VideoBuffer, UIPos, entity->type, 0, ArrayCount(EntityTypeNames)));

        Text(engineState, VideoBuffer, UIPos, ShaderNames[entity->ShaderIndex]);
        int LastShaderIndex = entity->ShaderIndex;
        entity->ShaderIndex = Slider(engineState, Input, VideoBuffer, UIPos, entity->ShaderIndex, 0, ArrayCount(ShaderNames));
        if (LastShaderIndex != entity->ShaderIndex)
        {
            for (int i = 0; i < ArrayCount(engineState->SelectedEntities); i++)
            {
                if (!Valid(engineState->SelectedEntities[i]))
                    continue;
                engineState->SelectedEntities[i]->ShaderIndex = entity->ShaderIndex;
            }
        }

        Text(engineState, VideoBuffer, UIPos, "Texture 1: ");
        int texture1 = FindImageIndex(engineState, entity->texture1);
        int LastTexture1 = texture1;
        texture1 = Slider(engineState, Input, VideoBuffer, UIPos, texture1, 0, engineState->CurrentImage);
        entity->texture1 = &engineState->Images[texture1];
        if (LastTexture1 != texture1)
        {
            for (int i = 0; i < ArrayCount(engineState->SelectedEntities); i++)
            {
                if (!Valid(engineState->SelectedEntities[i]))
                    continue;
                engineState->SelectedEntities[i]->texture1 = entity->texture1;
            }
        }

        int EntityCount = 0;
        for (int i = 0; i < ArrayCount(engineState->Entities); i++)
        {
            if (Valid(engineState->Entities[i]))
                EntityCount++;
        }

        Text(engineState, VideoBuffer, UIPos, "Entity Count: ", EntityCount);

        Text(engineState, VideoBuffer, UIPos, "Texture 2: ");
        int texture2 = FindImageIndex(engineState, entity->texture2);
        int LastTexture2 = texture2;
        texture2 = Slider(engineState, Input, VideoBuffer, UIPos, texture2, 0, engineState->CurrentImage);
        entity->texture2 = &engineState->Images[texture2];
        if (LastTexture2 != texture2)
        {
            for (int i = 0; i < ArrayCount(engineState->SelectedEntities); i++)
            {
                if (!Valid(engineState->SelectedEntities[i]))
                    continue;
                engineState->SelectedEntities[i]->texture2 = entity->texture2;
            }
        }

        if (entity->type == EntityType_Sphere || entity->type == EntityType_Box)
        {
            Text(engineState, VideoBuffer, UIPos, SurfaceTypeNames[entity->surfaceType]);
            int LastShaderIndex = entity->surfaceType;
            entity->surfaceType = (SurfaceType)(int)round(Slider(engineState, Input, VideoBuffer, UIPos, entity->surfaceType, 0, ArrayCount(SurfaceTypeNames)));
            if (LastShaderIndex != entity->surfaceType)
            {
                for (int i = 0; i < ArrayCount(engineState->SelectedEntities); i++)
                {
                    if (!Valid(engineState->SelectedEntities[i]))
                        continue;
                    engineState->SelectedEntities[i]->surfaceType = entity->surfaceType;
                }
            }

            entity->physics = Toggle(engineState, Input, VideoBuffer, UIPos, entity->physics, "Physics");
            entity->kinematic = Toggle(engineState, Input, VideoBuffer, UIPos, entity->kinematic, "Kinematic");
            if (entity->kinematic || !entity->physics)
            {
                entity->angularVelocity = 0;
                entity->velocity = float3(0, 0, 0);
            }
            Text(engineState, VideoBuffer, UIPos, "Mass: ", entity->mass);
            entity->mass = Slider(engineState, Input, VideoBuffer, UIPos, entity->mass, 0, 4);

        }
        else if (entity->type == EntityType_Effect)
        {
            EffectData* effect = &entity->effect;

            float2 UIPos2 = float2(160 * 2, VideoBuffer->Height - 260);
            float2 UIPos3 = float2(160, VideoBuffer->Height - 260);

            effect->burst = Toggle(engineState, Input, VideoBuffer, &UIPos3, effect->burst, "Burst");


            Text(engineState, VideoBuffer, &UIPos3, "spawnRate: ", effect->spawnRate);
            effect->spawnRate = Slider(engineState, Input, VideoBuffer, &UIPos3, effect->spawnRate, 0, 25);


            Text(engineState, VideoBuffer, &UIPos3, "spawnRadius: ", effect->spawnRadius);
            effect->spawnRadius = Slider(engineState, Input, VideoBuffer, &UIPos3, effect->spawnRadius, 0, 4);

            Text(engineState, VideoBuffer, &UIPos3, "drag: ", effect->drag);
            effect->drag = Slider(engineState, Input, VideoBuffer, &UIPos3, effect->drag, 0, 4);

            Text(engineState, VideoBuffer, &UIPos3, "gravity: ", effect->gravity);
            effect->gravity = Slider(engineState, Input, VideoBuffer, &UIPos3, effect->gravity, -25, 0);


            Text(engineState, VideoBuffer, &UIPos3, "startSize: ", effect->startSize);
            effect->startSize = Slider(engineState, Input, VideoBuffer, &UIPos3, effect->startSize, 0, 4);

            Text(engineState, VideoBuffer, &UIPos3, "endSize: ", effect->endSize);
            effect->endSize = Slider(engineState, Input, VideoBuffer, &UIPos3, effect->endSize, 0, 4);


            Text(engineState, VideoBuffer, &UIPos2, "minSpeed.x: ", effect->minSpeed.x);
            effect->minSpeed.x = Slider(engineState, Input, VideoBuffer, &UIPos2, effect->minSpeed.x, -1, 1);
            Text(engineState, VideoBuffer, &UIPos2, "minSpeed.y: ", effect->minSpeed.y);
            effect->minSpeed.y = Slider(engineState, Input, VideoBuffer, &UIPos2, effect->minSpeed.y, -1, 1);
            Text(engineState, VideoBuffer, &UIPos2, "minSpeed.z: ", effect->minSpeed.z);
            effect->minSpeed.z = Slider(engineState, Input, VideoBuffer, &UIPos2, effect->minSpeed.z, -1, 1);


            Text(engineState, VideoBuffer, &UIPos2, "maxSpeed.x: ", effect->maxSpeed.x);
            effect->maxSpeed.x = Slider(engineState, Input, VideoBuffer, &UIPos2, effect->maxSpeed.x, -1, 1);
            Text(engineState, VideoBuffer, &UIPos2, "maxSpeed.y: ", effect->maxSpeed.y);
            effect->maxSpeed.y = Slider(engineState, Input, VideoBuffer, &UIPos2, effect->maxSpeed.y, -1, 1);
            Text(engineState, VideoBuffer, &UIPos2, "maxSpeed.z: ", effect->maxSpeed.z);
            effect->maxSpeed.z = Slider(engineState, Input, VideoBuffer, &UIPos2, effect->maxSpeed.z, -1, 1);


            Text(engineState, VideoBuffer, &UIPos2, "minLifetime: ", effect->minLifetime);
            effect->minLifetime = Slider(engineState, Input, VideoBuffer, &UIPos2, effect->minLifetime, 0, 4);

            Text(engineState, VideoBuffer, &UIPos2, "maxLifetime: ", effect->maxLifetime);
            effect->maxLifetime = Slider(engineState, Input, VideoBuffer, &UIPos2, effect->maxLifetime, 0, 4);
        }
    }


    int UndoBufferLength = ArrayCount(engineState->EntityUndoBuffer) - 1;

    if (engineState->OperationHappened)
    {
        engineState->UndoSteps++; // add another undo step
        if (engineState->UndoSteps > UndoBufferLength)
            engineState->UndoSteps = UndoBufferLength;

        engineState->OperationHappened = false;
        engineState->UndoIndex++;
        if (engineState->UndoIndex < 0)
            engineState->UndoIndex = UndoBufferLength;
        if (engineState->UndoIndex > UndoBufferLength)
            engineState->UndoIndex = 0;
        for (int i = 0; i < ArrayCount(engineState->Entities); i++)
        {
            engineState->EntityUndoBuffer[engineState->UndoIndex][i] = engineState->Entities[i];
        }
    }
    // Undo
    if (Input->Ctrl && Input->ZDown)
    {
        if (engineState->UndoSteps > 0) // there are undo steps left
        {
            engineState->UndoIndex--;
            if (engineState->UndoIndex < 0)
                engineState->UndoIndex = UndoBufferLength;
            if (engineState->UndoIndex > UndoBufferLength)
                engineState->UndoIndex = 0;

            for (int i = 0; i < ArrayCount(engineState->Entities); i++)
            {
                engineState->Entities[i] = engineState->EntityUndoBuffer[engineState->UndoIndex][i];
            }
        }
        engineState->UndoSteps--;
        if (engineState->UndoSteps <= 0)
            engineState->UndoSteps = 0;
    }
}