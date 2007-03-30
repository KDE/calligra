/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2006 Gary Cramblitt <garycramblitt@comcast.net>
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

#include "KWPagePreview.h"

#include <KoGlobal.h>
#include <KoUnit.h>

#include <QPainter>
#include <kdebug.h>

KWPagePreview::KWPagePreview(QWidget *parent)
:QWidget(parent)
{
    m_columns.columns = 1;
    m_columns.columnSpacing = 0;
    m_pageLayout = KoPageLayout::standardLayout();
    setMinimumSize( 100, 100 );
}

void KWPagePreview::paintEvent(QPaintEvent *event) {
    // resolution[XY] is in pixel per pt
    double resolutionX = POINT_TO_INCH( static_cast<double>(KoGlobal::dpiX()) );
    double resolutionY = POINT_TO_INCH( static_cast<double>(KoGlobal::dpiY()) );

    double pageWidth = m_pageLayout.width * resolutionX;
    double pageHeight = m_pageLayout.height * resolutionY;
    if(m_pageLayout.orientation == KoPageFormat::Landscape)
        qSwap(pageHeight, pageWidth);

    const bool pageSpread = (m_pageLayout.left < 0 || m_pageLayout.right < 0);

    double zoomH = (height() * 90 / 100) / pageHeight;
    double zoomW = (width() * 90 / 100) / (pageWidth * (pageSpread?2:1));
    double zoom = qMin( zoomW, zoomH );

    pageWidth *= zoom;
    pageHeight *= zoom;
    QPainter painter( this );

    QRect page = QRectF((width() - pageWidth * (pageSpread?2:1)) / 2.0,
            (height() - pageHeight) / 2.0, pageWidth, pageHeight).toRect();

    painter.save();
    drawPage(painter, zoom, page, true);
    painter.restore();
    if(pageSpread) {
        page.moveLeft(page.left() + (int) (pageWidth));
        painter.save();
        drawPage(painter, zoom, page, false);
        painter.restore();
    }

    painter.end();

    // paint scale
}

void KWPagePreview::drawPage(QPainter &painter, double zoom, const QRect &dimensions, bool left) {
    painter.fillRect(dimensions, QBrush(Qt::white));
    painter.setPen(QPen(Qt::gray));
    painter.drawRect(dimensions);

    // draw text areas
    QRect textArea = dimensions;
    if(m_pageLayout.top == 0 && m_pageLayout.bottom == 0 &&
            ( m_pageLayout.left == 0 && m_pageLayout.right == 0) ||
            ( m_pageLayout.pageEdge == 0 && m_pageLayout.bindingSide == 0))
        // no margin
        return;
    else {
        textArea.setTop(textArea.top() + qRound(zoom * m_pageLayout.top));
        textArea.setBottom(textArea.bottom() - qRound(zoom * m_pageLayout.bottom));

        double leftMargin, rightMargin;
        if(m_pageLayout.bindingSide < 0) { // normal margins.
            leftMargin = m_pageLayout.left;
            rightMargin = m_pageLayout.right;
        }
        else { // margins mirrored for left/right pages
            leftMargin = m_pageLayout.bindingSide;
            rightMargin = m_pageLayout.pageEdge;
            if(left)
                qSwap(leftMargin, rightMargin);
        }
        textArea.setLeft(textArea.left() + qRound(zoom * leftMargin));
        textArea.setRight(textArea.right() - qRound(zoom * rightMargin));
    }
    painter.setBrush( QBrush( Qt::black, Qt::HorPattern ) );
    painter.setPen( Qt::lightGray );

    double columnWidth = (textArea.width() + (m_columns.columnSpacing * zoom)) / m_columns.columns;
    int width = qRound(columnWidth - m_columns.columnSpacing * zoom);
    for ( int i = 0; i < m_columns.columns; ++i )
        painter.drawRect( qRound(textArea.x() + i * columnWidth), textArea.y(), width, textArea.height());

}

void KWPagePreview::setPageLayout(const KoPageLayout &layout) {
    m_pageLayout = layout;
    update();
}

void KWPagePreview::setColumns(const KoColumns &columns) {
    m_columns = columns;
    update();
}

#include "KWPagePreview.moc"
