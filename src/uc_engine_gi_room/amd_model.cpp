#include "pch.h"

#include "amd_model.h"

#include <array>
#include <ppl.h>

#include <uc_public/assets/assets.h>
#include "amd_file_stream.h"


namespace AdvancedMicroDevices
{
    namespace GraphicDemos
    {
        namespace
        {
            struct OffsetTable
            {
                UInt32  m_material_chunk;
                UInt32  m_vertex_chunk;
                UInt32  m_triangle_chunk;
                UInt32  m_mesh_chunk;
                UInt32  m_bone_chunk;
            };
        }
       
        std::unique_ptr<Model> CreateFromFile(const wchar_t* file_name)
        {
            SimpleReadFileStream s(file_name);

            std::unique_ptr<Model> r = std::make_unique<Model>();

            wcscpy_s(&r->m_texture_path[0], 1, _T(""));

            uint32_t version = 0;
            OffsetTable offsetTable;

            char Header[9] = "AtiModel";
            char ReadMark[9];

            s.ReadBuffer(ReadMark, 8);
            ReadMark[8] = '\0';
            
            if ( _stricmp(ReadMark, Header) )
            {
                throw StreamReadException();
            }

            Read( s, r->m_animation_type );
            Read( s, version );
            Read( s, offsetTable );

            s.Seek(offsetTable.m_material_chunk);

            uint32_t num_materials;
            Read( s, num_materials );

            r->m_material.resize(num_materials);

            for ( auto i = 0U; i < num_materials; ++i )
            {
                //skip...
                
                s.SeekRelative(64);
                s.SeekRelative(sizeof(Float4));
                s.SeekRelative(sizeof(Float4));
                s.SeekRelative(sizeof(Float));

                {
                    char buf[64];
                    s.ReadBuffer(&buf[0], 64);
                    MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, buf, -1, &r->m_material[i].m_diffuse_map_file[0], static_cast<int>(r->m_material[i].m_diffuse_map_file.size()));
                }

                //skip
                s.SeekRelative(64);

                {
                    char buf[64];
                    s.ReadBuffer(&buf[0], 64);
                    MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, buf, -1, &r->m_material[i].m_bump_map_file[0], static_cast<int>(r->m_material[i].m_bump_map_file.size()));

                }

                //skip
                s.SeekRelative(64);
                s.SeekRelative(64);
            }

            /*
            fseek(fp, offsetTable.materialChunk, SEEK_SET);
            fread(&m_numMaterials, sizeof(DWORD), 1, fp);
            m_pMaterial = new Material[m_numMaterials];


            for (DWORD i = 0; i < m_numMaterials; i++)
            {
                fseek(fp, 64, SEEK_CUR);
                fseek(fp, sizeof(float4), SEEK_CUR);
                fseek(fp, sizeof(float4), SEEK_CUR);
                fseek(fp, sizeof(float), SEEK_CUR);

#ifdef _UNICODE
                char buf[64];
                fread(buf, 1, 64, fp);
                MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, buf, -1, m_pMaterial[i].diffuseMapFile, sizeof(m_pMaterial[i].diffuseMapFile));
#else
                fread(m_pMaterial[i].diffuseMapFile, 1, 64, fp);
#endif

                fread(buf, 1, 64, fp);
#ifdef _UNICODE
                fread(buf, 1, 64, fp);
                MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, buf, -1, m_pMaterial[i].bumpMapFile, sizeof(m_pMaterial[i].bumpMapFile));
#else
                fread(m_pMaterial[i].bumpMapFile, 1, 64, fp);
#endif
                fread(buf, 1, 64, fp);
                fread(buf, 1, 64, fp);
            }

            fseek(fp, offsetTable.vertexChunk, SEEK_SET);
            fread(&m_numVertices, sizeof(DWORD), 1, fp);
            */

            s.Seek(offsetTable.m_vertex_chunk);

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


