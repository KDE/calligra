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

#include <dcopclient.h>
#include <koApplication.h>
#include <KoApplicationIface.h>
#include <koQueryTrader.h>
#include <koDocument.h>
#include <koMainWindow.h>
#include <klocale.h>
#include <kimageio.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <qstringlist.h>
#include <kcmdlineargs.h>
#include <kdebug.h>
#include <stdlib.h>

KoApplication::KoApplication()
        : KApplication()
{
    // Install the libkoffice* translations
    KGlobal::locale()->insertCatalogue("koffice");

    KImageIO::registerFormats();

    // Tell the iconloader about share/apps/koffice/icons
    KGlobal::iconLoader()->addAppDir("koffice");

    // Quit when last window closed
    // This shouldn't be needed anymore, KTMW does it (David)
    //connect( this, SIGNAL( lastWindowClosed() ), this, SLOT( quit() ) );

    // Prepare a DCOP interface
    dcopClient()->setDefaultObject( (new KoApplicationIface())->objId() );
}

bool KoApplication::start()
{
    // Find out about the mimetype which is natively supported
    // by this application.
    QCString nativeFormat = KoDocument::readNativeFormatMimeType();
    if ( nativeFormat.isEmpty() )
    {
        kdError(30003) << "Couldn't find the native MimeType in " << kapp->name() << "'s desktop file. Check your installation !" << endl;
        return false;
    }

    // Find the *.desktop file corresponding to the mime type
    KoDocumentEntry entry = KoDocumentEntry::queryByMimeType( nativeFormat );
    if ( entry.isEmpty() )
    {
        kdError(30003) << "Unknown KOffice MimeType " << nativeFormat << ". Check your installation !" << endl;
        return false;
    }

    // Get the command line arguments which we have to parse
    KCmdLineArgs *args= KCmdLineArgs::parsedArgs();
    int argsCount = args->count();

    // No argument -> create an empty document
    if (!argsCount) {
        KoDocument* doc = entry.createDoc( 0, "Document" );
        if ( !doc )
            return false;
//        KoMainWindow* shell = doc->createShell();
        KoMainWindow *shell = new KoMainWindow( doc->instance() );
        shell->show();
        QObject::connect(doc, SIGNAL(sigProgress(int)), shell, SLOT(slotProgress(int)));
        if ( doc->initDoc() )
            {
                shell->setRootDocument( doc );
            }
        else
            return false;
        QObject::disconnect(doc, SIGNAL(sigProgress(int)), shell, SLOT(slotProgress(int)));
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
//                KoMainWindow* shell = doc->createShell();
                KoMainWindow *shell = new KoMainWindow( doc->instance() );
                shell->show();
                // now try to load
                QObject::connect(doc, SIGNAL(sigProgress(int)), shell, SLOT(slotProgress(int)));
                if ( doc->openURL( args->url(i) ) ) {
                    shell->setRootDocument( doc );
                    n++;
                    QObject::disconnect(doc, SIGNAL(sigProgress(int)), shell, SLOT(slotProgress(int)));
                } else {
                    // .... if failed
                    delete shell;
                }
            }
        }
        if (n == 0) // no doc, all URLs were malformed
          return false;
    }

    args->clear();
    // not calling this before since the program will quit there.
	return true;
}

KoApplication::~KoApplication()
{
}

#include <koApplication.moc>
