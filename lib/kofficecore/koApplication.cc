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
#include "koMainWindow.h"
#include <klocale.h>
#include <kimageio.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kstddirs.h>
#include <qstringlist.h>
#include <kcmdlineargs.h>
#include <kdebug.h>

KoApplication::KoApplication()
	: KApplication()
{
    // Install the libkoffice* translations
    KGlobal::locale()->insertCatalogue("koffice");
    // Tell the iconloader about share/apps/koffice/icons
    KGlobal::instance()->iconLoader()->addAppDir("koffice");

    KImageIO::registerFormats();

    // Quit when last window closed
    connect( this, SIGNAL( lastWindowClosed() ), this, SLOT( quit() ) );
}

void KoApplication::start()
{
    // Find out about the mimetype which is natively supported
    // by this application.
    QCString nativeFormat = KoDocument::readNativeFormatMimeType();
    if ( nativeFormat.isEmpty() )
    {
        kdError(30003) << "Couldn't find the native MimeType in " << kapp->name() << "'s desktop file. Check your installation !" << endl;
        ::exit(1);
    }

    // Find the *.desktop file corresponding to the mime type
    KoDocumentEntry entry = KoDocumentEntry::queryByMimeType( nativeFormat );
    if ( entry.isEmpty() )
    {
        kdError(30003) << "Unknown KOffice MimeType " << nativeFormat << ". Check your installation !" << endl;
        ::exit(1);
    }

    // Get the command line arguments which we have to parse
    KCmdLineArgs *args= KCmdLineArgs::parsedArgs();
    int argsCount = args->count();

    // No argument -> create an empty document
    if (!argsCount) {
        KoDocument* doc = entry.createDoc( 0, "Document" );
        if ( !doc )
            ::exit(1);
        KoMainWindow* shell = doc->createShell();
        shell->show();
        if ( doc->initDoc() )
        {
          shell->setRootDocument( doc );
        }
        else
          ::exit(1);
    } else {
        // Loop through arguments

        short int n=0;
        for(int i=0; i < argsCount; i++ )
        {
	    // For now create an empty document
            KoDocument* doc = entry.createDoc( 0 );
            if ( doc )
            {
                // show a shell asap
                KoMainWindow* shell = doc->createShell();
                shell->show();
		// now try to load
                if ( doc->openURL( args->url(i) ) )
	        {
		    shell->setRootDocument( doc );
		    n++;
                } else {
		    // .... if failed
		    delete shell;
                }
            }
        }
        if (n == 0) // no doc, all URLs were malformed
          ::exit(1);
    }

    args->clear();
    // not calling this before since the program will quit there.
}

KoApplication::~KoApplication()
{
}

#include "koApplication.moc"
