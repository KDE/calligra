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

#include "kscript_ext_qapplication.h"
#include "kscript_context.h"
#include "kscript_util.h"

#include <klocale.h>
#include <qapplication.h>

KSClass_QApplication::KSClass_QApplication( KSModule* m ) : KSScriptClass( m, "QApplication", 0 )
{
  nameSpace()->insert( "QApplication", new KSValue( (KSBuiltinMethod)&KSObject_QApplication::ksQApplication ) );
  nameSpace()->insert( "exec", new KSValue( (KSBuiltinMethod)&KSObject_QApplication::ksQApplication_exec ) );
  nameSpace()->insert( "delete", new KSValue( (KSBuiltinMethod)&KSObject_QApplication::ksQApplication_delete ) );
  nameSpace()->insert( "quit", new KSValue( (KSBuiltinMethod)&KSObject_QApplication::ksQApplication_quit ) );
}

KSScriptObject* KSClass_QApplication::createObject( KSClass* c )
{
  return new KSObject_QApplication( c );
}

KSObject_QApplication::KSObject_QApplication( KSClass* c ) : KS_Qt_Object( c )
{
}

bool KSObject_QApplication::ksQApplication( KSContext& context )
{
  qDebug("QApplication\n");

  if ( object() )
  {
    QString tmp( i18n("The constructor of the class QApplication was called twice.") );
    context.setException( new KSException( "ConstructorCalledTwice", tmp, -1 ) );
    return false;
  }

  if ( !KSUtil::checkArgumentsCount( context, 0, "QApplication::QApplication" ) )
      return false;

  Q_ASSERT( qApp );
  // We dont know the application object -> FALSE
  setObject( qApp, FALSE );

  return true;
}

bool KSObject_QApplication::ksQApplication_exec( KSContext& context )
{
  qDebug("QApplication::exec\n");

  if ( !object() )
  {
    QString tmp( i18n("The method QApplication::exec was called before the QApplication constructor or after the QApplication destructor") );
    context.setException( new KSException( "MethodCalledBeforeConstructor", tmp, -1 ) );
    return false;
  }

  if ( !KSUtil::checkArgumentsCount( context, 0, "QApplication::QApplication" ) )
      return false;

  QApplication* a = (QApplication*)object();
  a->exec();

  return true;
}

bool KSObject_QApplication::ksQApplication_delete( KSContext& context )
{
  qDebug("QApplication::delete\n");

  if ( !KSUtil::checkArgumentsCount( context, 0, "QApplication::QApplication" ) )
      return false;

  if ( !object() )
    return true;

  if ( hasOwnership() )
    delete object();
  setObject( 0 );

  return true;
}

bool KSObject_QApplication::ksQApplication_quit( KSContext& context )
{
  qDebug("QApplication::quit\n");

  if ( !KSUtil::checkArgumentsCount( context, 0, "QApplication::QApplication" ) )
      return false;

  if ( !object() )
    return true;

  QApplication *a = static_cast<QApplication *>( object() );
  a->quit();

  return true;
}
