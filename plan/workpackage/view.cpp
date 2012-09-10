/* This file is part of the KDE project
  Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
  Copyright (C) 2002 - 2009, 2011, 2012 Dag Andersen <danders@get2net.dk>

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
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*/

#include "view.h"
#include "mainwindow.h"
#include "taskworkpackageview.h"
#include "workpackage.h"
#include "packagesettings.h"
#include "taskcompletiondialog.h"
#include "planworksettings.h"
#include "kpttaskeditor.h"
#include "kpttaskdescriptiondialog.h"
#include "kptcommonstrings.h"

#include "KoDocumentInfo.h"
#include <KoMainWindow.h>
#include <KoToolManager.h>

#include <QApplication>
#include <QLabel>
#include <QString>
#include <QSize>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QPrinter>
#include <QPrintDialog>
#include <QDomDocument>
#include <QPointer>

#include <kactionmenu.h>
#include <kmenu.h>
#include <kstandardaction.h>
#include <klocale.h>
#include <kdebug.h>
#include <ktoolbar.h>
#include <kstandardshortcut.h>
#include <kaccelgen.h>

#include <kstatusbar.h>
#include <kxmlguifactory.h>
#include <kxmlguiclient.h>
#include <kstandarddirs.h>
#include <kdesktopfile.h>
#include <KToolInvocation>
#include <ktoggleaction.h>
#include <kfiledialog.h>
#include <kparts/event.h>
#include <kparts/partmanager.h>
#include <kparts/componentfactory.h>
#include <KActionCollection>
#include <KTemporaryFile>

#include <kmessagebox.h>
#include <kvbox.h>
#include <kmimetype.h>
#include <kprocess.h>
#include <kurl.h>

#include <KoIcon.h>

#include "part.h"
#include "factory.h"

#include "kptviewbase.h"
#include "kptdocumentseditor.h"

#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptcommand.h"
#include "kptdocuments.h"
#include "kpttaskprogressdialog.h"
#include "kptcalendar.h"

#include <assert.h>

#include "debugarea.h"

namespace KPlatoWork
{

View::View( Part *part,  QWidget *parent, KActionCollection *collection )
    : QWidget( parent ),
    m_part( part ),
    m_scheduleActionGroup( new QActionGroup( this ) ),
    m_manager( 0 )
{
    m_readWrite = part->isReadWrite();
    kDebug(planworkDbg())<<m_readWrite;

//    m_dbus = new ViewAdaptor( this );
//    QDBusConnection::sessionBus().registerObject( '/' + objectName(), this );

//     m_tab = new QTabWidget( this );
     QVBoxLayout *layout = new QVBoxLayout( this );
     layout->setMargin(0);
//     layout->addWidget( m_tab );

////////////////////////////////////////////////////////////////////////////////////////////////////

    // Add sub views
    createViews();

    // The menu items
    // ------ Edit
    actionCut = collection->addAction(KStandardAction::Cut,  "edit_cut", this, SLOT( slotEditCut() ));
    actionCopy = collection->addAction(KStandardAction::Copy,  "edit_copy", this, SLOT( slotEditCopy() ));
    actionPaste = collection->addAction(KStandardAction::Paste,  "edit_paste", this, SLOT( slotEditPaste() ));

    actionRemoveSelectedPackages  = new KAction(koIcon("edit-delete"), i18n("Remove Packages"), this);
    collection->addAction("package_remove_selected", actionRemoveSelectedPackages );
    connect( actionRemoveSelectedPackages, SIGNAL( triggered( bool ) ), SLOT( slotRemoveSelectedPackages() ) );

    actionRemoveCurrentPackage  = new KAction(koIcon("edit-delete"), i18n("Remove Package"), this);
    collection->addAction("package_remove_current", actionRemoveCurrentPackage );
    connect( actionRemoveCurrentPackage, SIGNAL( triggered( bool ) ), SLOT( slotRemoveCurrentPackage() ) );


//     actionTaskProgress  = new KAction(koIcon("document-edit"), i18n("Progress..."), this);
//     collection->addAction("task_progress", actionTaskProgress );
//     connect( actionTaskProgress, SIGNAL( triggered( bool ) ), SLOT( slotTaskProgress() ) );

    //------ Settings
    actionConfigure  = new KAction(koIcon("configure"), i18n("Configure PlanWork..."), this);
    collection->addAction("configure", actionConfigure );
    connect( actionConfigure, SIGNAL( triggered( bool ) ), SLOT( slotConfigure() ) );

    //------ Popups
    actionEditDocument  = new KAction(koIcon("document-edit"), i18n("Edit..."), this);
    collection->addAction("edit_document", actionEditDocument );
    connect( actionEditDocument, SIGNAL( triggered( bool ) ), SLOT( slotEditDocument() ) );

    actionViewDocument  = new KAction(koIcon("document-preview"), i18nc( "@verb", "View..."), this);
    collection->addAction("view_document", actionViewDocument );
    connect( actionViewDocument, SIGNAL( triggered( bool ) ), SLOT( slotViewDocument() ) );

    // FIXME remove UndoText::removeDocument() when string freeze is lifted
    actionRemoveDocument = new KAction(koIcon("list-remove"), UndoText::removeDocument(), this);
    collection->addAction("remove_document", actionRemoveDocument );
    connect( actionRemoveDocument, SIGNAL( triggered( bool ) ), SLOT( slotRemoveDocument() ) );

    actionSendPackage  = new KAction(koIcon("mail-send"), i18n("Send Package..."), this);
    collection->addAction("edit_sendpackage", actionSendPackage );
    connect( actionSendPackage, SIGNAL( triggered( bool ) ), SLOT( slotSendPackage() ) );

    actionPackageSettings  = new KAction(koIcon("document-properties"), i18n("Package Settings..."), this);
    collection->addAction("edit_packagesettings", actionPackageSettings );
    connect( actionPackageSettings, SIGNAL( triggered( bool ) ), SLOT( slotPackageSettings() ) );

    actionTaskCompletion  = new KAction(koIcon("document-edit"), i18n("Edit Progress..."), this);
    collection->addAction("task_progress", actionTaskCompletion );
    connect( actionTaskCompletion, SIGNAL( triggered( bool ) ), SLOT( slotTaskCompletion() ) );

    actionViewDescription  = new KAction(/*koIcon("document_view"),*/ i18n("View Description..."), this);
    collection->addAction("task_description", actionViewDescription );
    connect( actionViewDescription, SIGNAL( triggered( bool ) ), SLOT( slotTaskDescription() ) );


    updateReadWrite( m_readWrite );
    //kDebug(planworkDbg())<<" end";

//    connect( m_tab, SIGNAL( currentChanged( int ) ), SLOT( slotCurrentChanged( int ) ) );

    slotSelectionChanged();
}

View::~View()
{
//    removeStatusBarItem( m_estlabel );
//    delete m_estlabel;
}

void View::createViews()
{
    TaskWorkPackageView * v = createTaskWorkPackageView();
    Q_UNUSED(v); // XXX: shouldn't v be deleted?
}

TaskWorkPackageView *View::createTaskWorkPackageView()
{
    TaskWorkPackageView *v = new TaskWorkPackageView( part(), this );
    kDebug(planworkDbg())<<PlanWorkSettings::self();
    layout()->addWidget( v );

    connect( v, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );

    connect( v, SIGNAL( selectionChanged() ), SLOT( slotSelectionChanged() ) );
    v->updateReadWrite( m_readWrite );
    return v;
}

void View::setupPrinter( QPrinter &/*printer*/, QPrintDialog &/*printDialog */)
{
    //kDebug(planworkDbg());
}

void View::print( QPrinter &/*printer*/, QPrintDialog &/*printDialog*/ )
{
}

void View::slotSelectionChanged()
{
    bool enable = ! currentView()->selectedNodes().isEmpty();
    actionRemoveSelectedPackages->setEnabled( enable );
    actionRemoveCurrentPackage->setEnabled( enable );
}

void View::slotEditCut()
{
    //kDebug(planworkDbg());
}

void View::slotEditCopy()
{
    //kDebug(planworkDbg());
}

void View::slotEditPaste()
{
    //kDebug(planworkDbg());
}

void View::slotProgressChanged( int )
{
}

void View::slotConfigure()
{
}

ScheduleManager *View::currentScheduleManager() const
{
    return 0; // atm we always work with default manager
}

void View::updateReadWrite( bool readwrite )
{
    kDebug(planworkDbg())<<m_readWrite<<"->"<<readwrite;
    m_readWrite = readwrite;

//    actionTaskProgress->setEnabled( readwrite );

    emit sigUpdateReadWrite( readwrite );
}

Part *View::part() const
{
    return m_part;
}

void View::slotPopupMenu( const QString& name, const QPoint & pos )
{
    Q_ASSERT( m_part->factory() );
    if ( m_part->factory() == 0 ) {
        return;
    }
    QMenu *menu = ( ( QMenu* ) m_part->factory() ->container( name, m_part ) );
    if ( menu == 0 ) {
        return;
    }
    QList<QAction*> lst;
    TaskWorkPackageView *v = currentView();
    if ( v ) {
        lst = v->contextActionList();
        kDebug(planworkDbg())<<lst;
        if ( ! lst.isEmpty() ) {
            menu->addSeparator();
            foreach ( QAction *a, lst ) {
                menu->addAction( a );
            }
        }
    }
    menu->exec( pos );
    foreach ( QAction *a, lst ) {
        menu->removeAction( a );
    }
}

bool View::loadContext()
{
    //kDebug(planworkDbg())<<endl;
    return true;
}

void View::saveContext( QDomElement &/*me*/ ) const
{
    //kDebug(planworkDbg())<<endl;
}

void View::slotEditDocument()
{
    slotEditDocument( currentDocument() );
}

void View::slotEditDocument( Document *doc )
{
    kDebug(planworkDbg())<<doc;
    if ( doc == 0 ) {
        kDebug(planworkDbg())<<"No document";
        return;
    }
    if ( doc->type() != Document::Type_Product ) {
        KMessageBox::error( 0, i18n( "This file is not editable" ) );
        return;
    }
    part()->editWorkpackageDocument( doc );
}

void View::slotViewDocument()
{
    emit viewDocument( currentDocument() );
}

void View::slotRemoveDocument()
{
    part()->removeDocument( currentDocument() );
}

void View::slotPackageSettings()
{
    WorkPackage *wp = part()->findWorkPackage( currentNode() );
    if ( wp == 0 ) {
        return;
    }
    QPointer<PackageSettingsDialog> dia = new PackageSettingsDialog( *wp, this );
    if ( dia->exec() == QDialog::Accepted && dia ) {
        KUndo2Command *cmd = dia->buildCommand();
        if ( cmd ) {
            kDebug(planworkDbg());
            part()->addCommand( cmd );
        }
    }
    delete dia;
}

void View::slotSendPackage()
{
    Node *node = currentNode();
    if ( node == 0 ) {
        KMessageBox::error(0, i18n("No work package is selected" ) );
        return;
    }
    kDebug(planworkDbg())<<node->name();
    WorkPackage *wp = part()->findWorkPackage( node );
    if ( wp == 0 ) {
        KMessageBox::error(0, i18n("Cannot find work package" ) );
        return;
    }
/*    if ( wp->isModified() ) {
        int r = KMessageBox::questionYesNoCancel( 0, i18n("This work package has been modified.\nDo you want to save it before sending?" ), node->name() );
        switch ( r ) {
            case KMessageBox::Cancel: return;
            case KMessageBox::Yes: wp->saveToProjects( part() ); break;
            default: break;
        }
    }*/
    KTemporaryFile temp;
    temp.setSuffix( ".planwork" );
    temp.setAutoRemove( false );
    if ( ! temp.open() ) {
        KMessageBox::error( 0, i18n("Could not open temporary file. Sending is aborted." ) );
        return;
    }
    bool wasmodified = wp->isModified();
    wp->saveNativeFormat( part(), temp.fileName() );
    wp->setModified( wasmodified );

    QStringList attachURLs;
    attachURLs << temp.fileName();

    QString to = node->projectNode()->leader();
    QString cc;
    QString bcc;
    QString subject = i18n( "Work Package: %1", node->name() );
    QString body = node->projectNode()->name();
    QString messageFile;

    KToolInvocation::invokeMailer( to, cc, bcc, subject, body, messageFile, attachURLs );
}

void View::slotTaskDescription()
{
    Task *node = qobject_cast<Task*>( currentNode() );
    if ( node == 0 ) {
        return;
    }
    QPointer<TaskDescriptionDialog> dlg = new TaskDescriptionDialog( *node, this, true );
    dlg->exec();
    delete dlg;
}

TaskWorkPackageView *View::currentView() const
{
    QList<TaskWorkPackageView *> lst = findChildren<TaskWorkPackageView*>();
    if ( lst.isEmpty() ) {
        return 0;
    }
    return lst.first();
}

Node *View::currentNode() const
{
    QList<TaskWorkPackageView *> lst = findChildren<TaskWorkPackageView*>();
    if ( lst.isEmpty() ) {
        return 0;
    }
    return lst.first()->currentNode();
}

Document *View::currentDocument() const
{
    QList<TaskWorkPackageView *> lst = findChildren<TaskWorkPackageView*>();
    if ( lst.isEmpty() ) {
        return 0;
    }
    return lst.first()->currentDocument();
}

void View::slotTaskProgress()
{
    kDebug(planworkDbg());
    Task *n = qobject_cast<Task*>( currentNode() );
    if ( n == 0 ) {
        return;
    }
    StandardWorktime *w = qobject_cast<Project*>( n->projectNode() )->standardWorktime();
    QPointer<TaskProgressDialog> dlg = new TaskProgressDialog( *n, currentScheduleManager(), w, this );
    if ( dlg->exec() == QDialog::Accepted && dlg ) {
        KUndo2Command *cmd = dlg->buildCommand();
        if ( cmd ) {
            cmd->redo(); //FIXME m_part->addCommand( cmd );
        }
    }
}

void View::slotTaskCompletion()
{
    kDebug(planworkDbg());
    WorkPackage *wp = m_part->findWorkPackage( currentNode() );
    if ( wp == 0 ) {
        return;
    }
    QPointer<TaskCompletionDialog> dlg = new TaskCompletionDialog( *wp, currentScheduleManager(), this );
    if ( dlg->exec() == QDialog::Accepted && dlg ) {
        KUndo2Command *cmd = dlg->buildCommand();
        if ( cmd ) {
            m_part->addCommand( cmd );
        }
    }
    delete dlg;
}

void View::slotRemoveSelectedPackages()
{
    kDebug(planworkDbg());
    QList<Node*> lst = currentView()->selectedNodes();
    if ( lst.isEmpty() ) {
        return;
    }
    m_part->removeWorkPackages( lst );
}

void View::slotRemoveCurrentPackage()
{
    kDebug(planworkDbg());
    Node *n = currentNode();
    if ( n == 0 ) {
        return;
    }
    m_part->removeWorkPackage( n );
}


}  //KPlatoWork namespace

#include "view.moc"
