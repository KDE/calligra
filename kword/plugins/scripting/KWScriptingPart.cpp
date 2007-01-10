/*
 * This file is part of KWord
 *
 * Copyright (c) 2006 Sebastian Sauer <mail@dipe.org>
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

#include "KWScriptingPart.h"
#include "Module.h"

//#include <QApplication>
//#include <QFileInfo>
#include <QToolBar>
#include <QBoxLayout>
#include <QTreeView>
#include <QModelIndex>
#include <QHeaderView>
#include <QDockWidget>

#include <klocale.h>
#include <kicon.h>
#include <kgenericfactory.h>
#include <kstandarddirs.h>
#include <kactioncollection.h>
//#include <kcmdlineargs.h>
//#include <kurl.h>

#include <KWView.h>

#include <kross/core/manager.h>
#include <kross/core/action.h>
#include <kross/core/guiclient.h>
#include <kross/core/model.h>

/***********************************************************************
 * KWScriptingDocker
 */

KWScriptingDocker::KWScriptingDocker(QWidget* parent, Kross::GUIClient* guiclient)
    : QWidget(parent)
    , m_guiclient(guiclient)
{
    QBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);
    setLayout(layout);

    m_view = new QTreeView(this);
    m_view->setRootIsDecorated(false);
    m_view->header()->hide();
    m_model = new Kross::ActionCollectionProxyModel(this);
    m_view->setModel(m_model);
    layout->addWidget(m_view, 1);
    m_view->expandAll();

    QToolBar* tb = new QToolBar(this);
    layout->addWidget(tb);
    tb->setMovable(false);
    //tb->setOrientation(Qt::Vertical);
    //tb->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    tb->addAction(KIcon("player_play"), i18n("Run"), this, SLOT(runScript()) );
    tb->addAction(KIcon("player_stop"), i18n("Stop"), this, SLOT(stopScript()) );

    connect(m_view, SIGNAL(doubleClicked(const QModelIndex&)), SLOT(runScript()));
}

KWScriptingDocker::~KWScriptingDocker()
{
}

void KWScriptingDocker::runScript()
{
    QModelIndex index = m_model->mapToSource( m_view->currentIndex() );
    if( index.isValid() ) {
        Kross::Action* action = Kross::ActionCollectionModel::action(index);
        if( action )
            action->trigger();
    }
}

void KWScriptingDocker::stopScript()
{
    QModelIndex index = m_model->mapToSource( m_view->currentIndex() );
    if( index.isValid() ) {
        Kross::Action* action = Kross::ActionCollectionModel::action(index);
        if( action )
            action->finalize();
    }
}

/***********************************************************************
 * KWScriptingPart
 */

typedef KGenericFactory< KWScriptingPart > KWordScriptingFactory;
K_EXPORT_COMPONENT_FACTORY( krossmodulekword, KWordScriptingFactory( "krossmodulekword" ) )

/// \internal d-pointer class.
class KWScriptingPart::Private
{
    public:
        KWView* view;
        Kross::GUIClient* guiclient;
        Scripting::Module* module;
        Private() : module(0) {}
        ~Private() { delete module; }
};

KWScriptingPart::KWScriptingPart(QObject* parent, const QStringList&)
    : KParts::Plugin(parent)
    , d(new Private())
{
    setInstance(KWScriptingPart::instance());
    setXMLFile(KStandardDirs::locate("data","kword/kpartplugins/scripting.rc"), true);

    kDebug(32010) << "KWScripting plugin. Class: " << metaObject()->className() << ", Parent: " << parent->metaObject()->className() << endl;

    d->view = dynamic_cast< KWView* >(parent);
    Q_ASSERT(d->view);

    // Create the Kross GUIClient which is the higher level to let
    // Kross deal with scripting code.
    d->guiclient = new Kross::GUIClient(this, this);
    //d->guiclient ->setXMLFile(locate("data","kspreadplugins/scripting.rc"), true);

    d->module = new Scripting::Module();
    d->module->setView(d->view);

    // Setup the actions Kross provides and KSpread likes to have.
    KAction* execaction  = new KAction(i18n("Execute Script File..."), this);
    actionCollection()->addAction("executescriptfile", execaction);
    connect(execaction, SIGNAL(triggered(bool)), d->guiclient, SLOT(executeFile()));

    KAction* manageraction  = new KAction(i18n("Script Manager..."), this);
    actionCollection()->addAction("configurescripts", manageraction);
    connect(manageraction, SIGNAL(triggered(bool)), d->guiclient, SLOT(showManager()));

    QAction* scriptmenuaction = d->guiclient->action("scripts");
    actionCollection()->addAction("scripts", scriptmenuaction);

    connect(&Kross::Manager::self(), SIGNAL(started(Kross::Action*)), this, SLOT(started(Kross::Action*)));
    connect(&Kross::Manager::self(), SIGNAL(finished(Kross::Action*)), this, SLOT(finished(Kross::Action*)));

    QDockWidget* dock = d->view->createDock(i18n("Scripts"), new KWScriptingDocker(d->view, d->guiclient));
    Q_UNUSED(dock);
    //dock->setVisible(false);
}

KWScriptingPart::~KWScriptingPart()
{
    kDebug(32010) << "KWScriptingPart::~KWScriptingPart()" << endl;
    delete d;
}

void KWScriptingPart::started(Kross::Action*)
{
    kDebug(32010) << "KWScriptingPart::started" << endl;
    Kross::Manager::self().addObject(d->module, "KWord");
}

void KWScriptingPart::finished(Kross::Action*)
{
    kDebug(32010) << "KWScriptingPart::finished" << endl;
    //d->view->document()->setModified(true);
    //d->module->deleteLater();
    //QApplication::restoreOverrideCursor();
}

#include "KWScriptingPart.moc"
