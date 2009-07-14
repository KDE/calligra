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
#include "view.h"

#include "kptdocuments.h"

#include <QCursor>
#include <QSplitter>
#include <QLabel>
#include <QWidget>
#include <QUndoStack>

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

#include <kaboutdata.h>
#include <kxmlguifactory.h>
#include <kcomponentdata.h>
#include <kiconloader.h>
#include <ktoolinvocation.h>
#include <kservice.h>
#include <kio/netaccess.h>
#include <kxmlguiwindow.h>

#include <KoDocumentInfo.h>
#include <KoDocument.h>
#include <KoView.h>
#include <KoFilterManager.h>

KPlatoWork_MainWindow::KPlatoWork_MainWindow()
    : KParts::MainWindow()
{
    kDebug()<<this;

    m_part = new KPlatoWork::Part( this, this );

    KStandardAction::quit(kapp, SLOT(quit()), actionCollection());
 
    KStandardAction::open(this, SLOT(slotFileOpen()), actionCollection());
    
    KStandardAction::save(this, SLOT(slotFileSave()), actionCollection());

    QAction *a = KStandardAction::undo(m_part->undoStack(), SLOT(undo()), actionCollection());
    a->setEnabled( false );
    connect( m_part->undoStack(), SIGNAL( canUndoChanged( bool ) ), a, SLOT( setEnabled( bool ) ) );

    a = KStandardAction::redo(m_part->undoStack(), SLOT(redo()), actionCollection());
    a->setEnabled( false );
    connect( m_part->undoStack(), SIGNAL( canRedoChanged( bool ) ), a, SLOT( setEnabled( bool ) ) );
    
    setupGUI( KXmlGuiWindow::Default, "kplatowork_mainwindow.rc" );

    setCentralWidget( m_part->widget() );
    createGUI( m_part );
    connect( m_part, SIGNAL( captionChanged( const QString&, bool ) ), SLOT( setCaption( const QString&, bool ) ) );
}


KPlatoWork_MainWindow::~KPlatoWork_MainWindow()
{
    kDebug();
}

bool KPlatoWork_MainWindow::openDocument(const KUrl & url)
{
    if (!KIO::NetAccess::exists(url, KIO::NetAccess::SourceSide, 0)) {
        KMessageBox::error(0L, i18n("The file %1 does not exist.", url.url()));
//        d->recent->removeUrl(url); //remove the file from the recent-opened-file-list
//        saveRecentFiles();
        return false;
    }
    return m_part->openUrl( url );
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
    KPlatoWork::Part *doc = rootDocument();
    if ( doc == 0 ) {
        return true;
    }
    return doc->saveWorkPackages( silent );
}


bool KPlatoWork_MainWindow::queryClose()
{
    KPlatoWork::Part *part = rootDocument();
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

void KPlatoWork_MainWindow::slotFileSave()
{
    saveDocument();
}

void KPlatoWork_MainWindow::slotFileOpen()
{
    QString file = KFileDialog::getOpenFileName( KUrl(), "*.kplatowork" );
    if ( ! file.isEmpty() ) {
        openDocument( file );
    }
}


#include "mainwindow.moc"
