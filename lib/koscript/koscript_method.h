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

#ifndef __KSCRIPT_METHOD_H__
#define __KSCRIPT_METHOD_H__

#include <qshared.h>
#include <qmap.h>
#include <qptrlist.h>
#include <qstring.h>

#include "koscript_ptr.h"
#include "koscript_context.h"
#include "koscript_func.h"
#include "koscript_struct.h"

#include <string.h>

/**
 * A method holds a reference to some instance and some function. The function
 * may be of the type KSFunction, KSBuiltinFunction or KSStructBuiltinFunction.
 * The instance may be a KSObject or KSStruct or derived types.
 */
class KSMethod : public QShared
{
public:
  KSMethod( KSModule* m, const KSValue::Ptr& obj, const KSValue::Ptr& func )
      : QShared(), m_object( obj ), m_func( func ), m_module( m ) { }
  /**
   * Use this method if the function is KSBuiltinFunction or KSStructBuiltinFunction.
   * In this case we pass the name of the method. No copy of this name is made and the
   * destructor does not free the string. So you should only pass constants here.
   */
  KSMethod( KSModule* m, const KSValue::Ptr& obj, const KSValue::Ptr& func, const QString& name )
      : QShared(), m_object( obj ), m_func( func ), m_module( m ), m_methodName( name ) { }
  virtual ~KSMethod() { }

  bool call( KSContext& context );

  KSValue* object() { return m_object; }
  KSValue* function() { return m_func; }

  KSModule* module() { return m_module; }

  QString name() { return ( m_methodName.isEmpty() ? m_func->functionValue()->name() : m_methodName ); }

private:
  KSValue::Ptr m_object;
  KSValue::Ptr m_func;
  KSModule* m_module;

  QString m_methodName;
};

#endif
