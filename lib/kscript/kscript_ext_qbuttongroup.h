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

#ifndef __KSCRIPT_EXT_QBUTTONGROUP_H__
#define __KSCRIPT_EXT_QBUTTONGROUP_H__

#include "kscript_class.h"
#include "kscript_ext_qwidget.h"

class KSContext;

class KSClass_QButtonGroup : public KSClass_QWidget
{
public:
  KSClass_QButtonGroup( KSModule*, const char* name = "QButtonGroup" );

protected:
  virtual KSScriptObject* createObject( KSClass* c );
};

class KSObject_QButtonGroup : public KSObject_QWidget
{
public:
  KSObject_QButtonGroup( KSClass* );

  bool ksQButtonGroup( KSContext& );
  bool ksQButtonGroup_insert( KSContext& );
  bool ksQButtonGroup_remove( KSContext& );
  bool ksQButtonGroup_clicked( KSContext& );

  KSValue::Ptr member( KSContext& context, const QString& name );
  bool setMember( KSContext& context, const QString& name, const KSValue::Ptr& v );

  bool inherits( const char* name ) { return ( strcmp( name, "KSObject_QButtonGroup" ) == 0 || KSObject_QWidget::inherits( name ) ); }

  void setObject( QObject* obj );
};

class KSClass_QVButtonGroup : public KSClass_QButtonGroup
{
public:
  KSClass_QVButtonGroup( KSModule*, const char* name = "QVButtonGroup" );

protected:
  virtual KSScriptObject* createObject( KSClass* c );
};

class KSObject_QVButtonGroup : public KSObject_QButtonGroup
{
public:
  KSObject_QVButtonGroup( KSClass* );

  bool ksQVButtonGroup( KSContext& );

  KSValue::Ptr member( KSContext& context, const QString& name );
  bool setMember( KSContext& context, const QString& name, const KSValue::Ptr& v );

  bool inherits( const char* name ) { return ( strcmp( name, "KSObject_QVButtonGroup" ) == 0 || KSObject_QButtonGroup::inherits( name ) ); }

  void setObject( QObject* obj );
};

class KSClass_QHButtonGroup : public KSClass_QButtonGroup
{
public:
  KSClass_QHButtonGroup( KSModule*, const char* name = "QHButtonGroup" );

protected:
  virtual KSScriptObject* createObject( KSClass* c );
};

class KSObject_QHButtonGroup : public KSObject_QButtonGroup
{
public:
  KSObject_QHButtonGroup( KSClass* );

  bool ksQHButtonGroup( KSContext& );

  KSValue::Ptr member( KSContext& context, const QString& name );
  bool setMember( KSContext& context, const QString& name, const KSValue::Ptr& v );

  bool inherits( const char* name ) { return ( strcmp( name, "KSObject_QHButtonGroup" ) == 0 || KSObject_QButtonGroup::inherits( name ) ); }

  void setObject( QObject* obj );
};

#endif
