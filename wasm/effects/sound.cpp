#pragma once
#include "quote.h"

void DrawSoundBuffer(game_offscreen_buffer* VideoBuffer, game_sound_output_buffer* SoundBuffer, float3 Color)
{
    int LastX = 0;
    int LastY = 0;

    int sampleCount = SoundBuffer->SampleCount;
    int AverageSample = 0;
    if (sampleCount > 0)
    {
        for (int32 i = 0; i < SoundBuffer->SampleCount; i++)
        {
            AverageSample += (SoundBuffer->Samples[i].left / 10);
        }
        AverageSample /= SoundBuffer->SampleCount;
    }

    for (int32 i = 0; i < SoundBuffer->SampleCount; i++)
    {
        float t = (float)i / (float)sampleCount;

        int x = t*VideoBuffer->Width;
        int y = ((SoundBuffer->Samples[i].left / 10) - AverageSample) + VideoBuffer->Height/2;

        DrawLine(VideoBuffer, float2(LastX, LastY), float2(x, y), Color);
        LastX = x;
        LastY = y;
    }
}
void DrawSoundRaw(game_offscreen_buffer* VideoBuffer, Sound* sound, float2 pos, int StartSample, int EndSample)
{
    for (int x = StartSample; x < EndSample; x++)
    {
        int h = abs(sound->samples[x].right / 100);
        SetPixel(VideoBuffer, x + pos.x - StartSample, pos.y, 0xffffffff);
        for (int j = 0; j < h; j++)
        {
            SetPixel(VideoBuffer, x + pos.x - StartSample, j + pos.y, 0xffffffff);
        }
    }
}
void DrawSound(game_offscreen_buffer* VideoBuffer, Sound* sound, float2 pos, float2 size, int currentSample = 0)
{
    DrawRectangle(VideoBuffer, pos, size, float3(0.0, 0, 0), 0.25);
    if (sound == 0)
        return;

    float highestSample = 0;
    for (int i = 0; i < sound->sampleCount; i++)
    {
        int newSample = sound->samples[i].left;
        if (newSample > highestSample)
            highestSample = newSample;
    }

    int currentSamplePixel = ((float)currentSample / (float)sound->sampleCount) * size.x;

    for (int x = 0; x < size.x; x++)
    {
        float t = ((float)x) / ((float)size.x);

        int sampleIndex = sound->sampleCount * t;
        int samplesPerPixel = sound->sampleCount / size.x;

        float h = 0;
        float detail = 0.25f;
        for (int i = 0; i < samplesPerPixel * detail; i++)
        {
            int newSample = abs(sound->samples[sampleIndex + i].left);
            h += newSample;
        }
        h /= samplesPerPixel * detail;
        h /= highestSample;
        h *= size.y * 1.0;

        for (int j = 0; j < h; j++)
        {
            SetPixel(VideoBuffer, x + pos.x, j + pos.y, 0xffffffff);
        }

        if (currentSamplePixel == x)
        {
            for (int j = 0; j < size.y; j++)
            {
                SetPixel(VideoBuffer, x + pos.x, j + pos.y, 0xff00ff00);
            }
            for (int j = 0; j < size.y; j++)
            {
                SetPixel(VideoBuffer, x + pos.x+1, j + pos.y, 0xff00ff00);
            }
        }
    }
}
void DrawSound(game_offscreen_buffer* VideoBuffer, Sound* sound, float2* pos, float2 size, int currentSample = 0)
{
    DrawSound(VideoBuffer, sound, *pos + float2(0, 5), size, currentSample);
    pos->y += size.y+10;
}


Sound* LoadSound(game_memory* Memory, GameState* gameState, const char* path, float Length)
{
    int sampleRate = 44100;
    int sampleCount = Length * sampleRate;

    int padding = 100;

    Sample* SoundLocation = PushArray(&gameState->soundArena, sampleCount + padding, Sample);
    Sound* newSound = &gameState->Sounds[gameState->CurrentSound];
    newSound->samples = SoundLocation;
    newSound->sampleCount = sampleCount;

    for (int i = 0; i < newSound->sampleCount + padding; i++)
    {
        newSound->samples[i] = Sample(0, 0);
    }

    Memory->PlatformReadSoundIntoMemory(SoundLocation, path);


    // Check values
    Sample a[100];
    for (int i = 0; i < 100; i++)
    {
        a[i] = SoundLocation[i];
    }

    gameState->CurrentSound++;
    return newSound;
}

void AddToSample(Sample* OutSample, Sample NewSample, float volume)
{
    int32 NewSampleRight = (int32)OutSample->right + NewSample.right * volume;
    int32 NewSampleLeft = (int32)OutSample->left + NewSample.left * volume;
    NewSampleRight = clamp(NewSampleRight, -32000, 32000);
    NewSampleLeft = clamp(NewSampleLeft, -32000, 32000);
    OutSample->right = (int16)NewSampleRight;
    OutSample->left = (int16)NewSampleLeft;
}
void SetSampleVolume(Sample* OutSample, float volume)
{
    int32 NewSampleRight = (int32)OutSample->right * volume;
    int32 NewSampleLeft = (int32)OutSample->left * volume;
    NewSampleRight = clamp(NewSampleRight, -32000, 32000);
    NewSampleLeft = clamp(NewSampleLeft, -32000, 32000);
    OutSample->right = (int16)NewSampleRight;
    OutSample->left = (int16)NewSampleLeft;
}


void ClearSoundBuffer(game_sound_output_buffer* SoundBuffer)
{
    Sample* SampleOut = SoundBuffer->Samples;
    for (int i = 0; i < SoundBuffer->SampleCount; i++) // requests samples as 44100 hz, but out sound files only have 22050.
    {
        (*(SampleOut++)) = Sample(0, 0);
    }
}

void SetSoundBufferVolume(game_sound_output_buffer* SoundBuffer, float Volume)
{
    Sample* SampleOut = SoundBuffer->Samples;
    for (int i = 0; i < SoundBuffer->SampleCount; i++) // requests samples as 44100 hz, but out sound files only have 22050.
    {
        SetSampleVolume(SampleOut, Volume); Sample((*SampleOut).left * Volume, (*SampleOut).right * Volume);
        SampleOut++;
    }
}
void PlaySoundUpdate(game_sound_output_buffer* SoundBuffer, SoundChannel* channel)
{
    //, int* CurrentSample, float* SoundDelta, float Speed
    if (channel == 0 || channel->sound == 0 || channel->playing == false)
        return;

    Sound* sound = channel->sound;

    Sample* SampleOut = SoundBuffer->Samples;
    for (int i = 0; i < SoundBuffer->SampleCount; i++) // requests samples as 44100 hz, but out sound files only have 22050.
    {
        int SamplesToAdvance = 0;
        channel->soundDelta += channel->speed;
        while (channel->soundDelta > 1.0f)
        {
            channel->soundDelta -= 1.0f;
            SamplesToAdvance++;
        }

        channel->currentSample += SamplesToAdvance;

        if (channel->currentSample > sound->sampleCount)
        {
            if (channel->loop)
            {
                channel->currentSample = 0;
            }
            else
            {
                channel->playing = false;
                channel->volume = 0;
            }
        }

        if (channel->speed < 1.0f)
        {
            Sample CurrentSample = sound->samples[channel->currentSample];
            Sample NextSample = sound->samples[channel->currentSample + 1];
            Sample BlendedSample = Sample(lerp(CurrentSample.left,  NextSample.left,  channel->soundDelta), 
                                          lerp(CurrentSample.right, NextSample.right, channel->soundDelta));
            AddToSample(SampleOut, BlendedSample, channel->volume * 0.5f);
        }
        else
        {

            AddToSample(SampleOut, sound->samples[channel->currentSample], channel->volume * 0.25f);
        }

        *SampleOut++;
    }
}

void PlaySound(GameState* gameState, int channel, Sound* sound, float volume = 1.0f, bool loop = false, float speed = 1.0f)
{
    gameState->soundChannels[channel].currentSample = 0;
    gameState->soundChannels[channel].sound = sound;
    gameState->soundChannels[channel].loop = loop;
    gameState->soundChannels[channel].volume = volume;
    //gameState->soundChannels[channel].speed = speed;
    gameState->soundChannels[channel].playing = true;
}

void StopSound(GameState* gameState, int channel)
{
    gameState->soundChannels[channel].currentSample = 0;
    gameState->soundChannels[channel].sound = 0;
    gameState->soundChannels[channel].loop = true;
    gameState->soundChannels[channel].volume = 0;
    gameState->soundChannels[channel].speed = 1.0f;
    gameState->soundChannels[channel].playing = true;
}

int SoundIndex(GameState* gameState, Sound* sound)
{
    for (int i = 0; i < ArrayCount(gameState->Sounds); i++)
    {
        if (&gameState->Sounds[i] == sound)
            return i;
    }
    return 0;
}