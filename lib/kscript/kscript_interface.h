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

#if 0

#ifndef __KSCRIPT_INTERFACE_H__
#define __KSCRIPT_INTERFACE_H__

#include <qshared.h>
#include <qstring.h>

#include "kscript_context.h"
#include "kscript_typecode.h"
#include "kscript_value.h"

class KSParseNode;
class KSScriptObject;
class KSProxy;

class KSAttribute : public QShared
{
public:
  enum Access { ReadOnly, Normal };

  KSAttribute( KSModule* m, const QString& name, Access a, const KSTypeCode::Ptr& );

  void set( KSContext&, KSProxy*, KSValue* );
  KSValue::Ptr get( KSContext&, KSProxy* );

private:
  QString m_name;
  KSModule::Ptr m_module;
  KSTypeCode::Ptr m_typecode;
  Access m_access;
};

class KSInterface : public QShared
{
public:
  KSInterface( KSModule* m, const QString& _name );
  virtual ~KSInterface() { }

  /**
   * Internal. Called when processing a new parsetree for the first time.
   * As this time it is not shure that all KSValue items in this list
   * really contains interface declarations. This is checked by
   * @ref #allSuperInterfaces.
   */
  void setSuperInterfaces( const QValueList<KSValue::Ptr>& super ) { m_superInterfaces = super; }

  KSModule* module() { return m_module; }

  KSNamespace* nameSpace() { return &m_space; }
  const KSNamespace* nameSpace() const { return &m_space; }
  virtual KSValue::Ptr member( KSContext& context, const QString& name );

  /**
   * Creates a proxy for this interface.
   */
  virtual bool constructor( KSContext& context );
  /**
   * @param obj is considered to be of type CORBA::Object
   */
  virtual KSProxy* constructor( void* obj );

  /**
   * @return FALSE if one of the super interfaces is not an interface.
   *         That would indicate an error in the script.
   *
   * Fills the list with all direct and indirect super interfaces.
   * The function works recursively and does not clear the list
   * before inserting new items.
   */
  bool allSuperInterfaces( QValueList<KSValue::Ptr>& );

  /**
   * @return the name of the interface. This name is NOT qualified, that means
   *         it does not contain the name of the module etc.
   */
  QString name() const { return m_name; }

protected:
  QValueList<KSValue::Ptr> m_superInterfaces;
  KSNamespace m_space;
  QString m_name;
  KSModule* m_module;
};

#endif

#endif
