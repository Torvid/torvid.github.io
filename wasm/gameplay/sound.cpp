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
    DrawRectangle(VideoBuffer, pos, size, float3(0.0, 0.0, 0.0), 0.25);
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


Sound* LoadSound(EngineState* engineState, const char* path, float Length)
{
    int sampleRate = 44100;
    int sampleCount = Length * sampleRate;

    int padding = 100;

    Sample* SoundLocation = PushArray(&engineState->soundArena, sampleCount + padding, Sample);
    Sound* newSound = &engineState->Sounds[engineState->CurrentSound];
    for (int i = 0; i < StringLength(path); i++)
    {
        engineState->SoundNames[engineState->CurrentSound][i] = path[i];
    }

    newSound->samples = SoundLocation;
    newSound->sampleCount = sampleCount;

    for (int i = 0; i < newSound->sampleCount + padding; i++)
    {
        newSound->samples[i] = Sample(0, 0);
    }

    engineState->PlatformReadSoundIntoMemory(SoundLocation, path);

    // Check values
    Sample a[100];
    for (int i = 0; i < 100; i++)
    {
        a[i] = SoundLocation[i];
    }

    engineState->CurrentSound++;
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
bool SoundPlaying(EngineState* engineState, int channel)
{
    return engineState->soundChannels[channel].playing;
}
void PlaySound(EngineState* engineState, int channel, Sound* sound, float volume = 1.0f, bool loop = false, float speed = 1.0f)
{
    if (engineState->CurrentSound <= 0)
        return; // no sounds loaded.

    engineState->soundChannels[channel].currentSample = 0;
    engineState->soundChannels[channel].sound = sound;
    engineState->soundChannels[channel].loop = loop;
    engineState->soundChannels[channel].volume = volume;
    //engineState->soundChannels[channel].speed = speed;
    engineState->soundChannels[channel].playing = true;
}

void StopSound(EngineState* engineState, int channel)
{
    engineState->soundChannels[channel].currentSample = 0;
    engineState->soundChannels[channel].sound = 0;
    engineState->soundChannels[channel].loop = true;
    engineState->soundChannels[channel].volume = 0;
    engineState->soundChannels[channel].speed = 1.0f;
    engineState->soundChannels[channel].playing = true;
}

void StopAllSounds(EngineState* engineState)
{
    for (int i = 0; i < ArrayCount(engineState->soundChannels); i++)
    {
        StopSound(engineState, i);
    }
}

void SetSoundChannelVolume(EngineState* engineState, int channel, float volume)
{
    engineState->soundChannels[channel].volume = volume;
}

void SetSoundChannelSpeed(EngineState* engineState, int channel, float speed)
{
    engineState->soundChannels[channel].speed = speed;
}

int SoundIndex(EngineState* engineState, Sound* sound)
{
    for (int i = 0; i < ArrayCount(engineState->Sounds); i++)
    {
        if (&engineState->Sounds[i] == sound)
            return i;
    }
    return 0;
}





void drawString(game_offscreen_buffer* VideoBuffer, float* Points, int Length, int PixelsPerSample, int offsetx = 0, int offsety = 0, float3 Color = {})
{
    uint32 BufferColor = MakeBufferColor(VideoBuffer, Color);
    for (int32 i = 0; i < Length - 1; i++)
    {
        int xDist = PixelsPerSample;
        float current = Points[i];
        float next = Points[i + 1];
        int yDist = next - current;

        int dist = (int)sqrt((float)xDist * (float)xDist + (float)yDist * (float)yDist);

        for (int j = 0; j < dist; j++)
        {
            //float t = (float)j / (float)PixelsPerSample;
            float t = (float)j / (float)(dist);

            float r = lerp(current, next, t);
            float h = lerp(0, PixelsPerSample, t);

            *GetPixel(VideoBuffer, offsetx + (i * PixelsPerSample) + h, r + offsety) = BufferColor;// 255 << 16 | 255 << 8 | 255;
        }
    }
}

void RenderFFT(game_offscreen_buffer* VideoBuffer, game_sound_output_buffer* SoundBuffer, game_input* Input, string_data* StringData, float2 Pos, float3 Color)
{
    if (SoundBuffer->SampleCount > 0)
    {
        float average = 0;
        int sCount = SoundBuffer->SampleCount;
        for (int32 i = 0; i < sCount; i++)
        {
            average += SoundBuffer->Samples[i].right * 0.1f;
        }
        average /= sCount;

        for (int y = 0; y < 100; y++)
        {
            float windingfrequency = 0.00005f * StringData->FTScale * (float)(y * y);
            float AverageX = 0;
            float AverageY = 0;
            for (int32 i = 0; i < SoundBuffer->SampleCount; i++)
            {
                float angle = windingfrequency * (float)i;
                float vecX = sin(angle);
                float vecY = cos(angle);

                float height = (SoundBuffer->Samples[i].right * 0.1f) - average;

                AverageX += vecX * height;
                AverageY += vecY * height;
            }
            float magnitude = (sqrt(AverageX * AverageX + AverageY * AverageY));
            StringData->Magnitudes[y] = magnitude * -0.01f;
        }
    }

    drawString(VideoBuffer, StringData->Magnitudes, StringData->Segments, StringData->PixelsPerSample, Pos.x, Pos.y, Color);
}

void RenderStringSoundLine(game_offscreen_buffer* VideoBuffer, game_sound_output_buffer* SoundBuffer, game_input* Input, string_data* StringData, float2 Pos, float3 Color)
{
    drawString(VideoBuffer, StringData->Pos, StringData->Segments, StringData->PixelsPerSample, Pos.x, Pos.y, Color);
}

void StringSound(game_offscreen_buffer* VideoBuffer, game_sound_output_buffer* SoundBuffer, game_input* Input, string_data* StringData, bool MouseIsOverUI, float2 Pos)
{
    int MouseX = clamp(((int)Input->MouseX - (int)Pos.x), 0, (StringData->PixelsPerSample * StringData->Segments));
    int MouseY = clamp((Input->MouseY - Pos.y), -200, 200);

    Sample* SampleOut = SoundBuffer->Samples;
    int ExtraSteps = 1;

    float t = StringData->timescale;
    int samples = SoundBuffer->SampleCount;// *StringData->timescale;

    float Average = 0;
    float SkipTime = 0;

    for (int j = 0; j < SoundBuffer->SampleCount; j++)
    {
        float damp = (1.0f - StringData->damping);

        SkipTime += (1.0f - StringData->timescale);
        bool Skip = false;
        if (SkipTime > 1.0f)
        {
            Skip = true;
            SkipTime -= 1.0f;
        }

        if (!Skip)
        {
            Average = 0;
            for (int k = 0; k < ExtraSteps; k++)
            {
                //int RelativeMouse = Input->MouseY - Pos.y;// (VideoBuffer->Height / 2);
                if (Input->MouseLeft && !MouseIsOverUI)
                {
                    int x = (MouseX) / StringData->PixelsPerSample;
                    x = x < 0 ? 0 : x;
                    StringData->Pos[x] = MouseY;
                    StringData->Vel[x] = 0;
                    StringData->OldPos[x] = MouseY;
                    StringData->OldVel[x] = 0;

                    StringData->Pos[0] = 0;
                    StringData->Vel[0] = 0;
                    StringData->OldPos[0] = 0;
                    StringData->OldVel[0] = 0;

                    StringData->Pos[StringData->Segments - 1] = 0;
                    StringData->Vel[StringData->Segments - 1] = 0;
                    StringData->OldPos[StringData->Segments - 1] = 0;
                    StringData->OldVel[StringData->Segments - 1] = 0;

                }
                for (int32 i = 1; i < StringData->Segments - 1; i++)
                {
                    float AverageNeighbourPos = (StringData->OldPos[(i - 1)] + StringData->OldPos[(i + 1)]) * 0.5;
                    float Delta = AverageNeighbourPos - StringData->OldPos[i]; // Vector from us towards the average of neighbours
                    StringData->Vel[i] += Delta * StringData->speed;
                    StringData->Vel[i] *= damp;
                    StringData->Pos[i] += StringData->OldVel[i];

                    Average += (StringData->Pos[i] * StringData->Pos[i]);
                }

                float* TempPos = StringData->Pos;
                float* TempVel = StringData->Vel;
                StringData->Pos = StringData->OldPos;
                StringData->Vel = StringData->OldVel;
                StringData->OldPos = TempPos;
                StringData->OldVel = TempVel;
            }
            Average /= (StringData->Segments * ExtraSteps);
            //Average = clamp(Average, -32000, 32000); // prevent ear rape from int16 overflow
        }

        int32 NewRight = (int32)SampleOut->right + (int32)(-Average);
        int32 NewLeft = (int32)SampleOut->left + (int32)(-Average);
        NewRight = clamp(NewRight, -32000, 32000);
        NewLeft = clamp(NewLeft, -32000, 32000);
        Sample sample = { (int16)NewRight, (int16)NewLeft };
        *SampleOut = sample;
        SampleOut++;
    }
}