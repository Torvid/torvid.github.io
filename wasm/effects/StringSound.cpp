#pragma once

#include "intrinsics.cpp"
#include "quote.h"
#include "drawing.cpp"

//void RenderCross(game_offscreen_buffer *Buffer, int x, int y)
//{
//    int size = 8;
//    for (int i = -size; i < size; i++)
//    {
//        *GetPixel(Buffer, x + i, y) = 255 << 16 | 255 << 8 | 255;
//    }
//    for (int i = -size; i < size; i++)
//    {
//        *GetPixel(Buffer, x, y + i) = 255 << 16 | 255 << 8 | 255;;
//    }
//}
//
//void Clear(game_offscreen_buffer *Buffer)
//{
//    uint32* CurrentPixel = (uint32*)Buffer->Memory;
//    for (int y = 0; y < Buffer->Height; y++)
//    {
//        for (int x = 0; x < Buffer->Width; x++)
//        {
//            *CurrentPixel = 0 << 16 | 0 << 8 | 0;
//            CurrentPixel += 1;
//        }
//    }
//}



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
            float windingfrequency = 0.00005f * StringData->FTScale * (float)(y*y);
            float AverageX = 0;
            float AverageY = 0;
            for (int32 i = 0; i < SoundBuffer->SampleCount; i++)
            {
                float angle = windingfrequency * (float)i;
                float vecX = sinf(angle);
                float vecY = cosf(angle);
    
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

void RenderStringSoundLine(game_offscreen_buffer* VideoBuffer, game_sound_output_buffer *SoundBuffer, game_input* Input, string_data* StringData, float2 Pos, float3 Color)
{
    drawString(VideoBuffer, StringData->Pos, StringData->Segments, StringData->PixelsPerSample, Pos.x, Pos.y, Color);
    
}

int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

void StringSound(game_offscreen_buffer* VideoBuffer, game_sound_output_buffer *SoundBuffer, game_input* Input, string_data* StringData, bool MouseIsOverUI, float2 Pos)
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
        int32 NewLeft  = (int32)SampleOut->left  + (int32)(-Average);
        NewRight = clamp(NewRight, -32000, 32000);
        NewLeft = clamp(NewLeft, -32000, 32000);
        Sample sample = { (int16)NewRight, (int16)NewLeft };
        *SampleOut = sample;
        SampleOut++;
    }
}