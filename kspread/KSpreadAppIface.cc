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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.

*/

#include "KSpreadAppIface.h"

#include "kspread_doc.h"

#include <dcopclient.h>
#include <kapplication.h>
//Added by qt3to4:
#include <Q3ValueList>

using namespace KSpread;

AppIface::AppIface()
    : DCOPObject( "Application" )
{
}

DCOPRef AppIface::createDoc()
{
    Doc* doc = new Doc();
    doc->initDoc(KoDocument::InitDocFileNew);

    return DCOPRef( kapp->dcopClient()->appId(), doc->dcopObject()->objId() );
}

DCOPRef AppIface::createDoc( const QString& name )
{
    // ######### Torben: Check for duplicate names here
    Doc* doc = new Doc( 0, name.toLatin1() );
    doc->initDoc( KoDocument::InitDocEmpty, 0 );

    return DCOPRef( kapp->dcopClient()->appId(), doc->dcopObject()->objId() );
}

QMap<QString,DCOPRef> AppIface::documents()
{
    QMap<QString,DCOPRef> map;

    QList<Doc*> lst = Doc::documents();
    Q3ValueListIterator<Doc*> it = lst.begin();
    for( ; it != lst.end(); ++it )
    {
	map[ QString( (*it)->name() ) ] =
	 DCOPRef( kapp->dcopClient()->appId(), (*it)->dcopObject()->objId() );
    }

    return map;
}

DCOPRef AppIface::document( const QString& name )
{
    QList<Doc*> lst = Doc::documents();
    Q3ValueListIterator<Doc*> it = lst.begin();
    for( ; it != lst.end(); ++it )
	if ( name == (*it)->name() )
	    return DCOPRef( kapp->dcopClient()->appId(), (*it)->dcopObject()->objId() );

    return DCOPRef();
}

