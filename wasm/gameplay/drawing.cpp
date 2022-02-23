#pragma once
#include "quote.h"
#include "string.cpp"

#define FAST 0

void QueueDrawSphere(EngineState* engineState, transform t, Image* texture1, Image* texture2 = 0, int ShaderIndex = 0, bool selected = false, bool hovered = false)
{
    engineState->RenderCommands[engineState->NextRenderCommand].texture1 = texture1;
    engineState->RenderCommands[engineState->NextRenderCommand].texture2 = texture2;
    engineState->RenderCommands[engineState->NextRenderCommand].Type = RenderCommandType_Sphere;
    engineState->RenderCommands[engineState->NextRenderCommand].t = t;
    engineState->RenderCommands[engineState->NextRenderCommand].ShaderIndex = ShaderIndex;
    engineState->RenderCommands[engineState->NextRenderCommand].selected = selected;
    engineState->RenderCommands[engineState->NextRenderCommand].hovered = hovered;

    engineState->NextRenderCommand++;
}
void QueueDrawBox(EngineState* engineState, transform t, Image* texture1, Image* texture2 = 0, int ShaderIndex = 0, bool selected = false, bool hovered = false)
{
    engineState->RenderCommands[engineState->NextRenderCommand].texture1 = texture1;
    engineState->RenderCommands[engineState->NextRenderCommand].texture2 = texture2;
    engineState->RenderCommands[engineState->NextRenderCommand].Type = RenderCommandType_Box;
    engineState->RenderCommands[engineState->NextRenderCommand].t = t;
    engineState->RenderCommands[engineState->NextRenderCommand].ShaderIndex = ShaderIndex;
    engineState->RenderCommands[engineState->NextRenderCommand].selected = selected;
    engineState->RenderCommands[engineState->NextRenderCommand].hovered = hovered;
    engineState->NextRenderCommand++;
}
void QueueDrawSkyBox(EngineState* engineState, Image* SkyFaces[6])
{
    for (int i = 0; i < 6; i++)
    {
        engineState->RenderCommands[engineState->NextRenderCommand].cubemapTexture[i] = SkyFaces[i];
    }
    engineState->RenderCommands[engineState->NextRenderCommand].Type = RenderCommandType_Skybox;

    engineState->NextRenderCommand++;
}
void QueueDrawEffect(EngineState* engineState, transform t, Image* texture1, Image* texture2, int ShaderIndex, EffectData effect, float3 Color)
{
    engineState->RenderCommands[engineState->NextRenderCommand].effect = effect;
    engineState->RenderCommands[engineState->NextRenderCommand].t = t;
    engineState->RenderCommands[engineState->NextRenderCommand].Type = RenderCommandType_Effect;
    engineState->RenderCommands[engineState->NextRenderCommand].texture1 = texture1;
    engineState->RenderCommands[engineState->NextRenderCommand].texture2 = texture2;
    engineState->RenderCommands[engineState->NextRenderCommand].ShaderIndex = ShaderIndex;
    engineState->RenderCommands[engineState->NextRenderCommand].color = Color;

    engineState->NextRenderCommand++;
}
void QueueClear(EngineState* engineState, float3 Color)
{
    engineState->RenderCommands[engineState->NextRenderCommand].color = Color;
    engineState->RenderCommands[engineState->NextRenderCommand].Type = RenderCommandType_Clear;
    engineState->NextRenderCommand++;
}

uint32 MakeBufferColorNoChange(game_offscreen_buffer* Buffer, float3 Color)
{
    return  0 << 24 |
        RoundToInt(Color.x * 255.0f) << 16 |
        RoundToInt(Color.y * 255.0f) << 8 |
        RoundToInt(Color.z * 255.0f) << 0;
}
uint32 MakeBufferReverse(game_offscreen_buffer* Buffer, float3 Color)
{
    if (Buffer->Layout == ChannelLayout_ABGR)
    {
        return 0 << 24 |
            RoundToInt(Color.x * 255.0f) << 16 |
            RoundToInt(Color.y * 255.0f) << 8 |
            RoundToInt(Color.z * 255.0f) << 0;
    }
    else
    {
        return  0 << 24 |
            RoundToInt(Color.z * 255.0f) << 16 |
            RoundToInt(Color.y * 255.0f) << 8 |
            RoundToInt(Color.x * 255.0f) << 0;
    }
}
uint32 MakeBufferColor(game_offscreen_buffer* Buffer, float3 Color)
{
    if (Buffer->Layout == ChannelLayout_ABGR)
    {
        return  0 << 24 |
            RoundToInt(Color.z * 255.0f) << 16 |
            RoundToInt(Color.y * 255.0f) << 8 |
            RoundToInt(Color.x * 255.0f) << 0;
    }
    else
    {
        return 0 << 24 |
            RoundToInt(Color.x * 255.0f) << 16 |
            RoundToInt(Color.y * 255.0f) << 8 |
            RoundToInt(Color.z * 255.0f) << 0;
    }
}
uint32 MakeBufferColor(game_offscreen_buffer* Buffer, float Color)
{
    return MakeBufferColor(Buffer, float3(Color, Color, Color));
}
float3 GetBufferColorNoChange(game_offscreen_buffer* Buffer, uint32 Color)
{
    return float3(((uint8*)&Color)[2] / 256.0f,
                  ((uint8*)&Color)[1] / 256.0f,
                  ((uint8*)&Color)[0] / 256.0f);
}
float3 GetBufferColor(game_offscreen_buffer* Buffer, uint32 Color)
{

    if (Buffer->Layout == ChannelLayout_ABGR)
    {
        return float3(((uint8*)&Color)[2] / 256.0f,
                      ((uint8*)&Color)[1] / 256.0f,
                      ((uint8*)&Color)[0] / 256.0f);
    }
    else
    {
        return float3(((uint8*)&Color)[0] / 256.0f,
                      ((uint8*)&Color)[1] / 256.0f,
                      ((uint8*)&Color)[2] / 256.0f);
    }
}
float3 GetBufferColorReverse(game_offscreen_buffer* Buffer, uint32 Color)
{

    if (Buffer->Layout == ChannelLayout_ABGR)
    {
        return float3(((uint8*)&Color)[0] / 256.0f,
            ((uint8*)&Color)[1] / 256.0f,
            ((uint8*)&Color)[2] / 256.0f);
    }
    else
    {
        return float3(((uint8*)&Color)[2] / 256.0f,
            ((uint8*)&Color)[1] / 256.0f,
            ((uint8*)&Color)[0] / 256.0f);
    }
}

int2 GetThreadRegion(EngineState* engineState, float Regions, float Region, int Height, int MinY, int MaxY)
{
    if (Region >= 0)
    {
        float Start = engineState->RenderThreadStart[(int)Region] * Height;
        float End = engineState->RenderThreadEnd[(int)Region] * Height;
        //float Start = (Region / (Regions)) * Height;
        //float End = ((Region + 1) / (Regions)) * Height;

        if (Start > MinY)
            MinY = Start;

        if (End < MaxY)
            MaxY = End;
    }
    return int2(MinY, MaxY);
}

void DrawRectangle(game_offscreen_buffer* Buffer,
    float2 Pos,
    float2 Size, float3 Color, float A = 1)
{
    int MinX = FloorToInt(Pos.x);
    int MinY = FloorToInt(Pos.y);
    int MaxX = FloorToInt(Pos.x + Size.x);
    int MaxY = FloorToInt(Pos.y + Size.y);

    if (MinX < 0)
        MinX = 0;
    if (MinY < 0)
        MinY = 0;

    if (MaxX > Buffer->Width)
        MaxX = Buffer->Width;
    if (MaxY > Buffer->Height)
        MaxY = Buffer->Height;

    uint8* Row = ((uint8*)Buffer->Memory + MinY * Buffer->Pitch + MinX * Buffer->BytesPerPixel);
    // AA RR GG BB
    uint32 BufferColor = MakeBufferColor(Buffer, Color);
    if (A == 1)
    {
        for (int y = MinY; y < MaxY; y++)
        {
            uint32* Pixel = (uint32*)Row;
            for (int x = MinX; x < MaxX; x++)
            {
                *Pixel = BufferColor;// MakeBufferColor(Buffer, BufferColor);
                //((uint8*)Pixel)[0] = ((uint8*)&BufferColor)[0];
                //((uint8*)Pixel)[1] = ((uint8*)&BufferColor)[1];
                //((uint8*)Pixel)[2] = ((uint8*)&BufferColor)[2];
                //((uint8*)Pixel)[3] = ((uint8*)&BufferColor)[3];
                Pixel++;
            }
            Row += Buffer->Pitch;
        }
    }
    else
    {
        for (int y = MinY; y < MaxY; y++)
        {
            uint32* Pixel = (uint32*)Row;
            for (int x = MinX; x < MaxX; x++)
            {
                ////float wa = ((uint8*)&Color)[3] / 256.0f;
                //wr = wr * wr;//pow(wr, 2.2f);
                //wg = wg * wg;//pow(wg, 2.2f);
                //wb = wb * wb;//pow(wb, 2.2f);
                ////wa = wa * wa;//pow(wa, 2.2f);
                //
                ////float va = ((uint8*)Pixel)[3] / 256.0f;
                //
                //// This is meant to be pow 2.2 and 0.454545, but it's too slow.
                //vr = vr * vr;//pow(vr, 2.2f);
                //vg = vg * vg;//pow(vg, 2.2f);
                //vb = vb * vb;//pow(vb, 2.2f);
                ////va = va * va;//pow(va, 2.2f);
                //
                //vr *= 1.0 - A;
                //vg *= 1.0 - A;
                //vb *= 1.0 - A;
                ////va *= 1.0 - A;
                //vr += wr * A;
                //vg += wg * A;
                //vb += wb * A;
                ////va += wa * A;
                //
                //vr = sqrt(vr); //pow(vr, 0.4545f);
                //vg = sqrt(vg); //pow(vg, 0.4545f);
                //vb = sqrt(vb); //pow(vb, 0.4545f);
                ////va = sqrt(va); //pow(va, 0.4545f);
                float3 v = GetBufferColor(Buffer, *Pixel);
                float3 w = GetBufferColor(Buffer, BufferColor);
                //float vr = ((uint8*)Pixel)[0] / 256.0f;
                //float vg = ((uint8*)Pixel)[1] / 256.0f;
                //float vb = ((uint8*)Pixel)[2] / 256.0f;
                //
                //float wr = ((uint8*)&BufferColor)[0] / 256.0f;
                //float wg = ((uint8*)&BufferColor)[1] / 256.0f;
                //float wb = ((uint8*)&BufferColor)[2] / 256.0f;
                *Pixel = MakeBufferReverse(Buffer, BlendColor(w, v, A));

                //float3 BlendedColor = BlendColor(w, v, A);
                //((uint8*)Pixel)[0] = BlendedColor.x * 256.0f;
                //((uint8*)Pixel)[1] = BlendedColor.y * 256.0f;
                //((uint8*)Pixel)[2] = BlendedColor.z * 256.0f;
                //((uint8*)Pixel)[3] = va * 256.0f;

                Pixel++;
            }
            Row += Buffer->Pitch;
        }
    }
}
uint32 ImageSample(Image* image, float x, float y)
{
    if (!Valid(image))
        return ToColor(float3(1, 0, 1), 1);

    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;
    if (x > 1)
        x = 1;
    if (y > 1)
        y = 1;
    uint32 PixelX = x * image->width;
    uint32 PixelY = y * image->height;
    return image->ImageData[(PixelY)*image->width + (PixelX)];
}
uint32 ImageSample(Image* image, float2 UV)
{
    return ImageSample(image, UV.x, UV.y);
}

uint32 ImageSampleBiliniar(Image* image, float x, float y)
{
    float PixelSizeX = 1.0f / image->width;
    float PixelSizeY = 1.0f / image->height;
    x -= PixelSizeX * 0.5;
    y -= PixelSizeY * 0.5;

    uint8 image_current_x_int = FloorToInt(x * image->width * 256);
    uint8 image_current_y_int = FloorToInt(y * image->height * 256);

    uint32 Centerpixel = ImageSample(image, x + 0, y + 0);
    uint32 Rightpixel = ImageSample(image, x + PixelSizeX, y + 0);
    uint32 Downpixel = ImageSample(image, x + 0, y + PixelSizeY);
    uint32 DownRightpixel = ImageSample(image, x + PixelSizeX, y + PixelSizeY);

    uint8 Bpixel = ((uint8*)(&Centerpixel))[0];
    uint8 Gpixel = ((uint8*)(&Centerpixel))[1];
    uint8 Rpixel = ((uint8*)(&Centerpixel))[2];
    uint8 Apixel = ((uint8*)(&Centerpixel))[3];
    uint8 BRightpixel = ((uint8*)(&Rightpixel))[0];
    uint8 GRightpixel = ((uint8*)(&Rightpixel))[1];
    uint8 RRightpixel = ((uint8*)(&Rightpixel))[2];
    uint8 ARightpixel = ((uint8*)(&Rightpixel))[3];
    uint8 BDownpixel = ((uint8*)(&Downpixel))[0];
    uint8 GDownpixel = ((uint8*)(&Downpixel))[1];
    uint8 RDownpixel = ((uint8*)(&Downpixel))[2];
    uint8 ADownpixel = ((uint8*)(&Downpixel))[3];
    uint8 BDownRightpixel = ((uint8*)(&DownRightpixel))[0];
    uint8 GDownRightpixel = ((uint8*)(&DownRightpixel))[1];
    uint8 RDownRightpixel = ((uint8*)(&DownRightpixel))[2];
    uint8 ADownRightpixel = ((uint8*)(&DownRightpixel))[3];

    uint8 R1 = lerp(Bpixel, BRightpixel, (image_current_x_int));
    uint8 G1 = lerp(Gpixel, GRightpixel, (image_current_x_int));
    uint8 B1 = lerp(Rpixel, RRightpixel, (image_current_x_int));
    uint8 A1 = lerp(Apixel, ARightpixel, (image_current_x_int));
    uint8 R2 = lerp(BDownpixel, BDownRightpixel, (image_current_x_int));
    uint8 G2 = lerp(GDownpixel, GDownRightpixel, (image_current_x_int));
    uint8 B2 = lerp(RDownpixel, RDownRightpixel, (image_current_x_int));
    uint8 A2 = lerp(ADownpixel, ADownRightpixel, (image_current_x_int));
    uint8 R = lerp(R1, R2, (image_current_y_int));
    uint8 G = lerp(G1, G2, (image_current_y_int));
    uint8 B = lerp(B1, B2, (image_current_y_int));
    uint8 A = lerp(A1, A2, (image_current_y_int));

    return (A) << 24 | (B) << 16 | (G) << 8 | (R) << 0;
}
uint32 ImageSampleBiliniar(Image* image, float2 UV)
{
    return ImageSampleBiliniar(image, UV.x, UV.y);
}

void DrawImageScaledSmooth(game_offscreen_buffer* Buffer, Image* image,
    float X, float Y,
    float Width, float Height)
{
    int MinX = FloorToInt(X);
    int MinY = FloorToInt(Y);
    int MaxX = FloorToInt(X + Width);
    int MaxY = FloorToInt(Y + Height);

    int IntX = FloorToInt(X);
    int IntY = FloorToInt(Y);

    float outputWidth = MaxX - MinX;
    float outputHeight = MaxY - MinY;

    uint8 a = 200;
    uint8 b = 128;
    uint8 c = (a * b) / 256;

    if (MinX < 0)
        MinX = 0;
    if (MinY < 0)
        MinY = 0;

    if (MaxX > Buffer->Width)
        MaxX = Buffer->Width;
    if (MaxY > Buffer->Height)
        MaxY = Buffer->Height;

    uint8* Row = ((uint8*)Buffer->Memory + MinY * Buffer->Pitch + MinX * Buffer->BytesPerPixel);
    float ratioX = (image->width / outputWidth);
    float ratioY = (image->height / outputHeight);


    for (int y = MinY; y < MaxY; y++)
    {
        uint32* Pixel = (uint32*)Row;
        for (int x = MinX; x < MaxX; x++)
        {
            float image_current_x = (x - X) * ratioX;
            float image_current_y = (y - Y) * ratioY;

            uint32 NewPixel = ImageSampleBiliniar(image, (image_current_x) / image->width, (image_current_y) / image->height);

            *Pixel++ = NewPixel;
        }
        Row += Buffer->Pitch;
    }
}

//int safe_add(int a, int b) {
//    if (a >= 0) {
//        if (b > INT_MAX - a) {
//            /* handle overflow */
//        }
//        else {
//            return a + b;
//        }
//    }
//    else {
//        if (b < INT_MIN - a) {
//            /* handle negative overflow */
//        }
//        else {
//            return a + b;
//        }
//    }
//}

void DrawImageScaled(EngineState* engineState, game_offscreen_buffer* Buffer, Image* image, float2 Pos, float2 Size, float3 Color, int Shader = 0, int Region = -1, int Regions = 9, uint8 depth = 0)
{
    if (!Valid(image))
    {
        float2 HalfSize = Size * 0.25;
        for (int x = 0; x < 4; x++)
        {
            for (int y = 0; y < 4; y++)
            {
                int i = (x + y) % 2;
                DrawRectangle(Buffer, Pos + float2(x, y) * HalfSize, HalfSize, float3(i, 0, i));
            }
        }
        return;
    }

    int MinX = FloorToInt(Pos.x);
    int MinY = FloorToInt(Pos.y);
    int MaxX = FloorToInt(Pos.x + Size.x);
    int MaxY = FloorToInt(Pos.y + Size.y);

    int IntX = FloorToInt(Pos.x);
    int IntY = FloorToInt(Pos.y);

    float outputWidth = MaxX - MinX;
    float outputHeight = MaxY - MinY;

    int2 ChangedSize = GetThreadRegion(engineState, Regions, Region, Buffer->Height, MinY, MaxY);
    MinY = ChangedSize.x;
    MaxY = ChangedSize.y;

    if (MinX < 0)
        MinX = 0;
    if (MinY < 0)
        MinY = 0;

    if (MaxX > Buffer->Width)
        MaxX = Buffer->Width;
    if (MaxY > Buffer->Height)
        MaxY = Buffer->Height;

    uint8* Row = ((uint8*)Buffer->Memory + MinY * Buffer->Pitch + MinX * Buffer->BytesPerPixel);

    for (int y = MinY; y < MaxY; y++)
    {
        uint32* Pixel = (uint32*)Row;
        for (int x = MinX; x < MaxX; x++)
        {
            int image_x = (x - IntX) * (image->width / outputWidth);
            int image_y = (y - IntY) * (image->height / outputHeight);
            if (Shader == 0) // Opaque
            {
                uint32 OldPixelData = *Pixel;
                bool clippedVisible = depth < ((uint8*)(&OldPixelData))[3];
                if (clippedVisible)
                    *Pixel = image->ImageData[image_y * image->width + image_x];
            }
            else if(Shader == 1) // Additive
            {
                uint32 NewPixelData = image->ImageData[image_y * image->width + image_x];
                uint32 OldPixelData = *Pixel;
            
                float3 NewPixel = GetBufferColorReverse(Buffer, NewPixelData) * Color;
                float3 OldPixel = GetBufferColorReverse(Buffer, OldPixelData);
            
                float3 Result = clamp01(NewPixel + OldPixel);
            
                uint32 ResultData = MakeBufferColor(Buffer, Result);
                bool clippedVisible = depth < ((uint8*)(&OldPixelData))[3];
                if (clippedVisible)
                    *Pixel = ResultData;
            }
            else if (Shader == 2)
            {
                uint32 NewPixelData = image->ImageData[image_y * image->width + image_x];
                uint32 OldPixelData = *Pixel;
            
                bool alphaVisible = ((uint8*)(&NewPixelData))[3] > 128;
                bool clippedVisible = depth < ((uint8*)(&OldPixelData))[3];
            
                if (clippedVisible && alphaVisible)
                    *Pixel = NewPixelData;
            }
            else
            {
                uint32 OldPixelData = *Pixel;
                *Pixel = image->ImageData[image_y * image->width + image_x];
            }

            Pixel++;
        }
        Row += Buffer->Pitch;
    }
}

void DrawImage(game_offscreen_buffer* Buffer, Image* image, float X, float Y)
{
    float FloatMinX = X;
    float FloatMinY = Y;
    float FloatMaxX = X + image->width;
    float FloatMaxY = Y + image->height;

    int IntX = FloorToInt(FloatMinX);
    int IntY = FloorToInt(FloatMinY);

    int MinX = FloorToInt(FloatMinX);
    int MinY = FloorToInt(FloatMinY);
    int MaxX = FloorToInt(FloatMaxX);
    int MaxY = FloorToInt(FloatMaxY);

    if (MinX < 0)
        MinX = 0;
    if (MinY < 0)
        MinY = 0;

    if (MaxX > Buffer->Width)
        MaxX = Buffer->Width;
    if (MaxY > Buffer->Height)
        MaxY = Buffer->Height;

    uint8* Row = ((uint8*)Buffer->Memory + MinY * Buffer->Pitch + MinX * Buffer->BytesPerPixel);

    for (int y = MinY; y < MaxY; y++)
    {
        uint32* Pixel = (uint32*)Row;
        for (int x = MinX; x < MaxX; x++)
        {
            *Pixel++ = image->ImageData[(y - IntY) * image->width + (x - IntX)];
        }
        Row += Buffer->Pitch;
    }
}

void DrawMemory(game_offscreen_buffer* Buffer, uint8* InputBuffer, int count)
{
    uint8* CurrentBuffer = InputBuffer;
    int CurrentCount = 0;
    uint8* Row = (uint8*)Buffer->Memory + 0 * Buffer->Pitch + 0 * Buffer->BytesPerPixel;
    for (int y = 0; y < Buffer->Height; y++)
    {
        uint32* Pixel = (uint32*)Row;
        for (int x = 0; x < Buffer->Width; x++)
        {
            CurrentCount++;
            if (CurrentCount > count)
                return;

            float value = *CurrentBuffer / 255.0f;
            float3 NewColor = float3(value, value, value);
            float3 OldColor = GetBufferColor(Buffer, *Pixel);

            //*Pixel = MakeBufferColor(Buffer, lerp(OldColor, NewColor, 0.75f));
            *Pixel = MakeBufferColor(Buffer, NewColor);
            Pixel++;
            CurrentBuffer++;
        }
        Row += Buffer->Pitch;
    }
}

void DarkenBuffer(game_offscreen_buffer* Buffer)
{
    uint8* Row = (uint8*)Buffer->Memory;
    for (int y = 0; y < Buffer->Height; y++)
    {
        uint32* Pixel = (uint32*)Row;
        for (int x = 0; x < Buffer->Width; x++)
        {
            float3 OldColor = GetBufferColor(Buffer, *Pixel);
            *Pixel = MakeBufferReverse(Buffer, clamp01(OldColor * 0.5f));
            Pixel++;
        }
        Row += Buffer->Pitch;
    }
}

internal uint32* GetPixel(game_offscreen_buffer* Buffer, int y, int x)
{
    x = (int)min(max((double)x, 0.0), (double)(Buffer->Height - 1));
    y = (int)min(max((double)y, 0.0), (double)(Buffer->Width - 1));

    uint32 offset = (x * Buffer->Width + y);
    return (uint32*)Buffer->Memory + offset;
}

void SetPixel(game_offscreen_buffer* VideoBuffer, int x, int y, uint32 Color)
{
    //x = (int)min(max((float)x, 0.0), (float)(VideoBuffer->Width - 1));
    //y = (int)min(max((float)y, 0.0), (float)(VideoBuffer->Height - 1));
    x = clamp(x, 0, VideoBuffer->Width - 1);
    y = clamp(y, 0, VideoBuffer->Height - 1);
    uint32 offset = (y * VideoBuffer->Width + x);
    *((uint32*)VideoBuffer->Memory + offset) = Color;
}


void DrawPoint(game_offscreen_buffer* Buffer, float2 pos)
{
    *GetPixel(Buffer, pos.x, pos.y) = 0xFFFFFFFF;
    *GetPixel(Buffer, pos.x + 1, pos.y) = 0xFFFFFFFF;
    *GetPixel(Buffer, pos.x, pos.y + 1) = 0xFFFFFFFF;
    *GetPixel(Buffer, pos.x - 1, pos.y) = 0xFFFFFFFF;
    *GetPixel(Buffer, pos.x, pos.y - 1) = 0xFFFFFFFF;
}

void DrawCross(game_offscreen_buffer* Buffer, int x, int y)
{
    int size = 8;
    for (int i = -size; i < size; i++)
    {
        *GetPixel(Buffer, x + i, y) = 255 << 16 | 255 << 8 | 255;
    }
    for (int i = -size; i < size; i++)
    {
        *GetPixel(Buffer, x, y + i) = 255 << 16 | 255 << 8 | 255;;
    }
}

void DrawCircle(game_offscreen_buffer* Buffer, float2 pos, float radius)
{
    for (float i = 0; i < (int)(3.14152128f * 2.3f); i += (1.0f / radius))
    {
        float2 a = pos + float2(sin(i) * radius, cos(i) * radius);
        *GetPixel(Buffer, a.x, a.y) = 0xFFFFFFFF;
    }
    DrawPoint(Buffer, pos);
}

void DrawLine(game_offscreen_buffer* Buffer, float2 Start, float2 End, float3 Color, float A = 0.5)
{
    if ((Start.x < 0  || Start.y < 0 || Start.y > Buffer->Height || Start.x > Buffer->Width) && 
        (End.y > Buffer->Height || End.x < 0 || End.y < 0 || End.x > Buffer->Width))
        return;

    float dist = distance(Start, End);
    for (float i = 0; i < 1.0f; i += (1.0f / dist))
    {
        float2 l = lerp(Start, End, i);
        //BlendColor()
        *GetPixel(Buffer, l.x, l.y) = MakeBufferColor(Buffer, Color);
    }
}
void DrawLine(game_offscreen_buffer* Buffer, float2 Start, float2 End)
{
    DrawLine(Buffer, Start, End, float3(1, 1, 1));
}

void DrawBox(game_offscreen_buffer* Buffer, float2 Pos, float2 Size)
{
    float2 TopLeft = Pos + Size * float2(-0.5, 0.5);
    float2 TopRight = Pos + Size * float2(0.5, 0.5);
    float2 BottomLeft = Pos + Size * float2(0.5, -0.5);
    float2 BottomRight = Pos + Size * float2(-0.5, -0.5);
    DrawLine(Buffer, TopLeft, TopRight);
    DrawLine(Buffer, TopRight, BottomLeft);
    DrawLine(Buffer, BottomLeft, BottomRight);
    DrawLine(Buffer, BottomRight, TopLeft);
    DrawPoint(Buffer, Pos);
}
void DrawBox(game_offscreen_buffer* Buffer, float2 Pos, float2 Size, float2 Up, float2 Right)
{
    float2 TopLeft = Size * float2(-0.5, 0.5);
    float2 TopRight = Size * float2(0.5, 0.5);
    float2 BottomLeft = Size * float2(0.5, -0.5);
    float2 BottomRight = Size * float2(-0.5, -0.5);

    TopLeft     = Pos + float2(dot(TopLeft,     Right), dot(TopLeft,       Up));
    TopRight    = Pos + float2(dot(TopRight,    Right), dot(TopRight,      Up));
    BottomLeft  = Pos + float2(dot(BottomLeft,  Right), dot(BottomLeft,    Up));
    BottomRight = Pos + float2(dot(BottomRight, Right), dot(BottomRight,   Up));

    DrawLine(Buffer, TopLeft, TopRight);
    DrawLine(Buffer, TopRight, BottomLeft);
    DrawLine(Buffer, BottomLeft, BottomRight);
    DrawLine(Buffer, BottomRight, TopLeft);
    DrawPoint(Buffer, Pos);
}

const int PixelWidth = 192;
const int PixelHeight = 52;
const int CharactersWidth = 32;
const int CharactersHeight = 4;
const int SpriteWidth = 6;
const int SpriteHeight = 13;

int GetFontPos(const char c)
{
	if (c >= 65 && c <= 90) // Capital letters
		return c - 65;
	
	if (c >= 97 && c <= 122) // Lowercase letters
		return (c - 97) + 32;
	
	if (c >= 48 && c <= 57) // Numbers
		return (c - 48) + 64;
	
	switch (c)
	{
		//case '£':  return 74;
		case '$':  return 75;
			//case '€':  return 76;
			//case '¥':  return 77;
		case '!':  return 96 + 0;
		case '?':  return 96 + 1;
		case '#':  return 96 + 2;
		case '&':  return 96 + 3;
		case '_':  return 96 + 4;
		case '%':  return 96 + 5;
		case '\'': return 96 + 6;
		case ',':  return 96 + 7;
		case '.':  return 96 + 8;
		case ';':  return 96 + 9;
		case ':':  return 96 + 10;
		case '^':  return 96 + 11;
		case '|':  return 96 + 12;
			//case '§':  return 96 + 13;
			//case '½':  return 96 + 14;
		case '`':  return 96 + 15;
			//case '´':  return 96 + 16;
		case '~':  return 96 + 17;
		case '=':  return 96 + 18;
		case '<':  return 96 + 19;
		case '>':  return 96 + 20;
		case '+':  return 96 + 21;
		case '-':  return 96 + 22;
		case '*':  return 96 + 23;
		case '/':  return 96 + 24;
		case '\\': return 96 + 25;
		case '(':  return 96 + 26;
		case ')':  return 96 + 27;
		case '{':  return 96 + 28;
		case '}':  return 96 + 29;
		case '[':  return 96 + 30;
		case ']':  return 96 + 31;
		case ' ':  return 78;
		case '\n': return -1;
	}
	return 78; // empty
}

char GetFontPixel(Image* fontSpritesheet, int CharX, int CharY, int X, int Y)
{
	int index = 0;
	index += CharX * SpriteWidth;
	index += CharY * SpriteHeight * PixelWidth;
	index += X;
	index += Y * PixelWidth;
	return fontSpritesheet->ImageData[index];
}

void font_draw(Image* fontSpritesheet, game_offscreen_buffer* VideoBuffer, float X, float Y, const char* text, float R = 1.0, float G = 1.0, float B = 1.0, bool transparent = true, int LengthOverride = 0)
{
	X++;
	Y++;
	int TextLength = StringLength(text);
	if (LengthOverride > 0)
		TextLength = LengthOverride;
	int OffsetX = 0;
	int OffsetY = 0;
	for (int i = 0; i < TextLength; i++)
	{
		int f = GetFontPos(text[i]);
		if (f == -1) // newline
		{
			OffsetX = 0;
			OffsetY += SpriteHeight;
			continue;
		}
		for (int y = 0; y < SpriteHeight; y++)
		{
			for (int x = 0; x < SpriteWidth; x++)
			{
				char c = GetFontPixel(fontSpritesheet, f % 32, f / 32, x, y);
				//char c = fontSpritesheet->Sprites[f % 32][f / 32].data[x][y];
				char BB = R * 255;
				char GG = G * 255;
				char RR = B * 255;
				uint32 TextColor = 255 << 24 | BB << 16 | GG << 8 | RR;
				uint32 BackgroundColor = 0 << 16 | 0 << 8 | 0;
				if (transparent)
				{
					if (c == 0)
						SetPixel(VideoBuffer, OffsetX + x + X, OffsetY + y + Y, TextColor);
				}
				else
				{
					if (c == 0)
						SetPixel(VideoBuffer, OffsetX + x + X, OffsetY + y + Y, TextColor);
					else
						SetPixel(VideoBuffer, OffsetX + x + X, OffsetY + y + Y, BackgroundColor);
					
				}
			}
		}
		OffsetX += SpriteWidth;
	}
}
// asks the platform layer to start loading an image.
Image* LoadImage(EngineState* engineState, const char* path, int width, int height)
{
    uint32* ImageLocation = PushArray(&engineState->textureArena, width * height, uint32);
    Image* newImage = &engineState->Images[engineState->CurrentImage];
    for (int i = 0; i < StringLength(path); i++)
    {
        engineState->ImageNames[engineState->CurrentImage][i] = path[i];
    }
    newImage->ImageData = ImageLocation;
    newImage->width = width;
    newImage->height = height;

    // Fill image with missing image image
    int cells = 2;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            bool a = max((x % (width / cells)) - (width / cells / 2), 0);
            bool b = max((y % (width / cells)) - (width / cells / 2), 0);
            bool c = a ^ b;
            ((char*)newImage->ImageData)[(width * y + x) * 4 + 0] = c ? 255 : 0;
            ((char*)newImage->ImageData)[(width * y + x) * 4 + 1] = 0;
            ((char*)newImage->ImageData)[(width * y + x) * 4 + 2] = c ? 255 : 0;
            ((char*)newImage->ImageData)[(width * y + x) * 4 + 3] = 255;
        }
    }

    engineState->PlatformReadImageIntoMemory(ImageLocation, path);

    engineState->CurrentImage++;
    return newImage;
}
float3 RayPlaneIntersectWorldSpace(float3 RayPos, float3 RayDir, float3 planeP, float3 planeN)
{
    float d = dot(planeP, -planeN);
    float t = -(d + RayPos.z * planeN.z + RayPos.y * planeN.y + RayPos.x * planeN.x) / (RayDir.z * planeN.z + RayDir.y * planeN.y + RayDir.x * planeN.x);
    return RayPos + t * RayDir;
}
float RayBoxIntersect(float3 RayPos, float3 RayDir, float3* Normal)
{
    float3 hBoxSize = float3(0.5f, 0.5f, 0.5f);

    float XpDistance = (hBoxSize.x - RayPos.x) / RayDir.x;
    float XnDistance = (-hBoxSize.x - RayPos.x) / RayDir.x;
    float YpDistance = (hBoxSize.y - RayPos.y) / RayDir.y;
    float YnDistance = (-hBoxSize.y - RayPos.y) / RayDir.y;
    float ZpDistance = (hBoxSize.z - RayPos.z) / RayDir.z;
    float ZnDistance = (-hBoxSize.z - RayPos.z) / RayDir.z;
    float maxx = XnDistance > XpDistance ? XnDistance : XpDistance;
    float minx = XnDistance < XpDistance ? XnDistance : XpDistance;
    float maxy = YnDistance > YpDistance ? YnDistance : YpDistance;
    float miny = YnDistance < YpDistance ? YnDistance : YpDistance;
    float maxz = ZnDistance > ZpDistance ? ZnDistance : ZpDistance;
    float minz = ZnDistance < ZpDistance ? ZnDistance : ZpDistance;

    if (minx > maxy || miny > maxx || minz > maxz)
    {
        return -1;
    }

    float ClosestDistance = max(minx, miny, minz);
    float FurthestDistance = min(maxx, maxy, maxz);

    if (ClosestDistance > FurthestDistance)
    {
        return -1;
    }

    *Normal = {};
    if (ClosestDistance == XpDistance)
        (*Normal).x = 1;
    else if (ClosestDistance == XnDistance)
        (*Normal).x = -1;
    else if (ClosestDistance == YpDistance)
        (*Normal).y = 1;
    else if (ClosestDistance == YnDistance)
        (*Normal).y = -1;
    else if (ClosestDistance == ZpDistance)
        (*Normal).z = 1;
    else if (ClosestDistance == ZnDistance)
        (*Normal).z = -1;

    return ClosestDistance;
}

float3 RayBoxUV(float3 HitPoint, float3 Normal)
{
    float3 p2 = HitPoint + 0.5f;
    float u = 0;
    float v = 0;
    if (abs(Normal.x) > 0)
    {
        u = p2.y;
        v = 1 - p2.z;
    }
    else if (abs(Normal.y) > 0)
    {
        u = p2.x;
        v = 1 - p2.z;
    }
    else if (abs(Normal.z) > 0)
    {
        u = p2.x;
        v = p2.y;
    }
    return float3(u, v, 0);
}

float3 RayBoxUVScaled(float3 HitPoint, float3 Normal, float3 Scale)
{
    float3 p2 = HitPoint + 0.5f;
    float u = 0;
    float v = 0;
    if (abs(Normal.x) > 0)
    {
        u = p2.y;
        v = 1 - p2.z;
    }
    else if (abs(Normal.y) > 0)
    {
        u = frac(p2.x * Scale.x);
        v = 1 - p2.z;
    }
    else if (abs(Normal.z) > 0)
    {
        u = frac(p2.x * Scale.x);
        v = p2.y;
    }
    return float3(u, v, 0);
}

float RaySphereIntersect(float3 RayPos, float3 RayDir, float r)
{
    if (dot(RayPos, RayPos) < (r * r))
        return -1;

    float3 q = cross(RayDir, RayPos);
    if (dot(q, q) > (r * r))
        return -1;
    float dist = dot(-RayPos, RayDir) - sqrtFast((r * r) - dot(q, q));

    return dist;
}
// lol
float shitty_atan2(float MouseX, float MouseY)
{
    float len = sqrt(MouseX * MouseX + MouseY * MouseY);

    float value = 0;
    if (MouseX > 0 && MouseY > 0) // top right
    {
        value = abs(MouseY / len);
    }
    else if (MouseX < 0 && MouseY > 0) // bottom right
    {
        value = abs(MouseX / len) + 1;
    }
    else if (MouseX < 0 && MouseY < 0) // bottom left
    {
        value = abs(MouseY / len) + 2;
    }
    else if (MouseX > 0 && MouseY < 0) // top left
    {
        value = abs(MouseX / len) + 3;
    }
    return -value * 0.25f;
}

float3 VectorToPolar(float3 Normal)
{
    float u = atan2(Normal.x, Normal.y) / (3.141532f * 2.0f);
    float3 p =  normalize(float3(Normal.x, Normal.y, 0));
    float a = dot(Normal, p);
    float v = acos(a);
    if (Normal.z > 0) v *= -1;
    v /= (3.14152128*0.5);
    v = v * 0.5 + 0.5;

    return float3(frac(u), frac(v), 0);
}

// X and Y are UVs, Z is face index
float3 RayCubemap(float3 v)
{
#if 1
    float3 vAbs = abs(v);
    float3 habs = 0.5f / vAbs;
    if (vAbs.z >= vAbs.x && vAbs.z >= vAbs.y)
    {
        return float3(
            (v.z < 0.0f ? -v.x : v.x) * (0.5f / vAbs.z) + 0.5f,
            (-v.y) * (0.5f / vAbs.z) + 0.5f,
            v.z < 0.0f ? 5.0f : 4.0f);
    }
    else if (vAbs.y >= vAbs.x)
    {
        return float3(
            (v.y < 0.0f ? v.x : -v.x) * (0.5f / vAbs.y) + 0.5f,
            (-v.z) * (0.5f / vAbs.y) + 0.5f,
            v.y < 0.0f ? 3.0f : 2.0f);
    }
    else
    {
        return float3(
            (v.x < 0.0f ? -v.y : v.y) * (0.5f / vAbs.x) + 0.5f,
            (-v.z) * (0.5f / vAbs.x) + 0.5f,
            v.x < 0.0f ? 1.0f : 0.0f);
    }
#else
    float3 vAbs = abs(v);
    float3 habs = 0.5 / vAbs;
    if (vAbs.z >= vAbs.x && vAbs.z >= vAbs.y)
    {
        return float3(
            (v.z < 0.0 ? -v.x : v.x) * (0.5 / vAbs.z) + 0.5,
            (-v.y) * (0.5 / vAbs.z) + 0.5,
            v.z < 0.0 ? 5.0 : 4.0);
    }
    else if (vAbs.y >= vAbs.x)
    {
        return float3(
            (v.y < 0.0 ? v.x : -v.x) * (0.5 / vAbs.y) + 0.5,
            (-v.z) * (0.5 / vAbs.y) + 0.5,
            v.y < 0.0 ? 3.0 : 2.0);
    }
    else
    {
        return float3(
            (v.x < 0.0 ? -v.y : v.y) * (0.5 / vAbs.x) + 0.5,
            (-v.z) * (0.5 / vAbs.x) + 0.5,
            v.x < 0.0 ? 1.0 : 0.0);
    }
#endif
}

float3 PolarUVs(float3 Normal)
{
    float u = shitty_atan2(Normal.x, Normal.y);
    float v = -(Normal.z) * 0.5 + 0.5;
    return float3(v, u, 0);
}

float3 RaySphereUV(float3 Normal)
{
    return RayCubemap(Normal);
}


#define TEMPLATE_SAY(name) void name(game_memory* Memory)
#define TEMPLATE_SAY_CALL(name) name(Memory)

TEMPLATE_SAY(Dog)
{
    Memory->PlatformPrint("Bark!");
}
TEMPLATE_SAY(Cat)
{
    Memory->PlatformPrint("Meow!");
}
TEMPLATE_SAY(Human)
{
    Memory->PlatformPrint("Fuck!");
}
enum AnimalType
{
    AnimalType_Dog,
    AnimalType_Cat,
    AnimalType_Human
};

void Main(game_memory* Memory, AnimalType animal)
{
    switch (animal)
    {
    case AnimalType_Dog:
        TEMPLATE_SAY_CALL(Dog);
        break;
    case AnimalType_Cat:
        TEMPLATE_SAY_CALL(Cat);
        break;
    case AnimalType_Human:
        TEMPLATE_SAY_CALL(Human);
        break;
    default:
        break;
    }
}


#define SURFACE_SHADER(name) float3 name(game_offscreen_buffer* VideoBuffer, Image* texture1, Image* texture2, float3 Color, float3 LocalPos, float depth, float3 normal, float3 normalLocal, float3 viewDirection, EntityType type, float3 scale)
#define SURFACE_SHADER_CALL(name) name(VideoBuffer, texture1, texture2, float3(1, 1, 1), LocalPos, depth, Normal, NormalLocal, ViewDirection, Sphere ? EntityType_Sphere : EntityType_Box, scale)

SURFACE_SHADER(SurfaceShaderSimple)
{
    return float3(0.5f, 0.5f, 0.5f);
    return frac(LocalPos * 10.0f); 
}

SURFACE_SHADER(SurfaceShaderTexture)
{
    float3 UV = float3(0, 0, 0);
    if (type == EntityType_Sphere)
    {
        UV = RaySphereUV(normalLocal);
    }
    else
    {
        UV = RayBoxUV(LocalPos, normalLocal);
    }
    uint32 TextureSample = ImageSample(texture1, UV.x, UV.y);

    //uint32 TextureSample = ImageSample(texture1, frac(LocalPos.x), frac(LocalPos.y));

    return GetBufferColorReverse(VideoBuffer, TextureSample);
}
SURFACE_SHADER(SurfaceShaderSphere)
{
    float3 UV = PolarUVs(normalLocal);
    uint32 TextureSample = ImageSample(texture1, frac(UV.x), frac(UV.y * 3));

    return GetBufferColorReverse(VideoBuffer, TextureSample);
}


SURFACE_SHADER(SurfaceShaderLine)
{
    float3 UV = frac(LocalPos);// RayBoxUVScaled(LocalPos, normalLocal, scale);
    uint32 TextureSample = ImageSample(texture1, UV.x, UV.y);
    
    if(normalLocal.z > 0.75)
        TextureSample = ImageSample(texture2, UV.x, UV.y);

    return GetBufferColorReverse(VideoBuffer, TextureSample);
}

enum ShaderType
{
    ShaderType_Simple,
    ShaderType_Texture,
    ShaderType_Sphere,
    ShaderType_Line,
    ShaderType_Corner,
    ShaderType_Block,
    ShaderType_Additive,
    ShaderType_Null
};
char ShaderNames[8][100] = {
    "Shader: Simple",
    "Shader: Texture",
    "Shader: Sphere",
    "Shader: Line",
    "Shader: Corner",
    "Shader: Block",
    "Shader: Additive",
    "Shader: Null",
};

//uint32 Shader(EngineState* engineState, game_offscreen_buffer* VideoBuffer, float3 LocalPos, float depth, float3 Normal, float3 NormalLocal, float3 ViewDirection, bool Sphere, Image* texture1, Image* texture2, int ShaderIndex, float3 scale, bool selected, bool hovered)
//{
//
//}

float RayCircleIntersect(game_offscreen_buffer* VideoBuffer, float2 p, float2 d, float r)
{
    float q = dot(p, float2(-d.y, d.x));
    float o = -sqrt(r * r - q * q);

    float x = -d.y * q + d.x * o;
    float y = d.x * q + d.y * o;

    DrawLine(VideoBuffer, p, float2(0, 0));
    DrawLine(VideoBuffer, d * 100, float2(0, 0));
    DrawCircle(VideoBuffer, float2(x, y), 3);
    DrawCircle(VideoBuffer, float2(0, 0), r);

    return 0;
}

float RayBoxIntersect2D(game_offscreen_buffer* VideoBuffer, float rx, float ry, float dx, float dy, float x, float y, float width, float height)
{
    float hwidth = width / 2;
    float hheight = height / 2;
    float px = rx - x;
    float py = ry - y;
    float LeftDistance = (-hwidth - px) / dx;
    float BottomDistance = (-hheight - py) / dy;
    float RightDistance = (hwidth - px) / dx;
    float TopDistance = (hheight - py) / dy;

    float maxx = LeftDistance > RightDistance ? LeftDistance : RightDistance;
    float minx = LeftDistance < RightDistance ? LeftDistance : RightDistance;
    float maxy = TopDistance > BottomDistance ? TopDistance : BottomDistance;
    float miny = TopDistance < BottomDistance ? TopDistance : BottomDistance;
    float ClosestDistance = max(minx, miny);
    float FurthestDistance = min(maxx, maxy);

    if (minx > maxy || miny > maxx)
    {
        return -1;
    }
    return ClosestDistance;
}

void DrawSkybox(EngineState* engineState, game_offscreen_buffer* VideoBuffer, transform Camera, int Region = -1, int Regions = 9)
{
    ProfilerBeingSample(&engineState->profilingData);
    if (!Valid(engineState->SkyFaces[0]))
        return; // skybox not loaded.

    int sizeX = VideoBuffer->Width;
    int sizeY = VideoBuffer->Height;
    float AspectRatio = 0.5625f;
    float AspectRatio2 = 1.777777777f;

    float3 Forward = Camera.forward;
    float3 Right = Camera.right;
    float3 Up = Camera.up * AspectRatio;

    float3 TopLeft = ((Forward + -Right) + Up);
    float3 TopRight = ((Forward + Right) + Up);
    float3 BottomLeft = ((Forward + -Right) + -Up);
    float3 BottomRight = ((Forward + Right) + -Up);
    float3 RightOffset = (TopRight - TopLeft);
    float3 DownOffset = (BottomLeft - TopLeft);
    float3 RightStep = RightOffset / (VideoBuffer->Width);
    float3 DownStep = DownOffset / (VideoBuffer->Height);

    int MinX = 0;
    int MinY = 0;
    int MaxX = VideoBuffer->Width;
    int MaxY = VideoBuffer->Height;
    float RegionWidth = VideoBuffer->Width;

    int2 ChangedSize = GetThreadRegion(engineState, Regions, Region, VideoBuffer->Height, MinY, MaxY);
    MinY = ChangedSize.x;
    MaxY = ChangedSize.y;
    //float3 Dir = TopLeft;// +(RightStep * MinX) + (DownStep * MinY);
    float3 Dir = TopLeft + (RightStep * MinX) + (DownStep * MinY);

#define SkyboxMode 1

#if SkyboxMode == 0 // offsets the vector with every step, faster but gets inaccurate over time

    for (int yi = 0; yi < VideoBuffer->Height; yi++)
    {
        for (int xi = 0; xi < VideoBuffer->Width; xi++)
        {
            uint32* Pixel = &((uint32*)VideoBuffer->Memory)[(((yi)*VideoBuffer->Pitch) / 4 + (xi))];
            Dir += RightStep;
            float3 WorldDir = (Dir); // skip normalizing here, looks the same?

            float3 ReflectionUV = RayCubemap(WorldDir);

            Image* image = engineState->SkyFaces[(int)(ReflectionUV.z)];
            uint32 PixelX = ReflectionUV.x * image->width;
            uint32 PixelY = ReflectionUV.y * image->height;
            uint32 ReflectionSample = image->ImageData[(PixelY)*image->width + (PixelX)];

            *Pixel = ReflectionSample;
            ((uint8*)Pixel)[3] = 255;
        }
        Dir -= RightOffset;
        Dir += DownStep;
    }
#elif SkyboxMode == 1

    float3 LastReflectionUV = {};
    float3 CurrentReflectionUV = {};
    float SizeA = 32;
    float SizeB = 8;
    uint8 xStepLoop = 0;
    float xvalue = 0;
    float yvalue = 0;
    uint8 yStepLoop = 0;
    for (int yi = MinY; yi < MaxY; yi++)
    {
        for (int xi = MinX; xi < MaxX; xi++)
        {
            uint32* Pixel = &((uint32*)VideoBuffer->Memory)[yi * VideoBuffer->Width + xi];

            Dir += RightStep;

            if (xStepLoop == 0)
            {
                LastReflectionUV = RayCubemap(Dir);
                CurrentReflectionUV = RayCubemap(Dir + RightStep * SizeA);
                xvalue = 0;
            }
            float3 ReflectionUV = lerp(LastReflectionUV, CurrentReflectionUV, xvalue);
            
            if(LastReflectionUV.z != CurrentReflectionUV.z)
                ReflectionUV = RayCubemap(Dir);
            else
                ReflectionUV.z = LastReflectionUV.z;

            Image* image = engineState->SkyFaces[(int)(ReflectionUV.z)];
            uint32 PixelX = ReflectionUV.x * image->width;
            uint32 PixelY = ReflectionUV.y * image->height;
            uint32 ReflectionSample = image->ImageData[(PixelY) * image->width + (PixelX)];
            *Pixel = ReflectionSample;
            ((uint8*)Pixel)[3] = 255;

            xStepLoop += SizeB;
            xvalue += 1.0f / SizeA;
        }

        if (yStepLoop == 0)
        {
            yvalue = 0;
        }
        yvalue += 1.0f / SizeA;
        
        yStepLoop += SizeB;
        xStepLoop = 0;

        // set dir here?
        float yy = (float)yi / (float)VideoBuffer->Height;
        Dir = lerp(TopLeft, BottomLeft, yy);

        //Dir -= RightStep * RegionWidth;
        //Dir += DownStep;
    }
#elif SkyboxMode == 2 // slow but accurate version, samples the background and lerps for every pixel

    for (int yi = MinY; yi < MaxY; yi++)
    {
        for (int xi = MinX; xi < MaxX; xi++)
        {
            uint32* Pixel = &((uint32*)VideoBuffer->Memory)[yi * VideoBuffer->Width + xi];

            float x = (float)xi / (float)VideoBuffer->Width;
            float y = (float)yi / (float)VideoBuffer->Height;
            float3 dir1 = lerp(TopLeft, TopRight, x);
            float3 dir2 = lerp(BottomLeft, BottomRight, x);
            float3 Dirr = lerp(dir1, dir2, y);

            Dir += RightStep;

            float3 ReflectionUV = RayCubemap(Dirr);
            Image* image = engineState->SkyFaces[(int)(ReflectionUV.z)];
            uint32 PixelX = ReflectionUV.x * image->width;
            uint32 PixelY = ReflectionUV.y * image->height;
            uint32 ReflectionSample = image->ImageData[(PixelY)*image->width + (PixelX)];
            *Pixel = ReflectionSample;
            ((uint8*)Pixel)[3] = 255;
        }
        Dir -= RightStep * RegionWidth;
        Dir += DownStep;
    }
#endif

    ProfilerEndSample(&engineState->profilingData, "Sky");
}

float3 ToScreenPos(game_offscreen_buffer* VideoBuffer, float3 position, float3 CameraPos, float3 Forward, float3 Right, float3 Up)
{
    int sizeX = VideoBuffer->Width;
    int sizeY = VideoBuffer->Height;
    float3 LocalPos2 = float3(
        dot(CameraPos - position, Right),
        dot(CameraPos - position, Forward),
        dot(CameraPos - position, Up));
    float depth = -LocalPos2.y;
    int X = -(LocalPos2.x / depth) * sizeX * 0.5 + sizeX * 0.5f;
    int Y = (LocalPos2.z / depth) * sizeY * 1.58f + sizeY * 0.5f;
    return float3(X, Y, depth);
}

float3 ToScreenPos(game_offscreen_buffer* VideoBuffer, float3 position, transform Camera)
{
    int sizeX = VideoBuffer->Width;
    int sizeY = VideoBuffer->Height;
    float3 LocalPos2 = float3(
        dot(Camera.position - position, Camera.right),
        dot(Camera.position - position, Camera.forward),
        dot(Camera.position - position, Camera.up));
    float depth = -LocalPos2.y;
    int X = -(LocalPos2.x / depth) * sizeX * 0.5 + sizeX * 0.5f;
    int Y = (LocalPos2.z / depth) * sizeY * 1.58f + sizeY * 0.5f;
    return float3(X, Y, depth);
}

uint8 DistanceToDepthBufferValue(float dist)
{
    float FarZ = 32.0f;
    float distance01 = (dist / FarZ);
    if (distance01 > 1)
        return 255;
    distance01 = clamp01(distance01);
    distance01 = 1 - distance01;
    distance01 = distance01 * distance01;
    //distance01 = distance01 * distance01 * distance01 * distance01;
    distance01 = 1 - distance01;
    return (uint8)(distance01 * 255);
}

uint16 DistanceToDepthBufferValue2(float dist)
{
    float FarZ = 32.0f;
    //float distance01 = (dist / FarZ);
    //if (distance01 > 1)
    //    return 255;
    //distance01 = clamp01(distance01);
    //distance01 = 1 - distance01;
    //distance01 = distance01 * distance01;
    ////distance01 = distance01 * distance01 * distance01 * distance01;
    //distance01 = 1 - distance01;
    return (uint16)(clamp01(dist / FarZ)*(65536));
}

float DistanceToLine(float2 p, float2 a, float2 b)
{
    float2 pa = p - a, ba = b - a;
    float h = clamp01(dot(pa, ba) / dot(ba, ba));
    return length(pa - ba * h);
}
float DrawLine3D(game_offscreen_buffer* VideoBuffer, transform Camera, float3 Start, float3 End, float3 Color, bool zTest = true, float2 TestPoint = {})
{
    float AspectRatio = 0.5625f;
    float3 Up = Camera.up * AspectRatio;
    transform CameraTransform = Camera;
    CameraTransform.up = Up;

    float3 Start2D = ToScreenPos(VideoBuffer, Start, CameraTransform);
    float3 End2D = ToScreenPos(VideoBuffer, End, CameraTransform);
    // behind the camera

    if( Start2D.x == End2D.x && 
        Start2D.y == End2D.y && 
        Start2D.z == End2D.z)
        return 99999999;

    // outside edges
    if ((Start2D.z < 0 || End2D.z < 0) || 
        (Start2D.x < 0 || Start2D.y < 0 || Start2D.y > VideoBuffer->Height || Start2D.x > VideoBuffer->Width) &&
        (End2D.y > VideoBuffer->Height || End2D.x < 0 || End2D.y < 0 || End2D.x > VideoBuffer->Width))
        return 99999999;

    float dist = distance(Start2D, End2D);
    // doing this was a really, *really* bad meme. made it lock up when running with OFast. do nooooot
    //for (float i = 0; i < 1.0f; i += (1.0f / dist))
    //{
    for (float ii = 0; ii < dist; ii++)
    {
        float i = (float)ii / dist;

        float3 l = lerp(Start2D, End2D, i);
        uint8 NewDepth = DistanceToDepthBufferValue(l.z);
        uint32* Pixel = GetPixel(VideoBuffer, l.x, l.y);
        uint8 OldDepth = ((uint8*)Pixel)[3];
        if (NewDepth > OldDepth && zTest)
            continue;
        *GetPixel(VideoBuffer, l.x, l.y) = MakeBufferColor(VideoBuffer, Color);
        ((uint8*)Pixel)[3] = NewDepth;
    }

    return DistanceToLine(TestPoint, float2(Start2D.x, Start2D.y), float2(End2D.x, End2D.y));
}
float DrawLine3D(game_offscreen_buffer* VideoBuffer, transform Camera, float3 Start, float3 End, bool zTest = true)
{
    return DrawLine3D(VideoBuffer, Camera, Start, End, float3(1, 1, 1), zTest);
}
void DrawArrow3D(game_offscreen_buffer* VideoBuffer, transform Camera, float3 Start, float3 End, float3 Color, bool zTest = true)
{
    DrawLine3D(VideoBuffer, Camera, Start, End, Color, zTest);

    float3 delta = normalize(End - Start);
    float3 deltaNormalized = normalize(delta);
    float3 right = float3(1, 0, 0);
    float3 forward = float3(0, 1, 0);
    if (!(deltaNormalized.x == 0 && deltaNormalized.y == 0 && deltaNormalized.z == 1)) // Exactly upwards
    {
        right = normalize(cross(deltaNormalized, float3(0, 0, 1)));
        forward = normalize(cross(right, deltaNormalized));
        right = normalize(cross(forward, deltaNormalized));
    }

    float PointerSize = 0.05f;
    float PointerSize2 = 0.1f;

    DrawLine3D(VideoBuffer, Camera, delta * (1-PointerSize2) + Start + PointerSize * right, End, Color, zTest);
    DrawLine3D(VideoBuffer, Camera, delta * (1-PointerSize2) + Start + PointerSize * forward, End, Color, zTest);
    DrawLine3D(VideoBuffer, Camera, delta * (1-PointerSize2) + Start - PointerSize * right, End, Color, zTest);
    DrawLine3D(VideoBuffer, Camera, delta * (1-PointerSize2) + Start - PointerSize * forward , End, Color, zTest);
}

void DrawPoint3D(game_offscreen_buffer* VideoBuffer, transform Camera, float3 Pos, float size, float3 Color, bool zTest = true)
{
    DrawLine3D(VideoBuffer, Camera, Pos + float3(size, 0, 0), Pos + float3(-size, 0, 0), zTest);
    DrawLine3D(VideoBuffer, Camera, Pos + float3(0, size, 0), Pos + float3(0, -size, 0), zTest);
    DrawLine3D(VideoBuffer, Camera, Pos + float3(0, 0, size), Pos + float3(0, 0, -size), zTest);
}


float DrawCircle3D(game_offscreen_buffer* VideoBuffer, transform Camera, float3 Center, float3 Angle, float Radius, float3 Color, bool zTest = true, float2 TestPoint = {})
{
    float sweep = 1.0f;
    float3 Up = Angle;
    float3 Right = normalize(cross(Angle, float3(0,0.00001, 1.03)));
    float3 Forward = normalize(cross(Right, Up));
    Right = normalize(cross(Forward, Up));
    float TestPointDistance = 99999999;

    float steps = 100;
    float stepsize = (1.0f / steps) * (3.14152128f * 2.0f);
    //float3 lastP = float3(sin(-stepsize) * Radius, cos(-stepsize) * Radius, 0);
    float3 lastP = Right * sin(-stepsize) * Radius + Forward * cos(-stepsize) * Radius;
    for (float i = 0; i < (3.14152128f * 2.0f) * sweep; i += stepsize) // eh
    {
        float3 p = Right * sin(i) * Radius + Forward * cos(i) * Radius;
        float NewDistance = 9999999;
        if(distance(p + Center, Camera.position) < distance(Center, Camera.position) + 0.25)
            NewDistance = DrawLine3D(VideoBuffer, Camera, p + Center, lastP + Center, Color, zTest, TestPoint);

        if (NewDistance < TestPointDistance)
            TestPointDistance = NewDistance;
        lastP = p;
    }
    return TestPointDistance;
}

void DrawBox3D(game_offscreen_buffer* VideoBuffer, transform Camera, transform Box, float3 Color, bool zTest = true)
{
    float3 BoxPos = Box.position;
    float3 BoxForward = Box.forward;
    float3 BoxUp = Box.up;
    float3 BoxRight = Box.right;
    float3 BoxScale = Box.scale;

    float3 BoxScreenPos1 = (BoxPos + ((BoxRight * BoxScale.x) + (-BoxForward * BoxScale.y) + (-BoxUp * BoxScale.z)) * 0.5);
    float3 BoxScreenPos2 = (BoxPos + ((-BoxRight * BoxScale.x) + (-BoxForward * BoxScale.y) + (-BoxUp * BoxScale.z)) * 0.5);
    float3 BoxScreenPos3 = (BoxPos + ((-BoxRight * BoxScale.x) + (BoxForward * BoxScale.y) + (-BoxUp * BoxScale.z)) * 0.5);
    float3 BoxScreenPos4 = (BoxPos + ((BoxRight * BoxScale.x) + (BoxForward * BoxScale.y) + (-BoxUp * BoxScale.z)) * 0.5);

    float3 BoxScreenPos5 = (BoxPos + ((BoxRight * BoxScale.x) + (-BoxForward * BoxScale.y) + (BoxUp * BoxScale.z)) * 0.5);
    float3 BoxScreenPos6 = (BoxPos + ((-BoxRight * BoxScale.x) + (-BoxForward * BoxScale.y) + (BoxUp * BoxScale.z)) * 0.5);
    float3 BoxScreenPos7 = (BoxPos + ((-BoxRight * BoxScale.x) + (BoxForward * BoxScale.y) + (BoxUp * BoxScale.z)) * 0.5);
    float3 BoxScreenPos8 = (BoxPos + ((BoxRight * BoxScale.x) + (BoxForward * BoxScale.y) + (BoxUp * BoxScale.z)) * 0.5);

    DrawLine3D(VideoBuffer, Camera, BoxScreenPos1, BoxScreenPos2,Color,zTest);
    DrawLine3D(VideoBuffer, Camera, BoxScreenPos2, BoxScreenPos3,Color,zTest);
    DrawLine3D(VideoBuffer, Camera, BoxScreenPos3, BoxScreenPos4,Color,zTest);
    DrawLine3D(VideoBuffer, Camera, BoxScreenPos4, BoxScreenPos1,Color,zTest);
    DrawLine3D(VideoBuffer, Camera, BoxScreenPos5, BoxScreenPos6,Color,zTest);
    DrawLine3D(VideoBuffer, Camera, BoxScreenPos6, BoxScreenPos7,Color,zTest);
    DrawLine3D(VideoBuffer, Camera, BoxScreenPos7, BoxScreenPos8,Color,zTest);
    DrawLine3D(VideoBuffer, Camera, BoxScreenPos8, BoxScreenPos5,Color,zTest);
    DrawLine3D(VideoBuffer, Camera, BoxScreenPos1, BoxScreenPos5,Color,zTest);
    DrawLine3D(VideoBuffer, Camera, BoxScreenPos2, BoxScreenPos6,Color,zTest);
    DrawLine3D(VideoBuffer, Camera, BoxScreenPos3, BoxScreenPos7,Color,zTest);
    DrawLine3D(VideoBuffer, Camera, BoxScreenPos4, BoxScreenPos8,Color,zTest);
}

void DrawSphere3D(game_offscreen_buffer* VideoBuffer, transform Camera, transform Box, bool zTest = true)
{
    float3 BoxPos = Box.position;
    float3 BoxForward = Box.forward;
    float3 BoxUp = Box.up;
    float3 BoxRight = Box.right;
    float3 BoxScale = Box.scale;

    DrawCircle3D(VideoBuffer, Camera, BoxPos, BoxForward, max(BoxScale), float3(1,1,1), zTest);
    DrawCircle3D(VideoBuffer, Camera, BoxPos, BoxUp, max(BoxScale), float3(1, 1, 1), zTest);
    DrawCircle3D(VideoBuffer, Camera, BoxPos, BoxRight, max(BoxScale), float3(1, 1, 1), zTest);
}

float3 ScreenPointToRay(game_offscreen_buffer* VideoBuffer, transform Camera, float2 MousePos)
{
    float AspectRatio = 0.5625f;
    float3 Forward = Camera.forward;// CameraForward;
    float3 Right = Camera.right;//cross(CameraForward, CameraUp);
    float3 Up = Camera.up * AspectRatio;

    float3 TopLeft = ((Forward + -Right) + Up);
    float3 TopRight = ((Forward + Right) + Up);
    float3 BottomLeft = ((Forward + -Right) + -Up);
    float3 BottomRight = ((Forward + Right) + -Up);

    MousePos /= float2(VideoBuffer->Width, VideoBuffer->Height);

    return normalize(lerp(lerp(TopLeft, TopRight, MousePos.x), lerp(BottomLeft, BottomRight, MousePos.x), MousePos.y));
}

float DragBlend(float x, float d)
{
    return (1.0f - (1.0f / ((x * d) + 1.0f)));
}

float3 analyticConstantVelocity(float3 a, float x, float d)
{
    d = max(d, 0.00001f);
    return x * x * a - x * x * a * DragBlend(x, d);
}
float3 analyticConstantVelocity(float3 a, float x)
{
    return a * x * x;
}
float3 analyticInstantVelocity(float3 a, float x, float d)
{
    d = max(d, 0.00001f);
    return (a / d) * DragBlend(x, d);
}
float3 analyticInstantVelocity(float3 a, float x)
{
    return a * x;
}

void DrawEffect(EngineState* engineState, game_offscreen_buffer* VideoBuffer, transform Camera, RenderCommand* command, int Region = -1, int Regions = 9, int DrawIndex = 0)
{
    float AspectRatio = 0.5625f;
    float3 Up = Camera.up * AspectRatio;
    transform CameraTransform = Camera;
    CameraTransform.up = Up;

    //float random = hash(DrawIndex);
    EffectData* effect = &command->effect;

    //float TestLoopTime = 4.0f;
    float Time = engineState->GameTime - effect->startTime;
    
    // Wrap time such that if it goes above 60, it resets down to 1
    Time = min(min(Time, 2.0f) + mod(Time, 60.0f), Time);

    float SpawnDelay = 1.0f / effect->spawnRate;

    float3 Gravity = float3(0, 0, effect->gravity);
    
    float lifetime = 2;
    int MaxParticles = 2000;
    if (effect->burst)
        MaxParticles = effect->spawnRate;

    for (int i = 0; i < MaxParticles; i++)
    {
        int ri = 0;
        float random1 = rand(float2(i, ri++));
        float3 random2 = float3(rand(float2(i, ri++)), rand(float2(i, ri++)), rand(float2(i, ri++)));
        float3 random3 = float3(rand(float2(i, ri++)), rand(float2(i, ri++)), rand(float2(i, ri++)));

        float Lifetime = lerp(effect->minLifetime, effect->maxLifetime, random1);
        float3 StartVelocity = lerp(effect->minSpeed, effect->maxSpeed, random2) * 10;
        float3 StartPos = effect->spawnRadius * (random3 - 0.5);

        float SpawnTime = (SpawnDelay * i); // Make this wrap on some value to make this particle think it's some other particle
        if (effect->burst)
            SpawnTime = 0;

        float t = Time - SpawnTime;
        if (t < 0) // not spawned yet
            continue;

        if(t > Lifetime)
            continue;

        float tNormalized = t / Lifetime;

        float drag = 1.0f;

        float3 position = command->t.position;
        position += StartPos + analyticInstantVelocity(StartVelocity, t, effect->drag) + analyticConstantVelocity(Gravity, t, effect->drag);
        
        float3 CenterPoint = ToScreenPos(VideoBuffer, position, CameraTransform);
        if (CenterPoint.z < 0)
            continue;


        uint8 particleDepth = DistanceToDepthBufferValue(CenterPoint.z);

        float MinSize = min(VideoBuffer->Width, VideoBuffer->Height);
        float ScreenSize = MinSize / 500.0f;
        float size = ((100 * lerp(effect->startSize, effect->endSize, tNormalized)) / CenterPoint.z) * ScreenSize;
        float2 Pos = float2(CenterPoint.x, CenterPoint.y) - float2(size, size) * 0.5;
        float2 Size = float2(size, size);

        DrawImageScaled(engineState, VideoBuffer, command->texture1, Pos, Size, command->color, command->ShaderIndex, Region, Regions, particleDepth);
    }
}

void DrawObjectCheap(EngineState* engineState, game_offscreen_buffer* VideoBuffer, transform Camera, transform Box, bool Sphere = false)
{
    float AspectRatio = 0.5625f;
    float3 Up = Camera.up * AspectRatio;
    transform CameraTransform = Camera;
    CameraTransform.up = Up;

    float3 BoxPos = Box.position;
    float3 BoxForward = Box.forward;
    float3 BoxUp = Box.up;
    float3 BoxRight = Box.right;
    float3 BoxScale = Box.scale;

    float3 BoxScreenPos1 = (BoxPos + ((BoxRight * BoxScale.x) + (-BoxForward * BoxScale.y) + (-BoxUp * BoxScale.z)) * 0.5);
    float3 BoxScreenPos2 = (BoxPos + ((-BoxRight * BoxScale.x) + (-BoxForward * BoxScale.y) + (-BoxUp * BoxScale.z)) * 0.5);
    float3 BoxScreenPos3 = (BoxPos + ((-BoxRight * BoxScale.x) + (BoxForward * BoxScale.y) + (-BoxUp * BoxScale.z)) * 0.5);
    float3 BoxScreenPos4 = (BoxPos + ((BoxRight * BoxScale.x) + (BoxForward * BoxScale.y) + (-BoxUp * BoxScale.z)) * 0.5);

    float3 BoxScreenPos5 = (BoxPos + ((BoxRight * BoxScale.x) + (-BoxForward * BoxScale.y) + (BoxUp * BoxScale.z)) * 0.5);
    float3 BoxScreenPos6 = (BoxPos + ((-BoxRight * BoxScale.x) + (-BoxForward * BoxScale.y) + (BoxUp * BoxScale.z)) * 0.5);
    float3 BoxScreenPos7 = (BoxPos + ((-BoxRight * BoxScale.x) + (BoxForward * BoxScale.y) + (BoxUp * BoxScale.z)) * 0.5);
    float3 BoxScreenPos8 = (BoxPos + ((BoxRight * BoxScale.x) + (BoxForward * BoxScale.y) + (BoxUp * BoxScale.z)) * 0.5);

    if (Sphere)
    {
        DrawCircle3D(VideoBuffer, Camera, BoxPos, BoxForward, max(BoxScale), float3(1,1,1));
        DrawCircle3D(VideoBuffer, Camera, BoxPos, BoxUp, max(BoxScale), float3(1, 1, 1));
        DrawCircle3D(VideoBuffer, Camera, BoxPos, BoxRight, max(BoxScale), float3(1, 1, 1));

        //DrawCircle3D(VideoBuffer, Camera, BoxPos, normalize(BoxForward + BoxUp), max(BoxScale));
        //DrawCircle3D(VideoBuffer, Camera, BoxPos, normalize(BoxUp + BoxRight), max(BoxScale));
        //DrawCircle3D(VideoBuffer, Camera, BoxPos, normalize(BoxRight + BoxForward), max(BoxScale));
        //DrawCircle3D(VideoBuffer, Camera, BoxPos, normalize(BoxForward - BoxUp), max(BoxScale));
        //DrawCircle3D(VideoBuffer, Camera, BoxPos, normalize(BoxUp - BoxRight), max(BoxScale));
        //DrawCircle3D(VideoBuffer, Camera, BoxPos, normalize(BoxRight - BoxForward), max(BoxScale));
    }
    else
    {
        DrawLine3D(VideoBuffer, Camera, BoxScreenPos1, BoxScreenPos2);
        DrawLine3D(VideoBuffer, Camera, BoxScreenPos2, BoxScreenPos3);
        DrawLine3D(VideoBuffer, Camera, BoxScreenPos3, BoxScreenPos4);
        DrawLine3D(VideoBuffer, Camera, BoxScreenPos4, BoxScreenPos1);
        DrawLine3D(VideoBuffer, Camera, BoxScreenPos5, BoxScreenPos6);
        DrawLine3D(VideoBuffer, Camera, BoxScreenPos6, BoxScreenPos7);
        DrawLine3D(VideoBuffer, Camera, BoxScreenPos7, BoxScreenPos8);
        DrawLine3D(VideoBuffer, Camera, BoxScreenPos8, BoxScreenPos5);
        DrawLine3D(VideoBuffer, Camera, BoxScreenPos1, BoxScreenPos5);
        DrawLine3D(VideoBuffer, Camera, BoxScreenPos2, BoxScreenPos6);
        DrawLine3D(VideoBuffer, Camera, BoxScreenPos3, BoxScreenPos7);
        DrawLine3D(VideoBuffer, Camera, BoxScreenPos4, BoxScreenPos8);
    }
}

inline float3 RasterizeTriangle(float2 pos, float2 a, float2 b, float2 c)
{
    float2 uflippedvector = float2((a - c).y, -(a - c).x);
    float uo = dot(uflippedvector, (b - a));
    float u = dot(pos - a, uflippedvector) / uo;
    float2 vflippedvector = float2((a - b).y, -(a - b).x);
    float vo = dot(vflippedvector, (c - a));
    float v = dot(pos - a, vflippedvector) / vo;

    bool hit = max(max((u + v), 1 - u), 1 - v) < 1;
    return float3(u, v, hit);

}

inline float edgeFunction(float2 a, float2 b, float2 c)
{
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}
inline float edgeFunction(float3 a, float3 b, float3 c)
{
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}
inline float3 triangle(float2 p, float2 v0, float2 v1, float2 v2)
{
    float w0 = edgeFunction(v1, v2, p);
    float w1 = edgeFunction(v2, v0, p);
    float w2 = edgeFunction(v0, v1, p);
    return float3(w0, w1, w2);
}
inline bool triangle(float2 p, float3 v0i, float3 v1i, float3 v2i, float area, float3* coords)
{
    float2 v0 = float2(v0i.x, v0i.y);
    float2 v1 = float2(v1i.x, v1i.y);
    float2 v2 = float2(v2i.x, v2i.y);
    //float2 p = float2(xi, yi);

    float3 tr = triangle(p, v0, v1, v2);

    if (tr.x >= 0 && tr.y >= 0 && tr.z >= 0)
    {
        tr /= area;
        *coords = tr;
        return true;
        //dist = tr.x * BoxScreenPos1.z + tr.y * BoxScreenPos2.z + tr.z * BoxScreenPos3.z;
    }
    else
    {
        return false;
    }
}

inline float3 RasterizeSquare(float2 pos, float2 a, float2 b, float2 c, float2 d)
{
    float3 h1 = RasterizeTriangle(pos, a, b, c);
    float3 h2 = RasterizeTriangle(pos, d, c, b);
    if (h1.z > 0)
        return h1;
    else if (h2.z > 0)
        return 1.0f - h2;
    else return float3(0, 0, 0);
}
inline float3 CalculateUV(float3 LocalPos, float3 LocalDir, float dist, float3 Normal, bool Sphere)
{
    
    float3 UV = float3(0, 0, 0);
    if (Sphere)
    {
        UV = RaySphereUV(Normal);
    }
    else
    {
        float3 PosLocal = float3(LocalPos.x + LocalDir.x * dist,
                                 LocalPos.y + LocalDir.y * dist,
                                 LocalPos.z + LocalDir.z * dist);

        UV = RayBoxUV(PosLocal, Normal);
    }
    return UV;


}

inline float3 CalculateLight(float3 Normal, float3 BoxRightTransposed, float3 BoxForwardTransposed, float3 BoxUpTransposed)
{
    // Transform the normal from local space to world space.
    float3 WorldNormal = float3(dot(Normal, BoxRightTransposed), dot(Normal, BoxForwardTransposed), dot(Normal, BoxUpTransposed));
    float3 LightDir = normalize(float3(-0.8f, -0.5f, 0.75f));
    float3 DirectionalLightColor = float3(1, 1, 1);
    float3 AmbientLightColor = float3(0.658f, 0.6431f, 0.549f);

    float NdotL = clamp01(dot(WorldNormal, LightDir));
    float3 DirectionalLight = (DirectionalLightColor * NdotL);
    float3 Light = (AmbientLightColor + DirectionalLight);
    return Light;
}

void DrawSphere(EngineState* engineState, game_offscreen_buffer* VideoBuffer, transform Camera, transform Box, Image* texture1, Image* texture2, int ShaderIndex, bool selected, bool hovered, int Region = -1, int Regions = 9)
{
    ProfilerBeingSample(&engineState->profilingData);

    float AspectRatio = 0.5625f;
    transform CameraTransform = Camera;
    CameraTransform.up = CameraTransform.up * AspectRatio;

    float SphereScale = 0;

    SphereScale = max(Box.scale);
    Box.scale = float3(1, 1, 1);


    float depth = 1;
    int MinX = 0;
    int MaxX = 0;
    int MinY = 0;
    int MaxY = 0;

    float3 CenterPosScreenspace = ToScreenPos(VideoBuffer, Box.position, CameraTransform);
    CenterPosScreenspace = (CenterPosScreenspace - float3(VideoBuffer->Width / 2, VideoBuffer->Height / 2, 0));
    CenterPosScreenspace.x *= 0.1f;
    CenterPosScreenspace.y *= 0.1f;

    float3 BoxScreenPos1 = ToScreenPos(VideoBuffer, Box.position + normalize(CameraTransform.right) * 1.3 * SphereScale, CameraTransform);
    float3 BoxScreenPos2 = ToScreenPos(VideoBuffer, Box.position - normalize(CameraTransform.right) * 1.3 * SphereScale, CameraTransform);
    float3 BoxScreenPos3 = ToScreenPos(VideoBuffer, Box.position + normalize(CameraTransform.up) * 1.3 * SphereScale, CameraTransform);
    float3 BoxScreenPos4 = ToScreenPos(VideoBuffer, Box.position - normalize(CameraTransform.up) * 1.3 * SphereScale, CameraTransform);
    float3 BoxScreenPos5 = ToScreenPos(VideoBuffer, Box.position + normalize(CameraTransform.forward) * 1.3 * SphereScale, CameraTransform);
    float3 BoxScreenPos6 = ToScreenPos(VideoBuffer, Box.position - normalize(CameraTransform.forward) * 1.3 * SphereScale, CameraTransform);

    MinX = BoxScreenPos2.x + CenterPosScreenspace.x / CenterPosScreenspace.z;
    MaxX = BoxScreenPos1.x + CenterPosScreenspace.x / CenterPosScreenspace.z;
    MinY = BoxScreenPos3.y + CenterPosScreenspace.y / CenterPosScreenspace.z;
    MaxY = BoxScreenPos4.y + CenterPosScreenspace.y / CenterPosScreenspace.z;
    float MinZ = BoxScreenPos3.z;
    float MaxZ = BoxScreenPos4.z;
    depth = MaxZ;


    if (depth < 0)
    {
        ProfilerEndSample(&engineState->profilingData, "DrawSphere");
        return;
    }

    int X = (MinX + MaxX) / 2;
    int Y = (MinY + MaxY) / 2;
    int width = MaxX - MinX;
    int height = MaxY - MinY;

    int2 ChangedSize = GetThreadRegion(engineState, Regions, Region, VideoBuffer->Height, MinY, MaxY);
    MinY = ChangedSize.x;
    MaxY = ChangedSize.y;

    if (MinX < 0)
        MinX = 0;
    if (MinY < 0)
        MinY = 0;

    if (MaxX > VideoBuffer->Width)
        MaxX = VideoBuffer->Width;
    if (MaxY > VideoBuffer->Height)
        MaxY = VideoBuffer->Height;

    // Set up camera frustum.. thing
    float3 TopLeft = normalize((CameraTransform.forward + -CameraTransform.right) + CameraTransform.up);
    float3 TopRight = normalize((CameraTransform.forward + CameraTransform.right) + CameraTransform.up);
    float3 BottomLeft = normalize((CameraTransform.forward + -CameraTransform.right) + -CameraTransform.up);
    float3 BottomRight = normalize((CameraTransform.forward + CameraTransform.right) + -CameraTransform.up);

    float3 RightOffset = (TopRight - TopLeft);
    float3 DownOffset = (BottomLeft - TopLeft);
    float3 RightStep = RightOffset / (VideoBuffer->Width);
    float3 DownStep = DownOffset / (VideoBuffer->Height);
    float3 Dir = TopLeft + (RightStep * MinX) + (DownStep * MinY);

    ProfilerBeingSample(&engineState->profilingData);


    float SizeA = 16;
    float SizeB = 256 / SizeA;
    uint8 xStepLoop = 0;
    uint8 yStepLoop = 0;
    float xvalue = 0;
    float yvalue = 0;

    float dist = 0;
    float LastDist = 0;
    float CurrentDist = 0;
    float3 Normal = float3(0, 0, 1);
    float3 LastNormal = float3(0, 0, 1);
    float3 CurrentNormal = float3(0, 0, 1);
    float3 Light;
    float3 Light1;
    float3 Light2;
    float3 UV;
    float3 UV1;
    float3 UV2;

    float3 BoxRightTransposed = float3(Box.right.x, Box.forward.x, Box.up.x);
    float3 BoxForwardTransposed = float3(Box.right.y, Box.forward.y, Box.up.y);
    float3 BoxUpTransposed = float3(Box.right.z, Box.forward.z, Box.up.z);

    float3 LocalPos = float3(dot(Camera.position - Box.position, Box.right / Box.scale.x), dot(Camera.position - Box.position, Box.forward / Box.scale.y), dot(Camera.position - Box.position, Box.up / Box.scale.z));

    float3 aa = Box.right / Box.scale.x;
    float3 bb = Box.forward / Box.scale.y;
    float3 cc = Box.up / Box.scale.z;

    bool detailed = false;

    for (int yi = MinY; yi < MaxY; yi++)
    {
        for (int xi = MinX; xi < MaxX; xi++)
        {
            uint32* Pixel = &((uint32*)VideoBuffer->Memory)[yi * VideoBuffer->Width + xi];
            Dir += RightStep;

            bool skip = false;

            float3 DirNormalized = normalize(Dir);

            float3 LocalDir = float3(dot(DirNormalized, aa), dot(DirNormalized, bb), dot(DirNormalized, cc));
            dist = RaySphereIntersect(LocalPos, (LocalDir), SphereScale);
            float3 PosLocal = float3(LocalPos.x + LocalDir.x * dist,
                LocalPos.y + LocalDir.y * dist,
                LocalPos.z + LocalDir.z * dist);
            float3 n = normalizeFast(PosLocal);
            UV = CalculateUV(LocalPos, (LocalDir), dist, n, true);
            Light = CalculateLight(n, BoxRightTransposed, BoxForwardTransposed, BoxUpTransposed);

            if (dist < 0)
            {
                xStepLoop += SizeB;
                xvalue += 1.0f / SizeA;
                continue;
            }


            uint8 OldDepth = ((uint8*)Pixel)[3];

            uint8 NewDepth = DistanceToDepthBufferValue(dist);
            if (NewDepth == 255)
            {
                xStepLoop += SizeB;
                xvalue += 1.0f / SizeA;
                continue;
            }

            if (NewDepth > OldDepth)
            {
                xStepLoop += SizeB;
                xvalue += 1.0f / SizeA;
                continue;
            }

            uint32 TextureSample = ImageSample(texture1, UV.x, UV.y);
            float3 SurfaceColor = GetBufferColorReverse(VideoBuffer, TextureSample);
            SurfaceColor = clamp01(SurfaceColor * Light);

            if (selected)
                SurfaceColor += float3(0.25f, 0.25f, 0.5f);
            else if (hovered)
                SurfaceColor += float3(0.05f, 0.05f, 0.15f);
            SurfaceColor = clamp01(SurfaceColor);

            uint32 NewSample = MakeBufferColor(VideoBuffer, SurfaceColor);

            if (ShaderIndex == ShaderType_Additive)
            {
                float3 NewPixel = GetBufferColorReverse(VideoBuffer, NewSample);
                float3 OldPixel = GetBufferColorReverse(VideoBuffer, *Pixel);

                float3 Result = clamp01(NewPixel + OldPixel);

                uint32 ResultData = MakeBufferColor(VideoBuffer, Result);

                *Pixel = ResultData;
                ((uint8*)Pixel)[3] = OldDepth;
            }
            else
            {
                *Pixel = NewSample;

                ((uint8*)Pixel)[3] = NewDepth;
            }

            xStepLoop += SizeB;
            xvalue += 1.0f / SizeA;
        }

        if (yStepLoop == 0)
        {
            yvalue = 0;
        }
        yvalue += 1.0f / SizeA;

        yStepLoop += SizeB;
        xStepLoop = 0;

        Dir = Dir - (RightStep * (MaxX - MinX));
        Dir = Dir + DownStep;
    }
    ProfilerEndSample(&engineState->profilingData, "Rasterize");
    ProfilerEndSample(&engineState->profilingData, "DrawSphere");
}

void DrawBox(EngineState* engineState, game_offscreen_buffer* VideoBuffer, transform Camera, transform Box, Image* texture1, Image* texture2, int ShaderIndex, bool selected, bool hovered, int Region = -1, int Regions = 9)
{
    ProfilerBeingSample(&engineState->profilingData);

    float AspectRatio = 0.5625f;
    transform CameraTransform = Camera;
    CameraTransform.up = CameraTransform.up * AspectRatio;

    float SphereScale = 0;

    float depth = 1;
    int MinX = 0;
    int MaxX = 0;
    int MinY = 0;
    int MaxY = 0;

    float3 BoxPosW1 = Box.position + ((-Box.right * Box.scale.x) + (-Box.forward * Box.scale.y) + (-Box.up * Box.scale.z)) * 0.5;
    float3 BoxPosW2 = Box.position + ((Box.right * Box.scale.x) + (-Box.forward * Box.scale.y) + (-Box.up * Box.scale.z)) * 0.5;
    float3 BoxPosW3 = Box.position + ((-Box.right * Box.scale.x) + (Box.forward * Box.scale.y) + (-Box.up * Box.scale.z)) * 0.5;
    float3 BoxPosW4 = Box.position + ((Box.right * Box.scale.x) + (Box.forward * Box.scale.y) + (-Box.up * Box.scale.z)) * 0.5;
    float3 BoxPosW5 = Box.position + ((-Box.right * Box.scale.x) + (-Box.forward * Box.scale.y) + (Box.up * Box.scale.z)) * 0.5;
    float3 BoxPosW6 = Box.position + ((Box.right * Box.scale.x) + (-Box.forward * Box.scale.y) + (Box.up * Box.scale.z)) * 0.5;
    float3 BoxPosW7 = Box.position + ((-Box.right * Box.scale.x) + (Box.forward * Box.scale.y) + (Box.up * Box.scale.z)) * 0.5;
    float3 BoxPosW8 = Box.position + ((Box.right * Box.scale.x) + (Box.forward * Box.scale.y) + (Box.up * Box.scale.z)) * 0.5;

    float3 BoxScreenPos1 = ToScreenPos(VideoBuffer, BoxPosW1, CameraTransform);
    float3 BoxScreenPos2 = ToScreenPos(VideoBuffer, BoxPosW2, CameraTransform);
    float3 BoxScreenPos3 = ToScreenPos(VideoBuffer, BoxPosW3, CameraTransform);
    float3 BoxScreenPos4 = ToScreenPos(VideoBuffer, BoxPosW4, CameraTransform);
    float3 BoxScreenPos5 = ToScreenPos(VideoBuffer, BoxPosW5, CameraTransform);
    float3 BoxScreenPos6 = ToScreenPos(VideoBuffer, BoxPosW6, CameraTransform);
    float3 BoxScreenPos7 = ToScreenPos(VideoBuffer, BoxPosW7, CameraTransform);
    float3 BoxScreenPos8 = ToScreenPos(VideoBuffer, BoxPosW8, CameraTransform);

    MinX = VideoBuffer->Width;
    MaxX = 0;
    MinY = VideoBuffer->Height;
    MaxY = 0;
    if (BoxScreenPos1.z < 0 &&
        BoxScreenPos2.z < 0 &&
        BoxScreenPos3.z < 0 &&
        BoxScreenPos4.z < 0 &&
        BoxScreenPos5.z < 0 &&
        BoxScreenPos6.z < 0 &&
        BoxScreenPos7.z < 0 &&
        BoxScreenPos8.z < 0)
    {
        ProfilerEndSample(&engineState->profilingData, "DrawBox");
        return;
    }
    if (BoxScreenPos1.z > 0)
    {
        MinX = min(MinX, BoxScreenPos1.x);
        MaxX = max(MaxX, BoxScreenPos1.x);
        MinY = min(MinY, BoxScreenPos1.y);
        MaxY = max(MaxY, BoxScreenPos1.y);
    }
    if (BoxScreenPos2.z > 0)
    {
        MinX = min(MinX, BoxScreenPos2.x);
        MaxX = max(MaxX, BoxScreenPos2.x);
        MinY = min(MinY, BoxScreenPos2.y);
        MaxY = max(MaxY, BoxScreenPos2.y);
    }
    if (BoxScreenPos3.z > 0)
    {
        MinX = min(MinX, BoxScreenPos3.x);
        MaxX = max(MaxX, BoxScreenPos3.x);
        MinY = min(MinY, BoxScreenPos3.y);
        MaxY = max(MaxY, BoxScreenPos3.y);
    }
    if (BoxScreenPos4.z > 0)
    {
        MinX = min(MinX, BoxScreenPos4.x);
        MaxX = max(MaxX, BoxScreenPos4.x);
        MinY = min(MinY, BoxScreenPos4.y);
        MaxY = max(MaxY, BoxScreenPos4.y);
    }
    if (BoxScreenPos5.z > 0)
    {
        MinX = min(MinX, BoxScreenPos5.x);
        MaxX = max(MaxX, BoxScreenPos5.x);
        MinY = min(MinY, BoxScreenPos5.y);
        MaxY = max(MaxY, BoxScreenPos5.y);
    }
    if (BoxScreenPos6.z > 0)
    {
        MinX = min(MinX, BoxScreenPos6.x);
        MaxX = max(MaxX, BoxScreenPos6.x);
        MinY = min(MinY, BoxScreenPos6.y);
        MaxY = max(MaxY, BoxScreenPos6.y);
    }
    if (BoxScreenPos7.z > 0)
    {
        MinX = min(MinX, BoxScreenPos7.x);
        MaxX = max(MaxX, BoxScreenPos7.x);
        MinY = min(MinY, BoxScreenPos7.y);
        MaxY = max(MaxY, BoxScreenPos7.y);
    }
    if (BoxScreenPos8.z > 0)
    {
        MinX = min(MinX, BoxScreenPos8.x);
        MaxX = max(MaxX, BoxScreenPos8.x);
        MinY = min(MinY, BoxScreenPos8.y);
        MaxY = max(MaxY, BoxScreenPos8.y);
    }

    float MinZ = min(min(min(min(min(min(min(BoxScreenPos1.z, BoxScreenPos2.z), BoxScreenPos3.z), BoxScreenPos4.z), BoxScreenPos5.z), BoxScreenPos6.z), BoxScreenPos7.z), BoxScreenPos8.z);
    float MaxZ = max(max(max(max(max(max(max(BoxScreenPos1.z, BoxScreenPos2.z), BoxScreenPos3.z), BoxScreenPos4.z), BoxScreenPos5.z), BoxScreenPos6.z), BoxScreenPos7.z), BoxScreenPos8.z);

    depth = MaxZ;


    if (depth < 0)
    {
        ProfilerEndSample(&engineState->profilingData, "DrawBox");
        return;
    }

    int X = (MinX + MaxX) / 2;
    int Y = (MinY + MaxY) / 2;
    int width = MaxX - MinX;
    int height = MaxY - MinY;

    int2 ChangedSize = GetThreadRegion(engineState, Regions, Region, VideoBuffer->Height, MinY, MaxY);
    MinY = ChangedSize.x;
    MaxY = ChangedSize.y;

    if (MinX < 0)
        MinX = 0;
    if (MinY < 0)
        MinY = 0;

    if (MaxX > VideoBuffer->Width)
        MaxX = VideoBuffer->Width;
    if (MaxY > VideoBuffer->Height)
        MaxY = VideoBuffer->Height;

    // Set up camera frustum.. thing
    float3 TopLeft = normalize((CameraTransform.forward + -CameraTransform.right) + CameraTransform.up);
    float3 TopRight = normalize((CameraTransform.forward + CameraTransform.right) + CameraTransform.up);
    float3 BottomLeft = normalize((CameraTransform.forward + -CameraTransform.right) + -CameraTransform.up);
    float3 BottomRight = normalize((CameraTransform.forward + CameraTransform.right) + -CameraTransform.up);

    float3 RightOffset = (TopRight - TopLeft);
    float3 DownOffset = (BottomLeft - TopLeft);
    float3 RightStep = RightOffset / (VideoBuffer->Width);
    float3 DownStep = DownOffset / (VideoBuffer->Height);
    float3 Dir = TopLeft + (RightStep * MinX) + (DownStep * MinY);

    float SizeA = 8;
    float SizeB = 256 / SizeA;
    uint8 xStepLoop = 0;
    uint8 yStepLoop = 0;
    float xvalue = 0;
    float yvalue = 0;

    float3 Normal = float3(0, 0, 1);
    float3 LastNormal = float3(0, 0, 1);
    float3 CurrentNormal = float3(0, 0, 1);
    float3 Light;
    float3 Light1;
    float3 Light2;
    float dist = 0;
    float LastDist = 0;
    float CurrentDist = 0;
    float distStep = 0;
    float3 UV;
    float3 UV1;
    float3 UV2;
    float3 UVStep;

    float3 BoxRightTransposed = float3(Box.right.x, Box.forward.x, Box.up.x);
    float3 BoxForwardTransposed = float3(Box.right.y, Box.forward.y, Box.up.y);
    float3 BoxUpTransposed = float3(Box.right.z, Box.forward.z, Box.up.z);

    float3 LocalPos = float3(dot(Camera.position - Box.position, Box.right / Box.scale.x), dot(Camera.position - Box.position, Box.forward / Box.scale.y), dot(Camera.position - Box.position, Box.up / Box.scale.z));

    float3 aa = Box.right / Box.scale.x;
    float3 bb = Box.forward / Box.scale.y;
    float3 cc = Box.up / Box.scale.z;

    bool detailed = false;
    bool skip = false;


    ProfilerBeingSample(&engineState->profilingData);
    for (int yi = MinY; yi < MaxY; yi++)
    {
        bool draw = false;
        for (int xi = MinX; xi < MaxX; xi++)
        {
            Dir += RightStep;
            
            if (xStepLoop == 0)
            {
                skip = false;
                detailed = false;
                float3 Dir2 = Dir + RightStep * SizeA;
                float3 WorldDir2 = normalizeFast(Dir2);
                float3 LocalDir2 = float3(dot(WorldDir2, aa), dot(WorldDir2, bb), dot(WorldDir2, cc));

                LastNormal = CurrentNormal;
                LastDist = CurrentDist;
                CurrentDist = RayBoxIntersect(LocalPos, LocalDir2, &CurrentNormal);

                UV1 = UV2;
                Light1 = Light2;
                UV2 = CalculateUV(LocalPos, LocalDir2, CurrentDist, CurrentNormal, false);
                Light2 = CalculateLight(CurrentNormal, BoxRightTransposed, BoxForwardTransposed, BoxUpTransposed);
                Light = Light2;

                distStep = (CurrentDist - LastDist) / SizeA;
                UVStep = (UV2 - UV1) / SizeA;

                UV = UV1;
                dist = CurrentDist;

                if ((CurrentNormal.x != LastNormal.x || CurrentNormal.y != LastNormal.y || CurrentNormal.z != LastNormal.z) ||
                    LastDist < 0 ||
                    CurrentDist < 0)
                {
                    detailed = true;
                }

                skip = LastDist < 0 && CurrentDist < 0;
                xvalue = 0;
            }

            if (skip)
            {
                xStepLoop += SizeB;
                xvalue += 1.0f / SizeA;
                continue;
            }

            if (!skip && detailed)
            {
                float3 WorldDir = normalizeFast(Dir);
                float3 LocalDir = float3(dot(WorldDir, aa), dot(WorldDir, bb), dot(WorldDir, cc));
                dist = RayBoxIntersect(LocalPos, LocalDir, &Normal);
                UV = CalculateUV(LocalPos, LocalDir, dist, Normal, false);
                Light = CalculateLight(Normal, BoxRightTransposed, BoxForwardTransposed, BoxUpTransposed);

                if (dist < 0)
                {
                    xStepLoop += SizeB;
                    xvalue += 1.0f / SizeA;
                    continue;
                }
            }
            else
            {
                UV = lerp(UV1, UV2, xvalue);
                dist = lerp(LastDist, CurrentDist, xvalue);
            }

            uint32* Pixel = &((uint32*)VideoBuffer->Memory)[yi * VideoBuffer->Width + xi];
            uint8 OldDepth = ((uint8*)Pixel)[3];

            //bool other = xi % 2 == 0;
            //
            //uint8* left = &(((uint8*)Pixel)[3]);
            //uint8* right = &(((uint8*)Pixel)[3]);
            //if (other)
            //{
            //    //left = Pixel; &((uint32*)VideoBuffer->Memory)[yi * VideoBuffer->Width + xi];
            //    right = &(((uint8*)&((uint32*)VideoBuffer->Memory)[yi * VideoBuffer->Width + xi + 1])[3]);
            //    draw = false;
            //}
            //else
            //{
            //    left = &(((uint8*)&((uint32*)VideoBuffer->Memory)[yi * VideoBuffer->Width + xi - 1])[3]);
            //    //right = Pixel; &((uint32*)VideoBuffer->Memory)[yi * VideoBuffer->Width + xi];
            //}
            //
            //// Read the old depth
            //uint16 OldDepth = *left | (*right << 8);
            //
            //// Get and write the new depth
            //uint16 NewDepth = DistanceToDepthBufferValue2(dist);
            //
            uint8 NewDepth = DistanceToDepthBufferValue(dist);
            //if (!draw)
            //{
                if (NewDepth == (256 * 256) - 1)
                {
                    xStepLoop += SizeB;
                    xvalue += 1.0f / SizeA;
                    continue;
                }
            
                if (NewDepth > OldDepth)
                {
                    xStepLoop += SizeB;
                    xvalue += 1.0f / SizeA;
                    continue;
                }
            //}
            uint32 TextureSample = ImageSample(texture1, UV.x, UV.y);
            float3 SurfaceColor = GetBufferColorReverse(VideoBuffer, TextureSample);
            SurfaceColor = clamp01(SurfaceColor * Light);

            if (selected)
                SurfaceColor += float3(0.25f, 0.25f, 0.5f);
            else if (hovered)
                SurfaceColor += float3(0.05f, 0.05f, 0.15f);
            SurfaceColor = clamp01(SurfaceColor);

            uint32 NewSample = MakeBufferColor(VideoBuffer, SurfaceColor);

            *Pixel = NewSample;

            ((uint8*)Pixel)[3] = NewDepth;
            //*left = (uint8)NewDepth;
            //*right = (uint8)(NewDepth >> 8);
            draw = true;

            xStepLoop += SizeB;
            xvalue += 1.0f / SizeA;
        }

        if (yStepLoop == 0)
        {
            yvalue = 0;
        }
        yvalue += 1.0f / SizeA;

        yStepLoop += SizeB;
        xStepLoop = 0;

        Dir = Dir - (RightStep * (MaxX - MinX));
        Dir = Dir + DownStep;
    }
    ProfilerEndSample(&engineState->profilingData, "Rasterize");
    ProfilerEndSample(&engineState->profilingData, "DrawBox");
}

void DrawSceneRegion(int thread, game_memory* Memory, game_offscreen_buffer* VideoBuffer, game_sound_output_buffer* SoundBuffer)
{
    EngineState* engineState = (EngineState*)Memory->TransientStorage;
    EngineSaveState* engineSaveState = (EngineSaveState*)Memory->PermanentStorage;

    int regionCount = engineSaveState->ScreenThreads;
    for (int i = 0; i < engineState->NextRenderCommand; i++)
    {
        RenderCommand command = engineState->RenderCommands[i];
        if (command.Type == RenderCommandType_Sphere)
        {
            //if(engineState->renderTest2)
            //    DrawObject_very_old(engineState, VideoBuffer, engineState->Camera, command.t, true, command.texture1, command.texture2, command.ShaderIndex, command.selected, command.hovered, thread, regionCount);
            //else
            DrawSphere(engineState, VideoBuffer, engineState->Camera, command.t, command.texture1, command.texture2, command.ShaderIndex, command.selected, command.hovered, thread, regionCount);
        }
        else if (command.Type == RenderCommandType_Box)
        {
            //if (engineState->renderTest2)
            //    DrawObject_very_old(engineState, VideoBuffer, engineState->Camera, command.t, false, command.texture1, command.texture2, command.ShaderIndex, command.selected, command.hovered, thread, regionCount);
            //else
            DrawBox(engineState, VideoBuffer, engineState->Camera, command.t, command.texture1, command.texture2, command.ShaderIndex, command.selected, command.hovered, thread, regionCount);
        }
        else if (command.Type == RenderCommandType_Skybox)
        {
            DrawSkybox(engineState, VideoBuffer, engineState->Camera, thread, regionCount);
        }
        else if (command.Type == RenderCommandType_Clear)
        {
            DrawRectangle(VideoBuffer, float2(0, 0), float2(1920 / 2, 1080 / 2), command.color);
        }
    }

    // draw particles last so sorting works
    for (int i = 0; i < engineState->NextRenderCommand; i++)
    {
        RenderCommand command = engineState->RenderCommands[i];
        if (command.Type == RenderCommandType_Effect)
        {
            DrawEffect(engineState, VideoBuffer, engineState->Camera, &command, thread, regionCount);
        }
    }

    int MinX = 0;
    int MinY = 0;
    int MaxX = VideoBuffer->Width;
    int MaxY = VideoBuffer->Height;
    int2 ChangedSize = GetThreadRegion(engineState, regionCount, thread, VideoBuffer->Height, MinY, MaxY);
    MinY = ChangedSize.x;
    MaxY = ChangedSize.y;
    if (engineSaveState->ProfilingEnabled && engineSaveState->ThreadedRendering && engineSaveState->Visualize_ThreadedRendering)
    {
        DrawRectangle(VideoBuffer, float2(MinX, MinY + 1), float2(VideoBuffer->Width, MaxY - MinY - 1), frac(float3(thread / 3.0f, thread / 6.0f, thread / 12.0f)), 0.2f);
        DrawLine(VideoBuffer, float2(0, engineState->RenderThreadEnd[thread] * VideoBuffer->Height), float2(VideoBuffer->Width, engineState->RenderThreadEnd[thread] * VideoBuffer->Height));
    }
}

// draws the scene, threading and all!
void DrawScene(game_memory* Memory, EngineState* engineState, EngineSaveState* engineSaveState, game_offscreen_buffer* VideoBuffer)
{
    // Consume draw queue
    if (engineSaveState->ThreadedRendering)
    {
        //Memory->PlatformPrint("Main Thread - BeginDraw");
        DrawRectangle(VideoBuffer, float2(0, 0), float2(1920, 1080), float3(0.5f, 0, 0));
        static int framedelay = 0;
        framedelay++;
        if (framedelay > 10)
        {
            for (int i = 0; i < engineSaveState->ScreenThreads; i++)
            {
                Memory->PlatformThreadCall(&DrawSceneRegion, i);
            }
        }
        else
        {
            for (int i = 0; i < engineSaveState->ScreenThreads; i++)
            {
                Memory->ThreadsExecuting[i] = false;
            }
        }

        for (int i = 0; i < ArrayCount(engineState->RenderThreadTimes); i++)
        {
            engineState->RenderThreadTimes[i] = 0;
        }

        // Spinlock until they are all done.
        int count = 0;
        while (true)
        {
            count++;
            bool AnyExecuting = false;
            for (int i = 0; i < engineSaveState->ScreenThreads; i++)
            {
                if (Memory->ThreadsExecuting[i] == true)
                {
                    AnyExecuting = true;
                }
                else
                {
                    if (engineState->RenderThreadTimes[i] == 0)
                        engineState->RenderThreadTimes[i] = count;
                }
            }

            // If no threads are executing, we are done.
            if (!AnyExecuting)
                break;

        }

        if (engineSaveState->ProfilingEnabled && engineSaveState->Visualize_ThreadedRendering)
        {
            float2 start = float2(210, VideoBuffer->Height - 190);
            DrawRectangle(VideoBuffer, start - 5, float2(700, engineSaveState->ScreenThreads * 5 + 10), float3(0.0, 0.0, 0.0), 0.5);
            for (int i = 0; i < engineSaveState->ScreenThreads; i++)
            {
                float2 innerStart = float2(0, i * 5);
                DrawLine(VideoBuffer, start + innerStart, start + innerStart + float2(engineState->RenderThreadTimes[i] / 5000, 0));
            }
        }

        if (engineSaveState->ThreadedRendering_Dynamic)
        {
            int sum = 0;
            for (int i = 0; i < engineSaveState->ScreenThreads; i++)
            {
                sum += engineState->RenderThreadTimes[i];
            }
            int average = sum / engineSaveState->ScreenThreads;

            for (int i = 0; i < engineSaveState->ScreenThreads; i++)
            {
                if (engineState->RenderThreadTimes[i] > average) // Took longer, reduce its size.
                {
                    engineState->RenderThreadSize[i] *= 0.95f;
                }
                else // was faster, increase its size.
                {
                    engineState->RenderThreadSize[i] *= 1.05f;
                }

                if (engineState->RenderThreadSize[i] < 0.01f)
                    engineState->RenderThreadSize[i] = 0.01f;
                else if (engineState->RenderThreadSize[i] > 0.2f)
                    engineState->RenderThreadSize[i] = 0.2f;
            }

            float sizeAverage = 0;
            for (int i = 0; i < engineSaveState->ScreenThreads; i++)
            {
                sizeAverage += engineState->RenderThreadSize[i];
            }
            for (int i = 0; i < engineSaveState->ScreenThreads; i++)
            {
                engineState->RenderThreadSize[i] /= sizeAverage;
            }
            float h = 0;
            for (int i = 0; i < engineSaveState->ScreenThreads; i++)
            {
                float lastH = h;
                h += engineState->RenderThreadSize[i];
                engineState->RenderThreadStart[i] = lastH;
                engineState->RenderThreadEnd[i] = h;
            }
        }
        else
        {
            float h = 0;
            for (int i = 0; i < engineSaveState->ScreenThreads; i++)
            {
                float lastH = h;
                h += 1.0f / engineSaveState->ScreenThreads;
                engineState->RenderThreadStart[i] = lastH;
                engineState->RenderThreadEnd[i] = h;
            }
        }
    }
    else
    {
        DrawSceneRegion(-1, Memory, VideoBuffer, 0);
    }
}