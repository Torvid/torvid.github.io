#pragma once
#include "quote.h"

// Call this to start a profile region
void ProfilerBeingSample(ProfilingData* data)
{
    if (data->threading)
        return;

    data->timeStack[data->currentDepth][data->frameNumber] = data->memory->platformTime();
    data->currentDepth++;
}

// Call this to end a profile region
void ProfilerEndSample(ProfilingData* data, const char* name)
{
    if (data->threading)
        return;

    data->enterTime[data->index][data->frameNumber] = data->timeStack[data->currentDepth - 1][data->frameNumber];
    data->exitTime[data->index][data->frameNumber] = data->memory->platformTime();
    for (int i = 0; i < 50; i++)
    {
        data->functionNames[data->index][i] = name[i];
    }
    data->depth[data->index] = data->currentDepth;
    data->currentDepth--;
    data->index++;
}

// Call this somewhere near the start of the frame
void ProfilerStart(ProfilingData* data, GameMemory* memory)
{
    data->memory = memory;
    //data->Global_End = data->Global_Start;
    int64 lastStart = data->Global_Start;
    data->Global_Start = memory->platformTime();
    data->Delta = (lastStart - data->Global_Start);
    if (data->Delta < 0)
        data->Delta *= -1;
    //ProfilerEndSample(data, "Outside");
    ProfilerBeingSample(data);
}

// Call this somewhere near the end of the frame
void ProfilerEnd(ProfilingData* data, GameMemory* memory)
{

    ProfilerEndSample(data, "Frame");
    data->index = 0;
    data->currentDepth = 0;
    data->frameNumber++;
    data->frameNumber %= timeSampleCount;
    data->Global_End = memory->platformTime();
    //data->Delta = data->Global_End - data->Global_Start;
    //ProfilerBeingSample(data);
}

