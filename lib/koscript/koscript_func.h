#ifndef __KSCRIPT_FUNC_H__
#define __KSCRIPT_FUNC_H__

#include "koscript_value.h"
#include "koscript_context.h"
#include "koscript_parsenode.h"

#include <ksharedptr.h>

class KSParseNode;
class KSInterpreter;

KSModule::Ptr ksCreateModule_KScript( KSInterpreter* );

class KSFunction : public KShared
{
public:
  typedef KSharedPtr<KSFunction> Ptr;

  KSFunction( KSModule* m ) : KShared(), m_module( m ) { }

  virtual bool call( KSContext& context ) = 0;
  virtual QString name() const = 0;

  KSModule* module() { return m_module; }

private:
  KSModule* m_module;
};

typedef bool (*KSBuiltinFuncPtr)( KSContext& context );

class KSBuiltinFunction : public KSFunction
{
public:
  KSBuiltinFunction( KSModule* m, const QString& _name, KSBuiltinFuncPtr func ) : KSFunction( m ) { m_func = func; m_name = _name; }
  virtual ~KSBuiltinFunction() { }

  virtual bool call( KSContext& context ) { return m_func( context ); }
  virtual QString name() const { return m_name; }

private:
  KSBuiltinFuncPtr m_func;
  QString m_name;
};

class KSScriptFunction : public KSFunction
{
public:
  /**
   * The object does NOT take over the ownership of the module
   * since the module would never be deleted then. That is because
   * the module owns this function.
   */
  KSScriptFunction( KSModule* m, KSParseNode* node ) : KSFunction( m ) { m_node = node; }

  virtual bool call( KSContext& context );
  virtual QString name() const { return m_node->getIdent(); }

private:
  KSParseNode* m_node;
};

#endif
