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

#ifndef __KSCRIPT_FUNC_H__
#define __KSCRIPT_FUNC_H__

#include "koscript_value.h"
#include "koscript_context.h"
#include "koscript_ptr.h"
#include "koscript_parsenode.h"

#include <qshared.h>

class KSParseNode;
class KSInterpreter;

KSModule::Ptr ksCreateModule_KScript( KSInterpreter* );

class KSFunction : public QShared
{
public:
  typedef KSSharedPtr<KSFunction> Ptr;

  KSFunction( KSModule* m ) : QShared(), m_module( m ) { }

  virtual bool call( KSContext& context ) = 0;
  virtual QString name() const = 0;

  KSModule* module() { return m_module; }

private:
  KSModule* m_module;
};

typedef bool (*KSBuiltinFuncPtr)( KSContext& context );

class KSBuiltinFunction : public KSFunction
{
public:
  KSBuiltinFunction( KSModule* m, const QString& _name, KSBuiltinFuncPtr func ) : KSFunction( m ) { m_func = func; m_name = _name; }
  virtual ~KSBuiltinFunction() { }

  virtual bool call( KSContext& context ) { return m_func( context ); }
  virtual QString name() const { return m_name; }

private:
  KSBuiltinFuncPtr m_func;
  QString m_name;
};

class KSScriptFunction : public KSFunction
{
public:
  /**
   * The object does NOT take over the ownership of the module
   * since the module would never be deleted then. That is because
   * the module owns this function.
   */
  KSScriptFunction( KSModule* m, KSParseNode* node ) : KSFunction( m ) { m_node = node; }

  virtual bool call( KSContext& context );
  virtual QString name() const { return m_node->getIdent(); }

private:
  KSParseNode* m_node;
};

#endif
