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

#include "mainwindow.h"
#include "part.h"
#include "kptdocuments.h"

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
#include <kparts/event.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kdeversion.h>
#include <kaboutdata.h>
#include <kxmlguifactory.h>
#include <kcomponentdata.h>
#include <kiconloader.h>
#include <ktoolinvocation.h>
#include <kservice.h>
#include <kio/netaccess.h>

#include <KoDocumentInfo.h>
#include <KoDocument.h>
#include <KoView.h>
#include <KoFilterManager.h>

//TODO: KoMainWindow ->KMainWindow
KPlatoWork_MainWindow::KPlatoWork_MainWindow( const KComponentData &instance )
    : KoMainWindow( instance )
{
    kDebug()<<this;

}


KPlatoWork_MainWindow::~KPlatoWork_MainWindow()
{
    kDebug();
}

bool KPlatoWork_MainWindow::loadWorkPackages( KoDocument *doc )
{
    return static_cast<KPlatoWork::Part*>( doc )->loadWorkPackages();
}

void KPlatoWork_MainWindow::sendMail()
{
    kDebug();
}

bool KPlatoWork_MainWindow::openDocumentInternal( const KUrl & url, KoDocument* )
{
    kDebug()<<url.url()<<rootDocument();
    // this is only called when opening a new file
    KoDocument *maindoc = rootDocument();
    return maindoc->openUrl( url );
}


void KPlatoWork_MainWindow::saveSettings()
{
}

QString KPlatoWork_MainWindow::configFile() const
{
  //return readConfigFile( KStandardDirs::locate( "data", "koshell/koshell_shell.rc" ) );
  return QString(); // use UI standards only for now
}


//called from slotFileSave(), slotFileSaveAs(), queryClose(), slotEmailFile()
bool KPlatoWork_MainWindow::saveDocument( bool saveas, bool silent )
{
    kDebug()<<saveas<<silent;
    KoDocument *doc = rootDocument();
    if ( doc == 0 ) {
        return true;
    }
    return static_cast<KPlatoWork::Part*>( doc )->saveWorkPackages( silent );
}


void KPlatoWork_MainWindow::createShellGUI( bool  )
{
    guiFactory()->addClient( m_client );
}


bool KPlatoWork_MainWindow::queryClose()
{
    KPlatoWork::Part *part = qobject_cast<KPlatoWork::Part*>( rootDocument() );
    if ( part == 0 ) {
        return true;
    }
    return part->queryClose();
}

void KPlatoWork_MainWindow::slotFileClose()
{
    if (queryClose()) {
    }
}

void KPlatoWork_MainWindow::updateCaption()
{
    qDebug() << "KPlatoWork_MainWindow::updateCaption()";
    if ( rootDocument() == 0 ) {
        return updateCaption(QString(), false);
    }
    QString caption;
    // Get caption from document info (title(), in about page)
    if (rootDocument()->documentInfo()) {
        caption = rootDocument()->documentInfo()->aboutInfo("title");
    }
    updateCaption(caption, rootDocument()->isModified());
}

void KPlatoWork_MainWindow::closeEvent(QCloseEvent *e)
{
    KMainWindow::closeEvent( e );
//     if (queryClose()) {
//         saveWindowSettings();
//         setRootDocument(0L);
//         if (!d->m_dockWidgetVisibilityMap.isEmpty()) { // re-enable dockers for persistency
//             foreach(QDockWidget* dockWidget, d->m_dockWidgetMap)
//                 dockWidget->setVisible(d->m_dockWidgetVisibilityMap.value(dockWidget));
//         }
//         KParts::MainWindow::closeEvent(e);
//     } else
//         e->setAccepted(false);
}

///////////////////
KPlatoWork_MainGUIClient::KPlatoWork_MainGUIClient( KPlatoWork_MainWindow *window ) : KXMLGUIClient()
{
    setXMLFile( "kplatowork_mainwindow.rc", true, true );
}

#include "mainwindow.moc"
