#pragma once
#include <string>
#include <cassert>
#include <glm/glm.hpp>
#include "int.hpp"
#include "heap_array.hpp"

namespace core {
    struct Voxel {
        u16 type;
        i8 density;
    };

    enum {
        ChunkSize = 32, ChunkSizeLog2 = 5,
        ChunkVoxelCount = ChunkSize * ChunkSize * ChunkSize,
        ChunkByteSize = ChunkVoxelCount * sizeof(Voxel),

        RegionSize = 4, RegionSizeLog2 = 4,
        RegionChunkCount = RegionSize * RegionSize * RegionSize,

        WorldWidthRegions = 1, WorldHeightRegions = 1,
        WorldWidth = WorldWidthRegions * RegionSize,   // Measured
        WorldHeight = WorldHeightRegions * RegionSize, // in chunks

        RenderDistanceChunks = 8,
        RenderDistance = ChunkSize * RenderDistanceChunks
    };

    // TODO: Is this really necessary?
    using VoxelArray = HeapArray<Voxel, ChunkVoxelCount>;

    struct VoxelVertex {
        glm::vec3 pos;
    };
    using VoxelIndex = u16;

    // TODO: proper error handling
    struct File {
    public:
        File() {}
        File(const char* filename, const char* options = "rb+");
        ~File();
        void Open(const char* filename, const char* options = "rb+");
        void Close();
        void MoveAt(std::size_t offset, u32 origin = SEEK_SET);
        std::size_t Tell();
        void Read(void* data, std::size_t byte_size);
        void Write(const void* data, std::size_t byte_size);
        void ReadAt(std::size_t pos, void* data, std::size_t byte_size);
        void WriteAt(std::size_t pos, const void* data, std::size_t byte_size);
        void ReadAll(std::string* str);
        FILE* GetHandle() { return _handle; }
    private:
        FILE* _handle;
    };
}

