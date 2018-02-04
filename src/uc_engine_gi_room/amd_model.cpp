#include "pch.h"

#include "amd_model.h"

#include <array>
#include <ppl.h>

#include <uc_public/assets/assets.h>


namespace AdvancedMicroDevices
{
    namespace GraphicDemos
    {
       
        

        std::unique_ptr<Model> CreateFromFile(const wchar_t* file_name)
        {
            std::unique_ptr<Model> r = std::make_unique<Model>();

            return r;
        }

        /*
        void AddToMaterialRange(uint32_t *indices, uint32_t &index, const int32_t mat, const uint32_t startMesh, const uint32_t meshCount, const Mesh* meshes, const Triangle* triangles, uint32_t* material_range)
        {
        for (uint32_t mesh = startMesh; mesh < startMesh + meshCount; mesh++)
        {
        uint32_t base = meshes[mesh].m_base_triangle;
        uint32_t count = meshes[mesh].m_triangle_count;

        for (uint32_t p = 0; p < count; p++)
        {
        indices[index++] = triangles[base + p].m_v[0];
        indices[index++] = triangles[base + p].m_v[1];
        indices[index++] = triangles[base + p].m_v[2];
        }
        }
        material_range[mat + 1] = index;
        }

        std::unique_ptr<Model> CreateGIRoom()
        {
            std::unique_ptr<Model> r = std::make_unique<Model>();

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
            

            return r;

        }
        */
    }
}


