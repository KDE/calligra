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

#ifndef __KSCRIPT_EXT_QHBOXLAYOUT_H__
#define __KSCRIPT_EXT_QHBOXLAYOUT_H__

#include "kscript_class.h"
#include "kscript_ext_qboxlayout.h"

class KSContext;

class QHBoxLayout;

class KSClass_QHBoxLayout : public KSClass_QBoxLayout
{
public:
  KSClass_QHBoxLayout( KSModule*, const char* name = "QHBoxLayout" );

protected:
  virtual KSScriptObject* createObject( KSClass* c );
};

class KSObject_QHBoxLayout : public KSObject_QBoxLayout
{
public:
  KSObject_QHBoxLayout( KSClass* );

  bool ksQHBoxLayout( KSContext& );

  KSValue::Ptr member( KSContext& context, const QString& name );
  bool setMember( KSContext& context, const QString& name, const KSValue::Ptr& v );

  bool inherits( const char* name ) { return ( strcmp( name, "KSObject_QHBoxLayout" ) == 0 || KSObject_QBoxLayout::inherits( name ) ); }

  static QHBoxLayout* convert( KSValue* v ) { return (QHBoxLayout*) ((KS_Qt_Object*)v->objectValue())->object(); }
};

#endif
