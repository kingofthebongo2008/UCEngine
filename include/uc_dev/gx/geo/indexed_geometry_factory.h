#pragma once

#include <uc_dev/gx/geo/indexed_geometry.h>
#include <gsl/gsl>
#include <uc_dev/gx/lip/model.h>

namespace uc
{
    namespace gx
    {
        namespace dx12
        {
            class gpu_resource_create_context;
            class gpu_upload_queue;
        }
        namespace geo
        {
            indexed_geometry        create_indexed_geometry(gx::dx12::gpu_resource_create_context* rc, gx::dx12::gpu_upload_queue* upload_queue, const void* __restrict positions, size_t positions_size, const void* __restrict indices, size_t indices_size);
            parametrized_geometry   create_parametrized_geometry(gx::dx12::gpu_resource_create_context* rc, gx::dx12::gpu_upload_queue* upload_queue, const void* __restrict positions, size_t positions_size, const void* __restrict indices, size_t indices_size, const void* __restrict uv, size_t uv_size);
            multi_material_geometry create_multi_material_geometry(gx::dx12::gpu_resource_create_context* rc, gx::dx12::gpu_upload_queue* upload_queue, gsl::span<const gsl::byte> positions, gsl::span<const gsl::byte> indices, gsl::span<const gsl::byte> uv, const lip::reloc_array<lip::primitive_range>& ranges);
            skinned_geometry        create_skinned_geometry(gx::dx12::gpu_resource_create_context* rc, gx::dx12::gpu_upload_queue* upload_queue, gsl::span<const gsl::byte> positions, gsl::span<const gsl::byte> indices, gsl::span<const gsl::byte> uv, gsl::span<const gsl::byte> blend_weight, gsl::span<const gsl::byte> blend_indices, const lip::reloc_array<lip::primitive_range>& ranges);
        }
    }
}

