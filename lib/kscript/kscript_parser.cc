#include <iostream.h>

#include <qstring.h>

#include "kscript_parser.h"
#include "kscript_parsenode.h"

extern FILE* yyin;
extern char* yytext;
extern int yylex();

KSParser *theParser;

QString idl_lexFile;
QString toplevelFile;
int idl_line_no;

KSParser::KSParser( FILE *inp_file, const char *filename )
{
  rootNode = 0;

  idl_lexFile = toplevelFile = (char *) filename;
  yyin = inp_file;
}

KSParser::~KSParser()
{
  if ( rootNode )
    delete rootNode;
}

bool KSParser::parse()
{
  m_errorMessage = "";
  theParser = this;
  idl_line_no = 1;
  yyparse();

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
  m_errorMessage = m_errorMessage.arg( file ).arg( line).arg( err ).arg( yytext );
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
