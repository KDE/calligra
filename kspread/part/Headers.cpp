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
#include <QLabel>
#include <QPainter>
#include <QTextLayout>
#include <QToolTip>

// KDE
#include <klocale.h>
#include <kmessagebox.h>
#include <kwordwrap.h>

// KOffice
#include <KoCanvasController.h>
#include <KoToolProxy.h>
#include <KoZoomHandler.h>
#include <KoDpi.h>
#include <KoPointerEvent.h>

// KSpread
#include "CanvasBase.h"
#include "Cell.h"
#include "Doc.h"
#include "kspread_limits.h"
#include "RowColumnFormat.h"
#include "Sheet.h"
#include "Selection.h"

// commands
#include "commands/RowColumnManipulators.h"

using namespace KSpread;

/****************************************************************
 *
 * RowHeader
 *
 ****************************************************************/

RowHeader::RowHeader(CanvasBase *_canvas)
        : m_pCanvas(_canvas), m_bSelection(false),
        m_iSelectionAnchor(1), m_bResize(false), m_lSize(0), m_bMousePressed(false),
        m_cellToolIsActive(true)
{
}


RowHeader::~RowHeader()
{
}

void RowHeader::mousePress(KoPointerEvent * _ev)
{
    if (!m_cellToolIsActive)
        return;
    if (!m_pCanvas->doc()->isReadWrite())
        return;

    register Sheet * const sheet = m_pCanvas->activeSheet();
    if (!sheet)
        return;

    if (_ev->button() == Qt::LeftButton) {
        m_bMousePressed = true;
        m_pCanvas->enableAutoScroll();
    }

    double ev_PosY = m_pCanvas->zoomHandler()->unzoomItY(_ev->pos().y()) + m_pCanvas->yOffset();
    double dHeight = m_pCanvas->zoomHandler()->unzoomItY(height());
    m_bResize = false;
    m_bSelection = false;

    // We were editing a cell -> save value and get out of editing mode
    m_pCanvas->selection()->emitCloseEditor(true); // save changes

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
        if (_ev->button() != Qt::RightButton) {
            m_bSelection = true;
        }

        double tmp;
        int hit_row = sheet->topRow(ev_PosY, tmp);
        if (hit_row > KS_rowMax)
            return;

        m_iSelectionAnchor = hit_row;

        if (!m_pCanvas->selection()->contains(QPoint(1, hit_row)) ||
                !(_ev->button() == Qt::RightButton) ||
                !m_pCanvas->selection()->isRowSelected()) {
            QPoint newMarker(1, hit_row);
            QPoint newAnchor(KS_colMax, hit_row);
            if (_ev->modifiers() == Qt::ControlModifier) {
                m_pCanvas->selection()->extend(QRect(newAnchor, newMarker));
            } else if (_ev->modifiers() == Qt::ShiftModifier) {
                m_pCanvas->selection()->update(newMarker);
            } else {
                m_pCanvas->selection()->initialize(QRect(newAnchor, newMarker));
            }
        }

        if (_ev->button() == Qt::RightButton) {
            m_pCanvas->mousePressed(_ev);
        }
    }
}

void RowHeader::mouseRelease(KoPointerEvent * _ev)
{
    if (!m_cellToolIsActive)
        return;
    m_pCanvas->disableAutoScroll();
    if (m_lSize)
        m_lSize->hide();

    m_bMousePressed = false;

    if (!m_pCanvas->doc()->isReadWrite())
        return;

    register Sheet * const sheet = m_pCanvas->activeSheet();
    if (!sheet)
        return;

    double ev_PosY = m_pCanvas->zoomHandler()->unzoomItY(_ev->pos().y()) + m_pCanvas->yOffset();

    if (m_bResize) {
        // Remove size indicator painted by paintSizeIndicator
        removeSizeIndicator();

        int start = m_iResizedRow;
        int end = m_iResizedRow;
        QRect rect;
        rect.setCoords(1, m_iResizedRow, KS_colMax, m_iResizedRow);
        if (m_pCanvas->selection()->isRowSelected()) {
            if (m_pCanvas->selection()->contains(QPoint(1, m_iResizedRow))) {
                start = m_pCanvas->selection()->lastRange().top();
                end = m_pCanvas->selection()->lastRange().bottom();
                rect = m_pCanvas->selection()->lastRange();
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
        QRect rect = m_pCanvas->selection()->lastRange();

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
                if (m_pCanvas->selection()->isColumnSelected()) {
                    KMessageBox::error(/* XXX TODO this*/0, i18n("Area is too large."));
                    return;
                }

                HideShowManipulator* command = new HideShowManipulator();
                command->setSheet(sheet);
                command->setManipulateRows(true);
                command->setReverse(true);
                command->add(*m_pCanvas->selection());
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
        command->setSheet(m_pCanvas->activeSheet());
        command->setSize(qMax(2.0, resize));
        command->add(*m_pCanvas->selection());
        if (!command->execute())
            delete command;
    } else { // hide
        HideShowManipulator* command = new HideShowManipulator();
        command->setSheet(m_pCanvas->activeSheet());
        command->setManipulateRows(true);
        command->add(*m_pCanvas->selection());
        if (!command->execute())
            delete command;
    }
}

void RowHeader::mouseDoubleClick(KoPointerEvent*)
{
    if (!m_cellToolIsActive)
        return;
    register Sheet * const sheet = m_pCanvas->activeSheet();
    if (!sheet)
        return;

    if (!m_pCanvas->doc()->isReadWrite() || sheet->isProtected())
        return;

    AdjustColumnRowManipulator* command = new AdjustColumnRowManipulator();
    command->setSheet(sheet);
    command->setAdjustRow(true);
    command->add(*m_pCanvas->selection());
    command->execute();
}


void RowHeader::mouseMove(KoPointerEvent* _ev)
{
    if (!m_cellToolIsActive) {
        setCursor(Qt::ArrowCursor);
        return;
    }
    if (!m_pCanvas->doc()->isReadWrite())
        return;

    register Sheet * const sheet = m_pCanvas->activeSheet();
    if (!sheet)
        return;

    double ev_PosY = m_pCanvas->zoomHandler()->unzoomItY(_ev->pos().y()) + m_pCanvas->yOffset();
    double dHeight = m_pCanvas->zoomHandler()->unzoomItY(height());

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

        QPoint newAnchor = m_pCanvas->selection()->anchor();
        QPoint newMarker = m_pCanvas->selection()->marker();
        newMarker.setY(row);
        newAnchor.setY(m_iSelectionAnchor);
        m_pCanvas->selection()->update(newMarker);

        if (_ev->pos().y() < 0)
            m_pCanvas->setVertScrollBarPos(ev_PosY);
        else if (_ev->pos().y() > m_pCanvas->height()) {
            if (row < KS_rowMax) {
                const RowFormat* rowFormat = sheet->rowFormat(row + 1);
                y = sheet->rowPosition(row + 1);
                m_pCanvas->setVertScrollBarPos(ev_PosY + rowFormat->height() - dHeight);
            }
        }
    }
    // No button is pressed and the mouse is just moved
    else {

        //What is the internal size of 1 pixel
        const double unzoomedPixel = m_pCanvas->zoomHandler()->unzoomItY(1.0);
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

void RowHeader::wheel(KoPointerEvent* _ev)
{
    //XXX TODO m_pCanvas->wheel(_ev);
}

void RowHeader::paint(QPainter* painter, const QRectF& painterRect)
{
    register Sheet * const sheet = m_pCanvas->activeSheet();
    if (!sheet)
        return;

//     ElapsedTime et( "Painting vertical header", ElapsedTime::PrintOnlyTime );

    // FIXME Stefan: Make use of clipping. Find the repaint call after the scrolling.
    // kDebug(36004) << event->rect();

    // painting rectangle
    const QRectF paintRect = m_pCanvas->zoomHandler()->viewToDocument(painterRect);

    // the painter
    painter->scale(m_pCanvas->zoomHandler()->zoomedResolutionX(), m_pCanvas->zoomHandler()->zoomedResolutionY());
    painter->setRenderHint(QPainter::TextAntialiasing);

    // fonts
    QFont normalFont(painter->font());
    QFont boldFont(normalFont);
    boldFont.setBold(true);

    // background brush/color
    const QBrush backgroundBrush(palette().window());
    const QColor backgroundColor(backgroundBrush.color());

    // selection brush/color
    QColor selectionColor(palette().highlight().color());
    selectionColor.setAlpha(127);
    const QBrush selectionBrush(selectionColor);

    painter->setClipRect(paintRect);

    double yPos;
    // Get the top row and the current y-position
    int y = sheet->topRow(paintRect.y() + m_pCanvas->yOffset(), yPos);
    // Align to the offset
    yPos = yPos - m_pCanvas->yOffset();

    const KoViewConverter *converter = m_pCanvas->zoomHandler();
    const double width = converter->viewToDocumentX(this->width() - 1);

    QSet<int> selectedRows;
    QSet<int> affectedRows;
    if (!m_pCanvas->selection()->referenceSelectionMode() && m_cellToolIsActive) {
        selectedRows = m_pCanvas->selection()->rowsSelected();
        affectedRows = m_pCanvas->selection()->rowsAffected();
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
            painter->setPen(selectionColor.dark(150));
            painter->setBrush(selectionBrush);
        } else {
            painter->setPen(backgroundColor.dark(150));
            painter->setBrush(backgroundBrush);
        }
        painter->drawRect(QRectF(0, yPos, width, height));

        QString rowText = QString::number(y);

        // Reset painter
        painter->setFont(normalFont);
        painter->setPen(palette().text().color());

        if (selected)
            painter->setPen(palette().highlightedText().color());
        else if (highlighted)
            painter->setFont(boldFont);

        const int ascent = painter->fontMetrics().ascent();
        if (height >= ascent - painter->fontMetrics().descent()) {
            const double len = painter->fontMetrics().width(rowText);
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
                             yPos + (height - ascent) / 2),
//                            yPos + ( height - painter.fontMetrics().ascent() - painter.fontMetrics().descent() ) / 2 ),
                     rowText);
        }

        yPos += rowFormat->height();
        y++;
    }
}


void RowHeader::focusOut(QFocusEvent*)
{
    m_pCanvas->disableAutoScroll();
    m_bMousePressed = false;
}

void RowHeader::drawText(QPainter* painter, const QFont& font,
                         const QPointF& location, const QString& text) const
{
    register Sheet * const sheet = m_pCanvas->activeSheet();
    if (!sheet)
        return;

    const double scaleX = POINT_TO_INCH(double(KoDpi::dpiX()));
    const double scaleY = POINT_TO_INCH(double(KoDpi::dpiY()));

    // Qt scales the font already with the logical resolution. Do not do it twice!
    painter->save();
    painter->scale(1.0 / scaleX, 1.0 / scaleY);

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
    textLayout.draw(painter, loc);

    painter->restore();
}

void RowHeader::doToolChanged(const QString& toolId)
{
    m_cellToolIsActive = toolId.startsWith("KSpread");
    update();
}


/****************************************************************
 *
 * ColumnHeader
 *
 ****************************************************************/

ColumnHeader::ColumnHeader(CanvasBase *_canvas)
    : m_pCanvas(_canvas), m_bSelection(false),
    m_iSelectionAnchor(1), m_bResize(false), m_lSize(0), m_bMousePressed(false),
    m_cellToolIsActive(true)
{
}


ColumnHeader::~ColumnHeader()
{
}

void ColumnHeader::mousePress(KoPointerEvent * _ev)
{
    if (!m_cellToolIsActive)
        return;
    if (!m_pCanvas->doc()->isReadWrite())
        return;

    if (_ev->button() == Qt::LeftButton) {
        m_bMousePressed = true;
        m_pCanvas->enableAutoScroll();
    }

    const register Sheet * const sheet = m_pCanvas->activeSheet();
    if (!sheet)
        return;

    // We were editing a cell -> save value and get out of editing mode
    m_pCanvas->selection()->emitCloseEditor(true); // save changes

    double ev_PosX;
    double dWidth = m_pCanvas->zoomHandler()->unzoomItX(width());
    if (sheet->layoutDirection() == Qt::RightToLeft)
        ev_PosX = dWidth - m_pCanvas->zoomHandler()->unzoomItX(_ev->pos().x()) + m_pCanvas->xOffset();
    else
        ev_PosX = m_pCanvas->zoomHandler()->unzoomItX(_ev->pos().x()) + m_pCanvas->xOffset();
    m_bResize = false;
    m_bSelection = false;

    // Find the first visible column and the x position of this column.
    double x;

    const double unzoomedPixel = m_pCanvas->zoomHandler()->unzoomItX(1.0);
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
        if (_ev->button() != Qt::RightButton) {
            m_bSelection = true;
        }

        double tmp;
        int hit_col = sheet->leftColumn(ev_PosX, tmp);
        if (hit_col > KS_colMax)
            return;

        m_iSelectionAnchor = hit_col;

        if (!m_pCanvas->selection()->contains(QPoint(hit_col, 1)) ||
                !(_ev->button() == Qt::RightButton) ||
                !m_pCanvas->selection()->isColumnSelected()) {
            QPoint newMarker(hit_col, 1);
            QPoint newAnchor(hit_col, KS_rowMax);
            if (_ev->modifiers() == Qt::ControlModifier) {
                m_pCanvas->selection()->extend(QRect(newAnchor, newMarker));
            } else if (_ev->modifiers() == Qt::ShiftModifier) {
                m_pCanvas->selection()->update(newMarker);
            } else {
                m_pCanvas->selection()->initialize(QRect(newAnchor, newMarker));
            }
        }

        if (_ev->button() == Qt::RightButton) {
            m_pCanvas->mousePressed(_ev);
        }
    }
}

void ColumnHeader::mouseRelease(KoPointerEvent * _ev)
{
    if (!m_cellToolIsActive)
        return;
    m_pCanvas->disableAutoScroll();
    if (m_lSize)
        m_lSize->hide();

    m_bMousePressed = false;

    if (!m_pCanvas->doc()->isReadWrite())
        return;

    register Sheet * const sheet = m_pCanvas->activeSheet();
    if (!sheet)
        return;

    if (m_bResize) {
        double dWidth = m_pCanvas->zoomHandler()->unzoomItX(width());
        double ev_PosX;

        // Remove size indicator painted by paintSizeIndicator
        removeSizeIndicator();

        int start = m_iResizedColumn;
        int end   = m_iResizedColumn;
        QRect rect;
        rect.setCoords(m_iResizedColumn, 1, m_iResizedColumn, KS_rowMax);
        if (m_pCanvas->selection()->isColumnSelected()) {
            if (m_pCanvas->selection()->contains(QPoint(m_iResizedColumn, 1))) {
                start = m_pCanvas->selection()->lastRange().left();
                end   = m_pCanvas->selection()->lastRange().right();
                rect  = m_pCanvas->selection()->lastRange();
            }
        }

        double width = 0.0;
        double x;

        if (sheet->layoutDirection() == Qt::RightToLeft)
            ev_PosX = dWidth - m_pCanvas->zoomHandler()->unzoomItX(_ev->pos().x()) + m_pCanvas->xOffset();
        else
            ev_PosX = m_pCanvas->zoomHandler()->unzoomItX(_ev->pos().x()) + m_pCanvas->xOffset();

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
        QRect rect = m_pCanvas->selection()->lastRange();

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
                if (m_pCanvas->selection()->isRowSelected()) {
                    KMessageBox::error(0 /* XXX TODO this */, i18n("Area is too large."));
                    return;
                }

                HideShowManipulator* command = new HideShowManipulator();
                command->setSheet(sheet);
                command->setManipulateColumns(true);
                command->setReverse(true);
                command->add(*m_pCanvas->selection());
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
        command->setSheet(m_pCanvas->activeSheet());
        command->setSize(qMax(2.0, resize));
        command->add(*m_pCanvas->selection());
        if (!command->execute())
            delete command;
    } else { // hide
        HideShowManipulator* command = new HideShowManipulator();
        command->setSheet(m_pCanvas->activeSheet());
        command->setManipulateColumns(true);
        command->add(*m_pCanvas->selection());
        if (!command->execute())
            delete command;
    }
}

void ColumnHeader::mouseDoubleClick(KoPointerEvent*)
{
    if (!m_cellToolIsActive)
        return;
    register Sheet * const sheet = m_pCanvas->activeSheet();
    if (!sheet)
        return;

    if (!m_pCanvas->doc()->isReadWrite() || sheet->isProtected())
        return;

    AdjustColumnRowManipulator* command = new AdjustColumnRowManipulator();
    command->setSheet(sheet);
    command->setAdjustColumn(true);
    command->add(*m_pCanvas->selection());
    command->execute();
}

void ColumnHeader::mouseMove(KoPointerEvent* _ev)
{
    if (!m_cellToolIsActive)
        return;
    if (!m_pCanvas->doc()->isReadWrite())
        return;

    register Sheet * const sheet = m_pCanvas->activeSheet();

    if (!sheet)
        return;

    double dWidth = m_pCanvas->zoomHandler()->unzoomItX(width());
    double ev_PosX;
    if (sheet->layoutDirection() == Qt::RightToLeft)
        ev_PosX = dWidth - m_pCanvas->zoomHandler()->unzoomItX(_ev->pos().x()) + m_pCanvas->xOffset();
    else
        ev_PosX = m_pCanvas->zoomHandler()->unzoomItX(_ev->pos().x()) + m_pCanvas->xOffset();

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

        QPoint newMarker = m_pCanvas->selection()->marker();
        QPoint newAnchor = m_pCanvas->selection()->anchor();
        newMarker.setX(col);
        newAnchor.setX(m_iSelectionAnchor);
        m_pCanvas->selection()->update(newMarker);

        if (sheet->layoutDirection() == Qt::RightToLeft) {
            if (_ev->pos().x() < width() - m_pCanvas->width()) {
                const ColumnFormat *cl = sheet->columnFormat(col + 1);
                x = sheet->columnPosition(col + 1);
                m_pCanvas->setHorizScrollBarPos(- (int)((ev_PosX + cl->width()) - dWidth));
            } else if (_ev->pos().x() > width())
                m_pCanvas->setHorizScrollBarPos(- (ev_PosX - dWidth + m_pCanvas->zoomHandler()->unzoomItX(m_pCanvas->width())));
        } else {
            if (_ev->pos().x() < 0)
                m_pCanvas->setHorizScrollBarPos(ev_PosX);
            else if (_ev->pos().x() > m_pCanvas->width()) {
                if (col < KS_colMax) {
                    const ColumnFormat *cl = sheet->columnFormat(col + 1);
                    x = sheet->columnPosition(col + 1);
                    m_pCanvas->setHorizScrollBarPos(ev_PosX + cl->width() - dWidth);
                }
            }
        }

    }
    // No button is pressed and the mouse is just moved
    else {
        //What is the internal size of 1 pixel
        const double unzoomedPixel = m_pCanvas->zoomHandler()->unzoomItX(1.0);
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

            while (x < m_pCanvas->zoomHandler()->unzoomItY(width()) + m_pCanvas->xOffset() && tmpCol <= KS_colMax) {
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

void ColumnHeader::wheel(KoPointerEvent* _ev)
{
    //XXX TODO m_pCanvas->wheel(_ev);
}

void ColumnHeader::resize(const QSizeF& size, const QSizeF& oldSize)
{
    register Sheet * const sheet = m_pCanvas->activeSheet();
    if (!sheet)
        return;

    // workaround to allow horizontal resizing and zoom changing when sheet
    // direction and interface direction don't match (e.g. an RTL sheet on an
    // LTR interface)
    if (sheet->layoutDirection() == Qt::RightToLeft && !QApplication::isRightToLeft()) {
        int dx = size.width() - oldSize.width();
        scroll(dx, 0);
    } else if (sheet->layoutDirection() == Qt::LeftToRight && QApplication::isRightToLeft()) {
        int dx = size.width() - oldSize.width();
        scroll(-dx, 0);
    }
}

void ColumnHeader::paint(QPainter* painter, const QRectF& painterRect)
{
    register Sheet * const sheet = m_pCanvas->activeSheet();
    if (!sheet)
        return;

//     ElapsedTime et( "Painting horizontal header", ElapsedTime::PrintOnlyTime );

    // FIXME Stefan: Make use of clipping. Find the repaint call after the scrolling.
    // kDebug(36004) << event->rect();

    // painting rectangle
    const QRectF paintRect = m_pCanvas->zoomHandler()->viewToDocument(painterRect);

    // the painter
    painter->scale(m_pCanvas->zoomHandler()->zoomedResolutionX(), m_pCanvas->zoomHandler()->zoomedResolutionY());
    painter->setRenderHint(QPainter::TextAntialiasing);

    // fonts
    QFont normalFont(painter->font());
    QFont boldFont(normalFont);
    boldFont.setBold(true);

    // background brush/color
    const QBrush backgroundBrush(palette().window());
    const QColor backgroundColor(backgroundBrush.color());

    // selection brush/color
    QColor selectionColor(palette().highlight().color());
    selectionColor.setAlpha(127);
    const QBrush selectionBrush(selectionColor);

    painter->setClipRect(paintRect);

    double xPos;
    int x;

    if (sheet->layoutDirection() == Qt::RightToLeft) {
        //Get the left column and the current x-position
        x = sheet->leftColumn(int(m_pCanvas->zoomHandler()->unzoomItX(width()) - paintRect.x() + m_pCanvas->xOffset()), xPos);
        //Align to the offset
        xPos = m_pCanvas->zoomHandler()->unzoomItX(width()) - xPos + m_pCanvas->xOffset();
    } else {
        //Get the left column and the current x-position
        x = sheet->leftColumn(int(paintRect.x() + m_pCanvas->xOffset()), xPos);
        //Align to the offset
        xPos = xPos - m_pCanvas->xOffset();
    }

    const KoViewConverter *converter = m_pCanvas->zoomHandler();
    const double height = converter->viewToDocumentY(this->height() - 1);

    if (sheet->layoutDirection() == Qt::RightToLeft) {
        if (x > KS_colMax)
            x = KS_colMax;

        xPos -= sheet->columnFormat(x)->width();

        QSet<int> selectedColumns;
        QSet<int> affectedColumns;
        if (!m_pCanvas->selection()->referenceSelectionMode() && m_cellToolIsActive) {
            selectedColumns = m_pCanvas->selection()->columnsSelected();
            affectedColumns = m_pCanvas->selection()->columnsAffected();
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
                painter->setPen(selectionColor.dark(150));
                painter->setBrush(selectionBrush);
            } else {
                painter->setPen(backgroundColor.dark(150));
                painter->setBrush(backgroundBrush);
            }
            painter->drawRect(QRectF(xPos, 0, width, height));

            // Reset painter
            painter->setFont(normalFont);
            painter->setPen(palette().text().color());

            if (selected)
                painter->setPen(palette().highlightedText().color());
            else if (highlighted)
                painter->setFont(boldFont);

            QString colText = sheet->getShowColumnNumber() ? QString::number(x) : Cell::columnName(x);
            double len = painter->fontMetrics().width(colText);
            if (width >= len) {
                drawText(painter,
                         normalFont,
                         QPointF(xPos + (width - len) / 2,
                                 (height - painter->fontMetrics().ascent() - painter->fontMetrics().descent()) / 2),
                         colText,
                         width);
            }
            xPos += columnFormat->width();
            --x;
        }
    } else { // if ( sheet->layoutDirection() == Qt::LeftToRight )
        QSet<int> selectedColumns;
        QSet<int> affectedColumns;
        if (!m_pCanvas->selection()->referenceSelectionMode() && m_cellToolIsActive) {
            selectedColumns = m_pCanvas->selection()->columnsSelected();
            affectedColumns = m_pCanvas->selection()->columnsAffected();
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
                painter->setPen(selectionColor.dark(150));
                painter->setBrush(selectionBrush);
            } else {
                painter->setPen(backgroundColor.dark(150));
                painter->setBrush(backgroundBrush);
            }
            painter->drawRect(QRectF(xPos, 0, width, height));

            // Reset painter
            painter->setFont(normalFont);
            painter->setPen(palette().text().color());

            if (selected)
                painter->setPen(palette().highlightedText().color());
            else if (highlighted)
                painter->setFont(boldFont);

            QString colText = sheet->getShowColumnNumber() ? QString::number(x) : Cell::columnName(x);
            int len = painter->fontMetrics().width(colText);
            if (width >= len) {
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
                                 (height - painter->fontMetrics().ascent() - painter->fontMetrics().descent()) / 2),
                         colText,
                         width);
            }

            xPos += columnFormat->width();
            ++x;
        }
    }
}


void ColumnHeader::focusOut(QFocusEvent*)
{
    m_pCanvas->disableAutoScroll();
    m_bMousePressed = false;
}

void ColumnHeader::drawText(QPainter* painter, const QFont& font,
                            const QPointF& location, const QString& text,
                            double width) const
{
    register Sheet * const sheet = m_pCanvas->activeSheet();
    if (!sheet)
        return;

    const double scaleX = POINT_TO_INCH(double(KoDpi::dpiX()));
    const double scaleY = POINT_TO_INCH(double(KoDpi::dpiY()));

    // Qt scales the font already with the logical resolution. Do not do it twice!
    painter->save();
    painter->scale(1.0 / scaleX, 1.0 / scaleY);

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
    textLayout.draw(painter, loc);

    painter->restore();
}

void ColumnHeader::doToolChanged(const QString& toolId)
{
    m_cellToolIsActive = toolId.startsWith("KSpread");
    update();
}


/****************************************************************
 *
 * SelectAllButton
 *
 ****************************************************************/

SelectAllButton::SelectAllButton(CanvasBase* canvasBase, Selection* selection)
        : m_canvasBase(canvasBase)
        , m_selection(selection)
        , m_mousePressed(false)
        , m_cellToolIsActive(true)
{
}

SelectAllButton::~SelectAllButton()
{
}

void SelectAllButton::paint(QPainter* painter, const QRectF& painterRect)
{
    // the painter
    painter->setClipRect(painterRect);

    // if all cells are selected
    if (m_selection->isAllSelected() &&
            !m_selection->referenceSelectionMode() && m_cellToolIsActive) {
        // selection brush/color
        QColor selectionColor(palette().highlight().color());
        selectionColor.setAlpha(127);
        const QBrush selectionBrush(selectionColor);

        painter->setPen(selectionColor.dark(150));
        painter->setBrush(selectionBrush);
    } else {
        // background brush/color
        const QBrush backgroundBrush(palette().window());
        const QColor backgroundColor(backgroundBrush.color());

        painter->setPen(backgroundColor.dark(150));
        painter->setBrush(backgroundBrush);
    }
    painter->drawRect(painterRect.adjusted(0, 0, -1, -1));
}

void SelectAllButton::mousePress(KoPointerEvent* event)
{
    if (!m_cellToolIsActive)
        return;
    if (event->button() == Qt::LeftButton)
        m_mousePressed = true;
}

void SelectAllButton::mouseRelease(KoPointerEvent* event)
{
    if (!m_cellToolIsActive)
        return;
    Q_UNUSED(event);
    if (!m_mousePressed)
        return;
    m_mousePressed = false;
    m_selection->selectAll();
}

void SelectAllButton::wheel(KoPointerEvent* event)
{
    //XXX TODO m_canvasBase->wheel(event);
}

void SelectAllButton::doToolChanged(const QString& toolId)
{
    m_cellToolIsActive = toolId.startsWith("KSpread");
    update();
}
