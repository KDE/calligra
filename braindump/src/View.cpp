/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "View.h"

#include <QGridLayout>
#include <QToolBar>
#include <QScrollBar>
#include <QTimer>
#include <QApplication>
#include <QClipboard>

#include <KoCanvasController.h>
#include <KoCanvasResourceProvider.h>
#include <KoToolManager.h>
#include <KoToolProxy.h>
#include <KoZoomHandler.h>
#include <KoToolBoxFactory.h>
#include <KoShapeController.h>
#include <KoShapeManager.h>
#include <KoZoomAction.h>
#include <KoZoomController.h>
#include <KoSelection.h>
#include <KoToolDocker.h>
#include <KoMainWindow.h>
#include <KoDockerManager.h>
#include <KoShapeLayer.h>
#include <KoDrag.h>
#include <KoShapeDeleteCommand.h>
#include <KoCutController.h>
#include <KoCopyController.h>

#include "Canvas.h"
#include "Document.h"
#include "Section.h"

#include <kdebug.h>
#include <klocale.h>
#include <kicon.h>
#include <ktoggleaction.h>
#include <kactionmenu.h>
#include <kactioncollection.h>
#include <kstatusbar.h>
#include <kparts/event.h>
#include <kparts/partmanager.h>
#include "SectionsBoxDock.h"

View::View( Document *document, QWidget *parent )
: KoView( document, parent )
, m_doc( document )
, m_activeSection( 0 )
{
    initGUI();
    initActions();

    if ( m_doc->sections().count() > 0 )
        doUpdateActiveSection( m_doc->sections()[0] );
    Q_ASSERT(activeSection());
    setXMLFile("braindump.rc");
}

View::~View()
{
    KoToolManager::instance()->removeCanvasController( m_canvasController );
    delete m_zoomController;
}


Section* View::activeSection() const
{
    return m_activeSection;
}

void View::updateReadWrite( bool readwrite )
{
    Q_UNUSED( readwrite );
}

void View::initGUI()
{
    QGridLayout * gridLayout = new QGridLayout( this );
    gridLayout->setMargin( 0 );
    gridLayout->setSpacing( 0 );
    setLayout( gridLayout );

    m_canvas = new Canvas( this, m_doc );
    m_canvasController = new KoCanvasController( this );
    m_canvasController->setCanvas( m_canvas );
    m_canvasController->setCanvasMode( KoCanvasController::Infinite );
    KoToolManager::instance()->addController( m_canvasController );
    KoToolManager::instance()->registerTools( actionCollection(), m_canvasController );

    m_zoomController = new KoZoomController( m_canvasController, &m_zoomHandler, actionCollection());
    connect( m_zoomController, SIGNAL( zoomChanged( KoZoomMode::Mode, qreal ) ),
             this, SLOT( slotZoomChanged( KoZoomMode::Mode, qreal ) ) );

    m_zoomAction = m_zoomController->zoomAction();
    addStatusBarItem( m_zoomAction->createWidget( statusBar() ), 0, true );

    m_zoomController->setZoomMode( KoZoomMode::ZOOM_PAGE );

    gridLayout->addWidget( m_canvasController, 1, 1 );

    connect(m_canvasController, SIGNAL(canvasMousePositionChanged(const QPoint&)),
             this, SLOT(updateMousePosition(const QPoint&)));

    KoToolBoxFactory toolBoxFactory(m_canvasController, i18n("Tools") );
    createDockWidget( &toolBoxFactory );

    KoDockerManager *dockerMng = dockerManager();
    if (!dockerMng) {
        dockerMng = new KoDockerManager(this);
        setDockerManager(dockerMng);
    }

    connect( m_canvasController, SIGNAL( toolOptionWidgetsChanged(const QMap<QString, QWidget *> &, KoView *) ),
             dockerMng, SLOT( newOptionWidgets(const  QMap<QString, QWidget *> &, KoView *) ) );

    connect(shapeManager(), SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
    connect(m_canvas, SIGNAL(documentSize(const QSize&)), m_canvasController, SLOT(setDocumentSize(const QSize&)));
    connect(m_canvasController, SIGNAL(moveDocumentOffset(const QPoint&)),
            m_canvas, SLOT(setDocumentOffset(const QPoint&)));

    if (shell()) {
        SectionsBoxDockFactory structureDockerFactory;
        m_sectionsBoxDock = qobject_cast<SectionsBoxDock*>( createDockWidget( &structureDockerFactory ) );
        m_sectionsBoxDock->setup(document(), this);
#if 0
        connect( shell()->partManager(), SIGNAL( activePartChanged( KParts::Part * ) ),
                m_documentStructureDocker, SLOT( setPart( KParts::Part * ) ) );
        connect(m_documentStructureDocker, SIGNAL(pageChanged(KoPAPageBase*)), this, SLOT(updateActivePage(KoPAPageBase*)));
        connect(m_documentStructureDocker, SIGNAL(dockerReset()), this, SLOT(reinitDocumentDocker()));

        KoToolManager::instance()->requestToolActivation( m_canvasController );
#endif
    }

    show();
}

void View::initActions()
{
    KAction *action = actionCollection()->addAction( KStandardAction::Cut, "edit_cut", 0, 0);
    new KoCutController(kopaCanvas(), action);
    action = actionCollection()->addAction( KStandardAction::Copy, "edit_copy", 0, 0 );
    new KoCopyController(kopaCanvas(), action);
    m_editPaste = actionCollection()->addAction( KStandardAction::Paste, "edit_paste", this, SLOT( editPaste() ) );
    connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()));
    connect(m_canvas->toolProxy(), SIGNAL(toolChanged(const QString&)), this, SLOT(clipboardDataChanged()));
    clipboardDataChanged();
    actionCollection()->addAction(KStandardAction::SelectAll,  "edit_select_all", this, SLOT(editSelectAll()));
    actionCollection()->addAction(KStandardAction::Deselect,  "edit_deselect_all", this, SLOT(editDeselectAll()));

    KToggleAction *showGrid= m_doc->gridData().gridToggleAction(m_canvas);
    actionCollection()->addAction("view_grid", showGrid );

    m_actionViewSnapToGrid = new KToggleAction(i18n("Snap to Grid"), this);
    m_actionViewSnapToGrid->setChecked(m_doc->gridData().snapToGrid());
    actionCollection()->addAction("view_snaptogrid", m_actionViewSnapToGrid);
    connect( m_actionViewSnapToGrid, SIGNAL( triggered( bool ) ), this, SLOT (viewSnapToGrid( bool )));

    m_actionViewShowGuides  = new KToggleAction( KIcon( "guides" ), i18n( "Show Guides" ), this );
    m_actionViewShowGuides->setChecked( m_doc->guidesData().showGuideLines() );
    m_actionViewShowGuides->setCheckedState( KGuiItem( i18n( "Hide Guides" ) ) );
    m_actionViewShowGuides->setToolTip( i18n( "Shows or hides guides" ) );
    actionCollection()->addAction( "view_show_guides", m_actionViewShowGuides );
    connect( m_actionViewShowGuides, SIGNAL(triggered(bool)), this, SLOT(viewGuides(bool)));
}

void View::viewSnapToGrid(bool snap)
{
    m_doc->gridData().setSnapToGrid(snap);
    m_actionViewSnapToGrid->setChecked(snap);
}

void View::viewGuides(bool show)
{
    m_doc->guidesData().setShowGuideLines(show);
    m_canvas->update();
}

void View::editPaste()
{
    m_canvas->toolProxy()->paste();
}

void View::editDeleteSelection()
{
    m_canvas->toolProxy()->deleteSelection();
}

void View::editSelectAll()
{
    KoSelection* selection = kopaCanvas()->shapeManager()->selection();
    if( !selection )
        return;

    QList<KoShape*> shapes = activeSection()->iterator();

    foreach( KoShape *shape, shapes ) {
        KoShapeLayer *layer = dynamic_cast<KoShapeLayer *>( shape );

        if ( layer ) {
            QList<KoShape*> layerShapes( layer->iterator() );
            foreach( KoShape *layerShape, layerShapes ) {
                selection->select( layerShape );
                layerShape->update();
            }
        }
    }

    selectionChanged();
}

void View::editDeselectAll()
{
    KoSelection* selection = kopaCanvas()->shapeManager()->selection();
    if( selection )
        selection->deselectAll();

    selectionChanged();
    kopaCanvas()->update();
}

void View::slotZoomChanged( KoZoomMode::Mode mode, qreal zoom )
{
    Q_UNUSED(mode);
    Q_UNUSED(zoom);
    kopaCanvas()->update();
}

KoShapeManager* View::shapeManager() const
{
    return m_canvas->shapeManager();
}

void View::reinitDocumentDocker()
{
/*    if (shell()) {
        m_documentStructureDocker->setActivePage( m_activePage );
    }*/
}

void View::setActiveSection( Section* page )
{
    if ( !page )
        return;

    shapeManager()->removeAdditional( m_activeSection );
    m_activeSection = page;
    shapeManager()->addAdditional( m_activeSection );
    QList<KoShape*> shapes = page->iterator();
    shapeManager()->setShapes( shapes, KoShapeManager::AddWithoutRepaint );
    //Make the top most layer active
    if ( !shapes.isEmpty() ) {
        KoShapeLayer* layer = dynamic_cast<KoShapeLayer*>( shapes.last() );
        shapeManager()->selection()->setActiveLayer( layer );
    }

    bool pageChanged = page != m_activeSection;

    QSizeF pageSize( 1000, 1000 );
    m_zoomController->setPageSize( pageSize );
    m_zoomController->setDocumentSize( pageSize );
    m_canvas->sectionChanged(activeSection());

    m_canvas->update();

    updatePageNavigationActions();
}

void View::navigatePage( KoPageApp::PageNavigation pageNavigation )
{
  Q_UNUSED(pageNavigation);
  qFatal("unimplemented");
/*    KoPAPageBase * newPage = m_doc->pageByNavigation( m_activePage, pageNavigation );

    if ( newPage != m_activePage ) {
        doUpdateActivePage( newPage );
    }*/
}

void View::updateMousePosition(const QPoint& position)
{
    QPoint canvasOffset( m_canvasController->canvasOffsetX(), m_canvasController->canvasOffsetY() );
    // the offset is positive it the canvas is shown fully visible
    canvasOffset.setX(canvasOffset.x() < 0 ? canvasOffset.x(): 0);
    canvasOffset.setY(canvasOffset.y() < 0 ? canvasOffset.y(): 0);
    QPoint viewPos = position - canvasOffset;

}

void View::selectionChanged()
{
}

void View::clipboardDataChanged()
{
    const QMimeData* data = QApplication::clipboard()->mimeData();
    bool paste = false;

    if (data)
    {
        // TODO see if we can use the KoPasteController instead of having to add this feature in each koffice app.
        QStringList mimeTypes = m_canvas->toolProxy()->supportedPasteMimeTypes();
        mimeTypes << KoOdf::mimeType( KoOdf::Graphics );
        mimeTypes << KoOdf::mimeType( KoOdf::Presentation );

        foreach(const QString & mimeType, mimeTypes)
        {
            if ( data->hasFormat( mimeType ) ) {
                paste = true;
                break;
            }
        }

    }

    m_editPaste->setEnabled(paste);
}

void View::goToPreviousPage()
{
  qFatal("unimplemented");
}

void View::goToNextPage()
{
  qFatal("unimplemented");
}

void View::goToFirstPage()
{
  qFatal("unimplemented");
}

void View::goToLastPage()
{
  qFatal("unimplemented");
}

void View::updatePageNavigationActions()
{
  // FIXME
    int index = 0 ; //m_doc->pageIndex(activePage());
    int pageCount = 0; //m_doc->pages(m_viewMode->masterMode()).count();

    actionCollection()->action("page_previous")->setEnabled(index > 0);
    actionCollection()->action("page_first")->setEnabled(index > 0);
    actionCollection()->action("page_next")->setEnabled(index < pageCount - 1);
    actionCollection()->action("page_last")->setEnabled(index < pageCount - 1);
}

#include "View.moc"
