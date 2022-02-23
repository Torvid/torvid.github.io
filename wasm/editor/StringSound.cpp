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



void drawString(game_offscreen_buffer* VideoBuffer, float* Points, int Length, int PixelsPerSample, int offsetx = 0, int offsety = 0)
{
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

            *GetPixel(VideoBuffer, offsetx + (i * PixelsPerSample) + h, (VideoBuffer->Height / 2) + r + offsety) = 255 << 16 | 255 << 8 | 255;
        }
    }

}



void RenderStringSoundLine(game_offscreen_buffer* VideoBuffer, game_sound_output_buffer *SoundBuffer, game_input* Input, string_data* StringData)
{
    drawString(VideoBuffer, StringData->Pos, StringData->Segments, StringData->PixelsPerSample);
    
    if (SoundBuffer->SampleCount > 0)
    {
        float average = 0;
        int sCount = SoundBuffer->SampleCount;
        for (int32 i = 0; i < sCount; i++)
        {
            average += SoundBuffer->Samples[i] * 0.1f;
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
    
                float height = (SoundBuffer->Samples[i] * 0.1f) - average;
    
                AverageX += vecX * height;
                AverageY += vecY * height;
            }
            float magnitude = sqrt(AverageX * AverageX + AverageY * AverageY);
            StringData->Magnitudes[y] = magnitude * -0.01f;
        }
    }

    drawString(VideoBuffer, StringData->Magnitudes, 100, 6, 720, 20);

    //int rightoffset = 200;
    //int height = 2;
    //speed = DrawSlider(VideoBuffer, Input,
    //    rightoffset, height, 300, 20,
    //    speed, 0.01f, 0.5f);
    //
    //height += 26;
    //damping = DrawSlider(VideoBuffer, Input,
    //    rightoffset, height, 300, 20,
    //    damping, 0.0f, 0.005f);
    //
    //height += 26;
    //timescale = DrawSlider(VideoBuffer, Input,
    //    rightoffset, height, 300, 20,
    //    timescale, 0.0f, 1.0f);
    //
    //height += 26;
    //Segments = DrawSlider(VideoBuffer, Input,
    //    rightoffset, height, 300, 20,
    //    Segments, 4.0f, 200.0f);
    //
    //height += 26;
    //FTScale = DrawSlider(VideoBuffer, Input,
    //    rightoffset, height, 300, 20,
    //    FTScale, 0.0f, 50.0f);

    //print(timescale);
}

int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

void StringSound(game_offscreen_buffer* VideoBuffer, game_sound_output_buffer *SoundBuffer, game_input* Input, string_data* StringData)
{
    int MouseX = Input->MouseX % (StringData->PixelsPerSample * StringData->Segments);

    int16* SampleOut = SoundBuffer->Samples;
    int ExtraSteps = 1;

    int samples = SoundBuffer->SampleCount * StringData->timescale;
    if (StringData->timescale < 1.0f)
    {
        samples = (int)((float)SoundBuffer->SampleCount * StringData->timescale);
    }
    if (StringData->timescale == 0.0f)
    {
        samples = 1;
    }

    for (int j = 0; j < samples; j++)
    {
        
        //if (Input->Controllers[1].ActionUp.EndedDown)
        //{
        //    for (int32 i = 1; i < Segments -1; i++)
        //    {
        //        int32 Midpoint = Segments / 2;
        //        float x = ((float)(i - 40) / 0.5f);
        //        float v = (sinf(x) / x);
        //        if (isnan(v))
        //            v = 1;
        //        v *= 200;
        //        StringData->Pos[i] = v;
        //    }
        //    continue;
        //}

        float damp = (1.0f - StringData->damping);

        float Average = 0;
        for (int k = 0; k < ExtraSteps; k++)
        {
            int RelativeMouse = Input->MouseY - (VideoBuffer->Height / 2);
            if (Input->MouseLeft && Input->MouseX < 700 &&
                Input->MouseY < (VideoBuffer->Height / 2 + 300) &&
                Input->MouseY > (VideoBuffer->Height / 2 - 300))
            {
                int x = MouseX / StringData->PixelsPerSample;
                x = x < 0 ? 0 : x;
                StringData->Pos[    x] = RelativeMouse;
                StringData->Vel[    x] = 0;
                StringData->OldPos[ x] = RelativeMouse;
                StringData->OldVel[ x] = 0;

                StringData->Pos[0] = 0;
                StringData->Vel[0] = 0;
                StringData->OldPos[0] = 0;
                StringData->OldVel[0] = 0;

                StringData->Pos[StringData->Segments-1] = 0;
                StringData->Vel[StringData->Segments - 1] = 0;
                StringData->OldPos[StringData->Segments-1] = 0;
                StringData->OldVel[StringData->Segments-1] = 0;

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
        Average = min(max(Average, -32000), 32000); // prevent ear rape from int16 overflow
        
        if (StringData->timescale < 1.0f)
        {
            Average = 0;
        }
        *SampleOut++ = (int16)Average;
        *SampleOut++ = (int16)Average;
    }
}