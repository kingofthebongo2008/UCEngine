#pragma once

#include <uc_pso_ast.h>
#include "uc_pso_ast_adapted.h"

#include <parser/uc_pso_parser_includes.h>

#include "uc_pso_parser_render_target_blend_desc.h"


namespace uc
{
    namespace pso
    {
        namespace parser
        {
            template <typename iterator, typename error_handler>
            struct blend_desc : public qi::grammar< iterator, ast::blend_desc(), ascii::space_type  >
            {
                using base = qi::grammar< iterator, ast::blend_desc(), ascii::space_type >;

                blend_desc(error_handler& e) : base(m_start, "blend_desc")
                , m_elements(e)
                , m_alpha_to_coverage_enable(".AlphaToCoverageEnable")
                , m_independent_blend_enable(".IndependentBlendEnable")
                {
                    using qi::lit;
                    //qi::raw_type raw;
                    //qi::lexeme_type lexeme;
                    //qi::alpha_type alpha;
                    //qi::alnum_type alnum;

                    m_identifier = raw[lexeme[(alpha | '_') > *(alnum | '_')]];
                    m_start = lit("BlendDescription") > m_identifier > '=' > '{' 
                    > m_alpha_to_coverage_enable
                    > m_independent_blend_enable
                    > +m_elements 
                    > '}';
                    m_identifier.name("identifier");

                    {

                        using boost::phoenix::function;
                        using error_handler_function = function < error_handler >;
                        using annotation_function = function < annotation<iterator> >;

                        qi::_1_type     _1;
                        //qi::_3_type     _3;
                        qi::_val_type   _val;

                        using qi::on_error;
                        using qi::on_success;
                        using qi::fail;
                        
                        //on_error<fail>(m_start, error_handler_function(e)(_3));
                        auto success = annotation_function(annotation<iterator>(e.m_iters, e.m_first));
                        on_success(m_start, success(_val, _1));
                    }
                }

                named_value_bool<iterator>                                   m_alpha_to_coverage_enable;
                named_value_bool<iterator>                                   m_independent_blend_enable;
                render_target_blend_desc<iterator, error_handler>            m_elements;
                qi::rule<iterator, std::string(), ascii::space_type >        m_identifier;
                qi::rule<iterator, ast::blend_desc(), ascii::space_type >  m_start;
            };
        }
    }
}