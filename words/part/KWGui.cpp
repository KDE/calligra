/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2001 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2005-2007, 2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2010 Boudewijn Rempt <boud@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWGui.h"
#include "KWCanvas.h"
#include "KWDocument.h"
#include "KWPage.h"
#include "KWView.h"

#include <KActionCollection>
#include <KoCanvasControllerWidget.h>
#include <KoDockerManager.h>
#include <KoFlake.h>
#include <KoModeBoxFactory.h>
#include <KoRuler.h>
#include <KoRulerController.h>
#include <KoSelection.h>
#include <KoShape.h>
#include <KoShapeManager.h>
#include <KoToolManager.h>
#include <KoUnit.h>

#include <KoMainWindow.h>
#include <QAction>
#include <QGridLayout>
#include <QScrollBar>
#include <QTimer>

KWGui::KWGui(const QString &viewMode, KWView *parent)
    : QWidget(parent)
    , m_view(parent)
{
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setContentsMargins({});
    gridLayout->setSpacing(0);

    setMouseTracking(true);

    // Ruler
    m_horizontalRuler = new KoRuler(this, Qt::Horizontal, m_view->viewConverter());
    m_horizontalRuler->setShowMousePosition(true);
    m_horizontalRuler->setUnit(m_view->kwdocument()->unit());
    m_verticalRuler = new KoRuler(this, Qt::Vertical, m_view->viewConverter());
    m_verticalRuler->setUnit(m_view->kwdocument()->unit());
    m_verticalRuler->setShowMousePosition(true);

    m_canvas = new KWCanvas(viewMode, static_cast<KWDocument *>(m_view->koDocument()), m_view, this);
    KoCanvasControllerWidget *canvasController = new KoCanvasControllerWidget(m_view->actionCollection(), this);
    m_canvasController = canvasController;
    // We need to set this as QDeclarativeView sets them a bit different from QAbstractScrollArea
    canvasController->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    canvasController->setFocusPolicy(Qt::NoFocus);
    // setScene(0);

    canvasController->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_canvasController->setMargin(10);
    m_canvasController->setCanvas(m_canvas);
    m_canvasController->setCanvasMode(KoCanvasController::AlignTop);
    KoToolManager::instance()->addController(m_canvasController);
    KoToolManager::instance()->registerTools(m_view->actionCollection(), m_canvasController);

    if (m_view->mainWindow()) {
        KoModeBoxFactory modeBoxFactory(canvasController, qApp->applicationName(), i18n("Tools"));
        QDockWidget *modeBox = m_view->mainWindow()->createDockWidget(&modeBoxFactory);
        m_view->mainWindow()->dockerManager()->removeToolOptionsDocker();
        dynamic_cast<KoCanvasObserverBase *>(modeBox)->setObservedCanvas(m_canvas);
    }

    gridLayout->addWidget(m_horizontalRuler->tabChooser(), 0, 0);
    gridLayout->addWidget(m_horizontalRuler, 0, 1);
    gridLayout->addWidget(m_verticalRuler, 1, 0);
    gridLayout->addWidget(canvasController, 1, 1);

    new KoRulerController(m_horizontalRuler, m_canvas->resourceManager());

    connect(m_view->kwdocument(), &KoDocument::unitChanged, m_horizontalRuler, &KoRuler::setUnit);
    connect(m_view->kwdocument(), &KoDocument::unitChanged, m_verticalRuler, &KoRuler::setUnit);
    connect(m_view->kwdocument(), &KWDocument::pageSetupChanged, this, &KWGui::pageSetupChanged);

    connect(m_canvasController->proxyObject, &KoCanvasControllerProxyObject::canvasOffsetXChanged, m_horizontalRuler, &KoRuler::setOffset);
    connect(m_canvasController->proxyObject, &KoCanvasControllerProxyObject::canvasOffsetYChanged, m_verticalRuler, &KoRuler::setOffset);
    connect(m_canvasController->proxyObject, &KoCanvasControllerProxyObject::canvasOffsetYChanged, parent, &KWView::offsetInDocumentMoved);
    connect(m_canvasController->proxyObject, &KoCanvasControllerProxyObject::canvasMousePositionChanged, this, &KWGui::updateMousePos);
    connect(m_canvasController->proxyObject, &KoCanvasControllerProxyObject::moveDocumentOffset, m_canvas, &KWCanvas::setDocumentOffset);

    connect(m_canvas->shapeManager()->selection(), &KoSelection::selectionChanged, this, &KWGui::shapeSelectionChanged);

    m_verticalRuler->createGuideToolConnection(m_canvas);
    m_horizontalRuler->createGuideToolConnection(m_canvas);

    pageSetupChanged();

    QTimer::singleShot(0, this, &KWGui::setupUnitActions);
}

KWGui::~KWGui() = default;

int KWGui::visibleWidth() const
{
    return m_canvasController->visibleWidth();
}

int KWGui::visibleHeight() const
{
    return m_canvasController->visibleHeight();
}

QSize KWGui::viewportSize() const
{
    return m_canvasController->viewportSize();
}

bool KWGui::horizontalScrollBarVisible()
{
    return static_cast<KoCanvasControllerWidget *>(m_canvasController)->horizontalScrollBar()
        && static_cast<KoCanvasControllerWidget *>(m_canvasController)->horizontalScrollBar()->isVisible();
}

void KWGui::pageSetupChanged()
{
    const KWPageManager *pm = m_view->kwdocument()->pageManager();
    const KWPage firstPage = pm->begin();
    const KWPage lastPage = pm->last();
    int height = 0;
    if (lastPage.isValid())
        height = lastPage.offsetInDocument() + lastPage.height();
    m_verticalRuler->setRulerLength(height);
    updateRulers();
    int width = 0;
    if (firstPage.isValid())
        width = firstPage.width();
    m_horizontalRuler->setRulerLength(width);
    m_horizontalRuler->setActiveRange(0, width);
    m_verticalRuler->setActiveRange(0, height);
    updateRulers();
}

void KWGui::updateMousePos(const QPoint &point)
{
    QPoint canvasOffset(m_canvasController->canvasOffsetX(), m_canvasController->canvasOffsetY());
    // the offset is positive it the canvas is shown fully visible
    canvasOffset.setX(canvasOffset.x() < 0 ? canvasOffset.x() : 0);
    canvasOffset.setY(canvasOffset.y() < 0 ? canvasOffset.y() : 0);
    QPoint viewPos = point - canvasOffset;
    m_horizontalRuler->updateMouseCoordinate(viewPos.x());
    m_verticalRuler->updateMouseCoordinate(viewPos.y());
}

void KWGui::updateRulers() const
{
    m_verticalRuler->setVisible(m_view->kwdocument()->config().viewRulers());
    m_horizontalRuler->setVisible(m_view->kwdocument()->config().viewRulers());
}

void KWGui::shapeSelectionChanged()
{
}

void KWGui::setupUnitActions()
{
    QList<QAction *> unitActions = m_view->createChangeUnitActions();
    QAction *separator = new QAction(this);
    separator->setSeparator(true);
    unitActions.append(separator);
    unitActions.append(m_view->actionCollection()->action("format_page"));
    m_horizontalRuler->setPopupActionList(unitActions);
}

void KWGui::mouseMoveEvent(QMouseEvent *e)
{
    m_view->viewMouseMoveEvent(e);
}
