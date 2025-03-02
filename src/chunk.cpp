#include "chunk.hpp"
#include <memory.h>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "noise1234/noise1234.h"
#include "int.hpp"
#include "core.hpp"
#include "surface_nets.hpp"

enum {
    MaxRegionFilesOpenAtOnce = 8
};

Chunk::Chunk() {
    _VAO.Initialize(sizeof(core::VoxelVertex));
    _VBO.Initialize();
    _IBO.Initialize();

    _VAO.Bind();
    _VBO.Bind();
    _IBO.Bind();
    
    _VAO.AddAttrib(GL_FLOAT, 3); // Position
                                 // TODO: texture index
}

void Chunk::Mesh() {
    auto& surface_nets = SurfaceNets::GetInstance();
    surface_nets.CreateMesh(voxels);
    _VAO.Bind();
    _VBO.Source(surface_nets.GetVertices());
    _IBO.Source(surface_nets.GetIndices());
}

void Chunk::Render() {
    _VAO.Bind();
    glw::DrawIndexed(_IBO.Length(), GL_TRIANGLES, GL_UNSIGNED_SHORT);
}

RegionFile::RegionFile(const std::string& filename)
    : filename(filename), file(filename.c_str())
{
    // TODO: check if file is open
    file.Read(reinterpret_cast<char*>(&metadata[0]), sizeof(metadata));
}


void ChunkManager::Initialize(const std::string& world_path) {
    _world_path = world_path;
}

void ChunkManager::UpdateChunks(const glm::vec3& player_pos) {
    // Adding new chunks to the hashmap
    for (i32 x = -core::RenderDistanceChunks; x <= core::RenderDistanceChunks; x++)
        for (i32 y = -core::RenderDistanceChunks; y <= core::RenderDistanceChunks; y++)
            for (i32 z = -core::RenderDistanceChunks; z <= core::RenderDistanceChunks; z++) {
                if (x * x + y * y + z * z <= core::RenderDistance * core::RenderDistance) {
                    glm::ivec3 position = glm::ivec3(player_pos) + glm::ivec3(x, y, z);
                    if (position.x < 0 || position.y < 0 || position.z < 0 ||
                        position.x >= core::WorldWidth ||
                        position.y >= core::WorldHeight ||
                        position.z >= core::WorldWidth
                    ) continue;

                    if (!_map.contains(position))
                        LoadChunk(position);
                }
            }

    // Erasing chunks that are not needed anymore
    for (auto& it: _map) {
        glm::vec3 distance = glm::ivec3(player_pos) - it.first;
        if (glm::length2(distance) > (core::RenderDistance + 2) * (core::RenderDistance + 2)) {
            SaveChunk(it.first, it.second);
            _map.erase(it.first);
        }
    }
}

std::string GetRegionFilename(const glm::ivec3& chunk_pos) {
    const i32 region_x = chunk_pos.x >> core::RegionSizeLog2;
    const i32 region_y = chunk_pos.y >> core::RegionSizeLog2;
    const i32 region_z = chunk_pos.z >> core::RegionSizeLog2;
    return
        std::to_string(region_x) + "-" +
        std::to_string(region_y) + "-" +
        std::to_string(region_z) + ".region";
}
std::string ChunkManager::GetRegionPath(const glm::ivec3& chunk_pos) {
    return _world_path + "/" + GetRegionFilename(chunk_pos);
}

i32 GetIdxOfChunkInRegion(const glm::ivec3& chunk_pos) {
    return
        (chunk_pos.z & ((1 << core::RegionSizeLog2) - 1)) * core::RegionSize * core::RegionSize +
        (chunk_pos.y & ((1 << core::RegionSizeLog2) - 1)) * core::RegionSize +
        (chunk_pos.x & ((1 << core::RegionSizeLog2) - 1));
}

void ChunkManager::LoadChunk(const glm::ivec3& pos) {
    i32 idx_in_region = GetIdxOfChunkInRegion(pos);

    const std::string filename = GetRegionPath(pos);
    auto region_file = std::find_if(
        _region_files.begin(), _region_files.end(),
        [filename](const RegionFile& region_file) -> bool {
            return region_file.filename == filename;
        }
    );
    if (region_file == _region_files.end()) {
        _region_files.emplace_back(filename);
        if (_region_files.size() > MaxRegionFilesOpenAtOnce)
            _region_files.erase(_region_files.begin());
        region_file = std::prev(_region_files.end());
    }
    const auto it = _map.emplace(pos, Chunk());
    assert(it.second == true);

    const u32 offset = region_file->metadata[idx_in_region].offset;
    const u32 size = region_file->metadata[idx_in_region].size;
    region_file->file.ReadAt(
        offset, reinterpret_cast<char*>(it.first->second.voxels.Get()), size);
    //for(i32 i = 0; i < core::ChunkVoxelCount; i++) {
    //    if ((i32)(it.first->second.voxels).Get()[i].density != 0)
    //        std::cout << (i32)(it.first->second.voxels).Get()[i].density << " ";
    //}
    it.first->second.Mesh();
}

void ChunkManager::SaveChunk(const glm::ivec3& pos, const Chunk& chunk) {
    i32 idx_in_region = GetIdxOfChunkInRegion(pos);

    const std::string filename = GetRegionPath(pos);
    auto region_file = std::find_if(
        _region_files.begin(), _region_files.end(),
        [filename](const RegionFile& region_file) -> bool {
            return region_file.filename == filename;
        }
    );

    // Create new metadata
    RegionFile::ChunkMetadata new_metadata;
    region_file->file.MoveAt(0, SEEK_END);
    new_metadata.offset = region_file->file.Tell();
    new_metadata.size = core::ChunkByteSize;
    
    // Append new chunk data to the end of the file
    region_file->file.Write(
        reinterpret_cast<const char*>(chunk.voxels.Get()), new_metadata.size);

    // Overwrite metadata in memory
    region_file->metadata[idx_in_region] = new_metadata;

    // Overwrite metadata in file
    region_file->file.WriteAt(
        idx_in_region * sizeof(RegionFile::ChunkMetadata),
        reinterpret_cast<const char*>(&new_metadata), sizeof(new_metadata)
    );
}

void GenerateChunkTerrain(HeapArray<core::Voxel, core::ChunkVoxelCount>* voxels, i16 cx, i16 cy, i16 cz) {
    for (i32 x = 0; x < core::ChunkSize; x++) {
        for (i32 y = 0; y < core::ChunkSize; y++) {
            for (i32 z = 0; z < core::ChunkSize; z++) {
                float val = noise2(
                    (cx * (float)(core::ChunkSize - 2) + x) / 10.0f,
                    (cz * (float)(core::ChunkSize - 2) + z) / 10.0f
                );
                val = val * 10.0f + 10.0f;
                val = (cy * (float)(core::ChunkSize - 2) + y) - val;
                val = glm::clamp(val, -1.0f, 1.0f);
                const i32 idx =
                    z * core::ChunkSize * core::ChunkSize +
                    y * core::ChunkSize +
                    x;
                (*voxels)[idx].density = val * 127.0f;
            }
        }
    }
}

void GenerateRegionTerrain(const std::string& path, i16 rx, i16 ry, i16 rz) {
    core::File file;
    file.Open(
        (path + "/" + GetRegionFilename(glm::ivec3(rx, ry, rz))).c_str(),
        "wb+"
    );

    HeapArray<core::Voxel, core::ChunkVoxelCount> voxels;
    RegionFile::ChunkMetadata metadata[core::RegionChunkCount];
    file.Write(&metadata, sizeof(metadata));
    for (i32 cz = 0; cz < core::RegionSize; cz++) {
        for (i32 cy = 0; cy < core::RegionSize; cy++) {
            for (i32 cx = 0; cx < core::RegionSize; cx++) {
                const i32 idx =
                    cz * core::RegionSize * core::RegionSize +
                    cy * core::RegionSize +
                    cx;
                GenerateChunkTerrain(&voxels, cx, cy, cz);

                file.MoveAt(0, SEEK_END);
                metadata[idx].offset = file.Tell();
                metadata[idx].size = core::ChunkByteSize; // TODO: Update when introducing compression
                
                file.WriteAt(
                    idx * sizeof(RegionFile::ChunkMetadata),
                    &metadata[idx], sizeof(RegionFile::ChunkMetadata)
                );

                file.MoveAt(0, SEEK_END);
                file.Write(voxels.Get(), metadata[idx].size); // TODO: Update when introducing compression
            }
        }
    }
}

void CreateWorld(const std::string& path) {
    assert(
        std::filesystem::create_directory(path)
    );

    for (i32 rx = 0; rx < core::WorldWidthRegions; rx++)
        for (i32 ry = 0; ry < core::WorldHeightRegions; ry++)
            for (i32 rz = 0; rz < core::WorldWidthRegions; rz++)
                GenerateRegionTerrain(path, rx, ry, rz);
}

