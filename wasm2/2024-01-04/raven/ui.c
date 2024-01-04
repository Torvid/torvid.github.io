#pragma once
#include "platform.c"
#include "array.c"
#include "memory.c"
#include "math.c"
#include "string.c"
#include "draw.c"
#include "ui.c"

bool UIToggle(bool value, float2 pos, string text)
{
    float3 colorBright = float3(0.75, 0.75, 0.75);
    float3 colorMiddle = float3(0.50, 0.50, 0.50);
    float3 colorDark   = float3(0.25, 0.25, 0.25);
    
    int size = 32;
    float2 buttonSize = float2(size, size);
    DrawBoxFilled(pos, buttonSize, colorMiddle);
    bool intersects = IntersectPointBox(float2(platform->mouseX, platform->mouseY), pos, buttonSize);
    if(intersects)
    {
        if(platform->mouseLeft)
        {
            colorBright = float3(0.25, 0.25, 0.25);
            colorMiddle = float3(0.50, 0.50, 0.50);
            colorDark   = float3(0.75, 0.75, 0.75);
        }
    }
    DrawString(text, float2(pos.x+size/4+size, pos.y-size/16), size, float3(1,1,1));
    if(value)
    {
        DrawBox(float2(pos.x+size/6, pos.y+size/6), float2(buttonSize.x-size/3, buttonSize.y-size/3), float3(0,0,0), size/8);
    }
    int borderSize = size/16;
    DrawBoxFilled(pos, float2(buttonSize.x, borderSize), colorBright);
    DrawBoxFilled(pos, float2(borderSize, buttonSize.y), colorBright);
    DrawBoxFilled(float2(pos.x, pos.y+buttonSize.y-borderSize), float2(buttonSize.x, borderSize), colorDark);
    DrawBoxFilled(float2(pos.x+buttonSize.x-borderSize, pos.y), float2(borderSize, buttonSize.y), colorDark);
    if(intersects)
    {
        if (platform->mouseLeftUp)
        {
            return !value;
        }
    }
    return value;
}

bool UIButton(float2 pos, string text)
{
    float3 colorBright = float3(0.75, 0.75, 0.75);
    float3 colorMiddle = float3(0.50, 0.50, 0.50);
    float3 colorDark   = float3(0.25, 0.25, 0.25);

    int size = 32;
    float2 buttonSize = DrawStringSize(text, size);
    DrawBoxFilled(pos, buttonSize, colorMiddle);

    int offset = 0;
    bool intersects = IntersectPointBox(float2(platform->mouseX, platform->mouseY), pos, buttonSize);
    if(intersects)
    {
        if(platform->mouseLeft)
        {
            colorBright = float3(0.25, 0.25, 0.25);
            colorMiddle = float3(0.50, 0.50, 0.50);
            colorDark   = float3(0.75, 0.75, 0.75);
            offset = size/16;
        }
    }
    DrawString(text, float2(pos.x+size/4+offset, pos.y-size/16+offset), size, float3(0,0,0));
    
    int borderSize = size/16;
    DrawBoxFilled(pos, float2(buttonSize.x, borderSize), colorBright);
    DrawBoxFilled(pos, float2(borderSize, buttonSize.y), colorBright);
    DrawBoxFilled(float2(pos.x, pos.y+buttonSize.y-borderSize), float2(buttonSize.x, borderSize), colorDark);
    DrawBoxFilled(float2(pos.x+buttonSize.x-borderSize, pos.y), float2(borderSize, buttonSize.y), colorDark);
    
    if(intersects)
    {
        if (platform->mouseLeftUp)
        {
            return true;
        }
    }
    return false;
}