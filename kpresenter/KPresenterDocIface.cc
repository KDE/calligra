/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "KPresenterDocIface.h"

#include "kpresenter_doc.h"
#include "kpresenter_view.h"

#include <kapp.h>
#include <dcopclient.h>

KPresenterDocIface::KPresenterDocIface( KPresenterDoc *doc_ )
    : KoDocumentIface( doc_ )
{
   doc = doc_;
}

DCOPRef KPresenterDocIface::firstView()
{
    return DCOPRef( kapp->dcopClient()->appId(),
		    doc->getFirstView()->dcopObject()->objId() );
}

DCOPRef KPresenterDocIface::nextView()
{
    return DCOPRef( kapp->dcopClient()->appId(),
		    doc->getNextView()->dcopObject()->objId() );
}

int KPresenterDocIface::getNumObjects()
{
    return doc->objNums();
}

DCOPRef KPresenterDocIface::getObject( int num )
{
    return DCOPRef( kapp->dcopClient()->appId(),
		    doc->objectList()->at( num )->dcopObject()->objId() );
}

int KPresenterDocIface::getNumPages()
{
    return doc->getPageNums();
}

DCOPRef KPresenterDocIface::getPage( int num )
{
    return DCOPRef( kapp->dcopClient()->appId(),
		    doc->backgroundList()->at( num )->dcopObject()->objId() );
}

int KPresenterDocIface::getPageOfObj( int obj, double faktor )
{
    return doc->getPageOfObj( obj, 0, 0, faktor ) - 1;
}
