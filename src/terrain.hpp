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
    TElem& operator[](std::size_t idx) { return static_cast<TElem*>(_ptr)[idx]; }
    const TElem& operator[](std::size_t idx) const { return static_cast<TElem*>(_ptr)[idx]; }
    std::size_t Size() { return TSize; }
    std::size_t Size() const { return TSize; }
    TElem* Get() { return static_cast<TElem*>(_ptr); }
    const TElem* Get() const { return static_cast<TElem*>(_ptr); }
private:
    TElem* _ptr;
};

struct Voxel {
    u8 type;
    u8 density;
    glm::vec3 normal;
};

enum {
    ChunkSize = 32,
    VoxelCount = ChunkSize * ChunkSize * ChunkSize,
    ChunkByteSize = VoxelCount * sizeof(Voxel), // Note! This only takes into account the voxels
};

struct Chunk {
    u32 idx;
    HeapArray<Voxel, VoxelCount> voxels;
};

struct TerrainFile {
public:
    // size may be ignored when opening an existing file
    bool Initialize(const std::string& filename, u16 size = 0);
    ~TerrainFile() { _file.close(); }
    void WriteChunk(const Chunk& chunk);
    void ReadChunk(Chunk* chunk);
private:
    struct Header {
        u16 size;
    } _header;
    std::fstream _file;
};

