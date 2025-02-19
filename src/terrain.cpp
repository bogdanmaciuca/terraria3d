#include "terrain.hpp"
#include <iostream>
#include <string>
#include <fstream>    // TODO: maybe use fread or mmap
#include <filesystem> // TODO: maybe move all file/resource handling in a separate header
#include "noise1234/noise1234.h" // TODO: Use my own noise function for u8's

namespace fs = std::filesystem;

std::string FilenameFromPos(u16 position) {
    u16 coord_mask = (1 << RegionSizeLog2) - 1;
    u16 x = (position >> (RegionSizeLog2 * 0)) & coord_mask;
    u16 y = (position >> (RegionSizeLog2 * 1)) & coord_mask;
    u16 z = (position >> (RegionSizeLog2 * 2)) & coord_mask;
    return std::to_string(x) + "-" + std::to_string(y) + "-" + std::to_string(z) + ".region";
}

u16 PosFromFilename(const std::string& filename) {
    u16 result = 0;
    std::string current;
    u8 idx = 0;
    for (u8 i = 0; i < filename.size(); i++) {
        if (filename[i] == '-') {
            result |= std::stoi(current) << (RegionSizeLog2 * idx);
            std::cout << std::stoi(current) << "\n";
            current = "";
            idx++;
        }
        else if (filename[i] == '.') {
            result |= std::stoi(current) << (RegionSizeLog2 * idx);
            std::cout << std::stoi(current) << "\n";
            break;
        }
        else
            current += filename[i];
    }
    return result;
}

void RegionFile::WriteChunk(const Chunk& chunk) {
    // ...
}

void RegionFile::GenerateChunkTerrain(u16 chunk_idx) {
    static Chunk chunk; // TODO: this is destructed at the end of the program, but it could be destructed much sooner
    u32 chunk_coord_mask = (1 << RegionSizeLog2) - 1;
    u32 chunk_x = (chunk_idx >> RegionSizeLog2 * 0) & chunk_coord_mask;
    u32 chunk_y = (chunk_idx >> RegionSizeLog2 * 1) & chunk_coord_mask;
    u32 chunk_z = (chunk_idx >> RegionSizeLog2 * 2) & chunk_coord_mask;

    u32 voxel_coord_mask = (1 << ChunkSizeLog2) - 1;
    for (u32 i = 0; i < ChunkVoxelCount; i++) {
        u32 x = _x + chunk_x + (i >> ChunkSizeLog2 * 0) & voxel_coord_mask;
        u32 y = _y + chunk_y + (i >> ChunkSizeLog2 * 1) & voxel_coord_mask;
        u32 z = _z + chunk_z + (i >> ChunkSizeLog2 * 2) & voxel_coord_mask;
        chunk.voxels[i].density = noise3(x / 10.0f, y / 10.0f, z / 10.0f);
    }
    WriteChunk(chunk);
}

void RegionFile::Create(u16 position) {
    const std::string filename = FilenameFromPos(position);
    assert(!fs::exists(filename));
    _file.open(filename, std::ios::trunc | std::ios::binary);
    for (i32 i = 0; i < RegionChunkCount; i++) {
        GenerateChunkTerrain(i);
    }
    _file.close();
    _file.open(filename, std::ios::in | std::ios::out);
}

// Debugging only!!!
void Chunk::GenerateTerrain() {
    u32 voxel_coord_mask = (1 << ChunkSizeLog2) - 1;
    for (u32 i = 0; i < ChunkVoxelCount; i++) {
        u32 x = (i >> ChunkSizeLog2 * 0) & voxel_coord_mask;
        u32 y = (i >> ChunkSizeLog2 * 1) & voxel_coord_mask;
        u32 z = (i >> ChunkSizeLog2 * 2) & voxel_coord_mask;
        float val = noise3(x / 10.0f, y / 10.0f, z / 10.0f);
        voxels[i].density = val * 127.0f;
    }
}
