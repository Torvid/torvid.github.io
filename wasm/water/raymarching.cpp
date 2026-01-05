#pragma once
#include "quote.h"

struct Scene5
{
    float2 cameraPos;
    float perspective;
};

void raymarching(GameMemory* memory, EngineState* engineState, GameInput* input, VideoBuffer* videoBuffer)
{
    Scene5* scene = (Scene5*)(((uint8*)(engineState + 1)));

    if (input->SpaceDown)
        memory->initialized = false;

    if (!memory->initialized)
    {
        engineState->profilingData.zoom = 1;
        engineState->platformReadImageIntoMemory = memory->platformReadImageIntoMemory;
        engineState->platformReadSoundIntoMemory = memory->platformReadSoundIntoMemory;
        engineState->platformPrint = memory->platformPrint;
        engineState->platformSetForcefeedbackEffect = memory->platformSetForcefeedbackEffect;
        InitializeArena(&engineState->staticAssetsArena, Megabytes(12), (uint8*)memory->transientStorage + sizeof(EngineState) + sizeof(Scene3) + Kilobytes(100));
        engineState->fontSpritesheet = LoadImage(engineState, &engineState->staticAssetsArena, "font.tga", 192, 52);

        memory->initialized = true;
    }

    float2 mousePos = float2(input->MouseX, input->MouseY);

    DrawClear(videoBuffer, float3(0.5, 0, 0));

    DrawCircle(videoBuffer, mousePos, 5.0f);
    float2 circlePos = float2(400,400);
    float circleRad = 150;
    DrawCircle(videoBuffer, circlePos, circleRad);

    float stepCount = 1000;
    float2 samplePos = mousePos;
    scene->perspective += input->deltaTime * input->MouseLeft;
    scene->perspective -= input->deltaTime * input->MouseRight;
    // sum version
    float2 direction = float2(cos(scene->perspective), sin(scene->perspective));
    float2 nearHit = float2(-1, -1);
    float2 farHit = float2(-1, -1);

    float tiling = 30.0f;

    float sum = 0;
    for (int i = 0; i < stepCount; i++)
    {
        samplePos += direction;
        bool inSphere = distance(circlePos, samplePos) < circleRad;
        if(inSphere)
        {
            if(nearHit.x == -1)
            {
                nearHit = samplePos;
            }
        }
        else
        {
            if(nearHit.x != -1 && farHit.x == -1)
                farHit = samplePos;
        }

        //if(inSphere)
        //sum += sin(samplePos.x / tiling)*0.5+0.5;
        DrawPixel(videoBuffer, samplePos);
        if(floor(mod(samplePos.x / tiling, 2.0f)) && inSphere)
        {
            DrawPixel(videoBuffer, samplePos);
            sum += 1;
        }
        DrawPixel(videoBuffer, float2(i, sum));
    }
    DrawCircle(videoBuffer, nearHit, 5);
    DrawCircle(videoBuffer, farHit, 5);
    
    char text[512] = {};
    Append(text, "nearHit: ");
    Append(text, nearHit);
    DrawFont(engineState->fontSpritesheet, videoBuffer, 0, 0, text);
    Clear(text, 512);
    Append(text, "farHit: ");
    Append(text, farHit);
    DrawFont(engineState->fontSpritesheet, videoBuffer, 0, 15, text);

    float volumeDepth = distance(farHit, nearHit);
    // pure function version
    for (int i = 0; i < stepCount; i++)
    {
        float x = i;

        float mouseX = (nearHit.x) / tiling;
        float offset = (-cos(mouseX)+mouseX)*0.5f;

        //float offset = floor(mouseX * 0.5) + floor(mod(mouseX, 2.0f)) * mod(mouseX, 1.0f);
        
        x = min(x, volumeDepth);
        x *= -direction.x;
        x += tiling;
        x /= tiling;
        x -= mouseX;
        x = min(x, volumeDepth/(tiling*2.0));
        //x = (-cos(x)+x)*0.5f;
        x = floor(x * 0.5) + floor(mod(x, 2.0f)) * mod(x, 1.0f);
        x *= tiling;
        x += offset * tiling;
        
        x /= -direction.x;

        DrawPixel(videoBuffer, float2(i, x + 1), float3(0.5,1,0.5));
    }
}