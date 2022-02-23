
#include "quote.h"
#include "string.cpp"


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
    return image->ImageData[(PixelY) * image->width + (PixelX)];
}

uint32 ImageSampleBiliniar(Image* image, float x, float y)
{
    float PixelSizeX = 1.0f / image->width;
    float PixelSizeY = 1.0f / image->height;
    x -= PixelSizeX * 0.5;
    y -= PixelSizeY * 0.5;
    
    uint8 image_current_x_int = FloorToInt(x * image->width * 256);
    uint8 image_current_y_int = FloorToInt(y * image->height * 256);

    uint32 Centerpixel      = ImageSample(image, x + 0,          y + 0);
    uint32 Rightpixel       = ImageSample(image, x + PixelSizeX, y + 0);
    uint32 Downpixel        = ImageSample(image, x + 0,          y + PixelSizeY);
    uint32 DownRightpixel   = ImageSample(image, x + PixelSizeX, y + PixelSizeY);

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
            *Pixel++ = image->ImageData[(y- IntY) * image->width + (x- IntX)];
        }
        Row += Buffer->Pitch;
    }
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

void DrawRectangle(game_offscreen_buffer *Buffer, 
                    float X, float Y,
                    float Width, float Height, float R, float G, float B, float A = 1)
{
    int MinX = FloorToInt(X);
    int MinY = FloorToInt(Y);
    int MaxX = FloorToInt(X+ Width);
    int MaxY = FloorToInt(Y+ Height);

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
    uint32 Color = MakeBufferColor(Buffer, R, G, B);
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
                vr = vr*vr;//pow(vr, 2.2f);
                vg = vg*vg;//pow(vg, 2.2f);
                vb = vb*vb;//pow(vb, 2.2f);
                va = va*va;//pow(va, 2.2f);

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
                ((uint8*)Pixel)[3] = va * 256.0f;

                Pixel++;
            }
            Row += Buffer->Pitch;
        }
    }
}

void DrawRectangleSoft(game_offscreen_buffer *Buffer,
    float X, float Y,
    float Width, float Height, float R, float G, float B)
{
    int MinX = FloorToInt(X);
    int MinY = FloorToInt(Y);
    int MaxX = CeilToInt(X+ Width);
    int MaxY = CeilToInt(Y+ Height);


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

void DrawBox(game_offscreen_buffer *Buffer, int X, int Y, int width, int height, int R, int G, int B)
{
    if (X < 0)
    {
        width -= -X;
        X = 0;
    }
    if (Y < 0)
    {
        height -= -Y;
        Y = 0;
    }
    if ((X + width) > Buffer->Width)
    {
        width = X - Buffer->Width;
    }
    if ((X + height) > Buffer->Height)
    {
        height = Y - Buffer->Height;
    }


    int pitch = Buffer->Width;
    uint32* CurrentPixel = (uint32*)Buffer->Memory;
    CurrentPixel += pitch * Y;
    for (int x = 0; x < height; x++)
    {
        uint32* temp = CurrentPixel;
        CurrentPixel += X;
        for (int y = 0; y < width; y++)
        {
            *CurrentPixel = R << 16 | G << 8 | B << 0;
            CurrentPixel++;
        }
        CurrentPixel = temp;
        CurrentPixel += pitch; // skip to next row
    }
}

internal void GameOutputSound(GameState* GameState, game_sound_output_buffer* SoundBuffer, float ToneHz, float ToneVolume)
{
    int16* SampleOut = SoundBuffer->Samples;
    float TonePeriod = (int16)((float)SoundBuffer->SamplesPerSecond / ToneHz);
    int16 SampleValue = 0;

    for (int i = 0; i < SoundBuffer->SampleCount; i++)
    {
        GameState->tSine += 2.0f * Pi * 1.0f / (TonePeriod * 4.0f);
        if(GameState->tSine > 3.14152128f*2.0f)
        {
            GameState->tSine = 0;
        }


        GameState->tSine = 0;
        
        float SineValue = 0;// sinf(GameState->tSine);

        SampleValue = (int16)(SineValue * ToneVolume);

        *SampleOut++ = SampleValue;
        *SampleOut++ = SampleValue;
    }
}

internal void RenderWierdGradient(game_offscreen_buffer* Buffer, int offsetX, int offsetY, float blue)
{
    uint32* CurrentPixel = (uint32*)Buffer->Memory;
    for (int y = 0; y < Buffer->Height; y++)
    {
        for (int x = 0; x < Buffer->Width; x++)
        {
            float u = (float)(x + offsetX) / (float)Buffer->Width;
            float v = 1.0f - ((float)(y + offsetY) / (float)Buffer->Height);
            float Red = u;
            float Green = v;
            float Blue = 0.2f;// (float)abs((double)blue);

            uint8 R = (uint8)(Red * 255);
            uint8 G = (uint8)(Green * 255);
            uint8 B = (uint8)(Blue * 255);
            *CurrentPixel = R << 16 | G << 8 | B;

            CurrentPixel += 1; // go forward by 4 bytes
        }
    }
}

internal void RenderPlayer(game_offscreen_buffer* Buffer, int PlayerX, int PlayerY, uint32 Color)
{
    int Top = PlayerY;
    int Bottom = PlayerY+10;

    for (int X = PlayerX; X < PlayerX+10; X++)
    {
        uint8* Pixel = (uint8*)Buffer->Memory + X * Buffer->BytesPerPixel + Top * Buffer->Pitch;
        for (int i = Top; i < Bottom; i++)
        {
            uint8* end = (uint8*)Buffer->Memory + (Buffer->Height * Buffer->Width * Buffer->BytesPerPixel);
            if (Pixel < Buffer->Memory || (Pixel > end))
                continue;
            *(uint32*)Pixel = Color;
            Pixel += Buffer->Pitch;
        }
    }
}

internal uint32* GetPixel(game_offscreen_buffer *Buffer, int y, int x)
{
    x = (int)min(max((double)x, 0.0), (double)(Buffer->Height - 1));
    y = (int)min(max((double)y, 0.0), (double)(Buffer->Width - 1));

    uint32 offset = (x * Buffer->Width + y);
    return (uint32*)Buffer->Memory + offset;
}

inline float distance(float StartX, float StartY, float EndX, float EndY)
{
    return sqrt((StartX - EndX) * (StartX - EndX) + (StartY - EndY) * (StartY - EndY));
}

void DrawPoint(game_offscreen_buffer* Buffer, float x, float y)
{
    *GetPixel(Buffer, x, y) = 0xFFFFFFFF;
    *GetPixel(Buffer, x + 1, y) = 0xFFFFFFFF;
    *GetPixel(Buffer, x, y + 1) = 0xFFFFFFFF;
    *GetPixel(Buffer, x - 1, y) = 0xFFFFFFFF;
    *GetPixel(Buffer, x, y - 1) = 0xFFFFFFFF;
}
void DrawCross(game_offscreen_buffer *Buffer, int x, int y)
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

void DrawCircle(game_offscreen_buffer* Buffer, float x, float y, float radius)
{
    for (float i = 0; i < (int)(3.14152128f * 2.3f); i += (1.0f / radius))
    {
        float ax = sinf(i) * radius;
        float ay = cosf(i) * radius;
        *GetPixel(Buffer, x+ax, y+ay) = 0xFFFFFFFF;
    }
    DrawPoint(Buffer, x, y);
}

void DrawLine(game_offscreen_buffer* Buffer, float StartX, float StartY, float EndX, float EndY)
{
    float dist = distance(StartX, StartY, EndX, EndY);
    for (float i = 0; i < 1.0f; i += (1.0f / dist))
    {
        float x = lerp(StartX, EndX, i);
        float y = lerp(StartY, EndY, i);
        *GetPixel(Buffer, x, y) = 0xFFFFFFFF;
    }
}
void DrawBox(game_offscreen_buffer* Buffer, float X, float Y, float Width, float Height)
{
    DrawLine(Buffer, X - Width * 0.5f, Y - Height * 0.5f, X + Width * 0.5f, Y - Height * 0.5f);
    DrawLine(Buffer, X - Width * 0.5f, Y + Height * 0.5f, X + Width * 0.5f, Y + Height * 0.5f);
    DrawLine(Buffer, X - Width * 0.5f, Y - Height * 0.5f, X - Width * 0.5f, Y + Height * 0.5f);
    DrawLine(Buffer, X + Width * 0.5f, Y - Height * 0.5f, X + Width * 0.5f, Y + Height * 0.5f);
}
struct float2
{
    float x;
    float y;
};
float2 ctor_float2(float x, float y)
{
    float2 result;
    result.x = x;
    result.y = y;
    return result;
}

float2 MoveTowards(float2 start, float2 target, float t)
{
    float dist = distance(start.x, start.y, target.x, target.y);
    t = min(dist, t);
    return ctor_float2(start.x + ((target.x - start.x) / dist) * t, start.y + ((target.y - start.y) / dist) * t);
}


void DrawThing(Image* fontSpritesheet, game_offscreen_buffer* VideoBuffer, int* y, const char* text, char* value)
{
    char Space[100] = {};
    Append(Append(Space, text), value);
    font_draw(fontSpritesheet, VideoBuffer, 0, *y, Space);
    *y += 12;
}
void DrawThing(Image* fontSpritesheet, game_offscreen_buffer* VideoBuffer, int* y, const char* text, int value)
{
    char Space[100] = {};
    Append(Space, value);
    DrawThing(fontSpritesheet, VideoBuffer, y, text, Space);

}
void DrawThing(Image* fontSpritesheet, game_offscreen_buffer* VideoBuffer, int* y, const char* text, float value)
{
    char Space[100] = {};
    Append(Space, value);
    DrawThing(fontSpritesheet, VideoBuffer, y, text, Space);
}

char getNthBit(char c, char n)
{
    unsigned char tmp = 1 << n;
    return (c & tmp) >> n;
}

void DebugDrawInput(Image* fontSpritesheet, game_offscreen_buffer* VideoBuffer, game_input* Input)
{
    int ui_height = 0;

    ui_height += 12;
    ui_height += 12;
    ui_height += 12;
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "MouseX: ", Input->MouseX);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "MouseY: ", Input->MouseY);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "MouseZ: ", Input->MouseZ);
    ui_height += 12;

    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "MouseLeft: ", Input->MouseLeft);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "MouseRight: ", Input->MouseRight);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "MouseMiddle: ", Input->MouseMiddle);
    ui_height += 12;

    char Letters[100] = {};
    Append(Letters, "Letters: ");
    for (int i = 0; i < 26; i++)
    {
        char name[2] = " ";
        if (Input->Letters[i].Held)
            name[0] = (char)(i + 65);
        else
            name[0] = '-';

        Append(Letters, name);
    }
    font_draw(fontSpritesheet, VideoBuffer, 0, ui_height, Letters);
    ui_height += 12;

    char Numbers[100] = {};
    Append(Numbers, "Numbers: ");
    for (int i = 0; i < 10; i++)
    {
        char name[2] = " ";
        if (Input->Numbers[i].Held)
            name[0] = (char)(i + 48);
        else
            name[0] = '-';

        Append(Numbers, name);
    }
    font_draw(fontSpritesheet, VideoBuffer, 0, ui_height, Numbers);
    ui_height += 12;

    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Space: ", Input->Space);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Backspace: ", Input->Backspace);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Enter: ", Input->Enter);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Escape: ", Input->Escape);

    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Left: ", Input->Left);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Right: ", Input->Right);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Up: ", Input->Up);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Down: ", Input->Down);

    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Shift: ", Input->Shift);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Ctrl: ", Input->Ctrl);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Alt: ", Input->Alt);

    ui_height += 12;
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "A: ", Input->Controllers[0].A);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "B: ", Input->Controllers[0].B);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "X: ", Input->Controllers[0].X);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Y: ", Input->Controllers[0].Y);

    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Left: ", Input->Controllers[0].Left);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Right: ", Input->Controllers[0].Right);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Up: ", Input->Controllers[0].Up);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Down: ", Input->Controllers[0].Down);

    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "LeftStickX: ", Input->Controllers[0].LeftStickX);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "LeftStickY: ", Input->Controllers[0].LeftStickY);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "RightStickX: ", Input->Controllers[0].RightStickX);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "RightStickY: ", Input->Controllers[0].RightStickY);

    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "LeftBumper: ", Input->Controllers[0].LeftBumper);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "RightBumper: ", Input->Controllers[0].RightBumper);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "LeftTrigger: ", Input->Controllers[0].LeftTrigger);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "RightTrigger: ", Input->Controllers[0].RightTrigger);

    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Start: ", Input->Controllers[0].Start);
    DrawThing(fontSpritesheet, VideoBuffer, &ui_height, "Back: ", Input->Controllers[0].Back);

    font_draw(fontSpritesheet, VideoBuffer, 140, 18, "input struct raw data:");

    int x_offset = 100;
    int y = VideoBuffer->Pitch * 1;
    for (int i = 0; i < sizeof(game_input); i++) // for every byte of the data
    {
        int i_wrapped = i % 10;
        if (i % 10 == 0)
            y += VideoBuffer->Pitch / 32;

        char byte = ((char*)Input)[i];

        int PixelIndex = (i_wrapped - x_offset) + y;

        for (int j = 0; j < 8; j++)
        {
            char bit = getNthBit(byte, j) != 0 ? 255 : 0;

            int bit_index = (((PixelIndex) * 8) + j) * 4;
            ((char*)VideoBuffer->Memory)[bit_index + 0] = bit;
            ((char*)VideoBuffer->Memory)[bit_index + 1] = bit;
            ((char*)VideoBuffer->Memory)[bit_index + 2] = bit;
            ((char*)VideoBuffer->Memory)[bit_index + 3] = 255;
        }
    }
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
float RayBoxIntersect(float rx, float ry, float rz, 
    float dx, float dy, float dz, 
    float x, float y, float z, 
    float width, float height, float length,
    float* NormalX, float* NormalY, float* NormalZ)
{
    float hwidth = width / 2;
    float hheight = height / 2;
    float hlength = length / 2;
    float px = rx - x;
    float py = ry - y;
    float pz = rz - z;
    float XpDistance    = ( hwidth  - px) / dx;
    float XnDistance    = (-hwidth  - px) / dx;
    float YpDistance    = ( hheight - py) / dy;
    float YnDistance    = (-hheight - py) / dy;
    float ZpDistance    = ( hlength - pz) / dz;
    float ZnDistance    = (-hlength - pz) / dz;

    //float LeftHitX = px + dx * LeftDistance;
    //float LeftHitY = py + dy * LeftDistance;
    //float BottomHitX = px + dx * BottomDistance;
    //float BottomHitY = py + dy * BottomDistance;
    //float RightHitX = px + dx * RightDistance;
    //float RightHitY = py + dy * RightDistance;
    //float TopHitX = px + dx * TopDistance;
    //float TopHitY = py + dy * TopDistance;
    
    //*NormalX = x;
    //*NormalY = y;
    //*NormalZ = z;

    float ClosestDistance;
    float FurthestDistance;
    if (dx < 0)
    {
        if (dy < 0)
        {
            if (dz < 0)
            {
                ClosestDistance = max(XpDistance, YpDistance, ZpDistance);
                FurthestDistance = min(XnDistance, YnDistance, ZnDistance);
            }
            else
            {
                ClosestDistance = max(XpDistance, YpDistance, ZnDistance);
                FurthestDistance = min(XnDistance, YnDistance, ZpDistance);
            }
        }
        else
        {
            if (dz < 0)
            {
                ClosestDistance = max(XpDistance, YnDistance, ZpDistance);
                FurthestDistance = min(XnDistance, YpDistance, ZnDistance);
            }
            else
            {
                ClosestDistance = max(XpDistance, YnDistance, ZnDistance);
                FurthestDistance = min(XnDistance, YpDistance, ZpDistance);
            }
        }
    }
    else
    {
        if (dy < 0)
        {
            if (dz < 0)
            {
                ClosestDistance = max(XnDistance, YpDistance, ZpDistance);
                FurthestDistance = min(XpDistance, YnDistance, ZnDistance);
            }
            else
            {
                ClosestDistance = max(XnDistance, YpDistance, ZnDistance);
                FurthestDistance = min(XpDistance, YnDistance, ZpDistance);
            }
        }
        else
        {
            if (dz < 0)
            {
                ClosestDistance = max(XnDistance, YnDistance, ZpDistance);
                FurthestDistance = min(XpDistance, YpDistance, ZnDistance);
            }
            else
            {
                ClosestDistance = max(XnDistance, YnDistance, ZnDistance);
                FurthestDistance = min(XpDistance, YpDistance, ZpDistance);
            }
        }
    }

    *NormalX = 0;
    *NormalY = 0;
    *NormalZ = 0;

    if (ClosestDistance < FurthestDistance)
    {
        if (ClosestDistance == XpDistance)
            *NormalX = 1;
        else if (ClosestDistance == XnDistance)
            *NormalX = -1;
        else if (ClosestDistance == YpDistance)
            *NormalY = 1;
        else if (ClosestDistance == YnDistance)
            *NormalY = -1;
        else if (ClosestDistance == ZpDistance)
            *NormalZ = 1;
        else if (ClosestDistance == ZnDistance)
            *NormalZ = -1;

        return ClosestDistance;
    }
    return -1;
}

float RayBoxIntersect(float rx, float ry, float dx, float dy, float x, float y, float width, float height)
{
    float hwidth = width / 2;
    float hheight = height / 2;
    float px = rx - x;
    float py = ry - y;
    float LeftDistance = (-hwidth - px) / dx;
    float BottomDistance = (-hheight - py) / dy;
    float RightDistance = (hwidth - px) / dx;
    float TopDistance = (hheight - py) / dy;

    float LeftHitX = px + dx * LeftDistance;
    float LeftHitY = py + dy * LeftDistance;
    float BottomHitX = px + dx * BottomDistance;
    float BottomHitY = py + dy * BottomDistance;
    float RightHitX = px + dx * RightDistance;
    float RightHitY = py + dy * RightDistance;
    float TopHitX = px + dx * TopDistance;
    float TopHitY = py + dy * TopDistance;

    float ClosestDistance;
    float FurthestDistance;
    if (dx < 0)
    {
        if (dy < 0)
        {
            ClosestDistance = max(RightDistance, TopDistance);
            FurthestDistance = min(LeftDistance, BottomDistance);
        }
        else
        {
            ClosestDistance = max(RightDistance, BottomDistance);
            FurthestDistance = min(LeftDistance, TopDistance);
        }
    }
    else
    {
        if (dy < 0)
        {
            ClosestDistance = max(LeftDistance, TopDistance);
            FurthestDistance = min(RightDistance, BottomDistance);
        }
        else
        {
            ClosestDistance = max(LeftDistance, BottomDistance);
            FurthestDistance = min(RightDistance, TopDistance);
        }
    }
    if (ClosestDistance < FurthestDistance)
    {
        return ClosestDistance;
    }
    return -1;
}

//(thread_context* Thread, game_memory* Memory, game_input* Input, game_offscreen_buffer* VideoBuffer, game_sound_output_buffer* SoundBuffer)
extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    Assert(sizeof(GameState) <= Memory->PermanentStorageSize);
    //Assert((&Input->Controllers[0].Terminator - &Input->Controllers[0].Buttons[0]) == (ArrayCount(Input->Controllers[0].Buttons) - 1));
    
    InitGlobalFunctions(Memory);
    
    GameState* gameState = (GameState*)Memory->PermanentStorage;

    if (Input->SpaceDown)
    {
        Memory->Initialized = false;
        for (int i = 0; i < Memory->PermanentStorageSize; i++)
        {
            ((char*)Memory->PermanentStorage)[i] = 0;
        }
        for (int i = 0; i < Memory->TransientStorageSize; i++)
        {
            ((char*)Memory->TransientStorage)[i] = 0;
        }
    }
    Image* Apothem;
    Image* MagSqueeze;
    Image* Wuff;
    if (!Memory->Initialized)
    {
        Memory->Initialized = true;
        gameState->GameTime = 0;
    
        InitializeArena(&gameState->worldArena,     Megabytes(1), (uint8*)Memory->PermanentStorage + sizeof(GameState));
        InitializeArena(&gameState->textureArena,   Megabytes(6), (uint8*)Memory->PermanentStorage + sizeof(GameState) + Megabytes(1));
    
        //gameState->fontSpritesheet = PushStruct(&gameState->textureArena, Spritesheet);
        //font_init(gameState->fontSpritesheet, Memory);
    
        gameState->fontSpritesheet = LoadImage(Memory, gameState, "font.tga", 192, 52);
        Apothem      = LoadImage(Memory, gameState, "TestImage.tga", 300, 420);
        //MagSqueeze   = LoadImage(Memory, gameState, "TestImage2.tga", 512, 682);
        Wuff         = LoadImage(Memory, gameState, "Wuff.tga", 256, 256);

        gameState->RayStartX = 100;
        gameState->RayStartY = 100;

        gameState->RayDirX = 100;
        gameState->RayDirY = 100;
    }
    gameState->GameTime += Input->DeltaTime;
    DrawRectangle(VideoBuffer, 0, 0, 960, 540, 0.0, 0.0, 0.5);
    

    //float sinvalue = abs(sin(gameState->GameTime));
    //DrawImageScaledSmooth(VideoBuffer, &gameState->Images[1], 50, 180, Input->MouseX - 50, Input->MouseY - 180);
    //int number = 16;
    //for (int x = 0; x < number; x++)
    //{
    //    for (int y = 0; y < number; y++)
    //    {
    //        float t = abs(sin(gameState->GameTime + (x * y) * 0.01f));
    //        DrawRectangle(VideoBuffer, 100 + x * 24, 8 + y * 24, 20, 20, 0.4f, 0.2, 0.0, t);
    //    }
    //}
    

    if (distance(Input->MouseX, Input->MouseY, gameState->RayStartX, gameState->RayStartY) < 10)
    {
        DrawCircle(VideoBuffer, gameState->RayStartX, gameState->RayStartY, 8);
        DrawCircle(VideoBuffer, gameState->RayStartX, gameState->RayStartY, 7.5);
        DrawCircle(VideoBuffer, gameState->RayStartX, gameState->RayStartY, 7);
    
        if (Input->MouseLeftDown)
        {
            gameState->RayStartGrabbed = true;
        }
    }
    else
    {
        DrawCircle(VideoBuffer, gameState->RayStartX, gameState->RayStartY, 8);
    }
    if (distance(Input->MouseX, Input->MouseY, gameState->RayStartX + gameState->RayDirX, gameState->RayStartY + gameState->RayDirY) < 10)
    {
        DrawCircle(VideoBuffer, gameState->RayStartX + gameState->RayDirX, gameState->RayStartY + gameState->RayDirY, 8);
        DrawCircle(VideoBuffer, gameState->RayStartX + gameState->RayDirX, gameState->RayStartY + gameState->RayDirY, 7.5);
        DrawCircle(VideoBuffer, gameState->RayStartX + gameState->RayDirX, gameState->RayStartY + gameState->RayDirY, 7);
        if (Input->MouseLeftDown)
        {
            gameState->RayDirGrabbed = true;
        }
    }
    else
    {
        DrawCircle(VideoBuffer, gameState->RayStartX + gameState->RayDirX, gameState->RayStartY + gameState->RayDirY, 8);
    }
    if (Input->MouseLeftUp)
    {
        gameState->RayStartGrabbed = false;
        gameState->RayDirGrabbed = false;
    }
    if (gameState->RayStartGrabbed)
    {
        gameState->RayStartX += Input->MouseXDelta;
        gameState->RayStartY += Input->MouseYDelta;
        gameState->RayDirX -= Input->MouseXDelta;
        gameState->RayDirY -= Input->MouseYDelta;
    }
    if (gameState->RayDirGrabbed)
    {
        gameState->RayDirX += Input->MouseXDelta;
        gameState->RayDirY += Input->MouseYDelta;
    }
    
    DrawLine(VideoBuffer, gameState->RayStartX, gameState->RayStartY,   gameState->RayStartX + gameState->RayDirX * 2 , gameState->RayStartY + gameState->RayDirY * 2);

    
    float width = 150;
    float height = 60;
    float x = 300;
    float y = 300;
    DrawBox(VideoBuffer, x, y, width, height);
    float rx = gameState->RayStartX;
    float ry = gameState->RayStartY;
    float dx = gameState->RayDirX;
    float dy = gameState->RayDirY;
    float ClosestDistance = RayBoxIntersect(rx, ry, dx, dy, x, y, width, height);

    if (ClosestDistance > 0)
    {
        DrawCircle(VideoBuffer, rx + dx * ClosestDistance, ry + dy * ClosestDistance, 3);
        DrawCircle(VideoBuffer, rx + dx * ClosestDistance, ry + dy * ClosestDistance, 4);
        DrawCircle(VideoBuffer, rx + dx * ClosestDistance, ry + dy * ClosestDistance, 5);
        //DrawCircle(VideoBuffer, x + ((rx - x) + dx * FurthestDistance), y + ((ry - y) + dy * FurthestDistance), 3);
        //DrawCircle(VideoBuffer, x + ((rx - x) + dx * FurthestDistance), y + ((ry - y) + dy * FurthestDistance), 4);
        //DrawCircle(VideoBuffer, x + ((rx - x) + dx * FurthestDistance), y + ((ry - y) + dy * FurthestDistance), 5);
    }

    int sizeX = 1280/2;
    int sizeY = 1280/2;
    for (int xi = 0; xi < sizeX; xi++)
    {
        for (int yi = sizeY/1.7; yi < sizeY; yi++)
        {
            float x = xi / (float)sizeX;
            float y = yi / (float)sizeY;

            float ForwardX = cos(gameState->GameTime);
            float ForwardY = sin(gameState->GameTime);
            float ForwardZ = 0;

            float RightX = ForwardY;
            float RightY = -ForwardX;
            float RightZ = 0;

            float UpX = 0;
            float UpY = 0;
            float UpZ = 1;

            float TopLeftX = ForwardX + RightX + UpX; 
            float TopLeftY = ForwardY + RightY + UpY; 
            float TopLeftZ = ForwardZ + RightZ + UpZ; 

            float TopRightX = ForwardX - RightX + UpX;
            float TopRightY = ForwardY - RightY + UpY;
            float TopRightZ = ForwardZ - RightZ + UpZ;

            float BottomLeftX = ForwardX + RightX - UpX;
            float BottomLeftY = ForwardY + RightY - UpY;
            float BottomLeftZ = ForwardZ + RightZ - UpZ;

            float BottomRightX = ForwardX - RightX - UpX;
            float BottomRightY = ForwardY - RightY - UpY;
            float BottomRightZ = ForwardZ - RightZ - UpZ;



            float dx1 = lerp(TopLeftX, TopRightX, x);
            float dy1 = lerp(TopLeftY, TopRightY, x);
            float dz1 = lerp(TopLeftZ, TopRightZ, x);

            float dx2 = lerp(BottomLeftX, BottomRightX, x);
            float dy2 = lerp(BottomLeftY, BottomRightY, x);
            float dz2 = lerp(BottomLeftZ, BottomRightZ, x);

            float dx = lerp(dx1, dx2, y);
            float dy = lerp(dy1, dy2, y);
            float dz = lerp(dz1, dz2, y);

            float NormalX = 0;
            float NormalY = 0;
            float NormalZ = 0;
            float dist = RayBoxIntersect(0, 0, 1, 
                                        dx, dy, dz, 
                                        ForwardX * 2.2, ForwardY *2.2, 0,
                                        1, 1, 1,
                                        &NormalX, &NormalY, &NormalZ);
            if (dist > 0)
            {
                float px = 0 + dx * dist - ForwardX * 2.2;
                float py = 0 + dy * dist - ForwardY * 2.2;
                float pz = 1 + dz * dist - 0;
                float px2 = px + 0.5;
                float py2 = py + 0.5;
                float pz2 = pz + 0.5;
                float u = 0;
                float v = 0;
                if (abs(NormalX) > 0)
                {
                    u = py2;
                    v = 1-pz2;
                }
                else if (abs(NormalY) > 0)
                {
                    u = px2;
                    v = 1-pz2;
                }
                else if (abs(NormalZ) > 0)
                {
                    u = px2;
                    v = py2;
                }

                uint32 Sample = ImageSample(&gameState->Images[2], u, v);
                ((uint32*)VideoBuffer->Memory)[((yi-200) * VideoBuffer->Pitch/4 + (xi+150))] = Sample;

                //((char*)VideoBuffer->Memory)[(yi * VideoBuffer->Pitch + xi * 4) + 0] = 0 * 255;
                //((char*)VideoBuffer->Memory)[(yi * VideoBuffer->Pitch + xi * 4) + 1] = v * 255;
                //((char*)VideoBuffer->Memory)[(yi * VideoBuffer->Pitch + xi * 4) + 2] = u * 255;
                //((char*)VideoBuffer->Memory)[(yi * VideoBuffer->Pitch + xi * 4) + 3] = 255;
            }
        }
    }
    
    int uiOffset = 0;
    DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "Time: ", (float)(gameState->GameTime));
    DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "FPS: ", (float)(1.0f / Input->DeltaTime));
    DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "Space to reset.", 0);
    font_draw(gameState->fontSpritesheet, VideoBuffer, 200, 400, "int StringLength(const char* checkString)\n{\n    int i = 0;\n    while (checkString[i] != 0)\n    {\n        i++;\n    }\n    return i;\n}\n");

    
    //DebugDrawInput(gameState->fontSpritesheet, VideoBuffer, Input);
}