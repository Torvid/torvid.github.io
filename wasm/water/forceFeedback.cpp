
#include "quote.h"

void ForceFeedback(GameMemory* memory, EngineState* engineState, GameInput* input, VideoBuffer* videoBuffer)
{
    if (input->SpaceDown)
        memory->initialized = false;

    FontData* fontData = (FontData*)(((uint8*)(engineState + 2)));
    Font* font = &fontData->font;
    if (!memory->initialized)
    {
        engineState->profilingData.zoom = 1;
        engineState->platformReadImageIntoMemory = memory->platformReadImageIntoMemory;
        engineState->platformReadSoundIntoMemory = memory->platformReadSoundIntoMemory;
        engineState->platformPrint = memory->platformPrint;
        engineState->platformSetForcefeedbackEffect = memory->platformSetForcefeedbackEffect;
        InitializeArena(&engineState->staticAssetsArena, Megabytes(12), (uint8*)(engineState + 1));
        engineState->fontSpritesheet = LoadImage(engineState, &engineState->staticAssetsArena, "font.tga", 192, 52);

        //Clear(fontData, Megabytes(2));
        memory->initialized = true;
    }
    Clear(fontData, sizeof(FontData));

    //DrawClear(videoBuffer, float3(0, 0, 0));
    //DrawCircle(videoBuffer, float2(200, 200), 100);
    float2 pos = float2(0, 0);
    if (Button(engineState, input, videoBuffer, &pos, "ForceType_Nothing"))
        memory->platformSetForcefeedbackEffect(ForceType_Nothing);
    if (Button(engineState, input, videoBuffer, &pos, "ForceType_SpringCenter"))
        memory->platformSetForcefeedbackEffect(ForceType_SpringCenter);
    if (Button(engineState, input, videoBuffer, &pos, "ForceType_SpringAwayFromCenter"))
        memory->platformSetForcefeedbackEffect(ForceType_SpringAwayFromCenter);
    if (Button(engineState, input, videoBuffer, &pos, "ForceType_SpringForward"))
        memory->platformSetForcefeedbackEffect(ForceType_SpringForward);
    if (Button(engineState, input, videoBuffer, &pos, "ForceType_Sine"))
        memory->platformSetForcefeedbackEffect(ForceType_Sine);
    if (Button(engineState, input, videoBuffer, &pos, "ForceType_SineFast"))
        memory->platformSetForcefeedbackEffect(ForceType_SineFast);
    if (Button(engineState, input, videoBuffer, &pos, "ForceType_ConstantForceForward"))
        memory->platformSetForcefeedbackEffect(ForceType_ConstantForceForward);
    if (Button(engineState, input, videoBuffer, &pos, "ForceType_Friction"))
        memory->platformSetForcefeedbackEffect(ForceType_Friction);
    if (Button(engineState, input, videoBuffer, &pos, "ForceType_Inertia"))
        memory->platformSetForcefeedbackEffect(ForceType_Inertia);
    if (Button(engineState, input, videoBuffer, &pos, "ForceType_Damper"))
        memory->platformSetForcefeedbackEffect(ForceType_Damper);
}