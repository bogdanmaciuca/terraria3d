#include "glw.hpp"
#include "surface_nets.hpp"
#include "int.hpp"
#include "terrain.hpp"

void SurfaceNets::CreateMesh(const Chunk& chunk) {
    Timer timer("Surface nets");

    auto Pos = [](i32 x, i32 y, i32 z) {
        return z * (i32)ChunkSize * (i32)ChunkSize + y * (i32)ChunkSize + x; 
    };
    auto Val = [&chunk, Pos](i32 x, i32 y, i32 z) {
        return chunk.voxels[Pos(x, y, z)].density;
    };

    for (i32 z = 0; z < ChunkSize - 1; z++) {
        for (i32 y = 0; y < ChunkSize - 1; y++) {
            for (i32 x = 0; x < ChunkSize - 1; x++) {
                u8 corner_signs = 0;
                i8 corner_values[8];
                for (i32 i = 0; i < 8; i++) {
                    i8 weight = chunk.voxels[Pos(x + corners[i][0], y + cornersInt3[i][1], z + cornersInt3[i][2])].density;
                    corner_values[i] = weight;
                    corner_signs |= (1 << i) * ((weight & (1 << 7)) == 0);
                }
                const float edge_num = coefficients_sizes[corner_signs];
                if (edge_num > 0) {
                    Vertex vert = { .pos = glm::vec3(0), .normal = glm::vec3(0) };
                    const auto coef = coefficients[corner_signs];
                    for (i32 i = 0; i < edge_num; i++) {
                        i8 v0 = corner_values[coef[i][0]];
                        i8 v1 = corner_values[coef[i][1]];
                        vert.pos += corners[coef[i][0]] + (float)(surface - v0) / (v1 - v0) * (corners[coef[i][1]] - corners[coef[i][0]]);
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

    // TODO: if necessary, try HeapArrayCountable for _indices
    for (u32 i = 0; i < _active_cells.size(); i++) {
        ActiveCell e = _active_cells[i];
        if (e.x == 0 || e.y == 0 || e.z == 0 ||
                e.x >= ChunkSize - 2 || e.y >= ChunkSize - 2 || e.z >= ChunkSize - 2)
            continue;

        // Oz
        if ((Val(e.x, e.y, e.z) >= surface) != (Val(e.x, e.y, e.z+1) >= surface)) {
            _indices.push_back(Face(
                        _cell_indices[Pos(e.x - 1, e.y - 1, e.z)],
                        _cell_indices[Pos(e.x - 0, e.y - 1, e.z)],
                        _cell_indices[Pos(e.x - 0, e.y - 0, e.z)],
                        _cell_indices[Pos(e.x - 1, e.y - 0, e.z)]
                        ));
        }
        // Ox
        if ((Val(e.x, e.y, e.z) >= surface) != (Val(e.x+1, e.y, e.z) >= surface)) {
            _indices.push_back(Face(
                        _cell_indices[Pos(e.x, e.y - 1, e.z - 1)],
                        _cell_indices[Pos(e.x, e.y - 0, e.z - 1)],
                        _cell_indices[Pos(e.x, e.y - 0, e.z - 0)],
                        _cell_indices[Pos(e.x, e.y - 1, e.z - 0)]
                        ));
        }
        // Oy
        if ((Val(e.x, e.y, e.z) >= surface) != (Val(e.x, e.y+1, e.z) >= surface)) {
            _indices.push_back(Face(
                        _cell_indices[Pos(e.x - 1, e.y, e.z - 1)],
                        _cell_indices[Pos(e.x - 0, e.y, e.z - 1)],
                        _cell_indices[Pos(e.x - 0, e.y, e.z - 0)],
                        _cell_indices[Pos(e.x - 1, e.y, e.z - 0)]
                        ));
        }
    }
}

