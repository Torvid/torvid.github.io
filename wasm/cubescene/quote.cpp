//#pragma optimize( "g", on )

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
                //((uint8*)Pixel)[3] = va * 256.0f;

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

//void DrawBox(game_offscreen_buffer *Buffer, int X, int Y, int width, int height, int R, int G, int B)
//{
//    if (X < 0)
//    {
//        width -= -X;
//        X = 0;
//    }
//    if (Y < 0)
//    {
//        height -= -Y;
//        Y = 0;
//    }
//    if ((X + width) > Buffer->Width)
//    {
//        width = X - Buffer->Width;
//    }
//    if ((X + height) > Buffer->Height)
//    {
//        height = Y - Buffer->Height;
//    }
//
//
//    int pitch = Buffer->Width;
//    uint32* CurrentPixel = (uint32*)Buffer->Memory;
//    CurrentPixel += pitch * Y;
//    for (int x = 0; x < height; x++)
//    {
//        uint32* temp = CurrentPixel;
//        CurrentPixel += X;
//        for (int y = 0; y < width; y++)
//        {
//            *CurrentPixel = R << 16 | G << 8 | B << 0;
//            CurrentPixel++;
//        }
//        CurrentPixel = temp;
//        CurrentPixel += pitch; // skip to next row
//    }
//}

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
    if(value != 0)
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
        //*dist = -1;
        //return false;
    }

    float ClosestDistance = max(minx, miny, minz);
    float FurthestDistance = min(maxx, maxy, maxz);

    if (ClosestDistance > FurthestDistance)
    {
        return -1;
        //*dist = -1;
        //return false;
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
    //*dist = ClosestDistance;
    //return true;
}

float RayBoxIntersect(game_offscreen_buffer* VideoBuffer, float rx, float ry, float dx, float dy, float x, float y, float width, float height)
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

    DrawCircle(VideoBuffer, rx + dx * ClosestDistance,     ry + dy * ClosestDistance, 4);
    DrawCircle(VideoBuffer, rx + dx * FurthestDistance,     ry + dy * FurthestDistance, 3);

    if (minx > maxy || miny > maxx)
    {
        DrawCircle(VideoBuffer, 0, 0, 100);
    }
    return -1;
}

void DrawBox(GameState* gameState, game_offscreen_buffer* VideoBuffer, 
    float3 CameraPos, float3 CameraForward, float3 CameraUp, 
    float3 BoxPos,    float3 BoxForward, float3 BoxUp, float3 BoxScale)
{
    int sizeX = 1920 / 2;
    int sizeY = 1080 / 2;
    float AspectRatio = 0.5625f;
    float AspectRatio2 = 1.777777777f;

    float3 Forward = CameraForward;
    float3 Right = cross(CameraForward, CameraUp);
    float3 Up = CameraUp * AspectRatio;

    float3 TopLeft = ((Forward + -Right) + Up);
    float3 TopRight = ((Forward + Right) + Up);
    float3 BottomLeft = ((Forward + -Right) + -Up);
    float3 BottomRight = ((Forward + Right) + -Up);

    float3 LocalPos = CameraPos - BoxPos;
    float3 BoxRight = cross(BoxForward, BoxUp) * BoxScale.x;
    BoxForward = BoxForward * BoxScale.y;
    BoxUp = BoxUp * BoxScale.z;

    LocalPos = float3(dot(LocalPos, BoxRight), dot(LocalPos, BoxForward), dot(LocalPos, BoxUp));
    float3 LightDir = normalize(float3(0.5, 0.8, 0.75));
    
    // world space to screen space
    float3 LocalPos2 = float3(dot(CameraPos - BoxPos, Right), dot(CameraPos - BoxPos, Forward), dot(CameraPos - BoxPos, Up));
    float depth = -LocalPos2.y;
    int X = -(LocalPos2.x / depth) * sizeX * 0.5 + sizeX * 0.5f;
    int Y = (LocalPos2.z / depth) * sizeY * 1.58f + sizeY * 0.5f;
    int width = 1000 / depth;
    int height = 1000 / depth;

    if (depth < 0)
    {
        return;
    }

#if 1
    int MinX = FloorToInt((X - width / 2));
    int MinY = FloorToInt((Y - height / 2));
    int MaxX = FloorToInt((X - width / 2) + width);
    int MaxY = FloorToInt((Y - height / 2) + height);

    if (MinX < 0)
        MinX = 0;
    if (MinY < 0)
        MinY = 0;

    if (MaxX > VideoBuffer->Width)
        MaxX = VideoBuffer->Width;
    if (MaxY > VideoBuffer->Height)
        MaxY = VideoBuffer->Height;

    for (int yi = MinY; yi < MaxY; yi++)
    {
        for (int xi = MinX; xi < MaxX; xi++)
        {
            float x = xi / (float)sizeX;
            float y = yi / (float)sizeY;

            float3 d1 = lerp(TopLeft, TopRight, x);
            float3 d2 = lerp(BottomLeft, BottomRight, x);
            float3 WorldDir = lerp(d1, d2, y);

            // Transform CameraPos and d to the space of the box
            float3 LocalDir = float3(dot(WorldDir, BoxRight), dot(WorldDir, BoxForward), dot(WorldDir, BoxUp));

            float3 Normal = {};
            float dist = RayBoxIntersect(LocalPos, LocalDir, &Normal);
            if (dist < 0)
                continue;

            uint32* Pixel = &((uint32*)VideoBuffer->Memory)[(((yi)*VideoBuffer->Pitch) / 4 + (xi))];
            uint8 OldDepth = ((uint8*)Pixel)[3];
            float FarZ = 16.0f;
            uint8 NewDepth = (uint8)((dist * 256) / FarZ);

            if (NewDepth < OldDepth)
            {
                float3 p = float3(LocalPos.x + LocalDir.x * dist,
                    LocalPos.y + LocalDir.y * dist,
                    LocalPos.z + LocalDir.z * dist);
                float3 p2 = p + 0.5;
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

                //u = frac(u);
                //v = frac(v);
                uint32 NewSample = ImageSample(&gameState->Images[2], u, v);
                float3 brightness = clamp01(float3(0.6, 0.30, 0.35) + clamp01(dot(Normal, LightDir)));
                ((uint8*)&NewSample)[0] *= brightness.x;
                ((uint8*)&NewSample)[1] *= brightness.y;
                ((uint8*)&NewSample)[2] *= brightness.z;
                ((uint8*)(&NewSample))[3] = NewDepth;

                // Set all 4 bytes at once
                *Pixel = NewSample;
                ((uint8*)Pixel)[3] = NewDepth;
            }
        }
    }

#else
    for (int yi = 0; yi < sizeY; yi++)
    {
        for (int xi = 0; xi < sizeX; xi++)
        {
            float x = xi / (float)sizeX;
            float y = yi / (float)sizeY;

            float3 d1 = lerp(TopLeft, TopRight, x);
            float3 d2 = lerp(BottomLeft, BottomRight, x);
            float3 WorldDir = lerp(d1, d2, y);

            // Transform CameraPos and d to the space of the box
            float3 LocalDir = float3(dot(WorldDir, BoxRight), dot(WorldDir, BoxForward), dot(WorldDir, BoxUp));

            float3 Normal = {};
            float dist = RayBoxIntersect(LocalPos, LocalDir, &Normal);
            if (dist < 0)
                continue;

            uint32* Pixel = &((uint32*)VideoBuffer->Memory)[(((yi)*VideoBuffer->Pitch) / 4 + (xi))];
            uint8 OldDepth = ((uint8*)Pixel)[3];
            float FarZ = 16.0f;
            uint8 NewDepth = (uint8)((dist * 256) / FarZ);

            if (NewDepth < OldDepth)
            {
                float3 p = float3(  LocalPos.x + LocalDir.x * dist,
                                    LocalPos.y + LocalDir.y * dist,
                                    LocalPos.z + LocalDir.z * dist);
                float3 p2 = p + 0.5;
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

                //u = frac(u);
                //v = frac(v);
                uint32 NewSample = ImageSample(&gameState->Images[2], u, v);
                float3 brightness = clamp01(float3(0.6, 0.30, 0.35) + clamp01(dot(Normal, LightDir)));
                ((uint8*)&NewSample)[0] *= brightness.x;
                ((uint8*)&NewSample)[1] *= brightness.y;
                ((uint8*)&NewSample)[2] *= brightness.z;

                ((uint8*)(&NewSample))[3] = NewDepth;

#if 1
                // Set all 4 bytes at once
                *Pixel = NewSample;
                ((uint8*)Pixel)[3] = NewDepth;
#else
                // Set all bytes of the pixel individually
                ((uint8*)Pixel)[0] = ((uint8*)&NewSample)[0];
                ((uint8*)Pixel)[1] = ((uint8*)&NewSample)[1];
                ((uint8*)Pixel)[2] = ((uint8*)&NewSample)[2];
                ((uint8*)Pixel)[3] = NewDepth;
#endif
            }
        }
    }
#endif
    if (depth > 0 && !gameState->GameViewEnabled)
    {
        DrawBox(VideoBuffer, X, Y, width, height);
        DrawCross(VideoBuffer, X, Y);
    }
}

float GetSmoothedValue(float data[100], int* index, float NewTime, int count)
{
    (*index)++;
    (*index) %= count;
    data[*index] = NewTime;// (float)(time_end - time_start) / 10000000;
    float accumulatedDelta = 0;
    for (int i = 0; i < count; i++)
    {
        accumulatedDelta += data[i];
    }
    accumulatedDelta /= count;
    return accumulatedDelta;
}

//(thread_context* Thread, game_memory* Memory, game_input* Input, game_offscreen_buffer* VideoBuffer, game_sound_output_buffer* SoundBuffer)
#if WASM
#else
__declspec(dllexport)
#endif
extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    int64 Global_Start = Memory->PlatformTime();
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
        gameState->UseFiltering = true;
        gameState->CameraForward = float3(0, 1, 0);
        gameState->CameraUp = float3(0, 0, 1);
        gameState->CameraPos = float3(0, -3, 0);
        gameState->GameViewEnabled = true;

    }
    gameState->GameTime += Input->DeltaTime;
    DrawRectangle(VideoBuffer, 0, 0, 960, 540, 0.0, 0.0, 0.5);
    int uiOffset = 0;
    

    //float sinvalue = abs(sin(gameState->GameTime));
    //if (Input->FDown)
    //{
    //    gameState->UseFiltering = !gameState->UseFiltering;
    //}
    //if (gameState->UseFiltering)
    //    DrawImageScaledSmooth(VideoBuffer, &gameState->Images[1], 50, 180, Input->MouseX - 50, Input->MouseY - 180);
    //else
    //    DrawImageScaled(VideoBuffer, &gameState->Images[1], 50, 180, Input->MouseX - 50, Input->MouseY - 180);
    //int number = 16;
    //for (int x = 0; x < number; x++)
    //{
    //    for (int y = 0; y < number; y++)
    //    {
    //        float t = abs(sin(gameState->GameTime + (x * y) * 0.01f));
    //        DrawRectangle(VideoBuffer, 100 + x * 24, 8 + y * 24, 20, 20, 0.4f, 0.2, 0.0, t);
    //    }
    //}

    //float3 CameraPos = float3(0, -2, 0);
    //float3 CameraForward = float3(cos(-gameState->GameTime), sin(-gameState->GameTime), 0);
    //float3 CameraForward = float3(1, 0, 0);
    //float3 CameraUp = float3(0, 0, 1);
    if (Input->GDown)
        gameState->GameViewEnabled = !gameState->GameViewEnabled;

    if (Input->MouseLeft)
    {
        gameState->CameraForward = RotateAroundAxis(gameState->CameraForward, float3(0, 0, 1), -Input->MouseXDelta * 0.005f);
        gameState->CameraUp = RotateAroundAxis(gameState->CameraUp, float3(0, 0, 1), -Input->MouseXDelta * 0.005f);

        float3 CameraRight = cross(gameState->CameraUp, gameState->CameraForward);
        gameState->CameraForward = RotateAroundAxis(gameState->CameraForward, CameraRight, Input->MouseYDelta * 0.005f);
        gameState->CameraUp = RotateAroundAxis(gameState->CameraUp, CameraRight, Input->MouseYDelta * 0.005f);
    }

    int X = Input->W ? 1 : 0 + Input->S ? -1 : 0;
    int Y = Input->A ? 1 : 0 + Input->D ? -1 : 0;
    int Z = Input->E ? 1 : 0 + Input->Q ? -1 : 0;

    gameState->CameraPos = gameState->CameraPos + gameState->CameraForward * X * Input->DeltaTime * 2;
    gameState->CameraPos = gameState->CameraPos + cross(gameState->CameraUp, gameState->CameraForward) * Y * Input->DeltaTime * 2;
    gameState->CameraPos = gameState->CameraPos + float3(0,0,1) * Z * Input->DeltaTime * 2;

    float3 BoxForward = float3(1, 0, 0);
    float3 BoxUp = float3(0, 0, 1);
    float3 BoxScale = float3(1, 1, 1);
    float3 BoxScale2 = float3(1, 1, 10);

    float3 RotatingBoxForward = float3(cos(gameState->GameTime), sin(gameState->GameTime), 0);

    int64 time_start = Memory->PlatformTime();
    DrawBox(gameState, VideoBuffer, gameState->CameraPos, gameState->CameraForward, gameState->CameraUp, float3(16, 0, 1.1),   BoxForward, BoxUp, BoxScale);
    DrawBox(gameState, VideoBuffer, gameState->CameraPos, gameState->CameraForward, gameState->CameraUp, float3(0, 0, 2.2),    BoxForward, BoxUp, BoxScale);
    DrawBox(gameState, VideoBuffer, gameState->CameraPos, gameState->CameraForward, gameState->CameraUp, float3(1.5, 0, 0),    BoxForward, BoxUp, BoxScale2);
    DrawBox(gameState, VideoBuffer, gameState->CameraPos, gameState->CameraForward, gameState->CameraUp, float3(1.1, 0, 1.1),  BoxForward, BoxUp, BoxScale);
    DrawBox(gameState, VideoBuffer, gameState->CameraPos, gameState->CameraForward, gameState->CameraUp, float3(0, 0, 0),      RotatingBoxForward, BoxUp, BoxScale);

    int64 time_end = Memory->PlatformTime();
    
    //gameState->deltasIndex++;
    //gameState->deltasIndex %= 100;
    //gameState->deltas[gameState->deltasIndex] = (float)(time_end - time_start) / 10000000;
    //float accumulatedDelta = 0;
    //for (int i = 0; i < 100; i++)
    //{
    //    accumulatedDelta += gameState->deltas[i];
    //}
    //accumulatedDelta /= 100;
    float FPS = (float)(1.0f / Input->DeltaTime);
    float Delta = (float)(time_end - time_start) / 10000000;
    float accumulatedDelta = GetSmoothedValue(gameState->deltas, &gameState->deltasIndex, Delta, 30);
    float SmoothFPS = GetSmoothedValue(gameState->FPS, &gameState->FPSIndex, FPS, 30);


    DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "Controls", 0);
    DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "    Space to reset.", 0);
    DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "    WASD to move.", 0);
    DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "    Left mouse to look around.", 0);
    DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "    G to toggle debug view.", 0);

    if (!gameState->GameViewEnabled)
    {
        DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, " ", 0);
        DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "Timers", 0);
        DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "    Draw Boxes MS: ", Delta * 1000);
        DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "    Draw Boxes Smooth MS: ", accumulatedDelta * 1000);
        //DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "    Time: ", (float)(gameState->GameTime));
        DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "    FPS: ", FPS);
        DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "    Smooth FPS: ", SmoothFPS * 100);
    }

    //DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "Distance to box: ", ClosestDistance);
    //DrawThing(gameState->fontSpritesheet, VideoBuffer, &uiOffset, "Press F to toggle filtering.", gameState->UseFiltering);
    //font_draw(gameState->fontSpritesheet, VideoBuffer, 200, 400, "int StringLength(const char* checkString)\n{\n    int i = 0;\n    while (checkString[i] != 0)\n    {\n        i++;\n    }\n    return i;\n}\n");
    /*
    float width = 150;
    float height = 60;
    float x = 300;
    float y = 300;
    DrawBox(VideoBuffer, x, y, width, height);
    DrawBox(VideoBuffer, x, y, width, height * 16);
    DrawBox(VideoBuffer, x, y, width * 16, height);
    float rx = gameState->RayStartX;
    float ry = gameState->RayStartY;
    float dx = gameState->RayDirX;
    float dy = gameState->RayDirY;
    float ClosestDistance = RayBoxIntersect(VideoBuffer, rx, ry, dx, dy, x, y, width, height);

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

    DrawLine(VideoBuffer, gameState->RayStartX, gameState->RayStartY, gameState->RayStartX + gameState->RayDirX * 1.25, gameState->RayStartY + gameState->RayDirY * 1.25);



    if (ClosestDistance > 0)
    {
        DrawCircle(VideoBuffer, rx + dx * ClosestDistance, ry + dy * ClosestDistance, 3);
        DrawCircle(VideoBuffer, rx + dx * ClosestDistance, ry + dy * ClosestDistance, 4);
        DrawCircle(VideoBuffer, rx + dx * ClosestDistance, ry + dy * ClosestDistance, 5);
    }
    */
    //DebugDrawInput(gameState->fontSpritesheet, VideoBuffer, Input);

    if (!gameState->GameViewEnabled)
    {
        DrawRectangle(VideoBuffer, 0, VideoBuffer->Height - 200, 200, 200, 0.0, 0.0, 0.0, 0.5);

        int64 Global_End = Memory->PlatformTime();
        float TotalTimeDelta = (float)(Global_End - Global_Start) / 10000000;
        float SmoothedTime = GetSmoothedValue(gameState->deltas2, &gameState->deltas2Index, TotalTimeDelta, 100);

        float HeightMultiplier = 5000;
        float LastHeight = 0;
        for (int i = 0; i < 100; i++)
        {
            int h = (i + gameState->deltas2Index) % 100;
            float height = VideoBuffer->Height - gameState->deltas2[h] * HeightMultiplier;
            DrawLine(VideoBuffer, i * 2 - 2, LastHeight, i * 2, height);
            LastHeight = height;
        }

        float fps30dt = (1.0f / 30.0f);
        float fps60dt = (1.0f / 60.0f);
        float fps30height = VideoBuffer->Height - fps30dt * HeightMultiplier;
        float fps60height = VideoBuffer->Height - fps60dt * HeightMultiplier;
        float fpscurrentheight = VideoBuffer->Height - (SmoothedTime)*HeightMultiplier;
        DrawLine(VideoBuffer, 0, fps30height, 200, fps30height);
        DrawLine(VideoBuffer, 0, fps60height, 200, fps60height);
        font_draw(gameState->fontSpritesheet, VideoBuffer, 220, fps30height, "33.33 ms");
        font_draw(gameState->fontSpritesheet, VideoBuffer, 220, fps60height, "16.66 ms");

        char Space[100] = {};
        Append(Space, (int)(SmoothedTime));//"60 fps"
        font_draw(gameState->fontSpritesheet, VideoBuffer, 202, fpscurrentheight, Space);
    }

}