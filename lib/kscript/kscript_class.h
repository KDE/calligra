#ifndef __KSCRIPT_CLASS_H__
#define __KSCRIPT_CLASS_H__

#include <qshared.h>
#include <qstring.h>

#include "kscript_context.h"

class KSParseNode;
class KSScriptObject;

class KSClass : public QShared
{
public:
  KSClass( KSModule* m, const QString& _name ) { m_name = _name; m_module = m; }
  virtual ~KSClass() { }

  void setSuperClasses( const QValueList<KSValue::Ptr>& super ) { m_superClasses = super; }

  KSModule* module() { return m_module; }

  KSNamespace* nameSpace() { return &m_space; }
  const KSNamespace* nameSpace() const { return &m_space; }
  virtual KSValue::Ptr member( KSContext& context, const QString& name );

  virtual bool isBuiltin() = 0;

  /**
   * Creates a new instance of this class.
   */
  virtual bool constructor( KSParseNode* node, KSContext& context ) = 0;
  
  void allSuperClasses( QValueList<KSValue::Ptr>& );
  
  QString name() const { return m_name; }
  
  virtual KSScriptObject* createObject( KSClass* c ) = 0;
  
  virtual bool hasSignal( const QString& name ) = 0;
 
protected:
  QValueList<KSValue::Ptr> m_superClasses;
  KSNamespace m_space;
  QString m_name;
  KSModule* m_module;
};

class KSScriptClass : public KSClass
{
public:
  KSScriptClass( KSModule* m, const QString& _name, KSParseNode* node ) : KSClass( m, _name ), m_node( node ) { }
  ~KSScriptClass() { }

  virtual bool constructor( KSParseNode* node, KSContext& context );

  virtual bool isBuiltin() { return false; }

  virtual bool hasSignal( const QString& name );

protected:
  virtual KSScriptObject* createObject( KSClass* c );

private:
  KSParseNode* m_node;
};

#endif
