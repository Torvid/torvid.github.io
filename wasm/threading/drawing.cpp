#pragma once
#include "quote.h"


void QueueDrawSphere(GameState* gameState, transform t, Image* Texture)
{
    gameState->RenderCommands[gameState->NextRenderCommand].Texture = Texture;
    gameState->RenderCommands[gameState->NextRenderCommand].Type = RenderCommandType_Sphere;
    gameState->RenderCommands[gameState->NextRenderCommand].t = t;

    gameState->NextRenderCommand++;
}
void QueueDrawBox(GameState* gameState, transform t, Image* Texture)
{
    gameState->RenderCommands[gameState->NextRenderCommand].Texture = Texture;
    gameState->RenderCommands[gameState->NextRenderCommand].Type = RenderCommandType_Box;
    gameState->RenderCommands[gameState->NextRenderCommand].t = t;
    gameState->NextRenderCommand++;
}
void QueueDrawSkyBox(GameState* gameState, Image* SkyFaces[6])
{
    for (int i = 0; i < 6; i++)
    {
        gameState->RenderCommands[gameState->NextRenderCommand].CubemapTexture[i] = SkyFaces[i];
    }
    gameState->RenderCommands[gameState->NextRenderCommand].Type = RenderCommandType_Skybox;

    gameState->NextRenderCommand++;
}
void QueueClear(GameState* gameState, float3 Color)
{
    gameState->RenderCommands[gameState->NextRenderCommand].Color = Color;
    gameState->RenderCommands[gameState->NextRenderCommand].Type = RenderCommandType_Clear;
    gameState->NextRenderCommand++;
}
uint32 MakeBufferColor(game_offscreen_buffer* Buffer, float R, float G, float B)
{
    if (Buffer->Layout == ChannelLayout_ABGR)
    {
        return 255 << 24 |
            RoundToInt(R * 255.0f) << 16 |
            RoundToInt(G * 255.0f) << 8 |
            RoundToInt(B * 255.0f) << 0;
    }
    else
    {
        return  255 << 24 |
            RoundToInt(B * 255.0f) << 16 |
            RoundToInt(G * 255.0f) << 8 |
            RoundToInt(R * 255.0f) << 0;
    }
}

uint32 ToColor(float R, float G, float B, float A)
{
    return  (uint8)(clamp01(A) * 255) << 24 |
        (uint8)(clamp01(R) * 255) << 16 |
        (uint8)(clamp01(G) * 255) << 8 |
        (uint8)(clamp01(B) * 255) << 0;
}
uint32 ToColor(float3 Color, float A)
{
    return ToColor(Color.x, Color.y, Color.z, A);
}
uint32 ToColor(float3 Color)
{
    return ToColor(Color.x, Color.y, Color.z, 1);
}
uint32 ToColor(float R)
{
    return ToColor(R, R, R, 1);
}



uint32 ImageSample(Image* image, float x, float y)
{
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


void DrawImageScaled(game_offscreen_buffer* Buffer, Image* image,
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
            *Pixel++ = image->ImageData[image_y * image->width + image_x];
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


void DrawRectangle(game_offscreen_buffer* Buffer,
    float X, float Y,
    float Width, float Height, float R, float G, float B, float A = 1)
{
    int MinX = FloorToInt(X);
    int MinY = FloorToInt(Y);
    int MaxX = FloorToInt(X + Width);
    int MaxY = FloorToInt(Y + Height);

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
    uint32 Color = MakeBufferColor(Buffer, B, G, R);
    if (A == 1)
    {
        for (int y = MinY; y < MaxY; y++)
        {
            uint32* Pixel = (uint32*)Row;
            for (int x = MinX; x < MaxX; x++)
            {
                ((uint8*)Pixel)[0] = ((uint8*)&Color)[0];
                ((uint8*)Pixel)[1] = ((uint8*)&Color)[1];
                ((uint8*)Pixel)[2] = ((uint8*)&Color)[2];
                ((uint8*)Pixel)[3] = ((uint8*)&Color)[3];
                Pixel++;
            }
            Row += Buffer->Pitch;
        }
    }
    else
    {
        float wr = ((uint8*)&Color)[0] / 256.0f;
        float wg = ((uint8*)&Color)[1] / 256.0f;
        float wb = ((uint8*)&Color)[2] / 256.0f;
        float wa = ((uint8*)&Color)[3] / 256.0f;
        wr = pow(wr, 2.2f);
        wg = pow(wg, 2.2f);
        wb = pow(wb, 2.2f);
        wa = pow(wa, 2.2f);

        for (int y = MinY; y < MaxY; y++)
        {
            uint32* Pixel = (uint32*)Row;
            for (int x = MinX; x < MaxX; x++)
            {
                float vr = ((uint8*)Pixel)[0] / 256.0f;
                float vg = ((uint8*)Pixel)[1] / 256.0f;
                float vb = ((uint8*)Pixel)[2] / 256.0f;
                float va = ((uint8*)Pixel)[3] / 256.0f;

                // This is meant to be pow 2.2 and 0.454545, but it's too slow.
                vr = vr * vr;//pow(vr, 2.2f);
                vg = vg * vg;//pow(vg, 2.2f);
                vb = vb * vb;//pow(vb, 2.2f);
                va = va * va;//pow(va, 2.2f);

                vr *= 1.0 - A;
                vg *= 1.0 - A;
                vb *= 1.0 - A;
                va *= 1.0 - A;
                vr += wr * A;
                vg += wg * A;
                vb += wb * A;
                va += wa * A;

                vr = sqrt(vr); //pow(vr, 0.4545f);
                vg = sqrt(vg); //pow(vg, 0.4545f);
                vb = sqrt(vb); //pow(vb, 0.4545f);
                va = sqrt(va); //pow(va, 0.4545f);

                ((uint8*)Pixel)[0] = vr * 256.0f;
                ((uint8*)Pixel)[1] = vg * 256.0f;
                ((uint8*)Pixel)[2] = vb * 256.0f;
                //((uint8*)Pixel)[3] = va * 256.0f;

                Pixel++;
            }
            Row += Buffer->Pitch;
        }
    }
}
void DrawRectangle(game_offscreen_buffer* Buffer,
    float2 Pos,
    float2 Size, float3 Color, float A = 1)
{
    DrawRectangle(Buffer, Pos.x, Pos.y, Size.x, Size.y, Color.x, Color.y, Color.z, A);
}

void DrawRectangleSoft(game_offscreen_buffer* Buffer,
    float X, float Y,
    float Width, float Height, float R, float G, float B)
{
    int MinX = FloorToInt(X);
    int MinY = FloorToInt(Y);
    int MaxX = CeilToInt(X + Width);
    int MaxY = CeilToInt(Y + Height);

    float MinXAlpha = X - MinX;
    float MinYAlpha = Y - MinY;
    float MaxXAlpha = (X + Width) - MaxX;
    float MaxYAlpha = (Y + Height) - MaxY;

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

    for (int y = MinY; y < MaxY; y++)
    {
        uint32* Pixel = (uint32*)Row;
        for (int x = MinX; x < MaxX; x++)
        {
            float C = R;
            float MinXC = (1.0f - MinXAlpha);
            float MinYC = (1.0f - MinYAlpha);
            float MaxXC = (1.0f - (-MaxXAlpha));
            float MaxYC = (1.0f - (-MaxYAlpha));

            bool32 IsMinX = (x == MinX);
            bool32 IsMinY = (y == MinY);
            bool32 IsMaxX = (x == (MaxX - 1));
            bool32 IsMaxY = (y == (MaxY - 1));

            if (IsMinX) C = R * MinXC;
            if (IsMinY) C = R * MinYC;
            if (IsMaxX) C = R * MaxXC;
            if (IsMaxY) C = R * MaxYC;

            if (IsMinX && IsMinY) C = R * min(MinXC, MinYC);
            if (IsMinY && IsMaxX) C = R * min(MinYC, MaxXC);
            if (IsMaxX && IsMaxY) C = R * min(MaxXC, MaxYC);
            if (IsMaxY && IsMinX) C = R * min(MaxYC, MinXC);


            uint32 Color = MakeBufferColor(Buffer, C, C, C);
            *Pixel++ = Color;
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
        float2 a = pos + float2(sinf(i) * radius, cosf(i) * radius);
        *GetPixel(Buffer, a.x, a.y) = 0xFFFFFFFF;
    }
    DrawPoint(Buffer, pos);
}

void DrawLine(game_offscreen_buffer* Buffer, float2 Start, float2 End)
{
    if ((Start.x < 0  || Start.y < 0 || Start.y > Buffer->Height || Start.x > Buffer->Width) && 
        (End.y > Buffer->Height || End.x < 0 || End.y < 0 || End.x > Buffer->Width))
        return;

    float dist = distance(Start, End);
    for (float i = 0; i < 1.0f; i += (1.0f / dist))
    {
        float2 l = lerp(Start, End, i);
        *GetPixel(Buffer, l.x, l.y) = 0x00FFFFff;
    }
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

void DrawThing(Image* fontSpritesheet, game_offscreen_buffer* VideoBuffer, int* y, const char* text, char* value, int x = 0)
{
    char Space[100] = {};
    Append(Append(Space, text), value);
    font_draw(fontSpritesheet, VideoBuffer, x, *y, Space);
    *y += 12;
}
void DrawThing(Image* fontSpritesheet, game_offscreen_buffer* VideoBuffer, int* y, const char* text, int value, int x = 0)
{
    char Space[100] = {};
    if (value != 0)
        Append(Space, value);
    DrawThing(fontSpritesheet, VideoBuffer, y, text, Space, x);
}
void DrawThing(Image* fontSpritesheet, game_offscreen_buffer* VideoBuffer, int* y, const char* text, float2 value, int x = 0)
{
    char Space[100] = {};
    Append(Space, value);
    DrawThing(fontSpritesheet, VideoBuffer, y, text, Space, x);
}
void DrawThing(Image* fontSpritesheet, game_offscreen_buffer* VideoBuffer, int* y, const char* text, float3 value, int x = 0)
{
    char Space[100] = {};
    Append(Space, value);
    DrawThing(fontSpritesheet, VideoBuffer, y, text, Space, x);
}
void DrawThing(Image* fontSpritesheet, game_offscreen_buffer* VideoBuffer, int* y, const char* text, float value, int x = 0)
{
    char Space[100] = {};
    Append(Space, value);
    DrawThing(fontSpritesheet, VideoBuffer, y, text, Space, x);
}

// asks the platform layer to start loading an image.
Image* LoadImage(game_memory* Memory, GameState* gameState, const char* path, int width, int height)
{
    uint32* ImageLocation = PushArray(&gameState->textureArena, width * height, uint32);
    Image* newImage = &gameState->Images[gameState->CurrentImage];
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

    Memory->PlatformReadImageIntoMemory(ImageLocation, path);

    gameState->CurrentImage++;
    return newImage;
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
    float3 p2 = HitPoint + 0.5;
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


float RaySphereIntersect(float3 RayPos, float3 RayDir, float r)
{
    if (dot(RayPos, RayPos) < (r * r))
        return -1;

    float3 q = cross(RayDir, RayPos);
    if (dot(q, q) > (r * r))
        return -1;
    float dist = dot(-RayPos, RayDir) - sqrt((r * r) - dot(q, q));

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
#if 0
    float3 vAbs = abs(v);
    float ma;
    float3 uv;
    float index = 0;
    if (vAbs.z >= vAbs.x && vAbs.z >= vAbs.y)
    {
        index = v.z < 0.0 ? 5.0 : 4.0;
        ma = 0.5 / vAbs.z;
        uv = float3(v.z < 0.0 ? -v.x : v.x, -v.y, 0);
    }
    else if (vAbs.y >= vAbs.x)
    {
        index = v.y < 0.0 ? 3.0 : 2.0;
        ma = 0.5 / vAbs.y;
        uv = float3(v.y < 0.0 ? v.x : -v.x, -v.z, 0);
    }
    else
    {
        index = v.x < 0.0 ? 1.0 : 0.0;
        ma = 0.5 / vAbs.x;
        uv = float3(v.x < 0.0 ? -v.y : v.y, -v.z, 0);
    }
    uv = uv * ma + 0.5;
    uv.z = index;
    return uv;
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

float3 RaySphereUV(float3 Normal)
{
    //float u = shitty_atan2(Normal.x, Normal.y);
    //float v = -Normal.z * 0.5 + 0.5;
    //return float3(u, v, 0);

    return RayCubemap(Normal);
}

uint32 Shader(GameState* gameState, float3 UV, float depth, float3 Normal, float3 ViewDirection, bool Sphere)
{
    //if (Sphere)
    //    UV = frac(UV * 4);
    float3 LightDir = normalize(float3(-0.8, -0.5, 0.75));
    uint32 TextureSample = ImageSample(&gameState->Images[2], UV.x, UV.y);
    float3 TextureColor = float3(
        (float)((uint8*)&TextureSample)[0] / 255.0f,
        (float)((uint8*)&TextureSample)[1] / 255.0f,
        (float)((uint8*)&TextureSample)[2] / 255.0f);
    //float3 TextureColor = float3(0.5, 0.5, 0.5);
#if 1 
    float3 DirectionalLightColor = float3(1, 1, 1) * 1.5f;
    float3 AmbientLightColor = float3(0.658, 0.6431, 0.549) * 0.5;

    float NdotL = clamp01(dot(Normal, LightDir));
    float3 DirectionalLight = (DirectionalLightColor * NdotL);
    float3 Light = (AmbientLightColor + DirectionalLight);

    TextureColor = clamp01(TextureColor * Light);
#else
    float3 ReflectionUV = RayCubemap(reflect(ViewDirection, Normal));
    uint32 ReflectionSample = ImageSample(gameState->SkyFaces[(int)(ReflectionUV.z)], ReflectionUV.x, ReflectionUV.y);
    float3 ReflectionColor = float3(
        (float)((uint8*)&ReflectionSample)[0] / 255.0f,
        (float)((uint8*)&ReflectionSample)[1] / 255.0f,
        (float)((uint8*)&ReflectionSample)[2] / 255.0f);

    float3 DirectionalLightColor = float3(1, 1, 1) * 1.5f;
    float3 AmbientLightColor = float3(0.658, 0.6431, 0.549) * 1;
    float3 SpecularColor = float3(1, 1, 1);

    float CameraDot = (1 - -dot(ViewDirection, Normal));
    CameraDot = CameraDot * CameraDot;
    CameraDot = clamp01(CameraDot + 0.15f);

    float NdotL = clamp01(dot(Normal, LightDir));
    float3 DirectionalLight = (DirectionalLightColor * NdotL);
    float3 Light = (AmbientLightColor + DirectionalLight);

    float3 SpecularReflection = clamp01(SpecularColor * pow(clamp01(dot(reflect(ViewDirection, Normal), LightDir)), 200));

    TextureColor = clamp01(TextureColor * Light);
    TextureColor = clamp01(TextureColor + SpecularReflection + ReflectionColor * CameraDot);


#endif
    ((uint8*)&TextureSample)[0] = TextureColor.x * 255;
    ((uint8*)&TextureSample)[1] = TextureColor.y * 255;
    ((uint8*)&TextureSample)[2] = TextureColor.z * 255;

    return TextureSample;
}


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

    //DrawCircle(VideoBuffer, rx + dx * ClosestDistance,     ry + dy * ClosestDistance, 4);
    //DrawCircle(VideoBuffer, rx + dx * FurthestDistance,     ry + dy * FurthestDistance, 3);

    if (minx > maxy || miny > maxx)
    {
        //DrawCircle(VideoBuffer, 0, 0, 100);
        return -1;
    }
    return ClosestDistance;
}

//int2 GetThreadRegion(float Regions, float Region, int Height, int MinY, int MaxY)
//{
//    if (Region >= 0)
//    {
//        float Start = (Region / (Regions)) * Height;
//        float End = ((Region + 1) / (Regions)) * Height;
//
//        if (Start > MinY)
//            MinY = Start;
//
//        if (End < MaxY)
//            MaxY = End;
//    }
//    return int2(MinY, MaxY);
//}
int2 GetThreadRegion(GameState* gameState, float Regions, float Region, int Height, int MinY, int MaxY)
{
    if (Region >= 0)
    {
        float Start = gameState->RenderThreadStart[(int)Region] * Height;
        float End = gameState->RenderThreadEnd[(int)Region] * Height;
        //float Start = (Region / (Regions)) * Height;
        //float End = ((Region + 1) / (Regions)) * Height;

        if (Start > MinY)
            MinY = Start;

        if (End < MaxY)
            MaxY = End;
    }
    return int2(MinY, MaxY);
}



void DrawSkybox(GameState* gameState, game_offscreen_buffer* VideoBuffer, transform Camera, int Region = -1, int Regions = 9)
{
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

#if 0

    for (int yi = 0; yi < VideoBuffer->Height; yi++)
    {
        for (int xi = 0; xi < VideoBuffer->Width; xi++)
        {
            uint32* Pixel = &((uint32*)VideoBuffer->Memory)[(((yi)*VideoBuffer->Pitch) / 4 + (xi))];
            Dir += RightStep;
            float3 WorldDir = (Dir); // skip normalizing here, looks the same?

            float3 ReflectionUV = RayCubemap(WorldDir);

            Image* image = gameState->SkyFaces[(int)(ReflectionUV.z)];
            uint32 PixelX = ReflectionUV.x * image->width;
            uint32 PixelY = ReflectionUV.y * image->height;
            uint32 ReflectionSample = image->ImageData[(PixelY)*image->width + (PixelX)];

            *Pixel = ReflectionSample;
            ((uint8*)Pixel)[3] = 255;
        }
        Dir -= RightOffset;
        Dir += DownStep;
    }
#else

    int MinX = 0;
    int MinY = 0;
    int MaxX = VideoBuffer->Width;
    int MaxY = VideoBuffer->Height;
    float RegionWidth = VideoBuffer->Width;
    //if (Region >= 0)
    //{
    //    float Regions = 9;
    //    float Start = (Region / (Regions)) * VideoBuffer->Width;
    //    float End = ((Region + 1) / (Regions)) * VideoBuffer->Width;
    //    RegionWidth = End - Start;
    //
    //    if (Start > MinX)
    //        MinX = Start;
    //
    //    if (End < MaxX)
    //        MaxX = End;
    //}

    //if (Region >= 0)
    //{
    //    float Regions = 9;
    //    float Start = (Region / (Regions)) * VideoBuffer->Height;
    //    float End = ((Region + 1) / (Regions)) * VideoBuffer->Height;
    //    RegionWidth = End - Start;
    //
    //    if (Start > MinY)
    //        MinY = Start;
    //
    //    if (End < MaxY)
    //        MaxY = End;
    //}

    int2 ChangedSize = GetThreadRegion(gameState, Regions, Region, VideoBuffer->Height, MinY, MaxY);
    MinY = ChangedSize.x;
    MaxY = ChangedSize.y;
    //float3 Dir = TopLeft;// +(RightStep * MinX) + (DownStep * MinY);
    float3 Dir = TopLeft + (RightStep * MinX) + (DownStep * MinY);

    //float3 LastReflectionUV = {};
    //float3 CurrentReflectionUV = {};
    //float SizeA = 32;
    //float SizeB = 8;
    //uint8 x = 0;
    //float xvalue = 0;
    //float yvalue = 0;
    //uint8 y = 0;
    for (int yi = MinY; yi < MaxY; yi++)
    {
        for (int xi = MinX; xi < MaxX; xi++)
        {
            float x = (float)xi / (float)VideoBuffer->Width;
            float y = (float)yi / (float)VideoBuffer->Height;
            float3 dir1 = lerp(TopLeft, TopRight, x);
            float3 dir2 = lerp(BottomLeft, BottomRight, x);
            float3 Dirr = lerp(dir1, dir2, y);

            uint32* Pixel = &((uint32*)VideoBuffer->Memory)[(((yi)*VideoBuffer->Pitch) / 4 + (xi))];
            Dir += RightStep;

            //if (x == 0)
            //{
            //    LastReflectionUV = RayCubemap(Dir);
            //    CurrentReflectionUV = RayCubemap(Dir + RightStep * SizeA);
            //
            //    xvalue = 0;
            //}
            //float3 ReflectionUV = lerp(LastReflectionUV, CurrentReflectionUV, xvalue);
            //
            //if(LastReflectionUV.z != CurrentReflectionUV.z)
            //    ReflectionUV = RayCubemap(Dir);
            //else
            //    ReflectionUV.z = LastReflectionUV.z;
            float3 ReflectionUV = RayCubemap(Dirr);
            Image* image = gameState->SkyFaces[(int)(ReflectionUV.z)];
            uint32 PixelX = ReflectionUV.x * image->width;
            uint32 PixelY = ReflectionUV.y * image->height;
            uint32 ReflectionSample = image->ImageData[(PixelY)*image->width + (PixelX)];
            *Pixel = ReflectionSample;
            ((uint8*)Pixel)[3] = 255;

            //x += SizeB;
            //xvalue += 1.0f / SizeA;
        }

        //if (y == 0)
        //{
        //    yvalue = 0;
        //}
        //yvalue += 1.0f / SizeA;
        //
        //y += SizeB;
        //x = 0;
        Dir -= RightStep * RegionWidth;
        Dir += DownStep;
    }
#endif

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
    distance01 = distance01 * distance01 * distance01 * distance01;
    distance01 = 1 - distance01;
    return (uint8)(distance01 * 255);
}

void DrawLine3D(game_offscreen_buffer* VideoBuffer, transform Camera, float3 Start, float3 End)
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
        return;

    // outside edges
    if ((Start2D.z < 0 || End2D.z < 0) || 
        (Start2D.x < 0 || Start2D.y < 0 || Start2D.y > VideoBuffer->Height || Start2D.x > VideoBuffer->Width) &&
        (End2D.y > VideoBuffer->Height || End2D.x < 0 || End2D.y < 0 || End2D.x > VideoBuffer->Width))
        return;

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
        if (NewDepth > OldDepth)
            continue;
        *GetPixel(VideoBuffer, l.x, l.y) = 0xFFFFFFFF;
        ((uint8*)Pixel)[3] = NewDepth;
    }

    //DrawLine(VideoBuffer, float2(Start2D.x, Start2D.y), float2(End2D.x, End2D.y));
}

void DrawCircle3D(game_offscreen_buffer* VideoBuffer, transform Camera, float3 Center, float3 Angle, float Radius, float sweep = 1.0f)
{
    float3 Up = Angle;
    float3 Right = normalize(cross(Angle, float3(0,0.00001, 1.03)));
    float3 Forward = normalize(cross(Right, Up));
    Right = normalize(cross(Forward, Up));

    float steps = 100;
    float stepsize = (1.0f / steps) * (3.14152128f * 2.0f);
    //float3 lastP = float3(sin(-stepsize) * Radius, cos(-stepsize) * Radius, 0);
    float3 lastP = Right * sin(-stepsize) * Radius + Forward * cos(-stepsize) * Radius;
    for (float i = 0; i < (3.14152128f * 2.0f) * sweep; i += stepsize) // eh
    {
        float3 p = Right * sin(i) * Radius + Forward * cos(i) * Radius;

        DrawLine3D(VideoBuffer, Camera, p + Center, lastP + Center);
        lastP = p;
    }

}

void DrawObjectCheap(GameState* gameState, game_offscreen_buffer* VideoBuffer, transform Camera, transform Box, bool Sphere = false)
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

    float3 BoxScreenPos1 = (BoxPos + ((BoxRight  * BoxScale.x) + (-BoxForward * BoxScale.y) + (-BoxUp * BoxScale.z)) * 0.5);
    float3 BoxScreenPos2 = (BoxPos + ((-BoxRight * BoxScale.x) + (-BoxForward * BoxScale.y) + (-BoxUp * BoxScale.z)) * 0.5);
    float3 BoxScreenPos3 = (BoxPos + ((-BoxRight * BoxScale.x) + (BoxForward  * BoxScale.y) + (-BoxUp * BoxScale.z)) * 0.5);
    float3 BoxScreenPos4 = (BoxPos + ((BoxRight  * BoxScale.x) + (BoxForward  * BoxScale.y) + (-BoxUp * BoxScale.z)) * 0.5);

    float3 BoxScreenPos5 = (BoxPos + ((BoxRight  * BoxScale.x) + (-BoxForward * BoxScale.y) + (BoxUp * BoxScale.z)) * 0.5);
    float3 BoxScreenPos6 = (BoxPos + ((-BoxRight * BoxScale.x) + (-BoxForward * BoxScale.y) + (BoxUp * BoxScale.z)) * 0.5);
    float3 BoxScreenPos7 = (BoxPos + ((-BoxRight * BoxScale.x) + (BoxForward  * BoxScale.y) + (BoxUp * BoxScale.z)) * 0.5);
    float3 BoxScreenPos8 = (BoxPos + ((BoxRight  * BoxScale.x) + (BoxForward  * BoxScale.y) + (BoxUp * BoxScale.z)) * 0.5);

    if (Sphere)
    {
        DrawCircle3D(VideoBuffer, Camera, BoxPos, BoxForward,    max(BoxScale));
        DrawCircle3D(VideoBuffer, Camera, BoxPos, BoxUp,         max(BoxScale));
        DrawCircle3D(VideoBuffer, Camera, BoxPos, BoxRight,      max(BoxScale));

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

void DrawObject(GameState* gameState, game_offscreen_buffer* VideoBuffer, transform Camera, transform Box, bool Sphere = false, int Region = -1, int Regions = 9)
{
    int sizeX = VideoBuffer->Width;
    int sizeY = VideoBuffer->Height;
    float AspectRatio = 0.5625f;
    float AspectRatio2 = 1.777777777f;

    float3 BoxPos = Box.position;
    float3 Forward = Camera.forward;// CameraForward;
    float3 Right = Camera.right;//cross(CameraForward, CameraUp);
    float3 Up = Camera.up * AspectRatio;
    transform CameraTransform = Camera;
    CameraTransform.up = Up;

    float3 CameraPos = Camera.position;
    float3 LocalPos = CameraPos - BoxPos;
    float3 BoxRight = Box.right;// cross(Box->forward, Box->up);
    float3 BoxForward = Box.forward;
    float3 BoxUp = Box.up;
    float3 BoxScale = Box.scale;

    float SphereScale = 0;
    if (Sphere)
    {
        SphereScale = max(BoxScale);// *0.5;
        BoxScale = float3(1, 1, 1);
    }

    LocalPos = float3(dot(LocalPos, BoxRight / BoxScale.x), dot(LocalPos, BoxForward / BoxScale.y), dot(LocalPos, BoxUp / BoxScale.z));

    float depth = 1;
    int MinX = 0;
    int MaxX = 0;
    int MinY = 0;
    int MaxY = 0;
    int MinZ = 0;
    int MaxZ = 0;
    int X = 0;
    int Y = 0;
    int width = 0;
    int height = 0;
    if (Sphere)
    {
        float3 CenterPosScreenspace = ToScreenPos(VideoBuffer, BoxPos, CameraTransform);
        CenterPosScreenspace = (CenterPosScreenspace - float3(VideoBuffer->Width / 2, VideoBuffer->Height / 2, 0));
        CenterPosScreenspace.x *= 0.1f;
        CenterPosScreenspace.y *= 0.1f;

        float3 BoxScreenPos1 = ToScreenPos(VideoBuffer, BoxPos + normalize(Right) * 1.3 *    SphereScale, CameraTransform);
        float3 BoxScreenPos2 = ToScreenPos(VideoBuffer, BoxPos - normalize(Right) * 1.3 *    SphereScale, CameraTransform);
        float3 BoxScreenPos3 = ToScreenPos(VideoBuffer, BoxPos + normalize(Up) * 1.3 *       SphereScale, CameraTransform);
        float3 BoxScreenPos4 = ToScreenPos(VideoBuffer, BoxPos - normalize(Up) * 1.3 *       SphereScale, CameraTransform);
        float3 BoxScreenPos5 = ToScreenPos(VideoBuffer, BoxPos + normalize(Forward) * 1.3 *  SphereScale, CameraTransform);
        float3 BoxScreenPos6 = ToScreenPos(VideoBuffer, BoxPos - normalize(Forward) * 1.3 *  SphereScale, CameraTransform);

        MinX = BoxScreenPos2.x + CenterPosScreenspace.x / CenterPosScreenspace.z;
        MaxX = BoxScreenPos1.x + CenterPosScreenspace.x / CenterPosScreenspace.z;
        MinY = BoxScreenPos3.y + CenterPosScreenspace.y / CenterPosScreenspace.z;
        MaxY = BoxScreenPos4.y + CenterPosScreenspace.y / CenterPosScreenspace.z;
        MinZ = BoxScreenPos3.z;
        MaxZ = BoxScreenPos4.z;
        depth = MaxZ;
    }
    else
    {
        float3 BoxScreenPos1 = ToScreenPos(VideoBuffer, BoxPos + ((-BoxRight * BoxScale.x) + (-BoxForward * BoxScale.y) + (-BoxUp * BoxScale.z)) * 0.5, CameraTransform);
        float3 BoxScreenPos2 = ToScreenPos(VideoBuffer, BoxPos + ((BoxRight  * BoxScale.x) + (-BoxForward * BoxScale.y) + (-BoxUp * BoxScale.z)) * 0.5, CameraTransform);
        float3 BoxScreenPos3 = ToScreenPos(VideoBuffer, BoxPos + ((-BoxRight * BoxScale.x) + (BoxForward  * BoxScale.y) + (-BoxUp * BoxScale.z)) * 0.5, CameraTransform);
        float3 BoxScreenPos4 = ToScreenPos(VideoBuffer, BoxPos + ((BoxRight  * BoxScale.x) + (BoxForward  * BoxScale.y) + (-BoxUp * BoxScale.z)) * 0.5, CameraTransform);
        float3 BoxScreenPos5 = ToScreenPos(VideoBuffer, BoxPos + ((-BoxRight * BoxScale.x) + (-BoxForward * BoxScale.y) + (BoxUp  * BoxScale.z)) * 0.5, CameraTransform);
        float3 BoxScreenPos6 = ToScreenPos(VideoBuffer, BoxPos + ((BoxRight  * BoxScale.x) + (-BoxForward * BoxScale.y) + (BoxUp  * BoxScale.z)) * 0.5, CameraTransform);
        float3 BoxScreenPos7 = ToScreenPos(VideoBuffer, BoxPos + ((-BoxRight * BoxScale.x) + (BoxForward  * BoxScale.y) + (BoxUp  * BoxScale.z)) * 0.5, CameraTransform);
        float3 BoxScreenPos8 = ToScreenPos(VideoBuffer, BoxPos + ((BoxRight  * BoxScale.x) + (BoxForward  * BoxScale.y) + (BoxUp  * BoxScale.z)) * 0.5, CameraTransform);

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

        MinZ = min(min(min(min(min(min(min(BoxScreenPos1.z, BoxScreenPos2.z), BoxScreenPos3.z), BoxScreenPos4.z), BoxScreenPos5.z), BoxScreenPos6.z), BoxScreenPos7.z), BoxScreenPos8.z);
        MaxZ = max(max(max(max(max(max(max(BoxScreenPos1.z, BoxScreenPos2.z), BoxScreenPos3.z), BoxScreenPos4.z), BoxScreenPos5.z), BoxScreenPos6.z), BoxScreenPos7.z), BoxScreenPos8.z);
        depth = MaxZ;
    }
    if (depth < 0)
        return;
    X = (MinX + MaxX) / 2;
    Y = (MinY + MaxY) / 2;
    width = MaxX - MinX;
    height = MaxY - MinY;


    int2 ChangedSize = GetThreadRegion(gameState, Regions, Region, VideoBuffer->Height, MinY, MaxY);
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
    float3 TopLeft = ((Forward + -Right) + Up);
    float3 TopRight = ((Forward + Right) + Up);
    float3 BottomLeft = ((Forward + -Right) + -Up);
    float3 BottomRight = ((Forward + Right) + -Up);
    float3 RightOffset = (TopRight - TopLeft);
    float3 DownOffset = (BottomLeft - TopLeft);
    float3 RightStep = RightOffset / (VideoBuffer->Width);
    float3 DownStep = DownOffset / (VideoBuffer->Height);
    float3 Dir = TopLeft + (RightStep * MinX) + (DownStep * MinY);

    for (int yi = MinY; yi < MaxY; yi++)
    {
        for (int xi = MinX; xi < MaxX; xi++)
        {
            uint32* Pixel = &((uint32*)VideoBuffer->Memory)[(((yi)*VideoBuffer->Pitch) / 4 + (xi))];
            Dir = Dir + RightStep;
            float3 WorldDir = normalize(Dir);

            // Transform CameraPos and d to the space of the box
            float3 LocalDir = float3(dot(WorldDir, BoxRight / BoxScale.x), dot(WorldDir, BoxForward / BoxScale.y), dot(WorldDir, BoxUp / BoxScale.z));

            float3 Normal = float3(0, 0, 0);
            float dist = 0;
            if (Sphere)
                dist = RaySphereIntersect(LocalPos, LocalDir, SphereScale);
            else
                dist = RayBoxIntersect(LocalPos, LocalDir, &Normal);

            if (dist < 0)
                continue;

            uint8 OldDepth = ((uint8*)Pixel)[3];
            

            uint8 NewDepth = DistanceToDepthBufferValue(dist);
            if (NewDepth == 255)
                continue;

            if (NewDepth > OldDepth)
                continue;

            float3 p = float3(LocalPos.x + LocalDir.x * dist,
                LocalPos.y + LocalDir.y * dist,
                LocalPos.z + LocalDir.z * dist);

            float3 UV = float3(0, 0, 0);
            if (Sphere)
            {
                Normal = normalize(p);
                UV = RaySphereUV(Normal);
                //UV = RayBoxUV(p, Normal);
            }
            else
            {
                UV = RayBoxUV(p, Normal);
            }

            // Transform the normal from local space to world space.
            float3 BoxRightTransposed = float3(BoxRight.x, BoxForward.x, BoxUp.x);
            float3 BoxForwardTransposed = float3(BoxRight.y, BoxForward.y, BoxUp.y);
            float3 BoxUpTransposed = float3(BoxRight.z, BoxForward.z, BoxUp.z);

            Normal = float3(dot(Normal, BoxRightTransposed), dot(Normal, BoxForwardTransposed), dot(Normal, BoxUpTransposed));

            uint32 NewSample = Shader(gameState, float3(UV.x, UV.y, 0), dist, Normal, WorldDir, Sphere);

            // Set all 4 bytes at once
            *Pixel = NewSample;
            ((uint8*)Pixel)[3] = NewDepth;
        }
        Dir = Dir - (RightStep * (MaxX - MinX));
        Dir = Dir + DownStep;
    }

    //if (depth > 0 && !gameState->GameViewEnabled)
    //{
    //    DrawBox(VideoBuffer, float2(X, Y), float2(width, height));
    //    DrawCross(VideoBuffer, X, Y);
    //}
}

bool BoxContains(float2 Pos, float2 Size, float2 TestPos)
{
    float2 NormalizedPos = (TestPos - Pos) / Size;
    if (NormalizedPos.x > 0 && NormalizedPos.x < 1 &&
        NormalizedPos.y > 0 && NormalizedPos.y < 1)
    {
        return true;
    }
    return false;
}

bool Button(GameState* gameState, game_input* Input, game_offscreen_buffer* VideoBuffer, const char* Text, float2 Pos)
{
    float2 Size = float2(StringLength(Text) * 6 + 15, 25);
    float2 MousePos = float2(Input->MouseX, Input->MouseY);
    if (BoxContains(Pos, Size, MousePos))
    {
        if (Input->MouseLeft)
        {
            DrawRectangle(VideoBuffer, Pos, Size, float3(0.8, 0.8, 0.8));
        }
        else
        {
            DrawRectangle(VideoBuffer, Pos, Size, float3(0.6, 0.6, 0.6));
        }
        if (Input->MouseLeftUp)
        {
            return true;
        }
    }
    else
    {
        DrawRectangle(VideoBuffer, Pos, Size, float3(0.5, 0.5, 0.5));
    }
    
    font_draw(gameState->fontSpritesheet, VideoBuffer, Pos.x+5, Pos.y+5, Text);
    return false;
}

bool Toggle(GameState* gameState, game_input* Input, game_offscreen_buffer* VideoBuffer, bool Current, const char* Text, float2 Pos)
{
    float2 Size = float2(15, 15);
    float2 MousePos = float2(Input->MouseX, Input->MouseY);
    if (BoxContains(Pos, Size, MousePos))
    {
        if (Input->MouseLeft)
        {
            DrawRectangle(VideoBuffer, Pos, Size, float3(0.8, 0.8, 0.8));
        }
        else
        {
            DrawRectangle(VideoBuffer, Pos, Size, float3(0.6, 0.6, 0.6));
        }
        if (Input->MouseLeftUp)
        {
            return !Current;
        }
    }
    else
    {
        DrawRectangle(VideoBuffer, Pos, Size, float3(0.5, 0.5, 0.5));
    }
    if (Current)
    {
        DrawRectangle(VideoBuffer, Pos+float2(4, 4), Size-float2(8, 8), float3(1.0, 1.0, 1.0));
    }
    font_draw(gameState->fontSpritesheet, VideoBuffer, Pos.x + 17, Pos.y + 0, Text);
    return Current;
}