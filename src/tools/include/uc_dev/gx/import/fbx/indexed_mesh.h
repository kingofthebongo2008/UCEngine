#pragma once

#include <uc_dev/gx/import/geo/indexed_mesh.h>
#include <uc_dev/gx/import/fbx/fbx_common.h>

namespace uc
{
    namespace gx
    {
        namespace import
        {
            namespace fbx
            {

                inline std::shared_ptr<geo::indexed_mesh> create_mesh(const std::string& file_name)
                {
                    auto context = load_fbx_file(file_name);
                    auto scene = context->m_scene.get();

                    fbxsdk::FbxMesh* mesh;
                    fbxsdk::FbxNode* mesh_node;

                    std::tie( mesh, mesh_node ) = get_mesh(scene->GetRootNode());;

                    if (mesh == nullptr || !mesh->IsTriangleMesh())
                    {
                        ::uc::gx::throw_exception<uc::gx::fbx_exception>("file does not contain a triangle mesh");
                    }

                    assert(mesh_node);
          
                    mesh->RemoveBadPolygons();
                    mesh->ComputeBBox();

                    assert(mesh->GetPolygonSize(0));

                    return std::make_shared<geo::indexed_mesh>(get_positions(mesh), get_uvs(mesh), get_faces(mesh));
                }
            }
         }
     }
}
