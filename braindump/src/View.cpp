/*
 *  Copyright (c) 2009,2010 Cyrille Berger <cberger@cberger.net>
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
#include <QString>
#include <QVariant>
#include <QToolBar>
#include <QScrollBar>
#include <QStatusBar>
#include <QTimer>
#include <QApplication>
#include <QClipboard>
#include <QPluginLoader>
#include <QMimeData>
#include <QDebug>
#include <QAction>

#include <kactioncollection.h>
#include <KPluginFactory>

#include <KoCanvasControllerWidget.h>
#include <KoToolManager.h>
#include <KoToolProxy.h>
#include <KoZoomHandler.h>
#include <KoShapeController.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoMainWindow.h>
#include <KoDockRegistry.h>
#include <KoShapeLayer.h>
#include <KoDrag.h>
#include <KoCutController.h>
#include <KoCopyController.h>
#include "KoZoomController.h"
#include <KoZoomAction.h>
#include <KoIcon.h>
#include "KoToolBoxFactory.h"
#include <KoPluginLoader.h>
#include "KoOdf.h"
#include "KoShapeGroup.h"
#include "KoShapeDeleteCommand.h"
#include "KoShapeCreateCommand.h"
#include "KoShapeGroupCommand.h"
#include "KoShapeUngroupCommand.h"

#include "Canvas.h"
#include "RootSection.h"
#include "Section.h"
#include "ViewManager.h"
#include "import/DockerManager.h"
#include "MainWindow.h"
#include "SectionContainer.h"
#include "SectionsBoxDock.h"
#include "Layout.h"
#include "SectionPropertiesDock.h"
#include "commands/RememberPositionCommand.h"

View::View(RootSection *document, MainWindow* parent)
    : QWidget(parent)
    , m_doc(document)
    , m_canvas(0)
    , m_activeSection(0)
    , m_mainWindow(parent)
    , m_cutController(0)
    , m_copyController(0)
{
    setXMLFile("braindumpview.rc");

    m_doc->viewManager()->addView(this);

    m_editPaste = actionCollection()->addAction(KStandardAction::Paste, "edit_paste", this, SLOT(editPaste()));
    m_editCut = actionCollection()->addAction(KStandardAction::Cut, "edit_cut", 0, 0);
    m_editCopy = actionCollection()->addAction(KStandardAction::Copy, "edit_copy", 0, 0);
    initGUI();
    initActions();
    loadExtensions();

    if(m_doc->sections().count() > 0) {
        setActiveSection(m_doc->sections()[0]);
    } else {
        setActiveSection(0);
    }

    m_doc->viewManager()->viewHasFocus(this);
}

View::~View()
{
    m_doc->viewManager()->removeView(this);
    KoToolManager::instance()->removeCanvasController(m_canvasController);
    delete m_zoomController;
}


Section* View::activeSection() const
{
    return m_activeSection;
}

void View::initGUI()
{
    // add all plugins.
    foreach(const QString & docker, KoDockRegistry::instance()->keys()) {
        qDebug() << "Creating docker: " << docker;
        KoDockFactoryBase *factory = KoDockRegistry::instance()->value(docker);
        m_mainWindow->createDockWidget(factory);
    }

    // Init the widgets
    QGridLayout * gridLayout = new QGridLayout(this);
    gridLayout->setMargin(0);
    gridLayout->setSpacing(0);
    setLayout(gridLayout);


    m_canvasController = new KoCanvasControllerWidget(actionCollection(), this);
    m_canvasController->setCanvasMode(KoCanvasController::Infinite);

    createCanvas(0);

    KoToolManager::instance()->addController(m_canvasController);
    KoToolManager::instance()->registerTools(actionCollection(), m_canvasController);

    m_zoomController = new KoZoomController(m_canvasController, &m_zoomHandler, actionCollection());
    connect(m_zoomController, SIGNAL(zoomChanged(KoZoomMode::Mode,qreal)),
            this, SLOT(slotZoomChanged(KoZoomMode::Mode,qreal)));

    m_zoomAction = m_zoomController->zoomAction();
    m_mainWindow->addStatusBarItem(m_zoomAction->createWidget(m_mainWindow->statusBar()), 0, this);

    m_zoomController->setZoomMode(KoZoomMode::ZOOM_WIDTH);

    gridLayout->addWidget(m_canvasController, 1, 1);

    connect(m_canvasController->proxyObject, SIGNAL(canvasMousePositionChanged(QPoint)),
            this, SLOT(updateMousePosition(QPoint)));

    KoToolBoxFactory toolBoxFactory;
    m_mainWindow->createDockWidget(&toolBoxFactory);

    connect(m_canvasController, SIGNAL(toolOptionWidgetsChanged(QList<QPointer<QWidget> >)), m_mainWindow->dockerManager(), SLOT(newOptionWidgets(QList<QPointer<QWidget> >)));

    SectionsBoxDockFactory structureDockerFactory;
    m_sectionsBoxDock = qobject_cast<SectionsBoxDock*>(m_mainWindow->createDockWidget(&structureDockerFactory));
    Q_ASSERT(m_sectionsBoxDock);
    m_sectionsBoxDock->setup(m_doc, this);

    SectionPropertiesDockFactory sectionPropertiesDockerFactory;
    m_sectionPropertiesDock = qobject_cast<SectionPropertiesDock*>(m_mainWindow->createDockWidget(&sectionPropertiesDockerFactory));
    Q_ASSERT(m_sectionPropertiesDock);
    m_sectionPropertiesDock->setRootSection(m_doc);

    KoToolManager::instance()->requestToolActivation(m_canvasController);

    show();
}

void View::initActions()
{
    connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()));
    clipboardDataChanged();
    actionCollection()->addAction(KStandardAction::SelectAll,  "edit_select_all", this, SLOT(editSelectAll()));
    actionCollection()->addAction(KStandardAction::Deselect,  "edit_deselect_all", this, SLOT(editDeselectAll()));

    m_deleteSelectionAction = new QAction(koIcon("edit-delete"), i18n("D&elete"), this);
    actionCollection()->addAction("edit_delete", m_deleteSelectionAction);
    actionCollection()->setDefaultShortcut(m_deleteSelectionAction, QKeySequence("Del"));
    connect(m_deleteSelectionAction, SIGNAL(triggered()), this, SLOT(editDeleteSelection()));

    // Shapes menu
    // TODO: get an icon "edit-duplicate"
    QAction *actionDuplicate  = new QAction(i18nc("Duplicate selection", "&Duplicate"), this);
    actionCollection()->addAction("shapes_duplicate", actionDuplicate);
    actionCollection()->setDefaultShortcut(actionDuplicate, QKeySequence("Ctrl+D"));
    connect(actionDuplicate, SIGNAL(triggered()), this, SLOT(selectionDuplicate()));

    m_groupShapes = new QAction(koIcon("object-group"), i18n("Group Shapes"), this);
    actionCollection()->addAction("shapes_group", m_groupShapes);
    actionCollection()->setDefaultShortcut(m_groupShapes, QKeySequence("Ctrl+G"));
    connect(m_groupShapes, SIGNAL(triggered()), this, SLOT(groupSelection()));

    m_ungroupShapes  = new QAction(koIcon("object-ungroup"), i18n("Ungroup Shapes"), this);
    actionCollection()->addAction("shapes_ungroup", m_ungroupShapes);
    actionCollection()->setDefaultShortcut(m_ungroupShapes, QKeySequence("Ctrl+Shift+G"));
    connect(m_ungroupShapes, SIGNAL(triggered()), this, SLOT(ungroupSelection()));

}

void View::loadExtensions()
{
    const QList<KPluginFactory *> pluginFactories =
        KoPluginLoader::instantiatePluginFactories(QStringLiteral("braindump/extensions"));

    foreach (KPluginFactory* factory, pluginFactories) {
        QObject *object = factory->create<QObject>(this, QVariantList());
        KXMLGUIClient *clientPlugin = dynamic_cast<KXMLGUIClient*>(object);
        if (clientPlugin) {
            insertChildClient(clientPlugin);
        } else {
            // not our/valid plugin, so delete the created object
            object->deleteLater();
        }
    }
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
    if(!selection)
        return;

    KoShapeLayer *layer = activeSection()->sectionContainer()->layer();

    QList<KoShape*> layerShapes(layer->shapes());
    foreach(KoShape * layerShape, layerShapes) {
        selection->select(layerShape);
        layerShape->update();
    }
}

void View::editDeselectAll()
{
    KoSelection* selection = canvas()->shapeManager()->selection();
    if(selection)
        selection->deselectAll();

    canvas()->update();
}

void View::slotZoomChanged(KoZoomMode::Mode mode, qreal zoom)
{
    Q_UNUSED(mode);
    Q_UNUSED(zoom);
    canvas()->updateOriginAndSize();
    canvas()->update();
}

void View::createCanvas(Section* _currentSection)
{
    Canvas* canvas = new Canvas(this, m_doc, _currentSection);
    m_canvasController->setCanvas(canvas);
    // No need to delete the current canvas, it will be deleted in Viewport::setCanvas (flake/KoCanvasController_p.cpp)
    m_canvas = canvas;

    delete m_cutController;
    m_cutController = new KoCutController(m_canvas, m_editCut);
    delete m_copyController;
    m_copyController = new KoCopyController(m_canvas, m_editCopy);

    connect(m_canvas, SIGNAL(canvasReceivedFocus()), SLOT(canvasReceivedFocus()));
    connect(m_canvas, SIGNAL(documentRect(QRectF)), SLOT(documentRectChanged(QRectF)));
    connect(m_canvasController->proxyObject, SIGNAL(moveDocumentOffset(QPoint)),
            m_canvas, SLOT(setDocumentOffset(QPoint)));
    connect(m_canvas->toolProxy(), SIGNAL(toolChanged(QString)), this, SLOT(clipboardDataChanged()));

    m_canvas->updateOriginAndSize();

    setEnabled(_currentSection);
}

void View::setActiveSection(Section* page)
{

    m_activeSection = page;

    m_doc->setCurrentSection(page);

    createCanvas(m_activeSection);

    if(m_activeSection) {
        documentRectChanged(m_activeSection->layout()->boundingBox());
    }

    m_sectionsBoxDock->updateGUI();
    m_sectionPropertiesDock->setSection(m_activeSection);
}

void View::updateMousePosition(const QPoint& /*position*/)
{
    QPoint canvasOffset(m_canvasController->canvasOffsetX(), m_canvasController->canvasOffsetY());
    // the offset is positive it the canvas is shown fully visible
    canvasOffset.setX(canvasOffset.x() < 0 ? canvasOffset.x() : 0);
    canvasOffset.setY(canvasOffset.y() < 0 ? canvasOffset.y() : 0);
}

void View::clipboardDataChanged()
{
    const QMimeData* data = QApplication::clipboard()->mimeData();
    bool paste = false;

    if(data) {
        // TODO see if we can use the KoPasteController instead of having to add this feature in each calligra app.
        QStringList mimeTypes = m_canvas->toolProxy()->supportedPasteMimeTypes();
        mimeTypes << KoOdf::mimeType(KoOdf::Graphics);
        mimeTypes << KoOdf::mimeType(KoOdf::Presentation);

        foreach(const QString & mimeType, mimeTypes) {
            if(data->hasFormat(mimeType)) {
                paste = true;
                break;
            }
        }

    }

    m_editPaste->setEnabled(paste);
}

void View::focusInEvent(QFocusEvent * event)
{
    QWidget::focusInEvent(event);
    m_doc->viewManager()->viewHasFocus(this);
}

void View::canvasReceivedFocus()
{
    m_doc->viewManager()->viewHasFocus(this);
}

void View::documentRectChanged(const QRectF& bb)
{
    QSizeF pageSize(400, 400);
    // Make sure we never use an empty size
    if(!bb.isNull() && !bb.isEmpty()) {
        pageSize = bb.size();
    }
    m_zoomController->setPageSize(pageSize);
    m_zoomController->setDocumentSize(pageSize);
}

void View::selectionDuplicate()
{
    m_canvas->toolProxy()->copy();
    m_canvas->toolProxy()->paste();
}

void View::groupSelection()
{
    KoSelection* selection = m_canvas->shapeManager()->selection();
    if(! selection)
        return;

    QList<KoShape*> selectedShapes = selection->selectedShapes(KoFlake::TopLevelSelection);
    QList<KoShape*> groupedShapes;

    // only group shapes with an unselected parent
    foreach(KoShape * shape, selectedShapes) {
        if(selectedShapes.contains(shape->parent()))
            continue;
        groupedShapes << shape;
    }
    KoShapeGroup *group = new KoShapeGroup();
    if(selection->activeLayer())
        selection->activeLayer()->addShape(group);
    KUndo2Command *cmd = new KUndo2Command(kundo2_i18n("Group shapes"));
    new KoShapeCreateCommand(m_activeSection->sectionContainer(), group, cmd);
    new KoShapeGroupCommand(group, groupedShapes, cmd);
    m_canvas->addCommand(cmd);
}

void View::ungroupSelection()
{
    KoSelection* selection = m_canvas->shapeManager()->selection();
    if(! selection)
        return;

    QList<KoShape*> selectedShapes = selection->selectedShapes(KoFlake::TopLevelSelection);
    QList<KoShape*> containerSet;

    // only ungroup shape containers with an unselected parent
    foreach(KoShape * shape, selectedShapes) {
        if(selectedShapes.contains(shape->parent()))
            continue;
        containerSet << shape;
    }

    KUndo2Command *cmd = new KUndo2Command(kundo2_i18n("Ungroup shapes"));

    // add a ungroup command for each found shape container to the macro command
    foreach(KoShape * shape, containerSet) {
        KoShapeContainer *container = dynamic_cast<KoShapeContainer*>(shape);
        if(container) {
            new KoShapeUngroupCommand(container, container->shapes(), QList<KoShape*>(), cmd);
            new KoShapeDeleteCommand(m_activeSection->sectionContainer(), container, cmd);
            new RememberPositionCommand(container->shapes(), cmd);
        }
    }
    m_canvas->addCommand(cmd);
}
