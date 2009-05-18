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
#include <KoDockRegistry.h>
#include <KoShapeLayer.h>
#include <KoDrag.h>
#include <KoShapeDeleteCommand.h>
#include <KoCutController.h>
#include <KoCopyController.h>

#include "Canvas.h"
#include "RootSection.h"
#include "Section.h"
#include "ViewManager.h"
#include "import/DockerManager.h"

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
#include "KoOdf.h"

#include "MainWindow.h"

View::View( RootSection *document, MainWindow* parent )
: QWidget( parent )
, m_doc( document )
, m_activeSection( 0 )
, m_mainWindow(parent)
{
  
  m_doc->viewManager()->addView(this);
  
  initGUI();
  initActions();

  if ( m_doc->sections().count() > 0 )
    setActiveSection( m_doc->sections()[0] );

  setXMLFile("braindump.rc");
}

View::~View()
{
  m_doc->viewManager()->removeView(this);
  KoToolManager::instance()->removeCanvasController( m_canvasController );
  delete m_zoomController;
}


Section* View::activeSection() const
{
  return m_activeSection;
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

#if 0
    m_zoomAction = m_zoomController->zoomAction();
    addStatusBarItem( m_zoomAction->createWidget( statusBar() ), 0, true );
#endif

    m_zoomController->setZoomMode( KoZoomMode::ZOOM_PAGE );

    gridLayout->addWidget( m_canvasController, 1, 1 );

    connect(m_canvasController, SIGNAL(canvasMousePositionChanged(const QPoint&)),
             this, SLOT(updateMousePosition(const QPoint&)));

    KoToolBoxFactory toolBoxFactory(m_canvasController, i18n("Tools") );
    m_mainWindow->createDockWidget( &toolBoxFactory );

    connect( m_canvasController, SIGNAL( toolOptionWidgetsChanged(const QMap<QString, QWidget *> &) ), m_mainWindow->dockerManager(), SLOT( newOptionWidgets(const  QMap<QString, QWidget *> &) ) );

    connect(shapeManager(), SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
    connect(m_canvas, SIGNAL(documentSize(const QSize&)), m_canvasController, SLOT(setDocumentSize(const QSize&)));
    connect(m_canvasController, SIGNAL(moveDocumentOffset(const QPoint&)),
            m_canvas, SLOT(setDocumentOffset(const QPoint&)));

    SectionsBoxDockFactory structureDockerFactory;
    m_sectionsBoxDock = qobject_cast<SectionsBoxDock*>( m_mainWindow->createDockWidget( &structureDockerFactory ) );
    m_sectionsBoxDock->setup(m_doc, this);

    KoToolManager::instance()->requestToolActivation( m_canvasController );

    // add all plugins.
    foreach(const QString & docker, KoDockRegistry::instance()->keys()) {
        KoDockFactory *factory = KoDockRegistry::instance()->value(docker);
        m_mainWindow->createDockWidget(factory);
    }

    show();
}

void View::initActions()
{
    KAction *action = actionCollection()->addAction( KStandardAction::Cut, "edit_cut", 0, 0);
    new KoCutController(canvas(), action);
    action = actionCollection()->addAction( KStandardAction::Copy, "edit_copy", 0, 0 );
    new KoCopyController(canvas(), action);
    m_editPaste = actionCollection()->addAction( KStandardAction::Paste, "edit_paste", this, SLOT( editPaste() ) );
    connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()));
    connect(m_canvas->toolProxy(), SIGNAL(toolChanged(const QString&)), this, SLOT(clipboardDataChanged()));
    clipboardDataChanged();
    actionCollection()->addAction(KStandardAction::SelectAll,  "edit_select_all", this, SLOT(editSelectAll()));
    actionCollection()->addAction(KStandardAction::Deselect,  "edit_deselect_all", this, SLOT(editDeselectAll()));

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
    KoSelection* selection = canvas()->shapeManager()->selection();
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
    KoSelection* selection = canvas()->shapeManager()->selection();
    if( selection )
        selection->deselectAll();

    selectionChanged();
    canvas()->update();
}

void View::slotZoomChanged( KoZoomMode::Mode mode, qreal zoom )
{
    Q_UNUSED(mode);
    Q_UNUSED(zoom);
    canvas()->update();
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
  shapeManager()->removeAdditional( m_activeSection );
  m_activeSection = page;
  if(m_activeSection)
  {
    shapeManager()->addAdditional( m_activeSection );
    QList<KoShape*> shapes = page->iterator();
    shapeManager()->setShapes( shapes, KoShapeManager::AddWithoutRepaint );
    //Make the top most layer active
    if ( !shapes.isEmpty() ) {
        KoShapeLayer* layer = dynamic_cast<KoShapeLayer*>( shapes.last() );
        shapeManager()->selection()->setActiveLayer( layer );
    }
  }

  QSizeF pageSize( 1000, 1000 );
  m_zoomController->setPageSize( pageSize );
  m_zoomController->setDocumentSize( pageSize );
  m_canvas->sectionChanged(activeSection());

  m_canvas->update();
  m_sectionsBoxDock->updateGUI();
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

#include "View.moc"
