// Problem for future me


#pragma pack(push, 1)
struct png_chunk_header
{
    uint32 Size;
    union
    {
        char CookieString[4];
        uint32 Cookie;
    };
};

struct png_chunk_footer
{
    uint32 Crc; // redundancy stuff, don't care
};
struct png_chunk_IHDR
{
    png_chunk_header Header;
    uint32 SizeX;
    uint32 SizeY;
    uint8 BitDepth;
    uint8 ColorType;
    uint8 CompressionMethod;
    uint8 FilterMethod;
    uint8 InterlaceMethod;
    png_chunk_footer Footer;
};

struct png_header
{
    uint8 Signature[8]; // stuff we don't care about
    png_chunk_IHDR FirstChunk; // first chunk is always IHDR
};

struct png_chunk_IDAT
{
    png_chunk_header Header;
    uint8 Data;
};
//uint8 CompressionFlags;
//uint8 AdditionalFlags;
struct png_huffman_tree
{

};
void GenerateHuffmanTree(uint32 InputCount, uint32* Input, png_huffman_tree* Result)
{

}
uint32 ReadHuffmanValue(png_huffman_tree* Result, uint8** Current, uint8* BitCounter)
{
    return 0;
}

#pragma pack(pop)

uint32 EndianSwap(uint32 num)
{
    //return ((num << 24) & 0xff000000) | // move byte 0 to byte 3
    //       ((num << 8)  & 0x00ff0000) | // move byte 1 to byte 2
    //       ((num >> 8)  & 0x0000ff00) | // move byte 2 to byte 1
    //       ((num >> 24) & 0x000000ff);  // move byte 3 to byte 0

    return _byteswap_ulong(num);

    //return (num << 24) | ((num & 0xff00) << 8) | ((num >> 8) & 0xff00) | (num >> 24);
}
uint32 FourCC(const char* String)
{
    return *(uint32*)String;
}
void ReadToNextByte(uint8** Current, uint8* BitCounter)
{
    if (*BitCounter > 0)
        (*Current)++;

    *BitCounter = 0;
}
uint32 ReadBits(int BitCount, uint8** Current, uint8* BitCounter)
{
    uint8 Shifted = 0;
    if (BitCount <= 16)
    {
        uint32 FullSequence = *(uint32*)(*Current);
        FullSequence >>= *BitCounter;

        Shifted = (uint8)FullSequence;
        Shifted &= (1 << BitCount) - 1;

        FullSequence >>= BitCount;

        //uint8 Mask = (2 << BitCount) - 1;
    }

    *BitCounter += BitCount;
    *Current += (*BitCounter / 8);
    *BitCounter %= 8;
    return Shifted;
}

void PlatformReadImage(image* Image, const char* Path)
{
    void* File = 0;
    FileRead(Path, &File);
    wchar_t directory[500];

    GetCurrentDirectory(500, directory);

    if (!File)
        return;

    uint8* IDATChunks[100] = {};
    int ChunkSizes[100] = {};
    int ChunkCount = 0;
    int CompressedDataSize = 0;

    png_header* Header = (png_header*)File;
    // endianness fix
    Header->FirstChunk.SizeX = EndianSwap(Header->FirstChunk.SizeX);
    Header->FirstChunk.SizeY = EndianSwap(Header->FirstChunk.SizeY);
    uint8* Offset = (uint8*)&(Header->FirstChunk.Header);
    while (true)
    {
        png_chunk_header* CurrentChunk = (png_chunk_header*)Offset;

        CurrentChunk->Size = EndianSwap(CurrentChunk->Size);
        CurrentChunk->Cookie = EndianSwap(CurrentChunk->Cookie);
        printf("CHUNK: %c%c%c%c\n",
            CurrentChunk->CookieString[3],
            CurrentChunk->CookieString[2],
            CurrentChunk->CookieString[1],
            CurrentChunk->CookieString[0]);

        if (CurrentChunk->Cookie == 'IEND')
            break;

        if (CurrentChunk->Cookie == 'IHDR')
        {
            png_chunk_IHDR* Chunk = (png_chunk_IHDR*)CurrentChunk;
            Assert(Chunk->BitDepth == 8);
            Assert(Chunk->ColorType == 6);
            Assert(Chunk->CompressionMethod == 0); // png only uses DEFLATE
            Assert(Chunk->FilterMethod == 0);
            Assert(Chunk->InterlaceMethod == 0);
        }
        if (CurrentChunk->Cookie == 'IDAT')
        {
            printf("    Length: %d\n", CurrentChunk->Size);
            png_chunk_IDAT* Chunk = (png_chunk_IDAT*)CurrentChunk;
            uint8* Current = &Chunk->Data;

            Assert(ChunkCount < 99); // too many IDAT chunks!
            CompressedDataSize += CurrentChunk->Size;
            IDATChunks[ChunkCount] = Current;
            ChunkSizes[ChunkCount] = CurrentChunk->Size;
            ChunkCount++;
        }
        else if (CurrentChunk->Cookie == 'iCCP')
        {
        }
        else if (CurrentChunk->Cookie == 'pHYs')
        {
        }
        Offset += sizeof(png_chunk_header) + CurrentChunk->Size + sizeof(png_chunk_footer);
    }

    // Copy the compressed data into contiguous memory, easier that way.
    uint8* Data = (uint8*)VirtualAlloc(0, CompressedDataSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    uint8* Current = Data;
    for (int i = 0; i < ChunkCount; i++)
    {
        Copy(IDATChunks[i], Current, ChunkSizes[i]);
        Current += ChunkSizes[i];
    }

    Current = Data;
    uint8 BitCounter = 0;

    // CMF
    uint8 CM = ReadBits(4, &Current, &BitCounter);
    uint8 CINFO = ReadBits(4, &Current, &BitCounter);
    // FLG 
    uint8 FCHECK = ReadBits(5, &Current, &BitCounter);
    uint8 FDICT = ReadBits(1, &Current, &BitCounter);
    uint8 FLEVEL = ReadBits(2, &Current, &BitCounter);

    printf("   CMF: \n");
    printf("    CM: %d\n", CM);
    printf("    CINFO: %d\n", CINFO);
    printf("   FLG: \n");
    printf("    FCHECK: %d\n", FCHECK);
    printf("    FDICT: %d\n", FDICT);
    printf("    FLEVEL: %d\n", FLEVEL);

    Assert(CM == 8)
        Assert(FDICT == 0)
        Assert(FLEVEL == 1)

        // read blocks out of the DEFLATE stream!
        uint8 BFINAL = 0;
    while (BFINAL == 0)
    {
        BFINAL = ReadBits(1, &Current, &BitCounter);
        uint8 BTYPE = ReadBits(2, &Current, &BitCounter);

        // only BTYPE 2 and 0 is allowed
        Assert(BTYPE == 0 || BTYPE == 2);

        if (BTYPE == 0)
        {
            ReadToNextByte(&Current, &BitCounter); // move to the next whole byte
            uint16 LEN = ReadBits(16, &Current, &BitCounter);
            uint16 NLEN = ReadBits(16, &Current, &BitCounter);
            Assert(LEN == ~NLEN);
            Assert(BitCounter == 0); // should be 0 here, since we ReadToNextByte and then read four bytes exactly.
            uint8* RawData = Current;
        }
        else if (BTYPE == 2)
        {
            png_huffman_tree LiteralHuffmanTree = {};
            png_huffman_tree DistanceHuffmanTree = {};
            // TODO: Read huffman tree here

            uint32 HLIT = ReadBits(5, &Current, &BitCounter);
            uint32 HDIST = ReadBits(5, &Current, &BitCounter);
            uint32 HCLEN = ReadBits(4, &Current, &BitCounter);
            HLIT += 257;
            HDIST += 1;
            HCLEN += 4;

            uint8 Swizzle[] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
            uint32 CodeLengths[ArrayCapacity(Swizzle)] = {};

            for (int i = 0; i < HCLEN; i++)
            {
                CodeLengths[Swizzle[i]] = ReadBits(3, &Current, &BitCounter);
            }

            png_huffman_tree HuffmanTree = {};
            GenerateHuffmanTree(ArrayCapacity(Swizzle), CodeLengths, &HuffmanTree);

            // Decode stuff from the huffman tree untill all the HLIT and HDIST's are filled up!
            uint32 LitLenDistTable[512];
            uint32 LitLenCount = 0;
            while (LitLenCount < (HLIT + HDIST))
            {
                uint32 RepeatCount = 1;
                uint32 RepeatValue = 0;
                uint32 EncodedLen = ReadHuffmanValue(&HuffmanTree, &Current, &BitCounter);
                if (EncodedLen <= 15) // copy this value into the stream
                {
                    RepeatCount = 1;
                    RepeatValue = EncodedLen;
                    LitLenCount++;
                }
                else if (EncodedLen == 16) // copy the previous value this many times
                {
                    uint32 RepeatCount = 3 + ReadBits(2, &Current, &BitCounter);
                    Assert(LitLenCount > 0)
                        uint32 RepeatValue = LitLenDistTable[LitLenCount - 1];
                }
                else if (EncodedLen == 17) // repeat '0' 3-10 times
                {
                    uint32 RepeatCount = 3 + ReadBits(2, &Current, &BitCounter);
                }
                else if (EncodedLen == 18) // repeat '0' 11-138 times
                {
                    uint32 RepeatCount = 11 + ReadBits(7, &Current, &BitCounter);
                }

                for (int i = 0; i < RepeatCount; i++)
                {
                    LitLenDistTable[LitLenCount] = RepeatValue;
                    LitLenCount++;
                }
            }

            Assert(LitLenCount == (HLIT + HDIST))
                GenerateHuffmanTree(HLIT, LitLenDistTable, &LiteralHuffmanTree);
            GenerateHuffmanTree(HDIST, LitLenDistTable + HLIT, &DistanceHuffmanTree);


            int qwewq = 0;

            //uint32 LitLen = HuffmanDecode();
            //
            //if (LitLen < 256)
            //{
            //    uint8 Output = LitLen;
            //}
            //else
            //{
            //    if (LitLen == 256)
            //    {
            //        break; // end of block
            //    }
            //    else
            //    {
            //        uint32 Length = LitLen - 256;
            //        uint32 Distance = HuffmanDecode();
            //
            //    }
            //}

        }
    }
    FileFree(File);
}
