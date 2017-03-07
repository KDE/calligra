/* This file is part of the KDE project
 * Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
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

#include "KPrTimeLineView.h"

//Stage Headers
#include "KPrAnimationsTimeLineView.h"
#include "tools/animationtool/KPrAnimationGroupProxyModel.h"
#include "KPrShapeAnimations.h"

//QT HEADERS
#include <QScrollArea>
#include <QVBoxLayout>
#include <QRect>
#include <QModelIndex>
#include <QPixmap>
#include <QEvent>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QScrollBar>
#include <QToolTip>
#include <qmath.h>

//Default height for rows
const int LINE_HEIGHT = 25;
const int BAR_MARGIN = 2;
const int RESIZE_RADIUS = 2;

//Default invalid value for columns and rows index
const int INVALID = -1;

KPrTimeLineView::KPrTimeLineView(QWidget *parent)
    : QWidget(parent)
    , m_resize(false)
    , m_move(false)
    , m_resizedRow(INVALID)
    , startDragPos(0)
    , m_adjust(false)
{
    m_mainView = qobject_cast<KPrAnimationsTimeLineView*>(parent);
    Q_ASSERT(m_mainView);
    setFocusPolicy(Qt::WheelFocus);
    setMinimumSize(minimumSizeHint());
    setMouseTracking(true);
}

QSize KPrTimeLineView::sizeHint() const
{
    int rows = m_mainView->model()
            ? m_mainView->rowCount() : 1;
    return QSize(m_mainView->totalWidth(), rows * m_mainView->rowsHeight());
}

QSize KPrTimeLineView::minimumSizeHint() const
{
    int rows = m_mainView->model()
            ? m_mainView->rowCount() : 1;
    return QSize(m_mainView->totalWidth(), rows * m_mainView->rowsHeight());
}

bool KPrTimeLineView::eventFilter(QObject *target, QEvent *event)
{
    if (QScrollArea *scrollArea = m_mainView->scrollArea()) {
        if (target == scrollArea && event->type() == QEvent::Resize) {
            if (QResizeEvent *resizeEvent = static_cast<QResizeEvent*>(event)) {
                const int ExtraWidth = 5;
                QSize size = resizeEvent->size();
                size.setHeight(sizeHint().height());
                int width = size.width() - (ExtraWidth +
                                            scrollArea->verticalScrollBar()->sizeHint().width());
                size.setWidth(width);
                resize(size);
            }
        }
    }
    return QWidget::eventFilter(target, event);
}

void KPrTimeLineView::keyPressEvent(QKeyEvent *event)
{
    if (m_mainView->model()) {
        int row = INVALID;
        int column = INVALID;
        if (event->key() == Qt::Key_Left) {
            column = qMax(m_mainView->startColumn(), m_mainView->selectedColumn() - 1);
        }
        else if (event->key() == Qt::Key_Right) {
            column = qMin(m_mainView->endColumn(),
                          m_mainView->selectedColumn() + 1);
        }
        else if (event->key() == Qt::Key_Up) {
            row = qMax(0, m_mainView->selectedRow() - 1);
        }
        else if (event->key() == Qt::Key_Down) {
            row = qMin(m_mainView->model()->rowCount() - 1,
                       m_mainView->selectedRow() + 1);
        }
        row = row == INVALID ? m_mainView->selectedRow() : row;
        column = column == INVALID ? m_mainView->selectedColumn() : column;
        if (row != m_mainView->selectedRow() ||
                column != m_mainView->selectedColumn()) {
            QModelIndex index = m_mainView->model()->index(row, column);
            m_mainView->setCurrentIndex(index);
            emit clicked(index);
            return;
        }
    }
    QWidget::keyPressEvent(event);
}

void KPrTimeLineView::mousePressEvent(QMouseEvent *event)
{
    int row = rowAt(event->y());
    int column = columnAt(event->x());

    m_mainView->setSelectedRow(row);
    m_mainView->setSelectedColumn(column);
    // Request context menu
    if (event->button()== Qt::RightButton) {
        emit customContextMenuRequested(event->pos());
    }
    // Check if user wants to move the time bars
    if (event->button() == Qt::LeftButton) {
        if (column == KPrShapeAnimations::StartTime) {
            m_resize = false;
            m_move = false;

            QRectF lineRect = getRowRect(row, column);
            QRectF endLineRect = QRectF(lineRect.right() - RESIZE_RADIUS, lineRect.top(),
                                        RESIZE_RADIUS * 2, lineRect.height());

            // If the user clicks near the end of the line they could resize otherwise they move the bar.
            if (endLineRect.contains(event->x(), event->y())) {
                m_resize = true;
                m_resizedRow = row;
                setCursor(Qt::SizeHorCursor);
            } else {
                m_resize = false;
                m_move = false;
                if (lineRect.contains(event->x(), event->y())) {
                    startDragPos = event->x() - lineRect.x();
                    m_move = true;
                    m_resizedRow = row;
                    setCursor(Qt::DragMoveCursor);
                }
            }
        }
    }
    emit clicked(m_mainView->model()->index(row, column));


}

void KPrTimeLineView::mouseMoveEvent(QMouseEvent *event)
{
    // Resize the bar
    if (m_resize) {
        const qreal subSteps = 0.2;
        int startPos = 0;
        for (int i = 0; i < KPrShapeAnimations::StartTime; i++) {
            startPos = startPos + m_mainView->widthOfColumn(i);
        }
        int row = m_resizedRow;
        //calculate real start
        qreal startOffSet = m_mainView->calculateStartOffset(row) / 1000.0;

        qreal start = m_mainView->model()->data(m_mainView->model()->index(row, KPrShapeAnimations::StartTime)).toInt() / 1000.0;
        qreal duration = m_mainView->model()->data(m_mainView->model()->index(row, KPrShapeAnimations::Duration)).toInt() / 1000.0;
        qreal totalSteps = m_mainView->numberOfSteps();
        qreal stepSize  = m_mainView->widthOfColumn( KPrShapeAnimations::StartTime) / totalSteps;

        if ((event->pos().x() > (startPos + startOffSet*stepSize + stepSize * start - 5)) &&
                ((event->pos().x()) < (startPos + m_mainView->widthOfColumn( KPrShapeAnimations::StartTime)))) {
            qreal newLength = (event->pos().x() - startPos - stepSize * start) / (stepSize) - startOffSet;
            newLength = qFloor((newLength - modD(newLength, subSteps)) * 100.0) / 100.0;
            // update bar length
            m_mainView->model()->setData(m_mainView->model()->index(row, KPrShapeAnimations::Duration), newLength * 1000);
            emit timeValuesChanged(m_mainView->model()->index(row, KPrShapeAnimations::Duration));
            m_adjust = false;
            if (newLength < duration)
                m_adjust = true;
        } else if ( ((event->pos().x()) > (startPos + m_mainView->widthOfColumn( KPrShapeAnimations::StartTime)))) {
            m_mainView->incrementScale();
            m_adjust = true;
        }
        update();
    }
    //Move the bar
    if (m_move) {
        const int Padding = 2;
        int startPos = 0;
        const qreal subSteps = 0.2;
        for (int i = 0; i < KPrShapeAnimations::StartTime; i++) {
            startPos = startPos + m_mainView->widthOfColumn(i);
        }
        int row = m_resizedRow;
        //calculate real start
        qreal startOffSet = m_mainView->calculateStartOffset(row) / 1000;
        qreal duration = m_mainView->model()->data(m_mainView->model()->index(row, KPrShapeAnimations::Duration)).toInt() / 1000.0;
        qreal start = m_mainView->model()->data(m_mainView->model()->index(row, KPrShapeAnimations::StartTime)).toInt() / 1000.0;
        qreal totalSteps = m_mainView->numberOfSteps();
        qreal stepSize  = m_mainView->widthOfColumn(KPrShapeAnimations::StartTime) / totalSteps;
        qreal Threshold = 0.4;
        if ((event->pos().x() > (startPos + startDragPos + startOffSet*stepSize)) &&
                ((event->pos().x() + (duration * stepSize - startDragPos) + Padding * 2)  <
                 (startPos+m_mainView->widthOfColumn( KPrShapeAnimations::StartTime)))) {
            qreal newPos = (event->pos().x() - (startPos + startDragPos)) / (stepSize) - startOffSet;
            newPos = qFloor((newPos - modD(newPos, subSteps)) * 100.0) / 100.0;
            // update bar position
            m_mainView->model()->setData(m_mainView->model()->index(row, KPrShapeAnimations::StartTime), newPos * 1000);
            emit timeValuesChanged(m_mainView->model()->index(row, KPrShapeAnimations::StartTime));
            m_adjust = false;
            if (newPos <= start) {
                m_adjust = true;
            }
        }
        else if (((event->pos().x() + (duration*stepSize-startDragPos) + Padding * 2)  >
                    (startPos + m_mainView->widthOfColumn( KPrShapeAnimations::StartTime)))) {
            m_mainView->incrementScale();
        }
        else if (event->pos().x() < (startPos + startDragPos + startOffSet * stepSize + Threshold)) {
            m_mainView->changeStartLimit(row);
        }
        update();
    }
    int row = rowAt(event->y());
    int column = columnAt(event->x());
    if (column == KPrShapeAnimations::StartTime) {
        QRectF lineRect = getRowRect(row, column);
        QRectF endLineRect = QRectF(lineRect.right() - RESIZE_RADIUS, lineRect.top() + BAR_MARGIN,
                                    RESIZE_RADIUS * 2,  lineRect.height() - 2 * BAR_MARGIN);

        // If the user is near the end of the line they could resize
        if (endLineRect.contains(event->x(), event->y())) {
            setCursor(Qt::SizeHorCursor);
        }
        else {
            if (lineRect.contains(event->x(), event->y())) {
                setCursor(Qt::DragMoveCursor);
            }
            else {
                setCursor(Qt::ArrowCursor);
            }
        }
    }
    QWidget::mouseMoveEvent(event);
}

void KPrTimeLineView::mouseReleaseEvent(QMouseEvent *event)
{
    m_resize = false;
    m_move = false;
    if (m_adjust) {
        m_mainView->adjustScale();
        m_adjust = false;
    }
    m_mainView->animationsModel()->endTimeLineEdition();
    setCursor(Qt::ArrowCursor);
    QWidget::mouseReleaseEvent(event);
    update();
}

bool KPrTimeLineView::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip) {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        QModelIndex index = m_mainView->model()->index(rowAt(helpEvent->pos().y()),columnAt(helpEvent->pos().x()));
        if (index.isValid()) {
            QString text = m_mainView->model()->data(index, Qt::ToolTipRole).toString();
            QToolTip::showText(helpEvent->globalPos(), text);
        } else {
            QToolTip::hideText();
            event->ignore();
        }

        return true;
    }
    return QWidget::event(event);
}

int KPrTimeLineView::rowAt(int ypos)
{
    int row = static_cast<int>(ypos / m_mainView->rowsHeight());
    return row;
}

int KPrTimeLineView::columnAt(int xpos)
{
    int column;
    if (xpos  < m_mainView->widthOfColumn(KPrShapeAnimations::ShapeThumbnail)) {
        column = KPrShapeAnimations::ShapeThumbnail;
    }
    else if (xpos  < m_mainView->widthOfColumn(KPrShapeAnimations::ShapeThumbnail) +
             m_mainView->widthOfColumn( KPrShapeAnimations::AnimationIcon)) {
        column = KPrShapeAnimations::AnimationIcon;
    }
    else {
        column = KPrShapeAnimations::StartTime;
    }
    return column;
}

QRectF KPrTimeLineView::getRowRect(const int row, const int column)
{
    int startPos = 0;
    for (int i = 0; i < KPrShapeAnimations::StartTime; i++) {
        startPos = startPos + m_mainView->widthOfColumn(i);
    }
    int y = row * m_mainView->rowsHeight();
    QRect rect(startPos, y, startPos+m_mainView->widthOfColumn(column), m_mainView->rowsHeight());

    const int lineHeight = qMin(LINE_HEIGHT, rect.height());
    const int yCenter = (rect.height() - lineHeight) / 2;
    qreal  stepSize  = m_mainView->widthOfColumn(KPrShapeAnimations::StartTime) / m_mainView->numberOfSteps();
    qreal duration = m_mainView->model()->data(m_mainView->model()->index(row, KPrShapeAnimations::Duration)).toInt() / 1000.0;
    int startOffSet = m_mainView->calculateStartOffset(row);
    qreal start = (m_mainView->model()->data(m_mainView->model()->index(row, KPrShapeAnimations::StartTime)).toInt() +
            startOffSet) / 1000.0;
    return QRectF(rect.x() + stepSize * start, rect.y() + yCenter, stepSize * duration, lineHeight);
}

void KPrTimeLineView::paintEvent(QPaintEvent *event)
{
    if (!m_mainView->model()) {
        return;
    }
    const int rowHeight = m_mainView->rowsHeight();
    const int minY = qMax(0, event->rect().y() - rowHeight);
    const int maxY = minY + event->rect().height() + rowHeight;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing |
                           QPainter::TextAntialiasing);
    int row = minY / rowHeight;
    int y = row * rowHeight;

    int rowCount = m_mainView->rowCount();
    for (; row < rowCount; ++row) {
        paintRow(&painter, row, y, rowHeight);
        y += rowHeight;
        if (y > maxY) {
            break;
        }
    }
}

void KPrTimeLineView::paintRow(QPainter *painter, int row, int y, const int RowHeight)
{
    int start = 0;
    //Column 0
    int column = KPrShapeAnimations::ShapeThumbnail;
    paintIconRow(painter, start, y, row, column, RowHeight - 2, RowHeight);

    //Column 1
    column = KPrShapeAnimations::AnimationIcon;
    start = start + m_mainView->widthOfColumn(column - 1);
    paintIconRow(painter, start, y, row, column, RowHeight / 2, RowHeight);

    //Column 2 (6 y 7)
    column = KPrShapeAnimations::StartTime;
    start = start + m_mainView->widthOfColumn(column - 1);
    QRect rect(start, y, m_mainView->widthOfColumn(column), RowHeight);
    paintItemBackground(painter, rect,
                        row == m_mainView->selectedRow());
    paintLine(painter, row, rect,
              row == m_mainView->selectedRow());
}

void KPrTimeLineView::paintLine(QPainter *painter, int row, const QRect &rect, bool selected)
{
    QColor m_color = m_mainView->barColor(row);
    const int lineHeight = qMin(LINE_HEIGHT , rect.height());
    const int vPadding = (rect.height() - lineHeight) / 2;
    qreal stepSize  = m_mainView->widthOfColumn(KPrShapeAnimations::StartTime) / m_mainView->numberOfSteps();
    qreal startOffSet = m_mainView->calculateStartOffset(row) / 1000.0;
    qreal duration = m_mainView->model()->data(m_mainView->model()->index(row, KPrShapeAnimations::Duration)).toInt() / 1000.0;
    qreal start = m_mainView->model()->data(m_mainView->model()->index(row, KPrShapeAnimations::StartTime)).toInt() / 1000.0
            + startOffSet;
    QRectF lineRect(rect.x() + stepSize * start, rect.y() + vPadding, stepSize * duration, lineHeight);

    QRectF fillRect (lineRect.x(),lineRect.y() + BAR_MARGIN, lineRect.width(), lineRect.height() - BAR_MARGIN * 2);
    QLinearGradient s_grad(lineRect.center().x(), lineRect.top(),
                           lineRect.center().x(), lineRect.bottom());
    if (selected) {
        s_grad.setColorAt(0, m_color.darker(150));
        s_grad.setColorAt(0.5, m_color.lighter(150));
        s_grad.setColorAt(1, m_color.darker(150));
        s_grad.setSpread(QGradient::ReflectSpread);
        painter->fillRect(fillRect, s_grad);
    }
    else {
        s_grad.setColorAt(0, m_color.darker(200));
        s_grad.setColorAt(0.5, m_color.lighter(125));
        s_grad.setColorAt(1, m_color.darker(200));
        s_grad.setSpread(QGradient::ReflectSpread);
        painter->fillRect(fillRect, s_grad);
    }
    QRect startRect(lineRect.x(), lineRect.y(), 3, lineRect.height());
    painter->fillRect(startRect, Qt::black);
    QRect endRect(lineRect.x() + lineRect.width(), lineRect.y(), 3, lineRect.height());
    painter->fillRect(endRect, Qt::black);
}

void KPrTimeLineView::paintTextRow(QPainter *painter, int x, int y, int row, int column, const int RowHeight)
{
    QRect rect(x,y,m_mainView->widthOfColumn(column), RowHeight);
    paintItemBackground(painter, rect,
                        row == m_mainView->selectedRow());
    painter->drawText(rect,
                      m_mainView->model()->data(m_mainView->model()->index(row, column)).toString(),
                      QTextOption(Qt::AlignCenter));
}

void KPrTimeLineView::paintIconRow(QPainter *painter, int x, int y, int row, int column, int iconSize, const int RowHeight)
{
    QRect rect(x,y,m_mainView->widthOfColumn(column), RowHeight);
    paintItemBackground(painter, rect,
                        row == m_mainView->selectedRow());
    QPixmap thumbnail =  (m_mainView->model()->data(m_mainView->model()->index(row,column), Qt::DecorationRole)).value<QPixmap>();
    thumbnail.scaled(iconSize, iconSize , Qt::KeepAspectRatio);
    int width = 0;
    int height = 0;
    if (thumbnail.width() > thumbnail.height()) {
        width = iconSize;
        height = width * thumbnail.height() / thumbnail.width();
    } else {
        height = iconSize;
        width = height * thumbnail.width() / thumbnail.height();
    }

    qreal centerX = (m_mainView->widthOfColumn(column) - width) / 2;
    qreal centerY = (RowHeight - height) / 2;
    QRectF target(rect.x() + centerX, rect.y() + centerY, width, height);
    painter->save();
    if (row == m_mainView->selectedRow()) {
        painter->setCompositionMode(QPainter::CompositionMode_ColorBurn);
    }
    painter->drawPixmap(target, thumbnail, thumbnail.rect());
    painter->restore();
}

double KPrTimeLineView::modD(double x, double y)
{
    int intPart = static_cast<int>(x / y);
    return x - static_cast<double>(intPart) * y;
}

void KPrTimeLineView::paintItemBackground(QPainter *painter, const QRect &rect, bool selected)
{
    QLinearGradient gradient(rect.center().x(), rect.top(),
                             rect.center().x(), rect.bottom());
    QColor color = palette().highlight().color();
    gradient.setColorAt(0, color.lighter(125));
    gradient.setColorAt(1, color);
    painter->fillRect(rect, selected ? gradient : palette().base());
    m_mainView->paintItemBorder(painter, palette(), rect);
    painter->setPen(QPen(selected ? palette().highlightedText().color()
                                 : palette().windowText().color(), 0));
}
