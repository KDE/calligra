#ifndef __kspread_interpreter_h__
#define __kspread_interpreter_h__

#include <koscript.h>

#include "kspread_global.h"

class KSParseNode;
class KSContext;

class KSpreadDoc;
class KSpreadTable;
class KSpreadDepend;

class KSpreadInterpreter : public KSInterpreter
{
public:
  typedef KSharedPtr<KSpreadInterpreter> Ptr;

  KSpreadInterpreter( KSpreadDoc* );

  KSParseNode* parse( KSContext& context, KSpreadTable* table, const QString& formula, QList<KSpreadDepend>& );
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
