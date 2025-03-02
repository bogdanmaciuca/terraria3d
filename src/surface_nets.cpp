#include "surface_nets.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/normal.hpp>
#include "glw.hpp"
#include "int.hpp"
#include "heap_array.hpp"
#include "core.hpp"
#include "debug.hpp"

void SurfaceNets::PushFace(core::VoxelIndex i0, core::VoxelIndex i1, core::VoxelIndex i2, core::VoxelIndex i3, bool reversed) {
    if (reversed) {
        _indices.push_back(i3);
        _indices.push_back(i2);
        _indices.push_back(i0);
        _indices.push_back(i2);
        _indices.push_back(i1);
        _indices.push_back(i0);
    }
    else {
        _indices.push_back(i0);
        _indices.push_back(i1);
        _indices.push_back(i2);
        _indices.push_back(i0);
        _indices.push_back(i2);
        _indices.push_back(i3);
    }
}

void SurfaceNets::CreateMesh(const HeapArray<core::Voxel, core::ChunkVoxelCount>& voxels) {
    //Timer timer("Surface nets");
    _vertices.clear();
    _indices.clear();
    _active_cells.clear();

    auto Pos = [](i32 x, i32 y, i32 z) {
        return z * (i32)core::ChunkSize * (i32)core::ChunkSize + y * (i32)core::ChunkSize + x; 
    };
    auto Val = [&voxels, Pos](i32 x, i32 y, i32 z) {
        return voxels[Pos(x, y, z)].density;
    };

    for (i32 z = 0; z < core::ChunkSize - 1; z++) {
        for (i32 y = 0; y < core::ChunkSize - 1; y++) {
            for (i32 x = 0; x < core::ChunkSize - 1; x++) {
                u8 corner_signs = 0;
                i8 corner_values[8];
                for (i32 i = 0; i < 8; i++) {
                    i8 weight = voxels[Pos(x + corners[i][0], y + cornersInt3[i][1], z + cornersInt3[i][2])].density;
                    corner_values[i] = weight;
                    corner_signs |= (1 << i) * ((weight & (1 << 7)) == 0);
                }
                const float edge_num = coefficients_sizes[corner_signs];
                if (edge_num > 0) {
                    core::VoxelVertex vert = { .pos = glm::vec3(0) };

                    const auto edges = edges_list[corner_signs];
                    for (i32 i = 0; i < edge_num; i++) {
                        i8 v0 = corner_values[edges[i][0]];
                        i8 v1 = corner_values[edges[i][1]];
                        float lerp_t = static_cast<float>((surface - v0)) / (v1 - v0);
                        vert.pos += corners[edges[i][0]] + lerp_t * (corners[edges[i][1]] - corners[edges[i][0]]);
                    }
                    vert.pos /= edge_num;
                    vert.pos += glm::vec3(x, y, z);

                    _cell_indices[Pos(x, y, z)] = _vertices.size();
                    _vertices.push_back(vert);
                    if (edge_num > 1) {
                        _active_cells.push_back({
                            .x = static_cast<u8>(x),
                            .y = static_cast<u8>(y),
                            .z = static_cast<u8>(z)
                        });
                    }
                }
            }
        }
    }

    for (u32 i = 0; i < _active_cells.size(); i++) {
        ActiveCell e = _active_cells[i];
        if (e.x == 0 || e.y == 0 || e.z == 0 ||
                e.x >= core::ChunkSize - 1 || e.y >= core::ChunkSize - 1 || e.z >= core::ChunkSize - 1)
            continue;

        bool solid0, solid1;
        // Oz
        solid0 = Val(e.x, e.y, e.z) >= surface;
        solid1 = Val(e.x, e.y, e.z+1) >= surface;
        if (solid0 != solid1) {
            core::VoxelIndex i0 = _cell_indices[Pos(e.x - 1, e.y - 1, e.z)];
            core::VoxelIndex i1 = _cell_indices[Pos(e.x - 0, e.y - 1, e.z)];
            core::VoxelIndex i2 = _cell_indices[Pos(e.x - 0, e.y - 0, e.z)];
            core::VoxelIndex i3 = _cell_indices[Pos(e.x - 1, e.y - 0, e.z)];
            PushFace(i0, i1, i2, i3, solid0);
        }
        // Oy
        solid0 = Val(e.x, e.y, e.z) >= surface;
        solid1 = Val(e.x, e.y+1, e.z) >= surface;
        if (solid0 != solid1) {
            core::VoxelIndex i0 = _cell_indices[Pos(e.x - 1, e.y, e.z - 1)];
            core::VoxelIndex i1 = _cell_indices[Pos(e.x - 0, e.y, e.z - 1)];
            core::VoxelIndex i2 = _cell_indices[Pos(e.x - 0, e.y, e.z - 0)];
            core::VoxelIndex i3 = _cell_indices[Pos(e.x - 1, e.y, e.z - 0)];
            PushFace(i0, i1, i2, i3, solid1);
        }
        // Ox
        solid0 = Val(e.x, e.y, e.z) >= surface;
        solid1 = Val(e.x+1, e.y, e.z) >= surface;
        if (solid0 != solid1) {
            core::VoxelIndex i0 = _cell_indices[Pos(e.x, e.y - 1, e.z - 1)];
            core::VoxelIndex i1 = _cell_indices[Pos(e.x, e.y - 0, e.z - 1)];
            core::VoxelIndex i2 = _cell_indices[Pos(e.x, e.y - 0, e.z - 0)];
            core::VoxelIndex i3 = _cell_indices[Pos(e.x, e.y - 1, e.z - 0)];
            PushFace(i0, i1, i2, i3, solid0);
        }
    }
}

