/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 
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

#include "koQueryTrader.h"
#include "koIMR.h"
#include <opIMR.h>
#include <torben.h>

#include <qstring.h>
#include <qstrlist.h>

#include <kapp.h>
#include <opApplication.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kded_instance.h>
#include <ktrader.h>
#include <kactivator.h>

#include <string>
#include <list>
#include <string.h>
#include <sys/stat.h>

/**
 * Port from KOffice Trader to KTrader/KActivator (kded) by Simon Hausmann
 * (c) 1999 Simon Hausmann <hausmann@kde.org>
 */

/*******************************************************************
 *
 * Component
 *
 *******************************************************************/

KoComponentEntry koParseComponentProperties( KTrader::ServicePtr service )
{
  KoComponentEntry e;

  e.name = service->name();
  e.comment = service->comment();
  e.exec = service->CORBAExec();
  e.activationMode = service->activationMode();

  QStringList lst = KGlobal::dirs()->getResourceDirs("icon");
  QStringList::ConstIterator it = lst.begin();
  while (!e.icon.load( *it + "/" + service->icon() ) && it != lst.end() )
    it++;
  
  lst = KGlobal::dirs()->getResourceDirs("mini");
  it = lst.begin();
  while (!e.miniIcon.load( *it + "/" + service->icon() ) && it != lst.end() )
    it++;
  
  e.repoID = service->repoIds();

  return e;
}

/*******************************************************************
 *
 * Document
 *
 *******************************************************************/
  
vector<KoDocumentEntry> koQueryDocuments( const char *_constr, int _count )
{
  vector<KoDocumentEntry> lst;

  KTrader *trader = KdedInstance::self()->ktrader();
  KActivator *activator = KdedInstance::self()->kactivator();

  KTrader::OfferList offers = trader->query( "KOfficeDocument", _constr );

  lst.reserve( offers.count() );
  
  KTrader::OfferList::ConstIterator it = offers.begin();
  unsigned int max = offers.count();
  for( unsigned int i = 0; i < max; i++ )
  {
    KoDocumentEntry d( koParseComponentProperties( *it ) );

    //HACK
    d.mimeTypes = (*it)->serviceTypes();

    //strip off tag
    QString repoId = (*it)->repoIds().getFirst();
    QString tag = (*it)->name();
    int tagPos = repoId.findRev( '#' );
    if ( tagPos != -1 )
    {
      tag = repoId.mid( tagPos+1 );
      repoId.truncate( tagPos );
    }      
      
    d.reference = activator->activateService( (*it)->name(), repoId, tag );
        
    lst.push_back( d );
    
    it++;
  }
  
  return lst;
}

KoDocumentEntry::KoDocumentEntry( const KoComponentEntry& _e )
{
  comment = _e.comment;
  name = _e.name;
  exec = _e.exec;
  activationMode = _e.activationMode;
  repoID = _e.repoID;
  miniIcon = _e.miniIcon;
  icon = _e.icon;
}


/*******************************************************************
 *
 * Filters
 *
 *******************************************************************/

KoFilterEntry koParseFilterProperties( KTrader::ServicePtr service )
{
  KoFilterEntry e( koParseComponentProperties( service ) );

  e.import = service->property( "Import" )->stringValue();
  e.importDescription = service->property( "ImportDescription" )->stringValue();
  e.export = service->property( "Export" )->stringValue();
  e.exportDescription = service->property( "ExportDescription" )->stringValue();

  return e;
}

vector<KoFilterEntry> koQueryFilters( const char *_constr, int _count )
{
  vector<KoFilterEntry> lst;

  KTrader *trader = KdedInstance::self()->ktrader();
  KActivator *activator = KdedInstance::self()->kactivator();

  KTrader::OfferList offers = trader->query( "KOfficeFilter", _constr );
  
  lst.reserve( offers.count() );
  
  KTrader::OfferList::ConstIterator it = offers.begin();
  unsigned int max = offers.count();
  for( unsigned int i = 0; i < max; i++ )
  {
    KoFilterEntry f( koParseFilterProperties( *it ) );
    
    //strip off tag
    QString repoId = (*it)->repoIds().getFirst();
    QString tag = (*it)->name();
    int tagPos = repoId.findRev( '#' );
    if ( tagPos != -1 )
    {
      tag = repoId.mid( tagPos+1 );
      repoId.truncate( tagPos );
    }      
    
    f.reference = activator->activateService( (*it)->name(), repoId, tag );
    
    lst.push_back( f );
    
    it++;
  }
  
  return lst;
}

KoFilterEntry::KoFilterEntry( const KoComponentEntry& _e )
{
  comment = _e.comment;
  name = _e.name;
  exec = _e.exec;
  activationMode = _e.activationMode;
  repoID = _e.repoID;
  miniIcon = _e.miniIcon;
  icon = _e.icon;
}

