#ifndef __kspread_interpreter_h__
#define __kspread_interpreter_h__

#include <kscript.h>

#include "kspread_cell.h"

class KSParseNode;
class KSContext;

class KSpreadDoc;
class KSpreadTable;

class KSpreadInterpreter : public KSInterpreter
{
public:
  typedef KSSharedPtr<KSpreadInterpreter> Ptr;

  KSpreadInterpreter( KSpreadDoc* );

  KSParseNode* parse( KSContext& context, KSpreadTable* table, const QString& formula, QList<KSpreadDepend>& );
  bool evaluate( KSContext& context, KSParseNode*, KSpreadTable* );

  KSNamespace* globalNamespace() { return m_global; }

  virtual bool processExtension( KSContext& context, KSParseNode* node );

private:
  KSpreadDoc* m_doc;
  KSpreadTable* m_table;
};

#endif
