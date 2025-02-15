#include "terrain.hpp"
#include <iostream>
#include <fstream>
#include <filesystem> // TODO: maybe move all file/resource handling in a separate header

namespace fs = std::filesystem;

bool TerrainFile::Initialize(const std::string& filename, u16 size) {
    bool result = true;
    if (!fs::exists(filename)) {
        _header.size = size;
        std::ofstream f(filename, std::ios::trunc);
        f.write(reinterpret_cast<const char*>(&_header.size), sizeof(_header.size));
        f.flush();
        result = false;
    }
    _file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    return result;
}

void TerrainFile::WriteChunk(const Chunk& chunk) {
    _file.seekp(sizeof(Header) + chunk.idx * ChunkByteSize);
    _file.write(reinterpret_cast<const char*>(chunk.voxels.Get()), ChunkByteSize);
}

void TerrainFile::ReadChunk(Chunk* chunk) {
    _file.seekg(sizeof(Header) + chunk->idx * ChunkByteSize);
    _file.read(reinterpret_cast<char*>(chunk->voxels.Get()), ChunkByteSize);
}

