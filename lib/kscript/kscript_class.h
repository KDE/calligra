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

#ifndef __KSCRIPT_CLASS_H__
#define __KSCRIPT_CLASS_H__

#include <qshared.h>
#include <qstring.h>
#include <qcstring.h>

#include "kscript_context.h"

class KSParseNode;
class KSScriptObject;

class KSClass : public QShared
{
public:
  KSClass( KSModule* m, const QString& _name ) { m_name = _name; m_module = m; }
  virtual ~KSClass() { }

  void setSuperClasses( const QValueList<KSValue::Ptr>& super ) { m_superClasses = super; }
  void setSuperClass( const KSValue::Ptr& super ) { m_superClasses.clear(); m_superClasses.append( super ); }

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
  /**
   * @return the full name of the class, like this: "qt:QWidget" or "kde:KColorDialog".
   */
  QString fullName() const;

  virtual KSScriptObject* createObject( KSClass* c ) = 0;

  virtual bool hasSignal( const QString& name ) = 0;

  /**
   * @retrun TRUE if the class is of type @p name or inherits from this class.
   *
   * @param name is a full classname like "qt:QWidget" where "qt" is the module and "QWidget"
   *             the class name.
   */
  virtual bool inherits( const QCString& name ) const;

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
