/* This file is part of the KDE project
   Copyright (C) 2000 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "KoApplicationIface.h"
#include "koApplication.h"
#include "koDocument.h"
#include "koView.h"
#include "koQueryTrader.h"
#include "KoDocumentIface.h"
#include <dcopclient.h>
#include <kdebug.h>
#include <stdlib.h>

KoApplicationIface::KoApplicationIface()
 : DCOPObject( "KoApplicationIface" )
{
}

KoApplicationIface::~KoApplicationIface()
{
}

DCOPRef KoApplicationIface::createDocument( const QString &nativeFormat )
{
    KoDocumentEntry entry = KoDocumentEntry::queryByMimeType( nativeFormat );
    if ( entry.isEmpty() )
    {
        kdError(30003) << "Unknown KOffice MimeType " << nativeFormat << ". Check your installation !" << endl;
        ::exit(1);
    }
    KoDocument* doc = entry.createDoc( 0 );
    return DCOPRef( kapp->dcopClient()->appId(), doc->dcopObject()->objId() );
}

QValueList<DCOPRef> KoApplicationIface::getDocuments()
{
    QValueList<DCOPRef> lst;
    QList<KoDocument> *documents = KoDocument::documentList();
    if ( documents )
    {
      QListIterator<KoDocument> it( *documents );
      for (; it.current(); ++it )
        lst.append( DCOPRef( kapp->dcopClient()->appId(), it.current()->dcopObject()->objId() ) );
    }
    return lst;
}

QValueList<DCOPRef> KoApplicationIface::getViews()
{
    QValueList<DCOPRef> lst;
    QList<KoDocument> *documents = KoDocument::documentList();
    if ( documents )
    {
      QListIterator<KoDocument> it( *documents );
      for (; it.current(); ++it )
      {
          QListIterator<KoView> itview( it.current()->views() );
          for ( ; itview.current(); ++itview )
              lst.append( DCOPRef( kapp->dcopClient()->appId(), itview.current()->dcopObject()->objId() ) );
      }
    }
    return lst;
}

