#pragma once
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include "int.hpp"
#include "terrain.hpp"
#include "debug.hpp"

struct Int3 {
    i32 x = 0, y = 0, z = 0;
};

struct Edge {
    Int3 p0, p1;
};

constexpr Int3 corners[] = {
    { 0, 0, 0 }, // Bottom face, clockwise
    { 1, 0, 0 },
    { 1, 0, 1 },
    { 0, 0, 1 },
    { 0, 1, 0 }, // Top face, clockwise
    { 1, 1, 0 },
    { 1, 1, 1 },
    { 0, 1, 1 },
};

constexpr Edge edges[] = {
    { corners[0], corners[1] }, // Bottom face
    { corners[1], corners[2] },
    { corners[2], corners[3] },
    { corners[3], corners[0] },
    { corners[4], corners[5] }, // Top face
    { corners[5], corners[6] },
    { corners[6], corners[7] },
    { corners[7], corners[4] },
    { corners[0], corners[4] }, // Vertical edges
    { corners[1], corners[5] },
    { corners[2], corners[6] },
    { corners[3], corners[7] }
};

#define POS(x, y, z) ((z) * ChunkSize * ChunkSize + (y) * ChunkSize + (x))
#define VAL(x, y, z) (chunk.voxels[POS(x, y, z)].density)

constexpr u8 surface = 128;

template <typename TVertex, typename TIndex>
class SurfaceNets {
public:
    SurfaceNets() {}
    void CreateMesh(const Chunk& chunk) {
        Timer timer("Surface nets");
        _vertices.clear();
        _indices.clear();
        for (i32 i = 0; i < ChunkSize - 1; i++) {
            for (i32 j = 0; j < ChunkSize - 1; j++) {
                for (i32 k = 0; k < ChunkSize - 1; k++) {
                    glm::vec3 vert(0);
                    i32 edge_count = 0;
                    for (i32 edge_idx = 0; edge_idx < 12; edge_idx++) {
                        const Edge e = edges[edge_idx];
                        i32 x0 = i + e.p0.x, y0 = j + e.p0.y, z0 = k + e.p0.z;
                        i32 x1 = i + e.p1.x, y1 = j + e.p1.y, z1 = k + e.p1.z;
                        u8 v0 = chunk.voxels[POS(x0, y0, z0)].density;
                        u8 v1 = chunk.voxels[POS(x1, y1, z1)].density;
                        if ((v0 > surface) != (v1 > surface)) {
                            glm::vec3 v = glm::vec3(x0, y0, z0) + ((float)surface - v0) / ((float)v1 - v0) * (glm::vec3(x1, y1, z1) - glm::vec3(x0, y0, z0));
                            vert += v;
                            edge_count++;
                        }
                    }
                    if (edge_count >= 2) {
                        _active_cells.push_back({ .edge_type = 0, .pos = {i, j, k} });
                    }
                    if (edge_count >= 1) {
                        vert /= edge_count;
                        if (edge_count == 0) {
                            std::cout << vert.x << " " << vert.y << " " << vert.z << " " << edge_count << "\n";
                        }
                        _cell_vertices[POS(i, j, k)] = _vertices.size();
                        _vertices.push_back(vert);
                    }
                    
                    //glm::vec3 vert(0);
                    //i32 edge_count = 0;
                    //for (i32 dx = 0; dx < 2; dx++)
                    //    for (i32 dy = 0; dy < 2; dy++) {
                    //        u8 v0 = VAL(i+dx, j+dy, k+0), v1 = VAL(i+dx, j+dy, k+1);
                    //        if ((v0 >= surface) != (v1 >= surface)) {
                    //            vert += glm::vec3(i, j, k) + ((float)surface - v0) / ((float)v1 - v0) * glm::vec3(0, 0, 1);
                    //            edge_count++;
                    //        }
                    //    }
                    //for (i32 dx = 0; dx < 2; dx++)
                    //    for (i32 dz = 0; dz < 2; dz++) {
                    //        u8 v0 = VAL(i+dx, j+0, k+dz), v1 = VAL(i+dx, j+1, k+dz);
                    //        if ((v0 >= surface) != (v1 >= surface)) {
                    //            vert += glm::vec3(i, j, k) + ((float)surface - v0) / ((float)v1 - v0) * glm::vec3(0, 1, 0);
                    //            edge_count++;
                    //        }
                    //    }
                    //for (i32 dy = 0; dy < 2; dy++)
                    //    for (i32 dz = 0; dz < 2; dz++) {
                    //        u8 v0 = VAL(i+0, j+dy, k+dz), v1 = VAL(i+1, j+dy, k+dz);
                    //        if ((v0 >= surface) != (v1 >= surface)) {
                    //            vert += glm::vec3(i, j, k) + ((float)surface - v0) / ((float)v1 - v0) * glm::vec3(1, 0, 0);
                    //            edge_count++;
                    //        }
                    //    }
                    //if (edge_count >= 2) {
                    //    vert /= edge_count;
                    //    //_cell_vertices[POS(i, j, k)] = _vertices.size();
                    //    //_vertices.push_back(vert);
                    //    _points_vertices.push_back(vert);
                    //    _cell_vertices[POS(i, j, k)] = _vertices.size();
                    //    //_vertices.push_back(glm::vec3(i+0.5f, j+0.5f, k+0.5f));
                    //    _vertices.push_back(vert);
                    //}
                    //else {
                    //    _cell_vertices[POS(i, j, k)] = -1;
                    //    //_cell_vertices[POS(i, j, k)] = _vertices.size();
                    //    //_vertices.push_back(glm::vec3(i+0.5f, j+0.5f, k+0.5f));
                    //}
                }
            }
        }
        //for (i32 k = 1; k < ChunkSize - 1; k++) {
        //    for (i32 j = 1; j < ChunkSize - 1; j++) {
        //        for (i32 i = 1; i < ChunkSize - 1; i++) {
        //            u8 v0, v1;
        //            v0 = chunk.voxels[POS(i, j, k+0)].density;
        //            v1 = chunk.voxels[POS(i, j, k+1)].density;
        //            if ((v0 > surface) != (v1 > surface)) {
        //                _indices.push_back(_cell_vertices[POS(i - 1, j - 1, k)]);
        //                _indices.push_back(_cell_vertices[POS(i - 0, j - 1, k)]);
        //                _indices.push_back(_cell_vertices[POS(i - 0, j - 0, k)]);
        //                _indices.push_back(_cell_vertices[POS(i - 0, j - 0, k)]);
        //                _indices.push_back(_cell_vertices[POS(i - 1, j - 1, k)]);
        //                _indices.push_back(_cell_vertices[POS(i - 1, j - 0, k)]);
        //            }

        //            v0 = chunk.voxels[POS(i, j+0, k)].density;
        //            v1 = chunk.voxels[POS(i, j+1, k)].density;
        //            if ((v0 > surface) != (v1 > surface)) {
        //                _indices.push_back(_cell_vertices[POS(i - 1, j, k - 1)]);
        //                _indices.push_back(_cell_vertices[POS(i - 0, j, k - 1)]);
        //                _indices.push_back(_cell_vertices[POS(i - 0, j, k - 0)]);
        //                _indices.push_back(_cell_vertices[POS(i - 0, j, k - 0)]);
        //                _indices.push_back(_cell_vertices[POS(i - 1, j, k - 1)]);
        //                _indices.push_back(_cell_vertices[POS(i - 1, j, k - 0)]);
        //            }

        //            v0 = chunk.voxels[POS(i+0, j, k)].density;
        //            v1 = chunk.voxels[POS(i+1, j, k)].density;
        //            if ((v0 > surface) != (v1 > surface)) {
        //                _indices.push_back(_cell_vertices[POS(i, j - 1, k - 1)]);
        //                _indices.push_back(_cell_vertices[POS(i, j - 0, k - 1)]);
        //                _indices.push_back(_cell_vertices[POS(i, j - 0, k - 0)]);
        //                _indices.push_back(_cell_vertices[POS(i, j - 0, k - 0)]);
        //                _indices.push_back(_cell_vertices[POS(i, j - 1, k - 1)]);
        //                _indices.push_back(_cell_vertices[POS(i, j - 1, k - 0)]);
        //            }
        //        }
        //    }
        //}
        //i32 total = 0;
        //for (i32 i = 0; i < _indices.size(); i++) {
        //    if (_indices[i] == -1) {
        //        //std::cout << i << ": " << -1 << "\n";
        //        total++;
        //    }
        //}
        //std::cout << "Total: " << total << "\n";
        
        for (const auto& e : _active_cells) {
            if (e.pos.x == 0 || e.pos.y == 0 || e.pos.z == 0 ||
                    e.pos.x >= ChunkSize - 2 || e.pos.y >= ChunkSize - 2 || e.pos.z >= ChunkSize - 2)
                continue;

            if ((VAL(e.pos.x, e.pos.y, e.pos.z + 0) > surface) != VAL(e.pos.x, e.pos.y, e.pos.z + 1) > surface) {
                _indices.push_back(_cell_vertices[POS(e.pos.x - 1, e.pos.y - 1, e.pos.z)]);
                _indices.push_back(_cell_vertices[POS(e.pos.x - 0, e.pos.y - 1, e.pos.z)]);
                _indices.push_back(_cell_vertices[POS(e.pos.x - 0, e.pos.y - 0, e.pos.z)]);
                _indices.push_back(_cell_vertices[POS(e.pos.x - 0, e.pos.y - 0, e.pos.z)]);
                _indices.push_back(_cell_vertices[POS(e.pos.x - 1, e.pos.y - 1, e.pos.z)]);
                _indices.push_back(_cell_vertices[POS(e.pos.x - 1, e.pos.y - 0, e.pos.z)]);
            }

            if ((VAL(e.pos.x + 0, e.pos.y, e.pos.z) > surface) != VAL(e.pos.x + 1, e.pos.y, e.pos.z) > surface) {
                _indices.push_back(_cell_vertices[POS(e.pos.x, e.pos.y - 1, e.pos.z - 1)]);
                _indices.push_back(_cell_vertices[POS(e.pos.x, e.pos.y - 0, e.pos.z - 1)]);
                _indices.push_back(_cell_vertices[POS(e.pos.x, e.pos.y - 0, e.pos.z - 0)]);
                _indices.push_back(_cell_vertices[POS(e.pos.x, e.pos.y - 0, e.pos.z - 0)]);
                _indices.push_back(_cell_vertices[POS(e.pos.x, e.pos.y - 1, e.pos.z - 1)]);
                _indices.push_back(_cell_vertices[POS(e.pos.x, e.pos.y - 1, e.pos.z - 0)]);
            }

            if ((VAL(e.pos.x, e.pos.y + 0, e.pos.z) > surface) != VAL(e.pos.x, e.pos.y + 1, e.pos.z) > surface) {
                _indices.push_back(_cell_vertices[POS(e.pos.x - 1, e.pos.y, e.pos.z - 1)]);
                _indices.push_back(_cell_vertices[POS(e.pos.x - 0, e.pos.y, e.pos.z - 1)]);
                _indices.push_back(_cell_vertices[POS(e.pos.x - 0, e.pos.y, e.pos.z - 0)]);
                _indices.push_back(_cell_vertices[POS(e.pos.x - 0, e.pos.y, e.pos.z - 0)]);
                _indices.push_back(_cell_vertices[POS(e.pos.x - 1, e.pos.y, e.pos.z - 1)]);
                _indices.push_back(_cell_vertices[POS(e.pos.x - 1, e.pos.y, e.pos.z - 0)]);
            }

            //switch(e.edge_type) {
            //    case 0: // Oz
            //        _indices.push_back(_cell_vertices[POS(e.pos.x - 1, e.pos.y - 1, e.pos.z)]);
            //        _indices.push_back(_cell_vertices[POS(e.pos.x - 0, e.pos.y - 1, e.pos.z)]);
            //        _indices.push_back(_cell_vertices[POS(e.pos.x - 0, e.pos.y - 0, e.pos.z)]);
            //        _indices.push_back(_cell_vertices[POS(e.pos.x - 0, e.pos.y - 0, e.pos.z)]);
            //        _indices.push_back(_cell_vertices[POS(e.pos.x - 1, e.pos.y - 1, e.pos.z)]);
            //        _indices.push_back(_cell_vertices[POS(e.pos.x - 1, e.pos.y - 0, e.pos.z)]);
            //        break;
            //    case 3: // Ox
            //        _indices.push_back(_cell_vertices[POS(e.pos.x, e.pos.y - 1, e.pos.z - 1)]);
            //        _indices.push_back(_cell_vertices[POS(e.pos.x, e.pos.y - 0, e.pos.z - 1)]);
            //        _indices.push_back(_cell_vertices[POS(e.pos.x, e.pos.y - 0, e.pos.z - 0)]);
            //        _indices.push_back(_cell_vertices[POS(e.pos.x, e.pos.y - 0, e.pos.z - 0)]);
            //        _indices.push_back(_cell_vertices[POS(e.pos.x, e.pos.y - 1, e.pos.z - 1)]);
            //        _indices.push_back(_cell_vertices[POS(e.pos.x, e.pos.y - 1, e.pos.z - 0)]);
            //        break;
            //    case 8: // Oy
            //        _indices.push_back(_cell_vertices[POS(e.pos.x - 1, e.pos.y, e.pos.z - 1)]);
            //        _indices.push_back(_cell_vertices[POS(e.pos.x - 0, e.pos.y, e.pos.z - 1)]);
            //        _indices.push_back(_cell_vertices[POS(e.pos.x - 0, e.pos.y, e.pos.z - 0)]);
            //        _indices.push_back(_cell_vertices[POS(e.pos.x - 0, e.pos.y, e.pos.z - 0)]);
            //        _indices.push_back(_cell_vertices[POS(e.pos.x - 1, e.pos.y, e.pos.z - 1)]);
            //        _indices.push_back(_cell_vertices[POS(e.pos.x - 1, e.pos.y, e.pos.z - 0)]);
            //        break;
            //}
        }
    }
    const std::vector<TVertex>& GetVertices() { return _vertices; }
    const std::vector<TVertex>& GetPointsVertices() { return _points_vertices; }
    const std::vector<TIndex>& GetIndices() { return _indices; }
private:
    struct ActiveCell {
        u8 edge_type;
        Int3 pos;
    };
    HeapArray<TIndex, ChunkVoxelCount> _cell_vertices;
    std::vector<TVertex> _vertices;
    std::vector<TIndex> _indices;
    std::vector<TVertex> _points_vertices;
    std::vector<ActiveCell> _active_cells;
};

