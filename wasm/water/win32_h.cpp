

struct win32_game_code
{
    HMODULE gameCodeDLL;
    // function pointer can be null, this means it failed to load.
    game_update_and_render* gameUpdateAndRender;
    FILETIME LastWriteTime;
    bool32 IsValid;
};

struct win32_debug_time_marker
{
    DWORD PlayCursor;
    DWORD WriteCursor;
};


struct win32_offscreen_buffer // always 4 bytes per pixel BGRA
{
    BITMAPINFO Info;
    void* memory;
    int width;
    int height;
    int bytesPerPixel;
    int pitch; // Distance to get to the next line
};

struct win32_sound_output
{
    int samplesPerSecond;
    int bytesPerSample;
    int bufferSize;
    uint32 runningSampleIndex;
    int latencySampleCount;
};

struct win32_window_dimensions
{
    int width;
    int height;
};

#define WIN32_STATE_FILE_NAME_COUNT MAX_PATH

struct win32_replay_buffer
{
    HANDLE FileHandle;
    HANDLE MemoryMap;
    char Filename[WIN32_STATE_FILE_NAME_COUNT];
    void* MemoryBlock;
};

struct win32_state
{
    uint64 TotalSize;
    void* GameMemoryBlock;
    win32_replay_buffer ReplayBuffers[4];

    HANDLE RecordingHandle;
    int InputRecordingIndex;

    HANDLE PlaybackHandle;
    int InputPlayingIndex;


    char ExeFilename[WIN32_STATE_FILE_NAME_COUNT];
    char* OnePastLastSlash;
};

