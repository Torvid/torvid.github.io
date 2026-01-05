#pragma once

#include "quote.h"

/*
struct {

}

#pragma (pack, 1)
turn off padding / turn on padding

add an assert of packed and padded structs are different
ctime_assert()
assert()

headers within headers

upload_model
upload_texture // returns "reference"

fbx -> myfile -> game

wire_at
write_and_advance

xxd

what does transient mean
how does the UI work
    how do you get the names of variables
    how to you convert numbers to strings
    how do you do the minimizing UI

how does the UI work?
    button clicks


add code for checking if sizes have changed after reload, if they have reset instead of crashing


to test my imgui, recreate the full stomping grounds UI


+ infinite sliders
    exponential
    clamped
    2d (mouse)
    2d scaled
    bool version
    -1, 0, 1 version
    Make the infite sliders apply their value after all the game logic?

    ctrl, snap to valies
    shift, slow down
    alt, reset to default

OvO
for_array
zero_struct function
flags :o |= thing
A |= B : set the B flag to true
if(A & B) : check if B is set
A &= ~B : set the B flag to false
A ^= B : toggle B flag

wind sensor
pressure sensor
player heating the air?


layout_row_down

layout struct


__COUNTER__
logic counter


use 64 bit numbers for IDs

actor_t
serialization / loading
why _t
how do you do sub, add, mul, etc of wpos_t
    wpos_t ends up not being a problem because it's only used for positions that need to be stable

hard de-intersection?
   separating axis theorem

minkowski portal refinment
GJK Algorithm + Expanding polytope algorithm
*/

#include "wavefunctioncollapse.cpp"
#include "spatialhashing.cpp"
#include "ballgame.cpp"
#include "wirefun.cpp"
#include "movetowards.cpp"
#include "raymarching.cpp"
#include "font.cpp"
#include "forceFeedback.cpp"
#include "shallowwater.cpp"


#if WASM
#else
__declspec(dllexport)
#endif
extern "C" void gameUpdateAndRender(GameMemory* memory, GameInput* input, VideoBuffer* videoBuffer, game_sound_output_buffer* soundBuffer)
{
    int a = FromString(" 13 37");
    Assert(sizeof(EngineState) <= memory->transientStorageSize);
    InitGlobalFunctions(memory);

    EngineState* engineState = (EngineState*)memory->transientStorage;
    EngineSaveState* engineSaveState = (EngineSaveState*)memory->permanentStorage;

    ProfilerStart(&engineState->profilingData, memory);

    engineState->sliderID = 0;
    engineState->gameTime += input->deltaTime;
    
    //raymarching(memory, engineState, input, videoBuffer);
    //movetowards(memory, engineState, input, videoBuffer);
    //wirefun(memory, engineState, input, videoBuffer);I have 
    //FontReader(memory, engineState, input, videoBuffer);
    //spatialhashing(memory, engineState, input, videoBuffer);
    //wavefunctioncollapse(memory, engineState, input, videoBuffer);
    //ballgame(memory, engineState, engineSaveState, input, videoBuffer, soundBuffer);
    //ForceFeedback(memory, engineState, input, videoBuffer);
    //raymarching(memory, engineState, input, videoBuffer);
    
    shallowwater(memory, engineState, input, videoBuffer);

    ProfilerEnd(&engineState->profilingData, memory);
}