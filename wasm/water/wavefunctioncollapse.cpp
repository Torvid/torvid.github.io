#pragma once
#include "quote.h"

// TODO: If you look into this more, make it propegate the probabilities fully, not just one loop.

//#include "math.cpp"
//#include "random.cpp"
//#include "drawing.cpp"
//#include "ui.cpp"
//#include "array.cpp"

enum CellType
{
    CellType_Tree = 0,
    CellType_Grass = 1,
    CellType_Sand = 2,
    CellType_Water = 3,
    CellType_WaterDeep = 4,
    CellType_Count = 5
};

enum CellFlag
{
    CellFlag_Null = 0,
    CellFlag_Tree = (1 << CellType_Tree),
    CellFlag_Grass = (1 << CellType_Grass),
    CellFlag_Sand = (1 << CellType_Sand),
    CellFlag_Water = (1 << CellType_Water),
    CellFlag_WaterDeep = (1 << CellType_WaterDeep)
};

const int res = 128;

struct Cell
{
    //CellFlag Type;
    int AdjacencyMask;
    float3 Color;
};

struct WaveFunctionCollapseState
{
    Cell Cells[CellType_Count] = {
        { (CellFlag_Tree | CellFlag_Grass), float3(0.0, 0.5, 0.0) * 0.5f },
        { (CellFlag_Grass | CellFlag_Tree | CellFlag_Sand), float3(0.0, 1.0, 0.0) * 0.5f },
        { (CellFlag_Sand | CellFlag_Grass | CellFlag_Water), float3(1.0, 1.0, 0.0) * 0.5f },
        { (CellFlag_Water | CellFlag_Sand | CellFlag_WaterDeep), float3(0.0, 0.0, 1.0) * 0.75f },
        { (CellFlag_WaterDeep | CellFlag_Water), float3(0.0, 0.0, 1.0) * 0.5f }
    };

    int world[res][res];
    ArrayCreate(int2, pickArray, res * res);
    int step;
    int64 StartTime;
};

bool isInBounds(int x, int y)
{
    return (x >= 0) && (x < res) && (y >= 0) && (y < res);
}

bool OneBitIsSet(int64 value)
{
    return value != 0 && (value & (value - 1)) == 0;
}

#define changes 1
bool IsFinalizedCell(WaveFunctionCollapseState* game, int x, int y)
{
    if (!isInBounds(x, y))
        return false;

#if changes
    return OneBitIsSet(game->world[x][y]);
#else
    for (int i = 0; i < CellType_Count; i++)
    {
        if (game->world[x][y] == (1 << i))
            return true;
    }
#endif
    return false;
}
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))


Cell* CellFromPosition(WaveFunctionCollapseState* game, int x, int y)
{
    int cell = game->world[x][y];
    for (int i = 0; i < CellType_Count; i++)
    {
        if (cell == (1 << i))
            return &game->Cells[i];
    }
    return 0;
}
#define UINT32_MAX 0xFFFFFFFF
#define INT32_MAX 0x7FFFFFFF

int AdjacencyMaskFromCell(WaveFunctionCollapseState* game, int x, int y)
{
    int result = 0;
    for (int i = 0; i < CellType_Count; i++)
    {
        if(CHECK_BIT(game->world[x][y], i))
            result |= game->Cells[i].AdjacencyMask;
    }
    return result;
}

CellFlag IndexToFlag(int index)
{
    return (CellFlag)(1 << index);
}

bool HasOneFinalizedNeighbour(WaveFunctionCollapseState* game, int x, int y)
{
    int a = 0;
    if (isInBounds(x + 1, y) && IsFinalizedCell(game, x + 1, y))
        a++;
    if (isInBounds(x - 1, y) && IsFinalizedCell(game, x - 1, y))
        a++;
    if (isInBounds(x, y + 1) && IsFinalizedCell(game, x, y + 1))
        a++;
    if (isInBounds(x, y - 1) && IsFinalizedCell(game, x, y - 1))
        a++;
    return a == 1;
}

void wavefunctioncollapse(GameMemory* memory, EngineState* engineState, GameInput* Input, VideoBuffer* videoBuffer)
{
    WaveFunctionCollapseState* game = (WaveFunctionCollapseState*)(((uint8*)(engineState + 1)));
    if (Input->SpaceDown)
    {
        *game = {};

        // Init all the probabilities to 1
        for (int x = 0; x < res; x++)
        {
            for (int y = 0; y < res; y++)
            {
                game->world[x][y] = UINT32_MAX;
            }
        }

        game->StartTime = memory->platformTime();

        int StartX = GetNextRandom() % res;
        int StartY = GetNextRandom() % res;
        int StartCell = GetNextRandom() % CellType_Count;

        game->world[StartX][StartY] = IndexToFlag(StartCell);
        ArrayAdd(game->pickArray, int2(StartX + 1, StartY));
        ArrayAdd(game->pickArray, int2(StartX, StartY + 1));
        ArrayAdd(game->pickArray, int2(StartX - 1, StartY));
        ArrayAdd(game->pickArray, int2(StartX, StartY - 1));
    }
    


    int StepsPerFrame = 256;
    bool Generating = false;
    bool LastGenerating = false;
    for (int q = 0; q < StepsPerFrame; q++)
    {
        // reduce possibilities and keep track of one of the cells with "most reduced" probability
        int LowestX = -1;
        int LowestY = -1;
        int LowestI = -1;
        int LowestCount = 999;
        Generating = false;
        game->step++;

#if changes
        //for (int j = 0; j < 16; j++)
        {

            // Propegate probabilities to wavefront cells and find a wavefront cell with the lowest probability
            for (int i = 0; i < ArrayCount(game->pickArray); i++)
            {
                //int i = GetNextRandom() % ArrayCount(game->pickArray);
                int x = game->pickArray[i].x;
                int y = game->pickArray[i].y;
        
                if (!isInBounds(x, y))
                    continue;
        
                if (isInBounds(x + 1, y))
                    game->world[x][y] &= AdjacencyMaskFromCell(game, x + 1, y);
                if (isInBounds(x - 1, y))
                    game->world[x][y] &= AdjacencyMaskFromCell(game, x - 1, y);
                if (isInBounds(x, y + 1))
                    game->world[x][y] &= AdjacencyMaskFromCell(game, x, y + 1);
                if (isInBounds(x, y - 1))
                    game->world[x][y] &= AdjacencyMaskFromCell(game, x, y - 1);

                int count = popcount(game->world[x][y]);
        
                if (LowestCount > count && count != 1 && count != 0)
                {
                    LowestCount = count;
                    LowestX = x;
                    LowestY = y;
                    LowestI = i;
                    Generating = true;
                }
            }
        }
#else 
        for (int x = 1; x < res - 1; x++)
        {
            for (int y = 1; y < res - 1; y++)
            {
                if (IsFinalizedCell(game, x, y))
                    continue;
        
                // look at the 4 neighbouring cells and reduce our probability based on them.
                game->world[x][y] &= AdjacencyMaskFromCell(game, x + 1, y);
                game->world[x][y] &= AdjacencyMaskFromCell(game, x - 1, y);
                game->world[x][y] &= AdjacencyMaskFromCell(game, x, y + 1);
                game->world[x][y] &= AdjacencyMaskFromCell(game, x, y - 1);
        
                // Count number of set bits
#if changes
                int count = popcount(game->world[x][y]);
#else
                int count = 0;
                int test = game->world[x][y];
                for (int i = 0; i < CellType_Count; i++)
                {
                    if (test & 1)
                        count++;
                    test >>= 1;
                }
#endif

                if (LowestCount > count && count != 1 && count != 0)
                {
                    LowestCount = count;
                    LowestX = x;
                    LowestY = y;
                    Generating = true;
                }
            }
        }
#endif

        if (LastGenerating != Generating)
        {
            LastGenerating = Generating;
            if (!Generating)
            {
                int64 delta = memory->platformTime() - game->StartTime;
                char str[100] = {};
                ToString(str, (int)(delta / 10000.0f));
                Append(str, " ms");
                memory->platformPrint(str);

                //ToString(str, game->step);
                //Append(str, " steps");
                //Memory->PlatformPrint(str);
            }
        }

        // sweep through the bits of the cell one by one and pick a random 1 bit to keep, set the rest to 0.
        if (LowestX != -1)
        {
            int test = game->world[LowestX][LowestY];
            int RandomNew = GetNextRandom() % popcount(test);
            int count = 0;
            for (int i = 0; i < CellType_Count; i++)
            {
                if (test & 1)
                {
                    if (count == RandomNew)
                    {
                        game->world[LowestX][LowestY] = IndexToFlag(i);

#if changes
                        ArrayRemove(game->pickArray, LowestI);

                        if (HasOneFinalizedNeighbour(game, LowestX + 1, LowestY))
                            ArrayAdd(game->pickArray, int2(LowestX + 1, LowestY));
                        if (HasOneFinalizedNeighbour(game, LowestX - 1, LowestY))
                            ArrayAdd(game->pickArray, int2(LowestX - 1, LowestY));
                        if (HasOneFinalizedNeighbour(game, LowestX, LowestY + 1))
                            ArrayAdd(game->pickArray, int2(LowestX, LowestY + 1));
                        if (HasOneFinalizedNeighbour(game, LowestX, LowestY - 1))
                            ArrayAdd(game->pickArray, int2(LowestX, LowestY - 1));
#endif
                    }
                    count++;
                }
                test >>= 1;
            }
        }
    }

    DrawRectangle(videoBuffer, float2(0, 0), float2(1024 + res, 1024 + res), float3(0.5, 0, 0));
    // The original wavefunction collapse thing maybe draws these as like, "aveaged" values
    int size = 512 / res;
    // Draw world
    for (int x = 0; x < res; x++)
    {
        for (int y = 0; y < res; y++)
        {
            float2 CellPos = float2(x * size, y * size);

            float3 Color = float3(0, 0, 0);
            if (game->world[x][y])
            {
                Cell* cell = CellFromPosition(game, x, y);
                if (cell)
                    Color = cell->Color;
            }

            DrawRectangle(videoBuffer, CellPos, float2(size, size), Color);

            //for (int i = 0; i < CellType_Count; i++)
            //{
            //    if (game->World[x][y] >> i & 1)
            //        DrawRectangle(videoBuffer, CellPos + float2(0, i*3), float2(size, 1), float3(1, 1, 1));
            //}
        }
    }
    // Draw wavefront
    for (int i = 0; i < ArrayCount(game->pickArray); i++)
    {
        int x = game->pickArray[i].x;
        int y = game->pickArray[i].y;
        float2 CellPos = float2(x * size, y * size);
        DrawRectangle(videoBuffer, CellPos, float2(size, size), float3(1, 0, 0));
    }
}
