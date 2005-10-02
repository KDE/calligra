/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __KSCRIPT_EVAL_H__
#define __KSCRIPT_EVAL_H__

class KSContext;
class KSParseNode;

extern bool KSEval_definitions( KSParseNode* node, KSContext& context );
extern bool KSEval_exports( KSParseNode* node, KSContext& context );
extern bool KSEval_t_in_param_dcl( KSParseNode* node, KSContext& context );
extern bool KSEval_t_out_param_dcl( KSParseNode* node, KSContext& context );
extern bool KSEval_t_inout_param_dcl( KSParseNode* node, KSContext& context );
extern bool KSEval_t_float( KSParseNode* node, KSContext& context );
extern bool KSEval_param_dcls( KSParseNode* node, KSContext& context );
extern bool KSEval_t_vertical_line( KSParseNode* node, KSContext& context );
extern bool KSEval_t_circumflex( KSParseNode* node, KSContext& context );
extern bool KSEval_t_ampersand( KSParseNode* node, KSContext& context );
extern bool KSEval_t_shiftright( KSParseNode* node, KSContext& context );
extern bool KSEval_t_shiftleft( KSParseNode* node, KSContext& context );
extern bool KSEval_t_plus_sign( KSParseNode* node, KSContext& context );
extern bool KSEval_t_minus_sign( KSParseNode* node, KSContext& context );
extern bool KSEval_t_asterik( KSParseNode* node, KSContext& context );
extern bool KSEval_t_solidus( KSParseNode* node, KSContext& context );
extern bool KSEval_t_percent_sign( KSParseNode* node, KSContext& context );
extern bool KSEval_t_tilde( KSParseNode* node, KSContext& context );
extern bool KSEval_t_integer_literal( KSParseNode* node, KSContext& context );
extern bool KSEval_t_string_literal( KSParseNode* node, KSContext& context );
extern bool KSEval_t_character_literal( KSParseNode* node, KSContext& context );
extern bool KSEval_t_floating_pt_literal( KSParseNode* node, KSContext& context );
extern bool KSEval_t_boolean_literal( KSParseNode* node, KSContext& context );
extern bool KSEval_scoped_name( KSParseNode* node, KSContext& context );
extern bool KSEval_raises_expr( KSParseNode* node, KSContext& context );
extern bool KSEval_const_dcl( KSParseNode* node, KSContext& context );
extern bool KSEval_func_dcl( KSParseNode* node, KSContext& context );
extern bool KSEval_func_lines( KSParseNode* node, KSContext& context );
extern bool KSEval_assign_expr( KSParseNode* node, KSContext& context );
extern bool KSEval_t_equal( KSParseNode* node, KSContext& context );
extern bool KSEval_t_notequal( KSParseNode* node, KSContext& context );
extern bool KSEval_t_less_or_equal( KSParseNode* node, KSContext& context );
extern bool KSEval_t_greater_or_equal( KSParseNode* node, KSContext& context );
extern bool KSEval_t_array( KSParseNode* node, KSContext& context );
extern bool KSEval_t_dict( KSParseNode* node, KSContext& context );
extern bool KSEval_func_params( KSParseNode* node, KSContext& context );
extern bool KSEval_func_param_in( KSParseNode* node, KSContext& context );
extern bool KSEval_func_param_out( KSParseNode* node, KSContext& context );
extern bool KSEval_func_param_inout( KSParseNode* node, KSContext& context );
extern bool KSEval_t_func_call( KSParseNode* node, KSContext& context );
extern bool KSEval_member_expr( KSParseNode* node, KSContext& context );
extern bool KSEval_t_array_const( KSParseNode* node, KSContext& context );
extern bool KSEval_t_array_element( KSParseNode* node, KSContext& context );
extern bool KSEval_t_dict_const( KSParseNode* node, KSContext& context );
extern bool KSEval_t_dict_element( KSParseNode* node, KSContext& context );
extern bool KSEval_t_while( KSParseNode* node, KSContext& context );
extern bool KSEval_t_do( KSParseNode* node, KSContext& context );
extern bool KSEval_t_for( KSParseNode* node, KSContext& context );
extern bool KSEval_t_if( KSParseNode* node, KSContext& context );
extern bool KSEval_t_incr( KSParseNode* node, KSContext& context );
extern bool KSEval_t_decr( KSParseNode* node, KSContext& context );
extern bool KSEval_t_less( KSParseNode* node, KSContext& context );
extern bool KSEval_t_greater( KSParseNode* node, KSContext& context );
extern bool KSEval_t_foreach( KSParseNode* node, KSContext& context );
extern bool KSEval_t_match( KSParseNode* node, KSContext& context );
extern bool KSEval_t_subst( KSParseNode* node, KSContext& context );
extern bool KSEval_t_not( KSParseNode* node, KSContext& context );
extern bool KSEval_func_call_params( KSParseNode* node, KSContext& context );
extern bool KSEval_t_return( KSParseNode* node, KSContext& context );
extern bool KSEval_destructor_dcl( KSParseNode* node, KSContext& context );
extern bool KSEval_import( KSParseNode* node, KSContext& context );
extern bool KSEval_t_struct( KSParseNode* node, KSContext& context );
extern bool KSEval_t_struct_members( KSParseNode* node, KSContext& context );
extern bool KSEval_t_qualified_names( KSParseNode* node, KSContext& context );
extern bool KSEval_t_scope( KSParseNode* node, KSContext& context );
extern bool KSEval_t_try( KSParseNode* node, KSContext& context );
extern bool KSEval_t_catch( KSParseNode* node, KSContext& context );
extern bool KSEval_t_catch_default( KSParseNode* node, KSContext& context );
extern bool KSEval_t_raise( KSParseNode* node, KSContext& context );
extern bool KSEval_t_cell( KSParseNode* node, KSContext& context );
extern bool KSEval_t_range( KSParseNode* node, KSContext& context );
extern bool KSEval_from( KSParseNode* node, KSContext& context );
extern bool KSEval_plus_assign( KSParseNode* node, KSContext& context );
extern bool KSEval_minus_assign( KSParseNode* node, KSContext& context );
extern bool KSEval_bool_or( KSParseNode* node, KSContext& context );
extern bool KSEval_bool_and( KSParseNode* node, KSContext& context );
extern bool KSEval_t_regexp_group( KSParseNode* node, KSContext& context );
extern bool KSEval_t_input( KSParseNode* node, KSContext& context );
extern bool KSEval_t_line( KSParseNode* node, KSContext& context );
extern bool KSEval_t_match_line( KSParseNode* node, KSContext& context );

#endif
