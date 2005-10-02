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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __KSCRIPT_STRUCT_H__
#define __KSCRIPT_STRUCT_H__

#include <qshared.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qcstring.h>
#include <qmap.h>

#include "koscript_value.h"
#include "koscript_context.h"

class KSParseNode;
class KSStruct;

class KSStructClass : public QShared
{
public:
  typedef KSSharedPtr<KSStructClass> Ptr;

  KSStructClass( KSModule* module, const QString& name /*, const KSParseNode* n*/ );
  virtual ~KSStructClass() { }

  virtual bool constructor( KSContext& c );
  /**
   * Creates a new KSStruct of this class. The returned object
   * has a reference count of 1.
   */
  KSStruct* constructor();

  KSModule* module() { return m_module; }

  KSNamespace* nameSpace() { return &m_space; }
  const KSNamespace* nameSpace() const { return &m_space; }
  virtual KSValue::Ptr member( KSContext& context, const QString& name );

  const QStringList& vars() const { return m_vars; }
  void addVariable( const QString& v ) { m_vars.append( v ); }
  void setVariables( const QStringList& l ) { m_vars = l; }
  bool hasVariable( const QString& v ) { return m_vars.contains( v ); }

  /**
   * @return the name of the class, for example "QRect" or "QPixmap".
   *
   * @see #fullName
   */
  QString name() const { return m_name; }
  /**
   * @return the name of the class with prepended name of the module like this:
   * "qt:QRect" or "kde:KColorDialog"
   *
   * @see #name
   */
  QString fullName() const;

  /**
   * When getting a pointer to a KSObject via @ref KSValue::objectValue this function
   * helps to do some dynamic casting.
   */
  virtual bool inherits( const char* name ) { return ( strcmp( name, "KSStructClass" ) == 0 ); }

private:
  QString m_name;
  KSNamespace m_space;
    // const KSParseNode* m_node;
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

  /**
   * A convenience function
   */
  QString className() const { return m_class->name(); }

    // ########## Torben: Make real copies of the menus.
  virtual KSStruct* clone() { KSStruct *s = new KSStruct( m_class ); s->m_space = m_space; return s; }

  KSModule* module() { return m_class->module(); }
  KSNamespace* instanceNameSpace() { return &m_space; }
  const KSNamespace* instanceNameSpace() const { return &m_space; }

  /**
   * This function is used in @ref KSBuiltinStruct. We put that in here
   * to avoid casting to KSBuiltinStruct all the time.
   */
  virtual void* object() { return 0; }
  virtual const void* object() const { return 0; }

private:
  KSStructClass* m_class;
  KSNamespace m_space;
};

class KSBuiltinStruct;

class KSBuiltinStructClass : public KSStructClass
{
    friend class KSBuiltinStruct;
public:
    KSBuiltinStructClass( KSModule* module, const QString& name );
    virtual ~KSBuiltinStructClass() { }

    virtual bool constructor( KSContext& c ) = 0;
    virtual bool destructor( void* object ) = 0;
    virtual KSStruct* clone( KSBuiltinStruct* ) = 0;

    typedef bool (*MethodPtr)( void* object, KSContext&, const QValueList<KSValue::Ptr>& args );

    /**
     * @param signature is the signature of the method. Passing an empty string here means
     *                  that the method does not expect any parameter while a null string means
     *                  that the function will check the arguments itself.
     */
    void addMethod( const QString& name, MethodPtr func, const QCString& signature );
    bool hasMethod( const QString& ) const;

    bool call( void* instance, KSContext& context, const QString& name );

protected:
    /*
     * It can not happen that @p name is not the name of a variable, since @ref KSBuiltinStruct
     * checks wether @p name is really a variable of this struct before calling.
     */
    virtual KSValue::Ptr property( KSContext& context, void* object, const QString& name ) = 0;
    /**
     * If the type does not match the property, you must give an exception.
     * If the property is readonly just return 0 and dont give an exception.
     *
     * It can not happen that @p name is not the name of a variable, since @ref KSBuiltinStruct
     * checks wether @p name is really a variable of this struct before calling.
     */
    virtual bool setProperty( KSContext& context, void* object, const QString& name, const KSValue::Ptr value ) = 0;

private:
    struct Method
    {
	MethodPtr m_method;
	QCString m_signature;
    };

    QMap<QString,Method> m_methods;
};


class KSBuiltinStruct : public KSStruct
{
public:
    KSBuiltinStruct( KSStructClass* c, void* object );
    /**
     * Destroys the struct and the associated C++ object.
     *
     * @see KSBuiltinStructClass::destructor
     */
    virtual ~KSBuiltinStruct();

    virtual KSValue::Ptr member( KSContext&, const QString& name );
    virtual bool setMember( KSContext&, const QString& name, const KSValue::Ptr& v );

    /**
     * This is the universal method dispatcher.
     *
     * @see KSBuiltinStructClass::call
     */
    bool call( KSContext& context, const QString& name );

    /**
     * Make a real copy of the struct. That means that the C++ object
     * is cloned, too.
     *
     * @see KSBuiltinStructClass::clone
     */
    KSStruct* clone();

    /**
     * @return a pointer to the C++ object that holds the real data of this struct.
     */
    void* object();
    const void* object() const;

private:
    void* m_object;
};

#endif
