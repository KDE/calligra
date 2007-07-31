/*
 * Copyright (C) 2007 Igor Stepin <igor_for_os@stepin.name>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "KWCollaborationPart.h"

// qt
#include <QApplication>
#include <QDockWidget>
// kde
#include <klocale.h>
#include <kgenericfactory.h>
#include <kstandarddirs.h>
#include <kactioncollection.h>
#include <kparts/partmanager.h>
// koffice
#include <KoDockRegistry.h>
#include <KoMainWindow.h>
#include <KoDocument.h>
// kword
#include <KWView.h>

//#include <kactionmenu.h>
// #include <kmenu.h>
// #include <kmimetype.h>
// #include <kmessagebox.h>
// #include <kurl.h>

typedef KGenericFactory< KWCollaborationPart > KWordCollaborationFactory;
K_EXPORT_COMPONENT_FACTORY( collaborationmodulekword, KWordCollaborationFactory( "collaborationmodulekword" ) )

/// \internal d-pointer class.
class KWCollaborationPart::Private
{
    public:
        Private(QObject *parent):
                parent_( 0 )
        {
            Q_ASSERT( parent );
            KWView *parent_ = qobject_cast<KWView*>( parent );
            Q_ASSERT( parent_ );
        }

        KWView *parent() const { return parent_; }
    private:
        KWView *parent_;
};

KWCollaborationPart::KWCollaborationPart( QObject *parent, const QStringList & )
        : KParts::Plugin()
        , d( new Private(parent) )
{
    setComponentData( KWCollaborationPart::componentData() );//TODO?
    setXMLFile( KStandardDirs::locate( "data", "kword/kpartplugins/collaboration.rc" ), true );
    kDebug( 10101 ) <<"KWCollaboration plugin. Class:" << metaObject()->className();
    //kDebug( 10101 ) <<"parent:" << parent->metaObject()->className();

    KAction *connectToAction  = new KAction( i18n( "Connect to..." ), this );
    actionCollection()->addAction( "connect_to", connectToAction );

    KAction *shareCurrentDocumentAction  = new KAction( i18n( "Share current document..." ), this );
    actionCollection()->addAction( "share_current_document", shareCurrentDocumentAction );
    /*
        collaborationManager = new kcollaborate::CollaborationManager(this);
        undoStack = new kcollaborate::UndoStack();
        connectDialog = new kcollaborate::ConnectDialog(this);
        shareDialog = new kcollaborate::ShareDialog(this);

        connect(connectDialog, SIGNAL(connect(const QString&)),
                collaborationManager, SLOT(connect(const QString&)));
        connect(shareDialog, SIGNAL(share(const QString&)),
                collaborationManager, SLOT(share(const QString&)));



      <Action name="connect_to"/>
      <Action name="share_current_document"/>

        KAction* execaction  = new KAction(i18n("Execute Script File..."), this);
        actionCollection()->addAction("executescriptfile", execaction);
        connect(execaction, SIGNAL(triggered(bool)), this, SLOT(slotShowExecuteScriptFile()));

        d->scriptsmenu = new KActionMenu(i18n("Scripts"), this);
        actionCollection()->addAction("scripts", d->scriptsmenu);
        connect(d->scriptsmenu->menu(), SIGNAL(aboutToShow()), this, SLOT(slotMenuAboutToShow()));

        KAction* manageraction  = new KAction(i18n("Script Manager..."), this);
        actionCollection()->addAction("scriptmanager", manageraction);
        connect(manageraction, SIGNAL(triggered(bool)), this, SLOT(slotShowScriptManager()));

        connect(&Kross::Manager::self(), SIGNAL(started(Kross::Action*)), this, SLOT(slotStarted(Kross::Action*)));
        //connect(&Kross::Manager::self(), SIGNAL(finished(Kross::Action*)), this, SLOT(slotFinished(Kross::Action*)));

        if( Kross::Manager::self().property("configfile") == QVariant::Invalid ) {
            QString file = KGlobal::dirs()->locateLocal("appdata", "scripts/scripts.rc");
            QStringList files = KGlobal::dirs()->findAllResources("appdata", "scripts/*.rc");
            Kross::Manager::self().setProperty("configfile", file);
            Kross::Manager::self().setProperty("configfiles", files);

            if( QFileInfo(file).exists() )
                Kross::Manager::self().actionCollection()->readXmlFile(file);
            else
                foreach(QString f, files)
                    Kross::Manager::self().actionCollection()->readXmlFile(f);
        }

        KoView* view = d->module->view();
        KoMainWindow* mainwindow = view ? view->shell() : 0;
        if( mainwindow ) {
            KoScriptingDockerFactory factory( mainwindow );
            QDockWidget* docker = mainwindow->createDockWidget(&factory);
            Q_UNUSED(docker);
        }

    */

}

KWCollaborationPart::~KWCollaborationPart()
{
    kDebug( 10101 ) <<"KWCollaborationPart::~KWCollaborationPart()";
    delete d;
}

#include "KWCollaborationPart.moc"
