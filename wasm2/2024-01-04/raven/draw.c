#pragma once

uint32 ToPackedColor(float3 color)
{
    return (
		0 << 24 |
        RoundToInt(color.x * 255.0f) << 16 |
        RoundToInt(color.y * 255.0f) << 8 |
        RoundToInt(color.z * 255.0f) << 0);
}
float4 FromPackedColor(uint32 color)
{
    return float4(
		((uint8*)&color)[2] / 256.0f,
		((uint8*)&color)[1] / 256.0f,
		((uint8*)&color)[0] / 256.0f,
		((uint8*)&color)[3] / 256.0f);
}
uint32 alphaBlend(uint32 from, uint32 to)
{
    return (
		0 << 24 |
        lerp8(((uint8*)&from)[2], ((uint8*)&to)[2], ((uint8*)&to)[3]) << 16 |
        lerp8(((uint8*)&from)[1], ((uint8*)&to)[1], ((uint8*)&to)[3]) << 8 |
        lerp8(((uint8*)&from)[0], ((uint8*)&to)[0], ((uint8*)&to)[3]) << 0);
}
uint32 alphaBlendWithColor(uint32 from, uint32 to, uint32 color)
{
    return (
		0 << 24 |
        lerp8(((uint8*)&from)[2], ((uint8*)&color)[2], ((uint8*)&to)[3]) << 16 |
        lerp8(((uint8*)&from)[1], ((uint8*)&color)[1], ((uint8*)&to)[3]) << 8 |
        lerp8(((uint8*)&from)[0], ((uint8*)&color)[0], ((uint8*)&to)[3]) << 0);
}
void DrawImageRegion(Image* image, float2 pos, float2 offset, float2 size, float3 color)
{
	int X = pos.x;
	int Y = pos.y;
	int offsetX = offset.x;
	int offsetY = offset.y;
	int sizeX = size.x;
	int sizeY = size.y;

	int sourceMinX = offsetX;
	int sourceMinY = offsetY;
	int sourceMaxX = offsetX + sizeX;
	int sourceMaxY = offsetY + sizeY;
	
	if (sourceMinX < 0)
		sourceMinX = 0;
	if (sourceMinY < 0)
		sourceMinY = 0;

	if (sourceMaxX > image->sizeX)
		sourceMaxX = image->sizeX;
	if (sourceMaxY > image->sizeY)
		sourceMaxY = image->sizeY;
	
	sizeX = sourceMaxX - sourceMinX;
	sizeY = sourceMaxY - sourceMinY;

	int minX = X;
	int minY = Y;
	int maxX = X + sizeX;
	int maxY = Y + sizeY;

	if (minX < 0) minX = 0;
	if (minY < 0) minY = 0;
	if (maxX < 0) maxX = 0;
	if (maxY < 0) maxY = 0;
	if (maxX > platform->videoSizeX) maxX = platform->videoSizeX;
	if (maxY > platform->videoSizeY) maxY = platform->videoSizeY;
	if (minX > platform->videoSizeX) minX = platform->videoSizeX;
	if (minY > platform->videoSizeY) minY = platform->videoSizeY;
	
	int bytesPerPixel = 4;
	int stride = platform->videoSizeX * bytesPerPixel;
	uint8* row = (platform->videoBuffer + minY * stride + minX * bytesPerPixel);
	uint32 packedColor = ToPackedColor(color);
	for (int y = minY; y < maxY; y++)
	{
		uint32* pixel = (uint32*)row;
		for (int x = minX; x < maxX; x++)
		{
			int i = (y - Y + offsetY) * image->sizeX + (x - X + offsetX);
			*pixel = alphaBlendWithColor(*pixel, image->data[i], packedColor);
			pixel++;
		}
		row += stride;
	}
}
void DrawImage(Image* image, float2 pos)
{
	int X = pos.x;
	int Y = pos.y;
	int minX = X;
	int minY = Y;
	int maxX = X + image->sizeX;
	int maxY = Y + image->sizeY;

	if (minX < 0) minX = 0;
	if (minY < 0) minY = 0;
	if (maxX < 0) maxX = 0;
	if (maxY < 0) maxY = 0;
	if (maxX > platform->videoSizeX) maxX = platform->videoSizeX;
	if (maxY > platform->videoSizeY) maxY = platform->videoSizeY;
	if (minX > platform->videoSizeX) minX = platform->videoSizeX;
	if (minY > platform->videoSizeY) minY = platform->videoSizeY;

	int bytesPerPixel = 4;
	int stride = platform->videoSizeX * bytesPerPixel;
	uint8* row = (platform->videoBuffer + minY * stride + minX * bytesPerPixel);

	for (int y = minY; y < maxY; y++)
	{
		int i = (y - Y) * image->sizeX;
		uint32* pixel = (uint32*)row;
		for (int x = minX; x < maxX; x++)
		{
			*pixel = image->data[i];//alphaBlend(*pixel, image->data[i]);
			pixel++;
			i++;
		}
		row += stride;
	}
}
void DrawCircleFilled(float2 pos, float radius, float3 color)
{
	int X = pos.x;
	int Y = pos.y;
	int size = radius;
	
	int minX = X - size;
	int minY = Y - size;
	int maxX = X + size;
	int maxY = Y + size;

	if (minX < 0) minX = 0;
	if (minY < 0) minY = 0;
	if (maxX < 0) maxX = 0;
	if (maxY < 0) maxY = 0;
	if (maxX > platform->videoSizeX) maxX = platform->videoSizeX;
	if (maxY > platform->videoSizeY) maxY = platform->videoSizeY;
	if (minX > platform->videoSizeX) minX = platform->videoSizeX;
	if (minY > platform->videoSizeY) minY = platform->videoSizeY;

	X = (minX + maxX) / 2;
	Y = (minY + maxY) / 2;
	
    uint32 bufferColor = ToPackedColor(color);
	uint32* row = ((uint32*)platform->videoBuffer + (Y-size) * platform->videoSizeX + (X-size));
	for (int y = -size; y < size; y++)
	{
		int circleDist = sqrt(size*size- y*y);
		uint32* Pixel = (uint32*)row;
		for (int x = -size; x < size; x++)
		{
			if(x < circleDist && x > -circleDist)
				*Pixel = bufferColor;
			Pixel++;
		}
		row += platform->videoSizeX;
	}
}
void DrawCircle(float2 pos, float radius, float3 color, int thickness)
{
	int X = pos.x;
	int Y = pos.y;
	int size = radius;
	
	int minX = X - radius;
	int minY = Y - radius;
	int maxX = X + radius;
	int maxY = Y + radius;

	if (minX < 0) minX = 0;
	if (minY < 0) minY = 0;
	if (maxX < 0) maxX = 0;
	if (maxY < 0) maxY = 0;
	if (maxX > platform->videoSizeX) maxX = platform->videoSizeX;
	if (maxY > platform->videoSizeY) maxY = platform->videoSizeY;
	if (minX > platform->videoSizeX) minX = platform->videoSizeX;
	if (minY > platform->videoSizeY) minY = platform->videoSizeY;
		
	if(thickness == 1)
	{
		uint32 bufferColor = ToPackedColor(color);
		uint32* row = ((uint32*)platform->videoBuffer + (Y-size) * platform->videoSizeX + (0));
		for (int y = minY; y < maxY; y++)
		{
			int localY = y - Y;
			int circleDist = sqrt(size * size - localY * localY);
			row[clampi(circleDist + X, minX, maxX)] = bufferColor;
			row[clampi(-circleDist + X, minX, maxX)] = bufferColor;
			row += platform->videoSizeX;
		}
		row = ((uint32*)platform->videoBuffer + (0) * platform->videoSizeX + (X-size));
		for (int x = minX; x < maxX; x++)
		{
			int localX = x - X;
			int circleDist = sqrt(size * size - localX * localX);
			row[clampi(circleDist+Y, minY, maxY) * platform->videoSizeX] = bufferColor;
			row[clampi(-circleDist+Y, minY, maxY) * platform->videoSizeX] = bufferColor;
			row++;
		}
	}
	else
	{
		uint32* row = ((uint32*)platform->videoBuffer + minY * platform->videoSizeX + minX);
		uint32 bufferColor = ToPackedColor(color);
		for (int y = minY; y < maxY; y++)
		{
			int localY = y - Y;
			int circleDist = sqrt(size * size - localY * localY);
			int circleDist2 = sqrt((size-thickness)*(size-thickness)- localY * localY);

			uint32* Pixel = row;
			for (int x = minX; x < maxX; x++)
			{
				int localX = x - X;
				bool inCircleBig   = localX < circleDist  && localX > -circleDist;
				bool inCircleSmall = localX < circleDist2 && localX > -circleDist2;
				if(inCircleBig&& !inCircleSmall)
					*Pixel = bufferColor;
				Pixel++;
			}
			row += platform->videoSizeX;
		}
	}
}
void DrawBoxFilled(float2 pos, float2 size, float3 color)
{
	int X = pos.x;
	int Y = pos.y;
	int sizeX = size.x;
	int sizeY = size.y;
	
	int minX = X;
	int minY = Y;
	int maxX = X + sizeX;
	int maxY = Y + sizeY;

	if (minX < 0) minX = 0;
	if (minY < 0) minY = 0;
	if (maxX < 0) maxX = 0;
	if (maxY < 0) maxY = 0;
	if (maxX > platform->videoSizeX) maxX = platform->videoSizeX;
	if (maxY > platform->videoSizeY) maxY = platform->videoSizeY;
	if (minX > platform->videoSizeX) minX = platform->videoSizeX;
	if (minY > platform->videoSizeY) minY = platform->videoSizeY;

	uint32* row = ((uint32*)platform->videoBuffer + minY * platform->videoSizeX + minX);
    // AA RR GG BB
    uint32 bufferColor = ToPackedColor(color);
	for (int y = minY; y < maxY; y++)
	{
		uint32* Pixel = row;
		for (int x = minX; x < maxX; x++)
		{
			*Pixel = bufferColor;
			Pixel++;
		}
		row += platform->videoSizeX;
	}
}
void DrawBox(float2 pos, float2 size, float3 color, int thickness)
{
	int X = pos.x;
	int Y = pos.y;
	int sizeX = size.x;
	int sizeY = size.y;
	
	int minX = X;
	int minY = Y;
	int maxX = X + sizeX;
	int maxY = Y + sizeY;

	if (minX < 0) minX = 0;
	if (minY < 0) minY = 0;
	if (maxX < 0) maxX = 0;
	if (maxY < 0) maxY = 0;
	if (maxX > platform->videoSizeX) maxX = platform->videoSizeX;
	if (maxY > platform->videoSizeY) maxY = platform->videoSizeY;
	if (minX > platform->videoSizeX) minX = platform->videoSizeX;
	if (minY > platform->videoSizeY) minY = platform->videoSizeY;

	DrawBoxFilled(pos, float2(size.x, thickness), color);
	DrawBoxFilled(float2(pos.x, pos.y + size.y - thickness), float2(size.x, thickness), color);

	DrawBoxFilled(pos, float2(thickness, size.y), color);
	DrawBoxFilled(float2(pos.x + size.x - thickness, pos.y), float2(thickness, size.y), color);
}
void DrawBoxFilledCentered(float2 pos, float2 size, float3 color)
{
	DrawBoxFilled(sub2(pos, mul2(size, float2(0.5f, 0.5f))), float2(size.x*2.0, size.y*2.0), color);
}
void DrawBoxCentered(float2 pos, float2 size, float3 color, int thickness)
{
	DrawBox(sub2(pos, mul2(size, float2(0.5f, 0.5f))), size, color, thickness);
}
void DrawClear(float3 color)
{
	DrawBoxFilled(float2(0,0), float2(platform->videoSizeX, platform->videoSizeY), color);
}
void DrawLineConstrained(float2 pos0, float2 pos1, float3 color, int thickness, float2 boxPos, float2 boxSize)
{
	if(pos0.x == pos1.x && pos0.y == pos1.y)
		return;
	
	box2 newBox = FitBoxInBox(box2(boxPos, boxSize), box2(float2(0, 0), float2(platform->videoSizeX, platform->videoSizeY)));

	if(!ConstrainLine(pos0, pos1, newBox.pos, newBox.size, &pos0, &pos1))
		return;
		
	int x1 = pos0.x;
	int y1 = pos0.y;
	int x2 = pos1.x;
	int y2 = pos1.y;
	
	uint32 bufferColor = ToPackedColor(color);
	uint32* row = (uint32*)platform->videoBuffer;
		
	int dx = x2 - x1;
	int dy = y2 - y1;

	int steps = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);

	float xIncrement = dx / (float)steps;
	float yIncrement = dy / (float)steps;

	float x = x1, y = y1;

	for (int i = 0; i <= steps; i++)
	{
		row[((int)y) * platform->videoSizeX + ((int)x)] = bufferColor;
		x += xIncrement;
		y += yIncrement;
		if(i%((steps/128)+1)==0 && thickness > 1)
			DrawCircleFilled(float2(x, y), thickness, color);
	}
}
void DrawLine(float2 pos0, float2 pos1, float3 color, int thickness)
{
	DrawLineConstrained(pos0, pos1, color, thickness, float2(0, 0), float2(platform->videoSizeX, platform->videoSizeY));
}
void DrawCross(float2 pos, float radius, float3 color)
{
	DrawLine(float2(pos.x-radius, pos.y),float2(pos.x+radius, pos.y), color, 1);
	DrawLine(float2(pos.x, pos.y-radius),float2(pos.x, pos.y+radius), color, 1);
}
float2 DrawStringSize(string text, int size)
{
    int posX = 0;
	int posY = 0;
	for (int i = 0; i < len(text); i++)
	{
		char a = text.text[i] - 33;
		if((a+33) == '\n')
		{
			posY++;
			posX = 0;
			continue;
		}
		else if(a == -1)
		{
		}
		else if(a < 0 || a > 93)
		{
			continue;
		}
		char x = a % 16;
		char y = a / 16;
		posX++;
	}
    posX++;
    posY++;
    return float2(posX*size/2, posY*size);
}
void DrawString(string text, float2 pos, int size, float3 color)
{
	int X = pos.x;
	int Y = pos.y;
	size = RoundToNearestPowerOfTwo(size);
	if(size < 8)
		size = 8;
	if(size > 128)
		size = 128;

	Image* font = 0;
	if(size == 256)
		font = &game->font0;
	else if(size == 128)
		font = &game->font1;
	else if(size == 64)
		font = &game->font2;
	else if(size == 32)
		font = &game->font3;
	else if(size == 16)
		font = &game->font4;
	else
		return;

	int posX = 0;
	int posY = 0;
	for (int i = 0; i < len(text); i++)
	{
		char a = text.text[i] - 33;
		if((a+33) == '\n')
		{
			posY++;
			posX = 0;
			continue;
		}
		else if(a == -1)
		{
		}
		else if(a < 0 || a > 93)
		{
			continue;
		}
		char x = a % 16;
		char y = a / 16;
		DrawImageRegion(font, float2(X + posX * size/2, Y + posY * size), float2(x*size/2, y*size), float2(size/2, size), color);
		posX++;
	}
}
void DrawTriangleConstrained(float2 center, float size, float angle, float3 color, float2 boxPos, float2 boxSize)
{
	float2 pos0 = rotate(float2(size, 0), angle);
	float2 pos1 = rotate(pos0, (pi*2.0f)/3.0f);
	float2 pos2 = rotate(pos1, (pi*2.0f)/3.0f);
	pos0 = add2(pos0, center);
	pos1 = add2(pos1, center);
	pos2 = add2(pos2, center);
	DrawLineConstrained(pos0, pos1, color, 1, boxPos, boxSize);
	DrawLineConstrained(pos1, pos2, color, 1, boxPos, boxSize);
	DrawLineConstrained(pos2, pos0, color, 1, boxPos, boxSize);
}
void DrawTriangle(float2 center, float size, float angle, float3 color)
{
	DrawTriangleConstrained(center, size, angle, color, float2(0,0), float2(platform->videoSizeX, platform->videoSizeY));
}
void DrawStar(float2 center, float size, float angle, float3 color, float thickness)
{
	float2 pos = rotate(float2(size, 0), angle);
	float2 lastPos = float2(size, 0);
	for (int i = 0; i < 14; i++)
	{
		pos = rotate(pos, (pi*2.0f)/12.0f);
		float2 pos2 = pos;
		if(i % 2 == 0)
			pos2 = mul2(pos2, float2(0.5f, 0.5f));
		pos2 = add2(pos2, center);
		if(i>1)
			DrawLine(pos2, lastPos, color, thickness);
		lastPos = pos2;
	}
}
void DrawDemo(float2 boxMin)
{
	//float2 boxMin = float2(500,500);
	float2 boxSize = float2(350,280);
	float2 boxMax = add2(boxMin, boxSize);

	float border = 35;
	float2 insideMin = add2(boxMin, float2(border,border));
	float2 insideSize = sub2(boxSize, float2(border*2,border*2));
	float2 insideMax = add2(boxMin, boxSize);
	float2 boxCenter = float2(boxMin.x + boxSize.x * 0.5f, boxMin.y + boxSize.y * 0.5f);
	DrawBoxFilled(boxMin, boxSize, float3(0.2, 0.2, 0.9));

	// Box
	//DrawBox(sub2(boxMin, float2(150, 150)), add2(boxSize, float2(300, 300)), float3(0,0,0), 150);
	//DrawBoxWithBorder(boxMin, boxSize, 35, float3(0.2, 0.2, 0.9));
	DrawBoxFilled(insideMin, insideSize, float3(0.0,0.0,0.0));
	DrawString(str("Graphics Demo!"), float2(boxMin.x+50, boxMin.y+3), 32, float3(1,1,1));
	
	// Triangles
	float triangleTime = mod(game->time, 5.0f) / 5.0f;
	for (int i = 0; i < 32; i++)
	{
		float t = i / 7.0f;
		float size = lerp(10, 100, t*(triangleTime))+triangleTime * 200;
		float angleOffset = lerp(0, 2.5f, t * triangleTime * 1.0f) ;
		DrawTriangleConstrained(boxCenter, size, -(game->time)*2.0+angleOffset, float3(0,0.9,0.2), insideMin, insideSize);
	}

	// Stars
	float2 StarCenter = float2(90,-55);
	for (int i = 0; i < 8; i++)
	{
		float t = i / 7.0f;
		float x = sin(t*pi*2 - game->time) * 32;
		float y = cos(t*pi*2 - game->time) * 32;
		DrawStar(add2(boxCenter, add2(StarCenter, float2(x,y))), 13, -game->time, float3(1, 1, 1), 1);
	}
	
	// Counter
	int q = ((int)(game->time*4.0f) % 10);
	char a[2] = {('0' + q), 0};
	DrawString(str(&a), add2(boxMin, float2(50, 40)), 64, float3(1,1,1));

	// Name
	float2 textLine = add2(boxMin, sub2(boxSize, float2(110, 60)));
	DrawLine(textLine, add2(textLine, float2(52, 0)), float3(0.9, 0.4f, 0.9f), 9);
	DrawString(str("Torvid"), sub2(textLine, float2(0, 7)), 16, float3(1,1,1));

	// Galaxy
	float galaxyTime = mod(game->time, 0.4f) / 0.4f;
	float galaxyTime2 = mod(game->time, 0.8f) / 0.8f;
	for (int i = 0; i < 9; i++)
	{
		int size = 8;
		float t = i / 8.0f;
		float x = sin(t*pi*2 - game->time) * 75;
		float y = cos(t*pi*2 - game->time) * 25;
		if(galaxyTime2 > 0.5)
			DrawBoxFilledCentered(add2(boxCenter, float2(x,y)), float2(size*galaxyTime, size*galaxyTime), float3(x/100.0f, y/100.0f, 1));
		else
			DrawCircleFilled(add2(boxCenter, float2(x,y)), (size)*galaxyTime, float3(x/100.0f, y/100.0f, 1));
	}
	
	// Middle blob
	float2 blobPos0 = float2(tri(game->time*0.4694815811 + 0.7221105997)*80-40, tri(game->time*0.3703184192+ 0.8147562726)*80-40 );
	float2 blobPos1 = float2(tri(game->time*0.5900246762 + 0.1886877359)*80-40, tri(game->time*0.4456080350+ 0.0182039207)*80-40 );
	float2 blobPos2 = float2(tri(game->time*0.4858904584 + 0.7524831489)*80-40, tri(game->time*0.8018918949+ 0.3349515974)*80-40 );
	float2 blobPos3 = float2(tri(game->time*0.8006556825 + 0.3984536610)*80-40, tri(game->time*0.5553325929+ 0.4998262658)*80-40 );
	float2 blobPos4 = float2(tri(game->time*0.9192018389 + 0.3810667543)*80-40, tri(game->time*0.7050597790+ 0.4001217784)*80-40 );
	blobPos0 = add2(blobPos0, boxCenter);
	blobPos1 = add2(blobPos1, boxCenter);
	blobPos2 = add2(blobPos2, boxCenter);
	blobPos3 = add2(blobPos3, boxCenter);
	blobPos4 = add2(blobPos4, boxCenter);
	DrawLine(blobPos0, blobPos1, float3(1,1,1), 2);
	DrawLine(blobPos1, blobPos2, float3(1,1,1), 2);
	DrawLine(blobPos2, blobPos3, float3(1,1,1), 2);
	DrawLine(blobPos3, blobPos4, float3(1,1,1), 2);
	DrawLine(blobPos4, blobPos0, float3(1,1,1), 2);
	
	// Thingy
	float2 ThingyCenter = add2(boxCenter,float2(-90,60));
	for (int i = 0; i < 8; i++)
	{
		float t = i / 7.0f;
		float2 rot = rotate(float2(20, 0), t*pi*2 - game->time);
		float2 rot2 = rotate(float2(30, 0), t*pi*2 + game->time);
		
		DrawCircleFilled(add2(ThingyCenter, rot), 4, float3(1,0,0));
		DrawCross(add2(ThingyCenter, rot), 2, float3(1,1,1));

		DrawCircleFilled(add2(add2(ThingyCenter, float2(5,5)), rot), 2, float3(1,1,1));

		DrawCircleFilled(add2(add2(ThingyCenter, float2(5,5)), rot2), 2, float3(1,1,1));
		DrawCircleFilled(add2(add2(ThingyCenter, float2(7,5)), rot2), 2, float3(1,1,1));
		DrawCircleFilled(add2(add2(ThingyCenter, float2(8,4)), rot2), 2, float3(1,1,1));
		DrawCircleFilled(add2(add2(ThingyCenter, float2(6,6)), rot2), 2, float3(1,1,1));
	}
}

typedef struct 
{
    extents2 dimension;
    ArrayCreate(float2*, points, 25);
    ArrayCreate(int, pointCount, 25);
    ArrayCreate(float3, color, 25);
    ArrayCreate(const char*, title, 25);
    
} Plot;

void DrawPlotPrepareData(Plot* plot, float2* points, int pointCount, float3 color, const char* title)
{
    ArrayAdd(plot->points, points);
    ArrayAdd(plot->pointCount, pointCount);
    ArrayAdd(plot->color, color);
    ArrayAdd(plot->title, title);
    for (int i = 0; i < pointCount; i++)
    {
        plot->dimension.xMin = min(plot->dimension.xMin, points[i].x);
        plot->dimension.yMin = min(plot->dimension.yMin, points[i].y);
        plot->dimension.xMax = max(plot->dimension.xMax, points[i].x);
        plot->dimension.yMax = max(plot->dimension.yMax, points[i].y);
    }
}
void DrawPlot(Plot* plot, float2 pos, float2 size)
{
    DrawBoxFilled(pos, size, float3(0, 0, 0));
    for (int j = 0; j < ArrayCount(plot->points); j++)
    {
        float2* points = plot->points[j];
        int pointCount = plot->pointCount[j];
        float3 color = plot->color[j];
        const char* title = plot->title[j];
        
        float2 lastPoint = {};
        lastPoint.x = remap(points[0].x, plot->dimension.xMin, plot->dimension.xMax, pos.x, pos.x + size.x);
        lastPoint.y = remap(points[0].y, plot->dimension.yMin, plot->dimension.yMax, pos.y, pos.y + size.y);
        for (int i = 0; i < pointCount; i++)
        {
            float2 newPoint = {};
            newPoint.x = remap(points[i].x, plot->dimension.xMin, plot->dimension.xMax, pos.x, pos.x + size.x);
            newPoint.y = remap(points[i].y, plot->dimension.yMin, plot->dimension.yMax, pos.y, pos.y + size.y);
            DrawLine(newPoint, lastPoint, color, 1);
            lastPoint = newPoint;
        }

        DrawString(str(title), float2(pos.x, pos.y + 32 * j), 32, color);
    }
    
    float positionsX[3] = {plot->dimension.xMin, (plot->dimension.xMin+ plot->dimension.xMax) * 0.5, plot->dimension.xMax };
    float positionsY[3] = {plot->dimension.yMin, (plot->dimension.yMin+ plot->dimension.yMax) * 0.5, plot->dimension.yMax };

    for (int i = 0; i < 3; i++)
    {
        StringCreate(testString, 100);
        testString = StringAppendFloat(testString, positionsX[i]);
        DrawString(testString, float2(pos.x+(size.x/2)*i-16, pos.y+size.y+16), 16, float3(1, 1, 1));
    }
    for (int i = 0; i < 3; i++)
    {
        StringCreate(testString, 100);
        testString = StringAppendFloat(testString, positionsY[i]);
        DrawString(testString, float2(pos.x+size.x+16, pos.y+(size.y/2)*i-16), 16, float3(1, 1, 1));
    }
    
}
