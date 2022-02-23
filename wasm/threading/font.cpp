#pragma once

#include "quote_platform.cpp"

void SetPixel(game_offscreen_buffer *VideoBuffer, int x, int y, uint32 Color)
{
    //x = (int)min(max((float)x, 0.0), (float)(VideoBuffer->Width - 1));
    //y = (int)min(max((float)y, 0.0), (float)(VideoBuffer->Height - 1));
    x = clamp(x, 0, VideoBuffer->Width - 1);
    y = clamp(y, 0, VideoBuffer->Height - 1);
    uint32 offset = (y * VideoBuffer->Width + x);
    *((uint32*)VideoBuffer->Memory + offset) = Color;
}

int StringLength(const char* checkString)
{
    int i = 0;
    while (checkString[i] != 0)
    {
        i++;
    }
    return i;
}

bool EndsWith(char* inputString, const char* checkString)
{
    int endOfString = StringLength(inputString);
    int checkStringSize = StringLength(checkString);

    // Loop through string to check if the ending matches
    for (int i = 0; i < checkStringSize; i++)
    {
        if (checkString[i] != inputString[(endOfString - checkStringSize) + i])
        {
            return false;
        }
    }
    return true;
}

const int PixelWidth = 192;
const int PixelHeight = 52;
const int CharactersWidth = 32;
const int CharactersHeight = 4;
const int SpriteWidth = 6;
const int SpriteHeight = 13;

//struct Spritesheet
//{
//    char Image[PixelWidth * PixelHeight * 4];
//};

//void font_init(Spritesheet* fontSpritesheet, game_memory* Memory)
//{
//    uint32* Image = Memory->PlatformReadImageIntoMemory(fontSpritesheet->Image, "font.tga", 0, 0);
//}

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