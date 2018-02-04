#pragma once

#include <memory>

#include "amd_mesh.h"
#include "amd_bone.h"

namespace AdvancedMicroDevices
{
    namespace GraphicDemos
    {
        enum AnimationType : uint32_t
        {
            StaticGeometry      = 0,
            SkinningAnimation,
            ShapeAnimation,
            HierarchyAnimation
        };

        struct Material
        {
            std::array<char, 64> m_diffuse_map_file;
            std::array<char, 64> m_bump_map_file;
        };

        struct BlendData
        {
            UInt32   m_bone;
            Float3   m_offset;
            Float    m_weight;
        };

        struct SkinningData
        {
            UInt32      m_num_bones;
            BlendData*  m_blend_data;
        };

        struct Triangle
        {
            uint32_t v[3];
            Float3   m_normal;
        };

        struct OffsetTable
        {
            UInt32  m_material_chunk;
            UInt32  m_vertex_chunk;
            UInt32  m_triangle_chunk;
            UInt32  m_mesh_chunk;
            UInt32  m_bone_chunk;
        };

        class Model : public Transform
        {
            public:

            std::vector<Float3>             m_positions;
            std::vector<Float3>             m_normal;
            std::vector<Float3>             m_tangent;
            std::vector<Float3>             m_binormal;
            std::vector<Float2>             m_texture_coord;
            std::vector<Float4>             m_color;

        protected:

            std::array<char, 64>            m_texture_path;
            int32_t                         m_animation_type = 0;
            uint32_t                        m_num_vertices = 0;

            std::unique_ptr<SkinningData>   m_skinning_data;
            std::vector<Triangle>           m_triangle;
            std::vector<Material>           m_material;

            std::vector<Mesh>               m_mesh;

            std::vector< Transform>         m_mesh_hie;
            
            std::vector< Transform>         m_bone_hie;
            std::vector< Bone>              m_bones;
        };

        void AddToMaterialRange(uint32_t *indices, uint32_t &index, const int32_t mat, const uint32_t startMesh, const uint32_t meshCount, const Mesh* meshes, const Triangle* triangles, uint32_t* material_range);

        std::unique_ptr<Model> CreateFromFile( const wchar_t* file_name );
    }
}

