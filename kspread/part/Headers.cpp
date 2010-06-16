/* This file is part of the KDE project
   Copyright 2006 Robert Knight <robertknight@gmail.com>
   Copyright 2006 Inge Wallin <inge@lysator.liu.se>
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 1999-2002,2004 Laurent Montel <montel@kde.org>
   Copyright 2002-2005 Ariya Hidayat <ariya@kde.org>
   Copyright 1999-2004 David Faure <faure@kde.org>
   Copyright 2004-2005 Meni Livne <livne@kde.org>
   Copyright 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2003 Hamish Rodda <rodda@kde.org>
   Copyright 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright 2003 Lukas Tinkl <lukas@kde.org>
   Copyright 2000-2002 Werner Trobin <trobin@kde.org>
   Copyright 2002 Harri Porten <porten@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 2002 Daniel Naber <daniel.naber@t-online.de>
   Copyright 1999-2000 Torben Weis <weis@kde.org>
   Copyright 1999-2000 Stephan Kulow <coolo@kde.org>
   Copyright 2000 Bernd Wuebben <wuebben@kde.org>
   Copyright 2000 Wilco Greven <greven@kde.org>
   Copyright 2000 Simon Hausmann <hausmann@kde.org
   Copyright 1999 Michael Reiher <michael.reiher@gmx.de>
   Copyright 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>
   Copyright 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Local
#include "Headers.h"

// Qt
#include <QApplication>
#include <QDesktopWidget>
#include <QLabel>
#include <QPainter>
#include <QRubberBand>
#include <QStyle>
#include <QTextLayout>
#include <QToolTip>
#include <QScrollBar>

// KDE
#include <klocale.h>
#include <kmessagebox.h>
#include <kwordwrap.h>

// KOffice
#include <KoCanvasController.h>
#include <KoToolProxy.h>
#include <KoZoomHandler.h>
#include <KoDpi.h>

// KSpread
#include "Canvas.h"
#include "Canvas_p.h"
#include "Cell.h"
#include "Doc.h"
#include "Limits.h"
#include "RowColumnFormat.h"
#include "Sheet.h"
#include "Selection.h"
#include "View.h"

// commands
#include "commands/RowColumnManipulators.h"

using namespace KSpread;

/****************************************************************
 *
 * RowHeader
 *
 ****************************************************************/

RowHeader::RowHeader(QWidget *_parent, Canvas *_canvas, View *_view)
        : QWidget(_parent)
{
    m_pView = _view;
    m_pCanvas = _canvas;
    m_lSize = 0;
    m_rubberband = 0;
    m_cellToolIsActive = true;

    setAttribute(Qt::WA_StaticContents);

    setMouseTracking(true);
    m_bResize = false;
    m_bSelection = false;
    m_iSelectionAnchor = 1;
    m_bMousePressed = false;

    connect(m_pView, SIGNAL(autoScroll(const QPoint &)),
            this, SLOT(slotAutoScroll(const QPoint &)));
    connect(m_pCanvas->toolProxy(), SIGNAL(toolChanged(const QString&)),
            this, SLOT(toolChanged(const QString&)));
}


RowHeader::~RowHeader()
{
}

void RowHeader::mousePressEvent(QMouseEvent * _ev)
{
    if (!m_cellToolIsActive)
        return;
    if (!m_pView->koDocument()->isReadWrite())
        return;

    register Sheet * const sheet = m_pView->activeSheet();
    if (!sheet)
        return;

    if (_ev->button() == Qt::LeftButton) {
        m_bMousePressed = true;
        m_pView->enableAutoScroll();
    }

    double ev_PosY = m_pView->zoomHandler()->unzoomItY(_ev->pos().y()) + m_pCanvas->yOffset();
    double dHeight = m_pView->zoomHandler()->unzoomItY(height());
    m_bResize = false;
    m_bSelection = false;

    // We were editing a cell -> save value and get out of editing mode
    m_pView->selection()->emitCloseEditor(true); // save changes

    // Find the first visible row and the y position of this row.
    double y;
    int row = sheet->topRow(m_pCanvas->yOffset(), y);

    // Did the user click between two rows?
    while (y < (dHeight + m_pCanvas->yOffset()) && (!m_bResize) && row <= KS_rowMax) {
        double h = sheet->rowFormat(row)->height();
        row++;
        if (row > KS_rowMax)
            row = KS_rowMax;
        if ((ev_PosY >= y + h - 2) &&
                (ev_PosY <= y + h + 1) &&
                !(sheet->rowFormat(row)->isHiddenOrFiltered() && row == 1))
            m_bResize = true;
        y += h;
    }

    //if row is hide and it's the first row
    //you mustn't resize it.
    double tmp2;
    int tmpRow = sheet->topRow(ev_PosY - 1, tmp2);
    if (sheet->rowFormat(tmpRow)->isHiddenOrFiltered() && tmpRow == 1)
        m_bResize = false;

    // So he clicked between two rows ?
    if (m_bResize) {
        // Determine row to resize
        double tmp;
        m_iResizedRow = sheet->topRow(ev_PosY - 1, tmp);
        if (!sheet->isProtected())
            paintSizeIndicator(_ev->pos().y());
    } else {
        m_bSelection = true;

        double tmp;
        int hit_row = sheet->topRow(ev_PosY, tmp);
        if (hit_row > KS_rowMax)
            return;

        m_iSelectionAnchor = hit_row;

        if (!m_pView->selection()->contains(QPoint(1, hit_row)) ||
                !(_ev->button() == Qt::RightButton) ||
                !m_pView->selection()->isRowSelected()) {
            QPoint newMarker(1, hit_row);
            QPoint newAnchor(KS_colMax, hit_row);
            if (_ev->modifiers() == Qt::ControlModifier) {
                m_pView->selection()->extend(QRect(newAnchor, newMarker));
            } else if (_ev->modifiers() == Qt::ShiftModifier) {
                m_pView->selection()->update(newMarker);
            } else {
                m_pView->selection()->initialize(QRect(newAnchor, newMarker));
            }
        }

        if (_ev->button() == Qt::RightButton) {
            QApplication::sendEvent(m_pCanvas, _ev);
        }
    }
}

void RowHeader::mouseReleaseEvent(QMouseEvent * _ev)
{
    if (!m_cellToolIsActive)
        return;
    m_pView->disableAutoScroll();
    if (m_lSize)
        m_lSize->hide();

    m_bMousePressed = false;

    if (!m_pView->koDocument()->isReadWrite())
        return;

    register Sheet * const sheet = m_pView->activeSheet();
    if (!sheet)
        return;

    double ev_PosY = m_pView->zoomHandler()->unzoomItY(_ev->pos().y()) + m_pCanvas->yOffset();

    if (m_bResize) {
        // Remove size indicator painted by paintSizeIndicator
        if (m_rubberband) {
            delete m_rubberband;
            m_rubberband = 0;
        }

        int start = m_iResizedRow;
        int end = m_iResizedRow;
        QRect rect;
        rect.setCoords(1, m_iResizedRow, KS_colMax, m_iResizedRow);
        if (m_pView->selection()->isRowSelected()) {
            if (m_pView->selection()->contains(QPoint(1, m_iResizedRow))) {
                start = m_pView->selection()->lastRange().top();
                end = m_pView->selection()->lastRange().bottom();
                rect = m_pView->selection()->lastRange();
            }
        }

        double height = 0.0;
        double y = sheet->rowPosition(m_iResizedRow);
        if (ev_PosY - y <= 0.0)
            height = 0.0;
        else
            height = ev_PosY - y;

        if (height != 0.0) {
            ResizeRowManipulator* command = new ResizeRowManipulator();
            command->setSheet(sheet);
            command->setSize(height);
            command->add(Region(rect, sheet));
            if (!command->execute())
                delete command;
        } else { // hide
            HideShowManipulator* command = new HideShowManipulator();
            command->setSheet(sheet);
            command->setManipulateRows(true);
            command->add(Region(rect, sheet));
            if (!command->execute())
                delete command;
        }
        delete m_lSize;
        m_lSize = 0;
    } else if (m_bSelection) {
        QRect rect = m_pView->selection()->lastRange();

        // TODO: please don't remove. Right now it's useless, but it's for a future feature
        // Norbert
        bool m_frozen = false;
        if (m_frozen) {
            kDebug(36001) << "selected: T" << rect.top() << " B" << rect.bottom();

            int i;
            QList<int> hiddenRows;

            for (i = rect.top(); i <= rect.bottom(); ++i) {
                if (sheet->rowFormat(i)->isHidden()) {
                    hiddenRows.append(i);
                }
            }

            if (hiddenRows.count() > 0) {
                if (m_pView->selection()->isColumnSelected()) {
                    KMessageBox::error(this, i18n("Area is too large."));
                    return;
                }

                HideShowManipulator* command = new HideShowManipulator();
                command->setSheet(sheet);
                command->setManipulateRows(true);
                command->setReverse(true);
                command->add(*m_pView->selection());
                command->execute();
            }
        }
    }

    m_bSelection = false;
    m_bResize = false;
}

void RowHeader::equalizeRow(double resize)
{
    if (resize != 0.0) {
        ResizeRowManipulator* command = new ResizeRowManipulator();
        command->setSheet(m_pView->activeSheet());
        command->setSize(qMax(2.0, resize));
        command->add(*m_pView->selection());
        if (!command->execute())
            delete command;
    } else { // hide
        HideShowManipulator* command = new HideShowManipulator();
        command->setSheet(m_pView->activeSheet());
        command->setManipulateRows(true);
        command->add(*m_pView->selection());
        if (!command->execute())
            delete command;
    }
}

void RowHeader::mouseDoubleClickEvent(QMouseEvent*)
{
    if (!m_cellToolIsActive)
        return;
    register Sheet * const sheet = m_pView->activeSheet();
    if (!sheet)
        return;

    if (!m_pView->koDocument()->isReadWrite() || sheet->isProtected())
        return;

    AdjustColumnRowManipulator* command = new AdjustColumnRowManipulator();
    command->setSheet(sheet);
    command->setAdjustRow(true);
    command->add(*m_pView->selection());
    command->execute();
}


void RowHeader::mouseMoveEvent(QMouseEvent * _ev)
{
    if (!m_cellToolIsActive) {
        setCursor(Qt::ArrowCursor);
        return;
    }
    if (!m_pView->koDocument()->isReadWrite())
        return;

    register Sheet * const sheet = m_pView->activeSheet();
    if (!sheet)
        return;

    double ev_PosY = m_pView->zoomHandler()->unzoomItY(_ev->pos().y()) + m_pCanvas->yOffset();
    double dHeight = m_pView->zoomHandler()->unzoomItY(height());

    // The button is pressed and we are resizing ?
    if (m_bResize) {
        if (!sheet->isProtected())
            paintSizeIndicator(_ev->pos().y());
    }
    // The button is pressed and we are selecting ?
    else if (m_bSelection) {
        double y;
        int row = sheet->topRow(ev_PosY, y);
        if (row > KS_rowMax)
            return;

        QPoint newAnchor = m_pView->selection()->anchor();
        QPoint newMarker = m_pView->selection()->marker();
        newMarker.setY(row);
        newAnchor.setY(m_iSelectionAnchor);
        m_pView->selection()->update(newMarker);

        if (_ev->pos().y() < 0)
            m_pCanvas->vertScrollBar()->setValue((int) ev_PosY);
        else if (_ev->pos().y() > m_pCanvas->height()) {
            if (row < KS_rowMax) {
                const RowFormat* rowFormat = sheet->rowFormat(row + 1);
                y = sheet->rowPosition(row + 1);
                m_pCanvas->vertScrollBar()->setValue((int)(ev_PosY + rowFormat->height() - dHeight));
            }
        }
    }
    // No button is pressed and the mouse is just moved
    else {

        //What is the internal size of 1 pixel
        const double unzoomedPixel = m_pView->zoomHandler()->unzoomItY(1.0);
        double y;
        int tmpRow = sheet->topRow(m_pCanvas->yOffset(), y);

        while (y < dHeight + m_pCanvas->yOffset() && tmpRow <= KS_rowMax) {
            double h = sheet->rowFormat(tmpRow)->visibleHeight();
            //if col is hide and it's the first column
            //you mustn't resize it.
            if (ev_PosY >= y + h - 2 * unzoomedPixel &&
                    ev_PosY <= y + h + unzoomedPixel &&
                    !(sheet->rowFormat(tmpRow)->isHiddenOrFiltered() && tmpRow == 1)) {
                setCursor(Qt::SplitVCursor);
                return;
            }
            y += h;
            tmpRow++;
        }
        setCursor(Qt::ArrowCursor);
    }
}

void RowHeader::slotAutoScroll(const QPoint& scrollDistance)
{
    // NOTE Stefan: This slot is triggered by the same signal as
    //              Canvas::slotAutoScroll and ColumnHeader::slotAutoScroll.
    //              Therefore, nothing has to be done except the scrolling was
    //              initiated in this header.
    if (!m_bMousePressed)
        return;
    if (scrollDistance.y() == 0)
        return;
    const QPoint offset = m_pCanvas->viewConverter()->documentToView(m_pCanvas->offset()).toPoint();
    if (offset.y() + scrollDistance.y() < 0)
        return;
    m_pCanvas->setDocumentOffset(offset + QPoint(0, scrollDistance.y()));
    QMouseEvent event(QEvent::MouseMove, mapFromGlobal(QCursor::pos()),
                      Qt::NoButton, Qt::NoButton, QApplication::keyboardModifiers());
    QApplication::sendEvent(this, &event);
    m_pCanvas->update();
}

void RowHeader::wheelEvent(QWheelEvent* _ev)
{
    QApplication::sendEvent(m_pCanvas, _ev);
}


void RowHeader::paintSizeIndicator(int mouseY)
{
    register Sheet * const sheet = m_pView->activeSheet();
    if (!sheet)
        return;

    m_iResizePos = mouseY;

    // Don't make the row have a height < 2 pixel.
    double y = m_pView->zoomHandler()->zoomItY(sheet->rowPosition(m_iResizedRow) - m_pCanvas->yOffset());
    if (m_iResizePos < y + 2)
        m_iResizePos = (int) y;

    if (!m_rubberband) {
        m_rubberband = new QRubberBand(QRubberBand::Line, m_pCanvas);
        m_rubberband->setGeometry(0, m_iResizePos, m_pCanvas->width(), 2);
        m_rubberband->show();
    }
    m_rubberband->move(0, m_iResizePos);

    QString tmpSize;
    double hh = m_pView->zoomHandler()->unzoomItY(m_iResizePos - y);
    double hu = m_pView->doc()->unit().toUserValue(hh);
    if (hu > 0.01)
        tmpSize = i18n("Height: %1 %2", hu, KoUnit::unitName(m_pView->doc()->unit()));
    else
        tmpSize = i18n("Hide Row");

    if (!m_lSize) {
        int screenNo = QApplication::desktop()->screenNumber(this);
        m_lSize = new QLabel(QApplication::desktop()->screen(screenNo) , Qt::ToolTip);
        m_lSize->setAlignment(Qt::AlignVCenter);
        m_lSize->setAutoFillBackground(true);
        m_lSize->setPalette(QToolTip::palette());
        m_lSize->setMargin(1 + style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, 0, m_lSize));
        m_lSize->setFrameShape(QFrame::Box);
        m_lSize->setIndent(1);
    }

    m_lSize->setText(tmpSize);
    m_lSize->adjustSize();
    QPoint pos = (sheet->layoutDirection() == Qt::RightToLeft) ? QPoint(m_pCanvas->width() - m_lSize->width() - 3, (int)y + 3) :
                 QPoint(3, (int)y + 3);
    pos -= QPoint(0, m_lSize->height());
    m_lSize->move(m_pCanvas->mapToGlobal(pos).x(), m_pCanvas->mapToGlobal(pos).y());
    m_lSize->show();
}

void RowHeader::updateRows(int from, int to)
{
    register Sheet * const sheet = m_pView->activeSheet();
    if (!sheet)
        return;

    double y0 = m_pView->zoomHandler()->zoomItY(sheet->rowPosition(from));
    double y1 = m_pView->zoomHandler()->zoomItY(sheet->rowPosition(to + 1));
    update(0, (int) y0, width(), (int)(y1 - y0));
}

void RowHeader::paintEvent(QPaintEvent* event)
{
    register Sheet * const sheet = m_pView->activeSheet();
    if (!sheet)
        return;

//     ElapsedTime et( "Painting vertical header", ElapsedTime::PrintOnlyTime );

    // FIXME Stefan: Make use of clipping. Find the repaint call after the scrolling.
    // kDebug(36004) << event->rect();

    // painting rectangle
    const QRectF paintRect = m_pView->zoomHandler()->viewToDocument(event->rect());

    // the painter
    QPainter painter(this);
    painter.scale(m_pView->zoomHandler()->zoomedResolutionX(), m_pView->zoomHandler()->zoomedResolutionY());
    painter.setRenderHint(QPainter::TextAntialiasing);

    // fonts
    QFont normalFont(painter.font());
    QFont boldFont(normalFont);
    boldFont.setBold(true);

    // background brush/color
    const QBrush backgroundBrush(palette().window());
    const QColor backgroundColor(backgroundBrush.color());

    // selection brush/color
    QColor selectionColor(palette().highlight().color());
    selectionColor.setAlpha(127);
    const QBrush selectionBrush(selectionColor);

    painter.setClipRect(paintRect);

    double yPos;
    // Get the top row and the current y-position
    int y = sheet->topRow(paintRect.y() + m_pCanvas->yOffset(), yPos);
    // Align to the offset
    yPos = yPos - m_pCanvas->yOffset();

    const KoViewConverter *converter = m_pCanvas->viewConverter();
    const double width = converter->viewToDocumentX(this->width() - 1);

    QSet<int> selectedRows;
    QSet<int> affectedRows;
    if (!m_pView->selection()->referenceSelectionMode() && m_cellToolIsActive) {
        selectedRows = m_pView->selection()->rowsSelected();
        affectedRows = m_pView->selection()->rowsAffected();
    }
    // Loop through the rows, until we are out of range
    while (yPos <= paintRect.bottom() && y <= KS_rowMax) {
        const bool selected = (selectedRows.contains(y));
        const bool highlighted = (!selected && affectedRows.contains(y));

        const RowFormat* rowFormat = sheet->rowFormat(y);
        if (rowFormat->isHiddenOrFiltered()) {
            ++y;
            continue;
        }
        const double height = rowFormat->height();

        if (selected || highlighted) {
            painter.setPen(selectionColor.dark(150));
            painter.setBrush(selectionBrush);
        } else {
            painter.setPen(backgroundColor.dark(150));
            painter.setBrush(backgroundBrush);
        }
        painter.drawRect(QRectF(0, yPos, width, height));

        QString rowText = QString::number(y);

        // Reset painter
        painter.setFont(normalFont);
        painter.setPen(palette().text().color());

        if (selected)
            painter.setPen(palette().highlightedText().color());
        else if (highlighted)
            painter.setFont(boldFont);

        double len = painter.fontMetrics().width(rowText);
        if (!rowFormat->isHiddenOrFiltered()) {
#if 0
            switch (y % 3) {
            case 0: rowText = QString::number(height) + 'h'; break;
            case 1: rowText = QString::number(painter.fontMetrics().ascent()) + 'a'; break;
            case 2: rowText = QString::number(painter.fontMetrics().descent()) + 'd'; break;
            }
            //kDebug() << "font height: " << painter.fontMetrics().ascent();
            painter.drawLine(1, yPos, 4, yPos + 3);
#endif
            drawText(painter,
                     normalFont,
                     QPointF((width - len) / 2,
                             yPos + (height - painter.fontMetrics().ascent()) / 2),
//                            yPos + ( height - painter.fontMetrics().ascent() - painter.fontMetrics().descent() ) / 2 ),

                     rowText);
        }

        yPos += rowFormat->height();
        y++;
    }
}


void RowHeader::focusOutEvent(QFocusEvent*)
{
    m_pView->disableAutoScroll();
    m_bMousePressed = false;
}

void RowHeader::drawText(QPainter& painter, const QFont& font,
                         const QPointF& location, const QString& text) const
{
    register Sheet * const sheet = m_pView->activeSheet();
    if (!sheet)
        return;

    const double scaleX = POINT_TO_INCH(double(KoDpi::dpiX()));
    const double scaleY = POINT_TO_INCH(double(KoDpi::dpiY()));

    // Qt scales the font already with the logical resolution. Do not do it twice!
    painter.save();
    painter.scale(1.0 / scaleX, 1.0 / scaleY);

    QTextLayout textLayout(text, font);
    textLayout.beginLayout();
    forever {
        QTextLine line = textLayout.createLine();
        if (!line.isValid())
            break;
        line.setLineWidth(width() * scaleX);
    }
    textLayout.endLayout();
    QPointF loc(location.x() * scaleX, location.y() * scaleY);
    textLayout.draw(&painter, loc);

    painter.restore();
}

void RowHeader::toolChanged(const QString& toolId)
{
    m_cellToolIsActive = toolId.startsWith("KSpread");
    update();
}


/****************************************************************
 *
 * ColumnHeader
 *
 ****************************************************************/

ColumnHeader::ColumnHeader(QWidget *_parent, Canvas *_canvas, View *_view)
        : QWidget(_parent)
{
    m_pView = _view;
    m_pCanvas = _canvas;
    m_lSize = 0;
    m_rubberband = 0;
    m_cellToolIsActive = true;

    setAttribute(Qt::WA_StaticContents);

    setMouseTracking(true);
    m_bResize = false;
    m_bSelection = false;
    m_iSelectionAnchor = 1;
    m_bMousePressed = false;

    connect(m_pView, SIGNAL(autoScroll(const QPoint &)),
            this, SLOT(slotAutoScroll(const QPoint &)));
    connect(m_pCanvas->toolProxy(), SIGNAL(toolChanged(const QString&)),
            this, SLOT(toolChanged(const QString&)));
}


ColumnHeader::~ColumnHeader()
{
}

void ColumnHeader::mousePressEvent(QMouseEvent * _ev)
{
    if (!m_cellToolIsActive)
        return;
    if (!m_pView->koDocument()->isReadWrite())
        return;

    if (_ev->button() == Qt::LeftButton) {
        m_bMousePressed = true;
        m_pView->enableAutoScroll();
    }

    const register Sheet * const sheet = m_pView->activeSheet();
    if (!sheet)
        return;

    // We were editing a cell -> save value and get out of editing mode
    m_pView->selection()->emitCloseEditor(true); // save changes

    double ev_PosX;
    double dWidth = m_pView->zoomHandler()->unzoomItX(width());
    if (sheet->layoutDirection() == Qt::RightToLeft)
        ev_PosX = dWidth - m_pView->zoomHandler()->unzoomItX(_ev->pos().x()) + m_pCanvas->xOffset();
    else
        ev_PosX = m_pView->zoomHandler()->unzoomItX(_ev->pos().x()) + m_pCanvas->xOffset();
    m_bResize = false;
    m_bSelection = false;

    // Find the first visible column and the x position of this column.
    double x;

    const double unzoomedPixel = m_pView->zoomHandler()->unzoomItX(1.0);
    if (sheet->layoutDirection() == Qt::RightToLeft) {
        int tmpCol = sheet->leftColumn(m_pCanvas->xOffset(), x);

        kDebug() << "evPos:" << ev_PosX << ", x:" << x << ", COL:" << tmpCol;
        while (ev_PosX > x && (!m_bResize) && tmpCol <= KS_colMax) {
            double w = sheet->columnFormat(tmpCol)->width();

            kDebug() << "evPos:" << ev_PosX << ", x:" << x << ", w:" << w << ", COL:" << tmpCol;

            ++tmpCol;
            if (tmpCol > KS_colMax)
                tmpCol = KS_colMax;
            //if col is hide and it's the first column
            //you mustn't resize it.

            if (ev_PosX >= x + w - unzoomedPixel &&
                    ev_PosX <= x + w + unzoomedPixel &&
                    !(sheet->columnFormat(tmpCol)->isHiddenOrFiltered() && tmpCol == 1)) {
                m_bResize = true;
            }
            x += w;
        }

        //if col is hide and it's the first column
        //you mustn't resize it.
        double tmp2;
        tmpCol = sheet->leftColumn(dWidth - ev_PosX + 1, tmp2);
        if (sheet->columnFormat(tmpCol)->isHiddenOrFiltered() && tmpCol == 0) {
            kDebug() << "No resize:" << tmpCol << "," << sheet->columnFormat(tmpCol)->isHiddenOrFiltered();
            m_bResize = false;
        }

        kDebug() << "Resize:" << m_bResize;
    } else {
        int col = sheet->leftColumn(m_pCanvas->xOffset(), x);

        // Did the user click between two columns?
        while (x < (dWidth + m_pCanvas->xOffset()) && (!m_bResize) && col <= KS_colMax) {
            double w = sheet->columnFormat(col)->width();
            col++;
            if (col > KS_colMax)
                col = KS_colMax;
            if ((ev_PosX >= x + w - unzoomedPixel) &&
                    (ev_PosX <= x + w + unzoomedPixel) &&
                    !(sheet->columnFormat(col)->isHiddenOrFiltered() && col == 1))
                m_bResize = true;
            x += w;
        }

        //if col is hide and it's the first column
        //you mustn't resize it.
        double tmp2;
        int tmpCol = sheet->leftColumn(ev_PosX - 1, tmp2);
        if (sheet->columnFormat(tmpCol)->isHiddenOrFiltered() && tmpCol == 1)
            m_bResize = false;
    }

    // So he clicked between two rows ?
    if (m_bResize) {
        // Determine the column to resize
        double tmp;
        if (sheet->layoutDirection() == Qt::RightToLeft) {
            m_iResizedColumn = sheet->leftColumn(ev_PosX - 1, tmp);
            // kDebug() <<"RColumn:" << m_iResizedColumn <<", PosX:" << ev_PosX;

            if (!sheet->isProtected())
                paintSizeIndicator(_ev->pos().x());
        } else {
            m_iResizedColumn = sheet->leftColumn(ev_PosX - 1, tmp);

            if (!sheet->isProtected())
                paintSizeIndicator(_ev->pos().x());
        }

        // kDebug() <<"Column:" << m_iResizedColumn;
    } else {
        m_bSelection = true;

        double tmp;
        int hit_col = sheet->leftColumn(ev_PosX, tmp);
        if (hit_col > KS_colMax)
            return;

        m_iSelectionAnchor = hit_col;

        if (!m_pView->selection()->contains(QPoint(hit_col, 1)) ||
                !(_ev->button() == Qt::RightButton) ||
                !m_pView->selection()->isColumnSelected()) {
            QPoint newMarker(hit_col, 1);
            QPoint newAnchor(hit_col, KS_rowMax);
            if (_ev->modifiers() == Qt::ControlModifier) {
                m_pView->selection()->extend(QRect(newAnchor, newMarker));
            } else if (_ev->modifiers() == Qt::ShiftModifier) {
                m_pView->selection()->update(newMarker);
            } else {
                m_pView->selection()->initialize(QRect(newAnchor, newMarker));
            }
        }

        if (_ev->button() == Qt::RightButton) {
            QApplication::sendEvent(m_pCanvas, _ev);
        }
    }
}

void ColumnHeader::mouseReleaseEvent(QMouseEvent * _ev)
{
    if (!m_cellToolIsActive)
        return;
    m_pView->disableAutoScroll();
    if (m_lSize)
        m_lSize->hide();

    m_bMousePressed = false;

    if (!m_pView->koDocument()->isReadWrite())
        return;

    register Sheet * const sheet = m_pView->activeSheet();
    if (!sheet)
        return;

    if (m_bResize) {
        double dWidth = m_pView->zoomHandler()->unzoomItX(width());
        double ev_PosX;

        // Remove size indicator painted by paintSizeIndicator
        if (m_rubberband) {
            delete m_rubberband;
            m_rubberband = 0;
        }

        int start = m_iResizedColumn;
        int end   = m_iResizedColumn;
        QRect rect;
        rect.setCoords(m_iResizedColumn, 1, m_iResizedColumn, KS_rowMax);
        if (m_pView->selection()->isColumnSelected()) {
            if (m_pView->selection()->contains(QPoint(m_iResizedColumn, 1))) {
                start = m_pView->selection()->lastRange().left();
                end   = m_pView->selection()->lastRange().right();
                rect  = m_pView->selection()->lastRange();
            }
        }

        double width = 0.0;
        double x;

        if (sheet->layoutDirection() == Qt::RightToLeft)
            ev_PosX = dWidth - m_pView->zoomHandler()->unzoomItX(_ev->pos().x()) + m_pCanvas->xOffset();
        else
            ev_PosX = m_pView->zoomHandler()->unzoomItX(_ev->pos().x()) + m_pCanvas->xOffset();

        x = sheet->columnPosition(m_iResizedColumn);

        if (ev_PosX - x <= 0.0)
            width = 0.0;
        else
            width = ev_PosX - x;

        if (width != 0.0) {
            ResizeColumnManipulator* command = new ResizeColumnManipulator();
            command->setSheet(sheet);
            command->setSize(width);
            command->add(Region(rect, sheet));
            if (!command->execute())
                delete command;
        } else { // hide
            HideShowManipulator* command = new HideShowManipulator();
            command->setSheet(sheet);
            command->setManipulateColumns(true);
            command->add(Region(rect, sheet));
            if (!command->execute())
                delete command;
        }
        delete m_lSize;
        m_lSize = 0;
    } else if (m_bSelection) {
        QRect rect = m_pView->selection()->lastRange();

        // TODO: please don't remove. Right now it's useless, but it's for a future feature
        // Norbert
        bool m_frozen = false;
        if (m_frozen) {
            kDebug(36001) << "selected: L" << rect.left() << " R" << rect.right();

            int i;
            QList<int> hiddenCols;

            for (i = rect.left(); i <= rect.right(); ++i) {
                if (sheet->columnFormat(i)->isHidden()) {
                    hiddenCols.append(i);
                }
            }

            if (hiddenCols.count() > 0) {
                if (m_pView->selection()->isRowSelected()) {
                    KMessageBox::error(this, i18n("Area is too large."));
                    return;
                }

                HideShowManipulator* command = new HideShowManipulator();
                command->setSheet(sheet);
                command->setManipulateColumns(true);
                command->setReverse(true);
                command->add(*m_pView->selection());
                command->execute();
            }
        }
    }

    m_bSelection = false;
    m_bResize = false;
}

void ColumnHeader::equalizeColumn(double resize)
{
    if (resize != 0.0) {
        ResizeColumnManipulator* command = new ResizeColumnManipulator();
        command->setSheet(m_pView->activeSheet());
        command->setSize(qMax(2.0, resize));
        command->add(*m_pView->selection());
        if (!command->execute())
            delete command;
    } else { // hide
        HideShowManipulator* command = new HideShowManipulator();
        command->setSheet(m_pView->activeSheet());
        command->setManipulateColumns(true);
        command->add(*m_pView->selection());
        if (!command->execute())
            delete command;
    }
}

void ColumnHeader::mouseDoubleClickEvent(QMouseEvent*)
{
    if (!m_cellToolIsActive)
        return;
    register Sheet * const sheet = m_pView->activeSheet();
    if (!sheet)
        return;

    if (!m_pView->koDocument()->isReadWrite() || sheet->isProtected())
        return;

    AdjustColumnRowManipulator* command = new AdjustColumnRowManipulator();
    command->setSheet(sheet);
    command->setAdjustColumn(true);
    command->add(*m_pView->selection());
    command->execute();
}

void ColumnHeader::mouseMoveEvent(QMouseEvent * _ev)
{
    if (!m_cellToolIsActive)
        return;
    if (!m_pView->koDocument()->isReadWrite())
        return;

    register Sheet * const sheet = m_pView->activeSheet();

    if (!sheet)
        return;

    double dWidth = m_pView->zoomHandler()->unzoomItX(width());
    double ev_PosX;
    if (sheet->layoutDirection() == Qt::RightToLeft)
        ev_PosX = dWidth - m_pView->zoomHandler()->unzoomItX(_ev->pos().x()) + m_pCanvas->xOffset();
    else
        ev_PosX = m_pView->zoomHandler()->unzoomItX(_ev->pos().x()) + m_pCanvas->xOffset();

    // The button is pressed and we are resizing ?
    if (m_bResize) {
        if (!sheet->isProtected())
            paintSizeIndicator(_ev->pos().x());
    }
    // The button is pressed and we are selecting ?
    else if (m_bSelection) {
        double x;
        int col = sheet->leftColumn(ev_PosX, x);

        if (col > KS_colMax)
            return;

        QPoint newMarker = m_pView->selection()->marker();
        QPoint newAnchor = m_pView->selection()->anchor();
        newMarker.setX(col);
        newAnchor.setX(m_iSelectionAnchor);
        m_pView->selection()->update(newMarker);

        if (sheet->layoutDirection() == Qt::RightToLeft) {
            if (_ev->pos().x() < width() - m_pCanvas->width()) {
                const ColumnFormat *cl = sheet->columnFormat(col + 1);
                x = sheet->columnPosition(col + 1);
                m_pCanvas->horzScrollBar()->setValue(m_pCanvas->horzScrollBar()->maximum()
                                                     - (int)((ev_PosX + cl->width()) - dWidth));
            } else if (_ev->pos().x() > width())
                m_pCanvas->horzScrollBar()->setValue((int)(m_pCanvas->horzScrollBar()->maximum() - (ev_PosX - dWidth + m_pView->zoomHandler()->unzoomItX(m_pCanvas->width()))));
        } else {
            if (_ev->pos().x() < 0)
                m_pCanvas->horzScrollBar()->setValue((int) ev_PosX);
            else if (_ev->pos().x() > m_pCanvas->width()) {
                if (col < KS_colMax) {
                    const ColumnFormat *cl = sheet->columnFormat(col + 1);
                    x = sheet->columnPosition(col + 1);
                    m_pCanvas->horzScrollBar()->setValue((int)(ev_PosX + cl->width() - dWidth));
                }
            }
        }

    }
    // No button is pressed and the mouse is just moved
    else {
        //What is the internal size of 1 pixel
        const double unzoomedPixel = m_pView->zoomHandler()->unzoomItX(1.0);
        double x;

        if (sheet->layoutDirection() == Qt::RightToLeft) {
            int tmpCol = sheet->leftColumn(m_pCanvas->xOffset(), x);

            while (ev_PosX > x && tmpCol <= KS_colMax) {
                double w = sheet->columnFormat(tmpCol)->visibleWidth();
                ++tmpCol;

                //if col is hide and it's the first column
                //you mustn't resize it.
                if (ev_PosX >= x + w - unzoomedPixel &&
                        ev_PosX <= x + w + unzoomedPixel &&
                        !(sheet->columnFormat(tmpCol)->isHiddenOrFiltered() && tmpCol == 0)) {
                    setCursor(Qt::SplitHCursor);
                    return;
                }
                x += w;
            }
            setCursor(Qt::ArrowCursor);
        } else {
            int tmpCol = sheet->leftColumn(m_pCanvas->xOffset(), x);

            while (x < m_pView->zoomHandler()->unzoomItY(width()) + m_pCanvas->xOffset() && tmpCol <= KS_colMax) {
                double w = sheet->columnFormat(tmpCol)->visibleWidth();
                //if col is hide and it's the first column
                //you mustn't resize it.
                if (ev_PosX >= x + w - unzoomedPixel &&
                        ev_PosX <= x + w + unzoomedPixel &&
                        !(sheet->columnFormat(tmpCol)->isHiddenOrFiltered() && tmpCol == 1)) {
                    setCursor(Qt::SplitHCursor);
                    return;
                }
                x += w;
                tmpCol++;
            }
            setCursor(Qt::ArrowCursor);
        }
    }
}

void ColumnHeader::slotAutoScroll(const QPoint& scrollDistance)
{
    // NOTE Stefan: This slot is triggered by the same signal as
    //              Canvas::slotAutoScroll and RowHeader::slotAutoScroll.
    //              Therefore, nothing has to be done except the scrolling was
    //              initiated in this header.
    if (!m_bMousePressed)
        return;
    if (scrollDistance.x() == 0)
        return;
    const QPoint offset = m_pCanvas->viewConverter()->documentToView(m_pCanvas->offset()).toPoint();
    if (offset.x() + scrollDistance.x() < 0)
        return;
    m_pCanvas->setDocumentOffset(offset + QPoint(scrollDistance.x(), 0));
    QMouseEvent event(QEvent::MouseMove, mapFromGlobal(QCursor::pos()),
                      Qt::NoButton, Qt::NoButton, QApplication::keyboardModifiers());
    QApplication::sendEvent(this, &event);
    m_pCanvas->update();
}

void ColumnHeader::wheelEvent(QWheelEvent* _ev)
{
    QApplication::sendEvent(m_pCanvas, _ev);
}

void ColumnHeader::resizeEvent(QResizeEvent* _ev)
{
    register Sheet * const sheet = m_pView->activeSheet();
    if (!sheet)
        return;

    // workaround to allow horizontal resizing and zoom changing when sheet
    // direction and interface direction don't match (e.g. an RTL sheet on an
    // LTR interface)
    if (sheet->layoutDirection() == Qt::RightToLeft && !QApplication::isRightToLeft()) {
        int dx = _ev->size().width() - _ev->oldSize().width();
        scroll(dx, 0);
    } else if (sheet->layoutDirection() == Qt::LeftToRight && QApplication::isRightToLeft()) {
        int dx = _ev->size().width() - _ev->oldSize().width();
        scroll(-dx, 0);
    }
}

void ColumnHeader::paintSizeIndicator(int mouseX)
{
    register Sheet * const sheet = m_pView->activeSheet();
    if (!sheet)
        return;

    if (sheet->layoutDirection() == Qt::RightToLeft)
        m_iResizePos = mouseX + m_pCanvas->width() - width();
    else
        m_iResizePos = mouseX;

    // Don't make the column have a width < 2 pixels.
    double x = m_pView->zoomHandler()->zoomItX(sheet->columnPosition(m_iResizedColumn) - m_pCanvas->xOffset());

    if (sheet->layoutDirection() == Qt::RightToLeft) {
        x = m_pCanvas->width() - x;

        if (m_iResizePos > x - 2)
            m_iResizePos = (int) x;
    } else {
        if (m_iResizePos < x + 2)
            m_iResizePos = (int) x;
    }

    if (!m_rubberband) {
        m_rubberband = new QRubberBand(QRubberBand::Line, m_pCanvas);
        m_rubberband->setGeometry(m_iResizePos, 0, 2, m_pCanvas->height());
        m_rubberband->show();
    }
    m_rubberband->move(m_iResizePos, 0);

    QString tmpSize;
    double ww = m_pView->zoomHandler()->unzoomItX((sheet->layoutDirection() == Qt::RightToLeft) ? x - m_iResizePos : m_iResizePos - x);
    double wu = m_pView->doc()->unit().toUserValue(ww);
    if (wu > 0.01)
        tmpSize = i18n("Width: %1 %2", wu, KoUnit::unitName(m_pView->doc()->unit()));
    else
        tmpSize = i18n("Hide Column");

    if (!m_lSize) {
        int screenNo = QApplication::desktop()->screenNumber(this);
        m_lSize = new QLabel(QApplication::desktop()->screen(screenNo) , Qt::ToolTip);
        m_lSize->setAlignment(Qt::AlignVCenter);
        m_lSize->setAutoFillBackground(true);
        m_lSize->setPalette(QToolTip::palette());
        m_lSize->setMargin(1 + style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, 0, m_lSize));
        m_lSize->setFrameShape(QFrame::Box);
        m_lSize->setIndent(1);
    }

    m_lSize->setText(tmpSize);
    m_lSize->adjustSize();
    QPoint pos = (sheet->layoutDirection() == Qt::RightToLeft) ? QPoint((int) x - 3 - m_lSize->width(), 3) :
                 QPoint((int) x + 3, 3);
    pos -= QPoint(0, m_lSize->height());
    m_lSize->move(m_pCanvas->mapToGlobal(pos).x(), mapToGlobal(pos).y());
    m_lSize->show();
}

void ColumnHeader::updateColumns(int from, int to)
{
    register Sheet * const sheet = m_pView->activeSheet();
    if (!sheet)
        return;

    double x0 = m_pView->zoomHandler()->zoomItX(sheet->columnPosition(from));
    double x1 = m_pView->zoomHandler()->zoomItX(sheet->columnPosition(to + 1));
    update((int) x0, 0, (int)(x1 - x0), height());
}

void ColumnHeader::paintEvent(QPaintEvent* event)
{
    register Sheet * const sheet = m_pView->activeSheet();
    if (!sheet)
        return;

//     ElapsedTime et( "Painting horizontal header", ElapsedTime::PrintOnlyTime );

    // FIXME Stefan: Make use of clipping. Find the repaint call after the scrolling.
    // kDebug(36004) << event->rect();

    // painting rectangle
    const QRectF paintRect = m_pView->zoomHandler()->viewToDocument(event->rect());

    // the painter
    QPainter painter(this);
    painter.scale(m_pView->zoomHandler()->zoomedResolutionX(), m_pView->zoomHandler()->zoomedResolutionY());
    painter.setRenderHint(QPainter::TextAntialiasing);

    // fonts
    QFont normalFont(painter.font());
    QFont boldFont(normalFont);
    boldFont.setBold(true);

    // background brush/color
    const QBrush backgroundBrush(palette().window());
    const QColor backgroundColor(backgroundBrush.color());

    // selection brush/color
    QColor selectionColor(palette().highlight().color());
    selectionColor.setAlpha(127);
    const QBrush selectionBrush(selectionColor);

    painter.setClipRect(paintRect);

    double xPos;
    int x;

    if (sheet->layoutDirection() == Qt::RightToLeft) {
        //Get the left column and the current x-position
        x = sheet->leftColumn(int(m_pView->zoomHandler()->unzoomItX(width()) - paintRect.x() + m_pCanvas->xOffset()), xPos);
        //Align to the offset
        xPos = m_pView->zoomHandler()->unzoomItX(width()) - xPos + m_pCanvas->xOffset();
    } else {
        //Get the left column and the current x-position
        x = sheet->leftColumn(int(paintRect.x() + m_pCanvas->xOffset()), xPos);
        //Align to the offset
        xPos = xPos - m_pCanvas->xOffset();
    }

    const KoViewConverter *converter = m_pCanvas->viewConverter();
    const double height = converter->viewToDocumentY(this->height() - 1);

    if (sheet->layoutDirection() == Qt::RightToLeft) {
        if (x > KS_colMax)
            x = KS_colMax;

        xPos -= sheet->columnFormat(x)->width();

        QSet<int> selectedColumns;
        QSet<int> affectedColumns;
        if (!m_pView->selection()->referenceSelectionMode() && m_cellToolIsActive) {
            selectedColumns = m_pView->selection()->columnsSelected();
            affectedColumns = m_pView->selection()->columnsAffected();
        }
        //Loop through the columns, until we are out of range
        while (xPos <= paintRect.right() && x <= KS_colMax) {
            bool selected = (selectedColumns.contains(x));
            bool highlighted = (!selected && affectedColumns.contains(x));

            const ColumnFormat* columnFormat = sheet->columnFormat(x);
            if (columnFormat->isHiddenOrFiltered()) {
                ++x;
                continue;
            }
            double width = columnFormat->width();

            if (selected || highlighted) {
                painter.setPen(selectionColor.dark(150));
                painter.setBrush(selectionBrush);
            } else {
                painter.setPen(backgroundColor.dark(150));
                painter.setBrush(backgroundBrush);
            }
            painter.drawRect(QRectF(xPos, 0, width, height));

            // Reset painter
            painter.setFont(normalFont);
            painter.setPen(palette().text().color());

            if (selected)
                painter.setPen(palette().highlightedText().color());
            else if (highlighted)
                painter.setFont(boldFont);

            QString colText = sheet->getShowColumnNumber() ? QString::number(x) : Cell::columnName(x);
            double len = painter.fontMetrics().width(colText);
            if (!columnFormat->isHiddenOrFiltered())
                drawText(painter,
                         normalFont,
                         QPointF(xPos + (width - len) / 2,
                                 (height - painter.fontMetrics().ascent() - painter.fontMetrics().descent()) / 2),
                         colText,
                         width);

            xPos += columnFormat->width();
            --x;
        }
    } else { // if ( sheet->layoutDirection() == Qt::LeftToRight )
        QSet<int> selectedColumns;
        QSet<int> affectedColumns;
        if (!m_pView->selection()->referenceSelectionMode() && m_cellToolIsActive) {
            selectedColumns = m_pView->selection()->columnsSelected();
            affectedColumns = m_pView->selection()->columnsAffected();
        }
        //Loop through the columns, until we are out of range
        while (xPos <= paintRect.right() && x <= KS_colMax) {
            bool selected = (selectedColumns.contains(x));
            bool highlighted = (!selected && affectedColumns.contains(x));

            const ColumnFormat *columnFormat = sheet->columnFormat(x);
            if (columnFormat->isHiddenOrFiltered()) {
                ++x;
                continue;
            }
            double width = columnFormat->width();

            QColor backgroundColor = palette().window().color();

            if (selected || highlighted) {
                painter.setPen(selectionColor.dark(150));
                painter.setBrush(selectionBrush);
            } else {
                painter.setPen(backgroundColor.dark(150));
                painter.setBrush(backgroundBrush);
            }
            painter.drawRect(QRectF(xPos, 0, width, height));

            // Reset painter
            painter.setFont(normalFont);
            painter.setPen(palette().text().color());

            if (selected)
                painter.setPen(palette().highlightedText().color());
            else if (highlighted)
                painter.setFont(boldFont);

            QString colText = sheet->getShowColumnNumber() ? QString::number(x) : Cell::columnName(x);
            int len = painter.fontMetrics().width(colText);
            if (!columnFormat->isHiddenOrFiltered()) {
#if 0
                switch (x % 3) {
                case 0: colText = QString::number(height) + 'h'; break;
                case 1: colText = QString::number(painter.fontMetrics().ascent()) + 'a'; break;
                case 2: colText = QString::number(painter.fontMetrics().descent()) + 'd'; break;
                }
#endif
                drawText(painter,
                         normalFont,
                         QPointF(xPos + (width - len) / 2,
                                 (height - painter.fontMetrics().ascent() - painter.fontMetrics().descent()) / 2),
                         colText,
                         width);
            }

            xPos += columnFormat->width();
            ++x;
        }
    }
}


void ColumnHeader::focusOutEvent(QFocusEvent*)
{
    m_pView->disableAutoScroll();
    m_bMousePressed = false;
}

void ColumnHeader::drawText(QPainter& painter, const QFont& font,
                            const QPointF& location, const QString& text,
                            double width) const
{
    register Sheet * const sheet = m_pView->activeSheet();
    if (!sheet)
        return;

    const double scaleX = POINT_TO_INCH(double(KoDpi::dpiX()));
    const double scaleY = POINT_TO_INCH(double(KoDpi::dpiY()));

    // Qt scales the font already with the logical resolution. Do not do it twice!
    painter.save();
    painter.scale(1.0 / scaleX, 1.0 / scaleY);

    QTextLayout textLayout(text, font);
    textLayout.beginLayout();
    forever {
        QTextLine line = textLayout.createLine();
        if (!line.isValid())
            break;
        line.setLineWidth(width * scaleX);
    }
    textLayout.endLayout();
    QPointF loc(location.x() * scaleX, location.y() * scaleY);
    textLayout.draw(&painter, loc);

    painter.restore();
}

void ColumnHeader::toolChanged(const QString& toolId)
{
    m_cellToolIsActive = toolId.startsWith("KSpread");
    update();
}


/****************************************************************
 *
 * SelectAllButton
 *
 ****************************************************************/

SelectAllButton::SelectAllButton(KoCanvasBase* canvasBase, Selection* selection)
        : QWidget(canvasBase->canvasWidget())
        , m_canvasBase(canvasBase)
        , m_selection(selection)
        , m_mousePressed(false)
{
    m_cellToolIsActive = true;
    connect(m_canvasBase->toolProxy(), SIGNAL(toolChanged(const QString&)),
            this, SLOT(toolChanged(const QString&)));
}

SelectAllButton::~SelectAllButton()
{
}

void SelectAllButton::paintEvent(QPaintEvent* event)
{
    // the painter
    QPainter painter(this);
    painter.setClipRect(event->rect());

    // if all cells are selected
    if (m_selection->isAllSelected() &&
            !m_selection->referenceSelectionMode() && m_cellToolIsActive) {
        // selection brush/color
        QColor selectionColor(palette().highlight().color());
        selectionColor.setAlpha(127);
        const QBrush selectionBrush(selectionColor);

        painter.setPen(selectionColor.dark(150));
        painter.setBrush(selectionBrush);
    } else {
        // background brush/color
        const QBrush backgroundBrush(palette().window());
        const QColor backgroundColor(backgroundBrush.color());

        painter.setPen(backgroundColor.dark(150));
        painter.setBrush(backgroundBrush);
    }
    painter.drawRect(rect().adjusted(0, 0, -1, -1));
}

void SelectAllButton::mousePressEvent(QMouseEvent* event)
{
    if (!m_cellToolIsActive)
        return;
    if (event->button() == Qt::LeftButton)
        m_mousePressed = true;
}

void SelectAllButton::mouseReleaseEvent(QMouseEvent* event)
{
    if (!m_cellToolIsActive)
        return;
    Q_UNUSED(event);
    if (!m_mousePressed)
        return;
    m_mousePressed = false;
    m_selection->selectAll();
}

void SelectAllButton::wheelEvent(QWheelEvent* event)
{
    QApplication::sendEvent(m_canvasBase->canvasWidget(), event);
}

void SelectAllButton::toolChanged(const QString& toolId)
{
    m_cellToolIsActive = toolId.startsWith("KSpread");
    update();
}

#include "Headers.moc"
