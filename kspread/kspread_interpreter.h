#ifndef __kspread_interpreter_h__
#define __kspread_interpreter_h__

#include <koscript.h>

#include "kspread_global.h"
#include "kspread_depend.h"

class KSParseNode;
class KSContext;

class KSpreadDoc;
class KSpreadTable;

class KSpreadInterpreter : public KSInterpreter
{
public:
  typedef KSSharedPtr<KSpreadInterpreter> Ptr;

  KSpreadInterpreter( KSpreadDoc* );

  KSParseNode* parse( KSContext& context, KSpreadTable* table, const QString& formula, QPtrList<KSpreadDependancy>& );
  bool evaluate( KSContext& context, KSParseNode*, KSpreadTable* );

  KSNamespace* globalNamespace() { return m_global; }

  virtual bool processExtension( KSContext& context, KSParseNode* node );

  KSpreadDoc* document() { return m_doc; }
  KSpreadTable* table() { return m_table; }

private:
  KSpreadDoc* m_doc;
  KSpreadTable* m_table;
};

#endif
