/* This file is part of the KDE project
   Copyright (c) 2001 David Faure <faure@kde.org>

   $Id$

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
#include "KoMainWindowIface.h"

#include "koMainWindow.h"

#include <kapp.h>
#include <dcopclient.h>
#include <kdcopactionproxy.h>
#include <kaction.h>

KoMainWindowIface::KoMainWindowIface( KoMainWindow *mainwindow )
    : DCOPObject( mainwindow )
{
    m_pMainWindow = mainwindow;
    m_actionProxy = new KDCOPActionProxy( mainwindow->actionCollection(), this );
}

KoMainWindowIface::~KoMainWindowIface()
{
    delete m_actionProxy;
}

DCOPRef KoMainWindowIface::action( const QCString &name )
{
    return DCOPRef( kapp->dcopClient()->appId(), m_actionProxy->actionObjectId( name ) );
}

QCStringList KoMainWindowIface::actions()
{
    QCStringList res;
    QValueList<KAction *> lst = m_actionProxy->actions();
    QValueList<KAction *>::ConstIterator it = lst.begin();
    QValueList<KAction *>::ConstIterator end = lst.end();
    for (; it != end; ++it )
        res.append( (*it)->name() );

    return res;
}

QMap<QCString,DCOPRef> KoMainWindowIface::actionMap()
{
    return m_actionProxy->actionMap();
}
