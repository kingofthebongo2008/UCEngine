#include "pch.h"

#include <uc_dev/gx/anm/skeleton_instance.h>
#include <uc_dev/gx/anm/transforms.h>

#include <uc_dev/gx/lip/animation.h>

namespace uc {
    namespace gx {
        namespace anm {
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            skeleton_instance::skeleton_instance(const lip::skeleton* s) : m_skeleton(s)
            {
                reset();
            }
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            void skeleton_instance::reset()
            {
                m_joint_local_transforms2.resize(m_skeleton->m_joint_local_transforms2.size());

                auto s = m_skeleton->m_joint_local_transforms2.size();

                for (auto i = 0U; i < s; ++i)
                {
                    m_joint_local_transforms2[i] = math::load44(reinterpret_cast<const float*>(&m_skeleton->m_joint_local_transforms2[i]));
                }
            }
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            std::vector< math::float4x4 >& skeleton_instance::local_transforms()
            {
                return m_joint_local_transforms2;
            }
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
            std::vector< math::float4x4 > skeleton_instance::concatenate_transforms(math::afloat4x4 locomotion_transform)
            {
                return gx::anm::local_to_world_joints2(m_skeleton, local_transforms(), locomotion_transform);
            }
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        }
    }
}

           
