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

#include "koscript_parsenode.h"
#include "koscript_eval.h"
#include "koscript_context.h"

#include <iostream>
using namespace std;

extern QString idl_lexFile;
extern QString toplevelFile;
extern int idl_line_no;

typedef struct
{
  const char *str;
  int   data;     // 0 = no further information
                  // 1 = ident
} ParseNodeDoc;


static ParseNodeDoc parseNodeDoc[] = {
  { "definitions", 0 },
  { "exports", 0 },
  { "t_vertical_line", 0 },
  { "t_circumflex", 0 },
  { "t_ampersand", 0 },
  { "t_shiftright", 0 },
  { "t_shiftleft", 0 },
  { "t_plus_sign", 0 },
  { "t_minus_sign", 0 },
  { "t_asterik", 0 },
  { "t_solidus", 0 },
  { "t_percent_sign", 0 },
  { "t_tilde", 0 },
  { "t_integer_literal", 2 },
  { "t_string_literal", 3 },
  { "t_character_literal", 4 },
  { "t_floating_pt_literal", 5 },
  { "t_boolean_literal", 6 },
  { "scoped_name", 1 },
  { "const_dcl", 1 },
  { "func_dcl", 1 },
  { "func_lines", 0 },
  { "assign_expr", 0 },
  { "t_equal", 0 },
  { "t_notequal", 0 },
  { "t_less_or_equal", 0 },
  { "t_greater_or_equal", 0 },
  { "t_array", 0 },
  { "t_dict", 0 },
  { "func_params", 0 },
  { "func_param_in", 1 },
  { "func_param_out", 1 },
  { "func_param_inout", 1 },
  { "t_func_call", 0 },
  { "member_expr", 1 },
  { "t_array_const", 0 },
  { "t_array_element", 0 },
  { "t_dict_const", 0 },
  { "t_dict_element", 0 },
  { "t_while", 0 },
  { "t_do", 0 },
  { "t_for", 0 },
  { "t_if", 0 },
  { "t_incr", 0 },
  { "t_decr", 0 },
  { "t_less", 0 },
  { "t_greater", 0 },
  { "t_foreach", 0 },
  { "t_match", 1 },
  { "t_subst", 1 },
  { "t_not", 0 },
  { "func_call_params", 0 },
  { "t_return", 0 },
  { "destructor_dcl", 0 },
  { "import", 1 },
  { "t_struct", 1 },
  { "t_strtuct_members", 1 },
  { "t_qualified_names", 0 },
  { "t_scope", 0 },
  { "t_try", 0 },
  { "t_catch", 0 },
  { "t_catch_default", 0 },
  { "t_raise", 0 },
  { "t_cell", 0 },
  { "t_range", 0 },
  { "from", 0 },
  { "plus_assign", 0 },
  { "minus_assign", 0 },
  { "bool_or", 0 },
  { "bool_and", 0 },
  { "t_regexp_group", 0 },
  { "t_input", 0 },
  { "t_line", 0 },
  { "t_match_line", 0 }
};

typedef bool (*KSEval)( KSParseNode*, KSContext& );

static KSEval KSEvalJump[] = {
  KSEval_definitions,
  KSEval_exports,
  KSEval_t_vertical_line,
  KSEval_t_circumflex,
  KSEval_t_ampersand,
  KSEval_t_shiftright,
  KSEval_t_shiftleft,
  KSEval_t_plus_sign,
  KSEval_t_minus_sign,
  KSEval_t_asterik,
  KSEval_t_solidus,
  KSEval_t_percent_sign,
  KSEval_t_tilde,
  KSEval_t_integer_literal,
  KSEval_t_string_literal,
  KSEval_t_character_literal,
  KSEval_t_floating_pt_literal,
  KSEval_t_boolean_literal,
  KSEval_scoped_name,
  KSEval_const_dcl,
  KSEval_func_dcl,
  KSEval_func_lines,
  KSEval_assign_expr,
  KSEval_t_equal,
  KSEval_t_notequal,
  KSEval_t_less_or_equal,
  KSEval_t_greater_or_equal,
  KSEval_t_array,
  KSEval_t_dict,
  KSEval_func_params,
  KSEval_func_param_in,
  KSEval_func_param_out,
  KSEval_func_param_inout,
  KSEval_t_func_call,
  KSEval_member_expr,
  KSEval_t_array_const,
  KSEval_t_array_element,
  KSEval_t_dict_const,
  KSEval_t_dict_element,
  KSEval_t_while,
  KSEval_t_do,
  KSEval_t_for,
  KSEval_t_if,
  KSEval_t_incr,
  KSEval_t_decr,
  KSEval_t_less,
  KSEval_t_greater,
  KSEval_t_foreach,
  KSEval_t_match,
  KSEval_t_subst,
  KSEval_t_not,
  KSEval_func_call_params,
  KSEval_t_return,
  KSEval_destructor_dcl,
  KSEval_import,
  KSEval_t_struct,
  KSEval_t_struct_members,
  KSEval_t_qualified_names,
  KSEval_t_scope,
  KSEval_t_try,
  KSEval_t_catch,
  KSEval_t_catch_default,
  KSEval_t_raise,
  KSEval_t_cell,
  KSEval_t_range,
  KSEval_from,
  KSEval_plus_assign,
  KSEval_minus_assign,
  KSEval_bool_or,
  KSEval_bool_and,
  KSEval_t_regexp_group,
  KSEval_t_input,
  KSEval_t_line,
  KSEval_t_match_line,
};

KSParseNode::KSParseNode( KSParseNodeType aType, KSParseNode *one,
                      KSParseNode *two, KSParseNode *three,
                      KSParseNode *four, KSParseNode *five )
{
  m_extra = 0;
  str = 0;
  type = aType;
  ident = QString::null;
  b1 = one;
  b2 = two;
  b3 = three;
  b4 = four;
  b5 = five;
  fname = idl_lexFile;
  line_no = idl_line_no;
  bIsToplevel = ( idl_lexFile == toplevelFile );
}

KSParseNode::~KSParseNode()
{
  clear();
}

void KSParseNode::clear()
{
  if ( m_extra ) delete m_extra;
  m_extra = 0;
  if ( str ) delete str;
  str = 0;
  if( b1 ) delete b1;
  b1 = 0;
  // t_incr / t_decr checks as b1==b2 is used to detect postfix notation
  // This fixes the *ugly* memory corruption (double deletion)
  if( b2 && type!=t_incr && type!=t_decr ) delete b2;
  b2 = 0;
  if( b3 ) delete b3;
  b3 = 0;
  if( b4 ) delete b4;
  b4 = 0;
  if( b5 ) delete b5;
  b5 = 0;
}

void KSParseNode::setIdent( const QString& anIdent )
{
  ident = anIdent;
}

void KSParseNode::setIdent( QString* anIdent )
{
  ident = *anIdent;
  delete anIdent;
}

void KSParseNode::setIdent( const char *anIdent )
{
  ident = anIdent;
}

QString KSParseNode::getIdent()
{
  return ident;
}

QString KSParseNode::getFname()
{
  return fname;
}

long KSParseNode::getLineNo()
{
  return line_no;
}

bool KSParseNode::isToplevel()
{
  return bIsToplevel;
}

void KSParseNode::setIntegerLiteral( KScript::Long l )
{
  _u._int = l;
}

KScript::Long KSParseNode::getIntegerLiteral()
{
  return _u._int;
}

void KSParseNode::setStringLiteral( const char* s )
{
  str = new QString( s );
}

void KSParseNode::setStringLiteral( const QString& s )
{
  str = new QString( s );
}

void KSParseNode::setStringLiteral( QString* s )
{
  // We take ownership
  str = s;
}

QString KSParseNode::getStringLiteral()
{
  return *str;
}

void KSParseNode::setCharacterLiteral( const KScript::Char& c )
{
  _u._char = c.unicode();
}

KScript::Char KSParseNode::getCharacterLiteral()
{
  return QChar( _u._char );
}

void KSParseNode::setFloatingPtLiteral( KScript::Double f )
{
  _u._float = f;
}

KScript::Double KSParseNode::getFloatingPtLiteral()
{
  return _u._float;
}

void KSParseNode::setBooleanLiteral( KScript::Boolean b )
{
  _u._bool = b;
}

KScript::Boolean KSParseNode::getBooleanLiteral()
{
  return _u._bool;
}

KSParseNodeType KSParseNode::getType() const
{
  return type;
}


KSParseNode *KSParseNode::getBranch( int i )
{
  switch( i ) {
  case 1:
    return b1;
  case 2:
    return b2;
  case 3:
    return b3;
  case 4:
    return b4;
  case 5:
    return b5;
  default:
    Q_ASSERT( 0 );
  }
  return 0;
}


void KSParseNode::setBranch( int i, KSParseNode *node )
{
  switch( i ) {
  case 1:
    if( b1 ) delete b1;
    b1 = node;
    break;
  case 2:
    if( b2 ) delete b2;
    b2 = node;
    break;
  case 3:
    if( b3 ) delete b3;
    b3 = node;
    break;
  case 4:
    if( b4 ) delete b4;
    b4 = node;
    break;
  case 5:
    if( b5 ) delete b5;
    b5 = node;
    break;
  default:
    Q_ASSERT( 0 );
  }
}


void KSParseNode::printBranch( int indent, const char *tag, bool detailed )
{
  for( int i = 0; i < indent; i++ )
    cout << " ";
  cout << tag;
  cout << parseNodeDoc[ (int) type ].str;
  switch( parseNodeDoc[ (int) type ].data ) {
  case 0:
    break;
  case 1:
    cout << " (" << ident.local8Bit() << ")";
    break;
  case 2:
    cout << " (" << getIntegerLiteral() << ")";
    break;
  case 3:
    cout << " (" << getStringLiteral().local8Bit() << ")";
    break;
  case 4:
    cout << " (" << getCharacterLiteral() << ")";
    break;
  case 5:
    cout << " (" << getFloatingPtLiteral() << ")";
    break;
  case 6:
    cout << " (" << getBooleanLiteral() << ")";
    break;
  default:
    Q_ASSERT( 0 );
  }
  cout << endl;
  if( b1 )
    b1->printBranch( indent + 2, "1: ", detailed );
  if( b2 )
    b2->printBranch( indent + 2, "2: ", detailed );
  if( b3 )
    b3->printBranch( indent + 2, "3: ", detailed );
  if( b4 )
    b4->printBranch( indent + 2, "4: ", detailed );
  if( b5 )
    b5->printBranch( indent + 2, "5: ", detailed );
}

KSParseNodeExtra* KSParseNode::extra()
{
  return m_extra;
}

void KSParseNode::setExtra( KSParseNodeExtra* e )
{
  m_extra = e;
}

void KSParseNode::print( bool detailed )
{
  printBranch( 0, "", detailed );
}

bool KSParseNode::eval( KSContext& _context )
{
  return (KSEvalJump[ (int)type ])( this, _context );
}
