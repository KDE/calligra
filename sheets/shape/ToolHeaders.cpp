/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Robert Knight <robertknight@gmail.com>
   SPDX-FileCopyrightText: 2006 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 1999-2002, 2004 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2002-2005 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 1999-2004 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2004-2005 Meni Livne <livne@kde.org>
   SPDX-FileCopyrightText: 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2003 Hamish Rodda <rodda@kde.org>
   SPDX-FileCopyrightText: 2003 Joseph Wenninger <jowenn@kde.org>
   SPDX-FileCopyrightText: 2003 Lukas Tinkl <lukas@kde.org>
   SPDX-FileCopyrightText: 2000-2002 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 2002 Harri Porten <porten@kde.org>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 2002 Daniel Naber <daniel.naber@t-online.de>
   SPDX-FileCopyrightText: 1999-2000 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 1999-2000 Stephan Kulow <coolo@kde.org>
   SPDX-FileCopyrightText: 2000 Bernd Wuebben <wuebben@kde.org>
   SPDX-FileCopyrightText: 2000 Wilco Greven <greven@kde.org>
   SPDX-FileCopyrightText: 2000 Simon Hausmann <hausmann@kde.org
   SPDX-FileCopyrightText: 1999 Michael Reiher <michael.reiher@gmx.de>
   SPDX-FileCopyrightText: 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>
   SPDX-FileCopyrightText: 1999 Reginald Stadlbauer <reggie@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "ToolHeaders.h"

// Qt
#include <QApplication>
#include <QLabel>
#include <QPainter>
#include <QRubberBand>
#include <QScrollBar>
#include <QStyle>
#include <QToolTip>

// Calligra
#include "KoCanvasBase.h"
#include "KoViewConverter.h"
#include <KoGlobal.h>
#include <KoPointerEvent.h>
#include <KoUnit.h>

// Sheets
#include "core/ColFormatStorage.h"
#include "core/RowFormatStorage.h"
#include "core/Sheet.h"
#include "engine/CellBase.h"
#include "engine/calligra_sheets_limits.h"
#include "ui/Selection.h"
#include "ui/commands/RowColumnManipulators.h"

using namespace Calligra::Sheets;

/****************************************************************
 *
 * Tool::RowHeader
 *
 ****************************************************************/

Tool::RowHeader::RowHeader()
    : m_pCanvas(nullptr)
    , m_selection(nullptr)
    , m_HScrollBar(nullptr)
    , m_VScrollBar(nullptr)
    , m_bSelection(false)
    , m_iSelectionAnchor(1)
    , m_bResize(false)
    , m_lSize(0)
    , m_bMousePressed(false)
    , m_font(KoGlobal::defaultFont())
    , m_rubberband(nullptr)
    , m_sheetWidth(0.0)
{
}

Tool::RowHeader::~RowHeader()
{
}

void Tool::RowHeader::setSelection(Selection *selection)
{
    m_selection = selection;
}

void Tool::RowHeader::setScrollBars(QScrollBar *hbar, QScrollBar *vbar)
{
    m_HScrollBar = hbar;
    m_VScrollBar = vbar;
}

void Tool::RowHeader::setGeometry(const QRectF &geometry)
{
    m_geometry = geometry;
}

QRectF Tool::RowHeader::geometry() const
{
    return m_geometry;
}

QPointF Tool::RowHeader::position() const
{
    return m_geometry.topLeft();
}

QSizeF Tool::RowHeader::size() const
{
    return m_geometry.size();
}

qreal Tool::RowHeader::width() const
{
    return m_geometry.width();
}

qreal Tool::RowHeader::height() const
{
    return m_geometry.height();
}

qreal Tool::RowHeader::resizeAreaSize() const
{
    qreal pix = m_pCanvas->viewConverter()->documentToViewY(2.0);
    return pix;
}

int Tool::RowHeader::betweenRows(qreal pos) const
{
    const qreal resizeHeight = resizeAreaSize();
    qreal topBorderPos;
    int row = m_sheet->topRow(pos - resizeHeight, topBorderPos);
    if (row > 0 && row <= KS_rowMax) {
        const qreal bottomBorderPos = topBorderPos + m_sheet->rowFormats()->rowHeight(row);
        Q_ASSERT(bottomBorderPos > topBorderPos);
        if ((pos >= bottomBorderPos - resizeHeight) && (pos <= bottomBorderPos + resizeHeight)
            && !(row == 1 && m_sheet->rowFormats()->isHiddenOrFiltered(row))) // if row is hidden and is the first row it shall not be resized
        {
            return row;
        }
    }
    return 0;
}

void Tool::RowHeader::mousePress(KoPointerEvent *_ev)
{
    Sheet *const sheet = m_sheet;
    if (!sheet) {
        return;
    }
    if (_ev->button() == Qt::LeftButton) {
        m_bMousePressed = true;
    }
    m_mousePressedPoint = _ev->point;
    qreal ev_PosY = _ev->point.y();
    m_bResize = false;
    m_bSelection = false;

    // Did the user click between two rows?
    m_iResizedRow = betweenRows(ev_PosY);
    m_bResize = m_iResizedRow > 0;

    if (m_bResize) {
        if (!sheet->isProtected()) {
            m_lastResizePos = _ev->y();
            paintSizeIndicator(_ev);
        }
    } else {
        if (_ev->button() != Qt::RightButton) {
            m_bSelection = true;
        }

        qreal tmp;
        int hit_row = sheet->topRow(ev_PosY, tmp);
        if (hit_row > KS_rowMax) {
            return;
        }
        m_iSelectionAnchor = hit_row;
        if (m_selection && (!m_selection->contains(QPoint(1, hit_row)) || !(_ev->button() == Qt::RightButton) || !m_selection->isRowSelected())) {
            QPoint newMarker(1, hit_row);
            QPoint newAnchor(KS_colMax, hit_row);
            if (_ev->modifiers() == Qt::ControlModifier) {
                m_selection->extend(QRect(newAnchor, newMarker));
            } else if (_ev->modifiers() == Qt::ShiftModifier) {
                m_selection->update(newMarker);
            } else {
                m_selection->initialize(QRect(newAnchor, newMarker));
            }
        }

        if (_ev->button() == Qt::RightButton) {
            // m_pCanvas->mousePressed(_ev);
        }
    }
}

void Tool::RowHeader::mouseRelease(KoPointerEvent *_ev)
{
    if (m_lSize) {
        m_lSize->hide();
    }
    m_bMousePressed = false;

    Sheet *const sheet = m_sheet;
    if (!sheet) {
        return;
    }
    double ev_PosY = _ev->point.y();

    if (m_bResize) {
        // Remove size indicator painted by paintSizeIndicator
        removeSizeIndicator();

        QRect rect;
        rect.setCoords(1, m_iResizedRow, KS_colMax, m_iResizedRow);
        if (m_selection->isRowSelected()) {
            if (m_selection->contains(QPoint(1, m_iResizedRow))) {
                rect = m_selection->lastRange();
            }
        }
        double height = 0.0;
        double y = sheet->rowPosition(m_iResizedRow);
        if (ev_PosY - y <= 0.0) {
            height = 0.0;
        } else {
            height = ev_PosY - y;
        }
#if 1
        if (height != 0.0) {
            ResizeRowManipulator *command = new ResizeRowManipulator();
            command->setSheet(sheet);
            command->setSize(height);
            command->add(Region(rect, sheet));
            if (!command->execute(m_pCanvas)) {
                delete command;
            } else {
                m_pCanvas->canvasWidget()->update();
            }
        } else { // hide
            HideShowManipulator *command = new HideShowManipulator();
            command->setSheet(sheet);
            command->setManipulateRows(true);
            command->add(Region(rect, sheet));
            if (!command->execute(m_pCanvas)) {
                delete command;
            } else {
                m_pCanvas->canvasWidget()->update();
            }
        }
#endif
        delete m_lSize;
        m_lSize = nullptr;
    } else if (m_bSelection) {
#if 0
        QRect rect = m_selection->lastRange();

        // TODO: please don't remove. Right now it's useless, but it's for a future feature
        // Norbert
        bool m_frozen = false;
        if (m_frozen) {
            debugSheetsTableShape << "selected: T" << rect.top() << " B" << rect.bottom();

            int i;
            QList<int> hiddenRows;

            for (i = rect.top(); i <= rect.bottom(); ++i) {
                if (sheet->rowFormats()->isHidden(i)) {
                    hiddenRows.append(i);
                }
            }

            if (hiddenRows.count() > 0) {
                if (m_selection->isColumnSelected()) {
                    KMessageBox::error(/* XXX TODO this*/0, i18n("Area is too large."));
                    return;
                }

                HideShowManipulator* command = new HideShowManipulator();
                command->setSheet(sheet);
                command->setManipulateRows(true);
                command->setHide(true);
                command->add(*m_selection);
                command->execute();
            }
        }
#endif
    }

    m_bSelection = false;
    m_bResize = false;
}

void Tool::RowHeader::equalizeRow(double)
{
#if 0
    if (resize != 0.0) {
        ResizeRowManipulator* command = new ResizeRowManipulator();
        command->setSheet(m_pCanvas->activeSheet());
        command->setSize(qMax(2.0, resize));
        command->add(*m_selection);
        if (!command->execute())
            delete command;
    } else { // hide
        HideShowManipulator* command = new HideShowManipulator();
        command->setSheet(m_pCanvas->activeSheet());
        command->setManipulateRows(true);
        command->add(*m_selection);
        if (!command->execute())
            delete command;
    }
#endif
}

void Tool::RowHeader::mouseDoubleClick(KoPointerEvent *)
{
#if 0
    register Sheet * const sheet = m_pCanvas->activeSheet();
    if (!sheet)
        return;

    if (sheet->isProtected())
        return;

    AdjustColumnRowManipulator* command = new AdjustColumnRowManipulator();
    command->setSheet(sheet);
    command->setAdjustRow(true);
    command->add(*m_selection);
    command->execute();
#endif
}

void Tool::RowHeader::mouseMove(KoPointerEvent *_ev)
{
    Sheet *const sheet = m_sheet;
    if (!sheet) {
        return;
    }
    qreal ev_PosY = _ev->point.y();
    qreal bottom = scrollOffset() + height();

    // The button is pressed and we are resizing ?
    if (m_bResize) {
        if (ev_PosY < scrollOffset()) {
            m_VScrollBar->setValue(m_VScrollBar->value() - m_VScrollBar->singleStep());
        } else if (ev_PosY > bottom) {
            m_VScrollBar->setValue(m_VScrollBar->value() + m_VScrollBar->singleStep());
        }
        if (!sheet->isProtected()) {
            paintSizeIndicator(_ev);
        }
    } else if (m_bSelection) {
        // The button is pressed and we are selecting
        qreal y;
        int row = sheet->topRow(ev_PosY, y);
        if (row > KS_rowMax || row <= 0)
            return;

        QPoint newCursor = m_selection->cursor();
        newCursor.setY(row);
        m_selection->update(newCursor);

        if (_ev->point.y() < scrollOffset()) {
            if (row > 1) {
                const qreal rowHeight = sheet->rowFormats()->rowHeight(row - 1);
                y = sheet->rowPosition(row - 1);
                m_VScrollBar->setValue(m_VScrollBar->value() - rowHeight);
            }
        } else if (_ev->pos().y() > bottom) {
            if (row < KS_rowMax) {
                const qreal rowHeight = sheet->rowFormats()->rowHeight(row + 1);
                y = sheet->rowPosition(row + 1);
                m_VScrollBar->setValue(m_VScrollBar->value() + rowHeight);
            }
        }
    } else {
        // No button is pressed and the mouse is just moved
        if (betweenRows(ev_PosY)) {
            m_pCanvas->canvasWidget()->setCursor(Qt::SplitVCursor);
            return;
        }
        m_pCanvas->canvasWidget()->setCursor(Qt::ArrowCursor);
    }
}

void Tool::RowHeader::paintSizeIndicator(KoPointerEvent *event)
{
    Sheet *const sheet = m_sheet;
    if (!sheet) {
        return;
    }
    const KoViewConverter *viewConverter = m_pCanvas->viewConverter();
    qreal rowTop = sheet->rowPosition(m_iResizedRow);
    m_iResizePos = event->point.y();

    // Don't make the row have a height < 2 pixel.
    bool hideRow = false;
    if (m_iResizePos < rowTop + viewConverter->viewToDocumentY(2)) {
        m_iResizePos = m_lastResizePos;
        hideRow = true;
    }
    m_lastResizePos = m_iResizePos;
    if (!m_rubberband) {
        int x = viewConverter->documentToViewX(m_geometry.left());
        int w = viewConverter->documentToViewX(width() + m_sheetWidth);
        m_rubberband = new QRubberBand(QRubberBand::Line, m_pCanvas->canvasWidget());
        m_rubberband->setGeometry(x, 0, w, 2);
        m_rubberband->show();
    }
    int yPos = viewConverter->documentToViewY(m_geometry.top() - scrollOffset() + (hideRow ? rowTop : m_iResizePos));
    m_rubberband->move(m_rubberband->geometry().left(), yPos);
    QString tip;
    if (hideRow) {
        tip = i18n("Hide Row");
    } else {
        tip = i18n("Height: %1 %2", toUnitUserValue(m_iResizePos - rowTop), unitSymbol());
    }
    if (!m_lSize) {
        int screenNo = QApplication::desktop()->screenNumber(m_pCanvas->canvasWidget());
        m_lSize = new QLabel(QApplication::desktop()->screen(screenNo), Qt::ToolTip);
        m_lSize->setAlignment(Qt::AlignVCenter);
        m_lSize->setAutoFillBackground(true);
        m_lSize->setPalette(QToolTip::palette());
        m_lSize->setMargin(1 + m_pCanvas->canvasWidget()->style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, 0, m_lSize));
        m_lSize->setFrameShape(QFrame::Box);
        m_lSize->setIndent(1);
    }
    m_lSize->setText(tip);
    m_lSize->adjustSize();
    int xPos = viewConverter->documentToViewX(m_geometry.left());
    yPos = viewConverter->documentToViewX(m_geometry.top());
    yPos += viewConverter->documentToViewY(rowTop - scrollOffset());
    int w = viewConverter->documentToViewX(width());
    QPoint pos = (sheet->layoutDirection() == Qt::RightToLeft) ? QPoint(xPos + w - m_lSize->width() - 3, yPos - 3) : QPoint(xPos - 3, yPos - 3);
    pos -= QPoint(0, m_lSize->height());
    m_lSize->move(m_pCanvas->canvasWidget()->mapToGlobal(pos).x(), m_pCanvas->canvasWidget()->mapToGlobal(pos).y());
    m_lSize->show();
}

void Tool::RowHeader::removeSizeIndicator()
{
    delete m_rubberband;
    m_rubberband = 0;
}

QPalette Tool::RowHeader::palette() const
{
    return qApp->palette();
}

void Tool::RowHeader::paint(QPainter *painter, const QRectF &painterRect)
{
    Sheet *const sheet = m_sheet;
    if (!sheet) {
        return;
    }
    //     ElapsedTime et( "Painting vertical header", ElapsedTime::PrintOnlyTime );

    // FIXME Stefan: Make use of clipping. Find the repaint call after the scrolling.
    // debugSheetsRender << event->rect();

    // painting rectangle
    const QRectF paintRect = painterRect;

    // the painter
    painter->setRenderHint(QPainter::TextAntialiasing);

    // fonts
    QFont normalFont(m_font);
    QFont boldFont(normalFont);
    boldFont.setBold(true);

    // background brush/color
    const QBrush backgroundBrush(palette().window());
    const QColor backgroundColor(backgroundBrush.color());

    // selection brush/color
    QColor selectionColor(palette().highlight().color());
    selectionColor.setAlpha(127);
    const QBrush selectionBrush(selectionColor);

    qreal yPos = 0.0;
    // Get the top row and the current y-position
    int y = sheet->topRow(qMax<qreal>(0, paintRect.y()), yPos);
    const qreal width = this->width() - 1;

    bool useHighlight = (m_selection && !m_selection->referenceSelectionMode());
    // Loop through the rows, until we are out of range
    while (yPos <= paintRect.bottom() && y <= KS_rowMax) {
        if (sheet->rowFormats()->isHiddenOrFiltered(y)) {
            ++y;
            continue;
        }

        bool selected = false;
        bool highlighted = false;
        if (useHighlight) {
            selected = m_selection->isColumnSelected(y);
            highlighted = m_selection->isColumnAffected(y);
        }

        const qreal rawHeight = sheet->rowFormats()->rowHeight(y);
        const qreal height = rawHeight;
        const QRectF rect(0, yPos, width, height);
        if (selected || highlighted) {
            painter->setPen(QPen(selectionColor.darker(150), 0));
            painter->setBrush(selectionBrush);
        } else {
            painter->setPen(QPen(backgroundColor.darker(150), 0));
            painter->setBrush(backgroundBrush);
        }
        painter->drawRect(rect);

        const QString rowText = QString::number(y);

        // Reset painter
        painter->setFont(normalFont);
        painter->setPen(palette().text().color());

        if (selected) {
            painter->setPen(palette().highlightedText().color());
        } else if (highlighted) {
            painter->setFont(boldFont);
        }
        QFontMetricsF fm(painter->font());
        if (height < fm.ascent() - fm.descent()) {
            // try to scale down the font to make it fit
            QFont font = painter->font();
            qreal maxSize = font.pointSizeF();
            qreal minSize = maxSize / 2;
            while (minSize > 1) {
                font.setPointSizeF(minSize);
                const QFontMetricsF fm2(font);
                if (height >= fm2.ascent() - fm2.descent())
                    break;
                minSize /= 2;
            }
            while (minSize < 0.99 * maxSize) {
                qreal middle = (maxSize + minSize) / 2;
                font.setPointSizeF(middle);
                const QFontMetricsF fm2(font);
                if (height >= fm2.ascent() - fm2.descent()) {
                    minSize = middle;
                } else {
                    maxSize = middle;
                }
            }
            painter->setFont(font);
            fm = QFontMetricsF(font);
        }
        if (height >= fm.ascent() - fm.descent()) {
            painter->drawText(rect, Qt::AlignCenter, rowText);
        }

        yPos += rawHeight;
        y++;
    }
}

void Tool::RowHeader::setHeaderFont(const QFont &font)
{
    m_font = font;
}

QFont Tool::RowHeader::headerFont() const
{
    return m_font;
}

qreal Tool::RowHeader::toUnitUserValue(qreal value)
{
    return m_pCanvas->unit().toUserValue(value);
}

QString Tool::RowHeader::unitSymbol()
{
    return m_pCanvas->unit().symbol();
}

void Tool::RowHeader::setCanvas(KoCanvasBase *canvas)
{
    m_pCanvas = canvas;
}

void Tool::RowHeader::setSheetWidth(qreal width)
{
    m_sheetWidth = width;
}

qreal Tool::RowHeader::scrollOffset() const
{
    return (qreal)m_VScrollBar->value();
}

/****************************************************************
 *
 * Tool::ColumnHeader
 *
 ****************************************************************/

Tool::ColumnHeader::ColumnHeader()
    : m_pCanvas(nullptr)
    , m_selection(nullptr)
    , m_HScrollBar(nullptr)
    , m_VScrollBar(nullptr)
    , m_bSelection(false)
    , m_iSelectionAnchor(1)
    , m_bResize(false)
    , m_lSize(0)
    , m_bMousePressed(false)
    , m_font(KoGlobal::defaultFont())
    , m_rubberband(nullptr)
    , m_sheetHeight(0.0)
{
}

Tool::ColumnHeader::~ColumnHeader()
{
}

void Tool::ColumnHeader::setSelection(Selection *selection)
{
    m_selection = selection;
}

void Tool::ColumnHeader::setScrollBars(QScrollBar *hbar, QScrollBar *vbar)
{
    m_HScrollBar = hbar;
    m_VScrollBar = vbar;
}

qreal Tool::ColumnHeader::resizeAreaSize() const
{
    qreal pix = m_pCanvas->viewConverter()->documentToViewX(2.0);
    return pix;
}

int Tool::ColumnHeader::betweenColumns(qreal pos) const
{
    const qreal resizeWidth = resizeAreaSize();
    qreal leftBorderPos;
    int column = m_sheet->leftColumn(pos - resizeWidth, leftBorderPos);
    if (column > 0 && column <= KS_colMax) {
        const qreal rightBorderPos = leftBorderPos + m_sheet->columnFormats()->visibleWidth(column);
        Q_ASSERT(rightBorderPos > leftBorderPos);
        if ((pos >= rightBorderPos - resizeWidth) && (pos <= rightBorderPos + resizeWidth)
            && !(column == 1 && m_sheet->columnFormats()->isHiddenOrFiltered(column))) // if column is hidden and is the first column it shall not be resized
        {
            return column;
        }
    }
    return 0;
}

void Tool::ColumnHeader::setGeometry(const QRectF &geometry)
{
    m_geometry = geometry;
}

QRectF Tool::ColumnHeader::geometry() const
{
    return m_geometry;
}

QPointF Tool::ColumnHeader::position() const
{
    return m_geometry.topLeft();
}

QSizeF Tool::ColumnHeader::size() const
{
    return m_geometry.size();
}

qreal Tool::ColumnHeader::width() const
{
    return m_geometry.width();
}

qreal Tool::ColumnHeader::height() const
{
    return m_geometry.height();
}

void Tool::ColumnHeader::mousePress(KoPointerEvent *_ev)
{
    if (_ev->button() == Qt::LeftButton) {
        m_bMousePressed = true;
    }

    const Sheet *const sheet = m_sheet;
    if (!sheet) {
        return;
    }
    m_mousePressedPoint = _ev->point;
    qreal right = scrollOffset() + width();
    qreal ev_PosX = _ev->point.x();
    if (sheet->layoutDirection() == Qt::RightToLeft) {
        ev_PosX = right - ev_PosX;
    }
    m_bResize = false;
    m_bSelection = false;

    // Find the first visible column and the x position of this column.
    qreal x;
    const qreal resizeWidth = resizeAreaSize();
    if (sheet->layoutDirection() == Qt::RightToLeft) {
        int tmpCol = sheet->leftColumn(0.0, x);

        debugSheetsTableShape << "evPos:" << ev_PosX << ", x:" << x << ", COL:" << tmpCol;
        while (ev_PosX > x && (!m_bResize) && tmpCol <= KS_colMax) {
            double w = sheet->columnFormats()->colWidth(tmpCol);

            debugSheetsTableShape << "evPos:" << ev_PosX << ", x:" << x << ", w:" << w << ", COL:" << tmpCol;

            ++tmpCol;
            if (tmpCol > KS_colMax) {
                tmpCol = KS_colMax;
            }
            // if col is hide and it's the first column
            // you mustn't resize it.
            if (ev_PosX >= x + w - resizeWidth && ev_PosX <= x + w + resizeWidth && !(sheet->columnFormats()->isHiddenOrFiltered(tmpCol) && tmpCol == 1)) {
                m_bResize = true;
            }
            x += w;
        }

        // if col is hide and it's the first column
        // you mustn't resize it.
        qreal tmp2;
        tmpCol = sheet->leftColumn(right - ev_PosX + 1, tmp2);
        if (sheet->columnFormats()->isHiddenOrFiltered(tmpCol) && tmpCol == 0) {
            debugSheetsTableShape << "No resize:" << tmpCol << "," << sheet->columnFormats()->isHiddenOrFiltered(tmpCol);
            m_bResize = false;
        }
        debugSheetsTableShape << "Resize:" << m_bResize;
    } else {
        // the user click between two columns?
        m_iResizedColumn = betweenColumns(ev_PosX);
        m_bResize = m_iResizedColumn > 0;
    }
    if (m_bResize) {
        // Determine the column to resize
        qreal tmp;
        if (sheet->layoutDirection() == Qt::RightToLeft) {
            m_iResizedColumn = sheet->leftColumn(ev_PosX - 1, tmp);
            debugSheetsTableShape << "RColumn:" << m_iResizedColumn << ", PosX:" << ev_PosX;

            if (!sheet->isProtected()) {
                paintSizeIndicator(_ev);
            }
        } else {
            if (!sheet->isProtected()) {
                paintSizeIndicator(_ev);
            }
        }
        debugSheetsTableShape << "Resize Column:" << m_iResizedColumn;
    } else {
        if (_ev->button() != Qt::RightButton) {
            m_bSelection = true;
        }

        qreal tmp;
        int hit_col = sheet->leftColumn(ev_PosX, tmp);
        if (hit_col > KS_colMax) {
            return;
        }
        m_iSelectionAnchor = hit_col;
        if (!m_selection->contains(QPoint(hit_col, 1)) || !(_ev->button() == Qt::RightButton) || !m_selection->isColumnSelected()) {
            QPoint newMarker(hit_col, 1);
            QPoint newAnchor(hit_col, KS_rowMax);
            if (_ev->modifiers() == Qt::ControlModifier) {
                m_selection->extend(QRect(newAnchor, newMarker));
            } else if (_ev->modifiers() == Qt::ShiftModifier) {
                m_selection->update(newMarker);
            } else {
                m_selection->initialize(QRect(newAnchor, newMarker));
            }
        }

        if (_ev->button() == Qt::RightButton) {
            // m_pCanvas->mousePressed(_ev);
        }
    }
}

void Tool::ColumnHeader::mouseRelease(KoPointerEvent *_ev)
{
    if (m_lSize) {
        m_lSize->hide();
    }
    m_bMousePressed = false;

    Sheet *sheet = m_sheet;
    if (!sheet) {
        return;
    }
    if (m_bResize) {
        // Remove size indicator painted by paintSizeIndicator
        removeSizeIndicator();
#if 1
        QRect rect;
        rect.setCoords(m_iResizedColumn, 1, m_iResizedColumn, KS_rowMax);
        if (m_selection && m_selection->isColumnSelected()) {
            if (m_selection->contains(QPoint(m_iResizedColumn, 1))) {
                rect = m_selection->lastRange();
            }
        }
        qreal ev_PosX = _ev->point.x();
        if (sheet->layoutDirection() == Qt::RightToLeft) {
            ev_PosX = width() - ev_PosX;
        }
        qreal x = sheet->columnPosition(m_iResizedColumn);
        qreal width = 0.0;
        if (ev_PosX - x > 0.0) {
            width = ev_PosX - x;
        }
        if (width > 0.0) {
            ResizeColumnManipulator *command = new ResizeColumnManipulator();
            command->setSheet(sheet);
            command->setSize(width);
            command->add(Region(rect, sheet));
            if (!command->execute(m_pCanvas)) {
                delete command;
            }
        } else { // hide
            HideShowManipulator *command = new HideShowManipulator();
            command->setSheet(sheet);
            command->setManipulateColumns(true);
            command->add(Region(rect, sheet));
            if (!command->execute(m_pCanvas)) {
                delete command;
            }
        }
#endif
        delete m_lSize;
        m_lSize = 0;
    } else if (m_bSelection) {
#if 0
        QRect rect = m_selection->lastRange();

        // TODO: please don't remove. Right now it's useless, but it's for a future feature
        // Norbert
        bool m_frozen = false;
        if (m_frozen) {
            debugSheets << "selected: L" << rect.left() << " R" << rect.right();

            int i;
            QList<int> hiddenCols;

            for (i = rect.left(); i <= rect.right(); ++i) {
                if (sheet->columnFormat(i)->isHidden()) {
                    hiddenCols.append(i);
                }
            }

            if (hiddenCols.count() > 0) {
                if (m_selection->isRowSelected()) {
                    KMessageBox::error(0 /* XXX TODO this */, i18n("Area is too large."));
                    return;
                }

                HideShowManipulator* command = new HideShowManipulator();
                command->setSheet(sheet);
                command->setManipulateColumns(true);
                command->setHide(true);
                command->add(*m_selection);
                command->execute();
            }
        }
#endif
    }

    m_bSelection = false;
    m_bResize = false;
}

void Tool::ColumnHeader::equalizeColumn(double)
{
#if 0
    if (resize != 0.0) {
        ResizeColumnManipulator* command = new ResizeColumnManipulator();
        command->setSheet(m_pCanvas->activeSheet());
        command->setSize(qMax(2.0, resize));
        command->add(*m_selection);
        if (!command->execute())
            delete command;
    } else { // hide
        HideShowManipulator* command = new HideShowManipulator();
        command->setSheet(m_pCanvas->activeSheet());
        command->setManipulateColumns(true);
        command->add(*m_selection);
        if (!command->execute())
            delete command;
    }
#endif
}

void Tool::ColumnHeader::mouseDoubleClick(KoPointerEvent *)
{
#if 0
    register Sheet * const sheet = m_pCanvas->activeSheet();
    if (!sheet)
        return;

    if (sheet->isProtected())
        return;

    AdjustColumnRowManipulator* command = new AdjustColumnRowManipulator();
    command->setSheet(sheet);
    command->setAdjustColumn(true);
    command->add(*m_selection);
    command->execute();
#endif
}

void Tool::ColumnHeader::mouseMove(KoPointerEvent *_ev)
{
    const Sheet *const sheet = m_sheet;

    if (!sheet) {
        return;
    }
    qreal right = scrollOffset() + width();
    qreal ev_PosX = _ev->point.x();
    if (sheet->layoutDirection() == Qt::RightToLeft) {
        ev_PosX = right - ev_PosX;
    }
    // The button is pressed and we are resizing ?
    if (m_bResize) {
        if (ev_PosX < scrollOffset()) {
            m_HScrollBar->setValue(m_HScrollBar->value() - m_HScrollBar->singleStep());
        } else if (ev_PosX > right) {
            m_HScrollBar->setValue(m_HScrollBar->value() + m_HScrollBar->singleStep());
        }
        if (!sheet->isProtected()) {
            paintSizeIndicator(_ev);
        }
    } else if (m_selection && m_bSelection) {
        // The button is pressed and we are selecting
        qreal x;
        int col = sheet->leftColumn(ev_PosX, x);

        if (col > KS_colMax || col <= 0) {
            return;
        }
        QPoint newCursor = m_selection->cursor();
        newCursor.setX(col);
        m_selection->update(newCursor);

        if (sheet->layoutDirection() == Qt::RightToLeft) {
            //             if (_ev->pos().x() < width() - m_pCanvas->width()) {
            //                 const ColumnFormat *cl = sheet->columnFormat(col + 1);
            //                 x = sheet->columnPosition(col + 1);
            //                 m_HScrollBar->setValue(- (int)((ev_PosX + cl->width()) - right));
            //             } else if (_ev->pos().x() > width())
            //                 m_HScrollBar->setValue(- (ev_PosX - right + m_pCanvas->zoomHandler()->unzoomItX(m_pCanvas->width())));
        } else {
            if (_ev->pos().x() < 0)
                m_HScrollBar->setValue(ev_PosX);
            else if (_ev->pos().x() > /*m_pCanvas->*/ width()) {
                if (col < KS_colMax) {
                    const ColFormatStorage *cols = sheet->columnFormats();
                    x = sheet->columnPosition(col + 1);
                    m_HScrollBar->setValue(ev_PosX + cols->colWidth(col + 1) - right);
                }
            }
        }
    } else {
        // No button is pressed and the mouse is just moved
        qreal x;
        const qreal resizeWidth = resizeAreaSize();
        if (sheet->layoutDirection() == Qt::RightToLeft) {
            int tmpCol = sheet->leftColumn(0.0, x);

            while (ev_PosX > x && tmpCol <= KS_colMax) {
                double w = sheet->columnFormats()->visibleWidth(tmpCol);
                ++tmpCol;

                // if col is hide and it's the first column
                // you mustn't resize it.
                if (ev_PosX >= x + w - resizeWidth && ev_PosX <= x + w + resizeWidth && !(sheet->columnFormats()->isHiddenOrFiltered(tmpCol) && tmpCol == 0)) {
                    m_pCanvas->canvasWidget()->setCursor(Qt::SplitHCursor);
                    return;
                }
                x += w;
            }
            m_pCanvas->canvasWidget()->setCursor(Qt::ArrowCursor);
        } else {
            if (betweenColumns(ev_PosX)) {
                m_pCanvas->canvasWidget()->setCursor(Qt::SplitHCursor);
                return;
            }
            m_pCanvas->canvasWidget()->setCursor(Qt::ArrowCursor);
        }
    }
}

void Tool::ColumnHeader::paintSizeIndicator(KoPointerEvent *event)
{
    const Sheet *sheet = m_sheet;
    if (!sheet) {
        return;
    }
    const KoViewConverter *viewConverter = m_pCanvas->viewConverter();
    qreal columnLeft = sheet->columnPosition(m_iResizedColumn);
    m_iResizePos = event->point.x();

    bool hideColumn = false;
    if (m_iResizePos < columnLeft + viewConverter->viewToDocumentX(2)) {
        // Don't make the column have a height < 2 points.
        m_iResizePos = m_lastResizePos;
        hideColumn = true;
    }
    m_lastResizePos = m_iResizePos;
    if (!m_rubberband) {
        int y = viewConverter->documentToViewY(m_geometry.top());
        int h = viewConverter->documentToViewY(height() + m_sheetHeight);
        m_rubberband = new QRubberBand(QRubberBand::Line, m_pCanvas->canvasWidget());
        m_rubberband->setGeometry(0, y, 2, h);
        m_rubberband->show();
    }
    int xPos = viewConverter->documentToViewX(m_geometry.left() - scrollOffset() + (hideColumn ? columnLeft : m_iResizePos));
    m_rubberband->move(xPos, m_rubberband->geometry().top());
    QString tip;
    if (hideColumn) {
        tip = i18n("Hide Column");
    } else {
        tip = i18n("Width: %1 %2", toUnitUserValue(m_iResizePos - columnLeft), unitSymbol());
    }
    if (!m_lSize) {
        int screenNo = QApplication::desktop()->screenNumber(m_pCanvas->canvasWidget());
        m_lSize = new QLabel(QApplication::desktop()->screen(screenNo), Qt::ToolTip);
        m_lSize->setAlignment(Qt::AlignVCenter);
        m_lSize->setAutoFillBackground(true);
        m_lSize->setPalette(QToolTip::palette());
        m_lSize->setMargin(1 + m_pCanvas->canvasWidget()->style()->pixelMetric(QStyle::PM_ToolTipLabelFrameWidth, 0, m_lSize));
        m_lSize->setFrameShape(QFrame::Box);
        m_lSize->setIndent(1);
    }
    m_lSize->setText(tip);
    m_lSize->adjustSize();
    xPos = viewConverter->documentToViewX(m_geometry.left());
    xPos += viewConverter->documentToViewY(columnLeft - scrollOffset());
    int yPos = viewConverter->documentToViewX(m_geometry.top());
    QPoint pos = (sheet->layoutDirection() == Qt::RightToLeft) ? QPoint(xPos - 3 - m_lSize->width(), yPos - 3) : QPoint(xPos - 3, yPos - 3);
    pos -= QPoint(0, m_lSize->height());
    m_lSize->move(m_pCanvas->canvasWidget()->mapToGlobal(pos).x(), m_pCanvas->canvasWidget()->mapToGlobal(pos).y());
    m_lSize->show();
}

void Tool::ColumnHeader::removeSizeIndicator()
{
    delete m_rubberband;
    m_rubberband = nullptr;
}

void Tool::ColumnHeader::resize(const QSizeF &, const QSizeF &)
{
#if 0
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
#endif
}

QPalette Tool::ColumnHeader::palette() const
{
    return qApp->palette();
}

void Tool::ColumnHeader::paint(QPainter *painter, const QRectF &painterRect)
{
    const Sheet *sheet = m_sheet;
    if (!sheet) {
        return;
    }

    //     ElapsedTime et( "Painting horizontal header", ElapsedTime::PrintOnlyTime );

    // FIXME Stefan: Make use of clipping. Find the repaint call after the scrolling.
    // debugSheetsRender << event->rect();

    // painting rectangle
    const QRectF paintRect = painterRect;

    // the painter
    painter->setRenderHint(QPainter::TextAntialiasing);

    // fonts
    QFont normalFont(m_font);
    QFont boldFont(normalFont);
    boldFont.setBold(true);

    // background brush/color
    const QBrush backgroundBrush(palette().window());
    const QColor backgroundColor(backgroundBrush.color());

    // selection brush/color
    QColor selectionColor(palette().highlight().color());
    selectionColor.setAlpha(127);
    const QBrush selectionBrush(selectionColor);

    qreal xPos = 0.0;
    int x = 0;

    if (sheet->layoutDirection() == Qt::RightToLeft) {
        // Get the left column and the current x-position
        x = sheet->leftColumn(int(width() - paintRect.x()), xPos);
    } else {
        // Get the left column and the current x-position
        x = sheet->leftColumn(int(paintRect.x()), xPos);
    }

    const qreal height = this->height() - 1;

    bool useHighlight = (m_selection && !m_selection->referenceSelectionMode());

    int deltaX = 1;
    if (sheet->layoutDirection() == Qt::RightToLeft) {
        if (x > KS_colMax) {
            x = KS_colMax;
        }
        xPos -= sheet->columnFormats()->colWidth(x);
        deltaX = -1;
    }

    // Loop through the columns, until we are out of range
    while (xPos <= paintRect.right() && x <= KS_colMax) {
        const ColFormatStorage *cols = sheet->columnFormats();
        if (cols->isHiddenOrFiltered(x)) {
            ++x;
            continue;
        }

        bool selected = false;
        bool highlighted = false;
        if (useHighlight) {
            selected = m_selection->isColumnSelected(x);
            highlighted = m_selection->isColumnAffected(x);
        }

        const qreal width = cols->colWidth(x);
        const QRectF rect(xPos, 0, width, height);

        if (selected || highlighted) {
            painter->setPen(QPen(selectionColor.darker(150), 0));
            painter->setBrush(selectionBrush);
        } else {
            painter->setPen(QPen(backgroundColor.darker(150), 0));
            painter->setBrush(backgroundBrush);
        }
        painter->drawRect(rect);

        // Reset painter
        painter->setFont(normalFont);
        painter->setPen(palette().text().color());

        if (selected) {
            painter->setPen(palette().highlightedText().color());
        } else if (highlighted) {
            painter->setFont(boldFont);
        }
        QString colText = sheet->getShowColumnNumber() ? QString::number(x) : CellBase::columnName(x);
        QFontMetricsF fm(painter->font());
        if (width < fm.width(colText)) {
            // try to scale down the font to make it fit
            QFont font = painter->font();
            qreal maxSize = font.pointSizeF();
            qreal minSize = maxSize / 2;
            while (minSize > 1) {
                font.setPointSizeF(minSize);
                const QFontMetricsF fm2(font);
                if (width >= fm2.width(colText))
                    break;
                minSize /= 2;
            }
            while (minSize < 0.99 * maxSize) {
                qreal middle = (maxSize + minSize) / 2;
                font.setPointSizeF(middle);
                const QFontMetricsF fm2(font);
                if (width >= fm2.width(colText)) {
                    minSize = middle;
                } else {
                    maxSize = middle;
                }
            }
            painter->setFont(font);
            fm = QFontMetricsF(font);
        }
        if (width >= fm.width(colText)) {
#if 0
            switch (x % 3) {
            case 0: colText = QString::number(height) + 'h'; break;
            case 1: colText = QString::number(fm.ascent()) + 'a'; break;
            case 2: colText = QString::number(fm.descent()) + 'd'; break;
            }
#endif
            painter->drawText(rect, Qt::AlignCenter, colText);
        }
        xPos += width;
        x += deltaX;
    }
}

void Tool::ColumnHeader::setHeaderFont(const QFont &font)
{
    m_font = font;
}

QFont Tool::ColumnHeader::headerFont() const
{
    return m_font;
}

qreal Tool::ColumnHeader::toUnitUserValue(qreal value)
{
    return m_pCanvas->unit().toUserValue(value);
}

QString Tool::ColumnHeader::unitSymbol()
{
    return m_pCanvas->unit().symbol();
}

void Tool::ColumnHeader::setCanvas(KoCanvasBase *canvas)
{
    m_pCanvas = canvas;
}

void Tool::ColumnHeader::setSheetHeight(qreal height)
{
    m_sheetHeight = height;
}

qreal Tool::ColumnHeader::scrollOffset() const
{
    return (qreal)m_HScrollBar->value();
}
