#include "pch.h"

#include "gi_room.h"

#include <ppl.h>


namespace AdvancedMicroDevices
{
    namespace GraphicDemos
    {
        namespace
        {

            void AddToMaterialRange(uint32_t *indices, uint32_t &index, const int32_t mat, const uint32_t startMesh, const uint32_t meshCount, const Mesh* meshes, const Triangle* triangles, uint32_t* material_range)
            {
                for (uint32_t mesh = startMesh; mesh < startMesh + meshCount; mesh++)
                {
                    uint32_t base = meshes[mesh].m_base_triangle;
                    uint32_t count = meshes[mesh].m_num_triangles;

                    for (uint32_t p = 0; p < count; p++)
                    {
                        indices[index++] = triangles[base + p].m_v[0];
                        indices[index++] = triangles[base + p].m_v[1];
                        indices[index++] = triangles[base + p].m_v[2];
                    }
                }

                material_range[mat + 1] = index;
            }
        }


        Room::Room(std::unique_ptr<Model>&& m, Room::Textures&& textures):
            m_model(std::move(m))
            , m_textures(std::move(textures))
        {}

        Model* Room::GetModel() const
        {
            return m_model.get();
        }

        std::unique_ptr<Room> CreateRoom()
        {
            std::array<const wchar_t*, 7 > texture_file_names =
            {
                L"appdata/textures/lopal.texture",
                L"appdata/textures/headpal.texture",
                L"appdata/textures/picture.texture",
                L"appdata/textures/floor.texture",
                L"appdata/textures/globe.texture",
                L"appdata/textures/wall_lm.texture",
                L"appdata/textures/ceiling_lm.texture"
            };

            constexpr auto textures_size = sizeof(texture_file_names) / sizeof(texture_file_names[0]);

            concurrency::task_group g;

            std::array< std::unique_ptr<UniqueCreator::Assets::Texture2D>, textures_size > textures;

            for (auto i = 0; i < textures_size; ++i)
            {
                // 3. Load packaged binarized compressed image 
                g.run([&textures, &texture_file_names, i]
                {
                    auto factory = std::make_unique<UniqueCreator::Assets::Texture2DFactory>();
                    textures[i] = factory->CreateFromFile(texture_file_names[i]);
                });
            }

            std::unique_ptr<Model> m = CreateFromFile(L"appdata/models/gi_room.am");
            g.wait();

            std::unique_ptr<Room> r = std::make_unique<Room>( std::move(m), std::move(textures));
            auto model = r->GetModel();

            r->m_indices.resize(model->GetTriangleCount() * 3);

            r->m_material_range[0] = 0;

            {
                auto indices = &r->m_indices[0];
                uint32_t index = 0;

                const auto& meshes    = &model->m_mesh[0];
                const auto& triangles = &model->m_triangle[0];
                const auto& range     = &r->m_material_range[0];

                // Untextured materials
                AddToMaterialRange(indices, index, 0, 19, 1, meshes, triangles, range); // Hand
                AddToMaterialRange(indices, index, 1, 20, 1, meshes, triangles, range); // Ball
                AddToMaterialRange(indices, index, 1, 23, 1, meshes, triangles, range); // Horse
                AddToMaterialRange(indices, index, 1, 25, 1, meshes, triangles, range); // Sci-Fi weirdo
                AddToMaterialRange(indices, index, 1, 28, 1, meshes, triangles, range); // Bench
                AddToMaterialRange(indices, index, 1, 30, 1, meshes, triangles, range); // Frame
                AddToMaterialRange(indices, index, 2, 24, 1, meshes, triangles, range); // Horse stand
                AddToMaterialRange(indices, index, 2, 26, 1, meshes, triangles, range); // Sci-Fi weirdo stand
                AddToMaterialRange(indices, index, 2, 32, 1, meshes, triangles, range); // Globe stand
                AddToMaterialRange(indices, index, 3, 3, 15, meshes, triangles, range); // Ceiling, Pillars, Stands, Wall lights
                AddToMaterialRange(indices, index, 4, 0, 1, meshes, triangles, range); // Walls
                AddToMaterialRange(indices, index, 5, 21, 1, meshes, triangles, range); // Teapot

                // Masked materials
                AddToMaterialRange(indices, index, 6, 27, 1, meshes, triangles, range); // Globe

                // Textured materials
                AddToMaterialRange(indices, index, 7, 18, 1, meshes, triangles, range); // Ball-horse
                AddToMaterialRange(indices, index, 8, 22, 1, meshes, triangles, range); // Head
                AddToMaterialRange(indices, index, 9, 29, 1, meshes, triangles, range); // Picture
                AddToMaterialRange(indices, index, 10, 1, 1, meshes, triangles, range); // Floor

                // Lightmapped materials
                AddToMaterialRange(indices, index, 11, 2, 1, meshes, triangles, range); // Ceiling
                AddToMaterialRange(indices, index, 12, 31, 1, meshes, triangles, range); // Wall light quads


            }
            return r;
        }
    }
}


