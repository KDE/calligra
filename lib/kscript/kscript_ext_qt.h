#ifndef __KSCRIPT_EXT_QT_H__
#define __KSCRIPT_EXT_QT_H__

#include "kscript_context.h"
#include "kscript_object.h"
#include "kscript_value.h"

#include <qobject.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <qconnection.h>

class KSClass;
class KSInterpreter;

class QRect;

extern KSModule::Ptr ksCreateModule_Qt( KSInterpreter* );

class KS_Qt_Callback : public QObject
{
  Q_OBJECT
public:
  KS_Qt_Callback() { }
  ~KS_Qt_Callback() { }

  void connect( QObject*, KSObject* );
  void connect( QObject*, const QString& , KSObject*, const KSValue::Ptr& );

  void disconnect( KSObject* );

  static KS_Qt_Callback* self();

private slots:
  void slotDestroyed();

private:
  struct Connection
  {
    QObject* m_sender;
    QString m_signal;
    KSObject* m_receiver;
    KSValue::Ptr m_slot;
  };

  struct DestroyCallback
  {
    QObject* m_sender;
    KSObject* m_receiver;
  };

  QValueList<Connection> m_connections;
  QValueList<DestroyCallback> m_callbacks;

  static KS_Qt_Callback* s_pSelf;
};

class KS_Qt_Object : public KSScriptObject
{
public:
  enum Type { StringType,
              IntType,
              BoolType,
              DoubleType,
	      RectType,
              NoType };
  
  KS_Qt_Object( KSClass* c ) : KSScriptObject( c ) { m_object = 0; }
  ~KS_Qt_Object();
  
  void setObject( QObject*, bool ownership = true );
  QObject* object() { return m_object; }

  virtual KSValue::Ptr member( KSContext&, const QString& name );
  virtual bool setMember( KSContext&, const QString& name, KSValue* v );

  bool hasOwnership() { return m_ownership; }

  bool inherits( const char* name ) { return ( strcmp( name, "KS_Qt_Object" ) == 0 || KSScriptObject::inherits( name ) ); }

  static bool checkArguments( KSContext& context, KSValue* v, const QString& name, Type t1 = NoType, Type t2 = NoType,
			      Type t3 = NoType, Type t4 = NoType, Type t5 = NoType, Type t6 = NoType );
  static bool tryArguments( KSContext& context, KSValue* v, Type t1 = NoType, Type t2 = NoType,
			    Type t3 = NoType, Type t4 = NoType, Type t5 = NoType, Type t6 = NoType );
  /**
   * If parameter v can not be casted to the Qt type then an exception
   * is set in the context if _fatal is TRUE and FALSE is returned.
   * If the parameter has correct type, then TRUE is returned.
   */
  static bool checkType( KSContext& context, KSValue* v, KS_Qt_Object::Type type, bool _fatal = true );

protected:
  virtual bool destructor();

  bool checkDoubleConstructor( KSContext& context, const QString& name );
  bool checkLive( KSContext& context, const QString& name );

private:
  QObject* m_object;
  bool m_ownership;
};

#endif
