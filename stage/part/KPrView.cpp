/* This file is part of the KDE project
   Copyright (C) 2006-2007 Thorsten Zachmann <zachmann@kde.org>
   Copyright (C) 2008 Carlos Licea <carlos.licea@kdemail.org>
   Copyright (C) 2009-2010 Benjamin Port <port.benjamin@gmail.com>
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

#include <QStatusBar>
#include <QFileDialog>
#include <QUrl>

#include <KSharedConfig>
#include <klocalizedstring.h>
#include <ktoggleaction.h>
#include <kactioncollection.h>
#include <kactionmenu.h>

#include <KoIcon.h>

#include <KoComponentData.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoMainWindow.h>
#include <KoPACanvasBase.h>
#include <KoDocumentInfo.h>
#include <KoShapeLayer.h>
#include <KoZoomController.h>
#include <KoToolManager.h>
#include <tools/backgroundTool/KoPABackgroundTool.h>

#include "KPrDocument.h"
#include "KPrPart.h"
#include "KPrPage.h"
#include "KPrMasterPage.h"
#include "KPrPageApplicationData.h"

#ifndef QT_NO_DBUS
#include "KPrViewAdaptor.h"
#endif

#include "KPrViewModePresentation.h"
#include "KPrViewModeNotes.h"
#include "KPrViewModeSlidesSorter.h"
#include "KPrShapeManagerDisplayMasterStrategy.h"
#include "KPrPageSelectStrategyActive.h"
#include "KPrPicturesImport.h"
#include "KPrFactory.h"
#include "commands/KPrAnimationCreateCommand.h"
#include "KPrPageLayoutWidget.h"
#include "KPrHtmlExport.h"
#include "KPrCustomSlideShows.h"
#include "ui/KPrConfigureSlideShowDialog.h"
#include "ui/KPrConfigurePresenterViewDialog.h"
#include "ui/KPrHtmlExportDialog.h"

#include "KPrPdfPrintJob.h"
#include "gemini/ViewModeSwitchEvent.h"

KPrView::KPrView(KPrPart *part, KPrDocument *document, QWidget *parent)
  : KoPAView(part, document, KoPAView::ModeBox, parent)
  , m_part(part)
  , m_presentationMode( new KPrViewModePresentation( this, kopaCanvas() ))
  , m_normalMode( viewMode() )
  , m_notesMode( new KPrViewModeNotes( this, kopaCanvas() ))
  , m_slidesSorterMode(new KPrViewModeSlidesSorter(this, kopaCanvas()))
#ifndef QT_NO_DBUS
  , m_dbus( new KPrViewAdaptor( this ) )
#endif
{
    m_normalMode->setName(i18n("Normal"));
    initGUI();
    initActions();

    // Change strings because in Stage it's called slides and not pages
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
    actionCollection()->action("page_previous")->setToolTip(i18n("Go to previous slide"));
    actionCollection()->action("page_previous")->setWhatsThis(i18n("Go to previous slide"));
    actionCollection()->action("page_next")->setText(i18n("Next Slide"));
    actionCollection()->action("page_next")->setToolTip(i18n("Go to next slide"));
    actionCollection()->action("page_next")->setWhatsThis(i18n("Go to next slide"));
    actionCollection()->action("page_first")->setText(i18n("First Slide"));
    actionCollection()->action("page_first")->setToolTip(i18n("Go to first slide"));
    actionCollection()->action("page_first")->setWhatsThis(i18n("Go to first slide"));
    actionCollection()->action("page_last")->setText(i18n("Last Slide"));
    actionCollection()->action("page_last")->setToolTip(i18n("Go to last slide"));
    actionCollection()->action("page_last")->setWhatsThis(i18n("Go to last slide"));
    actionCollection()->action("configure")->setText(i18n("Configure Stage..."));

    masterShapeManager()->setPaintingStrategy( new KPrShapeManagerDisplayMasterStrategy( masterShapeManager(),
                                                   new KPrPageSelectStrategyActive( kopaCanvas() ) ) );

    connect(zoomController(), SIGNAL(zoomChanged(KoZoomMode::Mode,qreal)), this, SLOT(zoomChanged(KoZoomMode::Mode,qreal)));

    setAcceptDrops(true);
}

KPrView::~KPrView()
{
    stopPresentation();
    saveZoomConfig(zoomMode(), zoom());
    delete m_presentationMode;
    delete m_notesMode;
    delete m_slidesSorterMode;
}

KoViewConverter * KPrView::viewConverter( KoPACanvasBase * canvas )
{
    Q_ASSERT(viewMode());
    return viewMode()->viewConverter( canvas );
}

KPrDocument * KPrView::kprDocument() const
{
    return static_cast<KPrDocument *>( kopaDocument() );
}

#ifndef QT_NO_DBUS
KPrViewAdaptor * KPrView::dbusObject() const
{
    return m_dbus;
}
#endif

KPrViewModePresentation * KPrView::presentationMode() const
{
    return m_presentationMode;
}


KPrViewModeSlidesSorter * KPrView::slidesSorter() const
{
    return m_slidesSorterMode;
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

void KPrView::showStatusBar(bool toggled)
{
    statusBar()->setVisible(toggled);
}

void KPrView::initGUI()
{
    // add page effect docker to the main window
    /*
    if (mainWindow()) {
        KPrPageLayoutDockerFactory pageLayoutFactory;
        KPrPageLayoutDocker *pageLayoutDocker = qobject_cast<KPrPageLayoutDocker*>( mainWindow()->createDockWidget( &pageLayoutFactory ) );
        pageLayoutDocker->setView( this );
    }
    }*/
    KoPABackgroundTool *designTool = dynamic_cast<KoPABackgroundTool *>(KoToolManager::instance()->toolById(kopaCanvas(), "KoPABackgroundTool"));
    if (designTool) {
        KPrPageLayoutWidget *plw = new KPrPageLayoutWidget();
        plw->setView(this);
        designTool->addOptionWidget(plw);
    }
    KConfigGroup group( KSharedConfig::openConfig(), "stage" );
    if ( !group.hasKey( "State" ) ) {
        QString state = "AAAA/wAAAAD9AAAAAgAAAAAAAAEHAAACdfwCAAAAA/sAAAAOAFQAbwBvAGwAQgBvAHgBAAAAUgAAAF8AAABIAP////sAAAAuAEsAbwBTAGgAYQBwAGUAQwBvAGwAbABlAGMAdABpAG8AbgBEAG8AYwBrAGUAcgEAAAC0AAAAZQAAAE0A////+wAAACoAZABvAGMAdQBtAGUAbgB0ACAAcwBlAGMAdABpAG8AbgAgAHYAaQBlAHcBAAABHAAAAasAAABvAP///wAAAAEAAADlAAACdfwCAAAAEPsAAAAgAFMAaABhAHAAZQAgAFAAcgBvAHAAZQByAHQAaQBlAHMAAAAAAP////8AAAAYAP////sAAAAiAFMAaABhAGQAbwB3ACAAUAByAG8AcABlAHIAdABpAGUAcwAAAAAA/////wAAAJ8A////+wAAACQAUwBpAG0AcABsAGUAIABUAGUAeAB0ACAARQBkAGkAdABvAHIAAAAAAP////8AAAFOAP////sAAAAwAEQAZQBmAGEAdQBsAHQAVABvAG8AbABBAHIAcgBhAG4AZwBlAFcAaQBkAGcAZQB0AQAAAFIAAABOAAAATgD////7AAAAIgBEAGUAZgBhAHUAbAB0AFQAbwBvAGwAVwBpAGQAZwBlAHQBAAAAowAAAGMAAABjAP////sAAAAqAFMAbgBhAHAARwB1AGkAZABlAEMAbwBuAGYAaQBnAFcAaQBkAGcAZQB0AQAAAQkAAABQAAAAUAD////7AAAAIgBTAHQAcgBvAGsAZQAgAFAAcgBvAHAAZQByAHQAaQBlAHMBAAABXAAAALcAAAC3AP////sAAAAWAFMAdAB5AGwAZQBEAG8AYwBrAGUAcgEAAAIWAAAAWAAAAFgA////+wAAABgAUwBsAGkAZABlACAAbABhAHkAbwB1AHQBAAACcQAAAFYAAABWAP////sAAAAoAFAAaQBjAHQAdQByAGUAVABvAG8AbABGAGEAYwB0AG8AcgB5AEkAZAEAAAN6AAAAMQAAAAAAAAAA+wAAACQAVABlAHgAdABUAG8AbwBsAEYAYQBjAHQAbwByAHkAXwBJAEQBAAADJwAAAIQAAAAAAAAAAPsAAAAoAEMAZQBsAGwAVABvAG8AbABPAHAAdABpAG8AbgBXAGkAZABnAGUAdAEAAALBAAAA6gAAAAAAAAAA+wAAADAASwBvAFAAQQBCAGEAYwBrAGcAcgBvAHUAbgBkAFQAbwBvAGwAVwBpAGQAZwBlAHQBAAADnQAAAFgAAAAAAAAAAPsAAAAeAEQAdQBtAG0AeQBUAG8AbwBsAFcAaQBkAGcAZQB0AQAAAqgAAAAaAAAAAAAAAAD7AAAAKABQAGEAdAB0AGUAcgBuAE8AcAB0AGkAbwBuAHMAVwBpAGQAZwBlAHQBAAACxQAAAIYAAAAAAAAAAPsAAAAoAEsAYQByAGIAbwBuAFAAYQB0AHQAZQByAG4AQwBoAG8AbwBzAGUAcgEAAANOAAAAXQAAAAAAAAAAAAADfgAAAnUAAAAEAAAABAAAAAgAAAAI/AAAAAEAAAACAAAAAQAAABYAbQBhAGkAbgBUAG8AbwBsAEIAYQByAQAAAAAAAAVwAAAAAAAAAAA=";
        group.writeEntry( "State", state );
    }
    initZoomConfig();

    //unhide tab bar and populate with view modes
    setTabBarPosition(Qt::Horizontal);
    tabBar()->show();
    tabBar()->addTab(m_normalMode->name());
    tabBar()->addTab(m_notesMode->name());
    tabBar()->addTab(m_slidesSorterMode->name());
    tabBar()->setCurrentIndex(0);
}

void KPrView::initActions()
{
    setComponentName(KPrFactory::componentData().componentName(), KPrFactory::componentData().componentDisplayName());
    if (!koDocument()->isReadWrite() )
       setXMLFile( "calligrastage_readonly.rc" );
    else
       setXMLFile( "calligrastage.rc" );

    // do special stage stuff here
    m_actionExportHtml = new QAction(i18n("Export as HTML..."), this);
    actionCollection()->addAction("file_export_html", m_actionExportHtml);
    connect(m_actionExportHtml, SIGNAL(triggered()), this, SLOT(exportToHtml()));

    m_actionViewModeNormal = new QAction(m_normalMode->name(), this);
    m_actionViewModeNormal->setCheckable(true);
    m_actionViewModeNormal->setChecked(true);
    actionCollection()->setDefaultShortcut(m_actionViewModeNormal, QKeySequence("CTRL+F5"));
    actionCollection()->addAction("view_normal", m_actionViewModeNormal);
    connect(m_actionViewModeNormal, SIGNAL(triggered()), this, SLOT(showNormal()));

    m_actionViewModeNotes = new QAction(m_notesMode->name(), this);
    m_actionViewModeNotes->setCheckable(true);
    actionCollection()->setDefaultShortcut(m_actionViewModeNotes, QKeySequence("CTRL+F6"));
    actionCollection()->addAction("view_notes", m_actionViewModeNotes);
    connect(m_actionViewModeNotes, SIGNAL(triggered()), this, SLOT(showNotes()));

    m_actionViewModeSlidesSorter = new QAction(m_slidesSorterMode->name(), this);
    m_actionViewModeSlidesSorter->setCheckable(true);
    actionCollection()->setDefaultShortcut(m_actionViewModeSlidesSorter, QKeySequence("CTRL+F7"));
    actionCollection()->addAction("view_slides_sorter", m_actionViewModeSlidesSorter);
    connect(m_actionViewModeSlidesSorter, SIGNAL(triggered()), this, SLOT(showSlidesSorter()));

    if ( QAction *action = actionCollection()->action("view_masterpages") )
        actionCollection()->setDefaultShortcut(action, QKeySequence("CTRL+F8"));

    m_actionInsertPictures = new QAction(i18n("Insert Pictures as Slides..."), this);
    actionCollection()->addAction("insert_pictures", m_actionInsertPictures);
    connect(m_actionInsertPictures, SIGNAL(triggered()), this, SLOT(insertPictures()));

    QActionGroup *viewModesGroup = new QActionGroup(this);
    viewModesGroup->addAction(m_actionViewModeNormal);
    viewModesGroup->addAction(m_actionViewModeNotes);
    viewModesGroup->addAction(m_actionViewModeSlidesSorter);

    m_actionCreateAnimation = new QAction( i18n( "Create Appear Animation" ), this );
    actionCollection()->addAction( "edit_createanimation", m_actionCreateAnimation );
    connect( m_actionCreateAnimation, SIGNAL(triggered()), this, SLOT(createAnimation()) );

    m_actionEditCustomSlideShows = new QAction( i18n( "Edit Custom Slide Shows..." ), this );
    actionCollection()->addAction( "edit_customslideshows", m_actionEditCustomSlideShows );
    connect( m_actionEditCustomSlideShows, SIGNAL(triggered()), this, SLOT(editCustomSlideShows()) );

    KActionMenu *actionStartPresentation = new KActionMenu(koIcon("view-presentation"), i18n("Start Presentation"), this);
    actionCollection()->addAction( "slideshow_start", actionStartPresentation );
    connect( actionStartPresentation, SIGNAL(triggered()), this, SLOT(startPresentation()) ); // for the toolbar button
    QAction* action = new QAction( i18n( "From Current Slide" ), this );
    action->setShortcut(QKeySequence("Shift+F5"));
    actionStartPresentation->addAction( action );
    connect( action, SIGNAL(triggered()), this, SLOT(startPresentation()) );
    action = new QAction( i18n( "From First Slide" ), this );
    action->setShortcut(QKeySequence("F5"));
    actionStartPresentation->addAction( action );
    connect( action, SIGNAL(triggered()), this, SLOT(startPresentationFromBeginning()) );

    m_actionStopPresentation = new QAction( i18n( "Stop presentation" ), this );
    actionCollection()->addAction( "slideshow_stop", m_actionStopPresentation );
    m_actionStopPresentation->setShortcut(Qt::Key_Escape);
    connect(m_actionStopPresentation, SIGNAL(triggered()), this, SLOT(stopPresentation()));
    m_actionStopPresentation->setEnabled(false);

    KToggleAction *showStatusbarAction = new KToggleAction(i18n("Show Status Bar"), this);
    showStatusbarAction->setCheckedState(KGuiItem(i18n("Hide Status Bar")));
    showStatusbarAction->setToolTip(i18n("Shows or hides the status bar"));
    actionCollection()->addAction("showStatusBar", showStatusbarAction);
    connect(showStatusbarAction, SIGNAL(toggled(bool)), this, SLOT(showStatusBar(bool)));

    //Update state of status bar action
    if (showStatusbarAction && statusBar()){
        showStatusbarAction->setChecked(! statusBar()->isHidden());
    }

    action = new QAction( i18n( "Configure Slide Show..." ), this );
    actionCollection()->addAction( "slideshow_configure", action );
    connect( action, SIGNAL(triggered()), this, SLOT(configureSlideShow()) );

    action = new QAction( i18n( "Configure Presenter View..." ), this );
    actionCollection()->addAction( "slideshow_presenterview", action );
    connect( action, SIGNAL(triggered()), this, SLOT(configurePresenterView()) );

    m_actionDrawOnPresentation = new QAction( i18n( "Draw on the presentation..." ), this );
    actionCollection()->setDefaultShortcut(m_actionDrawOnPresentation, Qt::Key_P);
    m_actionDrawOnPresentation->setShortcutContext(Qt::ApplicationShortcut);
    actionCollection()->addAction( "draw_on_presentation", m_actionDrawOnPresentation );
    connect( m_actionDrawOnPresentation, SIGNAL(triggered()), this, SLOT(drawOnPresentation()) );
    m_actionDrawOnPresentation->setEnabled(false);

    m_actionHighlightPresentation = new QAction( i18n( "Highlight the presentation..." ), this );
    actionCollection()->setDefaultShortcut(m_actionHighlightPresentation, Qt::Key_H);
    m_actionHighlightPresentation->setShortcutContext(Qt::ApplicationShortcut);
    actionCollection()->addAction( "highlight_presentation", m_actionHighlightPresentation );
    connect( m_actionHighlightPresentation, SIGNAL(triggered()), this, SLOT(highlightPresentation()) );
    m_actionHighlightPresentation->setEnabled(false);

    m_actionBlackPresentation = new QAction( i18n( "Blackscreen on the presentation..." ), this );
    actionCollection()->setDefaultShortcut(m_actionBlackPresentation, Qt::Key_B);
    m_actionBlackPresentation->setShortcutContext(Qt::ApplicationShortcut);
    actionCollection()->addAction( "black_presentation", m_actionBlackPresentation );
    connect( m_actionBlackPresentation, SIGNAL(triggered()), this, SLOT(blackPresentation()) );
    m_actionBlackPresentation->setEnabled(false);

    connect(tabBar(), SIGNAL(currentChanged(int)), this, SLOT(changeViewByIndex(int)));
}

bool KPrView::event(QEvent* event)
{
    switch(static_cast<int>(event->type())) {
        case ViewModeSwitchEvent::AboutToSwitchViewModeEvent: {
            ViewModeSynchronisationObject* syncObject = static_cast<ViewModeSwitchEvent*>(event)->synchronisationObject();
            if (activePage()) {
                syncObject->currentIndex = kopaDocument()->pageIndex(activePage());
                syncObject->shapes = shapeManager()->shapes();
                syncObject->initialized = true;
            }

            return true;
        }
        case ViewModeSwitchEvent::SwitchedToDesktopModeEvent: {
            ViewModeSynchronisationObject* syncObject = static_cast<ViewModeSwitchEvent*>(event)->synchronisationObject();
            if (syncObject->initialized) {
                shapeManager()->setShapes(syncObject->shapes);
                doUpdateActivePage( kopaDocument()->pageByIndex(syncObject->currentIndex, false) );
                KoToolManager::instance()->switchToolRequested("InteractionTool");
            }

            return true;
        }
    }
    return QWidget::event(event);
}

void KPrView::startPresentation()
{
    m_actionDrawOnPresentation->setEnabled(true);
    m_actionHighlightPresentation->setEnabled(true);
    m_actionBlackPresentation->setEnabled(true);
    m_actionStopPresentation->setEnabled(true);
    setViewMode( m_presentationMode );
}

void KPrView::startPresentationFromBeginning()
{
    KPrDocument * doc = static_cast<KPrDocument *>( kopaDocument() );
    QList<KoPAPageBase*> slideshow = doc->slideShow();
    if ( !slideshow.isEmpty() ) {
        setActivePage( slideshow.first() );
    }
    startPresentation();
}

void KPrView::stopPresentation()
{
    m_actionDrawOnPresentation->setEnabled(false);
    m_actionHighlightPresentation->setEnabled(false);
    m_actionBlackPresentation->setEnabled(false);
    m_actionStopPresentation->setEnabled(false);

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
    QAction *action = actionCollection()->action("view_normal");
    tabBar()->setCurrentIndex(0);
    if (action){
        action-> setChecked(true);
    }
}

void KPrView::showNotes()
{
    // Make sure that we are not in master mode
    // since notes master is not supported yet
    if ( viewMode()->masterMode() ) {
        actionCollection()->action( "view_masterpages" )->setChecked( false );
        setMasterMode( false );
    }
    tabBar()->setCurrentIndex(1);
    setViewMode(m_notesMode);
}

void KPrView::showSlidesSorter()
{
    // Make sure that we are not in master mode
    // Sort master does not make sense
    if ( viewMode()->masterMode() ) {
        actionCollection()->action( "view_masterpages" )->setChecked( false );
        setMasterMode( false );
    }
    tabBar()->setCurrentIndex(2);
    setViewMode(m_slidesSorterMode);
}


void KPrView::changeViewByIndex(int index)
{
    switch (index) {
    case 0:
        m_actionViewModeNormal->trigger();
        break;
    case 1:
        m_actionViewModeNotes->trigger();
        break;
    case 2:
        m_actionViewModeSlidesSorter->trigger();
        break;
    default:
        break;
    }
}

void KPrView::editCustomSlideShows()
{
    slidesSorter()->setActiveCustomSlideShow(1);
    showSlidesSorter();
}

void KPrView::configureSlideShow()
{
    KPrDocument *doc = static_cast<KPrDocument *>( kopaDocument() );
    KPrConfigureSlideShowDialog *dialog = new KPrConfigureSlideShowDialog(doc, this);

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
        QUrl directoryUrl = QFileDialog::getExistingDirectoryUrl();
        if (directoryUrl.isValid()) {
            KPrHtmlExport exportHtml;
            exportHtml.exportHtml(KPrHtmlExport::Parameter(dialog->templateUrl(), this, dialog->checkedSlides(),
                                                           directoryUrl, dialog->author(),
                                                           dialog->title(), dialog->slidesNames(), dialog->openBrowser()));
        }
   }
}

KoPrintJob *KPrView::createPdfPrintJob()
{
    return new KPrPdfPrintJob(this);
}


void KPrView::insertPictures()
{
    // Make sure that we are in the normal mode and not on master pages
    setViewMode(m_normalMode);
    if (viewMode()->masterMode()) {
        setMasterMode(false);
    }
    KPrPicturesImport pictureImport;
    pictureImport.import(this);
}

void KPrView::initZoomConfig()
{
    KSharedConfigPtr config = KPrFactory::componentData().config();
    int m_zoom = 100;
    KoZoomMode::Mode m_zoomMode = KoZoomMode::ZOOM_PAGE;

    if (config->hasGroup("Interface")) {
        const KConfigGroup interface = config->group("Interface");
        m_zoom = interface.readEntry("Zoom", m_zoom);
        m_zoomMode = static_cast<KoZoomMode::Mode>(interface.readEntry("ZoomMode", (int) m_zoomMode));
    }
    zoomController()->setZoom(m_zoomMode, m_zoom/100.);
    setZoom(m_zoomMode, m_zoom);
    centerPage();
}

void KPrView::zoomChanged(KoZoomMode::Mode mode, qreal zoom)
{
    setZoom(mode, qRound(zoom * 100.));
}

void KPrView::saveZoomConfig(KoZoomMode::Mode mode, int zoom)
{
    KSharedConfigPtr config = KPrFactory::componentData().config();

    if (config->hasGroup("Interface")) {
        KConfigGroup interface = config->group("Interface");
        interface.writeEntry("Zoom", zoom);
        interface.writeEntry("ZoomMode", (int)mode);
    }
}

void KPrView::setZoom(KoZoomMode::Mode zoomMode, int zoom)
{
    m_zoom = zoom;
    m_zoomMode = zoomMode;
}

int KPrView::zoom()
{
    return m_zoom;
}

KoZoomMode::Mode KPrView::zoomMode()
{
    return m_zoomMode;
}

void KPrView::restoreZoomConfig()
{
    zoomController()->setZoom(zoomMode(), zoom()/100.);
    centerPage();
}

void KPrView::replaceActivePage(KoPAPageBase *page, KoPAPageBase *newActivePage)
{
    if (page == activePage() ) {
        viewMode()->updateActivePage(newActivePage);
    }
}
