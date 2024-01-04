#pragma once
// includes with no dependencies
#include "platform.c"
#include "array.c"
#include "memory.c"
#include "math.c"
#include "string.c"

typedef struct
{
	bool initialized;
	MemoryArena imagesArena;
	MemoryArena programArena;
	Image testImage;
	
	float time;
	float deltaTime;
	float2 videoSize;
	PlatformPrint* Print;
	
	Image font0;
	Image font1;
	Image font2;
	Image font3;
	Image font4;
	float2 mousePos;
	
} Game;

// Globals
static Platform* platform;
static Game* game;

void ImageLoad(const char* path, Image* image)
{
	// Fill out temp image
	image->sizeX = 256;
	image->sizeY = 256;
	image->data = (uint32*)&platform; // missing image image can be whatever
	platform->RequestImageRead(path, image);
	platform->Print(path);
	//platform->ReadImageSize(path, &result.sizeX, &result.sizeY);
	//result.data = (uint32*)ArenaPushBytes(&game->imagesArena, result.sizeX * result.sizeY * 4, path);
	//platform->ReadImage(path, result.data);
	//return result;
}

// include that depend on globals
#include "draw.c"
#include "ui.c"

#include "../program.c"

// exposed main function, called once per frame.
#if _WIN32
__declspec(dllexport)
#endif
void Update(Platform* inPlatform)
{
	platform = inPlatform;
	game = (Game*)platform->memory;
	game->time += platform->deltaTime;
	game->deltaTime = platform->deltaTime;
	// place the video buffer somewhere far out in memory, away from the game stuff.
	platform->videoBuffer = platform->memory + Megabytes(128);
	game->videoSize = float2(platform->videoSizeX, platform->videoSizeY); 
	game->mousePos = float2(platform->mouseX, platform->mouseY);
	game->Print = platform->Print;
	
	if(platform->spaceDown)
	{
		game->initialized = false;
	}
	
	if (!game->initialized)
	{
		game->imagesArena = ArenaInitialize(Megabytes(64), platform->memory + sizeof(Game), "Images Arena");
		game->programArena = ArenaInitialize(Megabytes(32), game->imagesArena.end, "Program Arena");
		platform->lastImageDataLocation = (uint32*)game->imagesArena.base;

		ArenaReset(&game->imagesArena);
		
		game->time = 0;
		Clear(platform->videoBuffer, platform->videoSizeX * platform->videoSizeY * 4);
		game->initialized = true;
		ImageLoad("raven/font0.png", &game->font0);
		ImageLoad("raven/font1.png", &game->font1);
		ImageLoad("raven/font2.png", &game->font2);
		ImageLoad("raven/font3.png", &game->font3);
		ImageLoad("raven/font4.png", &game->font4);
		ProgramStart();
	}

	ProgramUpdate();
}