#pragma once

#include <boost/program_options.hpp>

//#include "uc_model_exception.h"

namespace uc
{
    namespace model
    {
        inline auto build_option_description()
        {
            namespace po = boost::program_options;

            // Declare the supported options.
            po::options_description desc("uc model compiler. packs 3d models into a format suitable for faster loading");

            desc.add_options()

                ("help,?", "produce help message")

                ("input_model,i", po::value< std::string>(), "input 3d model")
                ("output_model,o", po::value< std::string>(), "output 3d model")
                ("type,t", po::value< std::string>(), "model type ( default, parametrized, textured, multi_textured, skinned )")
                ("swap_faces", po::value< bool >(), "swap front face and back faces")

                ("textures", po::value< std::vector<std::string>>(), "textures for 3d model")

                ("textures_formats", po::value< std::vector<std::string> >(), "texture formats for 3d models (unknown, bc1_unorm, bc1_unorm_srgb, bc2_unorm, bc2_unorm_srgb, bc3_unorm, bc3_unorm_srgb, bc4_unorm, bc4_snorm, bc5_unorm, bc5_snorm)")
                ("swap_y_z", po::value< bool >(), "swaps y and z coordinates, suitable for some tools like 3ds max")


              ;
                
            return desc;
        }

        inline auto build_option_map(const boost::program_options::options_description& desc, int32_t argc, const char* argv[])
        {
            namespace po = boost::program_options;

            po::variables_map vm;

            po::store(po::command_line_parser(argc, argv)
                .options(desc)
                .style(po::command_line_style::default_style
                    | po::command_line_style::case_insensitive
                    | po::command_line_style::allow_slash_for_short
                    | po::command_line_style::allow_long_disguise)
                .run(), vm);

            po::notify(vm);
            return make_tuple(vm, desc);
        }

        inline auto build_option_map(int32_t argc, const char* argv[])
        {
            return build_option_map(build_option_description(), argc, argv);
        }

        template <typename t, typename e >
        inline auto get_input_value(const boost::program_options::variables_map & map, const std::string& s, e e)
        {
            using namespace std;
            if (!map.count(s))
            {
                e();
                return t();
            }
            else
            {
                return map[s].as< t >();
            }
        }

        inline auto get_value_present(const boost::program_options::variables_map & map, const std::string& s)
        {
            using namespace std;
            return map.count(s);
        }

        /*
        inline auto get_input_model(const boost::program_options::variables_map & map)
        {
            return std::string( get_input_value< std::string > (map, "input_model", [ ]
            {
                raise_error<missing_input_exception>(missing_input_exception::missing_part::input_file_name);
            }));
        }

        inline auto get_output_model(const boost::program_options::variables_map & map)
        {
            return std::string(get_input_value< std::string >(map, "output_model", []
            {
                raise_error<missing_input_exception>(missing_input_exception::missing_part::output_file_name);
            }));
        }

        inline auto get_model_type(const boost::program_options::variables_map & map)
        {
            auto r = std::string("default");
            if (get_value_present(map, "type"))
            {
                r = get_input_value<std::string>(map, "type", [] {});
            }
            return r;
        }

        inline auto get_textures(const boost::program_options::variables_map & map)
        {
            using ret_type = std::vector< std::string >;
            return ret_type(get_input_value< ret_type >(map, "textures", []
            {
                raise_error<missing_input_exception>(missing_input_exception::missing_part::textures);
            }));
        }

        inline auto get_texture_formats(const boost::program_options::variables_map & map)
        {
            using ret_type = std::vector< std::string >;
            return ret_type(get_input_value< ret_type >(map, "textures_formats", []
            {
                raise_error<missing_input_exception>(missing_input_exception::missing_part::textures_formats);
            }));
        }

        inline auto get_bool_option(const boost::program_options::variables_map & map, const std::string& o)
        {
            auto r = false;
            if (get_value_present(map, o))
            {
                r = get_input_value<bool>(map, o, [] {});
            }

            return r;
        }

        inline auto get_swap_y_z(const boost::program_options::variables_map & map)
        {
            return get_bool_option(map, "swap_y_z");
        }

        inline auto get_swap_faces(const boost::program_options::variables_map & map)
        {
            return get_bool_option(map, "swap_faces");
        }

        inline uint64_t get_assimp_option(const boost::program_options::variables_map & map)
        {
            const std::array<std::string, 27> o=
            {
                "calc_tangent_space",
                "join_identical_vertices",
                "make_left_handed",
                "triangulate",
                "remove_component",
                "gen_normals",
                "gen_smooth_normals",
                "split_large_meshes",
                "pre_transform_vertices",
                "limit_bone_weights",
                "validate_data_structure",
                "improve_cache_locality",
                "remove_redundant_materials",
                "fix_infacing_normals",
                "missed_by_assimp_dummy_enum",
                "sort_by_ptype",
                "find_degenerates",
                "find_invalid_data",
                "gen_uv_coords",
                "transform_uv_coords",
                "find_instances",
                "optimize_meshes",
                "optimize_graph",
                "flip_uvs",
                "flip_winding_order",
                "split_by_bone_count",
                "debone"
            };

            uint64_t r = 0;
            for (auto i = 0U; i < o.size(); ++i)
            {
                r |= get_bool_option(map, o[i]) ? ( 1 << (i) ) : 0;

            }

            return r;
        }
        */

    }
}

