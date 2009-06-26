/* This file is part of the KDE project
  Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
  Copyright (C) 2002 - 2007 Dag Andersen <danders@get2net.dk>

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

#include "kpttaskeditor.h"

#include "KoDocumentInfo.h"
#include <KoMainWindow.h>
#include <KoToolManager.h>

#include <QApplication>
#include <QLabel>
#include <QString>
#include <qsize.h>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QPrinter>
#include <QPrintDialog>

#include <kicon.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kactionmenu.h>
#include <kmenu.h>
#include <kstandardaction.h>
#include <klocale.h>
#include <kdebug.h>
#include <ktoolbar.h>
#include <kstandardshortcut.h>
#include <kaccelgen.h>
#include <kdeversion.h>
#include <kstatusbar.h>
#include <kxmlguifactory.h>
#include <kstandarddirs.h>
#include <kdesktopfile.h>
#include <KToolInvocation>
#include <ktoggleaction.h>
#include <kfiledialog.h>
#include <kparts/event.h>
#include <kparts/partmanager.h>
#include <kparts/componentfactory.h>

#include <kmessagebox.h>
#include <krun.h>
#include <kvbox.h>
#include <kmimetype.h>
#include <kprocess.h>
#include <kurl.h>

#include "part.h"
#include "factory.h"

#include "kptviewbase.h"
#include "kptdocumentseditor.h"
#include "kptworkpackageview.h"

#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptcommand.h"
#include "kptdocuments.h"
#include "kpttaskprogressdialog.h"

#include <assert.h>

namespace KPlatoWork
{

View::View( Part* part, QWidget* parent )
    : KoView( part, parent ),
    m_currentWidget( 0 ),
    m_scheduleActionGroup( new QActionGroup( this ) ),
    m_manager( 0 )
{
    //kDebug();

    setComponentData( Factory::global() );

    if ( part->isSingleViewMode() ) { // NOTE: don't use part->isReadWrite() here
        setXMLFile( "kplatowork_readonly.rc" );
    } else {
        setXMLFile( "kplatowork.rc" );
    }

    m_readWrite = part->isReadWrite();
    kDebug()<<m_readWrite;

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
    actionCut = actionCollection()->addAction(KStandardAction::Cut,  "edit_cut", this, SLOT( slotEditCut() ));
    actionCopy = actionCollection()->addAction(KStandardAction::Copy,  "edit_copy", this, SLOT( slotEditCopy() ));
    actionPaste = actionCollection()->addAction(KStandardAction::Paste,  "edit_paste", this, SLOT( slotEditPaste() ));

    actionTaskProgress  = new KAction(KIcon( "document-properties" ), i18n("Progress..."), this);
    actionCollection()->addAction("task_progress", actionTaskProgress );
    connect( actionTaskProgress, SIGNAL( triggered( bool ) ), SLOT( slotTaskProgress() ) );

    // ------ Settings
    actionConfigure  = new KAction(KIcon( "configure" ), i18n("Configure KPlatoWork..."), this);
    actionCollection()->addAction("configure", actionConfigure );
    connect( actionConfigure, SIGNAL( triggered( bool ) ), SLOT( slotConfigure() ) );

    actionEditDocument  = new KAction(KIcon( "document-properties" ), i18n("Edit..."), this);
    actionCollection()->addAction("edit_document", actionEditDocument );
    connect( actionEditDocument, SIGNAL( triggered( bool ) ), SLOT( slotEditDocument() ) );

    actionViewDocument  = new KAction(KIcon( "document-properties" ), i18nc( "@verb", "View..."), this);
    actionCollection()->addAction("view_document", actionViewDocument );
    connect( actionViewDocument, SIGNAL( triggered( bool ) ), SLOT( slotViewDocument() ) );

    m_progress = 0;
    m_estlabel = new QLabel( "", 0 );
    if ( statusBar() ) {
        addStatusBarItem( m_estlabel, 0, true );
        //m_progress = new QProgressBar();
        //addStatusBarItem( m_progress, 0, true );
        //m_progress->hide();
    }

    loadContext();

    connect( part, SIGNAL( changed() ), SLOT( slotUpdate() ) );

    actionSendPackage  = new KAction(KIcon( "file_send_file" ), i18n("Send Package..."), this);
    actionCollection()->addAction("edit_sendpackage", actionSendPackage );
    connect( actionSendPackage, SIGNAL( triggered( bool ) ), SLOT( slotSendPackage() ) );

    actionTaskProgress  = new KAction(KIcon( "document-properties" ), i18n("Progress..."), this);
    actionCollection()->addAction("task_progress", actionTaskProgress );
    connect( actionTaskProgress, SIGNAL( triggered( bool ) ), SLOT( slotTaskProgress() ) );

    updateReadWrite( m_readWrite );
    //kDebug()<<" end";

//    connect( m_tab, SIGNAL( currentChanged( int ) ), SLOT( slotCurrentChanged( int ) ) );
}

View::~View()
{
    removeStatusBarItem( m_estlabel );
    delete m_estlabel;
}

void View::createViews()
{
    ViewBase * v = createTaskWorkPackageView();
    v->setFocus();
}

ViewBase *View::createTaskWorkPackageView()
{
    TaskWorkPackageView *v = new TaskWorkPackageView( getPart(), this );
    layout()->addWidget( v );

    connect( v, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    connect( v, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );
    v->updateReadWrite( m_readWrite );
    return v;
}

ViewBase *View::createTaskView()
{
    TaskView *v = new TaskView( getPart(), this );
    layout()->addWidget( v );
//    m_tab->addWidget( v );

//     ViewListItem *i = m_viewlist->addView( cat, tag, name, v, getPart(), "task_view", index );
//     i->setToolTip( 0, tip );

    v->draw( getProject() );
    v->setScheduleManager( currentScheduleManager() );

    connect( this, SIGNAL( currentScheduleManagerChanged( ScheduleManager* ) ), v, SLOT( setScheduleManager( ScheduleManager* ) ) );

    connect( v, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    connect( v, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );
    v->updateReadWrite( m_readWrite );
    return v;
}

ViewBase *View::createTaskInfoView()
{
    kDebug();
    WorkPackageInfoView *v = new WorkPackageInfoView( getPart(), m_tab );
    m_tab->addTab( v, i18n( "Information" ) );

    Project &p = getProject();
    v->setProject( &p );
    Task *t = 0;
    if ( p.numChildren() > 0 ) { // should be 1
        t = dynamic_cast<Task*>( p.childNode( 0 ) );
    }
    v->setTask( t );

    kDebug()<<p.allScheduleManagers();
    v->setScheduleManager( p.allScheduleManagers().value( 0 ) );

    connect( v, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    connect( v, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );

    connect( this, SIGNAL( sigUpdateReadWrite( bool ) ), v, SLOT( slotUpdateReadWrite( bool ) ) );

    v->updateReadWrite( m_readWrite );
    return v;
}

ViewBase *View::createDocumentsView()
{
    kDebug();
    DocumentsEditor *v = new DocumentsEditor( getPart(), m_tab );
    m_tab->addTab( v, i18n( "Documents" ) );

    Project &p = getProject();
    if ( p.numChildren() > 0 ) { // should be 1
        Node *n = p.childNode( 0 );
        kDebug()<<"Node: "<<n->name();
        v->draw( n->documents() );
    }
    connect( v, SIGNAL( editDocument( Document* ) ), SLOT( slotEditDocument( Document* ) ) );
    connect( v, SIGNAL( viewDocument( Document* ) ), SLOT( slotViewDocument( Document* ) ) );

    connect( v, SIGNAL( guiActivated( ViewBase*, bool ) ), SLOT( slotGuiActivated( ViewBase*, bool ) ) );

    connect( v, SIGNAL( requestPopupMenu( const QString&, const QPoint & ) ), this, SLOT( slotPopupMenu( const QString&, const QPoint& ) ) );

    connect( this, SIGNAL( sigUpdateReadWrite( bool ) ), v, SLOT( slotUpdateReadWrite( bool ) ) );

    v->updateReadWrite( m_readWrite );
    return v;
}

Project& View::getProject() const
{
    return getPart() ->getProject();
}

void View::setZoom( double )
{
    //TODO
}

KPlatoWork_MainWindow *View::kplatoWorkMainWindow() const
{
    return dynamic_cast<KPlatoWork_MainWindow*>( topLevelWidget() );
}

void View::setupPrinter( QPrinter &printer, QPrintDialog &printDialog )
{
    //kDebug();
}

void View::print( QPrinter &printer, QPrintDialog &printDialog )
{
}

// QWidget *View::canvas() const
// {
//     return this;//m_tab->currentWidget();
// }

KoDocument *View::hitTest( const QPoint &pos )
{
    kDebug()<<"---------->";
    // TODO: The gui handling can certainly be simplified (at least I think so),
    // by someone who have a better understanding of all the possibilities of KParts
    // than I have.
    // pos is in m_tab->currentWidget() coordinates
/*    QPoint gl = m_tab->currentWidget()->mapToGlobal(pos);
    kDebug()<<pos<<gl;
    if ( koDocument() == dynamic_cast<KoDocument*>(partManager()->activePart() ) ) {
        // just activating new view on the same doc
        return qobject_cast<KoDocument*>( koDocument()->hitTest( this, pos) );
    }*/
    return KoView::hitTest( pos );
    kDebug()<<"<----------";
}

void View::slotCurrentChanged( int index )
{
    kDebug()<<"---------->";
    if ( partManager() == 0 ) {
        kWarning()<<"No partmanager (yet)";
        kDebug()<<"<----------";
        return;
    }
    KParts::Part *ap = partManager()->activePart();
    QWidget *aw = partManager()->activeWidget();
    QWidget *cw = m_tab->currentWidget();
    kDebug()<<ap<<aw<<cw;
    if ( cw == m_currentWidget ) {
        kDebug()<<cw<<m_currentWidget;
        kDebug()<<"<----------";
        return;
    }
    ViewBase *cv = dynamic_cast<ViewBase*>( m_currentWidget );
    if ( cv ) {
        cv->setGuiActive( false );
    }
    partManager()->setActivePart( getPart(), this ); // restore my factory and clear ap->factory()
    cv = dynamic_cast<ViewBase*>( cw );
    if ( cv ) {
        cv->setGuiActive( true );
    } else {
        // might be a kpart
        if ( factory() ) kDebug()<<"this:"<<factory()->clients();
        if ( ap->factory() ) kDebug()<<"ap:"<<ap->factory()->clients();

        if ( m_partsMap.contains( cw ) ) {
            kDebug()<<"Part found"<<m_partsMap[cw]<<" for widget:"<<cw;
            if ( m_partsMap[ cw ]->factory() ) kDebug()<<"cp:"<<m_partsMap[ cw ]->factory()->clients();
            if ( factory() ) {
                factory()->addClient( m_partsMap[ cw ] );
            }
            partManager()->setActivePart( m_partsMap[ cw ] ); // this removes my factory()
        } else kDebug()<<"No part found for widget:"<<cw;
    }
    m_currentWidget = cw;
    kDebug()<<"<----------";
}

void View::slotGuiActivated( ViewBase *view, bool activate )
{
    kDebug()<<"---------->";
    if ( activate ) {
        foreach( QString name, view->actionListNames() ) {
            kDebug()<<"activate"<<name<<","<<view->actionList( name ).count();
            plugActionList( name, view->actionList( name ) );
        }
    } else {
        foreach( QString name, view->actionListNames() ) {
            kDebug()<<"deactivate"<<name;
            unplugActionList( name );
        }
    }
    kDebug()<<"<----------";
}

void View::guiActivateEvent( KParts::GUIActivateEvent *ev )
{
    KoView::guiActivateEvent( ev );
/*    kDebug()<<"---------->";
    kDebug()<<ev->activated();
    kDebug()<<"View:"<<partManager()<<partManager()->parts();
    kDebug()<<"Active:"<<partManager()->activePart()<<partManager()->activeWidget();
    kDebug()<<"Clients:"<<factory()->clients();
    KoView::guiActivateEvent( ev );
    if ( ev->activated() ) {
        // plug my own actionlists, they may be gone
        slotPlugScheduleActions();
        //Remove gui for all KParts
        foreach ( KParts::Part *p, m_partsMap ) {
            if ( p->factory() ) {
                p->factory()->removeClient( p );
            }
        }
    }
    // propagate to sub-view
    ViewBase *v = dynamic_cast<ViewBase*>( m_tab->currentWidget() );
    if ( v ) {
        v->setGuiActive( ev->activated() );
    }
    kDebug()<<"<----------";*/
}

void View::slotEditCut()
{
    //kDebug();
}

void View::slotEditCopy()
{
    //kDebug();
}

void View::slotEditPaste()
{
    //kDebug();
}

void View::slotProgressChanged( int )
{
}

void View::slotConfigure()
{
}

ScheduleManager *View::currentScheduleManager() const
{
    return getProject().scheduleManagers().value( 0 );
}

void View::updateReadWrite( bool readwrite )
{
    kDebug()<<m_readWrite<<"->"<<readwrite;
    m_readWrite = readwrite;

    actionTaskProgress->setEnabled( readwrite );

    emit sigUpdateReadWrite( readwrite );
}

Part *View::getPart() const
{
    return ( Part * ) koDocument();
}

QMenu * View::popupMenu( const QString& name )
{
    //kDebug();
    Q_ASSERT( factory() );
    if ( factory() )
        return ( ( QMenu* ) factory() ->container( name, this ) );
    return 0L;
}

void View::slotUpdate()
{
//    updateView( m_tab->currentWidget() );
}

void View::updateView( QWidget * )
{
    QApplication::setOverrideCursor( Qt::WaitCursor );

    QApplication::restoreOverrideCursor();
}

void View::slotPopupMenu( const QString& menuname, const QPoint & pos )
{
    QMenu * menu = this->popupMenu( menuname );
    if ( menu ) {
        //kDebug()<<menu<<":"<<menu->actions().count();
        QList<ViewBase *> list = findChildren<ViewBase*>();
        if ( list.isEmpty() ) {
            return;
        }
        ViewBase *v = list.first();
        kDebug()<<v<<menuname;
        QList<QAction*> lst;
        if ( v ) {
            lst = v->contextActionList();
            kDebug()<<lst;
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
}

bool View::loadContext()
{
    //kDebug()<<endl;
    return true;
}

void View::saveContext( QDomElement &me ) const
{
    //kDebug()<<endl;
}

void View::setLabel()
{
    //kDebug();
    Schedule *s = m_manager == 0 ? 0 : m_manager->expected();
    if ( s && !s->isDeleted() && s->isScheduled() ) {
        m_estlabel->setText( m_manager->name() );
        return;
    }
    m_estlabel->setText( i18n( "Not scheduled" ) );
}

void View::addPart( KParts::Part* part, const QString &name )
{
    kDebug()<<"---------->";
    m_partsMap.insert( part->widget(), part );
    int tab = m_tab->addTab( part->widget(), name );
    partManager()->addPart( part, false );
    if ( factory() ) {
/*        factory()->addClient( part );
        kDebug()<<factory()->clients();*/
    }
    m_tab->setCurrentIndex( tab );
    kDebug()<<"<----------";
}

bool View::viewDocument( const KUrl &filename )
{
    kDebug()<<"url:"<<filename;
    if ( ! filename.isValid() ) {
        //KMessageBox::error( 0, i18n( "Cannot open document. Invalid url: %1", filename.pathOrUrl() ) );
        return false;
    }
    KRun *run = new KRun( filename, 0 );
    return true;
}

void View::slotEditDocument()
{
    slotEditDocument( currentDocument() );
}

void View::slotEditDocument( Document *doc )
{
    kDebug()<<doc;
    if ( doc == 0 ) {
        kDebug()<<"No document";
        return;
    }
    if ( doc->type() != Document::Type_Product ) {
        KMessageBox::error( 0, i18n( "This file is not editable" ) );
        return;
    }
    getPart()->editWorkpackageDocument( doc );
}

void View::slotViewDocument()
{
    slotViewDocument( currentDocument() );
}

void View::slotViewDocument( Document *doc )
{
    kDebug()<<"---------->";
    kDebug()<<doc;
    if ( doc == 0 ) {
        return;
    }
    KUrl filename;
    if ( doc->sendAs() == Document::SendAs_Copy ) {
        filename = getPart()->extractFile( doc );
    } else {
        filename = doc->url();
    }
    // open for view
    viewDocument( filename );
    kDebug()<<"<----------";
}

void View::slotTaskProgress()
{
    //kDebug();
    Node * node = currentNode();
    if ( node == 0 ) {
        return ;
    }
    switch ( node->type() ) {
        case Node::Type_Task: {
            Task *task = static_cast<Task *>( node );
            TaskProgressDialog *dia = new TaskProgressDialog( *task, currentScheduleManager(),  getProject().standardWorktime() );
            if ( dia->exec()  == QDialog::Accepted) {
                QUndoCommand * m = dia->buildCommand();
                if ( m ) {
                    getPart() ->addCommand( m );
                }
            }
            delete dia;
            break;
        }
        default:
            break; // avoid warnings
    }
}

void View::slotSendPackage()
{
    Node *node = currentNode();
    if ( node == 0 ) {
        KMessageBox::error(0, i18n("No node selected" ) );
        return;
    }
    qDebug()<<"View::slotSendPackage:"<<node->name();
    WorkPackage *wp = getPart()->findWorkPackage( node );
    if ( wp == 0 ) {
        KMessageBox::error(0, i18n("Cannot find work package" ) );
        return;
    }
    if ( wp->isModified() ) {
        int r = KMessageBox::questionYesNoCancel( 0, i18n("This work package has been modified.\nDo you want to save it before sending?" ), node->name() );
        switch ( r ) {
            case KMessageBox::Cancel: return;
            case KMessageBox::Yes: wp->saveToProjects( getPart() ); break;
            default: break;
        }
    }
    QStringList attachURLs;
    attachURLs << wp->filePath();

    QString to = node->projectNode()->leader();
    QString cc;
    QString bcc;
    QString subject = i18n( "Work Package: %1", node->name() );
    QString body = node->description();
    QString messageFile;

    KToolInvocation::invokeMailer( to, cc, bcc, subject, body, messageFile, attachURLs );
}

Node *View::currentNode() const
{
    QList<ViewBase *> lst = findChildren<ViewBase*>();
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

}  //KPlatoWork namespace

#include "view.moc"
