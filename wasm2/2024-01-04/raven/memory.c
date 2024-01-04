#pragma once

// Checks if two blocks of memory are the same
bool Equals(uint8* a, uint8* b, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (a[i] != b[i])
            return false;
    }
    return true;
}

// Copies memory from one location to another
void Copy(uint8* destination, uint8* source,  int size)
{
    //globalMemcpy(destination, source, size);
    for (int i = 0; i < size; i++)
    {
        destination[i] = source[i];
    }
}

// Sets a block of memory to 0
void Clear(uint8* destination, int size)
{
    //globalMemset(destination, 0, size);
    for (int i = 0; i < size; i++)
    {
        destination[i] = 0;
    }
}

// Copies a struct somewhere in memory
#define StructCopy(myStruct, destination) Copy(myStruct, destination, sizeof(myStruct))

// Clears a struct to 0
#define StructClear(myStruct) Clear((uint8*)myStruct, sizeof(*myStruct))


// Memory arena
typedef struct
{
    uint32 size;
    uint8* base;
    uint32 used;
    uint8* end;
} MemoryArena;

MemoryArena ArenaInitialize(uint32 size, void* base, const char* name)
{
    MemoryArena arena = {};
    arena.size = size;
    arena.base = (uint8*)base;
    arena.used = 0;
    arena.end = arena.base + arena.size;
    return arena;
}

void ArenaReset(MemoryArena* arena)
{
    arena->used = 0;
}

#define ArenaPushStruct(arena, type, name) (type*)ArenaPushBytes(arena, sizeof(type), name) // pushes 1x of a struct and returns pointer to start of it.
#define ArenaPushArray(arena, Count, type, name) (type*)ArenaPushBytes(arena, (Count)*sizeof(type), name) // pushes Nx of a struct and returns pointer to start of it.
void* ArenaPushBytes(MemoryArena* arena, uint32 size, const char* name)
{
    Assert(arena->base); // Arena is not initialized
    Assert((arena->used + (size)) <= arena->size); // Arena ran out of memory
    uint8* result = arena->base + arena->used;
    arena->used += size;
    return result;
}
