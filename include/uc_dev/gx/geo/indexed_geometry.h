#pragma once

#include <d3d12.h>
#include <vector>

#include <uc_dev/gx/dx12/gpu/managed_buffer.h>
#include <uc_dev/gx/geo/vertex_strides.h>

namespace uc
{
    namespace gx
    {
        namespace geo
        {
            struct indexed_geometry_base
            {
                gx::dx12::managed_gpu_buffer    m_buffer;
                uint32_t                        m_vertex_count;
                uint32_t                        m_index_count;
            };

            struct indexed_geometry : public indexed_geometry_base
            {
                //todo: compress
                static constexpr auto position_stride   = default_geometry_position::stride::value;
                static constexpr auto index_stride      = default_geometry_index::stride::value;

                using base                              = indexed_geometry_base;
            };

            struct parametrized_geometry : public indexed_geometry
            {
                //todo: compress
                static constexpr auto position_stride   = default_geometry_position::stride::value;
                static constexpr auto uv_stride         = default_geometry_uv::stride::value;
                static constexpr auto index_stride      = default_geometry_index::stride::value;
            };

            struct primitive_range
            {
                uint32_t m_begin = 0;
                uint32_t m_end   = 0;

                primitive_range(uint32_t begin, uint32_t end) : m_begin(begin), m_end(end)
                {

                }

                uint32_t size() const
                {
                    return m_end - m_begin;
                }
            };

            struct multi_material_geometry : public parametrized_geometry
            {
                std::vector< primitive_range > m_ranges;

                primitive_range indexes() const
                {
                    return primitive_range ( m_ranges[0].m_begin, m_ranges.back().m_end );
                }
            };

            struct skinned_geometry : public multi_material_geometry
            {
                //todo: compress
                static constexpr auto blend_weights_stride = default_geometry_blend_weight::stride::value;
                static constexpr auto blend_indices_stride = default_geometry_blend_index::stride::value;
            };

            uint64_t size(const indexed_geometry* g);
            uint64_t size(const indexed_geometry& g);
            uint64_t size(const parametrized_geometry* g);
            uint64_t size(const parametrized_geometry& g);

            D3D12_VERTEX_BUFFER_VIEW make_position_buffer_view(const indexed_geometry* g, uint32_t offset = 0);
            D3D12_INDEX_BUFFER_VIEW make_index_buffer_view(const indexed_geometry* g, uint32_t begin = 0, uint32_t end = 0);
            D3D12_INDEX_BUFFER_VIEW make_index_buffer_view(const parametrized_geometry* g, uint32_t begin = 0, uint32_t end = 0);
            D3D12_VERTEX_BUFFER_VIEW make_uv_buffer_view(const parametrized_geometry* g, uint32_t offset = 0);
            D3D12_VERTEX_BUFFER_VIEW make_blend_weights_buffer_view(const skinned_geometry* g, uint32_t offset = 0);
            D3D12_VERTEX_BUFFER_VIEW make_blend_indices_buffer_view(const skinned_geometry* g, uint32_t offset = 0);
        }
    }
}

