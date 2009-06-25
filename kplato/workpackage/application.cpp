/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999 Simon Hausmann <hausmann@kde.org>
   Copyright (C) 2000-2005 David Faure <faure@kde.org>
   Copyright (C) 2005, 2006 Sven Lüppken <sven@kde.org>
   Copyright (C) 2008 Dag Andersen <kplato@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "application.h"
#include "part.h"
#include "mainwindow.h"

#include <QCursor>
#include <QSplitter>
#include <q3iconview.h>
#include <QLabel>
#include <q3vbox.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <Q3PtrList>

#include <assert.h>
#include <kicon.h>
//#include "koshellsettings.h"

#include <KoApplicationAdaptor.h>
#include <KoDocument.h>
#include <KoDocument.h>
#include <KoDocumentEntry.h>
#include <KoDocumentInfo.h>
#include <KoFilterManager.h>
#include <KoGlobal.h>
#include <KoView.h>

#include <kaboutdata.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kcomponentdata.h>
#include <kcomponentdata.h>
#include <kdebug.h>
#include <kdesktopfile.h>
#include <kdeversion.h>
#include <kfiledialog.h>
#include <kfileitem.h>
#include <kiconloader.h>
#include <kiconloader.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kmimetypetrader.h>
#include <kparts/partmanager.h>
#include <krecentdocument.h>
#include <kservice.h>
#include <kshortcutsdialog.h>
#include <kstandarddirs.h>
#include <ktemporaryfile.h>
#include <ktoolinvocation.h>
#include <kxmlguifactory.h>

KPlatoWork_Application::KPlatoWork_Application()
    : KUniqueApplication()
{
    // Tell the iconloader about share/apps/koffice/icons
/*    KIconLoader::global()->addAppDir("koffice");

    // Initialize all KOffice directories etc.
    KoGlobal::initialize();

    new KoApplicationAdaptor(this);
    QDBusConnection::sessionBus().registerObject("/application", this);*/
}

KPlatoWork_Application::~KPlatoWork_Application()
{
}

int KPlatoWork_Application::newInstance()
{
    kDebug()<<"starting------------------------";
    int status = KUniqueApplication::newInstance(); // bring up window (if any)
    if ( status != 0 ) {
        return status;
    }
    QList<KMainWindow*> lst = KMainWindow::memberList();
    qDebug()<<"newInstance() windows"<<lst.count();
    if ( lst.count() > 1 ) {
        return 1; // should never happen
    }
    // Find the *.desktop file corresponding to the kapp instance name
    KoDocumentEntry entry = KoDocumentEntry( KoDocument::readNativeService() );
    if ( entry.isEmpty() ) {
        kError() << KGlobal::mainComponent().componentName() << "part.desktop not found." << endl;
        kError() << "Run 'kde4-config --path services' to see which directories were searched, assuming kde startup had the same environment as your current shell." << endl;
        kError() << "Check your installation (did you install KOffice in a different prefix than KDE, without adding the prefix to /etc/kderc ?)" << endl;
        return false;
    }
    KoDocument* doc = 0;
    KPlatoWork_MainWindow *shell = 0;
    if ( lst.isEmpty() ) {
        QString errorMsg;
        doc = entry.createDoc( &errorMsg, 0 );
        if ( doc == 0 ) {
            if (!errorMsg.isEmpty()) {
                KMessageBox::error(0, errorMsg);
            }
            return 2;
        }
        shell = new KPlatoWork_MainWindow( doc->componentData() );
        shell->setRootDocument( doc );
        QObject::connect(doc, SIGNAL(sigProgress(int)), shell, SLOT(slotProgress(int)));
        shell->show();
        // open project storage...
        if ( ! shell->loadWorkPackages( doc ) ) {
            // failed!?
        }
    } else {
        shell = qobject_cast<KPlatoWork_MainWindow*>( lst.first() );
        if ( shell == 0 ) {
            KMessageBox::error(0, "Failed to (re)open KPlatoWork_MainWindow" );
            return 3;
        }
        doc = shell->rootDocument();
    }
    // Get the command line arguments which we have to parse
    KCmdLineArgs *args= KCmdLineArgs::parsedArgs();
    int argsCount = args->count();
    if ( argsCount > 0 ) {
        short int n=0; // number of documents open
        for(int i=0; i < argsCount; i++ ) {
            // For now create an empty document
            if ( ! shell->openDocument( args->url(i) ) ) {
                KMessageBox::error(0, "Failed to open document" );

            }
        }
    } else {
        shell->setRootDocument( doc );
    }
    args->clear();
    // not calling this before since the program will quit there.
    kDebug()<<"started------------------------";
    return true;
}

#include "application.moc"
