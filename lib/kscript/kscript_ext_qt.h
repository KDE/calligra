#ifndef __KSCRIPT_EXT_QT_H__
#define __KSCRIPT_EXT_QT_H__

#include "kscript_context.h"
#include "kscript_object.h"
#include "kscript_value.h"

#include <qobject.h>
#include <qstring.h>
#include <qcstring.h>
#include <qvaluelist.h>
#include <qconnection.h>

class KSClass;
class KSInterpreter;
class KS_Qt_Object;

extern KSModule::Ptr ksCreateModule_Qt( KSInterpreter* );

/**
 * There is at all times only one instance of this class
 * available.
 */
class KS_Qt_Callback : public QObject
{
  Q_OBJECT
public:
  /**
   * Use this function to connect to the destroyed signal of the QObject.
   * So we become informed if the QObject is about to die.
   */
  void connect( QObject*, KS_Qt_Object* );
  /**
   * Connect a qt_signal of the QObject to a qt_slot of KS_Qt_Callback.
   * KS_Qt_Callback in turn triggers the signal ks_sig of the KScript
   * object r.
   */
  void connect( QObject* s, const char* qt_sig, const char* qt_slot,
		KSObject* r, const char* ks_sig );


  void disconnect( KSObject* );

  static KS_Qt_Callback* self();

protected:
    KS_Qt_Callback() { }
    ~KS_Qt_Callback() { }

    void emitSignal( const QValueList<KSValue::Ptr>& params, const char* name );
    
private slots:
  void slotDestroyed();

  /**
   * For every signal that appears in any Qt class
   * we add a slot here. This is needed to map Qt signals
   * to KScript signals.
   */
  void textChanged( const QString& );
  void clicked();
  void activated( int );
  void activated( const QString& );
  void highlighted( int );
  void highlighted( const QString& );

private:
  struct Connection
  {
    QObject* m_sender;
    QCString m_kscriptSignal;
    KSObject* m_receiver;
  };

  struct DestroyCallback
  {
    QObject* m_sender;
    KS_Qt_Object* m_receiver;
  };

  QValueList<Connection> m_connections;
  QValueList<DestroyCallback> m_callbacks;

  static KS_Qt_Callback* s_pSelf;
};

class KSClass_QObject : public KSScriptClass
{
public:
    KSClass_QObject( KSModule*, const char* name = "QObject" );
    
    virtual bool isBuiltin() { return true; }
    virtual bool hasSignal( const QString& name );
    
    void addQtSignal( const QString& str );
    
private:
    QStringList m_signals;
};

/**
 * All objects that use interally some QObject object
 * must derive from this class.
 */
class KS_Qt_Object : public KSScriptObject
{
public:
  enum Type { StringType,
              IntType,
              BoolType,
              DoubleType,
	      RectType,
	      ObjectType,
	      WidgetType,
              NoType };

  KS_Qt_Object( KSClass* c ) : KSScriptObject( c ) { m_object = 0; }
  ~KS_Qt_Object();

  /**
   * Sets the QObject. If ownership is true, the QObject will be
   * destroyed when the KS_Qt_Object is destroyed.
   */
  void setObject( QObject*, bool ownership = true );
  QObject* object() { return m_object; }

  /**
   * Gets a member of the QObject/KS_Qt_Object. A member is a property or a method.
   */
  virtual KSValue::Ptr member( KSContext&, const QString& name );
  /**
   * Sets a member of the QObject/KS_Qt_Object.
   */
  virtual bool setMember( KSContext&, const QString& name, const KSValue::Ptr& v );

  bool hasOwnership() { return m_ownership; }

  /**
   * Inherited from KSScriptObject
   */
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

  static QObject* convert( KSValue* v ) { return ((KS_Qt_Object*)v->objectValue())->m_object; }

protected:
  virtual bool destructor();

  bool checkDoubleConstructor( KSContext& context, const QString& name );
  bool checkLive( KSContext& context, const QString& name );

private:
  QObject* m_object;
  bool m_ownership;
};

#endif
