#ifndef __kspread_interpreter_h__
#define __kspread_interpreter_h__

#include <koscript.h>

#include "kspread_global.h"

class KSParseNode;
class KSContext;

class KSpreadCell;
class KSpreadDoc;
class KSpreadSheet;

class KSpreadInterpreter : public KSInterpreter
{
public:
  typedef KSSharedPtr<KSpreadInterpreter> Ptr;

  KSpreadInterpreter( KSpreadDoc* );

  KSParseNode* parse( KSContext& context, KSpreadSheet* table, const QString& formula );
  bool evaluate( KSContext& context, KSParseNode*, KSpreadSheet*, KSpreadCell* );

  KSNamespace* globalNamespace()const  { return m_global; }

  virtual bool processExtension( KSContext& context, KSParseNode* node );

  KSpreadDoc* document()const { return m_doc; }
  KSpreadSheet* table()const { return m_table; }
  KSpreadCell* cell() const { return m_cell; }

private:
  KSpreadCell* m_cell;
  KSpreadDoc* m_doc;
  KSpreadSheet* m_table;
};

#endif
