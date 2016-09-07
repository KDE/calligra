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

#include "KPrTimeLineHeader.h"

#include "KPrAnimationsTimeLineView.h"
#include "KPrShapeAnimations.h"

#include <klocalizedstring.h>

//Qt Headers
#include <QPainter>
#include <QScrollArea>
#include <QEvent>
#include <QResizeEvent>
#include <QScrollBar>
#include <QStyle>
#include <QStyleOptionHeader>

//Default height of the header
const int HEADER_HEIGHT = 20;

KPrTimeLineHeader::KPrTimeLineHeader(QWidget *parent)
    : QWidget(parent)
{
    m_mainView = qobject_cast<KPrAnimationsTimeLineView*>(parent);
    Q_ASSERT(m_mainView);
    setMinimumSize(minimumSizeHint());
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

QSize KPrTimeLineHeader::minimumSizeHint() const
{
    return QSize(m_mainView->totalWidth() * 0.25, HEADER_HEIGHT);
}

void KPrTimeLineHeader::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    paintHeader(&painter, height());
    painter.setPen(QPen(palette().button().color().darker(), 0.5));
    painter.drawRect(0, 0, width(), height());
}

void KPrTimeLineHeader::paintHeader(QPainter *painter, const int RowHeight)
{
    int scroll = m_mainView->scrollArea()->horizontalScrollBar()->value();
    QFontMetrics fm(font());
    int minimumSize = fm.width(QString("W%1W").arg("seconds"));
    if (scroll < (m_mainView->totalWidth()-m_mainView->widthOfColumn(KPrShapeAnimations::StartTime) - minimumSize)) {
        //Seconds Header
        QRect rect(0, 0, m_mainView->totalWidth() - m_mainView->widthOfColumn(KPrShapeAnimations::StartTime) - scroll, RowHeight);
        paintHeaderItem(painter, rect, i18n("seconds"));
    } else if (scroll < (m_mainView->totalWidth()-m_mainView->widthOfColumn(KPrShapeAnimations::StartTime)) - 2) {
        QRect rect(0, 0, m_mainView->totalWidth()-m_mainView->widthOfColumn(KPrShapeAnimations::StartTime) - scroll, RowHeight);
        paintHeaderItem(painter, rect, QString(""));
    }
    // Paint time scale header
    QRect rect(m_mainView->totalWidth() - m_mainView->widthOfColumn(KPrShapeAnimations::StartTime) - scroll,
               0, m_mainView->widthOfColumn(KPrShapeAnimations::StartTime), RowHeight);
    paintHeaderItem(painter, rect, QString());
    paintTimeScale(painter, rect);

}

void KPrTimeLineHeader::paintHeaderItem(QPainter *painter, const QRect &rect, const QString &text)
{
    //Paint Background
    QStyleOptionHeader option;
    option.initFrom(this);
    option.rect = rect;
    style()->drawControl(QStyle::CE_HeaderSection, &option, painter, this);

    //Paint Border
    m_mainView->paintItemBorder(painter, palette(), rect);

    //Paint Text
    painter->setPen(palette().buttonText().color());
    painter->drawText(rect, text, QTextOption(Qt::AlignCenter));
}

void KPrTimeLineHeader::paintTimeScale(QPainter *painter, const QRect &rect)
{
    // set a marging
    const int Padding = 3;
    painter->setPen(QPen(palette().windowText().color(), 0));
    painter->setFont(QFont("", 8));
    // calculate size of scale steps
    int totalWidth = m_mainView->widthOfColumn(KPrShapeAnimations::StartTime);
    int stepScale = m_mainView->stepsScale();
    int stepRatio = m_mainView->numberOfSteps() / m_mainView->stepsScale();
    int stepSize = totalWidth / stepRatio;
    // Draw lines on steps and numbers
    for (int x = 0; x < totalWidth-Padding; x += stepSize) {
        int z = x + rect.x() + Padding;
        if (z > 0) {
            // Draw numbers
            qreal number = x/stepSize*stepScale;
            painter->drawText(((z - 19) > 1 ? (z - 19) : (z - 16)), rect.y(), 38, rect.height(),
                              Qt::AlignCenter, QString("%1").arg(number));
        }
    }
    // Draw substeps
    const int substeps = 5;
    stepSize = totalWidth / (stepRatio * substeps);
    for (qreal x = 0; x < (totalWidth - Padding); x += stepSize) {
        int z = x + rect.x() + Padding;
        if (z > 0) {
            painter->drawLine(z, 1, z, 3);
            painter->drawLine(z, rect.height() - 4, z, rect.height() - 2);
        }
    }
}

bool KPrTimeLineHeader::eventFilter(QObject *target, QEvent *event)
{
    int ExtraWidth = 10;
    if (QScrollArea *scrollArea = m_mainView->scrollArea()) {
        if (target == scrollArea && event->type() == QEvent::Resize) {
            if (QResizeEvent *resizeEvent = static_cast<QResizeEvent*>(event)) {
                QSize size = resizeEvent->size();
                size.setHeight(sizeHint().height());
                int width = size.width() + ExtraWidth - scrollArea->verticalScrollBar()->sizeHint().width();
                size.setWidth(width);
                setMinimumSize(QSize(m_mainView->totalWidth() - m_mainView->widthOfColumn(KPrShapeAnimations::StartTime),
                                     size.height()));
                resize(size);
            }
        }
    }
    return QWidget::eventFilter(target, event);
}
