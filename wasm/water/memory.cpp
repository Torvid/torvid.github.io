#pragma once
#include "quote.h"

// Arrays
//#define ArrayCreate(type, name, size) int name##_count; type name[size];
#define ArrayCount(name) name##_count
//#define ArrayCreate(name) {}; int name##_count = 0;
#define ArrayCreate(type, name, size) type name[size] = {}; int name##_count = 0;
bool IndexIsInArray(int index, int size)
{
    return (index >= 0) && (index < size);
}
#define IsInArray(name, index) IndexIsInArray(index, ArrayCapacity(name))

#define ArrayFull(name) (ArrayCount(name) >= ArrayCapacity(name))

#define ArrayAdd(name, var) \
{ \
    ArrayCount(name)++; \
    Assert(sizeof(name[0]) == sizeof(var), "Item is a different size than other items in the array"); \
    Assert(IsInArray(name, ArrayCount(name) - 1), "Array is out of capacity."); \
    name[ArrayCount(name)-1] = var; \
}

// scan backwards, moving items to the right by one, then insert.
#define ArrayInsert(name, var, index) \
{ \
    ArrayCount(name)++; \
    for (int _itter = ArrayCount(name); _itter >= index; _itter--) \
    { \
        name[_itter] = name[_itter-1]; \
    } \
    name[index] = var; \
} \
//void _ArrayInsert()
//{
//
//}


#define ArrayAddIndex(name) (&(name[ArrayCount(name)])); \
{ \
    ArrayCount(name)++; \
    Assert(IsInArray(name, ArrayCount(name) - 1), "Array is out of capacity."); \
} \

#define ArrayGetLast(name) (&(name[ArrayCount(name)-1]))

#define ArrayRemove(name, index) \
{ \
    Assert(IsInArray(name, index)); \
    ArrayCount(name)--; \
    name[index] = name[ArrayCount(name)]; \
} \

#define ArrayClear(name) ArrayCount(name) = 0;

// AddRange
#define ArrayAddRange(name, other) \
{ \
    int RemainingCapacity = ArrayCapacity(name) - ArrayCount(name); \
    Assert(RemainingCapacity > ArrayCount(other), "Array does not fit."); \
    Assert(sizeof(name[0]) == sizeof(other[0]), "Arrays have different item sizes."); \
    for (int _itter = 0; _itter < ArrayCount(other); _itter++) \
    { \
        ArrayAdd(name, other[_itter]); \
    } \
} \
// Resize


//void _ArrayAdd(uint8* array, uint8* var, int itemsize, int* length, int capacity)
//{
//    (*length)++;
//    bool in = IndexIsInArray((*length) - 1, capacity);
//    Assert(in, "Array is out of capacity.");
//    Copy(var, &array[((*length) - 1) * itemsize], itemsize);
//}
//
//#define ArrayAdd(name, var) { \
//    auto local = var; \
//    _ArrayAdd((uint8*)&name, (uint8*)&local, sizeof(name[0]), &ArrayCount(name), ArrayCapacity(name)); \
//} \


//#define ArrayRemove(name, var) \
//{ \
//    bool found = false; \
//    for (int i = 0; i < ArrayCount(name); i++) \
//    { \
//        if(name[i] == var && !found) \
//            found = true; \
//        if (found) \
//            name[i] = name[i + 1]; \
//    } \
//    if(found) \
//        ArrayCount(name)--;\
//} \
//

//void _ArrayRemove(uint8* array, uint8* item, int itemsize, int* length)
//{
//    bool found = false;
//    for (int i = 0; i < ((*length) * itemsize); i += itemsize)
//    {
//        if (equals(&(array[i]), item, itemsize) && !found)
//            found = true;
//        if (found)
//            copy(&array[i], &array[i * itemsize], itemsize);
//    }
//    if (found)
//        (*length)--;
//}



void array_test(GameMemory* memory)
{
    //ArrayCreate(int, Test, 100);

    ArrayCreate(int, test, 32)
        ArrayCreate(int, test2, 16)
        ArrayAdd(test2, 100);
    ArrayAdd(test2, 101);
    ArrayAdd(test2, 102);
    ArrayAdd(test2, 103);
    ArrayAdd(test2, 104);
    ArrayAdd(test2, 105);

    int length = ArrayCount(test);

    // all macros all the way
    ArrayAdd(test, 1);
    ArrayAdd(test, 2);
    ArrayAdd(test, 3);
    ArrayAdd(test, 4);

    ArrayRemove(test, 0);
    ArrayAdd(test, 5);

    ArrayAddRange(test, test2);


    for (int i = 0; i < ArrayCount(test); i++)
    {
        test[i] *= 2;
    }

}




// Utilities


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
void Copy(uint8* source, uint8* destination, int size)
{
    for (int i = 0; i < size; i++)
    {
        destination[i] = source[i];
    }
}

// Sets a block of memory to 0
void Clear(void* destination, int size)
{
    for (int i = 0; i < size; i++)
    {
        ((uint8*)destination)[i] = 0;
    }
}

// Copies a struct somewhere in memory
#define StructCopy(myStruct, destination) Copy(myStruct, destination, sizeof(myStruct))

// Clears a struct to 0
#define StructClear(myStruct) Clear((uint8*)myStruct, sizeof(*myStruct))



// Memory arena

struct MemoryArena
{
    uint32 size;
    uint8* base;
    uint32 used;
    //ArrayCreate(uint32, memoryArenaEntrys, 100); // max 100 things?
};

void InitializeArena(MemoryArena* arena, uint32 Size, uint8* Base)
{
    arena->size = Size;
    arena->base = Base;
    arena->used = 0;

    // clear it all to 0
    Clear(Base, Size);
}

//#define PushBytes(arena, Count) (char*)PushSize(arena, Count)
#define PushStruct(arena, type) (type*)PushBytes(arena, sizeof(type)) // pushes 1x of a struct and returns pointer to start of it.
#define PushArray(arena, Count, type) (type*)PushBytes(arena, (Count)*sizeof(type)) // pushes Nx of a struct and returns pointer to start of it.
void* PushBytes(MemoryArena* arena, uint32 size)
{
    Assert((arena->used + size) <= arena->size); // Assert that we won't overrun memory

    void* Result = arena->base + arena->used;
    arena->used += size;

    return Result;
}
