/* This file is part of the KDE project
   Copyright (C) 2006-2007 Thorsten Zachmann <zachmann@kde.org>
   Copyright (C) 2008 Carlos Licea <carlos.licea@kdemail.org>
   
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
#include "KPrView.h"

#include <klocale.h>
#include <ktoggleaction.h>
#include <kactioncollection.h>
#include <kactionmenu.h>

#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoMainWindow.h>
#include <KoPACanvas.h>
#include <KoPADocumentStructureDocker.h>

#include "KPrDocument.h"
#include "KPrPage.h"
#include "KPrMasterPage.h"
#include "KPrPageApplicationData.h"
#include "KPrViewModePresenterView.h"
#include "KPrViewModePresentation.h"
#include "KPrViewModeNotes.h"
#include "commands/KPrAnimationCreateCommand.h"
#include "dockers/KPrPageEffectDocker.h"
#include "dockers/KPrPageEffectDockerFactory.h"
#include "dockers/KPrClickActionDocker.h"
#include "dockers/KPrClickActionDockerFactory.h"
#include "shapeanimations/KPrAnimationMoveAppear.h"

#include "KPrCustomSlideShows.h"
#include "ui/KPrCustomSlideShowsDialog.h"
#include <QDebug>
#include <QtGui/QDesktopWidget>

KPrView::KPrView( KPrDocument *document, QWidget *parent )
: KoPAView( document, parent )
, m_presentationMode( new KPrViewModePresentation( this, m_canvas ))
, m_normalMode( m_viewMode )
, m_notesMode( new KPrViewModeNotes(this, m_canvas))
{
    initGUI();
    initActions();
    m_presenterViewMode = 0;
}

KPrView::~KPrView()
{
    delete m_presentationMode;
    delete m_presenterViewMode;
    delete m_notesMode;
}

KoViewConverter * KPrView::viewConverter()
{
    return viewMode()->viewConverter();
}

void KPrView::updateActivePage(KoPAPageBase *page)
{
    viewMode()->updateActivePage( page );

}

void KPrView::initGUI()
{ 
    // add page effect docker to the main window
    KPrPageEffectDockerFactory factory;
    KPrPageEffectDocker* docker;
    docker = qobject_cast<KPrPageEffectDocker*>( createDockWidget( &factory ) );
    docker->setView( this );

    // add page effect docker to the main window
    KPrClickActionDockerFactory clickActionFactory;
    KPrClickActionDocker *clickActionDocker;
    clickActionDocker = qobject_cast<KPrClickActionDocker*>( createDockWidget( &clickActionFactory ) );
    clickActionDocker->setView( this );
}

void KPrView::initActions()
{
    if ( !m_doc->isReadWrite() )
       setXMLFile( "kpresenter_readonly.rc" );
    else
       setXMLFile( "kpresenter.rc" );

    // do special kpresenter stuff here
    m_actionStartPresentation = new KActionMenu( KIcon("media-playback-start"), i18n( "Start Presentation" ), this );
    actionCollection()->addAction( "view_mode", m_actionStartPresentation );
    connect( m_actionStartPresentation, SIGNAL( activated() ), this, SLOT( startPresentation() ) );
    KAction* action = new KAction( i18n( "From Current Page" ), this );
    m_actionStartPresentation->addAction( action );
    connect( action, SIGNAL( activated() ), this, SLOT( startPresentation() ) );
    action = new KAction( i18n( "From First Page" ), this );
    m_actionStartPresentation->addAction( action );
    connect( action, SIGNAL( activated() ), this, SLOT( startPresentationFromBeginning() ) );

    m_actionViewModeNormal = new KAction(i18n("Normal"), this);
    m_actionViewModeNormal->setCheckable(true);
    m_actionViewModeNormal->setChecked(true);
    actionCollection()->addAction("view_normal", m_actionViewModeNormal);
    connect(m_actionViewModeNormal, SIGNAL(triggered()), this, SLOT(showNormal()));

    m_actionViewModeNotes = new KAction(i18n("Notes"), this);
    m_actionViewModeNotes->setCheckable(true);
    actionCollection()->addAction("view_notes", m_actionViewModeNotes);
    connect(m_actionViewModeNotes, SIGNAL(triggered()), this, SLOT(showNotes()));

    QActionGroup *viewModesGroup = new QActionGroup(this);
    viewModesGroup->addAction(m_actionViewModeNormal);
    viewModesGroup->addAction(m_actionViewModeNotes);

    m_actionCreateAnimation = new KAction( i18n( "Create Appear Animation" ), this );
    actionCollection()->addAction( "edit_createanimation", m_actionCreateAnimation );
    connect( m_actionCreateAnimation, SIGNAL( activated() ), this, SLOT( createAnimation() ) );

    m_actionCreateCustomSlideShowsDialog = new KAction( i18n( "Edit Custom Slide Shows..." ), this );
    actionCollection()->addAction( "edit_customslideshows", m_actionCreateCustomSlideShowsDialog );
    connect( m_actionCreateCustomSlideShowsDialog, SIGNAL( activated() ), this, SLOT( dialogCustomSlideShows() ) );
 
    KoPADocumentStructureDocker *docStructureDocker = documentStructureDocker();
    connect(docStructureDocker, SIGNAL(pageChanged(KoPAPageBase*)), this, SLOT(updateActivePage(KoPAPageBase*)));
}

void KPrView::startPresentation()
{
#if 0
    // TODO: check for second monitor + read from config file
    // Create a new view
    KoMainWindow *shell = new KoMainWindow( m_doc->componentData() );
    shell->setRootDocument( m_doc );
    m_doc->addShell( shell );
    shell->show();

    KPrView *view = dynamic_cast<KPrView *>( shell->rootView() );
    Q_ASSERT( view );
#endif
    setViewMode( m_presentationMode );
#if 0
    view->activatePresenterView( this );
#endif
}

void KPrView::startPresentationFromBeginning()
{
    setActivePage( m_doc->pageByNavigation( activePage(), KoPageApp::PageFirst ) );
    startPresentation();
}

void KPrView::activatePresenterView( KPrView *mainView )
{
    if ( !m_presenterViewMode ) {
        KPrViewModePresentation *presentationMode = dynamic_cast<KPrViewModePresentation *>( mainView->viewMode() );
        // To use presenter view, the main view should be in presentation mode
        Q_ASSERT( presentationMode );
        m_presenterViewMode = new KPrViewModePresenterView( this, m_canvas, presentationMode );
    }

    setViewMode( m_presenterViewMode );
}

void KPrView::createAnimation()
{
    static int animationcount = 0;
    KoSelection * selection = m_canvas->shapeManager()->selection();
    QList<KoShape*> selectedShapes = selection->selectedShapes();
    foreach( KoShape * shape, selectedShapes )
    {
        KPrShapeAnimation * animation = new KPrAnimationMoveAppear( shape, animationcount );
        KPrDocument * doc = dynamic_cast<KPrDocument *>( m_doc );
        Q_ASSERT( doc );
        KPrAnimationCreateCommand * command = new KPrAnimationCreateCommand( doc, animation );
        m_canvas->addCommand( command );
    }
    animationcount = ( animationcount + 1 ) % 3;
}

void KPrView::showNormal()
{
    setViewMode(m_normalMode);
}

void KPrView::showNotes()
{
    // Make sure that we are not in master mode
    // since notes master is not supported yet
    if ( m_viewMode->masterMode() ) {
        actionCollection()->action( "view_masterpages" )->setChecked( false );
        setMasterMode( false );
    }
    setViewMode(m_notesMode);
}

void KPrView::dialogCustomSlideShows()
{
    KPrDocument *doc = dynamic_cast<KPrDocument *>( m_doc );
    KPrCustomSlideShows *finalSlideShows;

    KPrCustomSlideShowsDialog dialog( this, doc->customSlideShows(), doc, finalSlideShows );
    dialog.setModal( true );
    if ( dialog.exec() == QDialog::Accepted ) {
        doc->setCustomSlideShows( finalSlideShows );
    }
    else {
        delete finalSlideShows;
    }
}

#include "KPrView.moc"
