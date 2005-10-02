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

#ifndef __KSCRIPT_OBJECT_H__
#define __KSCRIPT_OBJECT_H__

#include <qshared.h>
#include <qmap.h>
#include <qptrlist.h>
#include <qstring.h>

#include "koscript_ptr.h"
#include "koscript_context.h"
#include "koscript_func.h"
#include "koscript_struct.h"

#include <string.h>

class KSProperty : public QShared
{
public:
  KSProperty( const KSStruct::Ptr& struc, const QString& name ) { m_struct = struc; m_name = name; }
  KSProperty( const KSModule::Ptr& m, const QString& name ) { m_module = m; m_name = name; }

  virtual ~KSProperty() { }

  QString name() { return m_name; }

  virtual bool set( KSContext&, const KSValue::Ptr& v );

private:
  KSStruct::Ptr m_struct;
  KSModule::Ptr m_module;
  QString m_name;
};

#endif
