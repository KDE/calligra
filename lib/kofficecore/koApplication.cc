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

#include "koApplication.h"

#include <klocale.h>
#include <kimgio.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <qstringlist.h>

KoApplication::KoApplication(int &argc, char **argv, const QCString& rAppName)
    : KApplication(argc, argv, rAppName)
    , m_params( argc, argv )
{
    KGlobal::locale()->insertCatalogue("koffice");
    KGlobal::dirs()->addResourceType("toolbar", KStandardDirs::kde_default("data") + "/koffice/toolbar/");
    KGlobal::dirs()->addResourceType("toolbar", KStandardDirs::kde_default("data") + "/koffice/pics/");

    kimgioRegister();

    // checking whether the app is started as a server
    // ###### Torben: Is that needed ?
    /*
    QStringList::Iterator it;
    if( m_params.find( "--server", "-s", true, it ) )
    {
	m_bWithGUI = false;
	m_params.del( it );
    } */
}

KoApplication::~KoApplication()
{
}

#include "koApplication.moc"
