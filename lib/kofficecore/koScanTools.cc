/* This file is part of the KDE libraries
    Copyright (C) 1998 Torben Weis <weis@kde.org>

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

#include "koScanTools.h"
#include <opIMR.h>

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#include <ksimpleconfig.h>
#include <klocale.h>
#include <kapp.h>
#include <ktrader.h>
#include <kactivator.h>
#include <kded_instance.h>

/**
 * Port to KTrader/KActivator (kded) by Simon Hausmann
 * (c) 1999 Simon Hausmann <hausmann@kde.org>
 */

//TODO: move all this stuff into koQueryTrader

QList<KoToolEntry> KoToolEntry::findTools( const QString &_mime_type )
{
  QList<KoToolEntry> lst;

  KTrader *trader = KdedInstance::self()->ktrader();
  KActivator *activator = KdedInstance::self()->kactivator();

  QString mime = _mime_type;

//  cerr << "mimetype = "<< mime.ascii() << endl;
//  cerr << "constr = " << QString("'%1' in ServiceTypes").arg( mime ).ascii() << endl;

//  KTrader::OfferList offers = trader->query( "KOfficeTool", QString("'%1' in ServiceTypes").arg( mime ) );
  KTrader::OfferList offers = trader->query( "KOfficeTool" );

  KTrader::OfferList::ConstIterator it = offers.begin();
  for (; it != offers.end(); ++it )
  {
    cerr << "tool offer : " << (*it)->name().ascii() << endl;

    QString name = (*it)->name();
    QString comment = (*it)->comment();
    QStringList mimeTypes = (*it)->serviceTypes();
    QStringList commands = (*it)->property( "Commands" )->stringListValue();
    QStringList commandsI18N = (*it)->property( "CommandsI18N" )->stringListValue();

    if ( mimeTypes.find( _mime_type ) == mimeTypes.end() )
      continue;

    //strip off tag
    QString repoId = *( (*it)->repoIds().begin() );
    QString tag = (*it)->name();
    int tagPos = repoId.findRev( "#" );
    if ( tagPos != -1 )
    {
      tag = repoId.mid( tagPos+1 );
      repoId.truncate( tagPos );
    }

    CORBA::Object_var obj = activator->activateService( name, repoId, tag );

    lst.append( new KoToolEntry( name, comment, mimeTypes, commands, commandsI18N, obj ) );
  }

  return lst;
}

KoToolEntry::KoToolEntry( const QString &_name, const QString &_comment, const QStringList& _mimes,
                          const QStringList& _commands, const QStringList& _commands_i18n,
			  CORBA::Object_ptr _ref )
{
  m_strName = _name;
  m_strComment = _comment;
  m_strlstMimeTypes = _mimes;
  m_strlstCommands = _commands;
  m_strlstCommandsI18N = _commands_i18n;
  m_vRef = CORBA::Object::_duplicate( _ref );
}

bool KoToolEntry::supports( const QString &_mime_type )
{
  return ( m_strlstMimeTypes.find( _mime_type ) != m_strlstMimeTypes.end() );
}

