/* This file is part of the KDE project
   Copyright (C) 2006-2007 Thorsten Zachmann <zachmann@kde.org>
   Copyright (C) 2008 Carlos Licea <carlos.licea@kdemail.org>
   Copyright (C) 2009 Benjamin Port <port.benjamin@gmail.com>
   Copyright (C) 2009 Yannick Motta <yannick.motta@gmail.com>

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
#include <kmessagebox.h>
#include <kfiledialog.h>

#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoMainWindow.h>
#include <KoPACanvas.h>
#include <KoPADocumentStructureDocker.h>
#include <KoDocumentInfo.h>

#include "KPrDocument.h"
#include "KPrPage.h"
#include "KPrMasterPage.h"
#include "KPrPageApplicationData.h"
#include "KPrViewAdaptor.h"
#include "KPrViewModePresentation.h"
#include "KPrViewModeNotes.h"
#include "KPrShapeManagerDisplayMasterStrategy.h"
#include "KPrPageSelectStrategyActive.h"
#include "commands/KPrAnimationCreateCommand.h"
#include "commands/KPrSetCustomSlideShowsCommand.h"
#include "dockers/KPrPageLayoutDockerFactory.h"
#include "dockers/KPrPageLayoutDocker.h"
#include "KPrHtmlExport.h"
#include "KPrCustomSlideShows.h"
#include "ui/KPrCustomSlideShowsDialog.h"
#include "ui/KPrConfigureSlideShowDialog.h"
#include "ui/KPrConfigurePresenterViewDialog.h"
#include "ui/KPrHtmlExportDialog.h"
#include <QtGui/QDesktopWidget>

#include <KDebug>

KPrView::KPrView( KPrDocument *document, QWidget *parent )
  : KoPAView( document, parent )
  , m_presentationMode( new KPrViewModePresentation( this, kopaCanvas() ))
  , m_normalMode( viewMode() )
  , m_notesMode( new KPrViewModeNotes( this, kopaCanvas() ))
  , m_dbus( new KPrViewAdaptor( this ) )
{
    initGUI();
    initActions();

    // Change strings because in KPresenter it's called slides and not pages
    actionCollection()->action("view_masterpages")->setText(i18n("Show Master Slides"));
    actionCollection()->action("import_document")->setText(i18n("Import Slideshow..."));
    actionCollection()->action("page_insertpage")->setText(i18n( "Insert Slide"));
    actionCollection()->action("page_insertpage")->setToolTip(i18n("Insert a new slide after the current one"));
    actionCollection()->action("page_insertpage")->setWhatsThis(i18n("Insert a new slide after the current one"));
    actionCollection()->action("page_copypage")->setText(i18n("Copy Slide"));
    actionCollection()->action("page_copypage")->setToolTip(i18n("Copy the current slide"));
    actionCollection()->action("page_copypage")->setWhatsThis(i18n("Copy the current slide"));
    actionCollection()->action("page_deletepage")->setText(i18n("Delete Slide"));
    actionCollection()->action("page_deletepage")->setToolTip(i18n("Delete the current slide"));
    actionCollection()->action("page_deletepage")->setWhatsThis(i18n("Delete the current slide"));
    actionCollection()->action("format_masterpage")->setText(i18n("Master Slide..."));
    actionCollection()->action("page_previous")->setText(i18n("Previous Slide"));
    actionCollection()->action("page_next")->setText(i18n("Next Slide"));
    actionCollection()->action("page_first")->setText(i18n("First Slide"));
    actionCollection()->action("page_last")->setText(i18n("Last Slide"));
    actionCollection()->action("configure")->setText(i18n("Configure KPresenter..."));

    masterShapeManager()->setPaintingStrategy( new KPrShapeManagerDisplayMasterStrategy( masterShapeManager(),
                                                   new KPrPageSelectStrategyActive( this ) ) );
}

KPrView::~KPrView()
{
    delete m_presentationMode;
    delete m_notesMode;
}

KoViewConverter * KPrView::viewConverter( KoPACanvas * canvas )
{
    return viewMode()->viewConverter( canvas );
}

KPrDocument * KPrView::kprDocument() const
{
    return static_cast<KPrDocument *>( kopaDocument() );
}

KPrViewAdaptor * KPrView::dbusObject() const
{
    return m_dbus;
}

KPrViewModePresentation * KPrView::presentationMode() const
{
    return m_presentationMode;
}

bool KPrView::isPresentationRunning() const
{
    return ( viewMode() == m_presentationMode );
}

void KPrView::drawOnPresentation()
{
    if (isPresentationRunning())
    {
        m_presentationMode->presentationTool()->drawOnPresentation();
    }
}

void KPrView::highlightPresentation()
{
    if (isPresentationRunning())
    {
        m_presentationMode->presentationTool()->highlightPresentation();
    }
}

void KPrView::blackPresentation()
{
    if (isPresentationRunning())
    {
        m_presentationMode->presentationTool()->blackPresentation();
    }
}

void KPrView::initGUI()
{
    // add page effect docker to the main window
    if (shell()) {
        KPrPageLayoutDockerFactory pageLayoutFactory;
        KPrPageLayoutDocker *pageLayoutDocker = qobject_cast<KPrPageLayoutDocker*>( shell()->createDockWidget( &pageLayoutFactory ) );
        pageLayoutDocker->setView( this );
    }

    QString state( "AAAA/wAAAAD9AAAAAgAAAAAAAAEHAAADWfwCAAAAA/sAAAAOAFQAbwBvAGwAQgBvAHgBAAAAUgAAAEgAAABIAP////sAAAAuAEsAbwBTAGgAYQBwAGUAQwBvAGwAbABlAGMAdABpAG8AbgBEAG8AYwBrAGUAcgEAAACdAAAAbAAAAE0A////+wAAACoAZABvAGMAdQBtAGUAbgB0ACAAcwBlAGMAdABpAG8AbgAgAHYAaQBlAHcBAAABDAAAAp8AAABvAP///wAAAAEAAAFjAAADWfwCAAAAEPsAAAAiAFMAdAByAG8AawBlACAAUAByAG8AcABlAHIAdABpAGUAcwAAAAAA/////wAAALcA////+wAAACAAUwBoAGEAcABlACAAUAByAG8AcABlAHIAdABpAGUAcwAAAAAA/////wAAABgA////+wAAACIAUwBoAGEAZABvAHcAIABQAHIAbwBwAGUAcgB0AGkAZQBzAAAAAAD/////AAAAnwD////7AAAAJABTAGkAbQBwAGwAZQAgAFQAZQB4AHQAIABFAGQAaQB0AG8AcgAAAAAA/////wAAAU4A////+wAAADAARABlAGYAYQB1AGwAdABUAG8AbwBsAEEAcgByAGEAbgBnAGUAVwBpAGQAZwBlAHQBAAAAUgAAAE4AAABOAP////sAAAAiAEQAZQBmAGEAdQBsAHQAVABvAG8AbABXAGkAZABnAGUAdAEAAACjAAAAYwAAAGMA////+wAAACoAUwBuAGEAcABHAHUAaQBkAGUAQwBvAG4AZgBpAGcAVwBpAGQAZwBlAHQBAAABCQAAAFIAAABQAP////sAAAAWAFMAdAB5AGwAZQBEAG8AYwBrAGUAcgEAAAFeAAABhAAAAFgA////+wAAABgAUwBsAGkAZABlACAAbABhAHkAbwB1AHQBAAAC5QAAAMYAAABWAP////sAAAAoAFAAaQBjAHQAdQByAGUAVABvAG8AbABGAGEAYwB0AG8AcgB5AEkAZAEAAAN6AAAAMQAAAAAAAAAA+wAAACQAVABlAHgAdABUAG8AbwBsAEYAYQBjAHQAbwByAHkAXwBJAEQBAAADJwAAAIQAAAAAAAAAAPsAAAAoAEMAZQBsAGwAVABvAG8AbABPAHAAdABpAG8AbgBXAGkAZABnAGUAdAEAAALBAAAA6gAAAAAAAAAA+wAAADAASwBvAFAAQQBCAGEAYwBrAGcAcgBvAHUAbgBkAFQAbwBvAGwAVwBpAGQAZwBlAHQBAAADnQAAAFgAAAAAAAAAAPsAAAAeAEQAdQBtAG0AeQBUAG8AbwBsAFcAaQBkAGcAZQB0AQAAAqgAAAAaAAAAAAAAAAD7AAAAKABQAGEAdAB0AGUAcgBuAE8AcAB0AGkAbwBuAHMAVwBpAGQAZwBlAHQBAAACxQAAAIYAAAAAAAAAAPsAAAAoAEsAYQByAGIAbwBuAFAAYQB0AHQAZQByAG4AQwBoAG8AbwBzAGUAcgEAAANOAAAAXQAAAAAAAAAAAAADAAAAA1kAAAAEAAAABAAAAAgAAAAI/AAAAAEAAAACAAAAAQAAABYAbQBhAGkAbgBUAG8AbwBsAEIAYQByAQAAAAAAAAVwAAAAAAAAAAA=" );
    state = "AAAA/wAAAAD9AAAAAgAAAAAAAAEHAAACdfwCAAAAA/sAAAAOAFQAbwBvAGwAQgBvAHgBAAAAUgAAAF8AAABIAP////sAAAAuAEsAbwBTAGgAYQBwAGUAQwBvAGwAbABlAGMAdABpAG8AbgBEAG8AYwBrAGUAcgEAAAC0AAAAZQAAAE0A////+wAAACoAZABvAGMAdQBtAGUAbgB0ACAAcwBlAGMAdABpAG8AbgAgAHYAaQBlAHcBAAABHAAAAasAAABvAP///wAAAAEAAADlAAACdfwCAAAAEPsAAAAgAFMAaABhAHAAZQAgAFAAcgBvAHAAZQByAHQAaQBlAHMAAAAAAP////8AAAAYAP////sAAAAiAFMAaABhAGQAbwB3ACAAUAByAG8AcABlAHIAdABpAGUAcwAAAAAA/////wAAAJ8A////+wAAACQAUwBpAG0AcABsAGUAIABUAGUAeAB0ACAARQBkAGkAdABvAHIAAAAAAP////8AAAFOAP////sAAAAwAEQAZQBmAGEAdQBsAHQAVABvAG8AbABBAHIAcgBhAG4AZwBlAFcAaQBkAGcAZQB0AQAAAFIAAABOAAAATgD////7AAAAIgBEAGUAZgBhAHUAbAB0AFQAbwBvAGwAVwBpAGQAZwBlAHQBAAAAowAAAGMAAABjAP////sAAAAqAFMAbgBhAHAARwB1AGkAZABlAEMAbwBuAGYAaQBnAFcAaQBkAGcAZQB0AQAAAQkAAABQAAAAUAD////7AAAAIgBTAHQAcgBvAGsAZQAgAFAAcgBvAHAAZQByAHQAaQBlAHMBAAABXAAAALcAAAC3AP////sAAAAWAFMAdAB5AGwAZQBEAG8AYwBrAGUAcgEAAAIWAAAAWAAAAFgA////+wAAABgAUwBsAGkAZABlACAAbABhAHkAbwB1AHQBAAACcQAAAFYAAABWAP////sAAAAoAFAAaQBjAHQAdQByAGUAVABvAG8AbABGAGEAYwB0AG8AcgB5AEkAZAEAAAN6AAAAMQAAAAAAAAAA+wAAACQAVABlAHgAdABUAG8AbwBsAEYAYQBjAHQAbwByAHkAXwBJAEQBAAADJwAAAIQAAAAAAAAAAPsAAAAoAEMAZQBsAGwAVABvAG8AbABPAHAAdABpAG8AbgBXAGkAZABnAGUAdAEAAALBAAAA6gAAAAAAAAAA+wAAADAASwBvAFAAQQBCAGEAYwBrAGcAcgBvAHUAbgBkAFQAbwBvAGwAVwBpAGQAZwBlAHQBAAADnQAAAFgAAAAAAAAAAPsAAAAeAEQAdQBtAG0AeQBUAG8AbwBsAFcAaQBkAGcAZQB0AQAAAqgAAAAaAAAAAAAAAAD7AAAAKABQAGEAdAB0AGUAcgBuAE8AcAB0AGkAbwBuAHMAVwBpAGQAZwBlAHQBAAACxQAAAIYAAAAAAAAAAPsAAAAoAEsAYQByAGIAbwBuAFAAYQB0AHQAZQByAG4AQwBoAG8AbwBzAGUAcgEAAANOAAAAXQAAAAAAAAAAAAADfgAAAnUAAAAEAAAABAAAAAgAAAAI/AAAAAEAAAACAAAAAQAAABYAbQBhAGkAbgBUAG8AbwBsAEIAYQByAQAAAAAAAAVwAAAAAAAAAAA=";
    KConfigGroup group( KGlobal::config(), "kpresenter" );
    if ( !group.hasKey( "State" ) ) {
        group.writeEntry( "State", state );
    }
}

void KPrView::initActions()
{
    if ( !kopaDocument()->isReadWrite() )
       setXMLFile( "kpresenter_readonly.rc" );
    else
       setXMLFile( "kpresenter.rc" );

    // do special kpresenter stuff here
    m_actionExportHtml = new KAction(i18n("Export as HTML..."), this);
    actionCollection()->addAction("file_export_html", m_actionExportHtml);
    connect(m_actionExportHtml, SIGNAL(triggered()), this, SLOT(exportToHtml()));

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

    m_actionStartPresentation = new KActionMenu( KIcon("view-presentation"), i18n( "Start Presentation" ), this );
    actionCollection()->addAction( "slideshow_start", m_actionStartPresentation );
    connect( m_actionStartPresentation, SIGNAL( activated() ), this, SLOT( startPresentation() ) );
    KAction* action = new KAction( i18n( "From Current Slide" ),
this );
    action->setShortcut(QKeySequence("Shift+F5"));
    m_actionStartPresentation->addAction( action );
    connect( action, SIGNAL( activated() ), this, SLOT( startPresentation() ) );
    action = new KAction( i18n( "From First Slide" ), this );
    action->setShortcut(QKeySequence("F5"));
    m_actionStartPresentation->addAction( action );
    connect( action, SIGNAL( activated() ), this, SLOT( startPresentationFromBeginning() ) );

    action = new KAction( i18n( "Configure Slide Show..." ), this );
    actionCollection()->addAction( "slideshow_configure", action );
    connect( action, SIGNAL( activated() ), this, SLOT( configureSlideShow() ) );

    action = new KAction( i18n( "Configure Presenter View..." ), this );
    actionCollection()->addAction( "slideshow_presenterview", action );
    connect( action, SIGNAL( activated() ), this, SLOT( configurePresenterView() ) );

    action = new KAction( i18n( "Draw on the presentation..." ), this );
    action->setShortcut(Qt::Key_P);
    action->setShortcutContext(Qt::ApplicationShortcut);
    actionCollection()->addAction( "draw_on_presentation", action );
    connect( action, SIGNAL( activated() ), this, SLOT( drawOnPresentation() ) );

    action = new KAction( i18n( "Highlight the presentation..." ), this );
    action->setShortcut(Qt::Key_H);
    action->setShortcutContext(Qt::ApplicationShortcut);
    actionCollection()->addAction( "highlight_presentation", action );
    connect( action, SIGNAL( activated() ), this, SLOT( highlightPresentation() ) );

    action = new KAction( i18n( "Blackscreen on the presentation..." ), this );
    action->setShortcut(Qt::Key_B);
    action->setShortcutContext(Qt::ApplicationShortcut);
    actionCollection()->addAction( "black_presentation", action );
    connect( action, SIGNAL( activated() ), this, SLOT( blackPresentation() ) );
}

void KPrView::startPresentation()
{
    setViewMode( m_presentationMode );
}

void KPrView::startPresentationFromBeginning()
{
    KPrDocument * doc = dynamic_cast<KPrDocument *>( kopaDocument() );
    QList<KoPAPageBase*> slideshow = doc->slideShow();
    if ( !slideshow.isEmpty() ) {
        setActivePage( slideshow.first() );
    }
    startPresentation();
}

void KPrView::stopPresentation()
{
    if ( isPresentationRunning() ) {
        m_presentationMode->activateSavedViewMode();
    }
}

void KPrView::createAnimation()
{
    static int animationcount = 0;
    KoSelection * selection = kopaCanvas()->shapeManager()->selection();
    QList<KoShape*> selectedShapes = selection->selectedShapes();
    foreach( KoShape * shape, selectedShapes )
    {
        Q_UNUSED(shape);
        /*KPrShapeAnimationOld * animation = new KPrAnimationMoveAppear( shape, animationcount );
        KPrDocument * doc = static_cast<KPrDocument *>( kopaDocument() );
        KPrAnimationCreateCommand * command = new KPrAnimationCreateCommand( doc, animation );
        kopaCanvas()->addCommand( command );*/
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
    if ( viewMode()->masterMode() ) {
        actionCollection()->action( "view_masterpages" )->setChecked( false );
        setMasterMode( false );
    }
    setViewMode(m_notesMode);
}

void KPrView::dialogCustomSlideShows()
{
    KPrDocument *doc = static_cast<KPrDocument *>( kopaDocument() );
    KPrCustomSlideShows *finalSlideShows;
    KPrCustomSlideShowsDialog dialog( this, doc->customSlideShows(), doc, finalSlideShows );
    dialog.setModal( true );
    if ( dialog.exec() == QDialog::Accepted ) {
        kopaCanvas()->addCommand( new KPrSetCustomSlideShowsCommand( doc, finalSlideShows ) );
    }
    else {
        delete finalSlideShows;
    }
}

void KPrView::configureSlideShow()
{
    KPrDocument *doc = static_cast<KPrDocument *>( kopaDocument() );
    KPrConfigureSlideShowDialog *dialog = new KPrConfigureSlideShowDialog( doc, this );

    if ( dialog->exec() == QDialog::Accepted ) {
        doc->setActiveCustomSlideShow( dialog->activeCustomSlideShow() );
    }
    delete dialog;
}

void KPrView::configurePresenterView()
{
    KPrDocument *doc = static_cast<KPrDocument *>( kopaDocument() );
    KPrConfigurePresenterViewDialog *dialog = new KPrConfigurePresenterViewDialog( doc, this );

    if ( dialog->exec() == QDialog::Accepted ) {
        doc->setPresentationMonitor( dialog->presentationMonitor() );
        doc->setPresenterViewEnabled( dialog->presenterViewEnabled() );
    }
    delete dialog;
}

void KPrView::exportToHtml()
{
    KPrHtmlExportDialog *dialog = new KPrHtmlExportDialog(kopaDocument()->pages(),koDocument()->documentInfo()->aboutInfo("title"),
                                                          koDocument()->documentInfo()->authorInfo("creator"), this);
    if (dialog->exec() == QDialog::Accepted && !dialog->checkedSlides().isEmpty()) {
        // Get the export directory
        KUrl directoryUrl = KFileDialog::getExistingDirectoryUrl();
        if (directoryUrl.isValid()) {
            directoryUrl.adjustPath(KUrl::AddTrailingSlash);
            KPrHtmlExport exportHtml;
            exportHtml.exportHtml(KPrHtmlExport::Parameter(dialog->templateUrl(), this, dialog->checkedSlides(),
                                                           directoryUrl, dialog->author(),
                                                           dialog->title(), dialog->slidesNames(), dialog->openBrowser()));
        }
   }
}

#include "KPrView.moc"
