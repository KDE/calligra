/* This file is part of the KDE project
   
   Copyright 2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2003 David Faure <faure@kde.org>
   Copyright 2001 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2001 Laurent Montel <montel@kde.org>
   Copyright 2002 Werner Trobin <trobin@kde.org>
   Copyright 1999 Torben Weis <weis@kde.org>

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

#include "KSpreadAppIface.h"

#include "kspread_doc.h"

#include <dcopclient.h>
#include <kapplication.h>

KSpreadAppIface::KSpreadAppIface()
    : DCOPObject( "Application" )
{
}

DCOPRef KSpreadAppIface::createDoc()
{
    KSpreadDoc* doc = new KSpreadDoc();
    doc->initDoc(KoDocument::InitDocFileNew);

    return DCOPRef( kapp->dcopClient()->appId(), doc->dcopObject()->objId() );
}

DCOPRef KSpreadAppIface::createDoc( const QString& name )
{
    // ######### Torben: Check for duplicate names here
    KSpreadDoc* doc = new KSpreadDoc( 0, name.latin1() );
    doc->initDoc( KoDocument::InitDocEmpty, 0 );

    return DCOPRef( kapp->dcopClient()->appId(), doc->dcopObject()->objId() );
}

QMap<QString,DCOPRef> KSpreadAppIface::documents()
{
    QMap<QString,DCOPRef> map;

    QValueList<KSpreadDoc*> lst = KSpreadDoc::documents();
    QValueListIterator<KSpreadDoc*> it = lst.begin();
    for( ; it != lst.end(); ++it )
    {
	map[ QString( (*it)->name() ) ] =
	 DCOPRef( kapp->dcopClient()->appId(), (*it)->dcopObject()->objId() );
    }

    return map;
}

DCOPRef KSpreadAppIface::document( const QString& name )
{
    QValueList<KSpreadDoc*> lst = KSpreadDoc::documents();
    QValueListIterator<KSpreadDoc*> it = lst.begin();
    for( ; it != lst.end(); ++it )
	if ( name == (*it)->name() )
	    return DCOPRef( kapp->dcopClient()->appId(), (*it)->dcopObject()->objId() );

    return DCOPRef();
}

