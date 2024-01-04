// Win32 platform layer.

// minimal-ish win32 code for getting a window open that we can draw to, update loop, reading image files and getting mouse input and time

#include "../raven/platform.c"
//#include "../memory.h"

#include <windows.h>
#include <stdio.h>
#include <immintrin.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define printf WindowsPrint
int __cdecl WindowsPrint(const char* format, ...)
{
    char str[1024];

    va_list argptr;
    va_start(argptr, format);
    int ret = vsnprintf(str, sizeof(str), format, argptr);
    va_end(argptr);

    OutputDebugStringA(str);

    return ret;
}

bool running = true;
Platform platform = {};
BITMAPINFO backbufferInfo;
void* backbufferData;
GameUpdateAndRender* gameUpdateAndRender;
HMODULE GameCodeDLL;

void FileFree(void* contents)
{
    VirtualFree(contents, null, MEM_RELEASE);
}

void FileRead(const char* filename, void** contents)
{
    HANDLE fileHandle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (fileHandle == INVALID_HANDLE_VALUE)// failed to open file
        return;

    LARGE_INTEGER size;
    if (!GetFileSizeEx(fileHandle, &size)) // failed to get size
        return;

    // can't load files larger than 4 gb
    uint32 fileSize32 = (uint32)size.QuadPart;
    
	*contents = (uint8*)VirtualAlloc(0, fileSize32, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	// failed to allocate memoy
	if (!*contents)
        return;

    DWORD bytesRead = 0;
    if (!ReadFile(fileHandle, *contents, fileSize32, &bytesRead, 0)) // failed to read file
    {
        FileFree(*contents);
		CloseHandle(fileHandle);
        return;
    }

    if (bytesRead != fileSize32) // we didn't end up reading the entire file
    {
        FileFree(*contents);
		CloseHandle(fileHandle);
        return;
    }

    CloseHandle(fileHandle);
}

void Win32PlatformRequestImageRead(const char* path, Image* image)
{
    // Load image
    int sizeX;
    int sizeY;
    int channels;
	
    uint8* img = stbi_load(path, &sizeX, &sizeY, &channels, 4);
    Assert(img != 0)

	image->sizeX = sizeX;
	image->sizeY = sizeY;
	
	image->data = platform.lastImageDataLocation;
	platform.lastImageDataLocation += sizeX * sizeY;

    // Copy image into game memory
	for (int i = 0; i < sizeX * sizeY; i++)
	{
		((uint8*)image->data)[i*4+0] = img[i*4+2];
		((uint8*)image->data)[i*4+1] = img[i*4+1];
		((uint8*)image->data)[i*4+2] = img[i*4+0];
		((uint8*)image->data)[i*4+3] = img[i*4+3];
	}
		
    stbi_image_free(img);
}

void ResizeBackbuffer(void* buffer, BITMAPINFO* info, int* sizeX, int* sizeY) // resizes backbuffer
{
    info->bmiHeader.biSize = sizeof(info->bmiHeader);
    info->bmiHeader.biWidth = (*sizeX);
    info->bmiHeader.biHeight = -(*sizeY); // negative SizeY makes the image top down instead of bottom up.
    info->bmiHeader.biPlanes = 1;
    info->bmiHeader.biBitCount = 32;
    info->bmiHeader.biCompression = BI_RGB;
}

void GetWindowSize(HWND window, int* sizeX, int* sizeY)
{
    RECT WindowRect;
    GetClientRect(window, &WindowRect);
    *sizeX = WindowRect.right - WindowRect.left;
    *sizeY = WindowRect.bottom - WindowRect.top;
}

void CopyBufferToWindow(void* buffer, BITMAPINFO* info, HDC device, int sizeX, int sizeY)
{
    StretchDIBits(device, 0, 0, sizeX, sizeY, 0, 0, sizeX, sizeY, buffer, info, DIB_RGB_COLORS, SRCCOPY);
}

static int LastIndex;
const int max_file_count = 2000;
static int64 times1[max_file_count];
static char names1[max_file_count][100];

int ends_with(const char* str, const char* suffix)
{
	size_t str_len = strlen(str);
	size_t suffix_len = strlen(suffix);

	return (str_len >= suffix_len) && (!memcmp(str + str_len - suffix_len, suffix, suffix_len));
}

void dirListFiles(char* startDir, int64* times, int* index, bool* CodeFileChanged, bool* AssetFileChanged)
{
	HANDLE hFind;
	WIN32_FIND_DATA wfd;
	char path[MAX_PATH];

	sprintf(path, "%s\\*", startDir);
	
	if ((hFind = FindFirstFile(path, &wfd)) == INVALID_HANDLE_VALUE)
	{
		return;
	}
	while (true)
	{
		if ((strncmp(".", wfd.cFileName, 1) != 0) && (strncmp("..", wfd.cFileName, 2) != 0))
		{
			bool isDirectory = wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
			if (isDirectory)
			{
				sprintf(path, "%s\\%s", startDir, wfd.cFileName);
				dirListFiles(path, times, index, CodeFileChanged, AssetFileChanged);
			}
			else
			{
				bool isCode = 
					ends_with(wfd.cFileName, ".cpp") ||
					ends_with(wfd.cFileName, ".h") ||
					ends_with(wfd.cFileName, ".c");
					
				bool isAsset = 
					ends_with(wfd.cFileName, ".tga") ||
					ends_with(wfd.cFileName, ".obj") ||
					ends_with(wfd.cFileName, ".png") ||
					ends_with(wfd.cFileName, ".hlsl") ||
					ends_with(wfd.cFileName, ".wav") ||
					ends_with(wfd.cFileName, ".shader") ||
					ends_with(wfd.cFileName, ".shaderinc") ||
					ends_with(wfd.cFileName, ".mesh") ||
					ends_with(wfd.cFileName, ".anim") ||
					ends_with(wfd.cFileName, ".rad") ||
					ends_with(wfd.cFileName, ".hdr");

				if (isCode || isAsset)
				{
					Assert(*index < max_file_count)
					int64 date = ((int64)wfd.ftLastWriteTime.dwHighDateTime << 32) + wfd.ftLastWriteTime.dwLowDateTime;
					if (date != times[(*index)] || strcmp(names1[*index], wfd.cFileName))
					{
						if (isCode)
						{
							*CodeFileChanged = true;
						}
						if (isAsset)
						{
							*AssetFileChanged = true;
						}
					}

					strcpy(names1[*index], wfd.cFileName);
					times[(*index)] = date;
					(*index)++;
				}
			}
		}

		bool fileFound = FindNextFile(hFind, &wfd);
		if(!fileFound)
			break;
	}
	if (GetLastError() != ERROR_NO_MORE_FILES)
	{
		return;
	}
	if (FindClose(hFind) == FALSE)
	{
		return;
	}
}

bool ReloadFilesChanged(bool* assetChanged)
{
	bool CodeFileChanged = false;
	bool AssestFileChanged = false;
	int index = 0;
    TCHAR buffer[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, buffer);
	dirListFiles(buffer, times1, &index, &CodeFileChanged, &AssestFileChanged);
	if (assetChanged && AssestFileChanged)
		*assetChanged = true;
	
	// number of files changed
	if (LastIndex != index)
	{
		LastIndex = index;
		CodeFileChanged = true;
		AssestFileChanged = true;
	}
	
	return CodeFileChanged || AssestFileChanged;
}

LRESULT MainWindowCallback(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT Result = 0;
    switch (message)
    {
		case WM_SIZE:
		{
			GetWindowSize(window, &platform.videoSizeX, &platform.videoSizeY);
			ResizeBackbuffer(backbufferData, &backbufferInfo, &platform.videoSizeX, &platform.videoSizeY);
			if(gameUpdateAndRender)
				gameUpdateAndRender(&platform);

		} break;
		case WM_DESTROY: running = false; break;
		case WM_CLOSE: running = false; break;
		case WM_PAINT:
		{
			PAINTSTRUCT paint;
			HDC deviceContext = BeginPaint(window, &paint);
			int sizeX;
			int sizeY;
			GetWindowSize(window, &sizeX, &sizeY);
			CopyBufferToWindow(backbufferData, &backbufferInfo, deviceContext, sizeX, sizeY);
			ReleaseDC(window, deviceContext);
			EndPaint(window, &paint);
		} break;
		default:
		{
			Result = DefWindowProcA(window, message, wParam, lParam);
		} break;
    }
    return Result;
}

int64 Win32PlatformTime()
{
    LARGE_INTEGER frequency;
    LARGE_INTEGER result;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&result);
    return result.QuadPart;
}

int64 Win32PlatformFrequency()
{
    LARGE_INTEGER frequency;
    LARGE_INTEGER result;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&result);
    return frequency.QuadPart;
}

void Win32PlatformPrint(const char* string)
{
    printf(string);
}
bool FileExists(const char* file)
{
    WIN32_FIND_DATA FindFileData;
    HANDLE handle = FindFirstFile(file, &FindFileData);
    int found = handle != INVALID_HANDLE_VALUE;
    if (found)
    {
        //FindClose(&handle); this will crash
        FindClose(handle);
    }
    return (bool)found;
}
int64 timeLast;

// entry point
int WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCode)
{
    // The game gets 500 Megabytes of memory to play with.
    platform.memory = (uint8*)VirtualAlloc(0, Megabytes(500), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    platform.RequestImageRead = &Win32PlatformRequestImageRead;
    platform.Print = &Win32PlatformPrint;
    platform.time = &Win32PlatformTime;
	
    WNDCLASS windowClass = {};
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = (WNDPROC)MainWindowCallback;
    windowClass.hInstance = GetModuleHandle(0);
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.lpszClassName = L"HelloWindowClass";

    if (!RegisterClass(&windowClass))
        return 0;
	
    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    RECT windowrect;
    windowrect.left = 0;
    windowrect.top = 0;
    windowrect.right = 1920;
    windowrect.bottom = 1080;
    AdjustWindowRect(&windowrect, style, false);
    int windowSizeX = windowrect.right - windowrect.left;
    int windowSizeY = windowrect.bottom - windowrect.top;
    HWND window = CreateWindowEx(0, windowClass.lpszClassName, "Breakout", style, CW_USEDEFAULT, CW_USEDEFAULT, windowSizeX, windowSizeY, 0, 0, instance, null);
    if (!window)
        return 0;

	GameCodeDLL = LoadLibraryA("game.dll");
	if (GameCodeDLL)
	{
		gameUpdateAndRender = (GameUpdateAndRender*)GetProcAddress(GameCodeDLL, "Update");
	}

    while (running)
    {
        MSG message;
        while (PeekMessage(&message, null, null, null, PM_REMOVE))
        {
            switch (message.message)
            {
				case WM_QUIT: running = false; break;
				case WM_LBUTTONDOWN: platform.mouseLeft = true; break;
				case WM_LBUTTONUP: platform.mouseLeft = false; break;
				case WM_MBUTTONDOWN: platform.mouseMiddle = true; break;
				case WM_MBUTTONUP: platform.mouseMiddle = false; break;
				case WM_RBUTTONDOWN: platform.mouseRight = true; break;
				case WM_RBUTTONUP: platform.mouseRight = false; break;
				case WM_SYSKEYDOWN:
				case WM_SYSKEYUP:
				case WM_KEYDOWN:
				case WM_KEYUP:
				{
					bool WasDown = (message.lParam & (1 << 30)) != 0;
					bool IsDown = (message.lParam & (1 << 31)) == 0;

					if (WasDown != IsDown)
					{
						uint32 VKCode = (uint32)message.wParam;
						if(((VKCode - 65) >= 0) && ((VKCode - 65) < 26))
							platform.letters[VKCode - 65].held = IsDown;

						if(((VKCode - 48) >= 0) && ((VKCode - 48) < 10))
							platform.numbers[VKCode - 48].held = IsDown;

						if (VKCode == VK_SPACE)     platform.space = IsDown;
						if (VKCode == VK_BACK)      platform.backspace = IsDown;
						if (VKCode == VK_RETURN)    platform.enter = IsDown;
						if (VKCode == VK_ESCAPE)    platform.escape = IsDown;
						if (VKCode == VK_DELETE)    platform.delete = IsDown;
						if (VKCode == VK_UP)        platform.up = IsDown;
						if (VKCode == VK_DOWN)      platform.down = IsDown;
						if (VKCode == VK_LEFT)      platform.left = IsDown;
						if (VKCode == VK_RIGHT)     platform.right = IsDown;
						if (VKCode == VK_SHIFT)     platform.shift = IsDown;
						if (VKCode == VK_CONTROL)   platform.ctrl = IsDown;
						if (VKCode == VK_MENU)      platform.alt = IsDown;
					}
					// Alt + F4 close
					bool AltKeyIsDown = (message.lParam & (1 << 29)) != 0;
					if (AltKeyIsDown && message.wParam == VK_F4)
					{
						running = false;
					}

					// Esc key close
					if (message.wParam == VK_ESCAPE)
					{
						//GlobalRunning = false;
					}
					
				} break;
				case WM_MOUSEWHEEL:
				{
					platform.mouseZ += (float)(GET_WHEEL_DELTA_WPARAM(message.wParam) / 120);
				} break;
				default:
				{
					TranslateMessage(&message);
					DispatchMessage(&message);
				}
            }
        }
		
		RECT windowRect;
		GetWindowRect(window, &windowRect);
		POINT cursorPoint;
		GetCursorPos(&cursorPoint);
        platform.mouseX = cursorPoint.x - windowRect.left + windowrect.left;
        platform.mouseY = cursorPoint.y - windowRect.top + windowrect.top;
		
        if (FileExists("win32_hotreload.bat"))
        {
			bool assetChanged = false;
			bool fileChanged = ReloadFilesChanged(&assetChanged);
			
			if (fileChanged)
			{
				// unload game code
				if (GameCodeDLL)
					FreeLibrary(GameCodeDLL);
				gameUpdateAndRender = 0;
				
				system("cmd /C win32_hotreload.bat");
				
				GameCodeDLL = LoadLibraryA("game.dll");
				if (GameCodeDLL)
					gameUpdateAndRender = (GameUpdateAndRender*)GetProcAddress(GameCodeDLL, "Update");
				
				ReloadFilesChanged(&assetChanged);
			}
        }

		for (int i = 0; i < 26; i++)
		{
			UpdateButton(&platform.letters[i], platform.letters[i].held);
		}
		for (int i = 0; i < 10; i++)
		{
			UpdateButton(&platform.numbers[i], platform.letters[i].held);
		}
		UpdateButton(&platform.Statespace, 				platform.space);
		UpdateButton(&platform.Statebackspace,  		platform.backspace);
		UpdateButton(&platform.Stateenter,  			platform.enter);
		UpdateButton(&platform.Stateescape,  			platform.escape);
		UpdateButton(&platform.Statedelete,  			platform.delete);
		UpdateButton(&platform.Stateup ,  				platform.up);
		UpdateButton(&platform.Statedown ,  			platform.down);
		UpdateButton(&platform.Stateleft ,  			platform.left);
		UpdateButton(&platform.Stateright ,  			platform.right);
		UpdateButton(&platform.Stateshift ,  			platform.shift);
		UpdateButton(&platform.Statectrl ,  			platform.ctrl);
		UpdateButton(&platform.Statealt ,  				platform.alt);
		UpdateButton(&platform.StatemouseLeft ,  		platform.mouseLeft);
		UpdateButton(&platform.StatemouseMiddle ,  		platform.mouseMiddle);
		UpdateButton(&platform.StatemouseRight ,  		platform.mouseRight);
		int64 freq = Win32PlatformFrequency();
		int64 time = Win32PlatformTime();
		float delta = (time - timeLast) / (float)freq;
		platform.deltaTime = delta;
		timeLast = time;
		if(gameUpdateAndRender)
			gameUpdateAndRender(&platform);
			
        backbufferData = platform.videoBuffer;

        // Copy rendered image to screen
        HDC deviceContext = GetDC(window);
		int sizeX;
		int sizeY;
		GetWindowSize(window, &sizeX, &sizeY);
        CopyBufferToWindow(backbufferData, &backbufferInfo, deviceContext, sizeX, sizeY);
        ReleaseDC(window, deviceContext);
    }
	
    return 1;
}
