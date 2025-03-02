#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include "int.hpp"
#include "core.hpp"
#include "glw.hpp"

/*
 * Region files: x-y-z.region
 *
 * Chunk seams:
 * - solved by OVERLAPPING the chunks by 2 voxels, thus making them 30 voxels wide visually
 */

// TODO: maybe implement voxel palette
struct Chunk {
public:
    HeapArray<core::Voxel, core::ChunkVoxelCount> voxels;
    Chunk();
    void Mesh();
    void Render();
private:
    glw::VertexArrayObject _VAO;
    glw::VertexBuffer<core::VoxelVertex, GL_DYNAMIC_DRAW> _VBO;
    glw::IndexBuffer<core::VoxelIndex, GL_DYNAMIC_DRAW> _IBO;
};

struct RegionFile {
    RegionFile(const std::string& filename);
    HeapArray<core::Voxel, core::ChunkVoxelCount> ReadChunkVoxels();
    std::string filename;
    core::File file;
    struct ChunkMetadata {
        u32 offset;
        u32 size;
    } metadata[core::RegionChunkCount];
};

struct ChunkManager {
public:
    void Initialize(const std::string& world_path);
    void UpdateChunks(const glm::vec3& player_pos);
    Chunk& Get(const glm::ivec3& pos);
    const Chunk& Get(const glm::ivec3& pos) const;
    void GenerateChunks();
    auto& GetChunks() { return _map; }
private:
    struct IVec3Hash {
        std::size_t operator()(const glm::vec3& v) const noexcept {
            constexpr std::size_t p1 = 23, p2 = 71, p3 = 109;
            return p1 * v.x + p2 * v.y + p3 * v.z;
        }
    };
    std::string _world_path;
    std::unordered_map<glm::ivec3, Chunk, IVec3Hash> _map;
    std::vector<RegionFile> _region_files;
    void LoadChunk(const glm::ivec3& pos);
    void SaveChunk(const glm::ivec3& pos, const Chunk& chunk);
    void GenerateRegionTerrain(const std::string& path, i16 rx, i16 ry, i16 rz);
    std::string GetRegionPath(const glm::ivec3& pos);
};

void CreateWorld(const std::string& path);
