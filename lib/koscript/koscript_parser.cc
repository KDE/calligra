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

#include <iostream>

#include <qstring.h>

#include <klocale.h>

#include "koscript_parser.h"
#include "koscript_parsenode.h"

extern FILE* yyin;
extern char* yytext;
extern int yylex();

KSParser *theParser;

QString idl_lexFile;
QString toplevelFile;
int idl_line_no;

// Imported from yacc.yy
extern void kscriptParse( const char *_code, int extension, KLocale* locale );
extern void kscriptParse( int extension, KLocale* locale );

KSParser::KSParser()
{
  rootNode = 0;
}

KSParser::~KSParser()
{
  if ( rootNode )
    delete rootNode;
}

bool KSParser::parse( FILE *inp_file, const char *filename, int extension, KLocale* locale )
{
  idl_lexFile = toplevelFile = const_cast<char *>(filename);
  yyin = inp_file;

  m_errorMessage = "";
  theParser = this;
  idl_line_no = 1;
  kscriptParse( extension, locale );

  return m_errorMessage.isEmpty();
}

bool KSParser::parse( const char* code, int extension, KLocale* locale )
{
  m_errorMessage = "";
  theParser = this;
  idl_line_no = 1;
  kscriptParse( code, extension, locale );

  return m_errorMessage.isEmpty();
}

void KSParser::setRootNode( KSParseNode *node )
{
  rootNode = node;
}


KSParseNode *KSParser::getRootNode()
{
  return rootNode;
}


void KSParser::parse_error( const char *file, const char *err, int line )
{
  m_errorMessage = "%1:%2: %3 before '%4'";
  m_errorMessage = m_errorMessage.arg( file ).arg( line ).arg( err ).arg( yytext );
}

void KSParser::print( bool detailed )
{
  if ( rootNode )
    rootNode->print( detailed );
}

bool KSParser::eval( KSContext& context )
{
  if ( !rootNode )
    return FALSE;

  return rootNode->eval( context );
}

KSParseNode* KSParser::donateParseTree()
{
  KSParseNode* n = rootNode;
  rootNode = 0;
  return n;
}
