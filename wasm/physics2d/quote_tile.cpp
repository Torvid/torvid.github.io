
struct TileMapPosition
{
    // Map relative X and Y
    uint32 AbsTileX;
    uint32 AbsTileY;
    uint32 AbsTileZ;

    // Tile-relative X and Y
    float TileRelX;
    float TileRelY;

};

struct TileChunkPosition
{
    uint32 TileChunkX;
    uint32 TileChunkY;
    uint32 TileChunkZ;

    uint32 RelTileX;
    uint32 RelTileY;
};

struct TileChunk
{
    uint32 *Tiles;
};

struct TileMap
{
    int ChunkShiftValue;
    int ChunkMask;

    float TileSizeInMeters;


    int TileChunkCountX;
    int TileChunkCountY;
    int TileChunkCountZ;

    int ChunkDim;
    TileChunk* tileChunks;
};

inline void CanonicalizeCoord(TileMap* world, uint32* TileX, float* X)
{
    int32 Offset = RoundToInt(*X / world->TileSizeInMeters); // underflow/overflow X is in TileX;

    *TileX += Offset;
    *X -= Offset * world->TileSizeInMeters;

    Assert(*X >= -0.5f * world->TileSizeInMeters);
    Assert(*X <= 0.5f * world->TileSizeInMeters);
}

// always gives is the "real" part of the coordinate
TileMapPosition RecanonacalizePosition(TileMap* world, TileMapPosition Pos)
{
    TileMapPosition Result = Pos;
    TileMapPosition Result2 = Pos;

    CanonicalizeCoord(world, &Result.AbsTileX, &Result.TileRelX);
    CanonicalizeCoord(world, &Result.AbsTileY, &Result.TileRelY);

    return Result;
}

internal uint32 GetTileValueUnchecked(TileMap* world, TileChunk* tilemap, uint32 x, uint32 y)
{
    Assert(tilemap);
    Assert(x < world->ChunkDim);
    Assert(y < world->ChunkDim);

    return tilemap->Tiles[x + y * world->ChunkDim];
}

internal void SetTileValueUnchecked(TileMap* world, TileChunk* tilemap, uint32 x, uint32 y, uint32 TileValue)
{
    Assert(tilemap);
    Assert(x < world->ChunkDim);
    Assert(y < world->ChunkDim);

    tilemap->Tiles[x + y * world->ChunkDim] = TileValue;
}

internal TileChunk* GetTileChunk(TileMap* world, int32 x, int32 y, int32 z)
{
    TileChunk* tilemap = 0;
    if (x >= 0 && x < world->TileChunkCountX &&
        y >= 0 && y < world->TileChunkCountY &&
        z >= 0 && z < world->TileChunkCountZ)
    {
        tilemap = &world->tileChunks[x + 
                                     y * world->TileChunkCountX +
                                     z * world->TileChunkCountX * world->TileChunkCountY];
    }

    return tilemap;
}

internal uint32 GetTileValue(TileMap* world, TileChunk* tilemap, uint32 x, uint32 y)
{
    uint32 TileValue = false;
    if (tilemap && tilemap->Tiles)
    {
        TileValue = GetTileValueUnchecked(world, tilemap, x, y);
    }
    return TileValue;
}

internal void SetTileValue(TileMap* world, TileChunk* tilemap, uint32 x, uint32 y, uint32 TileValue)
{
    if (tilemap && tilemap->Tiles)
    {
        SetTileValueUnchecked(world, tilemap, x, y, TileValue);
    }
}

TileChunkPosition GetChunkPositionFor(TileMap *world, uint32 AbsTileX, uint32 AbsTileY, uint32 AbsTileZ)
{
    TileChunkPosition Result;

    Result.TileChunkX = AbsTileX >> world->ChunkShiftValue;
    Result.TileChunkY = AbsTileY >> world->ChunkShiftValue;
    Result.TileChunkZ = AbsTileZ;

    Result.RelTileX = AbsTileX & world->ChunkMask;
    Result.RelTileY = AbsTileY & world->ChunkMask;

    return Result;
}

internal uint32 GetTileValue(TileMap *world, uint32 AbsTileX, uint32 AbsTileY, uint32 AbsTileZ)
{
    TileChunkPosition ChunkPos = GetChunkPositionFor(world, AbsTileX, AbsTileY, AbsTileZ);
    TileChunk* tilemap = GetTileChunk(world, ChunkPos.TileChunkX, ChunkPos.TileChunkY, ChunkPos.TileChunkZ); // chunk
    uint32 TileChunkValue = GetTileValue(world, tilemap, ChunkPos.RelTileX, ChunkPos.RelTileY);

    return TileChunkValue;
}

internal bool32 IsWorldPointEmpty(TileMap *world, TileMapPosition CanPos)
{
    return GetTileValue(world, CanPos.AbsTileX, CanPos.AbsTileY, CanPos.AbsTileZ) == 1;

}

void SetTileValue(MemoryArena* arena, TileMap* world, uint32 x, uint32 y, uint32 z, uint32 value)
{
    TileChunkPosition ChunkPos = GetChunkPositionFor(world, x, y, z);
    TileChunk* tileChunk = GetTileChunk(world, ChunkPos.TileChunkX, ChunkPos.TileChunkY, ChunkPos.TileChunkZ); // chunk

    Assert(tileChunk);
    
    if (!tileChunk->Tiles)
    {
        uint32 TileCount = world->ChunkDim*world->ChunkDim;
        tileChunk->Tiles = PushArray(arena, TileCount, uint32);
        for (int i = 0; i < TileCount; i++)
        {
            tileChunk->Tiles[i] = -1;
        }
    }

    if (tileChunk)
    {
        SetTileValue(world, tileChunk, ChunkPos.RelTileX, ChunkPos.RelTileY, value);
    }
}