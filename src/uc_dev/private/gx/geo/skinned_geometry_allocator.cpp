#include "pch.h"

#include <uc_dev/gx/geo/skinned_geometry_allocator.h>
#include <uc_dev/gx/dx12/gpu/buffer.h>
#include <uc_dev/gx/dx12/gpu/resource_create_context.h>

namespace uc
{
    namespace gx
    {
        namespace geo
        {
            namespace details
            {
                static inline uint32_t make_positions_size( uint32_t vertex_count )
                {
                    return vertex_count * default_geometry_position::stride::value;
                }

                static inline uint32_t make_uv_size(uint32_t vertex_count)
                {
                    return vertex_count * default_geometry_uv::stride::value;
                }

                static inline uint32_t make_blend_weight_size(uint32_t vertex_count)
                {
                    return vertex_count * default_geometry_blend_weight::stride::value;
                }

                static inline uint32_t make_blend_index_size(uint32_t vertex_count)
                {
                    return vertex_count * default_geometry_blend_index::stride::value;
                }

                static inline uint32_t make_index_size(uint32_t index_count)
                {
                    return index_count * default_geometry_index::stride::value;
                }

                static inline skinned_meshes_allocator::allocator_addresses make_skinned_addresses(
                    const dx12::gpu_buffer* positions,
                    const dx12::gpu_buffer* uv,
                    const dx12::gpu_buffer* blend_weight,
                    const dx12::gpu_buffer* blend_index)
                {
                    skinned_meshes_allocator::allocator_addresses r;

                    {
                        r.m_positions       = positions->virtual_address();
                        r.m_positions_size  = static_cast<uint32_t>(size(positions));
                    }

                    {
                        r.m_uv = uv->virtual_address();
                        r.m_uv_size = static_cast<uint32_t>(size(uv));
                    }

                    {
                        r.m_blend_weights       = blend_weight->virtual_address();
                        r.m_blend_weights_size  = static_cast<uint32_t>(size(blend_weight));
                    }

                    {
                        r.m_blend_indices       = blend_index->virtual_address();
                        r.m_blend_indices_size  = static_cast<uint32_t>(size(blend_index));
                    }
                    
                    return r;
                }
            }

            skinned_geometry_allocator::skinned_geometry_allocator(dx12::gpu_resource_create_context* rc, uint32_t vertex_count) :
            m_skinned_mesh_position (dx12::create_buffer(rc, details::make_positions_size( vertex_count)))
            , m_skinned_mesh_uv(dx12::create_buffer(rc, details::make_uv_size(vertex_count)))
            , m_skinned_mesh_blend_weight(dx12::create_buffer(rc, details::make_blend_weight_size(vertex_count)))
            , m_skinned_mesh_blend_index(dx12::create_buffer(rc, details::make_blend_index_size(vertex_count)))
            , m_skinned_meshes(details::make_skinned_addresses( m_skinned_mesh_position.get(), m_skinned_mesh_uv.get(), m_skinned_mesh_blend_weight.get(), m_skinned_mesh_blend_index.get()))
            {

            }

            skinned_geometry_allocator::skinned_allocation*  skinned_geometry_allocator::allocate(size_t vertex_count)
            {
                return m_skinned_meshes.allocate(static_cast<uint32_t>(vertex_count));
            }

            void skinned_geometry_allocator::free(skinned_geometry_allocator::skinned_allocation* free)
            {
                m_skinned_meshes.free(free);
            }

            dx12::gpu_buffer* skinned_geometry_allocator::skinned_mesh_position() const
            {
                return m_skinned_mesh_position.get();
            }

            dx12::gpu_buffer* skinned_geometry_allocator::skinned_mesh_uv() const
            {
                return m_skinned_mesh_uv.get();
            }

            dx12::gpu_buffer* skinned_geometry_allocator::skinned_mesh_blend_weight() const
            {
                return m_skinned_mesh_blend_weight.get();
            }

            dx12::gpu_buffer* skinned_geometry_allocator::skinned_mesh_blend_index() const
            {
                return m_skinned_mesh_blend_index.get();
            }

            vertex_buffer_view   skinned_geometry_allocator::skinned_mesh_position_view() const
            {
                return m_skinned_meshes.view(gx::geo::skinned_meshes_allocator::component::position);
            }

            vertex_buffer_view   skinned_geometry_allocator::skinned_mesh_uv_view() const
            {
                return m_skinned_meshes.view(gx::geo::skinned_meshes_allocator::component::uv);
            }

            vertex_buffer_view   skinned_geometry_allocator::skinned_mesh_blend_weight_view() const
            {
                return m_skinned_meshes.view(gx::geo::skinned_meshes_allocator::component::blend_weight);
            }

            vertex_buffer_view   skinned_geometry_allocator::skinned_mesh_blend_index_view() const
            {
                return m_skinned_meshes.view(gx::geo::skinned_meshes_allocator::component::blend_index);
            }

            void skinned_geometry_allocator::sync()
            {
                m_skinned_meshes.sync();
            }
        }
    }
}

