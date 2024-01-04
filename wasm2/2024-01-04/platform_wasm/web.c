// Web platform layer (also in index.html)

#include "../raven/platform.c"
#include "../raven/game.c"

extern unsigned char* __data_end;
extern unsigned char* __heap_base;

extern void WebPlatformRequestImageRead(const char* path, Image* image);
extern void WebPlatformPrint(const char* string);

__attribute__((export_name("WebPlatformGetLastImageLocation")))
uint32* WebPlatformGetLastImageLocation()
{
    return platform->lastImageDataLocation;
}

__attribute__((export_name("WebPlatformRequestImageReadDone")))
void WebPlatformRequestImageReadDone(const char* path, Image* image, int sizeX, int sizeY)
{
    image->sizeX = sizeX;
    image->sizeY = sizeY;
    image->data = platform->lastImageDataLocation;
    platform->lastImageDataLocation += sizeX * sizeY;
}

__attribute__((export_name("KeyDown")))
void KeyDown(int keyCode)
{
    //KeyChanged(keyCode, true);
}

__attribute__((export_name("KeyUp")))
void KeyUp(int keyCode)
{
    //KeyChanged(keyCode, false);
}

// Mouse
__attribute__((export_name("MouseMove")))
void MouseMove(int mouseX, int mouseY)
{
    platform->mouseX = mouseX;
    platform->mouseY = mouseY;
}

__attribute__((export_name("MouseScroll")))
void MouseScroll(int MouseZ)
{
    platform->mouseZ += MouseZ;
}

__attribute__((export_name("MouseDown")))
void MouseDown(int which)
{
    if(which == 0)
        platform->mouseLeft = true;
    else if (which == 2)
        platform->mouseRight = true;
    else
        platform->mouseMiddle = true;
}

__attribute__((export_name("MouseUp")))
void MouseUp(int which)
{
    if(which == 0)
        platform->mouseLeft = false;
    else if (which == 2)
        platform->mouseRight = false;
    else
        platform->mouseMiddle = false;
}



static float timestampLast;
// called once per frame from JS
__attribute__((export_name("WebUpdate")))
void WebUpdate(float deltaTime, int SizeX, int SizeY)
{
    char* memory = (char*)(&__heap_base) + Megabytes(8); // add 8 megabytes to leave room for the stack and stuff
    Platform* platform = (Platform*)memory;
    platform->memory = memory + Megabytes(8);
    platform->RequestImageRead = &WebPlatformRequestImageRead;
    platform->Print = &WebPlatformPrint;
    platform->videoSizeX = SizeX;
    platform->videoSizeY = SizeY;
	platform->deltaTime = deltaTime;
    Update(platform);
}