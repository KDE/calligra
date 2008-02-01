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
#include <KoGlobal.h>
#include <KoQueryTrader.h>

#include <kcomponentdata.h>
#include <kmimetypetrader.h>

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kdesktopfile.h>
#include <ktemporaryfile.h>
#include <kfiledialog.h>
#include <kfileitem.h>
#include <klocale.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kshortcutsdialog.h>
#include <kstandarddirs.h>
#include <klibloader.h>
#include <kmenu.h>
#include <kservice.h>
#include <kmessagebox.h>
#include <krecentdocument.h>
#include <kparts/partmanager.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kdeversion.h>
#include <kaboutdata.h>
#include <kxmlguifactory.h>
#include <kcomponentdata.h>
#include <kiconloader.h>
#include <ktoolinvocation.h>

#include <KoQueryTrader.h>
#include <KoDocumentInfo.h>
#include <KoDocument.h>
#include <KoView.h>
#include <KoPartSelectDia.h>
#include <KoFilterManager.h>


KPlatoWork_Application::KPlatoWork_Application()
    : KoApplication()
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

// This gets called before entering KApplication::KApplication
// bool KPlatoWork_Application::initHack()
// {
//     KCmdLineOptions options;
//     options.add("print", ki18n("Only print and exit"));
//     options.add("template", ki18n("Open a new document with a template"));
//     options.add("dpi <dpiX,dpiY>", ki18n("Override display DPI"));
//     KCmdLineArgs::addCmdLineOptions( options, ki18n("KOffice"), "koffice", "kde" );
//     return true;
// }

bool KPlatoWork_Application::start()
{
    kDebug()<<"starting------------------------";
    // Find the *.desktop file corresponding to the kapp instance name
    KoDocumentEntry entry = KoDocumentEntry( KoDocument::readNativeService() );
    if ( entry.isEmpty() ) {
        kError() << KGlobal::mainComponent().componentName() << "part.desktop not found." << endl;
        kError() << "Run 'kde4-config --path services' to see which directories were searched, assuming kde startup had the same environment as your current shell." << endl;
        kError() << "Check your installation (did you install KOffice in a different prefix than KDE, without adding the prefix to /etc/kderc ?)" << endl;
        return false;
    }

    // Get the command line arguments which we have to parse
    KCmdLineArgs *args= KCmdLineArgs::parsedArgs();
    int argsCount = args->count();

    KCmdLineArgs *koargs = KCmdLineArgs::parsedArgs("koffice");
    QString dpiValues = koargs->getOption( "dpi" );
    if ( !dpiValues.isEmpty() ) {
        int sep = dpiValues.indexOf( QRegExp( "[x, ]" ) );
        int dpiX;
        int dpiY = 0;
        bool ok = true;
        if ( sep != -1 ) {
            dpiY = dpiValues.mid( sep+1 ).toInt( &ok );
            dpiValues.truncate( sep );
        }
        if ( ok ) {
            dpiX = dpiValues.toInt( &ok );
            if ( ok ) {
                if ( !dpiY ) dpiY = dpiX;
                KoGlobal::setDPI( dpiX, dpiY );
            }
        }
    }
    // No argument -> create an empty document
    if ( !argsCount ) {
        QString errorMsg;
        KoDocument* doc = entry.createDoc( &errorMsg );
        if ( !doc ) {
            if ( !errorMsg.isEmpty() )
                KMessageBox::error( 0, errorMsg );
            return false;
        }
        KPlatoWork_MainWindow *shell = new KPlatoWork_MainWindow( doc->componentData() );
        kDebug()<<shell;
        shell->show();
        QObject::connect(doc, SIGNAL(sigProgress(int)), shell, SLOT(slotProgress(int)));
        // for initDoc to fill in the recent docs list
        // and for KoDocument::slotStarted
        doc->addShell( shell );

        if ( doc->checkAutoSaveFile() ) {
            shell->setRootDocument( doc );
        } else {
            doc->showStartUpWidget( shell );
        }

        // FIXME This needs to be moved someplace else
        QObject::disconnect(doc, SIGNAL(sigProgress(int)), shell, SLOT(slotProgress(int)));
    } else {
        bool print = koargs->isSet("print");
        bool doTemplate = koargs->isSet("template");
        koargs->clear();

        // Loop through arguments

        short int n=0; // number of documents open
        short int nPrinted = 0;
        for(int i=0; i < argsCount; i++ ) {
            // For now create an empty document
            QString errorMsg;
            KoDocument* doc = entry.createDoc( &errorMsg, 0 );
            if ( doc ) {
                // show a shell asap
                KPlatoWork_MainWindow *shell = new KPlatoWork_MainWindow( doc->componentData() );
                kDebug()<<shell;
                if (!print)
                    shell->show();
                // are we just trying to open a template?
                if ( doTemplate ) {
                    QStringList paths;
                    if ( args->url(i).isLocalFile() && QFile::exists(args->url(i).path()) ) {
                        paths << QString(args->url(i).path());
                        kDebug() <<"using full path...";
                    } else {
                        QString desktopName(args->arg(i));
                        QString appName = KGlobal::mainComponent().componentName();

                        paths = KGlobal::dirs()->findAllResources("data", appName +"/templates/*/" + desktopName );
                        if ( paths.isEmpty()) {
                            paths = KGlobal::dirs()->findAllResources("data", appName +"/templates/" + desktopName );
                        }
                        if ( paths.isEmpty()) {
                            KMessageBox::error(0L, i18n("No template found for: %1 ", desktopName) );
                            delete shell;
                        } else if ( paths.count() > 1 ) {
                            KMessageBox::error(0L,  i18n("Too many templates found for: %1", desktopName) );
                            delete shell;
                        }
                    }
                    if ( !paths.isEmpty() ) {
                        KUrl templateBase;
                        templateBase.setPath(paths[0]);
                        KDesktopFile templateInfo(paths[0]);

                        QString templateName = templateInfo.readUrl();
                        KUrl templateURL;
                        templateURL.setPath( templateBase.directory() + "/" + templateName );
                        if ( shell->openDocument(doc, templateURL )) {
                            doc->resetURL();
                            doc->setEmpty();
                            doc->setTitleModified();
                            kDebug() <<"Template loaded...";
                            n++;
                        } else {
                            KMessageBox::error(0L, i18n("Template %1 failed to load.", templateURL.prettyUrl()) );
                            delete shell;
                        }
                    }
                // now try to load
                } else if ( shell->openDocument( doc, args->url(i) ) ) {
                    if ( print ) {
                        shell->slotFilePrint();
                    // delete shell; done by ~KoDocument
                        nPrinted++;
                    } else {
                    // Normal case, success
                        n++;
                    }
                } else {
                // .... if failed
                // delete doc; done by openDocument
                // delete shell; done by ~KoDocument
                }
            }
        }
        if ( print )
            return nPrinted > 0;
        if (n == 0) // no doc, e.g. all URLs were malformed
            return false;
    }
    args->clear();
    // not calling this before since the program will quit there.
    kDebug()<<"started------------------------";
    return true;
}

#include "application.moc"
