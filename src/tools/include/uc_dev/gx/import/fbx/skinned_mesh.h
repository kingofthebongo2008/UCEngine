#pragma once

#include <tuple>
#include <uc_dev/gx/import/geo/skinned_mesh.h>
#include <uc_dev/gx/import/fbx/fbx_common.h>
#include <uc_dev/gx/import/fbx/fbx_common_multi_material_mesh.h>

namespace uc
{
    namespace gx
    {
        namespace import
        {
            namespace fbx
            {
                struct joint_influence
                {
                    std::vector<float>    m_weight;
                    std::vector<uint32_t> m_index;
                };

                //////////////////////
                inline geo::skinned_mesh::blend_weight_t get_blend_weight(const std::vector<float>& v)
                {
                    geo::skinned_mesh::blend_weight_t r = {};
                    float* as_float = &r.x;
                    auto j = 0U;

                    for (auto&& i : v)
                    {
                        if (j >= 4)
                        {
                            break;
                        }
                        as_float[j++] = i;
                    }

                    return r;
                }

                //////////////////////
                inline geo::skinned_mesh::blend_index_t get_blend_index(const std::vector<uint32_t>& v)
                {
                    geo::skinned_mesh::blend_index_t r = {};
                    uint16_t* as_float = &r.x;
                    auto j = 0U;

                    for (auto&& i : v)
                    {
                        if (j >= 4)
                        {
                            break;
                        }
                        as_float[j++] = static_cast<uint16_t>(i);
                    }

                    return r;
                }

                // Get the geometry deformation local to a node. It is never inherited by the
                // children.
                inline fbxsdk::FbxAMatrix get_geometry(FbxNode* pNode)
                {
                    fbxsdk::FbxVector4 lT, lR, lS;
                    fbxsdk::FbxAMatrix lGeometry;

                    lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
                    lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
                    lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

                    lGeometry.SetT(lT);
                    lGeometry.SetR(lR);
                    lGeometry.SetS(lS);

                    return lGeometry;
                }

                inline math::float4 convert_to_float4(fbxsdk::FbxVector4 v)
                {
                    float r[4];
                    const double* d = v;
                    r[0] = static_cast<float>(d[0]);
                    r[1] = static_cast<float>(d[1]);
                    r[2] = static_cast<float>(d[2]);
                    r[3] = static_cast<float>(d[3]);
                    return math::load4u(&r[0]);
                }

                inline math::float4 convert_to_float4(fbxsdk::FbxQuaternion v)
                {
                    float r[4];
                    const double* d = v;
                    r[0] = static_cast<float>(d[0]);
                    r[1] = static_cast<float>(d[1]);
                    r[2] = static_cast<float>(d[2]);
                    r[3] = static_cast<float>(d[3]);
                    return math::load4u(&r[0]);
                }

                inline math::float4x4 convert_to_float4x4(const fbxsdk::FbxAMatrix v)
                {
                    math::float4 r[4];

                    r[0] = convert_to_float4(v.GetRow(0));
                    r[1] = convert_to_float4(v.GetRow(1));
                    r[2] = convert_to_float4(v.GetRow(2));
                    r[3] = convert_to_float4(v.GetRow(3));

                    return math::load44u(reinterpret_cast<const float*>(&r[0]));
                }

                inline geo::joint_transform joint_transform(fbxsdk::FbxAMatrix& m)
                {
                    geo::joint_transform r;

                    r.m_rotation    = math::quaternion_normalize(convert_to_float4(m.GetUnnormalizedQ()));
                    r.m_translation = convert_to_float4(m.GetT());

                    return r;
                }

                inline geo::joint_transform_matrix joint_transform_matrix(const fbxsdk::FbxAMatrix& m)
                {
                    geo::joint_transform_matrix r;

                    auto t = convert_to_float4(m.GetT());
                    
                    //move to row major
                    r.m_transform = convert_to_float4x4(m);
                    return r;
                }

                inline void get_skeleton_pose(const fbxsdk::FbxMesh* mesh)
                {
                    std::vector<int32_t>               parents;
                    std::vector<fbxsdk::FbxCluster*>   joints;
                    std::vector<fbxsdk::FbxAMatrix>    joints_matrices;

                    int skinCount = mesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin);

                    for (int skinIndex = 0; skinIndex < skinCount; skinIndex++)
                    {
                        FbxSkin* skin = (FbxSkin *)mesh->GetDeformer(skinIndex, fbxsdk::FbxDeformer::eSkin);
                        fbxsdk::FbxCluster::ELinkMode clusterMode0 = skin->GetCluster(0)->GetLinkMode();

                        int jointsCount = skin->GetClusterCount();

                        joints.resize(jointsCount);

                        for (int jointIndex = 0; jointIndex < jointsCount; jointIndex++)
                        {
                            fbxsdk::FbxCluster* joint = skin->GetCluster(jointIndex);

                            fbxsdk::FbxCluster::ELinkMode clusterMode = joint->GetLinkMode();
                            assert(clusterMode == clusterMode0);// "Different cluster modes in different joints";
                            joints[jointIndex] = joint;
                        }

                        assert((clusterMode0 == fbxsdk::FbxCluster::eNormalize || clusterMode0 == fbxsdk::FbxCluster::eTotalOne));// , "Unsupported cluster mode");
                    }

                    geo::skeleton_pose skeleton;

                    skeleton.m_joint_local_pose.resize(joints.size());

                    auto&& sjoints = skeleton.m_joint_local_pose;

                    for (auto&& i = 0U; i < joints.size(); ++i)
                    {
                        fbxsdk::FbxAMatrix bind_pose;
                        joints[i]->GetTransformLinkMatrix(bind_pose);

                        sjoints[i].m_transform          = joint_transform(bind_pose);
                        sjoints[i].m_transform_matrix   = joint_transform_matrix(bind_pose);
                    }

                   // sjoints[i].m_name = joints[i]->GetLink()->GetName();

                    //__debugbreak();
                }


                //////////////////////
                inline geo::skinned_mesh::blend_weights_t get_blend_weights(const fbxsdk::FbxMesh* mesh, const std::vector<int32_t>& triangle_indices)
                {
                    std::vector<joint_influence> influences;
                    influences.resize(mesh->GetControlPointsCount());

                    int skinCount = mesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin);
                    for (int skinIndex = 0; skinIndex < skinCount; skinIndex++)
                    {
                        FbxSkin* skin = (FbxSkin *)mesh->GetDeformer(skinIndex, fbxsdk::FbxDeformer::eSkin);
                        fbxsdk::FbxCluster::ELinkMode clusterMode0 = skin->GetCluster(0)->GetLinkMode();

                        int jointsCount = skin->GetClusterCount();
                        for (int jointIndex = 0; jointIndex < jointsCount; jointIndex++)
                        {
                            fbxsdk::FbxCluster* joint = skin->GetCluster(jointIndex);

                            fbxsdk::FbxCluster::ELinkMode clusterMode = joint->GetLinkMode();
                            assert(clusterMode == clusterMode0);// "Different cluster modes in different joints";

                            int influencedCount = joint->GetControlPointIndicesCount();

                            int* influenceIndices = joint->GetControlPointIndices();
                            double* influenceWeights = joint->GetControlPointWeights();

                            for (int influenceIndex = 0; influenceIndex < influencedCount; influenceIndex++)
                            {
                                int controlPointIndex = influenceIndices[influenceIndex];
                                assert(controlPointIndex < (int)influences.size());//"Invalid skin control point index"
                                influences[controlPointIndex].m_index.push_back(jointIndex);
                                influences[controlPointIndex].m_weight.push_back((float)influenceWeights[influenceIndex]);
                            }
                        }

                        assert((clusterMode0 == fbxsdk::FbxCluster::eNormalize || clusterMode0 == fbxsdk::FbxCluster::eTotalOne));// , "Unsupported cluster mode");
                    }

                    auto indices = mesh->GetPolygonVertices();
                    geo::skinned_mesh::blend_weights_t blend_weights;
                    for (auto triangle = 0; triangle < triangle_indices.size(); ++triangle)
                    {
                        auto triange_to_fetch = triangle_indices[triangle];
                        auto i0 = indices[triange_to_fetch * 3];
                        auto i1 = indices[triange_to_fetch * 3 + 1];
                        auto i2 = indices[triange_to_fetch * 3 + 2];

                        auto w0 = influences[i0];
                        auto w1 = influences[i1];
                        auto w2 = influences[i2];

                        auto wp0 = get_blend_weight(w0.m_weight);
                        auto wp1 = get_blend_weight(w1.m_weight);
                        auto wp2 = get_blend_weight(w2.m_weight);

                        blend_weights.push_back(wp0);
                        blend_weights.push_back(wp1);
                        blend_weights.push_back(wp2);
                    }

                    return blend_weights;
                }
                //////////////////////
                inline geo::skinned_mesh::blend_indices_t get_blend_indices(const fbxsdk::FbxMesh* mesh, const std::vector<int32_t>& triangle_indices)
                {
                    std::vector<joint_influence> influences;
                    influences.resize(mesh->GetControlPointsCount());

                    int skinCount = mesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin);
                    for (int skinIndex = 0; skinIndex < skinCount; skinIndex++)
                    {
                        FbxSkin* skin = (FbxSkin *)mesh->GetDeformer(skinIndex, fbxsdk::FbxDeformer::eSkin);
                        fbxsdk::FbxCluster::ELinkMode clusterMode0 = skin->GetCluster(0)->GetLinkMode();

                        int jointsCount = skin->GetClusterCount();
                        for (int jointIndex = 0; jointIndex < jointsCount; jointIndex++)
                        {
                            fbxsdk::FbxCluster* joint = skin->GetCluster(jointIndex);

                            fbxsdk::FbxCluster::ELinkMode clusterMode = joint->GetLinkMode();
                            assert(clusterMode == clusterMode0);// "Different cluster modes in different joints";

                            FbxAMatrix lMatrix;
                            lMatrix = joint->GetTransformMatrix(lMatrix);

                            int influencedCount = joint->GetControlPointIndicesCount();

                            int* influenceIndices = joint->GetControlPointIndices();
                            double* influenceWeights = joint->GetControlPointWeights();

                            for (int influenceIndex = 0; influenceIndex < influencedCount; influenceIndex++)
                            {
                                int controlPointIndex = influenceIndices[influenceIndex];
                                assert(controlPointIndex < (int)influences.size());//"Invalid skin control point index"
                                influences[controlPointIndex].m_index.push_back(jointIndex);
                                influences[controlPointIndex].m_weight.push_back((float)influenceWeights[influenceIndex]);
                            }
                        }

                        assert((clusterMode0 == fbxsdk::FbxCluster::eNormalize || clusterMode0 == fbxsdk::FbxCluster::eTotalOne));// , "Unsupported cluster mode");
                    }

                    auto indices = mesh->GetPolygonVertices();
                    geo::skinned_mesh::blend_indices_t blend_indices;
                    for (auto triangle = 0; triangle < triangle_indices.size(); ++triangle)
                    {
                        auto triange_to_fetch = triangle_indices[triangle];
                        auto i0 = indices[triange_to_fetch * 3];
                        auto i1 = indices[triange_to_fetch * 3 + 1];
                        auto i2 = indices[triange_to_fetch * 3 + 2];

                        auto w0 = influences[i0];
                        auto w1 = influences[i1];
                        auto w2 = influences[i2];

                        auto ip0 = get_blend_index(w0.m_index);
                        auto ip1 = get_blend_index(w1.m_index);
                        auto ip2 = get_blend_index(w2.m_index);

                        blend_indices.push_back(ip0);
                        blend_indices.push_back(ip1);
                        blend_indices.push_back(ip2);
                    }

                    return blend_indices;
                }

                //////////////////////
                inline std::shared_ptr<geo::skinned_mesh> create_skinned_mesh_internal(const fbxsdk::FbxMesh* mesh)
                {
                    const fbxsdk::FbxNode* mesh_node = mesh->GetNode();

                    //check
                    if (mesh == nullptr || !mesh->IsTriangleMesh())
                    {
                        ::uc::gx::throw_exception<uc::gx::fbx_exception>("file does not contain a triangle mesh");
                    }

                    assert(mesh_node);
                    assert(mesh->GetPolygonSize(0));


                    auto material_count = mesh->GetElementMaterialCount();

                    if (material_count == 0)
                    {
                        ::uc::gx::throw_exception<uc::gx::fbx_exception>("mesh does not have assigned materials");
                    }

                    std::vector< std::vector<int32_t> > materials_indices;
                    //assign polygons to materials
                    for (auto i = 0; i < material_count; ++i)
                    {
                        auto&& element_material = mesh->GetElementMaterial(i);
                        auto&& material_indices = element_material->GetIndexArray();
                        auto count = material_indices.GetCount();

                        if (element_material->GetMappingMode() == fbxsdk::FbxLayerElement::eAllSame)
                        {
                            auto polygon_count = mesh->GetPolygonCount();
                            auto material_index_for_polygon = material_indices.GetAt(0);

                            if (material_index_for_polygon + 1 > materials_indices.size())
                            {
                                materials_indices.resize(material_index_for_polygon + 1);
                            }

                            materials_indices[material_index_for_polygon].reserve(polygon_count);
                            for (auto k = 0; k < polygon_count; ++k)
                            {
                                materials_indices[material_index_for_polygon].push_back(k);
                            }
                        }
                        else
                        {
                            for (auto j = 0; j < count; ++j)
                            {
                                auto material_index_for_polygon = material_indices.GetAt(j);
                                if (material_index_for_polygon + 1 > materials_indices.size())
                                {
                                    materials_indices.resize(material_index_for_polygon + 1);
                                }

                                materials_indices[material_index_for_polygon].push_back(j);
                            }
                        }
                    }

                    std::vector<geo::skinned_mesh::positions_t> positions;                  //positions used by every material
                    std::vector<geo::skinned_mesh::uvs_t>       uvs;                        //uvs used by every material
                    std::vector<geo::skinned_mesh::blend_weights_t>   blend_weights;        //blend_weights used by every material
                    std::vector<geo::skinned_mesh::blend_indices_t>   blend_indices;        //blend_indices used by every material

                    //get_positions
                    positions.resize(materials_indices.size());
                    uvs.resize(materials_indices.size());
                    blend_weights.resize(materials_indices.size());
                    blend_indices.resize(materials_indices.size());

                    for (auto i = 0U; i < materials_indices.size(); ++i)
                    {
                        positions[i] = get_positions(mesh, materials_indices[i]);
                        uvs[i] = get_uvs(mesh, materials_indices[i]);
                        blend_weights[i] = get_blend_weights(mesh, materials_indices[i]);
                        blend_indices[i] = get_blend_indices(mesh, materials_indices[i]);
                    }

                    //reindex faces, these are indices in the separated positions and uvs
                    std::vector<geo::skinned_mesh::faces_t>  faces; //uvs used by every material
                    faces.resize(materials_indices.size());

                    for (auto i = 0; i < faces.size(); ++i)
                    {
                        //reorient triangles ccw, since they come cw from fbx
                        for (auto j = 0; j < materials_indices[i].size(); ++j)
                        {
                            auto triangle = j;
                            geo::skinned_mesh::face_t face;
                            face.v0 = triangle * 3;
                            face.v1 = triangle * 3 + 2;
                            face.v2 = triangle * 3 + 1;

                            faces[i].push_back(face);
                        }
                    }

                    geo::skinned_mesh::skeleton_pose_t pose;

                    get_skeleton_pose(mesh);

                    return std::make_shared<geo::skinned_mesh>(

                        std::move(positions), 
                        std::move(uvs),
                        std::move(faces),
                        get_materials(mesh_node, materials_indices.size()),
                        std::move(blend_weights),
                        std::move(blend_indices),
                        std::move(pose)
                        );
                }

                //////////////////////
                inline std::shared_ptr<geo::skinned_mesh> create_skinned_mesh(const std::string& file_name)
                {
                    std::unique_ptr<FbxManager, fbxmanager_deleter>     manager(FbxManager::Create(), fbxmanager_deleter());
                    std::unique_ptr<FbxScene, fbxscene_deleter>         scene(FbxScene::Create(manager.get(), ""), fbxscene_deleter());
                    std::unique_ptr<FbxImporter, fbximporter_deleter>   importer(FbxImporter::Create(manager.get(), ""), fbximporter_deleter());

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

                    FbxAxisSystem scene_axis_system = scene->GetGlobalSettings().GetAxisSystem();
                    FbxAxisSystem our_axis_system = FbxAxisSystem(FbxAxisSystem::EPreDefinedAxisSystem::eDirectX);

                    if (scene_axis_system != our_axis_system)
                    {
                        our_axis_system.ConvertScene(scene.get());
                    }

                    FbxSystemUnit units = scene->GetGlobalSettings().GetSystemUnit();
                    FbxSystemUnit meters = FbxSystemUnit::m;

                    if (units != FbxSystemUnit::m)
                    {
                        //FbxSystemUnit::m.ConvertScene(scene.get());
                    }

                    std::vector<fbxsdk::FbxMesh*> meshes;
                    meshes = get_meshes(scene->GetRootNode(), meshes);

                    for (auto& m : meshes)
                    {
                        m->RemoveBadPolygons();
                        m->ComputeBBox();
                    }

                    std::vector<  std::shared_ptr<geo::skinned_mesh> > multimeshes;
                    for (auto& m : meshes)
                    {
                        multimeshes.push_back(create_skinned_mesh_internal(m));
                    }

                    //merge all multimaterial meshes into one
                    std::vector< geo::skinned_mesh::positions_t > pos;
                    std::vector< geo::skinned_mesh::uvs_t >       uv;
                    std::vector< geo::skinned_mesh::faces_t >     faces;
                    std::vector< geo::skinned_mesh::material >    mat;
                    std::vector< geo::skinned_mesh::blend_weights_t >    blend_weights;
                    std::vector< geo::skinned_mesh::blend_indices_t >    blend_indices;

                    for (auto&& m : multimeshes)
                    {
                        pos.insert(pos.end(), m->m_positions.begin(), m->m_positions.end());
                        uv.insert(uv.end(), m->m_uv.begin(), m->m_uv.end());
                        faces.insert(faces.end(), m->m_faces.begin(), m->m_faces.end());
                        mat.insert(mat.end(), m->m_materials.begin(), m->m_materials.end());
                        blend_weights.insert(blend_weights.end(), m->m_blend_weights.begin(), m->m_blend_weights.end());
                        blend_indices.insert(blend_indices.end(), m->m_blend_indices.begin(), m->m_blend_indices.end());
                    }

                    geo::skinned_mesh::skeleton_pose_t pose;

                    return std::make_shared<geo::skinned_mesh>(
                        std::move(pos), 
                        std::move(uv),
                        std::move(faces),
                        std::move(mat),
                        std::move(blend_weights),
                        std::move(blend_indices),
                        std::move(pose));
                }
            }
        }
    }
}

