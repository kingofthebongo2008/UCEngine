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
                    std::unique_ptr<fbxsdk::FbxManager, fbxmanager_deleter>     manager(fbxsdk::FbxManager::Create(), fbxmanager_deleter() );
                    std::unique_ptr<fbxsdk::FbxScene, fbxscene_deleter>         scene(fbxsdk::FbxScene::Create(manager.get(),""), fbxscene_deleter() );
                    std::unique_ptr<fbxsdk::FbxImporter, fbximporter_deleter>   importer(fbxsdk::FbxImporter::Create(manager.get(), ""), fbximporter_deleter());

                    auto f = file_name;

                    auto import_status = importer->Initialize(f.c_str(), -1, manager->GetIOSettings());
                    
                    if (import_status == false)
                    {
                        auto status = importer->GetStatus();
                        auto error = status.GetErrorString();
                        ::uc::gx::throw_exception<uc::gx::fbx_exception>(error);
                    }

                    import_status = importer->Import(scene.get());
                    if (import_status == false)
                    {
                        auto status = importer->GetStatus();
                        auto error = status.GetErrorString();
                        ::uc::gx::throw_exception<uc::gx::fbx_exception>(error);
                    }

                    FbxGeometryConverter geometryConverter(manager.get());
                    geometryConverter.Triangulate(scene.get(), true);

                    fbxsdk::FbxAxisSystem scene_axis_system = scene->GetGlobalSettings().GetAxisSystem();
                    fbxsdk::FbxAxisSystem our_axis_system   = fbxsdk::FbxAxisSystem(fbxsdk::FbxAxisSystem::EPreDefinedAxisSystem::eDirectX);
                    
                    if (scene_axis_system != our_axis_system)
                    {
                       //our_axis_system.ConvertScene(scene.get());
                    }

                    fbxsdk::FbxSystemUnit units     = scene->GetGlobalSettings().GetSystemUnit();
                    fbxsdk::FbxSystemUnit meters    = fbxsdk::FbxSystemUnit::m;

                    if (units != fbxsdk::FbxSystemUnit::m)
                    {
                        //FbxSystemUnit::m.ConvertScene(scene.get());
                    }

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
