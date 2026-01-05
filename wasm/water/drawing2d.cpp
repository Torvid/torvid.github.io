#pragma once
#include "quote.h"


uint32 MakeBufferColorNoChange(VideoBuffer* Buffer, float3 Color)
{
    return  0 << 24 |
        RoundToInt(Color.x * 255.0f) << 16 |
        RoundToInt(Color.y * 255.0f) << 8 |
        RoundToInt(Color.z * 255.0f) << 0;
}
uint32 MakeBufferReverse(VideoBuffer* Buffer, float3 Color)
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
uint32 MakeBufferColor(VideoBuffer* Buffer, float3 Color)
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
uint32 MakeBufferColor(VideoBuffer* Buffer, float Color)
{
    return MakeBufferColor(Buffer, float3(Color, Color, Color));
}
float3 GetBufferColorNoChange(VideoBuffer* Buffer, uint32 Color)
{
    return float3(((uint8*)&Color)[2] / 256.0f,
        ((uint8*)&Color)[1] / 256.0f,
        ((uint8*)&Color)[0] / 256.0f);
}
float3 GetBufferColor(VideoBuffer* Buffer, uint32 Color)
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
float3 GetBufferColorReverse(VideoBuffer* Buffer, uint32 Color)
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

void DrawRectangle(VideoBuffer* Buffer,
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

    uint8* Row = ((uint8*)Buffer->memory + MinY * Buffer->Pitch + MinX * Buffer->BytesPerPixel);
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
                ////float wa = ((uint8*)&color)[3] / 256.0f;
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

void DrawClear(VideoBuffer* Buffer, float3 Color)
{
    DrawRectangle(Buffer, float2(0, 0), float2(Buffer->Width, Buffer->Height), Color);
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
    int PixelX = x * image->width;
    int PixelY = y * image->height;
    return image->ImageData[(PixelY) * image->width + PixelX];
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

void DrawImageScaledSmooth(VideoBuffer* Buffer, Image* image,
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

    uint8* Row = ((uint8*)Buffer->memory + MinY * Buffer->Pitch + MinX * Buffer->BytesPerPixel);
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

float DistanceToLine(float2 p, float2 a, float2 b)
{
    float2 pa = p - a, ba = b - a;
    float h = clamp01(dot(pa, ba) / dot(ba, ba));
    return length(pa - ba * h);
}

bool IsInScreen(VideoBuffer* videoBuffer, float2 pos)
{
    if (pos.x < 0 || pos.y < 0 || pos.y > videoBuffer->Height || pos.x > videoBuffer->Width)
        return false;

    return true;
}

uint32* GetPixel(VideoBuffer* Buffer, int y, int x)
{
    x = (int)min(max((double)x, 0.0), (double)(Buffer->Height - 1));
    y = (int)min(max((double)y, 0.0), (double)(Buffer->Width - 1));

    uint32 offset = (x * Buffer->Width + y);
    return (uint32*)Buffer->memory + offset;
}

void SetPixel(VideoBuffer* videoBuffer, int x, int y, uint32 Color)
{
    //x = (int)min(max((float)x, 0.0), (float)(videoBuffer->Width - 1));
    //y = (int)min(max((float)y, 0.0), (float)(videoBuffer->Height - 1));
    x = clamp(x, 0, videoBuffer->Width - 1);
    y = clamp(y, 0, videoBuffer->Height - 1);
    uint32 offset = (y * videoBuffer->Width + x);
    *((uint32*)videoBuffer->memory + offset) = Color;
}
void SetPixel(VideoBuffer* videoBuffer, int x, int y, float3 Color)
{
    SetPixel(videoBuffer, x, y, MakeBufferColor(videoBuffer, clamp01(Color)));
}
void SetColor(uint32* Pixel, uint32 NewSample)
{
    ((uint8*)Pixel)[0] = ((uint8*)&NewSample)[0];
    ((uint8*)Pixel)[1] = ((uint8*)&NewSample)[1];
    ((uint8*)Pixel)[2] = ((uint8*)&NewSample)[2];
}


void DrawImage(VideoBuffer* Buffer, Image* image, float X, float Y)
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

    uint8* Row = ((uint8*)Buffer->memory + MinY * Buffer->Pitch + MinX * Buffer->BytesPerPixel);

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

void DrawMemory(VideoBuffer* Buffer, uint8* InputBuffer, int count)
{
    uint8* CurrentBuffer = InputBuffer;
    int CurrentCount = 0;
    uint8* Row = (uint8*)Buffer->memory + 0 * Buffer->Pitch + 0 * Buffer->BytesPerPixel;
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

void DrawMemoryColor(VideoBuffer* Buffer, uint8* InputBuffer, int count)
{
    uint8* CurrentBuffer = InputBuffer;
    int CurrentCount = 0;
    uint8* Row = (uint8*)Buffer->memory + 0 * Buffer->Pitch + 0 * Buffer->BytesPerPixel;
    for (int y = 0; y < Buffer->Height; y++)
    {
        uint32* Pixel = (uint32*)Row;
        for (int x = 0; x < Buffer->Width; x++)
        {
            CurrentCount++;
            if (CurrentCount > count)
                return;
            
            float value0 = *CurrentBuffer / 255.0f;
            CurrentBuffer++;
            float value1 = *CurrentBuffer / 255.0f;
            CurrentBuffer++;
            float value2 = *CurrentBuffer / 255.0f;
            CurrentBuffer++;
            float value3 = *CurrentBuffer / 255.0f;
            CurrentBuffer++;
            float3 NewColor = float3(value2, value1, value0);
            float3 OldColor = GetBufferColor(Buffer, *Pixel);

            //*Pixel = MakeBufferColor(Buffer, lerp(OldColor, NewColor, 0.75f));
            *Pixel = MakeBufferColor(Buffer, NewColor);
            Pixel++;
        }
        Row += Buffer->Pitch;
    }
}


void DrawPoint(VideoBuffer* Buffer, float2 pos, float3 Color = { 1, 1, 1 })
{
    *GetPixel(Buffer, pos.x, pos.y) = MakeBufferColor(Buffer, Color);
    *GetPixel(Buffer, pos.x + 1, pos.y) = MakeBufferColor(Buffer, Color);
    *GetPixel(Buffer, pos.x, pos.y + 1) = MakeBufferColor(Buffer, Color);
    *GetPixel(Buffer, pos.x - 1, pos.y) = MakeBufferColor(Buffer, Color);
    *GetPixel(Buffer, pos.x, pos.y - 1) = MakeBufferColor(Buffer, Color);
}

void DrawPixel(VideoBuffer* Buffer, float2 pos, float3 Color = { 1, 1, 1 })
{
    *GetPixel(Buffer, pos.x, pos.y) = MakeBufferColor(Buffer, Color);
}

void DrawCross(VideoBuffer* Buffer, int x, int y)
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

void DrawCircle(VideoBuffer* Buffer, float2 pos, float radius, float3 color = {1,1,1})
{
    if (radius <= 0)
        return;
    for (float i = 0; i < (int)(3.14152128f * 2.3f); i += (1.0f / radius))
    {
        float2 a = pos + float2(sin(i) * radius, cos(i) * radius);
        *GetPixel(Buffer, a.x, a.y) = MakeBufferColor(Buffer, color);
    }
    DrawPoint(Buffer, pos, color);
}


bool IntersectRayBoxDistance(float2 rayStart, float2 rayDirection, float2 boxPos, float2 boxSize, float* hitClose, float* hitFar)
{
    rayDirection = normalize(rayDirection);
    float rx = rayStart.x;
    float ry = rayStart.y;
    float dx = rayDirection.x;
    float dy = rayDirection.y;
    float x = boxPos.x + boxSize.x/2;
    float y = boxPos.y + boxSize.y/2;
    float width = boxSize.x;
    float height = boxSize.y;

    float hwidth = width / 2;
    float hheight = height / 2;
    float px = rx - x;
    float py = ry - y;
    float LeftDistance = (-hwidth - px) / dx;
    float BottomDistance = (-hheight - py) / dy;
    float RightDistance = (hwidth - px) / dx;
    float TopDistance = (hheight - py) / dy;

    float xmax = LeftDistance > RightDistance ? LeftDistance : RightDistance;
    float xmin = LeftDistance < RightDistance ? LeftDistance : RightDistance;
    float ymax = TopDistance > BottomDistance ? TopDistance : BottomDistance;
    float ymin = TopDistance < BottomDistance ? TopDistance : BottomDistance;
    if (xmin > ymax || ymin > xmax)
    {
        return false;
    }
    *hitClose = max(xmin, ymin);
    *hitFar = min(xmax, ymax);
    return true;
}

bool ConstrainLine(float2 start, float2 end, float2 boxPos, float2 boxSize, float2* hitStart, float2* hitEnd)
{
    float hitClose2;
    float hitFar2;
    float2 rayDirection = (end - start);
    bool hit = IntersectRayBoxDistance(start, rayDirection, boxPos, boxSize, &hitClose2, &hitFar2);
    
    bool bothOut = (hitClose2 > 0) && ((hitFar2 - length(rayDirection)) > 0);

	if(hit && hitFar2 > 0 && hitClose2 - length(rayDirection)<0)
	{
		if(hitClose2 > 0)
		{
            *hitStart = (start+ (normalize(rayDirection) * float2(hitClose2, hitClose2)));
		}
		else
		{
            *hitStart = start;
		}

		if((hitFar2 - length(rayDirection)) < 0)
		{
            *hitEnd = (start+ (normalize(rayDirection) * float2(hitFar2, hitFar2)));
		}
		else
		{
            *hitEnd = end;
		}
        return true;
	}
    return false;
}

void DrawLine(VideoBuffer* Buffer, float2 pos0, float2 pos1, float3 color = {1, 1, 1}, float A = 1.0)
{
	if(pos0.x == pos1.x && pos0.y == pos1.y)
		return;
	
	//float4 newBox = FitBoxInBox(box2(boxPos, boxSize), box2(float2(0, 0), raven->videoSize));
    
    float dist = distance(pos0, pos1);
    dist *= 1.25f;

    if (dist > 10000) // TODO: clip it instead!
        return;

	if(!ConstrainLine(pos0, pos1, float2(0,0), float2(Buffer->Width, Buffer->Height), &pos0, &pos1))
		return;
		
	int x1 = pos0.x;
	int y1 = pos0.y;
	int x2 = pos1.x;
	int y2 = pos1.y;
	
	//uint32 bufferColor = ToPackedColor(color);
	uint32* row = (uint32*)Buffer->memory;
		
	int dx = x2 - x1;
	int dy = y2 - y1;

	int steps = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);

	float xIncrement = dx / (float)steps;
	float yIncrement = dy / (float)steps;

	float x = x1, y = y1;

	for (int i = 0; i <= steps; i++)
	{
        *GetPixel(Buffer, (int)x, (int)y) = MakeBufferColor(Buffer, color);

		//row[((int)y) * (int)raven->videoSize.x + ((int)x)] = bufferColor;
		x += xIncrement;
		y += yIncrement;
		//if(i%((steps/128)+1)==0 && thickness > 1)
		//	DrawCircleFilled(float2(x, y), thickness, color);
	}
}


void DrawLine_old(VideoBuffer* Buffer, float2 Start, float2 End, float3 color = {1, 1, 1}, float A = 1.0)
{
    if ((Start.x < 0 || Start.y < 0 || Start.y > Buffer->Height || Start.x > Buffer->Width) &&
        (End.y > Buffer->Height || End.x < 0 || End.y < 0 || End.x > Buffer->Width))
        return;

    float dist = distance(Start, End);
    dist *= 1.25f;

    if (dist > 10000) // TODO: clip it instead!
        return;

    for (float i = 0; i < 1.0f; i += (1.0f / dist))
    {
        float2 l = lerp(Start, End, i);
        //BlendColor()
        if (A >= 1.0f)
        {
            *GetPixel(Buffer, l.x, l.y) = MakeBufferColor(Buffer, color);
        }
        else
        {
            
            float3 v = GetBufferColor(Buffer, MakeBufferColor(Buffer, color));
            float3 w = GetBufferColor(Buffer, *GetPixel(Buffer, l.x, l.y));
            //float vr = ((uint8*)Pixel)[0] / 256.0f;
            //float vg = ((uint8*)Pixel)[1] / 256.0f;
            //float vb = ((uint8*)Pixel)[2] / 256.0f;
            //
            //float wr = ((uint8*)&BufferColor)[0] / 256.0f;
            //float wg = ((uint8*)&BufferColor)[1] / 256.0f;
            //float wb = ((uint8*)&BufferColor)[2] / 256.0f;
            *GetPixel(Buffer, l.x, l.y) = MakeBufferReverse(Buffer, BlendColor(v, w, A));
        }
    }
}
float singleBezier(float a, float b, float c, float t)
{
    return (a - 2 * b + c) * t * t + (2 * b - 2 * c) * t + c;
}
void DrawLineBezier(VideoBuffer* Buffer, float2 Start, float2 Middle, float2 End, float3 color = { 1, 1, 1 }, float A = 1.0)
{
    if ((Start.x < 0 || Start.y < 0 || Start.y > Buffer->Height || Start.x > Buffer->Width) &&
        (End.y > Buffer->Height || End.x < 0 || End.y < 0 || End.x > Buffer->Width))
        return;

    float dist = distance(Start, Middle) + distance(Middle, End);
    dist *= 1.5f;

    if (dist > 10000) // TODO: clip it instead!
        return;

    for (float i = 0; i < 1.0f; i += (1.0f / dist))
    {
        //float2 l0 = lerp(Start, Middle, i);
        //float2 l1 = lerp(Middle, End, i);
        //float2 l = lerp(l0, l1, i);

        float2 l;
        l.x = singleBezier(Start.x, Middle.x, End.x, i);
        l.y = singleBezier(Start.y, Middle.y, End.y, i);
        //if (A >= 1.0f)
        //{
            *GetPixel(Buffer, l.x, l.y) = MakeBufferColor(Buffer, color);
        //}
        //else
        //{
        //
        //    float3 v = GetBufferColor(Buffer, MakeBufferColor(Buffer, color));
        //    float3 w = GetBufferColor(Buffer, *GetPixel(Buffer, l.x, l.y));
        //    //float vr = ((uint8*)Pixel)[0] / 256.0f;
        //    //float vg = ((uint8*)Pixel)[1] / 256.0f;
        //    //float vb = ((uint8*)Pixel)[2] / 256.0f;
        //    //
        //    //float wr = ((uint8*)&BufferColor)[0] / 256.0f;
        //    //float wg = ((uint8*)&BufferColor)[1] / 256.0f;
        //    //float wb = ((uint8*)&BufferColor)[2] / 256.0f;
        //    *GetPixel(Buffer, l.x, l.y) = MakeBufferReverse(Buffer, BlendColor(v, w, A));
        //}
    }
}
void DrawBox(VideoBuffer* Buffer, float2 Pos, float2 Size, float3 color = { 1,1,1 })
{
    float2 TopLeft = Pos + Size * float2(-0.5, 0.5);
    float2 TopRight = Pos + Size * float2(0.5, 0.5);
    float2 BottomLeft = Pos + Size * float2(0.5, -0.5);
    float2 BottomRight = Pos + Size * float2(-0.5, -0.5);
    DrawLine(Buffer, TopLeft, TopRight, color);
    DrawLine(Buffer, TopRight, BottomLeft, color);
    DrawLine(Buffer, BottomLeft, BottomRight, color);
    DrawLine(Buffer, BottomRight, TopLeft, color);
    //DrawPoint(Buffer, Pos, color);
}
void DrawBox(VideoBuffer* Buffer, float2 Pos, float2 Size, float2 Up, float2 Right, float3 color = { 1,1,1 })
{
    float2 TopLeft = Size * float2(-0.5, 0.5);
    float2 TopRight = Size * float2(0.5, 0.5);
    float2 BottomLeft = Size * float2(0.5, -0.5);
    float2 BottomRight = Size * float2(-0.5, -0.5);

    TopLeft = Pos + float2(dot(TopLeft, Right), dot(TopLeft, Up));
    TopRight = Pos + float2(dot(TopRight, Right), dot(TopRight, Up));
    BottomLeft = Pos + float2(dot(BottomLeft, Right), dot(BottomLeft, Up));
    BottomRight = Pos + float2(dot(BottomRight, Right), dot(BottomRight, Up));

    DrawLine(Buffer, TopLeft, TopRight, color);
    DrawLine(Buffer, TopRight, BottomLeft, color);
    DrawLine(Buffer, BottomLeft, BottomRight, color);
    DrawLine(Buffer, BottomRight, TopLeft, color);
    DrawPoint(Buffer, Pos, color);
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

uint32 MakeBufferColor2(VideoBuffer* Buffer, float3 Color)
{
    if (Buffer->Layout == ChannelLayout_ABGR)
    {
        return  0 << 24 |
            RoundToInt(clamp01(Color.z) * 255.0f) << 16 |
            RoundToInt(clamp01(Color.y) * 255.0f) << 8 |
            RoundToInt(clamp01(Color.x) * 255.0f) << 0;
    }
    else
    {
        return 0 << 24 |
            RoundToInt(clamp01(Color.x) * 255.0f) << 16 |
            RoundToInt(clamp01(Color.y) * 255.0f) << 8 |
            RoundToInt(clamp01(Color.z) * 255.0f) << 0;
    }
}
void DrawFont(Image* fontSpritesheet, VideoBuffer* videoBuffer, float X, float Y, const char* text, float R = 1.0, float G = 1.0, float B = 1.0, bool transparent = true, int LengthOverride = 0)
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
                //char BB = R * 255;
                //char GG = G * 255;
                //char RR = B * 255;
                
                uint32 TextColor = MakeBufferColor2(videoBuffer, float3(R, G, B));// 255 << 24 | BB << 16 | GG << 8 | RR;
                uint32 BackgroundColor = 0 << 16 | 0 << 8 | 0;
                if (transparent)
                {
                    if (c == 0)
                        SetPixel(videoBuffer, OffsetX + x + X, OffsetY + y + Y, TextColor);
                }
                else
                {
                    if (c == 0)
                        SetPixel(videoBuffer, OffsetX + x + X, OffsetY + y + Y, TextColor);
                    else
                        SetPixel(videoBuffer, OffsetX + x + X, OffsetY + y + Y, BackgroundColor);

                }
            }
        }
        OffsetX += SpriteWidth;
    }
}

// asks the platform layer to start loading an image.
Image* LoadImage(EngineState* engineState, MemoryArena* arena, const char* path, int width, int height)
{
    Image* imageHeader = PushStruct(arena, Image);
    StringCopy(path, imageHeader->name);
    
    uint32* imageData = PushArray(arena, width * height, uint32);
    //Image* newImage = &engineState->images[engineState->currentImage];
    //for (int i = 0; i < StringLength(path); i++)
    //{
    //    engineState->imageNames[engineState->currentImage][i] = path[i];
    //}
    ArrayAdd(engineState->images, imageHeader);
    imageHeader->ImageData = imageData;
    imageHeader->width = width;
    imageHeader->height = height;

    // Fill image with missing image image
    int cells = 2;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            bool a = max((x % (width / cells)) - (width / cells / 2), 0);
            bool b = max((y % (width / cells)) - (width / cells / 2), 0);
            bool c = a ^ b;
            ((char*)imageHeader->ImageData)[(width * y + x) * 4 + 0] = c ? 255 : 0;
            ((char*)imageHeader->ImageData)[(width * y + x) * 4 + 1] = 0;
            ((char*)imageHeader->ImageData)[(width * y + x) * 4 + 2] = c ? 255 : 0;
            ((char*)imageHeader->ImageData)[(width * y + x) * 4 + 3] = 255;
        }
    }

    engineState->platformReadImageIntoMemory(imageData, path);

    //engineState->currentImage++;
    return imageHeader;
}

void ProfilerDrawFlameChart(GameInput* input, EngineState* engineState, VideoBuffer* videoBuffer)
{
    ProfilingData* profilingData = &engineState->profilingData;

    int64 lowest = 0;
    int64 highest = 0;
    for (int k = 0; k < timeSampleCount; k++)
    {
        lowest += profilingData->enterTime[0][k];
        highest += profilingData->exitTime[profilingData->index - 1][k];
    }
    lowest /= timeSampleCount;
    highest /= timeSampleCount;
    //int64 lowest = profilingData->EnterTime[0][0];
    //int64 highest = profilingData->ExitTime[profilingData->index - 1][0];
    float rightOffset = 250;
    float pixelWidth = videoBuffer->Width/(videoBuffer->Width - rightOffset);
    // Zoom
    if (input->MouseX > rightOffset && input->MouseY > (videoBuffer->Height - 80))
    {
        profilingData->zoom -= profilingData->zoom * input->MouseZDelta * 0.1f;
        profilingData->zoom = max(profilingData->zoom, 0.1f);

    }

    int64 length = highest - lowest;
    float sampleCount = (300000 * profilingData->zoom) * pixelWidth + 15000;
    float milliseconds = (float)length / 10000.0f;
    float StepSize = 10000.0f / (float)length;
    //for (int i = 0; i < milliseconds; i++)
    //{
    //    pos += StepSize;
    //    float t = pos * videoBuffer->Width;
    //    DrawLine(videoBuffer, float2(t, videoBuffer->Height), float2(t, videoBuffer->Height - 100));
    //}
    

    float q = round(((float)sampleCount) / (100000))*0.5f;

    DrawRectangle(videoBuffer, float2(rightOffset, videoBuffer->Height - 80), float2(videoBuffer->Width- rightOffset, 80), float3(0.0, 0.0, 0.0), 0.5);

    DrawLine(videoBuffer, float2(rightOffset, videoBuffer->Height - 1), float2(videoBuffer->Width, videoBuffer->Height - 1));
    for (int i = 0; i < 21; i++)
    {
        int point = (((float)i * 10000*q) / ((float)sampleCount))* videoBuffer->Width + rightOffset;
        DrawLine(videoBuffer, float2(point, videoBuffer->Height - 0), float2(point, videoBuffer->Height - 8));
        DrawLine(videoBuffer, float2(point+1, videoBuffer->Height - 0), float2(point+1, videoBuffer->Height - 8));
        char time[100] = {};
        Append(time, (int)(i*q));
        Append(time, "ms");
        DrawFont(engineState->fontSpritesheet, videoBuffer, point+2, videoBuffer->Height - 20, time);
    }
    float pos = 0;

    for (int i = 0; i < profilingData->index; i++)
    {
        int64 start = 0;
        int64 end = 0;
        for (int k = 0; k < timeSampleCount; k++)
        {
            start += profilingData->enterTime[i][k] - lowest;
            end += profilingData->exitTime[i][k] - lowest;
        }
        start /= timeSampleCount;
        end /= timeSampleCount;
        //int64 start = profilingData->EnterTime[i][0] - lowest;
        //int64 end = profilingData->ExitTime[i][0] - lowest;
        char* name = profilingData->functionNames[i];

        //float startNormalized = (float)start / (float)length;
        //float endNormalized = (float)end / (float)length;
        float startNormalized = (float)start / sampleCount;
        float endNormalized = (float)end / sampleCount;
        startNormalized *= videoBuffer->Width;
        endNormalized *= videoBuffer->Width;
        startNormalized = max(startNormalized, 0);

        float height = videoBuffer->Height - (17 * profilingData->depth[i]);
        DrawRectangle(videoBuffer, float2(round(startNormalized), height) + float2(rightOffset, 0), float2(round(endNormalized - startNormalized) - 1, 16), float3(0.5, 0.5, 0.5));
        char time[100] = {};
        Append(time, name);
        Append(time, ": ");
        Append(time, (int)((end - start) / 10));
        Append(time, "us");
        DrawFont(engineState->fontSpritesheet, videoBuffer, startNormalized + rightOffset + 2, height, time);
    }
}

void ProfilerDrawTimeChart(GameInput* input, EngineState* engineState, VideoBuffer* videoBuffer)
{
    ProfilingData* profilingData = &engineState->profilingData;

    DrawRectangle(videoBuffer, float2(0, videoBuffer->Height - 200), float2(200, 200), float3(0.0, 0.0, 0.0), 0.5);

    float FPS = (float)(1.0f / input->deltaTime);
    float Delta = (float)(profilingData->Delta) / 10000000;
    float accumulatedDelta = GetSmoothedValue(profilingData->deltas, &profilingData->deltasIndex, Delta, 30);
    float SmoothFPS = GetSmoothedValue(profilingData->FPS, &profilingData->FPSIndex, FPS, 30);
    
    //int64 Global_End = Memory->PlatformTime();
    float TotalTimeDelta = (float)(profilingData->Delta) / 10000000;
    float SmoothedTime = GetSmoothedValue(profilingData->deltas2, &profilingData->deltas2Index, TotalTimeDelta, 100);

    float HeightMultiplier = 5000;
    float LastHeight = 0;
    for (int i = 0; i < 100; i++)
    {
        int h = (i + profilingData->deltas2Index) % 100;
        float a = profilingData->deltas2[h] * HeightMultiplier;
        float height = videoBuffer->Height - a;
        DrawLine(videoBuffer, float2(i * 2 - 2, LastHeight), float2(i * 2, height));
        //DrawLine(videoBuffer, float2(i * 2 - 2, 493), float2(i * 2, 493));
        LastHeight = height;
    }

    //float fps30dt = (1.0f / 30.0f);
    //float fps60dt = (1.0f / 60.0f);
    //float fps30height = videoBuffer->Height - fps30dt * HeightMultiplier;
    //float fps60height = videoBuffer->Height - fps60dt * HeightMultiplier;
    float fpscurrentheight = videoBuffer->Height - (SmoothedTime)*HeightMultiplier;
    //DrawLine(videoBuffer, float2(0, fps30height), float2(200, fps30height));
    //DrawLine(videoBuffer, float2(0, fps60height), float2(200, fps60height));
    //DrawFont(engineState->fontSpritesheet, videoBuffer, 240, fps30height, "33.33ms");
    //DrawFont(engineState->fontSpritesheet, videoBuffer, 240, fps60height, "16.66ms");
    char text[100] = {};
    Append(text, TotalTimeDelta * 1000);
    Append(text, "ms");
    DrawFont(engineState->fontSpritesheet, videoBuffer, 200, fpscurrentheight, text);
}