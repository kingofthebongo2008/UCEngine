#pragma once

#include <uc_public/assets/graphics/skinned_model.h>
#include <uc_public/assets/graphics/texture_2d.h>
#include <uc_dev/gx/lip/model.h>

namespace UniqueCreator
{
    namespace Assets
    {
        class SkinnedModel::Impl
        {
        public:

            class Texture2DInternal : public Texture2D
            {
            public:
                Texture2DInternal(uc::lip::texture2d* texture)
                {
                    m_impl = texture;
                }
            };

            Impl() {}

            Impl(uc::lip::unique_lip_pointer<uc::lip::skinned_model> && model) : m_model(std::move(model))
            {
                m_texture_wrappers.resize(m_model->m_textures.size());
                m_texture_storage.resize(m_model->m_textures.size());

                auto size = m_model->m_textures.size();
                for ( auto i= 0U; i < size; ++i )
                {
                    m_texture_storage[i]    = std::make_unique<Texture2DInternal>(&m_model->m_textures[i]);
                    m_texture_wrappers[i]   = m_texture_storage[i].get();
                }
            }

            uc::lip::unique_lip_pointer<uc::lip::skinned_model> m_model;
            std::vector< Texture2D* >                           m_texture_wrappers;
            std::vector< std::unique_ptr<Texture2D> >           m_texture_storage;
        };
    }
}



