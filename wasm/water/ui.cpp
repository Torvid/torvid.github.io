#pragma once
#include "quote.h"

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

void Text(EngineState* engineState, VideoBuffer* videoBuffer, float2 Pos, const char* text)
{
    char String[100] = {};
    Append(String, text);
    DrawFont(engineState->fontSpritesheet, videoBuffer, Pos.x, Pos.y, String);
}
void Text(EngineState* engineState, VideoBuffer* videoBuffer, float2* Pos, const char* text)
{
    Text(engineState, videoBuffer, *Pos, text);
    Pos->y += 11;
}
void Text(EngineState* engineState, VideoBuffer* videoBuffer, float2* Pos, const char* text, float value)
{
    char String[100] = {};
    Append(String, text);
    Append(String, value);
    Text(engineState, videoBuffer, Pos, String);
}
void Text(EngineState* engineState, VideoBuffer* videoBuffer, float2 Pos, const char* text, float value)
{
    char String[100] = {};
    Append(String, text);
    Append(String, value);
    Text(engineState, videoBuffer, Pos, String);
}
void Text(EngineState* engineState, VideoBuffer* videoBuffer, float2* Pos, const char* text, int value)
{
    char String[100] = {};
    Append(String, text);
    Append(String, value);
    Text(engineState, videoBuffer, Pos, String);
}
void Text(EngineState* engineState, VideoBuffer* videoBuffer, float2* Pos, const char* text, float3 value)
{
    char String[100] = {};
    Append(String, text);
    Append(String, value);
    Text(engineState, videoBuffer, Pos, String);
}

float TextWidth(const char* text)
{
    return StringLength(text) * 6;
}

bool Button(EngineState* engineState, GameInput* Input, VideoBuffer* videoBuffer, float2 Pos, const char* text)
{
    Pos += float2(0, 4);
    float2 Size = float2(TextWidth(text) + 15, 17);
    float2 MousePos = float2(Input->MouseX, Input->MouseY);
    if (BoxContains(Pos, Size, MousePos))
    {
        engineState->mouseIsOverUI = true;
        if (Input->MouseLeft)
        {
            DrawRectangle(videoBuffer, Pos, Size, float3(0.8, 0.8, 0.8));
        }
        else
        {
            DrawRectangle(videoBuffer, Pos, Size, float3(0.6, 0.6, 0.6));
        }
        if (Input->MouseLeftUp)
        {
            return true;
        }
    }
    else
    {
        DrawRectangle(videoBuffer, Pos, Size, float3(0.5, 0.5, 0.5));
    }

    DrawFont(engineState->fontSpritesheet, videoBuffer, Pos.x + 4, Pos.y + 1, text);
    return false;
}

bool Button(EngineState* engineState, GameInput* Input, VideoBuffer* videoBuffer, float2* Pos, const char* text)
{
    bool result = Button(engineState, Input, videoBuffer, *Pos, text);
    Pos->y += 20;
    return result;
}


const float3 PressColor = float3(0.8, 0.8, 0.8);
const float3 HoverColor = float3(0.6, 0.6, 0.6);
const float3 IdleColor = float3(0.5, 0.5, 0.5);

const float3 PressColorBright = float3(1.0, 1.0, 1.0);
const float3 HoverColorBright = float3(0.95, 0.95, 0.95);
const float3 IdleColorBright = float3(0.9, 0.9, 0.9);

void DrawHoverRectangle(VideoBuffer* videoBuffer,
    float2 Pos,
    float2 Size, bool Hovered, bool MouseLeft)
{
    if (Hovered)
    {
        if (MouseLeft)
            DrawRectangle(videoBuffer, Pos, Size, PressColor);
        else
            DrawRectangle(videoBuffer, Pos, Size, HoverColor);
    }
    else
        DrawRectangle(videoBuffer, Pos, Size, IdleColor);
}

void DrawHoverRectangleBright(VideoBuffer* videoBuffer,
    float2 Pos,
    float2 Size, bool Hovered, bool MouseLeft)
{
    if (Hovered)
    {
        if (MouseLeft)
            DrawRectangle(videoBuffer, Pos, Size, PressColorBright);
        else
            DrawRectangle(videoBuffer, Pos, Size, HoverColorBright);
    }
    else
        DrawRectangle(videoBuffer, Pos, Size, IdleColorBright);
}
bool Toggle(EngineState* engineState, GameInput* Input, VideoBuffer* videoBuffer, float2 Pos, bool Current, const char* text)
{
    float2 Size = float2(12, 12);
    float2 MousePos = float2(Input->MouseX, Input->MouseY);
    Pos += float2(0, 4);
    bool Hovered = BoxContains(Pos, Size + float2(TextWidth(text), 0), MousePos);
    DrawHoverRectangle(videoBuffer, Pos, Size, Hovered, Input->MouseLeft);

    if (Hovered)
    {
        engineState->mouseIsOverUI = true;
        if (Input->MouseLeftUp)
        {
            return !Current;
        }
    }
    if (Current)
    {
        DrawHoverRectangleBright(videoBuffer, Pos + float2(3, 3), Size - float2(6, 6), Hovered, Input->MouseLeft);
    }
    DrawFont(engineState->fontSpritesheet, videoBuffer, Pos.x + 17, Pos.y + 0, text);
    return Current;
}

bool Toggle(EngineState* engineState, GameInput* Input, VideoBuffer* videoBuffer, float2* Pos, bool Current, const char* text)
{
    bool result = Toggle(engineState, Input, videoBuffer, *Pos, Current, text);
    Pos->y += 16;
    return result;
}
bool Toggle(EngineState* engineState, GameInput* Input, VideoBuffer* videoBuffer, float2* Pos, bool Current, const char* text, float2 offset)
{
    bool result = Toggle(engineState, Input, videoBuffer, *Pos + offset, Current, text);
    Pos->y += 20;
    return result;
}

float Slider(EngineState* engineState, GameInput* Input, VideoBuffer* videoBuffer, float2 Pos, float Current, float min, float max, float barwidth = 120)
{
    engineState->sliderID++;
    if (Input->MouseLeftUp)
    {
        engineState->pickedSliderID = -1;
    }

    float2 BoundsPos = Pos;
    float2 BoundsSize = float2(barwidth, 24);

    Pos += float2(0, 12);

    DrawRectangle(videoBuffer, Pos, float2(barwidth, 4), float3(0.25, 0.25, 0.25));
    float CurrentNormalized = clamp01((Current - min) / (max - min));

    float2 SliderPos = Pos - float2(2, 6) + float2(barwidth * CurrentNormalized, 0);
    float2 SliderSize = float2(8, 16);
    float2 MousePos = float2(Input->MouseX, Input->MouseY);
    bool Hovered = BoxContains(BoundsPos, BoundsSize, MousePos);
    DrawHoverRectangle(videoBuffer, SliderPos, SliderSize, Hovered, Input->MouseLeft);
    if (Hovered)
        engineState->mouseIsOverUI = true;

    if (Hovered && Input->MouseLeftDown)
    {
        engineState->pickedSliderID = engineState->sliderID;
    }
    if (Input->MouseLeftUp)
    {
        engineState->pickedSliderID = -1;
    }
    if (engineState->pickedSliderID == engineState->sliderID && Input->MouseLeft)
    {
        Current = (((float)Input->MouseX - Pos.x) / barwidth) * (max - min) + min;
    }
    Current = clamp(Current, min, max);
    return Current;
}

float Slider(EngineState* engineState, GameInput* Input, VideoBuffer* videoBuffer, float2* Pos, float Current, float min, float max, float2 offset = {}, float barwidth = 120)
{
    float result = Slider(engineState, Input, videoBuffer, *Pos + offset, Current, min, max, barwidth);
    Pos->y += 20;
    return result;
}

float GetLineCloseness(GameInput* Input, VideoBuffer* videoBuffer, Transform Camera, float3 Start, float3 End)
{
    float3 Current = Start;
    float3 Dir = End - Start;

    float3 Ray = ScreenPointToRay(videoBuffer, Camera, float2(Input->MouseX, Input->MouseY));
    float3 Hit = ClosestRayApproach(Camera.position, Ray, Current, Dir);

    float3 PointToCamera = normalize(Hit - Camera.position);

    //float a = clamp01((1.0f - dot(PointToCamera, Ray)) * 25 * dot((Hit - Camera.position), (Hit - Camera.position)));
    float a = ((1.0f - dot(PointToCamera, Ray)) * 25 * dot((Hit - Camera.position), (Hit - Camera.position)));
    float b = ClosestRayDistance(Camera.position, Ray, Current, Dir);
    //b = clamp01(abs((b - 0.5f) * 2.0f) - 1.0f);
    b = (abs((b - 0.5f) * 2.0f) - 1.0f);
    float closeness = max(a, b);
    return closeness;
}

int IDFromNumbers(float distX, float distY, float DistZ, float Closeness)
{
    if (distX < distY && distX < DistZ && distX < Closeness) // 1 smallest
    {
        return 0;
    }
    else if (distY < DistZ && distY < Closeness) // 2 smallest
    {
        return 1;
    }
    else if (DistZ < Closeness) // 3 smallest
    {
        return 2;
    }
    else
    {
        return -1;
    }
}

int GetGizmoClickID(GameInput* Input, VideoBuffer* videoBuffer, Transform Camera, float3 Start, float3 X, float3 Y, float3 Z)
{
    float distX = GetLineCloseness(Input, videoBuffer, Camera, Start, Start + X);
    float distY = GetLineCloseness(Input, videoBuffer, Camera, Start, Start + Y);
    float DistZ = GetLineCloseness(Input, videoBuffer, Camera, Start, Start + Z);

    float3 Ray = ScreenPointToRay(videoBuffer, Camera, float2(Input->MouseX, Input->MouseY));
    //float3 Dir = X - Start;

    return IDFromNumbers(distX, distY, DistZ, 0.25f);
}

float3 GetGizmoClickPoint(GameInput* Input, VideoBuffer* videoBuffer, Transform Camera, float3 Start, float3 X, float3 Y, float3 Z, float2 MousePos, int ID)
{
    float3 Ray = ScreenPointToRay(videoBuffer, Camera, MousePos);
    if (ID == 0)
    {
        return ClosestRayApproach(Camera.position, Ray, Start, X);
    }
    else if (ID == 1)
    {
        return ClosestRayApproach(Camera.position, Ray, Start, Y);
    }
    else if (ID == 2)
    {
        return ClosestRayApproach(Camera.position, Ray, Start, Z);
    }
    return float3(0, 0, 0);
}

// TODO: Change the picking to use the 2D distance instead of the current 3D intersection thingy.
float3 MoveGizmo(EngineState* engineState, GameInput* Input, VideoBuffer* videoBuffer, Transform Camera, Transform Current, bool WorldSpace)
{
    engineState->sliderID++;
    if (Input->MouseLeftUp)
    {
        engineState->pickedSliderID = -1;
    }

    float length = 1.0f;

    float3 X = float3(length, 0, 0);
    float3 Y = float3(0, length, 0);
    float3 Z = float3(0, 0, length);
    if (WorldSpace)
    {
        X = Current.right * length;
        Y = Current.forward * length;
        Z = Current.up * length;
    }

    int CurrentID = GetGizmoClickID(Input, videoBuffer, Camera, Current.position, X, Y, Z);

    bool Hovered = !(CurrentID == -1);
    bool Dragging = !(engineState->moveGizmoAxisID == -1);
    if (Hovered && Input->MouseLeftDown)
    {
        engineState->pickedSliderID = engineState->sliderID;
        engineState->moveGizmoAxisID = CurrentID;
    }
    if (Hovered && Input->MouseLeftUp)
    {
        engineState->operationHappened = true;
    }
    if (Input->MouseLeftUp)
    {
        engineState->pickedSliderID = -1;
        engineState->moveGizmoAxisID = -1;
    }

    DrawArrow3D(videoBuffer, Camera, Current.position, Current.position + X, (Dragging ? engineState->moveGizmoAxisID : CurrentID) == 0 ? float3(1, 1, 1) : float3(1, 0, 0), false);
    DrawArrow3D(videoBuffer, Camera, Current.position, Current.position + Y, (Dragging ? engineState->moveGizmoAxisID : CurrentID) == 1 ? float3(1, 1, 1) : float3(0, 1, 0), false);
    DrawArrow3D(videoBuffer, Camera, Current.position, Current.position + Z, (Dragging ? engineState->moveGizmoAxisID : CurrentID) == 2 ? float3(1, 1, 1) : float3(0, 0, 1), false);

    float3 LastClickPoint = GetGizmoClickPoint(Input, videoBuffer, Camera, Current.position, X, Y, Z, float2(Input->MouseX - Input->MouseXDelta, Input->MouseY - Input->MouseYDelta), engineState->moveGizmoAxisID);
    float3 ClickPoint = GetGizmoClickPoint(Input, videoBuffer, Camera, Current.position, X, Y, Z, float2(Input->MouseX, Input->MouseY), engineState->moveGizmoAxisID);

    float3 Offset = float3(0, 0, 0);

    if (Dragging && Input->MouseLeft)
        Offset += ClickPoint - LastClickPoint;
    //DrawPoint3D(videoBuffer, Camera, ClickPoint, 0.1f, float3(1, 1, 1), false);
    //float2 a = float2(400, 400);
    //Text(engineState, videoBuffer, &a, "MoveGizmoAxisID: ", engineState->MoveGizmoAxisID);
    //Text(engineState, videoBuffer, &a, "CurrentID: ", CurrentID);
    if (Input->MouseMiddle)
    {
        float MoveSpeed = 0.002;
        Offset += Input->MouseXDelta * Camera.right * MoveSpeed * distance(Camera.position, Current.position);
        Offset += -Input->MouseYDelta * Camera.up * MoveSpeed * distance(Camera.position, Current.position);
    }
    if (Input->MouseMiddleUp)
    {
        engineState->operationHappened = true;
    }

    return Offset;
}

float3 GetRotateGizmoClickPoint(GameInput* Input, VideoBuffer* videoBuffer, Transform Camera, float3 Start, float3 X, float3 Y, float3 Z, float2 MousePos, int ID)
{
    float3 Ray = ScreenPointToRay(videoBuffer, Camera, MousePos);
    if (ID == 0)
    {
        return RayPlaneIntersectWorldSpace(Camera.position, Ray, Start, X);
    }
    else if (ID == 1)
    {
        return RayPlaneIntersectWorldSpace(Camera.position, Ray, Start, Y);
    }
    else if (ID == 2)
    {
        return RayPlaneIntersectWorldSpace(Camera.position, Ray, Start, Z);
    }
}

struct Rotation
{
    float3 Axis;
    float Angle;
};

Rotation RotateGizmo(EngineState* engineState, GameInput* Input, VideoBuffer* videoBuffer, Transform Camera, Transform Current, bool WorldSpace)
{
    engineState->sliderID++;
    float2 MousePos = float2(Input->MouseX, Input->MouseY);
    float length = 1.0f;
    float3 X = float3(length, 0, 0);
    float3 Y = float3(0, length, 0);
    float3 Z = float3(0, 0, length);
    if (WorldSpace)
    {
        X = Current.right * length;
        Y = Current.forward * length;
        Z = Current.up * length;
    }

    float distX = DrawCircle3D(videoBuffer, Camera, Current.position, X, 1, float3(1, 1, 1), false, MousePos);
    float distY = DrawCircle3D(videoBuffer, Camera, Current.position, Y, 1, float3(1, 1, 1), false, MousePos);
    float distZ = DrawCircle3D(videoBuffer, Camera, Current.position, Z, 1, float3(1, 1, 1), false, MousePos);
    int CurrentID = IDFromNumbers(distX, distY, distZ, 25);
    bool Hovered = !(CurrentID == -1);
    bool Dragging = !(engineState->moveGizmoAxisID == -1);
    if (Hovered && Input->MouseLeftDown)
    {
        engineState->pickedSliderID = engineState->sliderID;
        engineState->moveGizmoAxisID = CurrentID;
    }
    if (Hovered && Input->MouseLeftUp)
    {
        engineState->operationHappened = true;
    }
    if (Input->MouseLeftUp)
    {
        engineState->pickedSliderID = -1;
        engineState->moveGizmoAxisID = -1;
    }

    DrawCircle3D(videoBuffer, Camera, Current.position, X, 1, (Dragging ? engineState->moveGizmoAxisID : CurrentID) == 0 ? float3(1, 1, 1) : float3(1, 0, 0), false, MousePos);
    DrawCircle3D(videoBuffer, Camera, Current.position, Y, 1, (Dragging ? engineState->moveGizmoAxisID : CurrentID) == 1 ? float3(1, 1, 1) : float3(0, 1, 0), false, MousePos);
    DrawCircle3D(videoBuffer, Camera, Current.position, Z, 1, (Dragging ? engineState->moveGizmoAxisID : CurrentID) == 2 ? float3(1, 1, 1) : float3(0, 0, 1), false, MousePos);

    float3 LastClickPoint = GetRotateGizmoClickPoint(Input, videoBuffer, Camera, Current.position, X, Y, Z, float2(Input->MouseX - Input->MouseXDelta, Input->MouseY - Input->MouseYDelta), engineState->moveGizmoAxisID);
    float3 ClickPoint = GetRotateGizmoClickPoint(Input, videoBuffer, Camera, Current.position, X, Y, Z, float2(Input->MouseX, Input->MouseY), engineState->moveGizmoAxisID);

    //DrawPoint3D(videoBuffer, Camera, ClickPoint, 0.1f, float3(1, 1, 1), false);
    float2 a = float2(500, 400);
    float3 LocalClickPoint = ClickPoint - Current.position;
    float3 LocalLastClickPoint = LastClickPoint - Current.position;
    float XAngle = AngleBetween(float2(LocalClickPoint.y, LocalClickPoint.z), float2(LocalLastClickPoint.y, LocalLastClickPoint.z));
    float YAngle = AngleBetween(float2(LocalClickPoint.z, LocalClickPoint.x), float2(LocalLastClickPoint.z, LocalLastClickPoint.x));
    float ZAngle = AngleBetween(float2(LocalClickPoint.x, LocalClickPoint.y), float2(LocalLastClickPoint.x, LocalLastClickPoint.y));
    //Text(engineState, videoBuffer, &a, "dist1: ", XAngle);
    //Text(engineState, videoBuffer, &a, "dist2: ", YAngle);
    //Text(engineState, videoBuffer, &a, "dist3: ", ZAngle);
    Rotation r = {};
    r.Axis = float3(1, 0, 0);
    r.Angle = 0;

    if (engineState->moveGizmoAxisID == 0)
    {
        r.Axis = X;
        r.Angle = XAngle;
    }
    else if (engineState->moveGizmoAxisID == 1)
    {
        r.Axis = Y;
        r.Angle = YAngle;
    }
    else if (engineState->moveGizmoAxisID == 2)
    {
        r.Axis = Z;
        r.Angle = ZAngle;
    }

    return r;
}

float3 ScaleGizmo(EngineState* engineState, GameInput* Input, VideoBuffer* videoBuffer, Transform Camera, Transform Current)
{
    engineState->sliderID++;
    if (Input->MouseLeftUp)
    {
        engineState->pickedSliderID = -1;
    }

    float length = 1.0f;

    float3 X = Current.right * Current.scale.x * 0.5;
    float3 Y = Current.forward * Current.scale.y * 0.5;
    float3 Z = Current.up * Current.scale.z * 0.5;

    int CurrentID = GetGizmoClickID(Input, videoBuffer, Camera, Current.position, X, Y, Z);

    bool Hovered = !(CurrentID == -1);
    bool Dragging = !(engineState->moveGizmoAxisID == -1);
    if (Hovered && Input->MouseLeftDown)
    {
        engineState->pickedSliderID = engineState->sliderID;
        engineState->moveGizmoAxisID = CurrentID;
    }
    if (Hovered && Input->MouseLeftUp)
    {
        engineState->operationHappened = true;
    }
    if (Input->MouseLeftUp)
    {
        engineState->pickedSliderID = -1;
        engineState->moveGizmoAxisID = -1;
    }

    float3 XColor = (Dragging ? engineState->moveGizmoAxisID : CurrentID) == 0 ? float3(1, 1, 1) : float3(1, 0, 0);
    float3 YColor = (Dragging ? engineState->moveGizmoAxisID : CurrentID) == 1 ? float3(1, 1, 1) : float3(0, 1, 0);
    float3 ZColor = (Dragging ? engineState->moveGizmoAxisID : CurrentID) == 2 ? float3(1, 1, 1) : float3(0, 0, 1);

    DrawLine3D(videoBuffer, Camera, Current.position, Current.position + X, XColor, false);
    DrawLine3D(videoBuffer, Camera, Current.position, Current.position + Y, YColor, false);
    DrawLine3D(videoBuffer, Camera, Current.position, Current.position + Z, ZColor, false);
    //float3(0.01, Current.scale.y, Current.scale.z) * 0.75)
    //float3(Current.scale.x, 0.01, Current.scale.z) * 0.75)
    //float3(Current.scale.x, Current.scale.y, 0.01) * 0.75)
    DrawBox3D(videoBuffer, Camera, transform(Current.position + X, normalize(Y), normalize(Z), float3(0.1, 0.1, 0.1)), XColor, false);
    DrawBox3D(videoBuffer, Camera, transform(Current.position + Y, normalize(Y), normalize(Z), float3(0.1, 0.1, 0.1)), YColor, false);
    DrawBox3D(videoBuffer, Camera, transform(Current.position + Z, normalize(Y), normalize(Z), float3(0.1, 0.1, 0.1)), ZColor, false);

    float3 LastClickPoint = GetGizmoClickPoint(Input, videoBuffer, Camera, Current.position, X, Y, Z, float2(Input->MouseX - Input->MouseXDelta, Input->MouseY - Input->MouseYDelta), engineState->moveGizmoAxisID);
    float3 ClickPoint = GetGizmoClickPoint(Input, videoBuffer, Camera, Current.position, X, Y, Z, float2(Input->MouseX, Input->MouseY), engineState->moveGizmoAxisID);

    float3 Offset = float3(0, 0, 0);

    if (Dragging && Input->MouseLeft)
        Offset += WorldToLocalVectorNoScale(Current, ClickPoint - LastClickPoint);
    //DrawPoint3D(videoBuffer, Camera, ClickPoint, 0.1f, float3(1, 1, 1), false);
    //float2 a = float2(400, 400);
    //Text(engineState, videoBuffer, &a, "MoveGizmoAxisID: ", engineState->MoveGizmoAxisID);
    //Text(engineState, videoBuffer, &a, "CurrentID: ", CurrentID);
    //if (Input->MouseMiddle)
    //{
    //    float MoveSpeed = 0.01;
    //    Offset += Input->MouseXDelta * Camera.right * MoveSpeed;
    //    Offset += -Input->MouseYDelta * Camera.up * MoveSpeed;
    //}

    return Offset;
}