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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __KSCRIPT_PARSENODE_H__
#define __KSCRIPT_PARSENODE_H__

#include <qstring.h>

#include "koscript_types.h"
#include <koffice_export.h>

#define MAX_NODE_SIZE 255

typedef enum
{
  definitions = 0,
  exports,
  t_vertical_line,
  t_circumflex,
  t_ampersand,
  t_shiftright,
  t_shiftleft,
  t_plus_sign,
  t_minus_sign,
  t_asterik,
  t_solidus,
  t_percent_sign,
  t_tilde,
  t_integer_literal,
  t_string_literal,
  t_character_literal,
  t_floating_pt_literal,
  t_boolean_literal,
  scoped_name,
  const_dcl,
  func_dcl,
  func_lines,
  assign_expr,
  t_equal,
  t_notequal,
  t_less_or_equal,
  t_greater_or_equal,
  t_array,
  t_dict,
  func_params,
  func_param_in,
  func_param_out,
  func_param_inout,
  t_func_call,
  member_expr,
  t_array_const,
  t_array_element,
  t_dict_const,
  t_dict_element,
  t_while,
  t_do,
  t_for,
  t_if,
  t_incr,
  t_decr,
  t_less,
  t_greater,
  t_foreach,
  t_match,
  t_subst,
  t_not,
  func_call_params,
  t_return,
  destructor_dcl,
  import,
  t_struct,
  t_struct_members,
  t_qualified_names,
  t_scope,
  t_try,
  t_catch,
  t_catch_default,
  t_raise,
  t_cell,
  t_range,
  from,
  plus_assign,
  minus_assign,
  bool_or,
  bool_and,
  t_regexp_group,
  t_input,
  t_line,
  t_match_line
} KSParseNodeType;

class KSContext;

/**
 * Derive from this class to store
 * extra data in some parsenode.
 */
class KSParseNodeExtra
{
public:
  virtual ~KSParseNodeExtra() { }
};

class KOSCRIPT_EXPORT KSParseNode
{
private:
  KSParseNode &operator=(const KSParseNode &rhs);
  KSParseNode(const KSParseNode &rhs);

  KSParseNodeType  type;
  QString ident;

  QString fname;
  long    line_no;
  bool    bIsToplevel;

  union u {
    KScript::Long    _int;
    KScript::Boolean _bool;
    KScript::Double  _float;
    ushort           _char;
  } _u;
  QString* str;

  KSParseNodeExtra* m_extra;
  KSParseNode *b1;
  KSParseNode *b2;
  KSParseNode *b3;
  KSParseNode *b4;
  KSParseNode *b5;

  void printBranch( int indent, const char *tag, bool detailed );

public:
  KSParseNode( KSParseNodeType aType, KSParseNode *one = NULL,
	     KSParseNode *two = NULL, KSParseNode *three = NULL,
	     KSParseNode *four = NULL, KSParseNode *five = NULL );
  ~KSParseNode();
  void clear();

  bool eval( KSContext& );

  KSParseNodeType getType() const;
  void setIdent( const char *anIdent );
  void setIdent( QString* anIdent );
  void setIdent( const QString& anIdent );
  QString getIdent();
  QString getFname();
  long getLineNo();
  bool isToplevel();

  void setIntegerLiteral( KScript::Long l );
  KScript::Long getIntegerLiteral();
  void setStringLiteral( const char *s );
  void setStringLiteral( const QString& s );
  void setStringLiteral( QString* s );
  QString getStringLiteral();
  void setCharacterLiteral( const KScript::Char& c );
  KScript::Char getCharacterLiteral();
  void setFloatingPtLiteral( KScript::Double f );
  KScript::Double getFloatingPtLiteral();
  void setBooleanLiteral( KScript::Boolean b );
  KScript::Boolean getBooleanLiteral();

  KSParseNode *branch1() { return b1; }
  KSParseNode *branch2() { return b2; }
  KSParseNode *branch3() { return b3; }
  KSParseNode *branch4() { return b4; }
  KSParseNode *branch5() { return b5; }
  KSParseNode *getBranch( int i );
  void setBranch( int i, KSParseNode *node );

  KSParseNodeExtra* extra();
  void setExtra( KSParseNodeExtra* e );

  void print( bool detailed = false );
};

#endif
