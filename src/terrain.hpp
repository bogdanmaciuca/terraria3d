#pragma once
#include <cstddef>
#include <cstdlib>
#include <string>
#include <fstream>
#include <glm/glm.hpp>
#include "int.hpp"

template<typename TElem, std::size_t TSize>
struct HeapArray {
public:
    HeapArray() { _ptr = static_cast<TElem*>(malloc(sizeof(TElem) * TSize)); }
    ~HeapArray() { free(_ptr); }
    TElem& operator[](std::size_t idx) { return _ptr[idx]; }
    const TElem& operator[](std::size_t idx) const { return _ptr[idx]; }
    std::size_t Size() { return TSize; }
    std::size_t Size() const { return TSize; }
    TElem* Get() { return static_cast<TElem*>(_ptr); }
    const TElem* Get() const { return static_cast<TElem*>(_ptr); }
private:
    TElem* _ptr;
};

struct Voxel {
    u8 type;
    i8 density;
    u8 normal_x, normal_y, normal_z;
};

enum {
    ChunkSize = 32, ChunkSizeLog2 = 5,
    ChunkVoxelCount = ChunkSize * ChunkSize * ChunkSize,
    ChunkByteSize = ChunkVoxelCount * sizeof(Voxel),
    RegionSize = 16, RegionSizeLog2 = 4,
    RegionChunkCount = RegionSize * RegionSize * RegionSize,
};

struct Chunk {
    u16 idx;
    HeapArray<Voxel, ChunkVoxelCount> voxels;
    void GenerateTerrain();
};

struct RegionFile {
public:
    void Create(u16 position);
    void Open(u16 position);
    void WriteChunk(const Chunk& chunk);
    void ReadChunk(Chunk* chunk);
private:
    u32 _x, _y, _z;
    u32 _offsets[RegionChunkCount];
    std::fstream _file;
    void GenerateChunkTerrain(u16 chunk_idx);
};

void CreateWorld(u16 size);

