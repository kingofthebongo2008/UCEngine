#pragma once

#include <cstdint>

#include <uc_dev/util/pimpl.h>
#include <uc_dev/gx/dx12/gpu/managed_buffer.h>
#include <uc_dev/gx/geo/vertex_strides.h>

#include <uc_dev/gx/dx12/gpu/buffer.h>


namespace uc
{
    namespace gx
    {
        namespace geo
        {
            class base_geometry_allocator_impl;

            using vertex_buffer_view     = D3D12_VERTEX_BUFFER_VIEW;
            using gpu_virtual_address   = D3D12_GPU_VIRTUAL_ADDRESS;

            class base_geometry_allocator : public util::noncopyable
            {
                protected:

                static inline vertex_buffer_view make_view(gpu_virtual_address address, size_t size, uint32_t stride)
                {
                    return { address, static_cast<uint32_t>(size), stride };
                }

                public:

                struct geometry_allocation
                {
                    uint32_t m_vertex_count;
                    uint32_t m_vertex_offset;
                    void*    m_opaque_handle;
                };

                base_geometry_allocator(vertex_buffer_view view );

                uint32_t            stride() const;
                uint32_t            size() const;
                gpu_virtual_address address() const;

                vertex_buffer_view  view() const;

                geometry_allocation allocate(uint32_t vertex_count);

                void                free(geometry_allocation free);
                void                sync();

                private:
                util::details::pimpl<base_geometry_allocator_impl> m_impl;
            };

            template< uint32_t stride_in_bytes > class typed_geometry_allocator : public base_geometry_allocator
            {
                using base = base_geometry_allocator;

                public:

                static constexpr uint32_t stride_in_bytes_local = stride_in_bytes;

                typed_geometry_allocator(gpu_virtual_address address, size_t size) : base(make_view( address, size, stride_in_bytes_local ) )
                {

                }

            };

            class position_geometry_allocator : public typed_geometry_allocator< default_geometry_position::stride::value >
            {
                using base = typed_geometry_allocator< default_geometry_position::stride::value >;

                public:

                static constexpr uint32_t stride_in_bytes = default_geometry_position::stride::value;

                position_geometry_allocator(gpu_virtual_address address, size_t size) : base(address, size)
                {

                }
            };

            class normal_geometry_allocator : public typed_geometry_allocator< default_geometry_normal::stride::value >
            {
                using base = typed_geometry_allocator< default_geometry_normal::stride::value >;
                public:
                static constexpr uint32_t stride_in_bytes = default_geometry_normal::stride::value;
                normal_geometry_allocator(gpu_virtual_address address, size_t size) : base(address, size)
                {

                }
            };

            class uv_geometry_allocator : public typed_geometry_allocator< default_geometry_uv::stride::value >
            {
                using base = typed_geometry_allocator< default_geometry_uv::stride::value >;

                public:
                static constexpr uint32_t stride_in_bytes = default_geometry_uv::stride::value;
                uv_geometry_allocator(gpu_virtual_address address, size_t size) : base(address, size)
                {

                }
            };

            class blend_weight_geometry_allocator : public typed_geometry_allocator< default_geometry_blend_weight::stride::value >
            {
                using base = typed_geometry_allocator< default_geometry_blend_weight::stride::value >;
                public:
                static constexpr uint32_t stride_in_bytes = default_geometry_blend_weight::stride::value;
                blend_weight_geometry_allocator(gpu_virtual_address address, size_t size) : base(address, size)
                {

                }
            };

            class blend_index_geometry_allocator : public typed_geometry_allocator< default_geometry_blend_index::stride::value >
            {
                using base = typed_geometry_allocator< default_geometry_blend_index::stride::value >;
                public:
                static constexpr uint32_t stride_in_bytes = default_geometry_blend_index::stride::value;
                blend_index_geometry_allocator(gpu_virtual_address address, size_t size) : base(address, size)
                {

                }
            };

            class index_geometry_allocator : public typed_geometry_allocator< default_geometry_index::stride::value >
            {
                using base = typed_geometry_allocator< default_geometry_index::stride::value >;

                public:
                static constexpr uint32_t stride_in_bytes = default_geometry_index::stride::value;
                index_geometry_allocator(gpu_virtual_address address, size_t size) : base(address, size)
                {

                }
            };
        }
    }
}

