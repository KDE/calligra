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
#include <kparts/factory.h>

#include <koQueryTrader.h>
#include <koDocument.h>
#include <ktrader.h>
#include <kservicetype.h>

#include <qstringlist.h>
#include <qfile.h>

#include <klocale.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kservice.h>
#include <kdebug.h>

/**
 * Port from KOffice Trader to KTrader/KActivator (kded) by Simon Hausmann
 * (c) 1999 Simon Hausmann <hausmann@kde.org>
 * Port to KService and simplifications by David Faure <faure@kde.org>
 */


/*******************************************************************
 *
 * KoDocumentEntry
 *
 *******************************************************************/

KoDocumentEntry::KoDocumentEntry( KService::Ptr service )
  : m_service( service )
{
}

KoDocument* KoDocumentEntry::createDoc( KoDocument* parent, const char* name )
{
    KLibFactory* factory = KLibLoader::self()->factory( QFile::encodeName(m_service->library()) );

    if( !factory )
        return 0;

    QObject* obj;
    if ( factory->inherits( "KParts::Factory" ) )
      obj = static_cast<KParts::Factory *>(factory)->createPart( 0L, "", parent, name, "KoDocument" );
    else {
      kdWarning(30003) << "factory doesn't inherit KParts::Factory ! It is a " << factory->className() << endl; // This shouldn't happen...
      obj = factory->create( parent, name, "KoDocument" );
    }

    if ( !obj || !obj->inherits( "KoDocument" ) )
    {
        delete obj;
        return 0;
    }

    return static_cast<KoDocument*>(obj);
}

KoDocumentEntry KoDocumentEntry::queryByMimeType( const QString & mimetype )
{
    QString constr = QString::fromLatin1( "[X-KDE-NativeMimeType] == '%1'" ).arg( mimetype );

    QValueList<KoDocumentEntry> vec = query( constr );
    if ( vec.isEmpty() )
    {
        kdWarning(30003) << "Got no results with " << constr << endl;
        // Fallback to the old way (which was probably wrong, but better be safe)
        QString constr = QString::fromLatin1( "'%1' in ServiceTypes" ).arg( mimetype );
        vec = query( constr );
        if ( vec.isEmpty() )
        {
            // Still no match. Either the mimetype itself is unknown, or we have no service for it.
            // Help the user debugging stuff by providing some more diagnostics
            if ( KServiceType::serviceType( mimetype ) == 0L )
            {
                kdError(30003) << "Unknown KOffice MimeType " << mimetype << "." << endl;
                kdError(30003) << "Check your installation (for instance, run 'kde-config --path mime' and check the result)." << endl;
            } else
            {
                kdError(30003) << "Found no KOffice part able to handle " << mimetype << "!" << endl;
                kdError(30003) << "Check your installation (does the desktop file have X-KDE-NativeMimeType and KOfficePart, did you install KOffice in a different prefix than KDE, without adding the prefix to /etc/kderc ?)" << endl;
            }
            return KoDocumentEntry();
        }
    }

    return vec[0];
}

QValueList<KoDocumentEntry> KoDocumentEntry::query( const QString & _constr )
{
  QValueList<KoDocumentEntry> lst;

  // Query the trader
  KTrader::OfferList offers = KTrader::self()->query( "KOfficePart", _constr );

  KTrader::OfferList::ConstIterator it = offers.begin();
  unsigned int max = offers.count();
  if ( max > 1 && !_constr.isEmpty() )
    kdWarning(30003) << "KoDocumentEntry::query " << _constr << " got " << max << " offers!" << endl;
  for( unsigned int i = 0; i < max; i++ )
  {
    kdDebug(3003) << "   desktopEntryPath=" << (*it)->desktopEntryPath()
                  << "   library=" << (*it)->library() << endl;
    // Parse the service
    KoDocumentEntry d( *it );

    // Append converted offer
    lst.append( d );
    // Next service
    ++it;
  }

  return lst;
}

/*******************************************************************
 *
 * KoFilterEntry
 *
 *******************************************************************/

KoFilterEntry::KoFilterEntry( KService::Ptr service )
  : m_service( service )
{
  import = service->property( "Import" ).toStringList();
  export_ = service->property( "Export" ).toStringList();
  implemented = service->property( "Implemented" ).toString();
}

QValueList<KoFilterEntry> KoFilterEntry::query( const QString & _constr )
{
    kdDebug(30003) << "KoFilterEntry::query( " << _constr << ")" << endl;
  QValueList<KoFilterEntry> lst;

  KTrader::OfferList offers = KTrader::self()->query( "KOfficeFilter", _constr );

  KTrader::OfferList::ConstIterator it = offers.begin();
  unsigned int max = offers.count();
  kdDebug(30003) << "Query returned " << max << " offers" << endl;
  for( unsigned int i = 0; i < max; i++ )
  {
    kdDebug(3003) << "   desktopEntryPath=" << (*it)->desktopEntryPath()
                  << "   library=" << (*it)->library() << endl;
    KoFilterEntry f( *it );
    // Append converted offer
    lst.append( f );
    // Next service
    it++;
  }

  return lst;
}

KoFilter* KoFilterEntry::createFilter( QObject* parent, const char* name )
{
    KLibFactory* factory = KLibLoader::self()->factory( QFile::encodeName(m_service->library()) );

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
 * KoFilterDialogEntry
 *
 *******************************************************************/

KoFilterDialogEntry::KoFilterDialogEntry( KService::Ptr service )
  : m_service( service )
{
  import = service->property( "Import" ).toStringList();
  export_ = service->property( "Export" ).toStringList();
}

QValueList<KoFilterDialogEntry> KoFilterDialogEntry::query( const QString & _constr )
{
  kdDebug(30003) << "KoFilterDialogEntry::query( " << _constr << ")" << endl;
  QValueList<KoFilterDialogEntry> lst;

  KTrader::OfferList offers = KTrader::self()->query( "KOfficeFilterDialog", _constr );

  KTrader::OfferList::ConstIterator it = offers.begin();
  unsigned int max = offers.count();
  kdDebug(30003) << "Query returned " << max << " offers" << endl;
  for( unsigned int i = 0; i < max; i++ )
  {
    KoFilterDialogEntry f( *it );
    // Append converted offer
    lst.append( f );
    // Next service
    it++;
  }

  return lst;
}

KoFilterDialog* KoFilterDialogEntry::createFilterDialog( QObject* parent, const char* name )
{
    KLibFactory* factory = KLibLoader::self()->factory( QFile::encodeName(m_service->library()) );

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
