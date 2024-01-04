#pragma once
#include "raven/platform.c"
#include "raven/array.c"
#include "raven/memory.c"
#include "raven/math.c"
#include "raven/string.c"
#include "raven/draw.c"
#include "raven/ui.c"

typedef struct
{
    int test0;
    int test1;

	Image test3;
	bool testButton;

    float2 followerLocation;
    float followerAngle;
    
	ArrayCreate(float2, plotPositions0, 1000);
	ArrayCreate(float2, plotPositions1, 1000);
} Program;

static Program* program;

float angleBetween(float angle0, float angle1)
{
    return mod(angle0 - angle1 + tau + pi, tau) - pi;
}
float angleOf(float2 vector)
{
    return atan2(vector.x, vector.y);
}
float rotateTowards(float from, float to, float dist)
{
    return sign(angleBetween(from, to)) * dist;
}
float angleFromTo(float2 pos0, float2 pos1)
{
    return angleOf(sub2(pos0, pos1));
}

void ProgramStart()
{
    program = (Program*)game->programArena.base;
	// make sure we didn't fuck ourselves and make a program struct that's too big!
    Assert(sizeof(Program) < game->programArena.size);

    // fill the two plots with data
    ArrayClear(program->plotPositions0);
    for (int i = 0; i < ArrayCapacity(program->plotPositions0); i++)
    {
        ArrayAdd(program->plotPositions0, float2(cos(i)*200+500, sin(i)*200+500));
    }
    ArrayClear(program->plotPositions1);
    for (int i = 0; i < ArrayCapacity(program->plotPositions1); i++)
    {
        ArrayAdd(program->plotPositions1, float2(cos(i)*300+350, sin(i)*100+300));
    }

    ImageLoad("raven/test3.png", &program->test3);
}

void ProgramUpdate()
{
    program = (Program*)game->programArena.base;
    
	DrawClear(float3(0.5, 0, 0));

	DrawImage(&program->test3, float2(1200, 400));
	StringCreate(amongus, 1000);
	amongus = StringAppendFloat2(amongus, float2(1, 2));
	amongus = StringAppend(amongus, ("\n"));
	amongus = StringAppend(amongus, ("~~~~Lorem Ipsum is simply dummy text of the printing and typesetting industry. \nLorem Ipsum has been the industry's standard dummy text ever since the 1500s,\nwhen an unknown printer took a galley of type and scrambled it to make a \ntype specimen book. It has survived not only five centuries, but also the\nleap into electronic typesetting, remaining essentially unchanged. It was \npopularised in the 1960s with the release of Letraset sheets containing \nLorem Ipsum passages, and more recently with desktop publishing software \nlike Aldus PageMaker including versions of Lorem Ipsum."));
	amongus = StringAppend(amongus, ("\n\n\n"));
	amongus = StringAppend(amongus, ("float sqrtfast(float x)\n{\n    int i = asint(x);\n    i = 0x1FBD1DF5 + (i >> 1);\n    return asfloat(i);\n}\n"));
	DrawString(amongus, float2(0, 16), 32, float3(1,1,1));

	DrawBox(float2(200, 200), float2(100, 200), float3(0.0, 0.5, 0.0), false);
	DrawBox(float2(400, 400), float2(150, 150), float3(0.0, 0.5, 0.0), false);
	DrawCircle(float2(700, 700), 600, float3(0, 0, 0.5), 1);
	DrawDemo(float2(800, 100));

	StringCreate(framerate, 100);
	framerate = StringAppend(framerate, "FPS: ");
	framerate = StringAppendInt(framerate, (int)(1.0f / platform->deltaTime));
	framerate = StringAppend(framerate, "\nMS: ");
	framerate = StringAppendInt(framerate, (int)(platform->deltaTime*1000));
	DrawString(framerate, float2(30, 600), 64, float3(1, 1, 1));
	
	//Plot plot = {};
	//DrawPlotPrepareData(&plot, program->plotPositions0, ArrayCount(program->plotPositions0), float3(1,1,0), "fist circle");
	//DrawPlotPrepareData(&plot, program->plotPositions1, ArrayCount(program->plotPositions1), float3(1,0,1), "second circle");
	//DrawPlot(&plot, float2(500, 400), float2(500, 500));

	float2 hitClose;
	float2 hitFar;
	float2 rayStart = float2(platform->mouseX, platform->mouseY);
	float2 rayDirection = float2(400, 100);
	float2 rayEnd = add2(rayStart, rayDirection);
	float2 boxPos = float2(100, 100);
	float2 boxSize = float2(600, 300);
	DrawBox(boxPos, boxSize, float3(0, 1, 0), 1);
	DrawLine(rayStart, add2(rayStart, rayDirection), float3(0, 1, 0), 1);
    
	
	DrawCircle(float2(400, 400), 200, float3(0.0, 0.5, 0.0), 23);
	DrawCircle(float2(450, 450), 200, float3(0.5, 0.5, 0.0), 16);
	DrawCircle(float2(500, 500), 200, float3(0.0, 0.5, 0.5), 9);

    DrawBox(float2(400, 100), float2(200, 200), float3(0.0, 0.5, 0.0), 25);
    DrawBox(float2(450, 150), float2(200, 200), float3(0.5, 0.5, 0.0), 12);
    DrawBox(float2(500, 200), float2(200, 200), float3(0.0, 0.5, 0.5), 5);

	program->testButton = UIToggle(program->testButton, float2(200, 200), str("amongus"));
	if(UIButton(float2(200, 250), str("amongus")))
	{
		
	}

	

}