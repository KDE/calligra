#ifndef __KSCRIPT_PARSER_H__
#define __KSCRIPT_PARSER_H__

#include <stdio.h>
#include <qstring.h>

class KSParser;
class KSParseNode;
class KSContext;
class KLocale;

extern KSParser *theParser;  // Defined in parser.cc
extern int yyparse();      // Defined through yacc.y

class KSParser
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
