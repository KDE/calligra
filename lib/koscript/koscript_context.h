#ifndef __KSCRIPT_CONTEXT_H__
#define __KSCRIPT_CONTEXT_H__

#include "koscript_value.h"

#include <qstring.h>
#include <qlist.h>

#include <ksharedptr.h>

class KSContext;
class KSParseNode;
class KSInterpreter;

typedef QMap<QString,KSValue::Ptr> KSNamespace;

/**
 * An instance of this class represents a loaded module. It holds the
 * namespace with all symbols of the module and owns the parse tree.
 * A module itself may be inserted in other namespaces.
 */
class KSModule : public KShared
{
public:
    typedef KSharedPtr<KSModule> Ptr;

    KSModule( KSInterpreter*, const QString& name, KSParseNode* = 0 );
    virtual ~KSModule();

    /**
     * Executes the code of the module. This method is usually called
     * after the module has been loaded.
     */
    virtual bool eval( KSContext& );

    /**
     * If @ref KSContext::leftExpr returns TRUE for the given context,
     * then a new member is added to the object if it did not exist.
     * Otherwise 0 is returned for a non existing member.
     * This function may nevertheless set an exception if a member is known but if
     * it could not be read for some reason.
     */
    virtual KSValue::Ptr member( KSContext&, const QString& name );
    /**
     * May set a exception if the member could not be set.
     */
    virtual bool setMember( KSContext&, const QString& name, const KSValue::Ptr& v );

    /**
     * Reimplemented by KSPebblesModule.
     */
    virtual bool isPebbles() const { return FALSE; }

    /**
     * @return the modules name.
     */
    QString name() const { return m_name; }

    /**
     * @return the namespace that contains all members of the module.
     *
     * This method is INTERNAL. Dont use!
     */
    KSNamespace* nameSpace() { return &m_space; }

    /**
     * Find a symbol in the modules namespace. If the symbol is not known 0
     * is returned.
     *
     * This method is INTERNAL. Dont use!
     *
     * @see #member
     */
    KSValue* object( const QString& name );
    /**
     * Insert a symbol in the modules namespace.
     *
     * This method is INTERNAL. Dont use!
     */
    void addObject( const QString& name, const KSValue::Ptr& v );
    /**
     * @return the interpreter that loaded and executed the module.
     */
    KSInterpreter* interpreter() { return m_interpreter; }

protected:
    void setCode( KSParseNode* node );

private:
    QString m_name;
    KSNamespace m_space;
    KSParseNode* m_code;
    KSInterpreter* m_interpreter;
};


class KSSubScope
{
public:
  KSSubScope() { }
  KSSubScope( KSNamespace* n ) { m_spaces.append( n ); }

  /**
   * Will search in the namespace in reverse order. If not found
   * and inserting is requested, the last namespace in the list will
   * be used for insertion.
   */
  KSValue* object( const QString& name, bool insert = FALSE );
  /**
   * Will insert a new value in the last namespace in the list.
   */
  void addObject( const QString& name, const KSValue::Ptr& );

  /**
   * Adds a new namespace at the end of the list.
   */
  void pushNamespace( KSNamespace* nspace ) { m_spaces.append( nspace ); }
  void popNamespace() { m_spaces.removeLast(); }

private:
  QList<KSNamespace> m_spaces;
};


class KSScope : public KShared
{
public:
  typedef KSharedPtr<KSScope> Ptr;

  /**
   * The scope does NOT take over ownership of the namespaces.
   */
  KSScope( const KSNamespace* globalSpace, KSModule *module );
  KSScope( const KSScope& s );

  void pushLocalScope( KSSubScope* scope ) { ASSERT( m_localScope == 0 ); m_localScope = scope;  }
  KSSubScope* popLocalScope() { KSSubScope* s = m_localScope; m_localScope = 0; return s; }
  KSSubScope* localScope() { return m_localScope; }

  void pushModule( KSModule* m ) { ASSERT( m_module == 0 ); m_module = m; m_moduleSpace = m->nameSpace(); }
  KSModule* popModule() { KSModule* n = m_module; m_module = 0; return n; }

  KSModule* module() { return m_module; }

  /**
   * @return the value associated with 'name' or 0.
   *
   * @param insert if TRUE, then a value is inserted if it could
   *        not be found. If there is a local namespace, then it is
   *        inserted there, otherwise in the module namespace.
   */
  KSValue* object( const QString& name, bool insert = FALSE );
  /**
   * Adds a new value to the scope. A value of the same name that already
   * exists in the most specific namespace will be dereferenced and the
   * new one is inserted. The most specific namespace is the local one.
   * if there is no local one the value is added to the module namespace.
   */
  void addObject( const QString& name, const KSValue::Ptr& );

private:
  KSModule* m_module;
  const KSNamespace* m_globalSpace;
  /**
   * Used for quick access. Contains the namespace of @ref #m_module.
   */
  KSNamespace* m_moduleSpace;
  KSSubScope* m_localScope;
};

class KSException : public KShared
{
public:
  typedef KSharedPtr<KSException> Ptr;

  KSException( const QString& _type, const KSValue::Ptr& _ptr, int _line = -1 );
  KSException( const QString& _type, const QString& _val, int _line = -1 );
  KSException( const KSValue::Ptr& _type, const KSValue::Ptr& ptr, int _line = -1 );
  ~KSException() { }

  const QValueList<int>& lines() { return m_lines; }
  void addLine( int l ) { if ( m_lines.isEmpty() ) m_lines.append( l ); else if ( m_lines.last() != l ) m_lines.append( l ); }

  QString toString( KSContext& context );
  void print( KSContext& context );

  KSValue* type() { return m_type; }
  KSValue* value() { return m_value; }

private:
  KSValue::Ptr m_type;
  KSValue::Ptr m_value;
  QValueList<int> m_lines;
};

class KSContext
{
public:
  KSContext();
  KSContext( KSContext& c, bool leftexpr = false );
  ~KSContext();

  void setValue( const KSValue::Ptr& p ) { m_value = p; }
  /**
   * Assumes that the value already has an increased refernce count. That means you
   * can pass new created values or values gotten from @ref #shareValue to this
   * function.
   */
  void setValue( KSValue* p ) { m_value = p; }
  /**
   * @return the current value or 0. The reference count is not changed by this function.
   *         use it to examine the value or make a copy of it, but dont assign it to
   *         another @ref KSContext directly.
   */
  KSValue* value() { return m_value; }
  /**
   * @return the value of this context. The value has an increaed reference count. That means you
   *         can assign it directly to another context. Mention however, that @ref KSValue is a shared
   *         object, so if you need a real copy, then dont use this function.
   */
  KSValue* shareValue() { if ( !m_value ) return 0; m_value->_KShared_ref(); return m_value; }

  void setException( KSContext& c ) { m_exception = c.exception(); if ( c.exception() ) c.exception()->_KShared_ref(); }
  void setException( KSException::Ptr& p ) { m_exception = p; }
  void setException( KSException* p ) { m_exception = p; }
  KSException* exception() { return m_exception; }
  KSException* shareException() { if ( !m_exception ) return 0; m_exception->_KShared_ref(); return m_exception; }

  void setScope( KSContext& c ) { m_scope = c.scope(); if ( c.scope() ) c.scope()->_KShared_ref(); }
  void setScope( KSScope::Ptr& p ) { m_scope = p; }
  void setScope( KSScope* p ) { m_scope = p; }
  KSScope* scope() { return m_scope; }
  KSValue* object( const QString& _name ) { if ( !!m_scope ) return m_scope->object( _name, m_bLeftExpr ); return 0; }

  void setLeftExpr( bool b ) { m_bLeftExpr = b; }
  bool leftExpr() { return m_bLeftExpr; }

  /**
   * This is a convenience function which calls scope()->module()
   */
  KSModule* module() { return m_scope->module(); }

  KSInterpreter* interpreter() { return m_scope->module()->interpreter(); }

  /**
   * @return TRUE if there was a return statement. This flag is set until
   *         the stack unwinding is finished.
   *
   * @see #clearReturnFlag
   */
  bool returnFlag() const { return m_bReturning; }
  /**
   * This function is called when stack unwinding ends.
   *
   * @see #returnFlag
   */
  void clearReturnFlag() { m_bReturning = false; }
  /**
   * This function is called when stack unwinding starts due to a
   * "return" expression in the source code.
   *
   * @see #returnFlag
   */
  void setReturnFlag( bool b = true ) { m_bReturning = b; }

  int tmpInt;

private:
  KSValue::Ptr m_value;
  KSException::Ptr m_exception;
  KSScope::Ptr m_scope;
  bool m_bLeftExpr;
  bool m_bReturning;
};

#endif
