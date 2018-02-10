#pragma once

#include "amd_model.h"

#include <uc_public/assets/assets.h>

namespace AdvancedMicroDevices
{
    namespace GraphicDemos
    {
        class Room
        {

            public:

            using Textures = std::array< std::unique_ptr<UniqueCreator::Assets::Texture2D>, 7 >;

            Room(std::unique_ptr<Model>&& m, Textures&& textures);

            Model* GetModel() const;

            private:

            std::unique_ptr<Model>                                             m_model;
            std::array< std::unique_ptr<UniqueCreator::Assets::Texture2D>, 7 > m_textures;

        };


        std::unique_ptr<Room> CreateRoom();
    }
}

