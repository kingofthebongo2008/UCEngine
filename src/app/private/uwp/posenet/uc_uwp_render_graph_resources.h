#pragma once
#include <uc_dev/util/noncopyable.h>
#include <cstdint>

namespace uc
{
    namespace uwp
    {
        namespace gxu
        {
            namespace render_graph
            {
                struct resource
                {
                    enum class type : uint32_t
                    {
                        unknown         = 0,
                        render_target   = 1,
                        depth_buffer    = 2,
                        swap_chain      = 3,
                    };

                    virtual ~resource() = default;

                    resource(const resource&) = delete;
                    resource& operator=(const resource&) = delete;

                    resource(resource&&) = default;
                    resource& operator=(resource&&) = default;

                    bool is_external() const
                    {
                        return m_flags == 1;
                    }

                    private:
                    type     m_type;
                    uint32_t m_flags = 0;

                    protected:
                    resource(type t) : m_type(t) {}

                    void set_flags(bool external)
                    {
                        m_flags = external ? 1 : 0;
                    }
                };

                struct render_target  final : public resource
                {
                    render_target() : resource(resource::type::render_target) {}
                };

                struct depth_buffer final : public resource
                {
                    depth_buffer() : resource(resource::type::depth_buffer) {}
                };

                struct swap_chain final : public resource
                {
                    swap_chain(void* e) : resource(resource::type::swap_chain), m_external_resource(e) { set_flags(true); }

                    void* m_external_resource;
                };

                enum flags : uint64_t
                {
                    flags_common = 0,
                    flags_vertex_and_constant_buffer = 0x1,
                    flags_index_buffer = 0x2,
                    flags_render_target = 0x4,
                    flags_unordered_access = 0x8,
                    flags_depth_write = 0x10,
                    flags_depth_read = 0x20,
                    flags_non_pixel_shader_resource = 0x40,
                    flags_pixel_shader_resource = 0x80,
                    flags_stream_out = 0x100,
                    flags_indirect_argument = 0x200,
                    flags_copy_dest = 0x400,
                    flags_copy_source = 0x800,
                    flags_resolve_dest = 0x1000,
                    flags_resolve_source = 0x2000,
                    flags_raytracing_acceleration_structure = 0x400000,
                    flags_shading_rate_source = 0x1000000,
                    flags_generic_read = (((((0x1 | 0x2) | 0x40) | 0x80) | 0x200) | 0x800),
                    flags_present = 0,
                    flags_predication = 0x200,
                    flags_video_decode_read = 0x10000,
                    flags_video_decode_write = 0x20000,
                    flags_video_process_read = 0x40000,
                    flags_video_process_write = 0x80000,
                    flags_video_encode_read = 0x200000,
                    flags_video_encode_write = 0x800000
                };

                struct writer
                {
                    resource* m_resource = nullptr;
                    uint64_t  m_flags    = 0;
                };

                struct reader
                {
                    resource* m_resource = nullptr;
                    uint64_t  m_flags    = 0;
                };
            }
        }
    }
}
    
