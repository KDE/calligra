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
#include "koQueryTypes.h"
#include "koDocument.h"
#include "shell.h"
#include <klocale.h>
#include <kimgio.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <qstringlist.h>
#include <qdir.h>

KoApplication::KoApplication(int &argc, char **argv, const QCString& rAppName, const QCString& rNativeMimeType)
    : KApplication(argc, argv, rAppName)
    , m_params( argc, argv )
    , m_nativeMimeType( rNativeMimeType )
{
    KGlobal::locale()->insertCatalogue("koffice");
    KGlobal::dirs()->addResourceType("toolbar", KStandardDirs::kde_default("data") + "/koffice/toolbar/");
    KGlobal::dirs()->addResourceType("toolbar", KStandardDirs::kde_default("data") + "/koffice/pics/");

    kimgioRegister();
}

void KoApplication::start()
{
    KoDocumentEntry entry = KoDocumentEntry::queryByMimeType( m_nativeMimeType );

    ASSERT( !entry.isEmpty() );

    QStringList open;
    // Parse command line parameters
    for( uint i = 0; i < m_params.count(); i++ )
        if( m_params.get( i ).left( 1 ) != "-" )
            open.append( m_params.get( i ) );

    // No argument
    if ( open.isEmpty() ) {
        KoDocument* doc = entry.createDoc( 0, "Document" );
        if ( doc && ( doc->initDoc() ) )
        {
          Shell* shell = doc->createShell();
          shell->show();
          // setMainWidget( shell ); // probably bad idea, says Torben...
        }
        else
          ::exit(1);
    } else {
        // Loop through arguments
        QStringList::Iterator it = open.begin();
        int n = 0;
        for( ; it != open.end(); ++it )
        {
            KoDocument* doc = entry.createDoc( 0 );
            KURL url( QDir::currentDirPath()+"/", *it ); // allow URLs relative to current dir
            if ( doc->loadFromURL( url ) )
            {
              Shell* shell = doc->createShell();
              shell->show();
              //setMainWidget( shell );
              n++;
            }
        }
        if (n == 0) // no doc, all URLs were malformed
          ::exit(1);
    }
}

KoApplication::~KoApplication()
{
}

#include "koApplication.moc"
