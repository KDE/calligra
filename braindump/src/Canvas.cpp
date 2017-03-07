/*
 *  Copyright (c) 2006,2007 Thorsten Zachmann <zachmann@kde.org>
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

#include "Canvas.h"

#include <QMenu>
#include <QMouseEvent>
#include <QPainter>

#include <kxmlguifactory.h>
#include <kundo2command.h>

#include <KoCanvasController.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoShapeLayer.h>
#include <KoToolProxy.h>
#include <KoUnit.h>

#include "RootSection.h"
#include "View.h"
#include "ViewManager.h"
#include "Section.h"
#include "Layout.h"
#include "SectionContainer.h"

Canvas::Canvas(View* view, RootSection* doc, Section* currentSection)
    : QWidget(view)
    , KoCanvasBase(currentSection ? currentSection->sectionContainer() : 0)
    , m_origin(0, 0)
    , m_view(view)
    , m_doc(doc)
{
    m_shapeManager = new KoShapeManager(this);
    connect(m_shapeManager, SIGNAL(selectionChanged()), SLOT(updateOriginAndSize()));
    m_toolProxy = new KoToolProxy(this);
    setFocusPolicy(Qt::StrongFocus);
    // this is much faster than painting it in the paintevent
    QPalette pal = palette();
    pal.setColor(QPalette::Base, Qt::white);
    pal.setColor(QPalette::Text, Qt::black);
    setPalette(pal);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    setAttribute(Qt::WA_InputMethodEnabled, true);

    if(currentSection) {

        QList<KoShape*> shapes;
        shapes.push_back(currentSection->sectionContainer()->layer());
        shapeManager()->setShapes(shapes, KoShapeManager::AddWithoutRepaint);

        KoShapeLayer* layer = currentSection->sectionContainer()->layer();
        shapeManager()->selection()->setActiveLayer(layer);

        // Make sure the canvas is enabled
        setEnabled(true);

        update();
    } else {
        setEnabled(false);
    }
}

Canvas::~Canvas()
{
    delete m_toolProxy;
    delete m_shapeManager;
}

void Canvas::setDocumentOffset(const QPoint &offset)
{
    m_originalOffset = offset;
    updateOffset();
}

void Canvas::addCommand(KUndo2Command *command)
{
    m_doc->addCommand(m_view->activeSection(), command);
    updateOriginAndSize();
}

KoShapeManager * Canvas::shapeManager() const
{
    return m_shapeManager;
}

void Canvas::updateCanvas(const QRectF& rc)
{
    QRect clipRect(viewToWidget(viewConverter()->documentToView(rc).toRect()));
    clipRect.adjust(-2, -2, 2, 2);   // Resize to fit anti-aliasing
    clipRect.moveTopLeft(clipRect.topLeft() - m_documentOffset);
    update(clipRect);

    emit canvasUpdated();
}

KoViewConverter * Canvas::viewConverter() const
{
    return m_view->zoomHandler();
}

KoUnit Canvas::unit() const
{
    return KoUnit(KoUnit::Centimeter);
}

const QPoint & Canvas::documentOffset() const
{
    return m_documentOffset;
}

QPoint Canvas::documentOrigin() const
{
    return m_origin;
}

void Canvas::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.translate(-documentOffset());
    painter.setRenderHint(QPainter::Antialiasing);
    QRectF clipRect = event->rect().translated(documentOffset());
    painter.setClipRect(clipRect);

    painter.translate(m_origin.x(), m_origin.y());

    const KoViewConverter* converter = viewConverter();
    shapeManager()->paint(painter, *converter, false);
    painter.setRenderHint(QPainter::Antialiasing, false);

    painter.setRenderHint(QPainter::Antialiasing);
    m_toolProxy->paint(painter, *converter);
}

void Canvas::tabletEvent(QTabletEvent *event)
{
    m_toolProxy->tabletEvent(event, viewConverter()->viewToDocument(widgetToView(event->pos() + m_documentOffset)));
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
    m_toolProxy->mousePressEvent(event, viewConverter()->viewToDocument(widgetToView(event->pos() + m_documentOffset)));

    if(!event->isAccepted() && event->button() == Qt::RightButton) {
        showContextMenu(event->globalPos(), toolProxy()->popupActionList());
    }

    event->setAccepted(true);
}

void Canvas::mouseDoubleClickEvent(QMouseEvent *event)
{
    m_toolProxy->mouseDoubleClickEvent(event, viewConverter()->viewToDocument(widgetToView(event->pos() + m_documentOffset)));
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
    m_toolProxy->mouseMoveEvent(event, viewConverter()->viewToDocument(widgetToView(event->pos() + m_documentOffset)));
}

void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
    m_toolProxy->mouseReleaseEvent(event, viewConverter()->viewToDocument(widgetToView(event->pos() + m_documentOffset)));
}

void Canvas::keyPressEvent(QKeyEvent *event)
{
    m_toolProxy->keyPressEvent(event);
#if 0

    if(! event->isAccepted()) {
        event->accept();
        switch(event->key()) {
        case Qt::Key_Home:
            m_view->navigatePage(KoPageApp::PageFirst);
            break;
        case Qt::Key_PageUp:
            m_view->navigatePage(KoPageApp::PagePrevious);
            break;
        case Qt::Key_PageDown:
            m_view->navigatePage(KoPageApp::PageNext);
            break;
        case Qt::Key_End:
            m_view->navigatePage(KoPageApp::PageLast);
            break;
        default:
            event->ignore();
            break;
        }
    }
    if(! event->isAccepted()) {
        if(event->key() == Qt::Key_Backtab
                or(event->key() == Qt::Key_Tab && (event->modifiers() & Qt::ShiftModifier))) {
            focusNextPrevChild(false);
        } else if(event->key() == Qt::Key_Tab) {
            focusNextPrevChild(true);
        }
    }
#endif
}

void Canvas::keyReleaseEvent(QKeyEvent *event)
{
    m_toolProxy->keyReleaseEvent(event);
}

void Canvas::wheelEvent(QWheelEvent * event)
{
    m_toolProxy->wheelEvent(event, viewConverter()->viewToDocument(widgetToView(event->pos() + m_documentOffset)));
}

void Canvas::closeEvent(QCloseEvent * event)
{
    event->ignore();
}

void Canvas::updateInputMethodInfo()
{
    updateMicroFocus();
}

QVariant Canvas::inputMethodQuery(Qt::InputMethodQuery query) const
{
    if (query == Qt::ImMicroFocus) {
        QRectF rect = (m_toolProxy->inputMethodQuery(query, *(viewConverter())).toRectF()).toRect();
        QPointF scroll(canvasController()->scrollBarValue());
        rect.translate(documentOrigin() - scroll);
        return rect.toRect();
    }
    return m_toolProxy->inputMethodQuery(query, *(viewConverter()));
}

void Canvas::inputMethodEvent(QInputMethodEvent *event)
{
    m_toolProxy->inputMethodEvent(event);
}

void Canvas::resizeEvent(QResizeEvent * event)
{
    emit sizeChanged(event->size());
    updateOriginAndSize();
}

void Canvas::showContextMenu(const QPoint& globalPos, const QList<QAction*>& actionList)
{
    m_view->unplugActionList("toolproxy_action_list");
    m_view->plugActionList("toolproxy_action_list", actionList);
    QMenu *menu = dynamic_cast<QMenu*>(m_view->factory()->container("default_canvas_popup", m_view));

    if(menu)
        menu->exec(globalPos);
}

void Canvas::setBackgroundColor(const QColor &color)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Normal, backgroundRole(), color);
    pal.setColor(QPalette::Inactive, backgroundRole(), color);
    setPalette(pal);
}

void Canvas::updateOriginAndSize()
{
    if(m_view->activeSection()) {
        QRectF rect = m_view->activeSection()->layout()->boundingBox();
        if(rect != m_oldDocumentRect) {
            m_oldDocumentRect = rect;
            emit(documentRect(rect));
            update();
        }
        QRect viewRect = viewConverter()->documentToView(rect).toRect();
        if(m_oldViewDocumentRect != viewRect) {
            m_oldViewDocumentRect = viewRect;
            m_origin = -viewRect.topLeft();
            KoCanvasController* controller = canvasController();
            if(controller) {
                // tell canvas controller the new document size in pixel
                controller->updateDocumentSize(viewRect.size(), true);
                // make sure the actual selection is visible
                KoSelection * selection = m_shapeManager->selection();
                if(selection->count())
                    controller->ensureVisible(viewConverter()->documentToView(selection->boundingRect()));
                updateOffset();
            }
        }
    }
}

void Canvas::updateOffset()
{
    qreal dx = qMax(0, (size().width() - m_oldViewDocumentRect.size().width()) / 2);
    qreal dy = qMax(0, (size().height() - m_oldViewDocumentRect.size().height()) / 2);
    m_documentOffset = m_originalOffset - QPoint(dx, dy);
}

void Canvas::gridSize(qreal *horizontal, qreal *vertical) const
{
    *horizontal = 1;
    *vertical = 1;
}

bool Canvas::snapToGrid() const
{
    return false;
}

void Canvas::setCursor(const QCursor &cursor)
{
    QWidget::setCursor(cursor);
}

void Canvas::focusInEvent(QFocusEvent * event)
{
    QWidget::focusInEvent(event);
    emit(canvasReceivedFocus());
}

QPoint Canvas::widgetToView(const QPoint& p) const
{
    return p - m_origin;
}

QRect Canvas::widgetToView(const QRect& r) const
{
    return r.translated(- m_origin);
}

QPoint Canvas::viewToWidget(const QPoint& p) const
{
    return p + m_origin;
}

QRect Canvas::viewToWidget(const QRect& r) const
{
    return r.translated(m_origin);
}
