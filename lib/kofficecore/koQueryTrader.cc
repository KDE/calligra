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

#include <klibloader.h>

#include "koQueryTypes.h"
#include "koDocument.h"
#include <ktrader.h>

#include <qstring.h>
#include <qstringlist.h>
#include <qmessagebox.h>

#include <klocale.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kservice.h>
#include <kdebug.h>

/**
 * Port from KOffice Trader to KTrader/KActivator (kded) by Simon Hausmann
 * (c) 1999 Simon Hausmann <hausmann@kde.org>
 */

/*******************************************************************
 *
 * KoComponentEntry
 *
 *******************************************************************/

KoComponentEntry::KoComponentEntry( const KoComponentEntry& e )
{
  operator=( e );
}

const KoComponentEntry& KoComponentEntry::operator=( const KoComponentEntry& e )
{
  comment = e.comment;
  name = e.name;
  libname = e.libname;
  miniIcon = e.miniIcon;
  icon = e.icon;

  return *this;
}

KoComponentEntry::~KoComponentEntry()
{
}

/*******************************************************************
 *
 * koParseComponentProperties
 *
 *******************************************************************/

static KoComponentEntry koParseComponentProperties( KService::Ptr service )
{
  KoComponentEntry e;

  e.name = service->name();
  e.comment = service->comment();
  e.libname = service->library();

  QStringList lst = KGlobal::dirs()->resourceDirs("icon");
  QStringList::ConstIterator it = lst.begin();
  while (!e.icon.load( *it + "/" + service->icon() ) && it != lst.end() )
    it++;

  lst = KGlobal::dirs()->resourceDirs("mini");
  it = lst.begin();
  while (!e.miniIcon.load( *it + "/" + service->icon() ) && it != lst.end() )
    it++;

  return e;
}

/*******************************************************************
 *
 * KoDocumentEntry
 *
 *******************************************************************/

KoDocumentEntry::KoDocumentEntry( const KoDocumentEntry& _e ) : KoComponentEntry( _e )
{
  mimeTypes = _e.mimeTypes;
}

KoDocumentEntry::KoDocumentEntry( const KoComponentEntry& _e ) : KoComponentEntry( _e )
{
}

const KoDocumentEntry& KoDocumentEntry::operator=( const KoDocumentEntry& e )
{
    KoComponentEntry::operator=( e );
    mimeTypes = e.mimeTypes;
    return *this;
}

KoDocument* KoDocumentEntry::createDoc( KoDocument* parent, const char* name )
{
    KLibFactory* factory = KLibLoader::self()->factory( libname );

    if( !factory )
	return 0;

    QObject* obj = factory->create( KLibFactory::KofficeDocument, parent, name );
    if ( !obj || !obj->inherits( "KoDocument" ) )
    {
	delete obj;
	return 0;
    }

    return (KoDocument*)obj;
}

KoDocumentEntry KoDocumentEntry::queryByMimeType( const char *mimetype )
{
    QString constr( "'%1' in ServiceTypes" );
    constr = constr.arg( mimetype );

    QValueList<KoDocumentEntry> vec = query( constr );
    if ( vec.isEmpty() )
	return KoDocumentEntry();

    return vec[0];
}

QValueList<KoDocumentEntry> KoDocumentEntry::query( const char *_constr, int /*_count*/ )
{
  QValueList<KoDocumentEntry> lst;

  KTrader *trader = KTrader::self();

  if ( !_constr )
    _constr = "";

  // Query the trader
  KTrader::OfferList offers = trader->query( "KOfficePart", _constr );

  KTrader::OfferList::ConstIterator it = offers.begin();
  unsigned int max = offers.count();
  for( unsigned int i = 0; i < max; i++ )
  {
    // Parse the service
    KoDocumentEntry d( koParseComponentProperties( *it ) );

    //HACK
    d.mimeTypes = (*it)->serviceTypes();

    // Append converted offer
    lst.append( d );
    // Next service
    it++;
  }

  return lst;
}

/*******************************************************************
 *
 * koParseFilterProperties
 *
 *******************************************************************/

static KoFilterEntry koParseFilterProperties( KService::Ptr service )
{
  KoFilterEntry e( koParseComponentProperties( service ) );

  e.import = service->property( "Import" )->toString();
  e.importDescription = service->property( "ImportDescription" )->toString();
  e.export_ = service->property( "Export" )->toString();
  e.exportDescription = service->property( "ExportDescription" )->toString();

  return e;
}

/*******************************************************************
 *
 * KoFilterEntry
 *
 *******************************************************************/

KoFilterEntry::KoFilterEntry( const KoFilterEntry& e ) : KoComponentEntry( e )
{
  import = e.import;
  importDescription = e.importDescription;
  export_ = e.export_;
  exportDescription = e.exportDescription;
}

KoFilterEntry::KoFilterEntry( const KoComponentEntry& _e ) : KoComponentEntry( _e )
{
}

QValueList<KoFilterEntry> KoFilterEntry::query( const char *_constr, int /*_count*/ )
{
  kDebugInfo( 30003, "KoFilterEntry::query( %s, <ignored> )", _constr );
  QValueList<KoFilterEntry> lst;

  KTrader *trader = KTrader::self();

  KTrader::OfferList offers = trader->query( "KOfficeFilter", _constr );

  KTrader::OfferList::ConstIterator it = offers.begin();
  unsigned int max = offers.count();
  kDebugInfo( 30003, "Query returned %d offers\n", max);
  for( unsigned int i = 0; i < max; i++ )
  {
    KoFilterEntry f( koParseFilterProperties( *it ) );

    // Append converted offer
    lst.append( f );
    // Next service
    it++;
  }

  return lst;
}

KoFilter* KoFilterEntry::createFilter( QObject* parent, const char* name )
{
    KLibFactory* factory = KLibLoader::self()->factory( libname );

    if( !factory )
	return 0;

    QObject* obj = factory->create( parent, name, "KoFilter" );
    if ( !obj || !obj->inherits( "KoFilter" ) )
    {
	delete obj;
	return 0;
    }

    return (KoFilter*)obj;
}


/*******************************************************************
 *
 * koParseFilterDialogProperties
 *
 *******************************************************************/

static KoFilterDialogEntry koParseFilterDialogProperties( KService::Ptr service )
{
  return KoFilterDialogEntry ( koParseComponentProperties( service ) );
}

/*******************************************************************
 *
 * KoFilterDialogEntry
 *
 *******************************************************************/

KoFilterDialogEntry::KoFilterDialogEntry( const KoFilterDialogEntry& e ) : KoComponentEntry( e )
{
}

KoFilterDialogEntry::KoFilterDialogEntry( const KoComponentEntry& _e ) : KoComponentEntry( _e )
{
}

QValueList<KoFilterDialogEntry> KoFilterDialogEntry::query( const char *_constr, int /*_count*/ )
{
  kDebugInfo( 30003, "KoFilterDialogEntry::query( %s, <ignored> )", _constr );
  QValueList<KoFilterDialogEntry> lst;

  KTrader *trader = KTrader::self();

  KTrader::OfferList offers = trader->query( "KOfficeFilterDialog", _constr );

  KTrader::OfferList::ConstIterator it = offers.begin();
  unsigned int max = offers.count();
  kDebugInfo( 30003, "Query returned %d offers\n", max);
  for( unsigned int i = 0; i < max; i++ )
  {
    KoFilterDialogEntry f( koParseFilterDialogProperties( *it ) );

    // Append converted offer
    lst.append( f );
    // Next service
    it++;
  }

  return lst;
}

KoFilterDialog* KoFilterDialogEntry::createFilterDialog( QObject* parent, const char* name )
{
    KLibFactory* factory = KLibLoader::self()->factory( libname );

    if( !factory )
	return 0;

    QObject* obj = factory->create( parent, name, "KoFilterDialog" );
    if ( !obj || !obj->inherits( "KoFilterDialog" ) )
    {
	delete obj;
	return 0;
    }

    return (KoFilterDialog*)obj;
}

/*******************************************************************
 *
 * koParseToolProperties
 *
 *******************************************************************/

static KoToolEntry koParseToolProperties( KService::Ptr service )
{
    KoToolEntry e( koParseComponentProperties( service ) );

    QStringList mimeTypes = service->property( "MimeTypes" )->toString();
    QStringList commands = service->property( "Commands" )->toString();
    QStringList commandsI18N = service->property( "CommandsI18N" )->toString();

    return e;
}

/*******************************************************************
 *
 * KoToolEntry
 *
 *******************************************************************/

KoToolEntry::KoToolEntry( const KoToolEntry& e ) : KoComponentEntry( e )
{
    mimeTypes = e.mimeTypes;
    commandsI18N = e.commandsI18N;
    commands = e.commands;
}

KoToolEntry::KoToolEntry( const KoComponentEntry& _e ) : KoComponentEntry( _e )
{
}

QValueList<KoToolEntry> KoToolEntry::query( const QString &_mime_type )
{
  QValueList<KoToolEntry> lst;

  KTrader *trader = KTrader::self();

  KTrader::OfferList offers = trader->query( "KOfficeTool" );

  KTrader::OfferList::ConstIterator it = offers.begin();
  for (; it != offers.end(); ++it )
  {
    KoToolEntry t( koParseToolProperties( *it ) );

    if ( t.mimeTypes.find( _mime_type ) != t.mimeTypes.end() )
	lst.append( t );
  }

  return lst;
}
