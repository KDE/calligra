/* This file is part of the KDE project
 * Copyright 2020 Dag Andersen <dag.andersen@kdemail.net>
 * Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 *  
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

// Local
#include "TableTool.h"
#include "SheetsEditor.h"

#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QSpinBox>
#include <QToolBar>
#include <QPushButton>
#include <QUrl>
#include <QMimeDatabase>
#include <QFileDialog>
#include <QScrollBar>
#include <QMarginsF>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFontMetrics>
#include <QMenu>

#include <KLocalizedString>
#include <KPageDialog>

#include <KoCanvasBase.h>
#include <KoPointerEvent.h>
#include <KoSelection.h>
#include <KoIcon.h>
#include <KoViewConverter.h>
#include <KoShapeContainer.h>
#include <KoShapeContainerDefaultModel.h>
#include <KoCanvasController.h>
#include <KoCanvasControllerWidget.h>
#include <KoDocumentResourceManager.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeRegistry.h>
#include <KoTextShapeData.h>
#include <KoShapePaintingContext.h>
#include <KoBorder.h>
#include <kundo2command.h>
#include <KoShapeManager.h>

#include "SheetsDebug.h"
#include "AutoFillStrategy.h"
#include "Cell.h"
#include "calligra_sheets_limits.h"
#include "Map.h"
#include "MergeStrategy.h"
#include "Selection.h"
#include "SelectionStrategy.h"
#include "Sheet.h"
#include "DragAndDropStrategy.h"
#include "PasteStrategy.h"
#include "HyperlinkStrategy.h"
#include "CellView.h"
#include "Database.h"
#include "SheetView.h"
#include "FilterPopup.h"
#include "RowColumnFormat.h"
#include "RowFormatStorage.h"
#include "RecalcManager.h"

#include "TableShape.h"
#include "ToolHeaders.h"

#include <algorithm>

#define TextShapeId "TextShapeID"

namespace Calligra {
namespace Sheets {

class TableToolResizeCommand : public KUndo2Command
{
public:
    TableToolResizeCommand(TableTool *tool, const QPointF &pos = QPointF(), const QSizeF &size = QSizeF(), KUndo2Command *parent = nullptr)
        : KUndo2Command(parent)
    {
        setText(kundo2_i18n("Resize spreadsheet"));
        m_tool = tool;
        m_shape = tool->shape();
        m_canvas = tool->canvas();
        m_oldpos = m_shape->absolutePosition(KoFlake::TopLeftCorner);
        m_oldsize = m_shape->size();
        m_newpos = pos;
        m_newsize = size;
    }
    bool isValid() const {
        bool r = m_newpos != QPointF() && m_newsize != QSizeF();
        r &= (m_newpos != m_oldpos || m_newsize != m_oldsize);
        return r;
    }
    void setPos(const QPointF &pos) {
        m_newpos = pos;
    }
    void setSize(const QSizeF &size) {
        m_newsize = size;
    }
    void redo() override {
        m_shape->setAbsolutePosition(m_newpos, KoFlake::TopLeftCorner);
        m_shape->resize(m_newsize);
        m_canvas->updateCanvas(QRectF(m_newpos, m_newsize) | QRectF(m_oldpos, m_oldpos));
        m_tool->slotShapeChanged(m_shape);
    }
    void undo() override {
        m_shape->setAbsolutePosition(m_oldpos, KoFlake::TopLeftCorner);
        m_shape->resize(m_oldsize);
        m_canvas->updateCanvas(QRectF(m_newpos, m_newsize) | QRectF(m_oldpos, m_oldpos));
        m_tool->slotShapeChanged(m_shape);
    }

private:
    TableTool *m_tool;
    TableShape *m_shape;
    KoCanvasBase *m_canvas;
    QPointF m_oldpos;
    QSizeF m_oldsize;
    QPointF m_newpos;
    QSizeF m_newsize;
};
}
}

using namespace Calligra::Sheets;

ScrollBar::ScrollBar(Qt::Orientation o, QWidget *parent)
    : QScrollBar(o, parent)
{
    setMaximum(100);
    setPageStep(5);
    setSingleStep(1);
}
bool ScrollBar::event(QEvent *event)
{
    if (event->type() == QEvent::ShortcutOverride) {
        event->accept();
    }
    return QScrollBar::event(event);
}

class TableTool::Private
{
public:
    Private() : resizeCommand(nullptr) {}
    TableTool *q;
    Selection* selection;
    TableShape* tableShape;
    QWidget *optionWidget;

    void setupTools();
    void paintTools(QPainter &painter, const KoViewConverter &viewConverter);
    void resize(const KoPointerEvent *event);
    QRect visibleCells() const;

    QMarginsF margins;
    QRectF toolRect;
    QRectF shapeRect;
    QRectF columnRect;
    QRectF rowRect;
    QRectF verticalScrollRect;
    QRectF horizontalScrollRect;
    QRectF sheetNameRect;
    QMarginsF resizeMargins;
    QRectF resizeRect;
    QRectF resizeAreaLeftRect;
    QRectF resizeAreaLeftRectScaled;
    QRectF resizeAreaRightRect;
    QRectF resizeAreaRightRectScaled;
    QRectF resizeAreaTopRect;
    QRectF resizeAreaTopRectScaled;
    QRectF resizeAreaBottomRect;
    QRectF resizeAreaBottomRectScaled;

    QRectF resizeAreaTopLeftRect;
    QRectF resizeAreaTopLeftRectScaled;
    QRectF resizeAreaTopRightRect;
    QRectF resizeAreaTopRightRectScaled;
    QRectF resizeAreaBottomLeftRect;
    QRectF resizeAreaBottomLeftRectScaled;
    QRectF resizeAreaBottomRightRect;
    QRectF resizeAreaBottomRightRectScaled;

    ToolType toolPressed;
    ToolType focus;

    Tool::ColumnHeader columnHeader;
    Tool::RowHeader rowHeader;
    ScrollBar *horizontalScrollBar;
    ScrollBar *verticalScrollBar;

    KoCanvasControllerWidget *controller;

    QMap<Sheet*, QPoint> savedAnchors;
    QMap<Sheet*, QPoint> savedMarkers;
    QMap<Sheet*, QPoint> savedOffsets;

    QAction separator;

    QList<QAction*> sheetActions;

    QPointF pressedPoint;

    TableToolResizeCommand *resizeCommand;
};


QRect TableTool::Private::visibleCells() const
{
    Sheet *sheet = tableShape->sheet();
    QRectF rect(tableShape->topLeftOffset(), shapeRect.size());
    QRect cells;
    qreal leftx;
    cells.setLeft(sheet->leftColumn(rect.left(), leftx));
    qreal rightx;
    cells.setRight(sheet->leftColumn(rect.right(), rightx));
    qreal topy;
    cells.setTop(sheet->topRow(rect.top(), topy));
    qreal bottomy;
    cells.setBottom(sheet->topRow(rect.bottom(), bottomy));
#if 0
    qreal leftw = sheet->nonDefaultColumnFormat(cells.left())->width();
    qreal rightw = sheet->nonDefaultColumnFormat(cells.right())->width();
    qreal toph = sheet->rowFormats()->rowHeight(cells.top());
    qreal bottomh = sheet->rowFormats()->rowHeight(cells.bottom());
#endif
    return cells;
}

void TableTool::Private::resize(const KoPointerEvent *event)
{
    QRectF oldrect = resizeRect;
    QPointF dp = event->point - pressedPoint;

    QPointF pos = shapeRect.topLeft();
    QSizeF size = shapeRect.size();
    switch (toolPressed) {
        case ResizeAreaLeft:
            pos.rx() += dp.rx();
            size.setWidth(size.width() - dp.x());
            break;
        case ResizeAreaRight:
            size.setWidth(size.width() + dp.x());
            break;
        case ResizeAreaTop:
            pos.ry() += dp.ry();
            size.setHeight(size.height() - dp.ry());
            break;
        case ResizeAreaBottom:
            size.setHeight(size.height() + dp.ry());
            break;
        case ResizeAreaTopLeft:
            pos.rx() += dp.rx();
            size.setWidth(size.width() - dp.x());
            pos.ry() += dp.ry();
            size.setHeight(size.height() - dp.ry());
            break;
        case ResizeAreaTopRight:
            size.setWidth(size.width() + dp.x());
            pos.ry() += dp.ry();
            size.setHeight(size.height() - dp.ry());
            break;
        case ResizeAreaBottomLeft:
            pos.rx() += dp.rx();
            size.setWidth(size.width() - dp.x());
            size.setHeight(size.height() + dp.ry());
            break;
        case ResizeAreaBottomRight:
            size.setWidth(size.width() + dp.x());
            size.setHeight(size.height() + dp.ry());
            break;
        default:
            return;
    }
    if (!resizeCommand) {
        resizeCommand = new TableToolResizeCommand(q);
    }
    resizeCommand->setPos(pos);
    resizeCommand->setSize(size);
    resizeCommand->redo();

    pressedPoint = event->point;
}

void TableTool::Private::setupTools()
{
    margins.setLeft(22.);
    margins.setRight(12.);
    margins.setTop(14.);
    margins.setBottom(12.);

    shapeRect = tableShape->boundingRect();
    toolRect = shapeRect + margins;

    const Sheet *sheet = tableShape->sheet();
    const QRect used = sheet->usedArea();
    int maxColumn = used.width();
    if (q->selection()->marker().x() > maxColumn) {
        maxColumn = q->selection()->marker().x();
    }
    sheetNameRect = QRectF(toolRect.left(), shapeRect.bottom(), 40., margins.bottom());

    horizontalScrollRect = QRectF(sheetNameRect.right(), shapeRect.bottom(), toolRect.width()-sheetNameRect.width()-margins.right(), margins.bottom());
    horizontalScrollBar->setFocusPolicy(Qt::StrongFocus);
    horizontalScrollBar->setInvertedControls(false);
    horizontalScrollBar->setMaximum(std::max(tableShape->size().width()*10, sheet->columnPosition(maxColumn)));
    horizontalScrollBar->setPageStep(tableShape->size().width());
    horizontalScrollBar->setSingleStep(1);
    horizontalScrollBar->setValue(tableShape->topLeftOffset().x());

    int maxRow = used.height();
    if (q->selection()->marker().y() > maxRow) {
        maxRow = q->selection()->marker().y();
    }
    verticalScrollRect = QRectF(shapeRect.right(), shapeRect.top(), margins.right(), shapeRect.height());
    verticalScrollBar->setFocusPolicy(Qt::StrongFocus);
    verticalScrollBar->setMaximum(std::max(tableShape->size().height()*10, sheet->rowPosition(maxRow)));
    verticalScrollBar->setPageStep(tableShape->size().height());
    verticalScrollBar->setSingleStep(1);
    verticalScrollBar->setValue(tableShape->topLeftOffset().y());

    columnRect = QRectF(shapeRect.left(), toolRect.top(), shapeRect.width(), margins.top());
    columnHeader.setGeometry(columnRect);
    columnHeader.setCanvas(controller->canvas());
    columnHeader.setScrollBars(horizontalScrollBar, verticalScrollBar);
    columnHeader.setSheetHeight(shapeRect.height());

    rowRect = QRectF(toolRect.left(), shapeRect.top(), margins.left(), shapeRect.height());
    rowHeader.setGeometry(rowRect);
    rowHeader.setCanvas(controller->canvas());
    rowHeader.setScrollBars(horizontalScrollBar, verticalScrollBar);
    rowHeader.setSheetWidth(shapeRect.width());

    resizeMargins = QMarginsF(5, 5, 5, 5);
    resizeRect = toolRect + resizeMargins;
    const QPointF center = resizeRect.center();
    resizeAreaLeftRect = QRectF(resizeRect.left(), center.y()-resizeMargins.left()/2., resizeMargins.left(), resizeMargins.left());
    resizeAreaRightRect = QRectF(resizeRect.right(), center.y()-resizeMargins.left()/2., resizeMargins.right(), resizeMargins.right()).translated(-resizeMargins.right(), 0);
    resizeAreaTopRect = QRectF(center.x()-resizeMargins.top()/2., resizeRect.top(), resizeMargins.top(), resizeMargins.top());
    resizeAreaBottomRect = QRectF(center.x()-resizeMargins.top()/2., resizeRect.bottom(), resizeMargins.bottom(), resizeMargins.bottom()).translated(0, -resizeMargins.bottom());

    resizeAreaTopLeftRect = QRectF(resizeRect.left(), resizeRect.top(), resizeMargins.left(), resizeMargins.top());
    resizeAreaTopRightRect = QRectF(resizeRect.right(), resizeRect.top(), resizeMargins.right(), resizeMargins.top()).translated(-resizeMargins.right(), 0);
    resizeAreaBottomLeftRect = QRectF(resizeRect.left(), resizeRect.bottom(), resizeMargins.left(), resizeMargins.bottom()).translated(0, -resizeMargins.bottom());
    resizeAreaBottomRightRect = QRectF(resizeRect.right(), resizeRect.bottom(), resizeMargins.right(), resizeMargins.bottom()).translated(-resizeMargins.bottom(), -resizeMargins.right());

}

void TableTool::Private::paintTools(QPainter &painter, const KoViewConverter &viewConverter)
{
    const QPointF position = tableShape->absolutePosition(KoFlake::TopLeftCorner);
    painter.translate(shapeRect.topLeft() - position);

    QRectF tr = resizeRect.translated(-position);
    painter.fillRect(tr, Qt::white);

    tr = toolRect.translated(-position);
    painter.fillRect(tr, qApp->palette().window());

    painter.save();
    QRectF clipRect(QPointF(), tableShape->size());
    painter.setClipRect(clipRect);
    tableShape->paintCells(painter);
    painter.restore();

    Sheet *sheet = tableShape->sheet();
    QRectF paintRect;

    painter.save();
    paintRect = sheetNameRect.translated(-position);
    QString name = sheet->sheetName();
    QFontMetrics fm(painter.font());
    qreal sx = paintRect.width() / fm.width(name);
    qreal sy = paintRect.height() / fm.height();
    painter.translate(paintRect.center());
    auto s = std::min(sx, sy);
    painter.scale(s, s);
    paintRect.setWidth(paintRect.width() / s);
    paintRect.setHeight(paintRect.height() / s);
    painter.translate(-paintRect.center());
    painter.fillRect(paintRect, qApp->palette().window().color().lighter());
    painter.setPen(Qt::black);
    painter.drawText(paintRect, name, Qt::AlignLeft | Qt::AlignVCenter);
    painter.restore();

    painter.save();
    columnHeader.setSheet(sheet);
    paintRect = QRectF(QPointF(tableShape->topLeftOffset().x(), 0.0), columnRect.size());
    painter.translate(-paintRect.left(), -paintRect.height());
    painter.setClipRect(paintRect);
    columnHeader.paint(&painter, paintRect);
    painter.restore();

    painter.save();
    rowHeader.setSheet(sheet);
    paintRect = QRectF(QPointF(0.0, tableShape->topLeftOffset().y()), rowRect.size());
    painter.translate(-paintRect.width(), -paintRect.top());
    painter.setClipRect(paintRect);
    rowHeader.paint(&painter, paintRect);
    painter.restore();

    const QPointF documentOrigin = q->canvas()->documentOrigin();
    const QPoint canvasScroll(controller->canvasOffsetX(), controller->canvasOffsetY());

    QRect rect = viewConverter.documentToView(horizontalScrollRect).translated(documentOrigin).toRect();
    rect.translate(canvasScroll);
    horizontalScrollBar->setGeometry(rect);

    rect = viewConverter.documentToView(verticalScrollRect).translated(documentOrigin).toRect();
    rect.translate(canvasScroll);
    verticalScrollBar->setGeometry(rect);

    painter.save();
    painter.setClipRect(resizeRect.translated(-position));

    QColor color("#87CEFA");
    QPen pen(color, 0);
    QBrush brush(color);

    resizeAreaLeftRectScaled = resizeAreaLeftRect;
    resizeAreaRightRectScaled = resizeAreaRightRect;
    resizeAreaTopRectScaled = resizeAreaTopRect;
    resizeAreaBottomRectScaled = resizeAreaBottomRect;
    resizeAreaTopLeftRectScaled = resizeAreaTopLeftRect;
    resizeAreaTopRightRectScaled = resizeAreaTopRightRect;
    resizeAreaBottomLeftRectScaled = resizeAreaBottomLeftRect;
    resizeAreaBottomRightRectScaled = resizeAreaBottomRightRect;
    const qreal scale = 1.0 / viewConverter.zoom();
    if (scale != 1.0) {
        QSizeF s1 = resizeAreaLeftRect.size() * scale;
        QSizeF s2 = s1 - resizeAreaLeftRectScaled.size();
        resizeAreaLeftRectScaled.setSize(s1);
        resizeAreaLeftRectScaled.translate(-s2.width(), -s2.height() / 2.);

        s1 = resizeAreaRightRect.size() * scale;
        s2 = s1 - resizeAreaRightRectScaled.size();
        resizeAreaRightRectScaled.setSize(s1);
        resizeAreaRightRectScaled.translate(0, -s2.height() / 2.);

        s1 = resizeAreaTopRect.size() * scale;
        s2 = s1 - resizeAreaTopRectScaled.size();
        resizeAreaTopRectScaled.setSize(s1);
        resizeAreaTopRectScaled.translate(-s2.width() / 2., -s2.height());

        s1 = resizeAreaBottomRect.size() * scale;
        s2 = s1 - resizeAreaBottomRectScaled.size();
        resizeAreaBottomRectScaled.setSize(s1);
        resizeAreaBottomRectScaled.translate(-s2.width() / 2., 0);

        s1 = resizeAreaTopLeftRect.size() * scale;
        s2 = s1 - resizeAreaTopLeftRectScaled.size();
        resizeAreaTopLeftRectScaled.setSize(s1);
        resizeAreaTopLeftRectScaled.translate(-s2.width(), -s2.height());

        s1 = resizeAreaTopRightRect.size() * scale;
        s2 = s1 - resizeAreaTopRightRectScaled.size();
        resizeAreaTopRightRectScaled.setSize(s1);
        resizeAreaTopRightRectScaled.translate(0, -s2.height());

        s1 = resizeAreaBottomLeftRect.size() * scale;
        s2 = s1 - resizeAreaBottomLeftRectScaled.size();
        resizeAreaBottomLeftRectScaled.setSize(s1);
        resizeAreaBottomLeftRectScaled.translate(-s2.width(), 0);

        s1 = resizeAreaBottomRightRect.size() * scale;
        s2 = s1 - resizeAreaBottomRightRectScaled.size();
        resizeAreaBottomRightRectScaled.setSize(s1);
    }
    paintRect = resizeAreaLeftRectScaled.translated(-position);
    painter.fillRect(paintRect, brush);
    paintRect = resizeAreaRightRectScaled.translated(-position);
    painter.fillRect(paintRect, brush);
    paintRect = resizeAreaTopRectScaled.translated(-position);
    painter.fillRect(paintRect, brush);
    paintRect = resizeAreaBottomRectScaled.translated(-position);
    painter.fillRect(paintRect, brush);

    paintRect = resizeAreaTopLeftRectScaled.translated(-position);
    painter.fillRect(paintRect, brush);
    paintRect = resizeAreaTopRightRectScaled.translated(-position);
    painter.fillRect(paintRect, brush);
    paintRect = resizeAreaBottomLeftRectScaled.translated(-position);
    painter.fillRect(paintRect, brush);
    paintRect = resizeAreaBottomRightRectScaled.translated(-position);
    painter.fillRect(paintRect, brush);

    QPainterPath path;
    path.addRect(QRectF(resizeAreaTopLeftRectScaled.translated(-position).center(), resizeAreaBottomRightRectScaled.translated(-position).center()));
    painter.setPen(pen);
    painter.drawPath(path);
    painter.restore();
}


TableTool::TableTool(KoCanvasBase* canvas)
        : CellToolBase(canvas)
        , d(new Private)
{
    setObjectName(QLatin1String("TableTool"));
    d->q = this;
    d->selection = new Selection(canvas);
    d->controller = nullptr;
    for (QWidget *w = canvas->canvasWidget(); w; w = w->parentWidget()) {
        d->controller = qobject_cast<KoCanvasControllerWidget*>(w);
        if (d->controller) {
            break;
        }
    }
    d->horizontalScrollBar = new ScrollBar(Qt::Horizontal, d->controller);
    d->horizontalScrollBar->hide();
    d->verticalScrollBar = new ScrollBar(Qt::Vertical, d->controller);
    d->verticalScrollBar->hide();

    d->columnHeader.setSelection(d->selection);
    d->rowHeader.setSelection(d->selection);
    d->tableShape = nullptr;
    d->optionWidget = nullptr;

    createActions();
}

TableTool::~TableTool()
{
    delete d->selection;
    delete d;
}

TableTool::ToolType TableTool::mouseOn(KoPointerEvent* event)
{
    if (d->tableShape->hitTest(event->point)) {
        return Table;
    }
    if (d->columnRect.contains(event->point)) {
        return ColumnHeader;
    }
    if (d->rowRect.contains(event->point)) {
        return RowHeader;
    }
    if (d->horizontalScrollRect.contains(event->point)) {
        return HScrollBar;
    }
    if (d->verticalScrollRect.contains(event->point)) {
        return VScrollBar;
    }
    if (d->sheetNameRect.contains(event->point)) {
        return SheetName;
    }
    if (d->resizeAreaLeftRectScaled.adjusted(-8, -8, 8, 16).contains(event->point)) {
        return ResizeAreaLeft;
    }
    if (d->resizeAreaRightRectScaled.adjusted(0, -8, 8, 16).contains(event->point)) {
        return ResizeAreaRight;
    }
    if (d->resizeAreaTopRectScaled.adjusted(-8, -8, 16, 8).contains(event->point)) {
        return ResizeAreaTop;
    }
    if (d->resizeAreaBottomRectScaled.adjusted(-8, 0, 16, 8).contains(event->point)) {
        return ResizeAreaBottom;
    }
    if (d->resizeAreaTopLeftRectScaled.adjusted(-8, -8, 8, 8).contains(event->point)) {
        return ResizeAreaTopLeft;
    }
    if (d->resizeAreaTopRightRectScaled.adjusted(-8, -8, 8, 8).contains(event->point)) {
        return ResizeAreaTopRight;
    }
    if (d->resizeAreaBottomLeftRectScaled.adjusted(-8, -8, 8, 16).contains(event->point)) {
        return ResizeAreaBottomLeft;
    }
    if (d->resizeAreaBottomRightRectScaled.adjusted(0, -8, 16, 8).contains(event->point)) {
        return ResizeAreaBottomRight;
    }
    return None;
}

void TableTool::mousePressEvent(KoPointerEvent* event)
{
    d->horizontalScrollBar->clearFocus();
    d->verticalScrollBar->clearFocus();
    d->toolPressed = mouseOn(event);
    d->pressedPoint = event->point;
    switch (d->toolPressed) {
        case Table: {
            d->focus = Table;
            m_lastMousePoint = event->point;
            CellToolBase::mousePressEvent(event);
            break;
        }
        case ColumnHeader: {
            d->focus = ColumnHeader;
            selection()->emitCloseEditor(true);
            KoPointerEvent e(event, event->point - d->columnRect.topLeft() + d->tableShape->topLeftOffset());
            d->columnHeader.mousePress(&e);
            if (event->button() == Qt::RightButton) {
                setPopupActionList(popupMenuActionList());
                event->ignore();
            }
            break;
        }
        case RowHeader: {
            d->focus = RowHeader;
            selection()->emitCloseEditor(true);
            KoPointerEvent e(event, event->point - d->rowRect.topLeft() + d->tableShape->topLeftOffset());
            d->rowHeader.mousePress(&e);
            if (event->button() == Qt::RightButton) {
                setPopupActionList(popupMenuActionList());
                event->ignore();
            }
            break;
        }
        case HScrollBar: {
            d->focus = HScrollBar;
            selection()->emitCloseEditor(true);
            d->horizontalScrollBar->setFocus();
            const KoViewConverter *viewConverter = canvas()->viewConverter();
            QPoint pos = viewConverter->documentToView(event->point - d->horizontalScrollRect.topLeft()).toPoint();
            QMouseEvent e(QEvent::MouseButtonPress, pos, event->globalPos(), event->globalPos(), event->button(), event->buttons(), event->modifiers());
            qApp->sendEvent(d->horizontalScrollBar, &e);
            canvas()->updateCanvas(d->toolRect);
            break;
        }
        case VScrollBar: {
            d->focus = VScrollBar;
            selection()->emitCloseEditor(true);
            d->verticalScrollBar->setFocus();
            const KoViewConverter *viewConverter = canvas()->viewConverter();
            QPoint pos = viewConverter->documentToView(event->point - d->verticalScrollRect.topLeft()).toPoint();
            QMouseEvent e(QEvent::MouseButtonPress, pos, event->globalPos(), event->globalPos(), event->button(), event->buttons(), event->modifiers());
            qApp->sendEvent(d->verticalScrollBar, &e);
            canvas()->updateCanvas(d->toolRect);
            break;
        }
        case SheetName: {
            d->focus = SheetName;
            selection()->emitCloseEditor(true);
            if (event->button() == Qt::LeftButton) {
                sheetSelectionCombo();
                event->accept();
            } else {
                setPopupActionList(QList<QAction*>()<<action("sheets-editor"));
                event->ignore();
            }
            break;
        }
        case ResizeAreaLeft:
        case ResizeAreaRight:
        case ResizeAreaTop:
        case ResizeAreaBottom:
        case ResizeAreaTopLeft:
        case ResizeAreaTopRight:
        case ResizeAreaBottomLeft:
        case ResizeAreaBottomRight: {
            d->focus = d->toolPressed;
            break;
        }
        default: {
            d->focus = None;
            event->ignore();
            break;
        }
    }
}

void TableTool::mouseReleaseEvent(KoPointerEvent* event)
{
    if (d->resizeCommand) {
        if (d->resizeCommand->isValid()) {
            canvas()->addCommand(d->resizeCommand);
        } else {
            delete d->resizeCommand;
        }
        d->resizeCommand = nullptr;
    }
    switch (d->toolPressed) {
        case Table: {
            d->toolPressed = None;
            CellToolBase::mouseReleaseEvent(event);
            break;
        }
        case ColumnHeader: {
            d->toolPressed = None;
            KoPointerEvent e(event, event->point - d->columnRect.topLeft() + d->tableShape->topLeftOffset());
            d->columnHeader.mouseRelease(&e);
            canvas()->updateCanvas(d->toolRect);
            break;
        }
        case RowHeader: {
            d->toolPressed = None;
            KoPointerEvent e(event, event->point - d->rowRect.topLeft() + d->tableShape->topLeftOffset());
            d->rowHeader.mouseRelease(&e);
            canvas()->updateCanvas(d->toolRect);
            break;
        }
        case HScrollBar: {
            d->toolPressed = None;
            const KoViewConverter *viewConverter = canvas()->viewConverter();
            QPoint pos = viewConverter->documentToView(event->point - d->horizontalScrollRect.topLeft()).toPoint();
            QMouseEvent e(QEvent::MouseButtonRelease, pos, event->globalPos(), event->globalPos(), event->button(), event->buttons(), event->modifiers());
            qApp->sendEvent(d->horizontalScrollBar, &e);
            canvas()->updateCanvas(d->toolRect);
            break;
        }
        case VScrollBar: {
            d->toolPressed = None;
            const KoViewConverter *viewConverter = canvas()->viewConverter();
            QPoint pos = viewConverter->documentToView(event->point - d->verticalScrollRect.topLeft()).toPoint();
            QMouseEvent e(QEvent::MouseButtonRelease, pos, event->globalPos(), event->globalPos(), event->button(), event->buttons(), event->modifiers());
            qApp->sendEvent(d->verticalScrollBar, &e);
            canvas()->updateCanvas(d->toolRect);
            break;
        }
        case SheetName: {
            d->toolPressed = None;
            break;
        }
        case ResizeAreaLeft:
        case ResizeAreaRight:
        case ResizeAreaTop:
        case ResizeAreaBottom:
        case ResizeAreaTopLeft:
        case ResizeAreaTopRight:
        case ResizeAreaBottomLeft:
        case ResizeAreaBottomRight: {
            d->toolPressed = None;
            break;
        }
        default: {
            d->toolPressed = None;
            if (!d->toolRect.contains(event->point)) {
                event->ignore();
            }
            break;
        }
    }
}

bool TableTool::autoScroll(ToolType type, KoPointerEvent* event)
{
    //TODO: improve autoscrolling
    QRectF rect = d->tableShape->boundingRect();
    if (type == Table || type == ColumnHeader) {
        if (event->point.x() < rect.left()) {
            if (event->point.x() >= m_lastMousePoint.x()) {
                // no scroll
                m_lastMousePoint.setX(event->point.x());
                event->accept();
                return false;
            }
        } else if (event->point.x() > rect.right()) {
            if (event->point.x() <= m_lastMousePoint.x()) {
                // no scroll
                m_lastMousePoint.setX(event->point.x());
                event->accept();
                return false;
            }
        }
        m_lastMousePoint.setX(event->point.x());
    }
    if (type == Table || type == RowHeader) {
        if (event->point.y() < rect.top()) {
            if (event->point.y() >= m_lastMousePoint.y()) {
                // no scroll
                m_lastMousePoint.setY(event->point.y());
                event->accept();
                return false;
            }
        } else if (event->point.y() > rect.bottom()) {
            if (event->point.y() <= m_lastMousePoint.y()) {
                // no scroll
                m_lastMousePoint.setY(event->point.y());
                event->accept();
                return false;
            }
        }
        m_lastMousePoint.setY(event->point.y());
    }
    return true;
}

void TableTool::mouseMoveEvent(KoPointerEvent* event)
{
    setStatusText(QString());
    ToolType type = d->toolPressed == None ? mouseOn(event) : d->toolPressed;
    switch (type) {
        case Table:
            if (event->buttons() & Qt::LeftButton) {
                if (!autoScroll(type, event)) {
                    break;
                }
            }
            CellToolBase::mouseMoveEvent(event);
            break;
        case ColumnHeader: {
            if (event->buttons() & Qt::LeftButton) {
                if (!autoScroll(type, event)) {
                    break;
                }
            }
            KoPointerEvent e(event, event->point - d->columnRect.topLeft() + d->tableShape->topLeftOffset());
            d->columnHeader.mouseMove(&e);
            break;
        }
        case RowHeader: {
            if (event->buttons() & Qt::LeftButton) {
                if (!autoScroll(type, event)) {
                    break;
                }
            }
            KoPointerEvent e(event, event->point - d->rowRect.topLeft() + d->tableShape->topLeftOffset());
            d->rowHeader.mouseMove(&e);
            break;
        }
        case HScrollBar: {
            const KoViewConverter *viewConverter = canvas()->viewConverter();
            QPoint pos = viewConverter->documentToView(event->point - d->horizontalScrollRect.topLeft()).toPoint();
            QMouseEvent e(QEvent::MouseMove, pos, event->globalPos(), event->globalPos(), event->button(), event->buttons(), event->modifiers());
            qApp->sendEvent(d->horizontalScrollBar, &e);
            canvas()->updateCanvas(d->toolRect);
            break;
        }
        case VScrollBar: {
            const KoViewConverter *viewConverter = canvas()->viewConverter();
            QPoint pos = viewConverter->documentToView(event->point - d->verticalScrollRect.topLeft()).toPoint();
            QMouseEvent e(QEvent::MouseMove, pos, event->globalPos(), event->globalPos(), event->button(), event->buttons(), event->modifiers());
            qApp->sendEvent(d->verticalScrollBar, &e);
            canvas()->updateCanvas(d->toolRect);
            break;
        }
        case SheetName: {
            break;
        }
        case ResizeAreaLeft:
            canvas()->canvasWidget()->setCursor(Qt::SizeHorCursor);
            if (d->toolPressed == ResizeAreaLeft) {
                d->resize(event);
            }
            setStatusText(i18n("Resize to show more or fewer cells"));
            return;
        case ResizeAreaRight:
            canvas()->canvasWidget()->setCursor(Qt::SizeHorCursor);
            if (d->toolPressed == ResizeAreaRight) {
                d->resize(event);
            }
            setStatusText(i18n("Resize to show more or fewer cells"));
            return;
        case ResizeAreaTop:
            canvas()->canvasWidget()->setCursor(Qt::SizeVerCursor);
            if (d->toolPressed == ResizeAreaTop) {
                d->resize(event);
            }
            setStatusText(i18n("Resize to show more or fewer cells"));
            return;
        case ResizeAreaBottom: {
            canvas()->canvasWidget()->setCursor(Qt::SizeVerCursor);
            if (d->toolPressed == ResizeAreaBottom) {
                d->resize(event);
            }
            setStatusText(i18n("Resize to show more or fewer cells"));
            return;
        }
        case ResizeAreaTopLeft: {
            canvas()->canvasWidget()->setCursor(Qt::SizeFDiagCursor);
            if (d->toolPressed == ResizeAreaTopLeft) {
                d->resize(event);
            }
            setStatusText(i18n("Resize to show more or fewer cells"));
            return;
        }
        case ResizeAreaTopRight: {
            canvas()->canvasWidget()->setCursor(Qt::SizeBDiagCursor);
            if (d->toolPressed == ResizeAreaTopRight) {
                d->resize(event);
            }
            setStatusText(i18n("Resize to show more or fewer cells"));
            return;
        }
        case ResizeAreaBottomLeft: {
            canvas()->canvasWidget()->setCursor(Qt::SizeBDiagCursor);
            if (d->toolPressed == ResizeAreaBottomLeft) {
                d->resize(event);
            }
            return;
        }
        case ResizeAreaBottomRight: {
            canvas()->canvasWidget()->setCursor(Qt::SizeFDiagCursor);
            if (d->toolPressed == ResizeAreaBottomRight) {
                d->resize(event);
            }
            setStatusText(i18n("Resize to show more or fewer cells"));
            return;
        }
        default:
            if (!d->toolRect.contains(event->point)) {
                event->ignore();
            }
            break;
    }
    if (d->toolPressed == None && !(type == ColumnHeader || type == RowHeader)) {
        canvas()->canvasWidget()->setCursor(Qt::ArrowCursor);
    }
}

void TableTool::mouseDoubleClickEvent(KoPointerEvent* event)
{
    switch (mouseOn(event)) {
        case Table: {
            d->toolPressed = Table;
            d->focus = Table;
            CellToolBase::mouseDoubleClickEvent(event);
            break;
        }
        case ColumnHeader: {
            d->toolPressed = ColumnHeader;
            d->focus = ColumnHeader;
            selection()->emitCloseEditor(true);
            KoPointerEvent e(event, event->point - d->columnRect.topLeft());
            d->columnHeader.mouseDoubleClick(&e);
            break;
        }
        case RowHeader: {
            d->toolPressed = RowHeader;
            d->focus = RowHeader;
            selection()->emitCloseEditor(true);
            KoPointerEvent e(event, event->point - d->rowRect.topLeft());
            d->rowHeader.mouseDoubleClick(&e);
            break;
        }
        case HScrollBar: {
            d->toolPressed = HScrollBar;
            d->focus = HScrollBar;
            selection()->emitCloseEditor(true);
            d->horizontalScrollBar->setFocus();
            const KoViewConverter *viewConverter = canvas()->viewConverter();
            QPoint pos = viewConverter->documentToView(event->point - d->horizontalScrollRect.topLeft()).toPoint();
            QMouseEvent e(QEvent::MouseButtonDblClick, pos, event->globalPos(), event->globalPos(), event->button(), event->buttons(), event->modifiers());
            qApp->sendEvent(d->horizontalScrollBar, &e);
            canvas()->updateCanvas(d->toolRect);
            break;
        }
        case VScrollBar: {
            d->toolPressed = VScrollBar;
            d->focus = VScrollBar;
            selection()->emitCloseEditor(true);
            d->verticalScrollBar->setFocus();
            const KoViewConverter *viewConverter = canvas()->viewConverter();
            QPoint pos = viewConverter->documentToView(event->point - d->verticalScrollRect.topLeft()).toPoint();
            QMouseEvent e(QEvent::MouseButtonDblClick, pos, event->globalPos(), event->globalPos(), event->button(), event->buttons(), event->modifiers());
            qApp->sendEvent(d->verticalScrollBar, &e);
            canvas()->updateCanvas(d->toolRect);
            break;
        }
        case SheetName: {
            openSheetsEditor();
            break;
        }
        default: {
            d->toolPressed = None;
            d->focus = None;
            if (!d->toolRect.contains(event->point)) {
                event->ignore();
            }
            break;
        }
    }
}

void TableTool::keyPressEvent(QKeyEvent* event)
{
    switch(d->focus) {
        case Table:
            CellToolBase::keyPressEvent(event);
            event->accept();
            break;
        case ColumnHeader:
        case RowHeader:
            if (event->key() == Qt::Key_Menu) {
                setPopupActionList(popupMenuActionList());
            }
            event->accept();
            break;
        case HScrollBar:
        case VScrollBar:
            event->accept();
            break;
        case SheetName: {
            break;
        }
        default:
            break;
    }
}

KoInteractionStrategy* TableTool::createStrategy(KoPointerEvent* event)
{
    if (d->tableShape->hitTest(event->point)) {
        return CellToolBase::createStrategy(event);
    }
    return nullptr;
}

#if 0
void TableTool::exportDocument()
{
    const QString filterString =
        QMimeDatabase().mimeTypeForName("application/vnd.oasis.opendocument.spreadsheet").filterString();
    // TODO: i18n for title
    QString file = QFileDialog::getSaveFileName(0, "Export", QString(), filterString);
    if (file.isEmpty())
        return;
#if 0 // FIXME Stefan: Port!
    d->tableShape->doc()->exportDocument(file);
#endif
}
#endif

void TableTool::repaintDecorations()
{
    if (!d->tableShape) return;
    // TODO Stefan: restrict to the changed area
    canvas()->updateCanvas(d->tableShape->boundingRect());
}

Selection* TableTool::selection()
{
    return d->selection;
}

void TableTool::slotSelectionChanged(const Region&)
{
    scrollToCell(selection()->marker());
    canvas()->updateCanvas(d->toolRect);
}

void TableTool::activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes)
{
    foreach(KoShape* shape, shapes) {
        d->tableShape = dynamic_cast<TableShape*>(shape);
        if (d->tableShape)
            break;
    }
    if (!d->tableShape) {
        warnSheetsTableShape << Q_FUNC_INFO << "No table shape found in selection.";
        emit done();
        return;
    }
    d->separator.setSeparator(true);
    d->toolPressed = None;
    d->focus = None;

    d->tableShape->setPaintingDisabled(true);
    d->setupTools();

    d->selection->setActiveSheet(d->tableShape->sheet());
    d->selection->setOriginSheet(d->tableShape->sheet());
    useCursor(Qt::ArrowCursor);

    CellToolBase::activate(toolActivation, shapes);
    connect(d->selection, SIGNAL(changed(const Region&)), this, SLOT(slotSelectionChanged(const Region&)));

    activateSheet();

    connect(d->horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(slotHorizontalScrollBarValueChanged(int)));
    connect(d->verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(slotVerticalScrollBarValueChanged(int)));
    d->horizontalScrollBar->show();
    d->verticalScrollBar->show();
}

void TableTool::deactivate()
{
    d->horizontalScrollBar->hide();
    d->verticalScrollBar->hide();
    disconnect(d->horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(slotHorizontalScrollBarValueChanged(int)));
    disconnect(d->verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(slotVerticalScrollBarValueChanged(int)));

    disconnect(d->selection, SIGNAL(changed(const Region&)), this, SLOT(slotSelectionChanged(const Region&)));

    // undoing createShape crash if we where editing a cell and try to apply user input
    deleteEditor(false /*do not save*/);
    CellToolBase::deactivate();

    if (d->tableShape) {
        disconnect(d->tableShape->sheet(), SIGNAL(documentSizeChanged(const QSizeF&)), this, SLOT(update()));

        d->tableShape->setPaintingDisabled(false);
        d->tableShape->update();
        d->tableShape = nullptr;
    }
    update();
}

void TableTool::paint(QPainter &painter, const KoViewConverter &viewConverter)
{
    QTransform painterMatrix = painter.worldTransform();
    painter.setWorldTransform(d->tableShape->absoluteTransformation(&viewConverter) * painterMatrix);
    KoShape::applyConversion(painter, viewConverter);

    d->tableShape->sheetView()->setViewConverter(&viewConverter);
    // paint decoration
    d->paintTools(painter, viewConverter);

    painter.setClipRect(QRectF(QPointF(), size()));
    painter.translate(-d->tableShape->topLeftOffset());
    const QRectF paintRect = QRectF(d->tableShape->topLeftOffset(), size());
    paintReferenceSelection(painter, paintRect);
    paintSelection(painter, paintRect);
}

QPointF TableTool::offset() const
{
    QPointF point = d->tableShape->absolutePosition(KoFlake::TopLeftCorner);
    point -= d->tableShape->topLeftOffset();
    return point;
}

QSizeF TableTool::size() const
{
    return d->tableShape->size();
}

qreal TableTool::canvasOffsetX() const
{
    return canvas()->viewConverter()->viewToDocumentX(canvas()->documentOrigin().x() - canvas()->canvasController()->scrollBarValue().x());
}

qreal TableTool::canvasOffsetY() const
{
    return canvas()->viewConverter()->viewToDocumentY(canvas()->documentOrigin().y() - canvas()->canvasController()->scrollBarValue().y());
}

// not used anywhere
QPointF TableTool::canvasOffset() const
{
    return QPointF(canvasOffsetX(), canvasOffsetY());
}

int TableTool::maxCol() const
{
    return KS_colMax;
}

int TableTool::maxRow() const
{
    return KS_rowMax;
}

SheetView* TableTool::sheetView(const Sheet* sheet) const
{
    Q_UNUSED(sheet);
    return d->tableShape->sheetView();
}

void TableTool::update()
{
    canvas()->updateCanvas(d->resizeRect);
}

void TableTool::saveCurrentSheetSelection()
{
    /* save the current selection on this sheet */
    Sheet *sheet = selection()->activeSheet();
    if (sheet) {
        d->savedAnchors.remove(sheet);
        d->savedAnchors.insert(sheet, selection()->anchor());
        d->savedMarkers.remove(sheet);
        d->savedMarkers.insert(sheet, selection()->marker());
        d->savedOffsets.remove(sheet);
        d->savedOffsets.insert(sheet, QPoint(d->horizontalScrollBar->value(), d->verticalScrollBar->value()));
    }
}

void TableTool::activateSheet()
{
    saveCurrentSheetSelection();
    deleteEditor(true);

    Sheet *sheet = d->tableShape->sheet();
    if (sheet) {
        connect(sheet, SIGNAL(documentSizeChanged(const QSizeF&)), this, SLOT(update()));

        /* see if there was a previous selection on this other sheet */
        QMap<Sheet*, QPoint>::ConstIterator it = d->savedAnchors.constFind(sheet);
        QMap<Sheet*, QPoint>::ConstIterator it2 = d->savedMarkers.constFind(sheet);

        // restore the old anchor and marker
        const QPoint newAnchor = (it == d->savedAnchors.constEnd()) ? QPoint(1, 1) : *it;
        const QPoint newMarker = (it2 == d->savedMarkers.constEnd()) ? QPoint(1, 1) : *it2;

        selection()->setActiveSheet(sheet);
        selection()->setOriginSheet(sheet);
        selection()->initialize(newAnchor);
        selection()->update(newMarker);

        QMap<Sheet*, QPoint>::ConstIterator it3 = d->savedOffsets.constFind(sheet);
        d->horizontalScrollBar->setValue((it3 == d->savedOffsets.constEnd()) ? 0 : it3.value().x());
        d->verticalScrollBar->setValue((it3 == d->savedOffsets.constEnd()) ? 0 : it3.value().y());
    }
    update();
}

void TableTool::sheetActivated(const QString& sheetName)
{
    Sheet *sheet = d->tableShape->sheet();
    if (sheet) {
        disconnect(sheet, SIGNAL(documentSizeChanged(const QSizeF&)), this, SLOT(update()));
    }
    d->tableShape->setSheet(sheetName);
    activateSheet();
}

void TableTool::openSheetsEditor()
{
    SheetsEditor *dialog = new SheetsEditor(d->tableShape);
    connect(dialog, SIGNAL(sheetModified(Sheet*)), this, SLOT(slotSheetModified(Sheet*)));
    connect(dialog, SIGNAL(sheetRemoved(int)), this, SLOT(slotSheetRemoved(int)));
    dialog->exec();
    delete dialog;
}

QList<QPointer<QWidget> > TableTool::createOptionWidgets()
{
    return CellToolBase::createOptionWidgets();
}

void TableTool::slotHorizontalScrollBarValueChanged(int value)
{
    QPointF pos = d->tableShape->topLeftOffset();
    pos.rx() = value;
    d->tableShape->setTopLeftOffset(pos);
    canvas()->updateCanvas(d->toolRect);
}

void TableTool::slotVerticalScrollBarValueChanged(int value)
{
    QPointF pos = d->tableShape->topLeftOffset();
    pos.ry() = value;
    d->tableShape->setTopLeftOffset(pos);
    canvas()->updateCanvas(d->toolRect);
}

void TableTool::scrollToCell(const QPoint &location)
{
    if (location.x() > KS_colMax || location.y() > KS_rowMax) {
        return;
    }
    const Sheet *sheet = d->tableShape->sheet();
    const QRectF bounds(d->tableShape->topLeftOffset(), d->tableShape->size());
    const Cell cell = Cell(sheet, location).masterCell();

    const qreal left = sheet->columnPosition(cell.cellPosition().x());
    const qreal right = left + cell.width();
    if (left < bounds.left()) {
        d->horizontalScrollBar->setValue(left);
    } else if (right > bounds.right()) {
        if (d->horizontalScrollBar->maximum() < right) {
            d->horizontalScrollBar->setMaximum(right);
        }
        d->horizontalScrollBar->setValue(right - bounds.width() + 1);
    }

    const qreal top = sheet->rowPosition(cell.cellPosition().y());
    const qreal bottom = top + cell.height();
    if (top < bounds.top()) {
        d->verticalScrollBar->setValue(top);
    } else if (bottom > bounds.bottom()) {
        if (d->verticalScrollBar->maximum() < bottom) {
            d->verticalScrollBar->setMaximum(bottom);
        }
        d->verticalScrollBar->setValue(bottom - bounds.height() + 1);
    }
}

void TableTool::recalcSheet()
{
    d->tableShape->map()->recalcManager()->recalcSheet(d->tableShape->sheet());
}

void TableTool::recalcMap()
{
    d->tableShape->map()->recalcManager()->recalcMap();
}

void TableTool::createActions()
{
#if 0
    QAction* exportAction = new QAction(koIcon("document-export"), i18n("Export OpenDocument Spreadsheet File"), this);
    exportAction->setIconText(i18n("Export"));
    addAction("export", exportAction);
    connect(exportAction, SIGNAL(triggered()), this, SLOT(exportDocument()));
#endif

    QAction *a = new QAction(i18n("Recalculate Sheet"), this);
    a->setIcon(koIcon("view-refresh"));
    a->setIconText(i18n("Recalculate"));
    a->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_F9));
    a->setToolTip(i18n("Recalculate the value of every cell in the current worksheet"));
    addAction("RecalcWorkSheet", a);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(recalcSheet()));

    a = new QAction(i18n("Recalculate Document"), this);
    a->setIcon(koIcon("view-refresh"));
    a->setIconText(i18n("Recalculate"));
    a->setShortcut(QKeySequence(Qt::Key_F9));
    a->setToolTip(i18n("Recalculate the value of every cell in all worksheets"));
    addAction("RecalcWorkBook", a);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(recalcMap()));

    a = new QAction(koIcon("document-edit"), i18n("Sheets..."), this);
    addAction("sheets-editor", a);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(openSheetsEditor()));
    a->setToolTip(i18n("Add, rename or remove sheet"));
}

QList<QAction*> TableTool::popupMenuActionList() const
{
    QList<QAction*> actions = CellToolBase::popupMenuActionList();
    actions << &d->separator;
    actions << action("RecalcWorkSheet");
    actions << action("RecalcWorkBook");
    return actions;
}

void TableTool::sheetSelected()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (action && action->text() != d->tableShape->sheet()->sheetName()) {
        sheetActivated(action->text());
    }
}
void TableTool::sheetSelectionCombo()
{
    const Map *map = d->tableShape->map();
    QMenu menu;
    QList<QAction*> actions;
    foreach(const QString &s, map->visibleSheets()) {
        actions << new QAction(s, &menu);
        connect(actions.last(), SIGNAL(triggered(bool)), this, SLOT(sheetSelected()));
    }
    menu.addActions(actions);
    const KoViewConverter *viewConverter = canvas()->viewConverter();
    const QPointF documentOrigin = canvas()->documentOrigin();
    const QPoint canvasScroll(d->controller->canvasOffsetX(), d->controller->canvasOffsetY());
    QRect rect = viewConverter->documentToView(d->sheetNameRect).translated(documentOrigin).toRect();
    rect.translate(canvasScroll);
    menu.exec(d->controller->mapToGlobal(rect.bottomLeft()));
    return;
}

void TableTool::slotSheetAdded(Sheet*)
{

}
void TableTool::slotSheetRemoved(int row)
{
    const QStringList names = d->tableShape->map()->visibleSheets();
    d->tableShape->setSheet(names.value(0));
    update();
}

void TableTool::slotSheetModified(Sheet *sheet)
{
    Q_ASSERT(sheet);
    QString name = sheet->sheetName();
    if (sheet->isHidden() && sheet == d->tableShape->sheet()) {
        name = d->tableShape->map()->visibleSheets().value(0);
    }
    d->tableShape->setSheet(name);
    update();
}

void TableTool::slotShapeChanged(KoShape *shape)
{
    if (shape == d->tableShape) {
        QRectF old = d->resizeRect;
        d->setupTools();
        canvas()->updateCanvas(old | d->resizeRect);
    }
}

TableShape *TableTool::shape() const
{
    return d->tableShape;
}
