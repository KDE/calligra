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

#ifndef __KSCRIPT_PARSER_H__
#define __KSCRIPT_PARSER_H__

#include <stdio.h>
#include <qstring.h>
#include <koffice_export.h>

class KSParser;
class KSParseNode;
class KSContext;
class KLocale;

extern KSParser *theParser;  // Defined in koscript_parser.cc
extern int yyparse();      // Defined through yacc.y

class KOSCRIPT_EXPORT KSParser
{
public:
  KSParser();
  ~KSParser();

  bool eval( KSContext& );

  bool parse( FILE* inp_file, const char *filename = NULL, int extension = 0, KLocale* locale = 0 );
  bool parse( const char* code, int extension = 0, KLocale* locale = 0 );
  void setRootNode( KSParseNode *node );
  KSParseNode *getRootNode();
  void parse_error( const char *file, const char *err, int line );

  void print( bool detailed = false );

  KSParseNode* donateParseTree();

  QString errorMessage() const { return m_errorMessage; }

private:
  KSParseNode *rootNode;
  QString m_errorMessage;
};


#endif
