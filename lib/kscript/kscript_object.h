#ifndef __KSCRIPT_OBJECT_H__
#define __KSCRIPT_OBJECT_H__

#include <qshared.h>
#include <qmap.h>
#include <qlist.h>
#include <qstring.h>

#include "kscript_ptr.h"
#include "kscript_context.h"
#include "kscript_class.h"
#include "kscript_func.h"
#include "kscript_struct.h"
#include "kscript_proxy.h"

#include <string.h>

class KSObject : public QShared
{
public:
  typedef KSSharedPtr<KSObject> Ptr;

  enum Status { Dead, Alive };

  KSObject( KSClass* c );
  virtual ~KSObject() { destructor(); }

  bool kill() { bool b = destructor(); m_status = Dead; return b; }

  KSModule* module() { return m_class->module(); }
  KSSubScope* scope() { return &m_scope; }
  KSNamespace* instanceNameSpace() { return &m_space; }
  const KSNamespace* instanceNameSpace() const { return &m_space; }

  const KSClass* getClass() const { return m_class; }
  KSClass* getClass() { return m_class; }

  Status status() { return m_status; }
  void setStatus( Status s ) { m_status = s; }

  bool connect( const QString& sig, KSObject* r, KSValue* s );
  void disconnect();
  void disconnect( KSObject* o );
  void disconnect( KSObject* o, KSValue* _slot );
  bool emitSignal( const QString& name, KSContext& context );

  void removeSender( KSObject* o ) { m_sender.removeRef( o ); }
  void appendSender( KSObject* o ) { m_sender.append( o ); }

  /**
   * Implements a KScript function of the same name.
   */
  bool disconnect( KSContext& context );
  /**
   * Implements a KScript function of the same name.
   */
  bool inherits( KSContext& context );
  /**
   * Implements a KScript function of the same name.
   */
  bool isA( KSContext& context );

  /**
   * If @ref KSContext::leftExpr retruns TRUE for the given context,
   * then a new member is added to the object if it did not exist.
   * Otherwise 0 is returned for a non existing member.
   * This function may nevertheless set a exception if a member is known but if
   * it could not be read for some reason.
   */
  virtual KSValue::Ptr member( KSContext&, const QString& name );
  /**
   * May set a exception if the member could not be set.
   */
  virtual bool setMember( KSContext&, const QString& name, const KSValue::Ptr& v );

  /**
   * When getting a pointer to a KSObject via @ref KSValue::objectValue this function
   * helps to do some dynamic casting.
   */
  virtual bool inherits( const char* name ) { return ( strcmp( name, "KSObject" ) == 0 ); }

protected:
  struct Signal
  {
    KSObject* m_receiver;
    KSValue::Ptr m_slot;
  };

  typedef QValueList<Signal> SignalList;
  typedef QMap<QString,SignalList> SignalMap;

  SignalList* findSignal( const QString& name, bool insert = false );

  virtual bool destructor();

private:
  /**
   * This namespace holds the variables of this instance.
   */
  KSNamespace m_space;
  /**
   * This scope holds all namespaces of base classes and the local
   * namespace.
   */
  KSSubScope m_scope;
  /**
   * This objects class.
   */
  KSClass* m_class;
  Status m_status;

  SignalMap m_signals;
  QList<KSObject> m_sender;
};

class KSScriptObject : public KSObject
{
public:
  KSScriptObject( KSClass* c ) : KSObject( c ) { }
  ~KSScriptObject();

  bool inherits( const char* name ) { return ( strcmp( name, "KSScriptObject" ) == 0 || KSObject::inherits( name ) ); }

protected:
  virtual bool destructor();
};

/**
 * A method holds a reference to some instance and some function. The function
 * may be of the type @ref KSFunction, KSBuiltinFunction or KSStructBuiltinFunction.
 * The instance may be a @ref KSObject or @ref KSStruct or derived types.
 */
class KSMethod : public QShared
{
public:
  KSMethod( KSModule* m, const KSValue::Ptr& obj, const KSValue::Ptr& func ) : QShared(), m_object( obj ), m_func( func ), m_module( m ), m_method_name( 0 ) { }
  /**
   * Use this method if the function is KSBuiltinFunction or KSStructBuiltinFunction.
   * In this case we pass the name of the method. No copy of this name is made and the
   * destructor does not free the string. So you should only pass constants here.
   */
  KSMethod( KSModule* m, const KSValue::Ptr& obj, const KSValue::Ptr& func, const char* name )
      : QShared(), m_object( obj ), m_func( func ), m_module( m ), m_method_name( name ) { }
  virtual ~KSMethod() { }

  bool call( KSContext& context );

  KSValue* object() { return m_object; }
  KSValue* function() { return m_func; }

  KSModule* module() { return m_module; }

  const char* name() { return ( m_method_name != 0 ? m_method_name : m_func->functionValue()->name().latin1() ); }

private:
  KSValue::Ptr m_object;
  KSValue::Ptr m_func;
  KSModule* m_module;

  const char* m_method_name;
};

class KSProperty : public QShared
{
public:
  KSProperty( const KSObject::Ptr& obj, const QString& name ) { m_obj = obj; m_name = name; }
  KSProperty( const KSStruct::Ptr& struc, const QString& name ) { m_struct = struc; m_name = name; }
  KSProperty( const KSProxy::Ptr& proxy, const QString& name ) { m_proxy = proxy; m_name = name; }

  virtual ~KSProperty() { }

  QString name() { return m_name; }

  virtual bool set( KSContext&, const KSValue::Ptr& v );

private:
  KSObject::Ptr m_obj;
  KSStruct::Ptr m_struct;
  KSProxy::Ptr m_proxy;
  QString m_name;
};

#endif
