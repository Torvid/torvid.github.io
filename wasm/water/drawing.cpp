#pragma once
#include "quote.h"

void QueueDrawSphere(EngineState* engineState, Transform t, Image* texture1, Image* texture2 = 0, int ShaderIndex = 0, bool selected = false, bool hovered = false)
{
    RenderCommand renderCommand = {};
    renderCommand.texture1 = texture1;
    renderCommand.texture2 = texture2;
    renderCommand.Type = RenderCommandType_Sphere;
    renderCommand.t = t;
    renderCommand.shaderIndex = ShaderIndex;
    renderCommand.selected = selected;
    renderCommand.hovered = hovered;
    ArrayAdd(engineState->renderCommands, renderCommand);
}
void QueueDrawBox(EngineState* engineState, Transform t, Image* texture1, Image* texture2 = 0, int ShaderIndex = 0, bool selected = false, bool hovered = false)
{
    RenderCommand renderCommand = {};
    renderCommand.texture1 = texture1;
    renderCommand.texture2 = texture2;
    renderCommand.Type = RenderCommandType_Box;
    renderCommand.t = t;
    renderCommand.shaderIndex = ShaderIndex;
    renderCommand.selected = selected;
    renderCommand.hovered = hovered;
    ArrayAdd(engineState->renderCommands, renderCommand);
}
void QueueDrawSkyBox(EngineState* engineState, Image* SkyFaces[6])
{
    RenderCommand renderCommand = {};
    for (int i = 0; i < 6; i++)
    {
        renderCommand.cubemapTexture[i] = SkyFaces[i];
    }
    renderCommand.Type = RenderCommandType_Skybox;
    ArrayAdd(engineState->renderCommands, renderCommand);
}
void QueueDrawEffect(EngineState* engineState, Transform t, Image* texture1, Image* texture2, int ShaderIndex, EffectData effect, float3 Color)
{
    RenderCommand renderCommand = {};
    renderCommand.effect = effect;
    renderCommand.t = t;
    renderCommand.Type = RenderCommandType_Effect;
    renderCommand.texture1 = texture1;
    renderCommand.texture2 = texture2;
    renderCommand.shaderIndex = ShaderIndex;
    renderCommand.color = Color;
    ArrayAdd(engineState->renderCommands, renderCommand);
}
void QueueClear(EngineState* engineState, float3 Color)
{
    RenderCommand renderCommand = {};
    renderCommand.color = Color;
    renderCommand.Type = RenderCommandType_Clear;
    ArrayAdd(engineState->renderCommands, renderCommand);
}
int2 GetThreadRegion(EngineState* engineState, float Regions, float Region, int Height, int MinY, int MaxY)
{
    if (Region >= 0)
    {
        float Start = engineState->renderThreadStart[(int)Region] * Height;
        float End = engineState->renderThreadEnd[(int)Region] * Height;
        //float Start = (Region / (Regions)) * Height;
        //float End = ((Region + 1) / (Regions)) * Height;

        if (Start > MinY)
            MinY = Start;

        if (End < MaxY)
            MaxY = End;
    }
    return int2(MinY, MaxY);
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

bool DepthTest1(float dist, uint32* Pixel, int xi, bool* draw)
{
    uint8 OldDepth = ((uint8*)Pixel)[3];
    uint8 NewDepth = DistanceToDepthBufferValue(dist);
    if (NewDepth > OldDepth || NewDepth == 255)
    {
        return true;
    }
    ((uint8*)Pixel)[3] = NewDepth;
    return false;
}

uint16 DistanceToDepthBufferValue2(float dist)
{
    float FarZ = 64.0f;
    return (uint16)(clamp01(dist / FarZ) * (65536));
}
void DarkenBuffer(VideoBuffer* Buffer)
{
    uint8* Row = (uint8*)Buffer->memory;
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

bool DepthTest2(float dist, uint32* Pixel, int xi, bool* draw)
{
    bool other = xi % 2 == 0;

    uint8* left = &(((uint8*)Pixel)[3]);
    uint8* right = &(((uint8*)Pixel)[3]);
    if (other)
    {
        right = &(((uint8*)((Pixel + 1)))[3]);
        *draw = false;
    }
    else
    {
        left = &(((uint8*)(Pixel - 1))[3]);
    }

    // Read the old depth
    uint16 OldDepth = *left | (*right << 8);

    // Get and write the new depth
    uint16 NewDepth = DistanceToDepthBufferValue2(dist);

    //uint8 NewDepth = DistanceToDepthBufferValue(dist); 
    if (!(*draw))
    {
        if (NewDepth > OldDepth || NewDepth == 255)
        {
            return true;
        }
    }
    *left = (uint8)NewDepth;
    *right = (uint8)(NewDepth >> 8);
    *draw = true;
    return false;
}
bool DepthTest(float dist, uint32* Pixel, int xi, bool* draw)
{
    return DepthTest2(dist, Pixel, xi, draw);
}

void DrawSprite(EngineState* engineState, VideoBuffer* Buffer, Image* image, float2 Pos, float2 Size, float3 Color, int Shader = 0, int Region = -1, int Regions = 9, float dist = 0)
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

    uint8* Row = ((uint8*)Buffer->memory + MinY * Buffer->Pitch + MinX * Buffer->BytesPerPixel);

    for (int y = MinY; y < MaxY; y++)
    {
        bool draw;
        uint32* Pixel = (uint32*)Row;
        for (int x = MinX; x < MaxX; x++)
        {
            int image_x = (x - IntX) * (image->width / outputWidth);
            int image_y = (y - IntY) * (image->height / outputHeight);
            if (Shader == 0) // Opaque
            {
                uint32 OldPixelData = *Pixel;

                if (!DepthTest(dist, Pixel, x, &draw))
                {
                    SetColor(Pixel, image->ImageData[image_y * image->width + image_x]);
                }
            }
            else if (Shader == 1) // Additive
            {
                uint32 NewPixelData = image->ImageData[image_y * image->width + image_x];
                uint32 OldPixelData = *Pixel;

                float3 NewPixel = GetBufferColorReverse(Buffer, NewPixelData) * Color;
                float3 OldPixel = GetBufferColorReverse(Buffer, OldPixelData);

                float3 Result = clamp01(NewPixel + OldPixel);

                uint32 ResultData = MakeBufferColor(Buffer, Result);

                if (!DepthTest(dist, Pixel, x, &draw))
                {
                    SetColor(Pixel, ResultData);
                }
            }
            else if (Shader == 2) // Cutout?
            {
                uint32 NewPixelData = image->ImageData[image_y * image->width + image_x];
                uint32 OldPixelData = *Pixel;

                bool alphaVisible = ((uint8*)(&NewPixelData))[3] > 128;

                if (!DepthTest(dist, Pixel, x, &draw) && alphaVisible)
                {
                    SetColor(Pixel, NewPixelData);
                }
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

void DrawSkybox(EngineState* engineState, VideoBuffer* videoBuffer, Transform Camera, int Region = -1, int Regions = 9)
{
    ProfilerBeingSample(&engineState->profilingData);
    if (!Valid(engineState->skyFaces[0]))
        return; // skybox not loaded.

    int sizeX = videoBuffer->Width;
    int sizeY = videoBuffer->Height;
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
    float3 RightStep = RightOffset / (videoBuffer->Width);
    float3 DownStep = DownOffset / (videoBuffer->Height);

    int MinX = 0;
    int MinY = 0;
    int MaxX = videoBuffer->Width;
    int MaxY = videoBuffer->Height;
    float RegionWidth = videoBuffer->Width;

    int2 ChangedSize = GetThreadRegion(engineState, Regions, Region, videoBuffer->Height, MinY, MaxY);
    MinY = ChangedSize.x;
    MaxY = ChangedSize.y;
    //float3 Dir = TopLeft;// +(RightStep * MinX) + (DownStep * MinY);
    float3 Dir = TopLeft + (RightStep * MinX) + (DownStep * MinY);
    


    float3 LastReflectionUV = {};
    float3 CurrentReflectionUV = {};
    float3 delta = {};
    float3 ReflectionUV;
    float SizeA = 32;
    float SizeB = 8;
    uint8 xStepLoop = 0;
    float xvalue = 0;
    float yvalue = 0;
    uint8 yStepLoop = 0;
    float w = engineState->skyFaces[0]->width;
    float h = engineState->skyFaces[0]->height;
    for (int yi = MinY; yi < MaxY; yi++)
    {
        for (int xi = MinX; xi < MaxX; xi++)
        {
            Dir += RightStep;
        
            if (xStepLoop == 0)
            {
                CurrentReflectionUV = LastReflectionUV;
                LastReflectionUV = RayCubemap(Dir + RightStep * SizeA);
                delta = (LastReflectionUV - CurrentReflectionUV) / SizeA;
                ReflectionUV = CurrentReflectionUV;
            }
            ReflectionUV += delta;
        
            if (LastReflectionUV.z != CurrentReflectionUV.z)
                ReflectionUV = RayCubemap(Dir);
            else
                ReflectionUV.z = LastReflectionUV.z;
        
            Image* image = engineState->skyFaces[(int)(ReflectionUV.z)];
            uint32 PixelX = ReflectionUV.x * w;
            uint32 PixelY = ReflectionUV.y * h;
        
            ((uint32*)videoBuffer->memory)[yi * videoBuffer->Width + xi] = image->ImageData[(PixelY)*image->width + (PixelX)];
        
        
            xStepLoop += SizeB;
        }
        
        if (yStepLoop == 0)
        {
            yvalue = 0;
        }
        yvalue += 1.0f / SizeA;
        
        yStepLoop += SizeB;
        xStepLoop = 0;
        
        // set dir here?
        float yy = (float)yi / (float)videoBuffer->Height;
        Dir = lerp(TopLeft, BottomLeft, yy);
        
    }

    ProfilerEndSample(&engineState->profilingData, "Sky");
}

float3 ToScreenPos(VideoBuffer* videoBuffer, float3 position, float3 CameraPos, float3 Forward, float3 Right, float3 Up)
{
    int sizeX = videoBuffer->Width;
    int sizeY = videoBuffer->Height;
    float3 LocalPos2 = float3(
        dot(CameraPos - position, Right),
        dot(CameraPos - position, Forward),
        dot(CameraPos - position, Up));
    float depth = -LocalPos2.y;
    int X = -(LocalPos2.x / depth) * sizeX * 0.5 + sizeX * 0.5f;
    int Y = (LocalPos2.z / depth) * sizeY * 1.58f + sizeY * 0.5f;
    return float3(X, Y, depth);
}

float3 ToScreenPos(VideoBuffer* videoBuffer, float3 position, Transform Camera)
{
    int sizeX = videoBuffer->Width;
    int sizeY = videoBuffer->Height;
    float3 LocalPos2 = float3(
        dot(Camera.position - position, Camera.right),
        dot(Camera.position - position, Camera.forward),
        dot(Camera.position - position, Camera.up));
    float depth = -LocalPos2.y;
    int X = -(LocalPos2.x / depth) * sizeX * 0.5 + sizeX * 0.5f;
    int Y = (LocalPos2.z / depth) * sizeY * 1.58f + sizeY * 0.5f;
    return float3(X, Y, depth);
}

float DrawLine3D(VideoBuffer* videoBuffer, Transform Camera, float3 Start, float3 End, float3 Color, bool zTest = true, float2 TestPoint = {})
{
    float AspectRatio = 0.5625f;
    float3 Up = Camera.up * AspectRatio;
    Transform CameraTransform = Camera;
    CameraTransform.up = Up;

    float3 Start2D = ToScreenPos(videoBuffer, Start, CameraTransform);
    float3 End2D = ToScreenPos(videoBuffer, End, CameraTransform);
    // behind the camera

    if( Start2D.x == End2D.x && 
        Start2D.y == End2D.y && 
        Start2D.z == End2D.z)
        return 99999999;

    // outside edges
    if ((Start2D.z < 0 || End2D.z < 0) || 
        (Start2D.x < 0 || Start2D.y < 0 || Start2D.y > videoBuffer->Height || Start2D.x > videoBuffer->Width) &&
        (End2D.y > videoBuffer->Height || End2D.x < 0 || End2D.y < 0 || End2D.x > videoBuffer->Width))
        return 99999999;

    float dist = distance(Start2D, End2D);
    // doing this was a really, *really* bad meme. made it lock up when running with OFast. do nooooot
    //for (float i = 0; i < 1.0f; i += (1.0f / dist))
    //{
    bool draw;
    for (float ii = 0; ii < dist; ii++)
    {
        float i = (float)ii / dist;

        float3 l = lerp(Start2D, End2D, i);
        if (!IsInScreen(videoBuffer, float2(l.x, l.y)))
            continue;
        float dist = l.z;
        uint32* Pixel = GetPixel(videoBuffer, l.x, l.y);
        if (DepthTest(dist, Pixel, l.x, &draw) && zTest)
        {
            continue;
        }
        SetColor(Pixel, MakeBufferColor(videoBuffer, Color));
    }

    return DistanceToLine(TestPoint, float2(Start2D.x, Start2D.y), float2(End2D.x, End2D.y));
}

float DrawLine3D(VideoBuffer* videoBuffer, Transform Camera, float3 Start, float3 End, bool zTest = true)
{
    return DrawLine3D(videoBuffer, Camera, Start, End, float3(1, 1, 1), zTest);
}

void DrawArrow3D(VideoBuffer* videoBuffer, Transform Camera, float3 Start, float3 End, float3 Color, bool zTest = true)
{
    DrawLine3D(videoBuffer, Camera, Start, End, Color, zTest);

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

    DrawLine3D(videoBuffer, Camera, delta * (1-PointerSize2) + Start + PointerSize * right, End, Color, zTest);
    DrawLine3D(videoBuffer, Camera, delta * (1-PointerSize2) + Start + PointerSize * forward, End, Color, zTest);
    DrawLine3D(videoBuffer, Camera, delta * (1-PointerSize2) + Start - PointerSize * right, End, Color, zTest);
    DrawLine3D(videoBuffer, Camera, delta * (1-PointerSize2) + Start - PointerSize * forward , End, Color, zTest);
}

void DrawPoint3D(VideoBuffer* videoBuffer, Transform Camera, float3 Pos, float size, float3 Color, bool zTest = true)
{
    DrawLine3D(videoBuffer, Camera, Pos + float3(size, 0, 0), Pos + float3(-size, 0, 0), zTest);
    DrawLine3D(videoBuffer, Camera, Pos + float3(0, size, 0), Pos + float3(0, -size, 0), zTest);
    DrawLine3D(videoBuffer, Camera, Pos + float3(0, 0, size), Pos + float3(0, 0, -size), zTest);
}

float DrawCircle3D(VideoBuffer* videoBuffer, Transform Camera, float3 Center, float3 Angle, float Radius, float3 Color, bool zTest = true, float2 TestPoint = {})
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
            NewDistance = DrawLine3D(videoBuffer, Camera, p + Center, lastP + Center, Color, zTest, TestPoint);

        if (NewDistance < TestPointDistance)
            TestPointDistance = NewDistance;
        lastP = p;
    }
    return TestPointDistance;
}

void DrawBox3D(VideoBuffer* videoBuffer, Transform Camera, Transform Box, float3 Color, bool zTest = true)
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

    DrawLine3D(videoBuffer, Camera, BoxScreenPos1, BoxScreenPos2,Color,zTest);
    DrawLine3D(videoBuffer, Camera, BoxScreenPos2, BoxScreenPos3,Color,zTest);
    DrawLine3D(videoBuffer, Camera, BoxScreenPos3, BoxScreenPos4,Color,zTest);
    DrawLine3D(videoBuffer, Camera, BoxScreenPos4, BoxScreenPos1,Color,zTest);
    DrawLine3D(videoBuffer, Camera, BoxScreenPos5, BoxScreenPos6,Color,zTest);
    DrawLine3D(videoBuffer, Camera, BoxScreenPos6, BoxScreenPos7,Color,zTest);
    DrawLine3D(videoBuffer, Camera, BoxScreenPos7, BoxScreenPos8,Color,zTest);
    DrawLine3D(videoBuffer, Camera, BoxScreenPos8, BoxScreenPos5,Color,zTest);
    DrawLine3D(videoBuffer, Camera, BoxScreenPos1, BoxScreenPos5,Color,zTest);
    DrawLine3D(videoBuffer, Camera, BoxScreenPos2, BoxScreenPos6,Color,zTest);
    DrawLine3D(videoBuffer, Camera, BoxScreenPos3, BoxScreenPos7,Color,zTest);
    DrawLine3D(videoBuffer, Camera, BoxScreenPos4, BoxScreenPos8,Color,zTest);
}

void DrawSphere3D(VideoBuffer* videoBuffer, Transform Camera, Transform Box, bool zTest = true)
{
    float3 BoxPos = Box.position;
    float3 BoxForward = Box.forward;
    float3 BoxUp = Box.up;
    float3 BoxRight = Box.right;
    float3 BoxScale = Box.scale;

    DrawCircle3D(videoBuffer, Camera, BoxPos, BoxForward, max(BoxScale), float3(1,1,1), zTest);
    DrawCircle3D(videoBuffer, Camera, BoxPos, BoxUp, max(BoxScale), float3(1, 1, 1), zTest);
    DrawCircle3D(videoBuffer, Camera, BoxPos, BoxRight, max(BoxScale), float3(1, 1, 1), zTest);
}

float3 ScreenPointToRay(VideoBuffer* videoBuffer, Transform Camera, float2 MousePos)
{
    float AspectRatio = 0.5625f;
    float3 Forward = Camera.forward;// CameraForward;
    float3 Right = Camera.right;//cross(CameraForward, CameraUp);
    float3 Up = Camera.up * AspectRatio;

    float3 TopLeft = ((Forward + -Right) + Up);
    float3 TopRight = ((Forward + Right) + Up);
    float3 BottomLeft = ((Forward + -Right) + -Up);
    float3 BottomRight = ((Forward + Right) + -Up);

    MousePos /= float2(videoBuffer->Width, videoBuffer->Height);

    return normalize(lerp(lerp(TopLeft, TopRight, MousePos.x), lerp(BottomLeft, BottomRight, MousePos.x), MousePos.y));
}

void DrawEffect(EngineState* engineState, VideoBuffer* videoBuffer, Transform Camera, RenderCommand* command, int Region = -1, int Regions = 9, int DrawIndex = 0)
{
    float AspectRatio = 0.5625f;
    float3 Up = Camera.up * AspectRatio;
    Transform CameraTransform = Camera;
    CameraTransform.up = Up;

    //float random = hash(DrawIndex);
    EffectData* effect = &command->effect;

    //float TestLoopTime = 4.0f;
    float Time = engineState->gameTime - effect->startTime;
    
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
        
        float3 CenterPoint = ToScreenPos(videoBuffer, position, CameraTransform);
        if (CenterPoint.z < 0)
            continue;

        float MinSize = min(videoBuffer->Width, videoBuffer->Height);
        float ScreenSize = MinSize / 500.0f;
        float size = ((100 * lerp(effect->startSize, effect->endSize, tNormalized)) / CenterPoint.z) * ScreenSize;
        float2 Pos = float2(CenterPoint.x, CenterPoint.y) - float2(size, size) * 0.5;
        float2 Size = float2(size, size);

        DrawSprite(engineState, videoBuffer, command->texture1, Pos, Size, command->color, command->shaderIndex, Region, Regions, CenterPoint.z);
    }
}

void DrawWireBox(EngineState* engineState, VideoBuffer* videoBuffer, Transform Camera, Transform Box, bool Sphere = false)
{
    float AspectRatio = 0.5625f;
    float3 Up = Camera.up * AspectRatio;
    Transform CameraTransform = Camera;
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
        DrawCircle3D(videoBuffer, Camera, BoxPos, BoxForward, max(BoxScale), float3(1,1,1));
        DrawCircle3D(videoBuffer, Camera, BoxPos, BoxUp, max(BoxScale), float3(1, 1, 1));
        DrawCircle3D(videoBuffer, Camera, BoxPos, BoxRight, max(BoxScale), float3(1, 1, 1));
    }
    else
    {
        DrawLine3D(videoBuffer, Camera, BoxScreenPos1, BoxScreenPos2);
        DrawLine3D(videoBuffer, Camera, BoxScreenPos2, BoxScreenPos3);
        DrawLine3D(videoBuffer, Camera, BoxScreenPos3, BoxScreenPos4);
        DrawLine3D(videoBuffer, Camera, BoxScreenPos4, BoxScreenPos1);
        DrawLine3D(videoBuffer, Camera, BoxScreenPos5, BoxScreenPos6);
        DrawLine3D(videoBuffer, Camera, BoxScreenPos6, BoxScreenPos7);
        DrawLine3D(videoBuffer, Camera, BoxScreenPos7, BoxScreenPos8);
        DrawLine3D(videoBuffer, Camera, BoxScreenPos8, BoxScreenPos5);
        DrawLine3D(videoBuffer, Camera, BoxScreenPos1, BoxScreenPos5);
        DrawLine3D(videoBuffer, Camera, BoxScreenPos2, BoxScreenPos6);
        DrawLine3D(videoBuffer, Camera, BoxScreenPos3, BoxScreenPos7);
        DrawLine3D(videoBuffer, Camera, BoxScreenPos4, BoxScreenPos8);
    }
}

float3 RasterizeTriangle(float2 pos, float2 a, float2 b, float2 c)
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

float edgeFunction(float2 a, float2 b, float2 c)
{
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}
float edgeFunction(float3 a, float3 b, float3 c)
{
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}
float3 triangle(float2 p, float2 v0, float2 v1, float2 v2)
{
    float w0 = edgeFunction(v1, v2, p);
    float w1 = edgeFunction(v2, v0, p);
    float w2 = edgeFunction(v0, v1, p);
    return float3(w0, w1, w2);
}
bool triangle(float2 p, float3 v0i, float3 v1i, float3 v2i, float area, float3* coords)
{
    float2 v0 = float2(v0i.x, v0i.y);
    float2 v1 = float2(v1i.x, v1i.y);
    float2 v2 = float2(v2i.x, v2i.y);

    float3 tr = triangle(p, v0, v1, v2);

    if (tr.x >= 0 && tr.y >= 0 && tr.z >= 0)
    {
        tr /= area;
        *coords = tr;
        return true;
    }
    else
    {
        return false;
    }
}

float3 RasterizeSquare(float2 pos, float2 a, float2 b, float2 c, float2 d)
{
    float3 h1 = RasterizeTriangle(pos, a, b, c);
    float3 h2 = RasterizeTriangle(pos, d, c, b);
    if (h1.z > 0)
        return h1;
    else if (h2.z > 0)
        return 1.0f - h2;
    else return float3(0, 0, 0);
}

float3 CalculateUV(float3 LocalPos, float3 LocalDir, float dist, float3 Normal, bool Sphere)
{
    float3 UV = float3(0, 0, 0);
    if (Sphere)
    {
        UV = RaySphereUV(Normal);
    }
    else
    {
        UV = RayBoxUV(LocalPos + LocalDir * dist, Normal);
    }
    return UV;
}

float3 CalculateLight(float3 Normal, float3 BoxRightTransposed, float3 BoxForwardTransposed, float3 BoxUpTransposed)
{
    // Transform the normal from local space to world space.
    float3 WorldNormal = float3(dot(Normal, BoxRightTransposed), dot(Normal, BoxForwardTransposed), dot(Normal, BoxUpTransposed));
    float NdotL = dot(WorldNormal, float3(-0.663791850916f, -0.4148699068f, 0.6223048602337667f));
    return float3(0.658f, 0.6431f, 0.549f) + float3(NdotL, NdotL, NdotL);
}

void DrawSphere(EngineState* engineState, VideoBuffer* videoBuffer, Transform Camera, Transform Box, Image* texture1, Image* texture2, int ShaderIndex, bool selected, bool hovered, int Region = -1, int Regions = 9)
{
    ProfilerBeingSample(&engineState->profilingData);

    float AspectRatio = 0.5625f;
    Transform CameraTransform = Camera;
    CameraTransform.up = CameraTransform.up * AspectRatio;

    float SphereScale = 0;

    SphereScale = max(Box.scale);
    Box.scale = float3(1, 1, 1);


    float depth = 1;
    int MinX = 0;
    int MaxX = 0;
    int MinY = 0;
    int MaxY = 0;

    float3 CenterPosScreenspace = ToScreenPos(videoBuffer, Box.position, CameraTransform);
    CenterPosScreenspace = (CenterPosScreenspace - float3(videoBuffer->Width / 2, videoBuffer->Height / 2, 0));
    CenterPosScreenspace.x *= 0.1f;
    CenterPosScreenspace.y *= 0.1f;

    float3 BoxScreenPos1 = ToScreenPos(videoBuffer, Box.position + normalize(CameraTransform.right) * 1.3 * SphereScale, CameraTransform);
    float3 BoxScreenPos2 = ToScreenPos(videoBuffer, Box.position - normalize(CameraTransform.right) * 1.3 * SphereScale, CameraTransform);
    float3 BoxScreenPos3 = ToScreenPos(videoBuffer, Box.position + normalize(CameraTransform.up) * 1.3 * SphereScale, CameraTransform);
    float3 BoxScreenPos4 = ToScreenPos(videoBuffer, Box.position - normalize(CameraTransform.up) * 1.3 * SphereScale, CameraTransform);
    float3 BoxScreenPos5 = ToScreenPos(videoBuffer, Box.position + normalize(CameraTransform.forward) * 1.3 * SphereScale, CameraTransform);
    float3 BoxScreenPos6 = ToScreenPos(videoBuffer, Box.position - normalize(CameraTransform.forward) * 1.3 * SphereScale, CameraTransform);

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

    int2 ChangedSize = GetThreadRegion(engineState, Regions, Region, videoBuffer->Height, MinY, MaxY);
    MinY = ChangedSize.x;
    MaxY = ChangedSize.y;

    if (MinX < 0)
        MinX = 0;
    if (MinY < 0)
        MinY = 0;

    if (MaxX > videoBuffer->Width)
        MaxX = videoBuffer->Width;
    if (MaxY > videoBuffer->Height)
        MaxY = videoBuffer->Height;

    // Set up camera frustum.. thing
    float3 TopLeft = normalize((CameraTransform.forward + -CameraTransform.right) + CameraTransform.up);
    float3 TopRight = normalize((CameraTransform.forward + CameraTransform.right) + CameraTransform.up);
    float3 BottomLeft = normalize((CameraTransform.forward + -CameraTransform.right) + -CameraTransform.up);
    float3 BottomRight = normalize((CameraTransform.forward + CameraTransform.right) + -CameraTransform.up);

    float3 RightOffset = (TopRight - TopLeft);
    float3 DownOffset = (BottomLeft - TopLeft);
    float3 RightStep = RightOffset / (videoBuffer->Width);
    float3 DownStep = DownOffset / (videoBuffer->Height);
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
        bool draw;
        for (int xi = MinX; xi < MaxX; xi++)
        {
            uint32* Pixel = &((uint32*)videoBuffer->memory)[yi * videoBuffer->Width + xi];
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

            if (DepthTest(dist, Pixel, xi, &draw))
            {
                xStepLoop += SizeB;
                xvalue += 1.0f / SizeA;
                continue;
            }

            uint32 TextureSample = ImageSample(texture1, UV.x, UV.y);
            float3 SurfaceColor = GetBufferColorReverse(videoBuffer, TextureSample);
            SurfaceColor = clamp01(SurfaceColor * Light);

            if (selected)
                SurfaceColor += float3(0.25f, 0.25f, 0.5f);
            else if (hovered)
                SurfaceColor += float3(0.05f, 0.05f, 0.15f);
            SurfaceColor = clamp01(SurfaceColor);

            uint32 NewSample = MakeBufferColor(videoBuffer, SurfaceColor);

            if (ShaderIndex == ShaderType_Additive)
            {
                float3 NewPixel = GetBufferColorReverse(videoBuffer, NewSample);
                float3 OldPixel = GetBufferColorReverse(videoBuffer, *Pixel);

                float3 Result = clamp01(NewPixel + OldPixel);

                uint32 ResultData = MakeBufferColor(videoBuffer, Result);

                SetColor(Pixel, ResultData);
            }
            else
            {
                SetColor(Pixel, NewSample);
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

void DrawBox(EngineState* engineState, VideoBuffer* videoBuffer, Transform Camera, Transform Box, Image* texture1, Image* texture2, int ShaderIndex, bool selected, bool hovered, int Region = -1, int Regions = 9)
{
    ProfilerBeingSample(&engineState->profilingData);

    float AspectRatio = 0.5625f;
    Transform CameraTransform = Camera;
    CameraTransform.up = CameraTransform.up * AspectRatio;

    float SphereScale = 0;

    float depth = 1;
    int MinX = 0;
    int MaxX = 0;
    int MinY = 0;
    int MaxY = 0;

    float3 BoxPosW1 = Box.position + ((-Box.right * Box.scale.x) + (-Box.forward * Box.scale.y) + (-Box.up * Box.scale.z)) * 0.5;
    float3 BoxPosW2 = Box.position + (( Box.right * Box.scale.x) + (-Box.forward * Box.scale.y) + (-Box.up * Box.scale.z)) * 0.5;
    float3 BoxPosW3 = Box.position + ((-Box.right * Box.scale.x) + ( Box.forward * Box.scale.y) + (-Box.up * Box.scale.z)) * 0.5;
    float3 BoxPosW4 = Box.position + (( Box.right * Box.scale.x) + ( Box.forward * Box.scale.y) + (-Box.up * Box.scale.z)) * 0.5;
    float3 BoxPosW5 = Box.position + ((-Box.right * Box.scale.x) + (-Box.forward * Box.scale.y) + (Box.up  * Box.scale.z)) * 0.5;
    float3 BoxPosW6 = Box.position + (( Box.right * Box.scale.x) + (-Box.forward * Box.scale.y) + (Box.up  * Box.scale.z)) * 0.5;
    float3 BoxPosW7 = Box.position + ((-Box.right * Box.scale.x) + ( Box.forward * Box.scale.y) + (Box.up  * Box.scale.z)) * 0.5;
    float3 BoxPosW8 = Box.position + (( Box.right * Box.scale.x) + ( Box.forward * Box.scale.y) + (Box.up  * Box.scale.z)) * 0.5;

    float3 BoxScreenPos1 = ToScreenPos(videoBuffer, BoxPosW1, CameraTransform);
    float3 BoxScreenPos2 = ToScreenPos(videoBuffer, BoxPosW2, CameraTransform);
    float3 BoxScreenPos3 = ToScreenPos(videoBuffer, BoxPosW3, CameraTransform);
    float3 BoxScreenPos4 = ToScreenPos(videoBuffer, BoxPosW4, CameraTransform);
    float3 BoxScreenPos5 = ToScreenPos(videoBuffer, BoxPosW5, CameraTransform);
    float3 BoxScreenPos6 = ToScreenPos(videoBuffer, BoxPosW6, CameraTransform);
    float3 BoxScreenPos7 = ToScreenPos(videoBuffer, BoxPosW7, CameraTransform);
    float3 BoxScreenPos8 = ToScreenPos(videoBuffer, BoxPosW8, CameraTransform);

    MinX = videoBuffer->Width;
    MaxX = 0;
    MinY = videoBuffer->Height;
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

    int2 ChangedSize = GetThreadRegion(engineState, Regions, Region, videoBuffer->Height, MinY, MaxY);
    MinY = ChangedSize.x;
    MaxY = ChangedSize.y;

    if (MinX < 0)
        MinX = 0;
    if (MinY < 0)
        MinY = 0;

    if (MaxX > videoBuffer->Width)
        MaxX = videoBuffer->Width;
    if (MaxY > videoBuffer->Height)
        MaxY = videoBuffer->Height;

    // Set up camera frustum.. thing
    float3 TopLeft = normalize((CameraTransform.forward + -CameraTransform.right) + CameraTransform.up);
    float3 TopRight = normalize((CameraTransform.forward + CameraTransform.right) + CameraTransform.up);
    float3 BottomLeft = normalize((CameraTransform.forward + -CameraTransform.right) + -CameraTransform.up);
    float3 BottomRight = normalize((CameraTransform.forward + CameraTransform.right) + -CameraTransform.up);

    float3 RightOffset = (TopRight - TopLeft);
    float3 DownOffset = (BottomLeft - TopLeft);
    float3 RightStep = RightOffset / (videoBuffer->Width);
    float3 DownStep = DownOffset / (videoBuffer->Height);
    float3 Dir = TopLeft + (RightStep * MinX) + (DownStep * MinY);

    float SizeA = 16;
    float SizeB = 256 / SizeA;
    int8 xStepLoop = 0;
    int8 yStepLoop = 0;
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

            uint32* Pixel = &((uint32*)videoBuffer->memory)[yi * videoBuffer->Width + xi];
            
            if (DepthTest(dist, Pixel, xi, &draw))
            {
                xStepLoop += SizeB;
                xvalue += 1.0f / SizeA;
                continue;
            }

            uint32 TextureSample = ImageSample(texture1, UV.x, UV.y);
#if 1
            ((uint8*)Pixel)[0] = ((uint8*)&TextureSample)[0] ;//* clamp01(Light.z);
            ((uint8*)Pixel)[1] = ((uint8*)&TextureSample)[1] ;//* clamp01(Light.y);
            ((uint8*)Pixel)[2] = ((uint8*)&TextureSample)[2] ;//* clamp01(Light.x);
#else
            float3 SurfaceColor = GetBufferColorReverse(videoBuffer, TextureSample);
            SurfaceColor = clamp01(SurfaceColor * Light);

            if (selected)
                SurfaceColor += float3(0.25f, 0.25f, 0.5f);
            else if (hovered)
                SurfaceColor += float3(0.05f, 0.05f, 0.15f);
            SurfaceColor = clamp01(SurfaceColor);

            uint32 NewSample = MakeBufferColor(videoBuffer, SurfaceColor);
            SetColor(Pixel, NewSample);
#endif

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

const int count = 2000;
int index;
int64 DrawTimes[count];

void DrawSceneRegion(int thread, GameMemory* memory, VideoBuffer* videoBuffer, game_sound_output_buffer* SoundBuffer)
{
    EngineState* engineState = (EngineState*)memory->transientStorage;
    EngineSaveState* engineSaveState = (EngineSaveState*)memory->permanentStorage;
    int64 start = memory->platformTime();

    // TODO: Implement platform independnat 'clocks' function for maximum accuracy
    uint64 startClocks = 0;//__rdtsc();

    int regionCount = engineSaveState->screenThreads;
    for (int i = 0; i < ArrayCount(engineState->renderCommands); i++)
    {
        RenderCommand command = engineState->renderCommands[i];
        if (command.Type == RenderCommandType_Sphere)
        {
            DrawSphere(engineState, videoBuffer, engineState->camera, command.t, command.texture1, command.texture2, command.shaderIndex, command.selected, command.hovered, thread, regionCount);
        }
        else if (command.Type == RenderCommandType_Box)
        {
            DrawBox(engineState, videoBuffer, engineState->camera, command.t, command.texture1, command.texture2, command.shaderIndex, command.selected, command.hovered, thread, regionCount);
        }
        else if (command.Type == RenderCommandType_Skybox)
        {
            DrawSkybox(engineState, videoBuffer, engineState->camera, thread, regionCount);
        }
        else if (command.Type == RenderCommandType_Clear)
        {
            DrawRectangle(videoBuffer, float2(0, 0), float2(1920 / 2, 1080 / 2), command.color);
        }
    }
    int64 end = memory->platformTime();
    
    // TODO: Implement platform independnat 'clocks' function for maximum accuracy
    uint64 endClocks = 0;//__rdtsc();

    if (engineSaveState->profilingEnabled)
    {
        index++;
        if (index > count)
        {
            index = 0;
        }
        DrawTimes[index] = (endClocks - startClocks);
        int64 DrawTime = 0;
        for (int i = 0; i < count; i++)
        {
            DrawTime += DrawTimes[i];
        }
        DrawTime /= count;

        int dt = (int)DrawTime;
        char text[100] = {};
        Append(text, "Thread: ");
        Append(text, thread);
        Append(text, ", Time: ");
        Append(text, dt);
        Append(text, "clocks");
        DrawFont(engineState->fontSpritesheet, videoBuffer, 200, thread * 12 + 160, text);
    }

    // draw particles last so sorting works
    for (int i = 0; i < ArrayCount(engineState->renderCommands); i++)
    {
        RenderCommand command = engineState->renderCommands[i];
        if (command.Type == RenderCommandType_Effect)
        {
            DrawEffect(engineState, videoBuffer, engineState->camera, &command, thread, regionCount);
        }
    }

    int MinX = 0;
    int MinY = 0;
    int MaxX = videoBuffer->Width;
    int MaxY = videoBuffer->Height;
    int2 ChangedSize = GetThreadRegion(engineState, regionCount, thread, videoBuffer->Height, MinY, MaxY);
    MinY = ChangedSize.x;
    MaxY = ChangedSize.y;
    if (engineSaveState->profilingEnabled && engineSaveState->threadedRendering && engineSaveState->visualize_ThreadedRendering)
    {
        DrawRectangle(videoBuffer, float2(MinX, MinY + 1), float2(videoBuffer->Width, MaxY - MinY - 1), frac(float3(thread / 3.0f, thread / 6.0f, thread / 12.0f)), 0.2f);
        DrawLine(videoBuffer, float2(0, engineState->renderThreadEnd[thread] * videoBuffer->Height), float2(videoBuffer->Width, engineState->renderThreadEnd[thread] * videoBuffer->Height));
    }
}

// draws the scene, threading and all!
void DrawScene(GameMemory* memory, EngineState* engineState, EngineSaveState* engineSaveState, VideoBuffer* videoBuffer)
{
    // Consume draw queue
    if (engineSaveState->threadedRendering)
    {
        for (int i = 0; i < engineSaveState->screenThreads; i++)
        {
            memory->platformThreadCall(&DrawSceneRegion, i);
        }
        for (int i = 0; i < ArrayCapacity(engineState->renderThreadTimes); i++)
        {
            engineState->renderThreadTimes[i] = 0;
        }

        // Spinlock until they are all done.
        int count = 0;
        while (true)
        {
            count++;
            bool AnyExecuting = false;
            for (int i = 0; i < engineSaveState->screenThreads; i++)
            {
                if (memory->threadsExecuting[i] == true)
                {
                    AnyExecuting = true;
                }
                else
                {
                    if (engineState->renderThreadTimes[i] == 0)
                        engineState->renderThreadTimes[i] = count;
                }
            }

            // If no threads are executing, we are done.
            if (!AnyExecuting)
                break;
        }

        if (engineSaveState->profilingEnabled && engineSaveState->visualize_ThreadedRendering)
        {
            float2 start = float2(210, videoBuffer->Height - 190);
            DrawRectangle(videoBuffer, start - 5, float2(700, engineSaveState->screenThreads * 5 + 10), float3(0.0, 0.0, 0.0), 0.5);
            for (int i = 0; i < engineSaveState->screenThreads; i++)
            {
                float2 innerStart = float2(0, i * 5);
                DrawLine(videoBuffer, start + innerStart, start + innerStart + float2(engineState->renderThreadTimes[i] / 5000, 0));
            }
        }

        if (engineSaveState->threadedRendering_Dynamic)
        {
            int sum = 0;
            for (int i = 0; i < engineSaveState->screenThreads; i++)
            {
                sum += engineState->renderThreadTimes[i];
            }
            int average = sum / engineSaveState->screenThreads;

            for (int i = 0; i < engineSaveState->screenThreads; i++)
            {
                if (engineState->renderThreadTimes[i] > average) // Took longer, reduce its size.
                {
                    engineState->renderThreadSize[i] *= 0.95f;
                }
                else // was faster, increase its size.
                {
                    engineState->renderThreadSize[i] *= 1.05f;
                }

                if (engineState->renderThreadSize[i] < 0.01f)
                    engineState->renderThreadSize[i] = 0.01f;
                else if (engineState->renderThreadSize[i] > 0.2f)
                    engineState->renderThreadSize[i] = 0.2f;
            }

            float sizeAverage = 0;
            for (int i = 0; i < engineSaveState->screenThreads; i++)
            {
                sizeAverage += engineState->renderThreadSize[i];
            }
            for (int i = 0; i < engineSaveState->screenThreads; i++)
            {
                engineState->renderThreadSize[i] /= sizeAverage;
            }
            float h = 0;
            for (int i = 0; i < engineSaveState->screenThreads; i++)
            {
                float lastH = h;
                h += engineState->renderThreadSize[i];
                engineState->renderThreadStart[i] = lastH;
                engineState->renderThreadEnd[i] = h;
            }
        }
        else
        {
            float h = 0;
            for (int i = 0; i < engineSaveState->screenThreads; i++)
            {
                float lastH = h;
                h += 1.0f / engineSaveState->screenThreads;
                engineState->renderThreadStart[i] = lastH;
                engineState->renderThreadEnd[i] = h;
            }
        }
    }
    else
    {
        DrawSceneRegion(-1, memory, videoBuffer, 0);
    }
}