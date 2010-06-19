/* This file is part of the KDE project
   Copyright 2010 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>
   Copyright 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
   Copyright 2004-2005 Tomas Mecir <mecirt@gmail.com>
   Copyright 2004-2006 Inge Wallin <inge@lysator.liu.se>
   Copyright 1999-2002,2004,2005 Laurent Montel <montel@kde.org>
   Copyright 2002-2005 Ariya Hidayat <ariya@kde.org>
   Copyright 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2003 Reinhart Geiser <geiseri@kde.org>
   Copyright 2003-2005 Meni Livne <livne@kde.org>
   Copyright 2003 Peter Simonsson <psn@linux.se>
   Copyright 1999-2002 David Faure <faure@kde.org>
   Copyright 2000-2002 Werner Trobin <trobin@kde.org>
   Copyright 1999,2002 Harri Porten <porten@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 1998-2000 Torben Weis <weis@kde.org>
   Copyright 2000 Bernd Wuebben <wuebben@kde.org>
   Copyright 2000 Simon Hausmann <hausmann@kde.org
   Copyright 1999 Stephan Kulow <coolo@kde.org>
   Copyright 1999 Michael Reiher <michael.reiher@gmx.de>
   Copyright 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>
   Copyright 1998-1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

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
#include "CellView.h"

// Qt
#include <QApplication>
#include <QColor>
#include <QPainter>
#include <QRectF>
#include <QStyleOptionComboBox>
#include <QTextLayout>
#include <QTextCursor>
#include <QAbstractTextDocumentLayout>

// KDE
#include <KColorUtils>

// KOffice
#include <KoPostscriptPaintDevice.h>
#include <KoZoomHandler.h>

// KSpread
#include "ApplicationSettings.h"
#include "CellStorage.h"
#include "Condition.h"
#include "Map.h"
#include "PrintSettings.h"
#include "RowColumnFormat.h"
#include "Selection.h"
#include "Sheet.h"
#include "SheetPrint.h"
#include "SheetView.h"
#include "StyleManager.h"
#include "Value.h"
#include "ValueFormatter.h"

using namespace KSpread;

const int s_borderSpace = 1;

class CellView::Private : public QSharedData
{
public:
    Private(Style* defaultStyle, qreal defaultWidth, qreal defaultHeight)
            : style(*defaultStyle)
            , width(defaultWidth)
            , height(defaultHeight)
            , textX(0.0)
            , textY(0.0)
            , textWidth(0.0)
            , textHeight(0.0)
            , textLinesCount(0)
            , shrinkToFitFontSize(0.0)
            , hidden(false)
            , merged(false)
            , obscured(false)
            , fittingHeight(true)
            , fittingWidth(true)
            , filterButton(false)
            , obscuredCellsX(0)
            , obscuredCellsY(0)
            , richText(0) {}
    ~Private() {
    }

    Style style;
    qreal  width;
    qreal  height;

    // Position and dimension of displayed text.
    // Doc coordinate system; points; no zoom
    qreal  textX;
    qreal  textY;
    qreal  textWidth;
    qreal  textHeight;

    int    textLinesCount;
    qreal shrinkToFitFontSize;

    bool hidden         : 1;
    bool merged         : 1;
    bool obscured       : 1;
    bool fittingHeight  : 1;
    bool fittingWidth   : 1;
    bool filterButton   : 1;
    // NOTE Stefan: A cell is either obscured by an other one or obscures others itself.
    //              But never both at the same time, so we can share the memory for this.
    union {
    int obscuringCellX : 16; // KS_colMax
    int obscuredCellsX : 16; // KS_colMax
    };
    union {
    int obscuringCellY : 16; // KS_rowMax
    int obscuredCellsY : 16; // KS_rowMax
    };

    // This is the text we want to display. Not necessarily the same
    // as the user input, e.g. Cell::userInput()="1" and displayText="1.00".
    QString displayText;
    QSharedPointer<QTextDocument> richText;
public:
    void checkForFilterButton(const Cell&);
    void calculateTextSize(const QFont& font, const QFontMetricsF& fontMetrics);
    void calculateHorizontalTextSize(const QFont& font, const QFontMetricsF& fontMetrics);
    void calculateVerticalTextSize(const QFont& font, const QFontMetricsF& fontMetrics);
    void calculateAngledTextSize(const QFont& font, const QFontMetricsF& fontMetrics);
    void calculateRichTextSize(const QFont& font, const QFontMetricsF& fontMetrics);
    void truncateText(const QFont& font, const QFontMetricsF& fontMetrics);
    void truncateHorizontalText(const QFont& font, const QFontMetricsF& fontMetrics);
    void truncateVerticalText(const QFont& font, const QFontMetricsF& fontMetrics);
    void truncateAngledText(const QFont& font, const QFontMetricsF& fontMetrics);
    QFont calculateFont() const;
    QTextOption textOptions() const;
};

QFont CellView::Private::calculateFont() const
{
    QFont f = style.font();
    if (shrinkToFitFontSize > 0.0)
        f.setPointSizeF(shrinkToFitFontSize);
    return f;
}

CellView::CellView(SheetView* sheetView)
        : d(new Private(sheetView->sheet()->map()->styleManager()->defaultStyle(),
                        sheetView->sheet()->map()->defaultColumnFormat()->width(),
                        sheetView->sheet()->map()->defaultRowFormat()->height()))
{
}

CellView::CellView(SheetView* sheetView, int col, int row)
        : d(sheetView->defaultCellView().d)
{
    Q_ASSERT(1 <= col && col <= KS_colMax);
    Q_ASSERT(1 <= row && row <= KS_rowMax);

    const Sheet* sheet = sheetView->sheet();
    Cell cell(sheet, col, row);

    // create the effective style
    if (cell.isPartOfMerged()) {
        d->merged = true;
        Cell masterCell = cell.masterCell();
        d->style = sheetView->cellView(masterCell.column(), masterCell.row()).style();
    } else {
        // lookup the 'normal' style
        Style style = cell.style();
        if (!style.isDefault())
            d->style = style;

        // use conditional formatting attributes
        Conditions conditions = cell.conditions();
        if (Style* style = conditions.testConditions(cell, sheetView->sheet()->map()->styleManager()))
            d->style.merge(*style);
    }

    if (cell.width() != sheetView->sheet()->map()->defaultColumnFormat()->width())
        d->width = cell.width();
    if (cell.height() != sheetView->sheet()->map()->defaultRowFormat()->height())
        d->height = cell.height();

    if (sheet->columnFormat(col)->isHiddenOrFiltered() ||
            sheet->rowFormat(row)->isHiddenOrFiltered() ||
            (sheet->columnFormat(col)->width() <= sheetView->viewConverter()->viewToDocumentY(2)) ||
            (sheet->rowFormat(row)->height() <= sheetView->viewConverter()->viewToDocumentY(2))) {
        d->hidden = true;
        d->height = 0.0;
        d->width = 0.0;
        return; // nothing more to do
    }

    d->checkForFilterButton(cell);

    // do not touch the other Private members, just return here.
    if (cell.isDefault()) return;

    Value value;
    // Display a formula if warranted.  If not, simply display the value.
    if (cell.isFormula() && cell.sheet()->getShowFormula() &&
            !(cell.sheet()->isProtected() && d->style.hideFormula())) {
        d->displayText = cell.userInput();
        value.setFormat(Value::fmt_String);
    } else if (!cell.isEmpty()) {
        // Format the value appropriately and set the display text.
        // The format of the resulting value is used below to determine the alignment.
        value = sheet->map()->formatter()->formatText(cell.value(), d->style.formatType(),
                d->style.precision(), d->style.floatFormat(),
                d->style.prefix(), d->style.postfix(),
                d->style.currency().symbol(), d->style.customFormat());
        d->displayText = value.asString();

        QSharedPointer<QTextDocument> doc = cell.richText();
        if (!doc.isNull())
            d->richText = QSharedPointer<QTextDocument>(doc->clone());
    }

    // Hide zero.
    if (sheet->getHideZero() && cell.value().isNumber() && cell.value().asFloat() == 0.0)
        d->displayText.clear();

    // If text is empty, there's nothing more to do.
    if (d->displayText.isEmpty())
        return;

    // horizontal align
    if (d->style.halign() == Style::HAlignUndefined) {
        // errors are always centered
        if (cell.value().type() == Value::Error)
            d->style.setHAlign(Style::Center);
        // if the format is text, align it according to the text direction
        else if (d->style.formatType() == Format::Text || value.format() == Value::fmt_String)
            d->style.setHAlign(d->displayText.isRightToLeft() ? Style::Right : Style::Left);
        // if the style does not define a specific format, align it according to the sheet layout
        else
            d->style.setHAlign(cell.sheet()->layoutDirection() == Qt::RightToLeft ? Style::Left : Style::Right);
    }
    // force left alignment, if there's a formula and it should be shown
    if (cell.isFormula() && sheet->getShowFormula() && !(sheet->isProtected() && d->style.hideFormula()))
        d->style.setHAlign(Style::Left);

    makeLayout(sheetView, cell);
}

CellView::CellView(const CellView& other)
        : d(other.d)
{
}

CellView::~CellView()
{
}

Style CellView::style() const
{
    return d->style;
}

qreal CellView::textWidth() const
{
    return d->textWidth;
}

qreal CellView::textHeight() const
{
    return d->textHeight;
}

QRectF CellView::textRect() const
{
    return QRectF(d->textX, d->textY, d->textWidth, d->textWidth);
}

QString CellView::testAnchor(const Cell& cell, qreal x, qreal y) const
{
    if (cell.link().isEmpty())
        return QString();

    if (x > d->textX) if (x < d->textX + d->textWidth)
            if (y > d->textY - d->textHeight) if (y < d->textY)
                    return cell.link();

    return QString();
}

bool CellView::hitTestFilterButton(const Cell& cell, const QRect& cellRect, const QPoint& position) const
{
    if (!d->filterButton)
        return false;

    QStyleOptionComboBox options;
    options.direction = cell.sheet()->layoutDirection();
    options.editable = true;
//     options.fontMetrics = painter.fontMetrics();
    options.frame = false;
    options.rect = cellRect;
//     options.subControls = QStyle::SC_ComboBoxEditField | QStyle::SC_ComboBoxArrow;

    return QApplication::style()->hitTestComplexControl(QStyle::CC_ComboBox, &options, position) == QStyle::SC_ComboBoxArrow;
}

// ================================================================
//                            Painting


// Paint the cell.  This is the main function that calls a lot of
//                  helper functions.
//
// `paintRect'  is the rectangle that we should paint on in document coordinates.
//              If the cell does not overlap this, we can return immediately.
// `coordinate' is the origin (the upper left) of the cell in document
//              coordinates.
//
void CellView::paintCellContents(const QRectF& paintRect, QPainter& painter,
                                 QPaintDevice* paintDevice, const QPointF& coordinate,
                                 const Cell& cell, SheetView* sheetView)
{
    if (d->hidden)
        return;
    if (d->merged)
        return;
    if (d->obscured)
        return;

    // ----------------  Start the actual painting.  ----------------

    // If the rect of this cell doesn't intersect the rect that should
    // be painted, we can skip the rest and return. (Note that we need
    // to calculate `left' first before we can do this.)
    const QRectF cellRect(coordinate, QSizeF(d->width, d->height));
    if (!cellRect.intersects(paintRect))
        return;
    // Does the cell intersect the clipped painting region?
    if (!painter.clipRegion().intersects(cellRect.toRect()))
        return;

    // 0. Paint possible filter button
    if (d->filterButton && !dynamic_cast<QPrinter*>(painter.device()))
        paintFilterButton(painter, coordinate, cell, sheetView);

    // 1. Paint possible comment indicator.
    if (!dynamic_cast<QPrinter*>(painter.device())
            || cell.sheet()->printSettings()->printCommentIndicator())
        paintCommentIndicator(painter, coordinate, cell);

    // 2. Paint possible formula indicator.
    if (!dynamic_cast<QPrinter*>(painter.device())
            || cell.sheet()->printSettings()->printFormulaIndicator()) {
        paintFormulaIndicator(painter, coordinate, cell);
        paintMatrixElementIndicator(painter, coordinate, cell);
    }

    // 3. Paint possible indicator for clipped text.
    paintMoreTextIndicator(painter, coordinate);

    // 5. Paint the text in the cell unless:
    //  a) it is empty
    //  b) something indicates that the text should not be painted
    //  c) the sheet is protected and the cell is hidden.
    if (!d->displayText.isEmpty()
            && (!dynamic_cast<QPrinter*>(painter.device()) || style().printText())
            && !(cell.sheet()->isProtected()
                 && style().hideAll())) {
        paintText(painter, coordinate, paintDevice, cell);
    }
}

void CellView::paintCellBorders(const QRectF& paintRegion, QPainter& painter,
                                const QPointF& coordinate,
                                const QRect& cellRegion,
                                const Cell& cell, SheetView* sheetView)
{
    // If the rect of this cell doesn't intersect the rect that should
    // be painted, we can skip the rest and return. (Note that we need
    // to calculate `left' first before we can do this.)
    const QRectF  cellRect(coordinate.x(), coordinate.y(), d->width, d->height);
    if (!cellRect.intersects(paintRegion))
        return;
    // Does the cell intersect the clipped painting region?
    if (!painter.clipRegion().intersects(cellRect.toRect()))
        return;

    const int col = cell.column();
    const int row = cell.row();

    CellView::Borders paintBorder = CellView::NoBorder;

    // borders
    // NOTE Stefan: the borders of the adjacent cells are taken for the case,
    //              that the cell is located on the edge of the cell range,
    //              that is painted.
    // NOTE Sebsauer: this won't work for merged cells
    if (col == 1)
        paintBorder |= LeftBorder;
    else if (d->style.leftPenValue() < sheetView->cellView(col - 1, row).style().rightPenValue())
        d->style.setLeftBorderPen(sheetView->cellView(col - 1, row).style().rightBorderPen());
    else // if ( d->style.leftPenValue() >= sheetView->cellView( col - 1, row ).style().rightPenValue() )
        paintBorder |= LeftBorder;

    if (col == KS_colMax)
        paintBorder |= CellView::RightBorder;
    else if (d->style.rightPenValue() < sheetView->cellView(col + 1, row).style().leftPenValue())
        d->style.setRightBorderPen(sheetView->cellView(col + 1, row).style().leftBorderPen());
    else // if (d->style.rightPenValue() > sheetView->cellView(col + 1, row).style().leftPenValue())
        paintBorder |= CellView::RightBorder;

    if (row == 1)
        paintBorder |= TopBorder;
    else if (d->style.topPenValue() < sheetView->cellView(col, row - 1).style().bottomPenValue())
        d->style.setTopBorderPen(sheetView->cellView(col, row - 1).style().bottomBorderPen());
    else // if ( d->style.topPenValue() >= sheetView->cellView( col, row - 1 ).style().bottomPenValue() )
        paintBorder |= TopBorder;

    if (row == KS_rowMax)
        paintBorder |= BottomBorder;
    else if (d->style.bottomPenValue() < sheetView->cellView(col, row + 1).style().topPenValue())
        d->style.setBottomBorderPen(sheetView->cellView(col, row + 1).style().topBorderPen());
    else // if (d->style.bottomPenValue() >= sheetView->cellView(col, row + 1).style().topPenValue())
        paintBorder |= BottomBorder;

    // Paint border if outermost cell or if the pen is more "worth"
    // than the border pen of the cell on the other side of the
    // border or if the cell on the other side is not painted. In
    // the latter case get the pen that is of more "worth"
    if (col == cellRegion.right())
        paintBorder |= CellView::RightBorder;
    if (row == cellRegion.bottom())
        paintBorder |= CellView::BottomBorder;
    if (col == cellRegion.left())
        paintBorder |= CellView::LeftBorder;
    if (row == cellRegion.top())
        paintBorder |= CellView::TopBorder;

    // ----------------  Start the actual painting.  ----------------

    // 2. Paint the borders of the cell if no other cell is forcing this
    // one, i.e. this cell is not part of a merged cell.
    //

    // If we print pages, then we disable clipping, otherwise borders are
    // cut in the middle at the page borders.
    if (dynamic_cast<QPrinter*>(painter.device()))
        painter.setClipping(false);

    // Paint the borders if this cell is not part of another merged cell.
    if (!d->merged) {
        paintCustomBorders(painter, paintRegion, coordinate, paintBorder);
    }

    // Turn clipping back on.
    if (dynamic_cast<QPrinter*>(painter.device()))
        painter.setClipping(true);

    // 3. Paint diagonal lines and page borders.
    paintCellDiagonalLines(painter, coordinate);
    paintPageBorders(painter, coordinate, paintBorder, cell);
}

//
// Paint the background of this cell.
//
void CellView::paintCellBackground(QPainter& painter, const QPointF& coordinate)
{
    if (d->merged)
        return;

    const QRectF cellRect = QRectF(coordinate, QSizeF(d->width, d->height));
    // Does the cell intersect the clipped painting region?
    if (!painter.clipRegion().intersects(cellRect.toRect()))
        return;

    // disable antialiasing
    painter.setRenderHint(QPainter::Antialiasing, false);

    if (d->style.backgroundColor().isValid() &&
            d->style.backgroundColor() != QApplication::palette().base().color()) {
        // Simply fill the cell with its background color,
        painter.fillRect(cellRect, d->style.backgroundColor());
    }

    // restore antialiasing
    painter.setRenderHint(QPainter::Antialiasing, true);

    if (d->style.backgroundBrush().style() != Qt::NoBrush) {
        // Draw the background pattern.
        painter.fillRect(cellRect, d->style.backgroundBrush());
    }
}


// Paint the standard light grey borders that are always visible.
//
void CellView::paintDefaultBorders(QPainter& painter, const QRectF& paintRect,
                                   const QPointF &coordinate,
                                   Borders paintBorder, const QRect& cellRegion,
                                   const Cell& cell, SheetView* sheetView)
{
    // Should the default borders be shown?
    if (!cell.sheet()->getShowGrid())
        return;
    // Does the cell intersect the clipped painting region?
    if (!painter.clipRegion().intersects(QRectF(coordinate, QSizeF(d->width, d->height)).toRect()))
        return;
    // Don't draw the border if a background fill-color was define.
    if (d->style.backgroundColor().isValid())
        return;
    // disable antialiasing
    painter.setRenderHint(QPainter::Antialiasing, false);

    /*
    *** Notes about optimization ***

    This function was painting the top, left, right & bottom lines in almost
    all cells previously, contrary to what the comment below says should happen.
    There doesn't appear to be a UI option to enable or disable showing of the
    grid when printing at the moment, so I have disabled drawing of right and
    bottom borders for all cells.

    I also couldn't work out under what conditions the variables dt / db would
    come out as anything other than 0 in the code for painting the various borders.
    The cell.effTopBorderPen / cell.effBottomBorderPen calls were taking
    up a lot of time according some profiling I did.  If that code really is
    necessary, we need to find a more efficient way of getting the widths than
    grabbing the whole QPen object and asking it.

    --Robert Knight (robertknight@gmail.com)
    */
    const bool paintingToExternalDevice = dynamic_cast<QPrinter*>(painter.device());

    const int col = cell.column();
    const int row = cell.row();

    paintBorder = CellView::NoBorder;

    // borders
    // Paint border if outermost cell or if the pen is more "worth"
    // than the border pen of the cell on the other side of the
    // border or if the cell on the other side is not painted. In
    // the latter case get the pen that is of more "worth"

    // Each cell is responsible for drawing it's top and left portions
    // of the "default" grid. --Or not drawing it if it shouldn't be
    // there.  It's also responsible to paint the right and bottom, if
    // it is the last cell on a print out.

    // NOTE Stefan: the borders of the adjacent cells are taken for the case,
    //              that the cell is located on the edge of the cell range,
    //              that is painted.
    if (col == 1)
        paintBorder |= LeftBorder;
    else if (d->style.leftPenValue() < sheetView->cellView(col - 1, row).style().rightPenValue())
        d->style.setLeftBorderPen(sheetView->cellView(col - 1, row).style().rightBorderPen());
    else // if ( d->style.leftPenValue() >= sheetView->cellView( col - 1, row ).style().rightPenValue() )
        paintBorder |= LeftBorder;
    if (col == KS_colMax)
        paintBorder |= CellView::RightBorder;
    else if (d->style.rightPenValue() < sheetView->cellView(col + cell.mergedXCells(), row).style().leftPenValue())
        d->style.setRightBorderPen(sheetView->cellView(col + cell.mergedXCells(), row).style().leftBorderPen());
    else if (d->style.rightPenValue() > sheetView->cellView(col + cell.mergedXCells(), row).style().leftPenValue())
        paintBorder |= CellView::RightBorder;
    if (row == 1)
        paintBorder |= TopBorder;
    else if (d->style.topPenValue() < sheetView->cellView(col, row - 1).style().bottomPenValue())
        d->style.setTopBorderPen(sheetView->cellView(col, row - 1).style().bottomBorderPen());
    else // if ( d->style.topPenValue() >= sheetView->cellView( col, row - 1 ).style().bottomPenValue() )
        paintBorder |= TopBorder;
    if (row == KS_rowMax)
        paintBorder |= BottomBorder;
    else if (d->style.bottomPenValue() < sheetView->cellView(col, row + cell.mergedYCells()).style().topPenValue())
        d->style.setBottomBorderPen(sheetView->cellView(col, row + cell.mergedYCells()).style().topBorderPen());
    else if (d->style.bottomPenValue() >= sheetView->cellView(col, row + cell.mergedYCells()).style().topPenValue())
        paintBorder |= BottomBorder;

    // Check merging...
    if (d->merged) {
        // by default: none ...
        paintBorder = NoBorder;
        // left and top, only if it's the left or top of the merged cell
        if (cell.column() == cell.masterCell().column())
            paintBorder |= LeftBorder;
        else if (cell.row() == cell.masterCell().row())
            paintBorder |= TopBorder;
        // right and bottom only, if it's the outermost border of the cell region being painted
        // checked later below...
    }

    // Check obscuring...
    if (isObscured()) {
        // by default: none ...
        paintBorder = NoBorder;
        // left and top, only if it's the left or top of the obscuring cell
        const QPoint obscuringCell = this->obscuringCell();
        if (cell.column() == obscuringCell.x())
            paintBorder |= LeftBorder;
        else if (cell.row() == obscuringCell.y())
            paintBorder |= TopBorder;
        // right and bottom only, if it's the outermost border of the cell region being painted
        // checked later below...
    }

    // Force painting, if it's the outermost border of the cell region being painted...
    if (col == cellRegion.right())
        paintBorder |= CellView::RightBorder;
    if (row == cellRegion.bottom())
        paintBorder |= CellView::BottomBorder;
    if (col == cellRegion.left())
        paintBorder |= CellView::LeftBorder;
    if (row == cellRegion.top())
        paintBorder |= CellView::TopBorder;

    // Check, if a custom border exists and the default border is not necessary...
    if (d->style.leftBorderPen().style() != Qt::NoPen)
        paintBorder &= ~LeftBorder;
    if (d->style.topBorderPen().style() != Qt::NoPen)
        paintBorder &= ~TopBorder;
    if (d->style.rightBorderPen().style() != Qt::NoPen)
        paintBorder &= ~RightBorder;
    if (d->style.bottomBorderPen().style() != Qt::NoPen)
        paintBorder &= ~BottomBorder;

    // Check if the neighbor-cells do have a background fill-color in which case the border is not drawn.
    if(col > 1 && sheetView->cellView(col - 1, row).style().backgroundColor().isValid())
        paintBorder &= ~LeftBorder;
    if(col < KS_colMax && sheetView->cellView(col + 1, row).style().backgroundColor().isValid())
        paintBorder &= ~RightBorder;
    if(row > 1 && sheetView->cellView(col, row - 1).style().backgroundColor().isValid())
        paintBorder &= ~TopBorder;
    if(row < KS_rowMax && sheetView->cellView(col, row + 1).style().backgroundColor().isValid())
        paintBorder &= ~BottomBorder;

    // Set the single-pixel width pen for drawing the borders with.
    // NOTE Stefan: Use a cosmetic pen (width = 0), because we want the grid always one pixel wide
    painter.setPen(QPen(cell.sheet()->map()->settings()->gridColor(), 0, Qt::SolidLine));

    QLineF line;

    // The left border.
    if (paintBorder & LeftBorder) {
        int dt = 0;
        int db = 0;

#if 0
        if (cellRef.x() > 1) {
            Cell  *cell_west = Cell(cell.sheet(), cellRef.x() - 1,
                                    cellRef.y());
            QPen t = cell_west->effTopBorderPen(cellRef.x() - 1, cellRef.y());
            QPen b = cell_west->effBottomBorderPen(cellRef.x() - 1, cellRef.y());

            if (t.style() != Qt::NoPen)
                dt = (t.width() + 1) / 2;
            if (b.style() != Qt::NoPen)
                db = (t.width() / 2);
        }
#endif

        // If we are on paper printout, we limit the length of the lines.
        // On paper, we always have full cells, on screen not.
        if (paintingToExternalDevice) {
            line = QLineF(qMax(paintRect.left(),   coordinate.x()),
                          qMax(paintRect.top(),    coordinate.y() + dt),
                          qMin(paintRect.right(),  coordinate.x()),
                          qMin(paintRect.bottom(), coordinate.y() + d->height - db));
        } else {
            line = QLineF(coordinate.x(),
                          coordinate.y() + dt,
                          coordinate.x(),
                          coordinate.y() + d->height - db);
        }
        painter.drawLine(line);
    }


    // The top border.
    if (paintBorder & TopBorder) {
        int dl = 0;
        int dr = 0;

#if 0
        if (cellRef.y() > 1) {
            Cell  *cell_north = Cell(cell.sheet(), cellRef.x(),
                                     cellRef.y() - 1);

            QPen l = cell_north->effLeftBorderPen(cellRef.x(), cellRef.y() - 1);
            QPen r = cell_north->effRightBorderPen(cellRef.x(), cellRef.y() - 1);

            if (l.style() != Qt::NoPen)
                dl = (l.width() - 1) / 2 + 1;
            if (r.style() != Qt::NoPen)
                dr = r.width() / 2;
        }
#endif

        // If we are on paper printout, we limit the length of the lines.
        // On paper, we always have full cells, on screen not.
        if (paintingToExternalDevice) {
            line = QLineF(qMax(paintRect.left(),   coordinate.x() + dl),
                          qMax(paintRect.top(),    coordinate.y()),
                          qMin(paintRect.right(),  coordinate.x() + d->width - dr),
                          qMin(paintRect.bottom(), coordinate.y()));
        } else {
            line = QLineF(coordinate.x() + dl,
                          coordinate.y(),
                          coordinate.x() + d->width - dr,
                          coordinate.y());
        }
        painter.drawLine(line);
    }


    // The right border.
    if (paintBorder & RightBorder) {
        int dt = 0;
        int db = 0;

#if 0
        if (cellRef.x() < KS_colMax) {
            Cell  *cell_east = Cell(cell.sheet(), cellRef.x() + 1,
                                    cellRef.y());

            QPen t = cell_east->effTopBorderPen(cellRef.x() + 1, cellRef.y());
            QPen b = cell_east->effBottomBorderPen(cellRef.x() + 1, cellRef.y());

            if (t.style() != Qt::NoPen)
                dt = (t.width() + 1) / 2;
            if (b.style() != Qt::NoPen)
                db = (t.width() / 2);
        }
#endif

        //painter.setPen( QPen( cell.sheet()->map()->settings()->gridColor(), 1, Qt::SolidLine ) );

        // If we are on paper printout, we limit the length of the lines.
        // On paper, we always have full cells, on screen not.
        if (dynamic_cast<QPrinter*>(painter.device())) {
            line = QLineF(qMax(paintRect.left(),   coordinate.x() + d->width),
                          qMax(paintRect.top(),    coordinate.y() + dt),
                          qMin(paintRect.right(),  coordinate.x() + d->width),
                          qMin(paintRect.bottom(), coordinate.y() + d->height - db));
        } else {
            line = QLineF(coordinate.x() + d->width,
                          coordinate.y() + dt,
                          coordinate.x() + d->width,
                          coordinate.y() + d->height - db);
        }
        painter.drawLine(line);
    }

    // The bottom border.
    if (paintBorder & BottomBorder) {
        int dl = 0;
        int dr = 0;
#if 0
        if (cellRef.y() < KS_rowMax) {
            Cell  *cell_south = Cell(cell.sheet(), cellRef.x(),
                                     cellRef.y() + 1);

            QPen l = cell_south->effLeftBorderPen(cellRef.x(), cellRef.y() + 1);
            QPen r = cell_south->effRightBorderPen(cellRef.x(), cellRef.y() + 1);

            if (l.style() != Qt::NoPen)
                dl = (l.width() - 1) / 2 + 1;
            if (r.style() != Qt::NoPen)
                dr = r.width() / 2;
        }
#endif

        // If we are on paper printout, we limit the length of the lines.
        // On paper, we always have full cells, on screen not.
        if (dynamic_cast<QPrinter*>(painter.device())) {
            line = QLineF(qMax(paintRect.left(),   coordinate.x() + dl),
                          qMax(paintRect.top(),    coordinate.y() + d->height),
                          qMin(paintRect.right(),  coordinate.x() + d->width - dr),
                          qMin(paintRect.bottom(), coordinate.y() + d->height));
        } else {
            line = QLineF(coordinate.x() + dl,
                          coordinate.y() + d->height,
                          coordinate.x() + d->width - dr,
                          coordinate.y() + d->height);
        }
        painter.drawLine(line);
    }

    // restore antialiasing
    painter.setRenderHint(QPainter::Antialiasing, true);
}


// Paint a comment indicator if the cell has a comment.
//
void CellView::paintCommentIndicator(QPainter& painter,
                                     const QPointF& coordinate,
                                     const Cell& cell)
{
    // Point the little corner if there is a comment attached
    // to this cell.
    if ((!cell.comment().isEmpty())
            && d->width > 10.0
            && d->height > 10.0
            && (cell.sheet()->printSettings()->printCommentIndicator()
                || (!dynamic_cast<QPrinter*>(painter.device()) && cell.sheet()->getShowCommentIndicator()))) {
        QColor penColor = Qt::red;

        // If background has high red part, switch to blue.
        if (qRed(d->style.backgroundColor().rgb()) > 127 &&
                qGreen(d->style.backgroundColor().rgb()) < 80 &&
                qBlue(d->style.backgroundColor().rgb()) < 80) {
            penColor = Qt::blue;
        }

        // Get the triangle.
        QPolygonF polygon(3);
        polygon.clear();
        if (cell.sheet()->layoutDirection() == Qt::RightToLeft) {
            polygon << QPointF(coordinate.x() + 6.0, coordinate.y());
            polygon << QPointF(coordinate.x(), coordinate.y());
            polygon << QPointF(coordinate.x(), coordinate.y() + 6.0);
        } else {
            polygon << QPointF(coordinate.x() + cell.width() - 5.0, coordinate.y());
            polygon << QPointF(coordinate.x() + cell.width(), coordinate.y());
            polygon << QPointF(coordinate.x() + cell.width(), coordinate.y() + 5.0);
        }

        // And draw it.
        painter.setBrush(QBrush(penColor));
        painter.setPen(Qt::NoPen);
        painter.drawPolygon(polygon);
    }
}


// Paint a small rectangle if this cell holds a formula.
//
void CellView::paintFormulaIndicator(QPainter& painter,
                                     const QPointF& coordinate,
                                     const Cell& cell)
{
    if (cell.isFormula() &&
            cell.sheet()->getShowFormulaIndicator() &&
            d->width  > 10.0 &&
            d->height > 10.0) {
        QColor penColor = Qt::blue;
        // If background has high blue part, switch to red.
        if (qRed(d->style.backgroundColor().rgb()) < 80 &&
                qGreen(d->style.backgroundColor().rgb()) < 80 &&
                qBlue(d->style.backgroundColor().rgb()) > 127) {
            penColor = Qt::red;
        }

        // Get the triangle...
        QPolygonF polygon(3);
        polygon.clear();
        if (cell.sheet()->layoutDirection() == Qt::RightToLeft) {
            polygon << QPointF(coordinate.x() + d->width - 6.0, coordinate.y() + d->height);
            polygon << QPointF(coordinate.x() + d->width, coordinate.y() + d->height);
            polygon << QPointF(coordinate.x() + d->width, coordinate.y() + d->height - 6.0);
        } else {
            polygon << QPointF(coordinate.x(), coordinate.y() + d->height - 6.0);
            polygon << QPointF(coordinate.x(), coordinate.y() + d->height);
            polygon << QPointF(coordinate.x() + 6.0, coordinate.y() + d->height);
        }

        // ...and draw it.
        painter.setBrush(QBrush(penColor));
        painter.setPen(Qt::NoPen);
        painter.drawPolygon(polygon);
    }
}


// Paint a small rectangle if this cell is an element of a matrix.
//
void CellView::paintMatrixElementIndicator(QPainter& painter,
        const QPointF& coordinate,
        const Cell& cell)
{
    if (cell.isLocked() &&
            cell.sheet()->getShowFormulaIndicator() &&
            d->width  > 10.0 &&
            d->height > 10.0) {
        QColor penColor = Qt::blue;
        // If background has high blue part, switch to red.
        if (qRed(d->style.backgroundColor().rgb()) < 80 &&
                qGreen(d->style.backgroundColor().rgb()) < 80 &&
                qBlue(d->style.backgroundColor().rgb()) > 127) {
            penColor = Qt::red;
        }

        // Get the triangle...
        QPolygonF polygon(3);
        polygon.clear();
        if (cell.sheet()->layoutDirection() == Qt::RightToLeft) {
            polygon << QPointF(coordinate.x() + d->width - 6.0, coordinate.y());
            polygon << QPointF(coordinate.x() + d->width, coordinate.y());
            polygon << QPointF(coordinate.x() + d->width, coordinate.y() + 6.0);
        } else {
            polygon << QPointF(coordinate.x(), coordinate.y() + 6.0);
            polygon << QPointF(coordinate.x(), coordinate.y());
            polygon << QPointF(coordinate.x() + 6.0, coordinate.y());
        }

        // ...and draw it.
        painter.setBrush(QBrush(penColor));
        painter.setPen(Qt::NoPen);
        painter.drawPolygon(polygon);
    }
}


// Paint an indicator that the text in the cell is cut.
//
void CellView::paintMoreTextIndicator(QPainter& painter, const QPointF& coordinate)
{
    if (d->style.shrinkToFit())
        return;
    // Show a red triangle when it's not possible to write all text in cell.
    // Don't print the red triangle if we're printing.
    if (!d->fittingWidth &&
            !dynamic_cast<QPrinter*>(painter.device()) &&
            d->height > 4.0  &&
            d->width  > 4.0) {
        QColor penColor = Qt::red;
        // If background has high red part, switch to blue.
        if (qRed(d->style.backgroundColor().rgb()) > 127
                && qGreen(d->style.backgroundColor().rgb()) < 80
                && qBlue(d->style.backgroundColor().rgb()) < 80) {
            penColor = Qt::blue;
        }

        // Get the triangle...
        QPolygonF polygon(3);
        polygon.clear();
        if (d->displayText.isRightToLeft()) {
            polygon << QPointF(coordinate.x() + 4.0, coordinate.y() + d->height / 2.0 - 4.0);
            polygon << QPointF(coordinate.x(), coordinate.y() + d->height / 2.0);
            polygon << QPointF(coordinate.x() + 4.0, coordinate.y() + d->height / 2.0 + 4.0);
        } else {
            polygon << QPointF(coordinate.x() + d->width - 4.0, coordinate.y() + d->height / 2.0 - 4.0);
            polygon << QPointF(coordinate.x() + d->width, coordinate.y() + d->height / 2.0);
            polygon << QPointF(coordinate.x() + d->width - 4.0, coordinate.y() + d->height / 2.0 + 4.0);
        }

        // ...and paint it.
        painter.setBrush(QBrush(penColor));
        painter.setPen(Qt::NoPen);
        painter.drawPolygon(polygon);
    }
}

static int fixAngle(int angle) {
    angle = ((angle % 360) + 360) % 360;
    // now angle is between 0 and 359, but 181-359 should be -179 - -1
    if (angle > 180) angle = angle - 360;
    return angle;
}

// Paint the real contents of a cell - the text.
//
void CellView::paintText(QPainter& painter,
                         const QPointF& coordinate,
                         QPaintDevice* paintDevice, const Cell& cell)
{
    Q_UNUSED(paintDevice);

    QColor textColorPrint = d->style.fontColor();
    // Resolve the text color if invalid (=default).
    if (!textColorPrint.isValid()) {
        if (dynamic_cast<QPrinter*>(painter.device()))
            textColorPrint = Qt::black;
        else
            textColorPrint = QApplication::palette().text().color();

        QColor bgColor = d->style.backgroundColor();
        if (bgColor.isValid()) {
            qreal contrast = KColorUtils::contrastRatio(bgColor, textColorPrint);
            kDebug() << bgColor << textColorPrint << contrast;
            if (contrast < 3)
                textColorPrint = QColor(255 - textColorPrint.red(), 255 - textColorPrint.green(), 255 - textColorPrint.blue());
        }
    }

    QPen tmpPen(textColorPrint);
    QFont font = d->calculateFont();

    // Check for red font color for negative values.
    if (cell.value().isNumber()
            && !(cell.sheet()->getShowFormula()
                 && !(cell.sheet()->isProtected()
                      && style().hideFormula()))) {
        if (style().floatColor() == Style::NegRed && cell.value().asFloat() < 0.0)
            tmpPen.setColor(Qt::red);
    }

    // Check for blue color, for hyperlink.
    if (!cell.link().isEmpty()) {
        tmpPen.setColor(QApplication::palette().link().color());
        font.setUnderline(true);
    }
    painter.setPen(tmpPen);

    qreal indent = 0.0;
    qreal offsetCellTooShort = 0.0;
    const Style::HAlign hAlign = d->style.halign();
    const Style::VAlign vAlign = d->style.valign();

    // Apply indent if text is align to left not when text is at right or middle.
    if (hAlign == Style::Left && !cell.isEmpty()) {
        indent = d->style.indentation();
    }

    // Made an offset, otherwise ### is under red triangle.
    if (hAlign == Style::Right && !cell.isEmpty() && !d->fittingWidth)
        offsetCellTooShort = 4;

    KoPostscriptPaintDevice device;
    const QFontMetricsF fontMetrics(font, &device);
    qreal fontOffset = 0.0;

    if (style().valign() == Style::Bottom || style().valign() == Style::VAlignUndefined) {
        // The descent can be bigger then the underlinePos which seems to be the case at least
        // with thai characters. So, to be sure we are not losing the bottom characters we are
        // using either the underlinePos() (plus 1 for the underline itself) or the descent()
        // whatever is bigger to be sure we do not but of anything, neither parts of the font
        // nor the an optional displayed underline.
        // According to the docs this is still not perfect cause some unusual character in
        // an exotic language can still be bigger but we ignore that here.
        fontOffset = qMax(fontMetrics.underlinePos() + 1, fontMetrics.descent());
    }

    const int tmpAngle = fixAngle(d->style.angle());
    const bool tmpVerticalText = d->style.verticalText();
    // force multiple rows on explicitly set line breaks
    const bool tmpMultiRow = d->style.wrapText() || d->displayText.contains('\n');
    const bool tmpVDistributed = vAlign == Style::VJustified || vAlign == Style::VDistributed;
    const bool tmpRichText = !d->richText.isNull();


    // set a clipping region for non-rotated text
    painter.save();
    if (tmpAngle == 0)
        painter.setClipRect(QRectF(coordinate, QSizeF(d->width, d->height)));


    // Actually paint the text.
    //    There are 5 possible cases:
    //        - One line of plain text , horizontal
    //        - Angled text
    //        - Multiple rows of plain text , horizontal
    //        - Vertical text
    //        - Rich text
    if (!tmpMultiRow && !tmpVerticalText && !tmpAngle && !tmpRichText) {
        // Case 1: The simple case, one line, no angle.

        const QPointF position(indent + coordinate.x() - offsetCellTooShort,
                               coordinate.y() + d->textY - fontOffset);
        drawText(painter, position, d->displayText.split('\n'), cell);
    } else if (tmpAngle != 0) {
        // Case 2: an angle.

        painter.rotate(tmpAngle);
        qreal x;
        if (tmpAngle > 0)
            x = indent + coordinate.x();
        else
            x = indent + coordinate.x()
                - (fontMetrics.descent() + fontMetrics.ascent()) * ::sin(tmpAngle * M_PI / 180);
        qreal y;
        if (tmpAngle > 0)
            y = coordinate.y() + d->textY;
        else
            y = coordinate.y() + d->textY + d->textHeight;
        if (tmpAngle < -90 || tmpAngle > 90) {
            x += d->textWidth;
        }
        const QPointF position(x * ::cos(tmpAngle * M_PI / 180) + y * ::sin(tmpAngle * M_PI / 180),
                               -x * ::sin(tmpAngle * M_PI / 180) + y * ::cos(tmpAngle * M_PI / 180));
        drawText(painter, position, d->displayText.split('\n'), cell);
        painter.rotate(-tmpAngle);
    } else if (tmpMultiRow && !tmpVerticalText && !tmpRichText) {
        // Case 3: Multiple rows, but horizontal.
        const QPointF position(indent + coordinate.x(), coordinate.y() + d->textY);
        const qreal space = d->height - d->textHeight;
        const qreal lineSpacing = tmpVDistributed && space > 0 ? space / (d->textLinesCount - 1) : 0;
        drawText(painter, position, d->displayText.split('\n'), cell, lineSpacing);
    } else if (tmpVerticalText && !d->displayText.isEmpty()) {
        // Case 4: Vertical text.
        QStringList textLines = d->displayText.split('\n');
        qreal dx = 0.0;

        qreal space = d->width - d->textWidth;
        if (space > 0) {
            switch (hAlign) {
                case Style::Center:
                case Style::HAlignUndefined:
                    dx += space/2;
                    break;
                case Style::Right:
                    dx += space;
                    break;
                default:
                    break;
            }
        }
        for (int i = 0; i < textLines.count(); ++i) {
            QStringList textColumn;
            for (int j = 0; j < textLines[i].count(); ++j)
                textColumn << QString(textLines[i][j]);

            const QPointF position(indent + coordinate.x() + dx, coordinate.y() + d->textY);
            drawText(painter, position, textColumn, cell);
            dx += fontMetrics.maxWidth();
        }
    } else if (tmpRichText) {
        // Case 5: Rich text.
        QSharedPointer<QTextDocument> doc = d->richText;
        doc->setDefaultTextOption(d->textOptions());
        const QPointF position(coordinate.x() + indent,
                               coordinate.y() + d->textY);
        painter.translate(position);

        QAbstractTextDocumentLayout::PaintContext ctx;
        ctx.palette.setColor(QPalette::Text, textColorPrint);
        doc->documentLayout()->draw(&painter, ctx);

//        painter.drawRect(QRectF(QPointF(0, 0), QSizeF(d->textWidth, d->textHeight)));
    }

    painter.restore();
}


// Paint page borders on the page.  Only do this on the screen.
//
void CellView::paintPageBorders(QPainter& painter, const QPointF& coordinate,
                                Borders paintBorder, const Cell& cell)
{
    // Not screen?  Return immediately.
    if (dynamic_cast<QPrinter*>(painter.device()))
        return;

    if (! cell.sheet()->isShowPageBorders())
        return;

    SheetPrint* const print = cell.sheet()->print();
    const PrintSettings *const settings = cell.sheet()->printSettings();
    const QRect printRange = settings->printRegion().lastRange();

    // Draw page borders
    QLineF line;

    if (cell.column() >= printRange.left()
        && cell.column() <= printRange.right() + 1
        && cell.row() >= printRange.top()
        && cell.row() <= printRange.bottom() + 1) {
        if (print->isColumnOnNewPage(cell.column())
            && cell.row() <= printRange.bottom()) {
            painter.setPen(cell.sheet()->map()->settings()->pageBorderColor());

            if (cell.sheet()->layoutDirection() == Qt::RightToLeft)
                line = QLineF(coordinate.x() + d->width, coordinate.y(),
                              coordinate.x() + d->width, coordinate.y() + d->height);
            else
                line = QLineF(coordinate.x(), coordinate.y(),
                              coordinate.x(), coordinate.y() + d->height);
            painter.drawLine(line);
        }

        if (print->isRowOnNewPage(cell.row()) &&
            (cell.column() <= printRange.right())) {
            painter.setPen(cell.sheet()->map()->settings()->pageBorderColor());
            line = QLineF(coordinate.x(),  coordinate.y(),
                          coordinate.x() + d->width, coordinate.y());
            painter.drawLine(line);
        }

        if (paintBorder & RightBorder) {
            if (print->isColumnOnNewPage(cell.column() + 1)
                && cell.row() <= printRange.bottom()) {
                painter.setPen(cell.sheet()->map()->settings()->pageBorderColor());

                if (cell.sheet()->layoutDirection() == Qt::RightToLeft)
                    line = QLineF(coordinate.x(), coordinate.y(),
                                  coordinate.x(), coordinate.y() + d->height);
                else
                    line = QLineF(coordinate.x() + d->width, coordinate.y(),
                                  coordinate.x() + d->width, coordinate.y() + d->height);
                painter.drawLine(line);
            }
        }

        if (paintBorder & BottomBorder) {
            if (print->isRowOnNewPage(cell.row() + 1)
                && cell.column() <= printRange.right()) {
                painter.setPen(cell.sheet()->map()->settings()->pageBorderColor());
                line = QLineF(coordinate.x(),  coordinate.y() + d->height,
                              coordinate.x() + d->width, coordinate.y() + d->height);
                painter.drawLine(line);
            }
        }
    }
}


// Paint the cell borders.
//
void CellView::paintCustomBorders(QPainter& painter, const QRectF& paintRect,
                                  const QPointF& coordinate, Borders paintBorder)
{
    //Sanity check: If we are not painting any of the borders then the function
    //really shouldn't be called at all.
    if (paintBorder == NoBorder)
        return;

    // Must create copies of these since otherwise the zoomIt()
    // operation will be performed on them repeatedly.
    QPen  leftPen(d->style.leftBorderPen());
    QPen  rightPen(d->style.rightBorderPen());
    QPen  topPen(d->style.topBorderPen());
    QPen  bottomPen(d->style.bottomBorderPen());

    // Determine the pens that should be used for drawing
    // the borders.
    // NOTE Stefan: This prevents cosmetic pens (width==0).
    int left_penWidth   = qMax(1, (leftPen.width()));
    int right_penWidth  = qMax(1, (rightPen.width()));
    int top_penWidth    = qMax(1, (topPen.width()));
    int bottom_penWidth = qMax(1, (bottomPen.width()));

    leftPen.setWidth(left_penWidth);
    rightPen.setWidth(right_penWidth);
    topPen.setWidth(top_penWidth);
    bottomPen.setWidth(bottom_penWidth);

    QLineF line;

    if ((paintBorder & LeftBorder) && leftPen.style() != Qt::NoPen) {
        painter.setPen(leftPen);

        //kDebug(36004) <<"    painting left border of cell" << name();

        // If we are on paper printout, we limit the length of the lines.
        // On paper, we always have full cells, on screen not.
        if (dynamic_cast<QPrinter*>(painter.device())) {
            if (coordinate.x() >= paintRect.left() + left_penWidth / 2)
                line = QLineF(coordinate.x() ,
                              qMax(paintRect.top(), coordinate.y()),
                              coordinate.x(),
                              qMin(paintRect.bottom(), coordinate.y() + d->height));
        } else {
            line = QLineF(coordinate.x(), coordinate.y(), coordinate.x(), coordinate.y() + d->height);
        }
        painter.drawLine(line);
    }

    if ((paintBorder & RightBorder) && rightPen.style() != Qt::NoPen) {
        painter.setPen(rightPen);

        //kDebug(36004) <<"    painting right border of cell" << name();

        // If we are on paper printout, we limit the length of the lines.
        // On paper, we always have full cells, on screen not.
        if (dynamic_cast<QPrinter*>(painter.device())) {
            // Only print the right border if it is visible.
            if (coordinate.x() + d->width <= paintRect.right() + right_penWidth / 2)
                line = QLineF(coordinate.x() + d->width,
                              qMax(paintRect.top(), coordinate.y()),
                              coordinate.x() + d->width,
                              qMin(paintRect.bottom(), coordinate.y() + d->height));
        } else {
            line = QLineF(coordinate.x() + d->width, coordinate.y(), coordinate.x() + d->width, coordinate.y() + d->height);
        }
        painter.drawLine(line);
    }

    if ((paintBorder & TopBorder) && topPen.style() != Qt::NoPen) {
        painter.setPen(topPen);

        //kDebug(36004) <<"    painting top border of cell" << name()
        //       << " [" << coordinate.x() << "," << coordinate.x() + d->width
        //       << ": " << coordinate.x() + d->width - coordinate.x() << "]" << endl;

        // If we are on paper printout, we limit the length of the lines.
        // On paper, we always have full cells, on screen not.
        if (dynamic_cast<QPrinter*>(painter.device())) {
            if (coordinate.y() >= paintRect.top() + top_penWidth / 2)
                line = QLineF(qMax(paintRect.left(),   coordinate.x()),
                              coordinate.y(),
                              qMin(paintRect.right(),  coordinate.x() + d->width),
                              coordinate.y());
        } else {
            line = QLineF(coordinate.x(), coordinate.y(), coordinate.x() + d->width, coordinate.y());
        }
        painter.drawLine(line);
    }

    if ((paintBorder & BottomBorder) && bottomPen.style() != Qt::NoPen) {
        painter.setPen(bottomPen);

        //kDebug(36004) <<"    painting bottom border of cell" << name()
        //       << " [" << coordinate.x() << "," << coordinate.x() + d->width
        //       << ": " << coordinate.x() + d->width - coordinate.x() << "]" << endl;

        // If we are on paper printout, we limit the length of the lines.
        // On paper, we always have full cells, on screen not.
        if (dynamic_cast<QPrinter*>(painter.device())) {
            if (coordinate.y() + d->height <= paintRect.bottom() + bottom_penWidth / 2)
                line = QLineF(qMax(paintRect.left(),   coordinate.x()),
                              coordinate.y() + d->height,
                              qMin(paintRect.right(),  coordinate.x() + d->width),
                              coordinate.y() + d->height);
        } else {
            line = QLineF(coordinate.x(), coordinate.y() + d->height, coordinate.x() + d->width, coordinate.y() + d->height);
        }
        painter.drawLine(line);
    }
}


// Paint diagonal lines through the cell.
//
void CellView::paintCellDiagonalLines(QPainter& painter, const QPointF& coordinate)
{
    if (d->merged)
        return;

    QPen fallDiagonalPen(d->style.fallDiagonalPen());
    QPen goUpDiagonalPen(d->style.goUpDiagonalPen());

    if (fallDiagonalPen.style() != Qt::NoPen) {
        painter.setPen(fallDiagonalPen);
        painter.drawLine(QLineF(coordinate.x(), coordinate.y(), coordinate.x() + d->width, coordinate.y() + d->height));
    }

    if (goUpDiagonalPen.style() != Qt::NoPen) {
        painter.setPen(goUpDiagonalPen);
        painter.drawLine(QLineF(coordinate.x(), coordinate.y() + d->height, coordinate.x() + d->width, coordinate.y()));
    }
}

void CellView::paintFilterButton(QPainter& painter, const QPointF& coordinate,
                                 const Cell& cell, SheetView* sheetView)
{
    Q_UNUSED(cell);
    QStyleOptionComboBox options;
    options.direction = cell.sheet()->layoutDirection();
    options.editable = true;
    options.fontMetrics = painter.fontMetrics();
    options.frame = false;
    options.rect = sheetView->viewConverter()->documentToView(QRectF(coordinate, QSizeF(d->width, d->height))).toRect();
//     options.subControls = QStyle::SC_ComboBoxEditField | QStyle::SC_ComboBoxArrow;

    painter.save();
    painter.scale(sheetView->viewConverter()->viewToDocumentX(1.0),
                  sheetView->viewConverter()->viewToDocumentY(1.0));
    QApplication::style()->drawComplexControl(QStyle::CC_ComboBox, &options, &painter);
    painter.restore();
}


// Cut d->displayText, so that it only holds the part that can be displayed.
//
// Used in paintText().
//
QString CellView::textDisplaying(const QFontMetricsF& fm, const Cell& cell)
{
    Style::HAlign hAlign = style().halign();
    if (!d->fittingWidth)
        hAlign = Style::Left; // force left alignment, if text does not fit

    const bool isNumeric = cell.value().isNumber();

    if (style().wrapText() || d->richText) {
        // For wrapping text and richtext always draw all text
        return d->displayText;
    } else if (!style().verticalText()) {
        // Non-vertical text: the ordinary case.

        // Not enough space but align to left
        qreal  len = 0.0;

        len = d->width;
#if 0
        for (int i = cell.column(); i <= cell.column() + d->obscuredCellsX; i++) {
            ColumnFormat *cl2 = cell.sheet()->columnFormat(i);
            len += cl2->width() - 1.0; //-1.0 because the pixel in between 2 cells is shared between both cells
        }
#endif

        QString  tmp;
        qreal   tmpIndent = 0.0;
        if (!cell.isEmpty())
            tmpIndent = style().indentation();

        // Estimate worst case length to reduce the number of iterations.
        int start = qRound((len - 4.0 - 1.0 - tmpIndent) / fm.width('.'));
        start = qMin(d->displayText.length(), start);
        // Start out with the whole text, cut one character at a time, and
        // when the text finally fits, return it.
        for (int i = start; i != 0; i--) {
            //Note that numbers are always treated as left-aligned since if we have to cut digits off, they should
            //always be the least significant ones at the end of the string
            if (hAlign == Style::Left || hAlign == Style::HAlignUndefined || isNumeric)
                tmp = d->displayText.left(i);
            else if (hAlign == Style::Right)
                tmp = d->displayText.right(i);
            else
                tmp = d->displayText.mid((d->displayText.length() - i) / 2, i);

            if (isNumeric) {
                //For numeric values, we can cut off digits after the decimal point to make it fit,
                //but not the integer part of the number.
                //If this number still contains a fraction part then we don't need to do anything, if we have run
                //out of space to fit even the integer part of the number then display #########
                //TODO Perhaps try to display integer part in standard form if there is not enough room for it?

                if (!tmp.contains('.'))
                    d->displayText = QString().fill('#', 20);
            }

            // 4 equal length of red triangle +1 point.
            if (fm.width(tmp) + tmpIndent < len - 4.0 - 1.0) {
                if (style().angle() != 0) {
                    QString tmp2;
                    const RowFormat *rl = cell.sheet()->rowFormat(cell.row());
                    if (d->textHeight > rl->height()) {
                        for (int j = d->displayText.length(); j != 0; j--) {
                            tmp2 = d->displayText.left(j);
                            if (fm.width(tmp2) < rl->height() - 1.0) {
                                return d->displayText.left(qMin(tmp.length(), tmp2.length()));
                            }
                        }
                    } else
                        return tmp;

                } else
                    return tmp;
            }
        }
        return QString("");
    } else if (style().verticalText()) {
        // Vertical text.

        const RowFormat  *rl = cell.sheet()->rowFormat(cell.row());
        qreal      tmpIndent = 0.0;

        // Not enough space but align to left.
        qreal  len = 0.0;

        len = d->width;
#if 0
        for (int i = cell.column(); i <= cell.column() + d->obscuredCellsX; i++) {
            ColumnFormat  *cl2 = cell.sheet()->columnFormat(i);

            // -1.0 because the pixel in between 2 cells is shared between both cells
            len += cl2->width() - 1.0;
        }
#endif

        if (!cell.isEmpty())
            tmpIndent = style().indentation();

        if ((d->textWidth + tmpIndent > len) || d->textWidth == 0.0)
            return QString("");

        for (int i = d->displayText.length(); i != 0; i--) {
            if (fm.ascent() + fm.descent() * i < rl->height() - 1.0)
                return d->displayText.left(i);
        }

        return QString("");
    }

    QString tmp;
    for (int i = d->displayText.length(); i != 0; i--) {
        tmp = d->displayText.left(i);

        // 4 equals length of red triangle +1 pixel
        if (fm.width(tmp) < d->width - 4.0 - 1.0)
            return tmp;
    }

    return  QString();
}


//                        End of Painting
// ================================================================

// ================================================================
//                              Layout


// Recalculate the entire layout.  This includes the following members:
//
//   d->textX,     d->textY
//   d->textWidth, d->textHeight
//   d->obscuredCellsX, d->obscuredCellsY
//   d->width, d->height
//
// and, of course,
//
//   d->displayText
//
void CellView::makeLayout(SheetView* sheetView, const Cell& cell)
{
    // Up to here, we have just cared about the contents, not the
    // painting of it.  Now it is time to see if the contents fits into
    // the cell and, if not, maybe rearrange the outtext a bit.

    // First, create a device independent font and its metrics.
    KoPostscriptPaintDevice device;
    QFont font(d->style.font(), &device);
    QFontMetricsF fontMetrics(font, &device);

    // Then calculate text dimensions, i.e. d->textWidth and d->textHeight,
    // and check whether the text fits into the cell dimension by the way.

    d->calculateTextSize(font, fontMetrics);
    d->shrinkToFitFontSize = 0.0;

    //if shrink-to-fit is enabled, try to find a font size so that the string fits into the cell
    if (d->style.shrinkToFit()) {
        int lower = 1;
        int upper = font.pointSize() * 2;
        int siz = 0;
        while (lower != upper) {
            siz = static_cast<int>( std::ceil( lower + ( upper - lower ) / 2. ) );
            font.setPointSizeF(siz / 2.);
            fontMetrics = QFontMetricsF(font, &device);
            d->calculateTextSize(font, fontMetrics);
            if (d->fittingWidth)\
                lower = siz;
            else
                upper = siz - 1;
        }
        d->shrinkToFitFontSize = upper / 2.0;
        font.setPointSizeF(upper / 2.0);
        fontMetrics = QFontMetricsF(font, &device);
        d->calculateTextSize(font, fontMetrics); // update fittingWidth et al
        d->fittingWidth = true;
    }

    // Obscure horizontal cells, if necessary.
    if (!d->fittingWidth) {
        obscureHorizontalCells(sheetView, cell);
        // Recalculate the text dimensions and check whether the text fits.
        d->calculateTextSize(font, fontMetrics);
    }

    // Obscure vertical cells, if necessary.
    if (!d->fittingHeight) {
        obscureVerticalCells(sheetView, cell);
        // Recalculate the text dimensions and check whether the text fits.
        d->calculateTextSize(font, fontMetrics);
    }

    // text still does not fit into cell dimension?
    if (!d->fittingWidth || !d->fittingHeight) {
        // Truncate the output text.
        d->displayText = textDisplaying(fontMetrics, cell);
//         d->truncateText(font, fontMetrics);
//         // Recalculate the text dimensions and check whether the text fits.
//         d->calculateTextSize(font, fontMetrics);
    }
    // Recalculate the text offset.
    textOffset(fontMetrics, cell);
}


void CellView::calculateCellDimension(const Cell& cell)
{
    Q_UNUSED(cell);
#if 0
    qreal width  = cell.sheet()->columnFormat(cell.column())->width();
    qreal height = cell.sheet()->rowFormat(cell.row())->height();

    // Calculate extraWidth and extraHeight if we have a merged cell.
    if (cell.testFlag(Cell::Flag_Merged)) {
        // FIXME: Introduce qreal extraWidth/Height here and use them
        //        instead (see FIXME about this in paintCell()).

        for (int x = cell.column() + 1; x <= cell.column() + d->obscuredCellsX; x++)
            width += cell.sheet()->columnFormat(x)->width();

        for (int y = cell.row() + 1; y <= cell.row() + d->obscuredCellsY; y++)
            height += cell.sheet()->rowFormat(y)->height();
    }

    // Cache the newly calculated extraWidth and extraHeight if we have
    // already allocated a struct for it.  Otherwise it will be zero, so
    // don't bother.
    if (cell.d->hasExtra()) {
        cell.d->extra()->extraWidth  = width;
        cell.d->extra()->extraHeight = height;
    }
#endif
}


// Recalculate d->textX and d->textY.
//
// Used in makeLayout().
//
void CellView::textOffset(const QFontMetricsF& fontMetrics, const Cell& cell)
{
    Q_UNUSED(cell)
    const qreal ascent = fontMetrics.ascent();
    const Style::HAlign hAlign = d->style.halign();
    const Style::VAlign vAlign = d->style.valign();
    const int tmpAngle = fixAngle(d->style.angle());
    const bool tmpVerticalText = d->style.verticalText();
    const bool tmpMultiRow = d->style.wrapText() || d->displayText.contains('\n');
    const bool tmpRichText = !d->richText.isNull();

    qreal  w = d->width;
    qreal  h = d->height;

    // doc coordinate system; no zoom applied
    const qreal effTop = s_borderSpace + 0.5 * d->style.topBorderPen().width();
    const qreal effBottom = h - s_borderSpace - 0.5 * d->style.bottomBorderPen().width();

    // Calculate d->textY based on the vertical alignment and a few
    // other inputs.
    switch (vAlign) {
    case Style::VJustified:
    case Style::Top: {
        if (tmpAngle == 0 && tmpRichText) {
            d->textY = effTop;
        } else if (tmpAngle == 0) {
            d->textY = effTop + ascent;
        } else if (tmpAngle < 0) {
            d->textY = effTop;
        } else {
            d->textY = effTop + ascent * ::cos(tmpAngle * M_PI / 180);
        }
        break;
    }
    case Style::VAlignUndefined: // fall through
    case Style::Bottom: {
        if (!tmpVerticalText && !tmpMultiRow && !tmpAngle && !tmpRichText) {
            d->textY = effBottom;
        } else if (tmpAngle != 0) {
            // Is enough place available?
            if (effBottom - effTop - d->textHeight > 0) {
                if (tmpAngle < 0) {
                    d->textY = effBottom - d->textHeight;
                } else {
                    d->textY = effBottom - d->textHeight + ascent * ::cos(tmpAngle * M_PI / 180);
                }
                if (tmpAngle < -90 || tmpAngle > 90) {
                    d->textY += ascent * ::cos(tmpAngle * M_PI / 180);
                }
            } else {
                if (tmpAngle < 0) {
                    d->textY = effTop;
                } else {
                    d->textY = effTop + ascent * ::cos(tmpAngle * M_PI / 180);
                }
            }
        } else if (tmpRichText) {
            d->textY = effBottom - d->textHeight;
        } else if (tmpMultiRow && !tmpVerticalText) {
            // Is enough place available?
            if (effBottom - effTop - d->textHeight > 0) {
                d->textY = effBottom - d->textHeight + ascent;
            } else {
                d->textY = effTop + ascent;
            }
        } else {
            // Is enough place available?
            if (effBottom - effTop - d->textHeight > 0) {
                d->textY = effBottom - d->textHeight + ascent;
            } else {
                d->textY = effTop + ascent;
            }
        }
        break;
    }
    case Style::VDistributed:
        if (!tmpVerticalText && !tmpAngle && d->textLinesCount > 1) {
            d->textY = effTop + ascent;
            break;
        }
        // fall through
    case Style::Middle: {
        if (!tmpVerticalText && !tmpMultiRow && !tmpAngle && !tmpRichText) {
            d->textY = (h - d->textHeight) / 2 + ascent;
        } else if (tmpAngle != 0) {
            // Is enough place available?
            if (effBottom - effTop - d->textHeight > 0) {
                if (tmpAngle < 0) {
                    d->textY = (h - d->textHeight) / 2;
                } else {
                    d->textY = (h - d->textHeight) / 2 + ascent * ::cos(tmpAngle * M_PI / 180);
                }
            } else {
                if (tmpAngle < 0) {
                    d->textY = effTop;
                } else {
                    d->textY = effTop + ascent * ::cos(tmpAngle * M_PI / 180);
                }
            }
        } else if (tmpRichText && !tmpVerticalText) {
            d->textY = (h - d->textHeight) / 2;
        } else if (tmpMultiRow && !tmpVerticalText) {
            // Is enough place available?
            if (effBottom - effTop - d->textHeight > 0) {
                d->textY = (h - d->textHeight) / 2 + ascent;
            } else {
                d->textY = effTop + ascent;
            }
        } else {
            // Is enough place available?
            if (effBottom - effTop - d->textHeight > 0) {
                d->textY = (h - d->textHeight) / 2 + ascent;
            } else
                d->textY = effTop + ascent;
        }
        break;
    }
    }

    // Calculate d->textX based on alignment and textwidth.
    switch (hAlign) {
    case Style::Left:
        d->textX = 0.5 * d->style.leftBorderPen().width() + s_borderSpace;
        break;
    case Style::Right:
        d->textX = w - s_borderSpace - d->textWidth
                   - 0.5 * d->style.rightBorderPen().width();
        break;
    case Style::Center:
        d->textX = 0.5 * (w - s_borderSpace - d->textWidth -
                          0.5 * d->style.rightBorderPen().width());
        break;
    default:
        break;
    }
}

void CellView::obscureHorizontalCells(SheetView* sheetView, const Cell& masterCell)
{
    if (d->hidden)
        return;

    qreal extraWidth = 0.0;
    const Style::HAlign align = d->style.halign();

    // Get indentation.  This is only used for left aligned text.
    qreal indent = 0.0;
    if (align == Style::Left && !masterCell.isEmpty())
        indent = style().indentation();

    // Set d->fittingWidth to false, if the text is vertical or angled, and too
    // high for the cell.
    if (style().verticalText() || style().angle() != 0)
        if (d->textHeight >= d->height)
            d->fittingWidth = false;

    // Do we have to occupy additional cells to the right?  This is only
    // done for cells that have no merged cells in the Y direction.
    //
    // FIXME: Check if all cells along the merged edge to the right are
    //        empty and use the extra space?  No, probably not.
    //
    if (d->textWidth + indent > (d->width - 2 * s_borderSpace
                                 - style().leftBorderPen().width() - style().rightBorderPen().width()) &&
            masterCell.mergedYCells() == 0) {
        const int effectiveCol = masterCell.column() + masterCell.mergedXCells();
        int col = effectiveCol;

        // Find free cells to the right of this one.
        enum { Undefined, EnoughSpace, NotEnoughSpace } status = Undefined;
        while (status == Undefined) {
            Cell nextCell = Cell(masterCell.sheet(), col + 1, masterCell.row()).masterCell();

            if (nextCell.isEmpty()) {
                extraWidth += nextCell.width();
                col += 1 + nextCell.mergedXCells();

                // Enough space?
                if (d->textWidth + indent <= (d->width + extraWidth - 2 * s_borderSpace
                                              - style().leftBorderPen().width() - style().rightBorderPen().width()))
                    status = EnoughSpace;
            } else
                // Not enough space, but the next cell is not empty
                status = NotEnoughSpace;
        }

        // Try to use additional space from the neighboring cells that
        // were calculated in the last step.
        //
        // Currently this is only done for left aligned cells. We have to
        // check to make sure we haven't already force-merged enough cells
        //
        // FIXME: Why not right/center aligned text?
        //
        // FIXME: Shouldn't we check to see if end == -1 here before
        //        setting d->fittingWidth to false?
        //
        if (style().halign() == Style::Left || (style().halign() == Style::HAlignUndefined
                                                && !masterCell.value().isNumber())) {
            if (col > effectiveCol) {
                d->obscuredCellsX = col - effectiveCol;
                d->width += extraWidth;

                const QRect obscuredRange(effectiveCol + 1, masterCell.row(), d->obscuredCellsX, 1);
                sheetView->obscureCells(obscuredRange, masterCell.cellPosition());

                // Not enough space
                if (status == NotEnoughSpace)
                    d->fittingWidth = false;
            } else
                d->fittingWidth = false;
        } else
            d->fittingWidth = false;
    }
}

void CellView::obscureVerticalCells(SheetView* sheetView, const Cell& masterCell)
{
    if (d->hidden)
        return;

    qreal extraHeight = 0.0;

    // Do we have to occupy additional cells at the bottom ?
    //
    // FIXME: Setting to make the current cell grow.
    //
    if (d->displayText.contains('\n') &&
            d->textHeight > (d->height - 2 * s_borderSpace
                             - style().topBorderPen().width() - style().bottomBorderPen().width())) {
        const int effectiveRow = masterCell.row() + masterCell.mergedYCells();
        int row = effectiveRow;

        // Find free cells bottom to this one
        enum { Undefined, EnoughSpace, NotEnoughSpace } status = Undefined;
        while (status == Undefined) {
            Cell nextCell = Cell(masterCell.sheet(), masterCell.column(), row + 1).masterCell();

            if (nextCell.isEmpty()) {
                extraHeight += nextCell.height();
                row += 1 + nextCell.mergedYCells();

                // Enough space ?
                if (d->textHeight <= (d->height + extraHeight - 2 * s_borderSpace
                                      - style().topBorderPen().width() - style().bottomBorderPen().width()))
                    status = EnoughSpace;
            } else
                // Not enough space, but the next cell is not empty.
                status = NotEnoughSpace;
        }

        // Check to make sure we haven't already force-merged enough cells.
        if (row > effectiveRow) {
            d->obscuredCellsY = row - effectiveRow;
            d->height += extraHeight;

            const QRect obscuredRange(masterCell.column(), effectiveRow + 1, 1, d->obscuredCellsY);
            sheetView->obscureCells(obscuredRange, masterCell.cellPosition());

            // Not enough space
            if (status == NotEnoughSpace)
                d->fittingHeight = false;
        } else
            d->fittingHeight = false;
    }
}

void CellView::drawText(QPainter& painter, const QPointF& location, const QStringList& textLines,
                        const Cell& cell, qreal lineSpacing) const
{
    Q_UNUSED(cell)

    KoPostscriptPaintDevice device;
    const QFont font(d->calculateFont(), &device);
    const QFontMetricsF fontMetrics(font, &device);

    const qreal leading = fontMetrics.leading();

    const QTextOption options = d->textOptions();

    const bool tmpVerticalText = d->style.verticalText();
    const qreal lineWidth = tmpVerticalText ? fontMetrics.maxWidth() :
                                        (d->width - 2 * s_borderSpace
                                        - 0.5 * d->style.leftBorderPen().width()
                                        - 0.5 * d->style.rightBorderPen().width());

    qreal offset = 1.0 - fontMetrics.ascent();
    for (int i = 0; i < textLines.count(); ++i) {
        QTextLayout textLayout(textLines[i], font);
        textLayout.setCacheEnabled(true);
        textLayout.setTextOption(options);
        textLayout.beginLayout();
        qreal height = 0.0;
        forever {
            // we don't need to break if the text no longer fits in the cell;
            // a clipping region is set on the painter to make sure we won't draw outside the cell
            QTextLine line = textLayout.createLine();
            if (!line.isValid())
                break;
            line.setLineWidth(lineWidth);
            height += leading;
            line.setPosition(QPointF((s_borderSpace + 0.5 * d->style.leftBorderPen().widthF()),
                                    height));

            height += line.height() + lineSpacing;
        }
        textLayout.endLayout();

        textLayout.draw(&painter, QPointF(location.x(), (location.y() + offset)));
        offset += height;
    }
}

void CellView::obscure(int col, int row)
{
    d->obscured = true;
    d->obscuringCellX = col;
    d->obscuringCellY = row;
}

QSize CellView::obscuredRange() const
{
    return d->obscured ? QSize() : QSize(d->obscuredCellsX, d->obscuredCellsY);
}

QPoint CellView::obscuringCell() const
{
    return d->obscured ? QPoint(d->obscuringCellX, d->obscuringCellY) : QPoint();
}

bool CellView::isObscured() const
{
    return d->obscured && (d->obscuringCellX != 0 && d->obscuringCellY != 0);
}

bool CellView::obscuresCells() const
{
    return !d->obscured && (d->obscuredCellsX != 0 || d->obscuredCellsY != 0);
}

qreal CellView::cellHeight() const
{
    return d->height;
}

qreal CellView::cellWidth() const
{
    return d->width;
}

bool CellView::dimensionFits() const
{
    return d->fittingHeight && d->fittingWidth;
}

CellView& CellView::operator=(const CellView & other)
{
    d = other.d;
    return *this;
}


void CellView::Private::checkForFilterButton(const Cell& cell)
{
    const Database database = cell.database();
    if (database.isEmpty() || !database.displayFilterButtons()) {
        filterButton = false;
        return;
    }
    if (database.orientation() == Qt::Horizontal)
        filterButton = database.range().firstRange().left() == cell.column();
    else // Qt::Vertical
        filterButton = database.range().firstRange().top() == cell.row();
}

void CellView::Private::calculateTextSize(const QFont& font, const QFontMetricsF& fontMetrics)
{
    if (style.angle() != 0)
        calculateAngledTextSize(font, fontMetrics);
    else if (style.verticalText())
        calculateVerticalTextSize(font, fontMetrics);
    else if (richText)
        calculateRichTextSize(font, fontMetrics);
    else
        calculateHorizontalTextSize(font, fontMetrics);
}

void CellView::Private::calculateHorizontalTextSize(const QFont& font, const QFontMetricsF& fontMetrics)
{
    const QStringList textLines = displayText.split('\n');
    const qreal leading = fontMetrics.leading();
    const QTextOption options = textOptions();

    textHeight = 0.0;
    textWidth = 0.0;
    textLinesCount = 0;
    fittingHeight = true;
    fittingWidth = true;
    for (int i = 0; i < textLines.count(); ++i) {
        textWidth = qMax(textWidth, fontMetrics.width(textLines[i]));
        QTextLayout textLayout(textLines[i], font);
        textLayout.setTextOption(options);
        textLayout.beginLayout();
        forever {
            QTextLine line = textLayout.createLine();
            if (!line.isValid())
                break; // forever
            line.setLineWidth(width);
            textHeight += leading + line.height();
            if ((textHeight - fontMetrics.descent()) > (height - 2 * s_borderSpace
                    - 0.5 * style.topBorderPen().width()
                    - 0.5 * style.bottomBorderPen().width())) {
                fittingHeight = false;
                break; // forever
            }
        }
        textLinesCount += textLayout.lineCount();
        textLayout.endLayout();
    }
    // The width fits, if the text is wrapped or all lines are smaller than the cell width.
    fittingWidth = style.wrapText() ||
                   textWidth <= (width - 2 * s_borderSpace
                                 - 0.5 * style.leftBorderPen().width()
                                 - 0.5 * style.rightBorderPen().width());
}

void CellView::Private::calculateVerticalTextSize(const QFont& font, const QFontMetricsF& fontMetrics)
{
    Q_UNUSED(font)
    int rows = 0;
    const QStringList textLines = displayText.split('\n');
    for (int i = 0; i < textLines.count(); ++i)
        rows = qMax(rows, textLines[i].count());
    textHeight = (fontMetrics.ascent() + fontMetrics.descent()) * rows;
    textWidth  = (displayText.count('\n') + 1) * fontMetrics.maxWidth();
    fittingHeight = textHeight <= this->width;
    fittingWidth = textWidth <= this->height;
}

void CellView::Private::calculateAngledTextSize(const QFont& font, const QFontMetricsF& fontMetrics)
{
    Q_UNUSED(font)
    const qreal angle = fixAngle(style.angle());
    const qreal height = fontMetrics.ascent() + fontMetrics.descent();
    const qreal width  = fontMetrics.width(displayText);
    textHeight = qAbs(height * ::cos(angle * M_PI / 180)) + qAbs(width * ::sin(angle * M_PI / 180));
    textWidth = qAbs(height * ::sin(angle * M_PI / 180)) + qAbs(width * ::cos(angle * M_PI / 180));
    fittingHeight = textHeight <= this->width;
    fittingWidth = textWidth <= this->height;
}

void CellView::Private::calculateRichTextSize(const QFont& font, const QFontMetricsF& fontMetrics)
{
    Q_UNUSED(fontMetrics);

    richText->setDefaultFont(font);
    richText->setDocumentMargin(0);

    const qreal lineWidth = width - 2 * s_borderSpace
                - 0.5 * style.leftBorderPen().widthF()
                - 0.5 * style.rightBorderPen().widthF();

    if (style.wrapText())
        richText->setTextWidth(lineWidth);
    else
        richText->setTextWidth(-1);
    const QSizeF textSize = richText->size();
    textHeight = textSize.height();
    textWidth = textSize.width();
    textLinesCount = richText->lineCount();
    // TODO: linescount is not correct, and distributed vertical allignment doesn't
    // work anyway for richtext at the momemnt
    fittingHeight = textHeight <= (height - 2 * s_borderSpace
                            - 0.5 * style.topBorderPen().widthF()
                            - 0.5 * style.bottomBorderPen().widthF());
    fittingWidth = textWidth <= lineWidth;
}

void CellView::Private::truncateText(const QFont& font, const QFontMetricsF& fontMetrics)
{
    if (style.angle() != 0)
        truncateAngledText(font, fontMetrics);
    else if (style.verticalText())
        truncateVerticalText(font, fontMetrics);
    else
        truncateHorizontalText(font, fontMetrics);
}

void CellView::Private::truncateHorizontalText(const QFont& font, const QFontMetricsF& fontMetrics)
{
    if (!style.wrapText()) {
        const QStringList textLines = displayText.split('\n');
        displayText.clear();
        qreal height = font.pointSizeF();
        for (int i = 0; i < textLines.count(); ++i) {
            if (height > this->height)
                break;
            int count = 0;
            while (count < textLines[i].count() && fontMetrics.width(textLines[i].left(count)) <= this->width)
                ++count;
            displayText += textLines[i].left(count);
            height += fontMetrics.height();
            if (height <= this->height)
                displayText += '\n';
        }
    }
    // else it is handled by QTextLayout
}

void CellView::Private::truncateVerticalText(const QFont& font, const QFontMetricsF& fontMetrics)
{
    Q_UNUSED(font);
    Q_UNUSED(fontMetrics);
}

void CellView::Private::truncateAngledText(const QFont& font, const QFontMetricsF& fontMetrics)
{
    Q_UNUSED(font);
    Q_UNUSED(fontMetrics);
}

QTextOption CellView::Private::textOptions() const
{
    QTextOption options;
    switch (style.halign()) {
    default:
    case Style::Left:
        options.setAlignment(Qt::AlignLeft);
        break;
    case Style::Right:
        options.setAlignment(Qt::AlignRight);
        break;
    case Style::Center:
        options.setAlignment(Qt::AlignHCenter);
    case Style::Justified:
        options.setAlignment(Qt::AlignJustify);
    }
    // The text consists of a single character, if it's vertical. Always center it.
    if (style.verticalText())
        options.setAlignment(Qt::AlignHCenter);
    options.setWrapMode(style.wrapText() ? QTextOption::WrapAtWordBoundaryOrAnywhere : QTextOption::NoWrap);
    return options;
}
