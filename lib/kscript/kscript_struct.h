#ifndef __KSCRIPT_STRUCT_H__
#define __KSCRIPT_STRUCT_H__

#include <qshared.h>
#include <qstring.h>
#include <qstringlist.h>

#include "kscript_value.h"
#include "kscript_context.h"

class KSParseNode;
class KSStruct;

class KSStructClass : public QShared
{
public:
  typedef KSSharedPtr<KSStructClass> Ptr;

  KSStructClass( KSModule* module, const QString& name, const KSParseNode* n );
  virtual ~KSStructClass() { }

  bool constructor( KSContext& c );
  /**
   * Creates a new KSStruct of this class. The returned object
   * has a reference count of 1.
   */
  virtual KSStruct* constructor();

  KSModule* module() { return m_module; }

  KSNamespace* nameSpace() { return &m_space; }
  const KSNamespace* nameSpace() const { return &m_space; }
  virtual KSValue::Ptr member( KSContext& context, const QString& name );

  const QStringList& vars() const { return m_vars; }
  void addVariable( const QString& v ) { m_vars.append( v ); }

  const QString name() const { return m_name; }

  /**
   * When getting a pointer to a KSObject via @ref KSValue::objectValue this function
   * helps to do some dynamic casting.
   */
  virtual bool inherits( const char* name ) { return ( strcmp( name, "KSStructClass" ) == 0 ); }

private:
  QString m_name;
  KSNamespace m_space;
  const KSParseNode* m_node;
  QStringList m_vars;
  KSModule* m_module;
};

class KSStruct : public QShared
{
public:
  typedef KSSharedPtr<KSStruct> Ptr;

  KSStruct( KSStructClass* c ) { m_class = c; }
  virtual ~KSStruct() { }

  /**
   * Implements a KScript function of the same name.
   */
  bool isA( KSContext& context );

  virtual KSValue::Ptr member( KSContext&, const QString& name );
  virtual bool setMember( KSContext&, const QString& name, const KSValue::Ptr& v );

  const KSStructClass* getClass() const { return m_class; }
  KSStructClass* getClass() { return m_class; }

  virtual KSStruct* clone() { KSStruct *s = new KSStruct( m_class ); s->m_space = m_space; return s; }

  KSModule* module() { return m_class->module(); }
  KSNamespace* instanceNameSpace() { return &m_space; }
  const KSNamespace* instanceNameSpace() const { return &m_space; }

private:
  KSStructClass* m_class;
  KSNamespace m_space;
};

#endif
