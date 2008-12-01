/* This file is part of the KDE project
   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002 Till Busch <till@bux.at>
   Copyright (C) 2002 Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003-2008 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexirecordmarker.h"

#include <QApplication>
#include <QColor>
#include <QBrush>
#include <QPixmap>
#include <QPainter>
#include <QImage>
#include <QPolygon>
#include <QPaintEvent>
#include <QStyle>
#include <QStyleOptionHeader>

#include <KDebug>

#include <kexiutils/utils.h>
#include <kexi_global.h>

#include "../../pics/tableview_pen.xpm"
#include "../../pics/tableview_plus.xpm"

//! @internal
/*! @warning not reentrant! */
struct KexiRecordMarkerStatic {
    KexiRecordMarkerStatic()
    {
      initPixmaps();
    }

//! @todo handle application's palette changes...
    void initPixmaps()
    {
        // color-ify the icons to match the foreground color
        QColor fc(qApp->palette().color(QPalette::Foreground));
        pen = KexiUtils::replaceColors( QPixmap(tableview_pen_xpm), fc);
        plus = KexiUtils::replaceColors( QPixmap(tableview_plus_xpm), fc);
    }
    QPixmap pen, plus;
};

K_GLOBAL_STATIC(KexiRecordMarkerStatic, KexiRecordMarker_static)

//----------------------------------------------------------------

//! @internal
class KexiRecordMarker::Private
{
public:
    Private()
            : rowHeight(1)
            , offset(0)
            , currentRow(-1)
            , highlightedRow(-1)
            , editRow(-1)
            , rows(0)
            , showInsertRow(true) {
    }
    int rowHeight;
    int offset;
    int currentRow;
    int highlightedRow;
    int editRow;
    int rows;
    QBrush selectionBackgroundBrush;
bool showInsertRow : 1;
};

//----------------------------------------------------------------

KexiRecordMarker::KexiRecordMarker(QWidget *parent)
        : QWidget(parent)
        , d(new Private())
{
    setSelectionBackgroundBrush(parent->palette().brush(QPalette::Highlight));
    setMouseTracking(true);
}

KexiRecordMarker::~KexiRecordMarker()
{
    delete d;
}

//static
const QPixmap* KexiRecordMarker::penPixmap()
{
    return &KexiRecordMarker_static->pen;
}

//static
const QPixmap* KexiRecordMarker::plusPixmap()
{
    return &KexiRecordMarker_static->plus;
}

void KexiRecordMarker::addLabel(bool upd)
{
    d->rows++;
    if (upd)
        update();
}

void KexiRecordMarker::removeLabel(bool upd)
{
    if (d->rows > 0) {
        d->rows--;
        if (upd)
            update();
    }
}

void KexiRecordMarker::addLabels(int num, bool upd)
{
    d->rows += num;
    if (upd)
        update();
}

void KexiRecordMarker::clear(bool upd)
{
    d->rows = 0;
    if (upd)
        update();
}

int KexiRecordMarker::rows() const
{
    if (d->showInsertRow)
        return d->rows + 1;
    else
        return d->rows;
}

void KexiRecordMarker::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    QRect r(rect());

    int first = (r.top() + d->offset) / d->rowHeight;
    int last  = (r.bottom() + d->offset) / d->rowHeight;
    if (last > (d->rows - 1 + (d->showInsertRow ? 1 : 0)))
        last = d->rows - 1 + (d->showInsertRow ? 1 : 0);

    p.setBrush(palette().brush(backgroundRole()));
    p.save();
    for (int i = first; i <= last; i++) {
        int y = ((d->rowHeight * i) - d->offset);
        p.drawRect(r);
        QStyleOptionHeader optionHeader;
        optionHeader.initFrom(this);
        optionHeader.orientation = Qt::Vertical;
        optionHeader.state |= QStyle::State_Raised;
        if (isEnabled())
            optionHeader.state |= QStyle::State_Enabled;
        if (window()->isActiveWindow())
            optionHeader.state |= QStyle::State_Active;
        optionHeader.rect = QRect(0, y, width(), d->rowHeight);
        optionHeader.section = 0;
        // alter background for selected or highlighted row
        QColor alteredColor;
//! @todo Qt4: blend entire QBrush?
        if (d->currentRow == i) {
            alteredColor = KexiUtils::blendedColors(
                               palette().color(QPalette::Window), d->selectionBackgroundBrush.color(), 2, 1);
        }
        else if (d->highlightedRow == i) {
            alteredColor = KexiUtils::blendedColors(
                               palette().color(QPalette::Window), d->selectionBackgroundBrush.color(), 4, 1);
            optionHeader.state |= QStyle::State_MouseOver;
        }

        if (alteredColor.isValid()) {
            optionHeader.palette.setBrush(QPalette::Button, d->selectionBackgroundBrush);
            optionHeader.palette.setColor(QPalette::Button, alteredColor);
            //set background color as well (e.g. for thinkeramik)
            optionHeader.palette.setBrush(QPalette::Window, d->selectionBackgroundBrush);
            optionHeader.palette.setColor(QPalette::Window, alteredColor);
        }
        style()->drawControl(QStyle::CE_Header, &optionHeader, &p);
    }
    p.restore();

    if (d->editRow != -1 && d->editRow >= first && d->editRow <= (last/*+1 for insert row*/)) {
        //show pen when editing
        int ofs = d->rowHeight / 4;
        int pos = ((d->rowHeight * (d->currentRow >= 0 ? d->currentRow : 0)) - d->offset) - ofs / 2 + 1;
        p.drawPixmap((d->rowHeight - KexiRecordMarker_static->pen.width()) / 2,
                    (d->rowHeight - KexiRecordMarker_static->pen.height()) / 2 + pos, KexiRecordMarker_static->pen);
    } else if (d->currentRow >= first && d->currentRow <= last
               && (!d->showInsertRow || (d->showInsertRow && d->currentRow < last))) { /*don't display marker for 'insert' row*/
        //show marker
        p.setBrush(palette().brush(foregroundRole()));
        p.setPen(QPen(Qt::NoPen));
        QPolygon points(3);
        int ofs = d->rowHeight / 4;
        int ofs2 = (width() - ofs) / 2;
        int pos = ((d->rowHeight * d->currentRow) - d->offset) - ofs / 2 + 2;
        points.putPoints(0, 3, ofs2, pos + ofs, ofs2 + ofs, pos + ofs*2,
                         ofs2, pos + ofs*3);
        p.drawPolygon(points);
    }
    if (d->showInsertRow && d->editRow < last
            && last == (d->rows - 1 + (d->showInsertRow ? 1 : 0))) {
        //show plus sign
        int pos = ((d->rowHeight * last) - d->offset) + (d->rowHeight - KexiRecordMarker_static->plus.height()) / 2;
        p.drawPixmap((width() - KexiRecordMarker_static->plus.width()) / 2, pos, KexiRecordMarker_static->plus);
    }
}

void KexiRecordMarker::setCurrentRow(int row)
{
    if (row == d->currentRow)
        return;
    int oldRow = d->currentRow;
    d->currentRow = row;

    if (oldRow != -1)
        update(0, (d->rowHeight*(oldRow)) - d->offset - 1, width() + 2, d->rowHeight + 2);
    if (d->currentRow != -1)
        update(0, (d->rowHeight*d->currentRow) - d->offset - 1, width() + 2, d->rowHeight + 2);
}

void KexiRecordMarker::setHighlightedRow(int row)
{
    if (row == d->highlightedRow)
        return;
    int oldRow = d->highlightedRow;
    d->highlightedRow = row;

    if (oldRow != -1)
        update(0, (d->rowHeight*(oldRow)) - d->offset - 1, width() + 2, d->rowHeight + 2);
    if (d->currentRow != -1)
        update(0, (d->rowHeight*d->highlightedRow) - d->offset - 1, width() + 2, d->rowHeight + 2);
}

void KexiRecordMarker::setOffset(int offset)
{
    int oldOff = d->offset;
    d->offset = offset;
    scroll(0, oldOff - offset);
}

void KexiRecordMarker::setCellHeight(int cellHeight)
{
    d->rowHeight = cellHeight;
}

void KexiRecordMarker::setEditRow(int row)
{
    d->editRow = row;
//TODO: update only needed area!
    update();
}

void KexiRecordMarker::showInsertRow(bool show)
{
    d->showInsertRow = show;
//TODO: update only needed area!
    update();
}

void KexiRecordMarker::setSelectionBackgroundBrush(const QBrush &brush)
{
    d->selectionBackgroundBrush = brush;
}

QBrush KexiRecordMarker::selectionBackgroundBrush() const
{
    return d->selectionBackgroundBrush;
}

void KexiRecordMarker::mouseMoveEvent(QMouseEvent *e)
{
    const int y = e->y();
//    kDebug() << "y:" << y << "d->rowHeight:" << d->rowHeight;
//    kDebug() << "y / d->rowHeight:" << (y / d->rowHeight);
    const uint row = y / d->rowHeight;
    if ((int)row < rows()) {
        setHighlightedRow(row);
        emit rowHighlighted(row);
    }
    else {
        setHighlightedRow(-1);
        emit rowHighlighted(-1);
    }
    QWidget::mouseMoveEvent(e);
}

void KexiRecordMarker::mousePressEvent(QMouseEvent *e)
{
    const int y = e->y();
    const uint row = y / d->rowHeight;
    if ((int)row < rows()) {
        emit rowPressed(row);
    }
    QWidget::mousePressEvent(e);
}

void KexiRecordMarker::leaveEvent(QEvent *e)
{
    setHighlightedRow(-1);
    emit rowHighlighted(-1);
    QWidget::leaveEvent(e);
}

#include "kexirecordmarker.moc"
