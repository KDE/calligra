/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __KSCRIPT_CONTEXT_H__
#define __KSCRIPT_CONTEXT_H__

#include "koscript_value.h"
#include "koscript_ptr.h"

#include <koffice_export.h>

#include <qshared.h>
#include <qstring.h>
#include <qptrlist.h>

class KSContext;
class KSParseNode;
class KSInterpreter;

typedef QMap<QString,KSValue::Ptr> KSNamespace;

/**
 * An instance of this class represents a loaded module. It holds the
 * namespace with all symbols of the module and owns the parse tree.
 * A module itself may be inserted in other namespaces.
 */
class KOSCRIPT_EXPORT KSModule : public QShared
{
public:
    typedef KSSharedPtr<KSModule> Ptr;

    KSModule( KSInterpreter*, const QString& name, KSParseNode* = 0 );
    virtual ~KSModule();

    /**
     * Executes the code of the module. This method is usually called
     * after the module has been loaded.
     */
    virtual bool eval( KSContext& );

    /**
     * If KSContext::leftExpr returns TRUE for the given context,
     * then a new member is added to the object if it did not exist.
     * Otherwise 0 is returned for a non existing member.
     * This function may nevertheless set an exception if a member is known but if
     * it could not be read for some reason.
     */
    virtual KSValue::Ptr member( KSContext&, const QString& name );
    /**
     * May set an exception if the member could not be set.
     */
    virtual bool setMember( KSContext&, const QString& name, const KSValue::Ptr& v );

    /**
     * Reimplemented by KSPebblesModule.
     */
    virtual bool isPebbles() const { return FALSE; } // BIC: remove

    /**
     * @return the module's name.
     */
    QString name() const { return m_name; }

    /**
     * @internal
     * @return the namespace that contains all members of the module.
     *
     * This method is INTERNAL. Don't use!
     */
    KSNamespace* nameSpace() { return &m_space; }

    /**
     * @internal
     * Find a symbol in the modules namespace. If the symbol is not known 0
     * is returned.
     *
     * This method is INTERNAL. Don't use!
     *
     * @see #member
     */
    KSValue* object( const QString& name );
    /**
     * @internal
     * Insert a symbol in the modules namespace.
     *
     * This method is INTERNAL. Don't use!
     */
    void addObject( const QString& name, const KSValue::Ptr& v );
    /**
     * @internal
     * Remove a symbol in the modules namespace.
     *
     * This method is INTERNAL. Don't use!
     */
    void removeObject( const QString& name );
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

  /**
   * Removes the last namespace from the list.
   */
  void popNamespace() { m_spaces.removeLast(); }

private:
  QPtrList<KSNamespace> m_spaces;
};


class KOSCRIPT_EXPORT KSScope : public QShared
{
public:
  typedef KSSharedPtr<KSScope> Ptr;

  /**
   * The scope does NOT take over ownership of the namespaces.
   */
  KSScope( const KSNamespace* globalSpace, KSModule *module );
  KSScope( const KSScope& s );

  void pushLocalScope( KSSubScope* scope ) { Q_ASSERT( m_localScope == 0 ); m_localScope = scope;  }
  KSSubScope* popLocalScope() { KSSubScope* s = m_localScope; m_localScope = 0; return s; }
  KSSubScope* localScope() { return m_localScope; }

  void pushModule( KSModule* m ) { Q_ASSERT( m_module == 0 ); m_module = m; m_moduleSpace = m->nameSpace(); }
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

class KOSCRIPT_EXPORT KSException : public QShared
{
public:
  typedef KSSharedPtr<KSException> Ptr;

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

class KOSCRIPT_EXPORT KSContext
{
public:
  KSContext();
  KSContext( KSContext& c, bool leftexpr = false );
  ~KSContext();

  void setValue( const KSValue::Ptr& p ) { m_value = p; }
  /**
   * Assumes that the value already has an increased reference count. That means you
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
   *         object, so if you need a real copy, then don't use this function.
   */
  KSValue* shareValue() { if ( !m_value ) return 0; m_value->ref(); return m_value; }

  /**
   * Set a field of generic extra data for this context
   */
  void setExtraData( KSValue* p ) { m_extraData = p; }

  /**
   * Retrieve any extra data associated with this context
   */
  KSValue* extraData() { return m_extraData; }


  void setException( KSContext& c ) { m_exception = c.exception(); if ( c.exception() ) c.exception()->ref(); }
  void setException( KSException::Ptr& p ) { m_exception = p; }
  void setException( KSException* p ) { m_exception = p; }
  KSException* exception() { return m_exception; }
  KSException* shareException() { if ( !m_exception ) return 0; m_exception->ref(); return m_exception; }

  void setScope( KSContext& c ) { m_scope = c.scope(); if ( c.scope() ) c.scope()->ref(); }
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
  KSValue::Ptr m_extraData;
  KSException::Ptr m_exception;
  KSScope::Ptr m_scope;
  bool m_bLeftExpr;
  bool m_bReturning;
};

#endif
