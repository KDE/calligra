/* This file is part of the KDE project
   Copyright 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2004 Tomas Mecir <mecirt@gmail.com>
   Copyright 1999-2002,2004 Laurent Montel <montel@kde.org>
   Copyright 2002,2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2003 Stefan Hetzl <shetzl@chello.at>
   Copyright 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002 Harri Porten <porten@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 1999-2001 David Faure <faure@kde.org>
   Copyright 2000-2001 Werner Trobin <trobin@kde.org>
   Copyright 2000 Simon Hausmann <hausmann@kde.org
   Copyright 1998-1999 Torben Weis <weis@kde.org>
   Copyright 1999 Michael Reiher <michael.reiher@gmx.de>
   Copyright 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef KSPREAD_CELL_VIEW
#define KSPREAD_CELL_VIEW

// Qt
#include <QLinkedList>
#include <QSharedDataPointer>

// KSpread
#include "Global.h"
#include "Style.h"

class QPaintDevice;
class QRectF;

namespace KSpread
{
class Cell;
class Conditions;
class Sheet;
class SheetView;
class Style;

/**
 * Responsible for the painting of Cell.
 * For each visible Cell exists a CellView, which is cached in the
 * corresponding SheetView. If the content of a Cell has changed, the CellView
 * will be destroyed and a new one will be created.
 */
class KSPREAD_EXPORT CellView
{
    friend class SheetView;

public:
    CellView( SheetView* sheetView, int col, int row );
    CellView( const CellView& other );
    ~CellView();

    enum Border
    {
        NoBorder     = 0x0,
        LeftBorder   = 0x1,
        RightBorder  = 0x2,
        TopBorder    = 0x4,
        BottomBorder = 0x8
    };
    Q_DECLARE_FLAGS(Borders, Border)

    /**
     * \return the style for the cell associated with this view
     */
    Style style() const;

    /**
     * \ingroup Painting
     * Paints the cell's background.
     * \param painter the used painter
     * \param paintCoordinate the top left coordinate (scroll offset dependent)
     */
    void paintCellBackground( QPainter& painter, const QPointF& paintCoordinate );

  /**
   * \ingroup Painting
   * Paints the cell.
   * \param paintRegion the portion of the canvas that is actually in view
   * \param painter the used painter
   * \param paintDevice the paintDevice of this data.  Used for:
   *        \li layout direction (should become obsolete),
   *        \li painting obscured cells (should become obsolete)
   *        \li (was used for selection painting, which is now in QPaintDevice)
   * \param paintCoordinate the top left coordinate (scroll offset dependent)
   * \param cellCoordinate the cell position (should be removed!)
   * \param mergedCellsPainted a list of merged cells already painted
   * \param cell the Cell (should be removed!)
   */
  void paintCellContents( const QRectF& paintRegion, QPainter& painter,
                          QPaintDevice* paintDevice, const QPointF& paintCoordinate,
                          const QPoint & cellCoordinate,
                          QLinkedList<QPoint> &mergedCellsPainted, const Cell& cell );

  /**
   * \ingroup Painting
   * Paints the cell custom borders, the page borders, diagonal lines.
   * \param paintRegion the portion of the canvas that is actually in view
   * \param painter the used painter
   * \param paintCoordinate the top left coordinate (scroll offset dependent)
   * \param cellCoordinate the cell position (should be removed!)
   * \param cellRange the cell range, that is painted (should be removed!)
   * \param mergedCellsPainted a list of merged cells already painted
   * \param cell the Cell (should be removed!)
   * \param sheetView the SheetView (should be removed!)
   */
  void paintCellBorders( const QRectF& paintRegion, QPainter& painter,
                         const QPointF& paintCoordinate,
                         const QPoint& cellCoordinate, const QRect& cellRange,
                         QLinkedList<QPoint> &mergedCellsPainted,
                         const Cell& cell, SheetView* sheetView );

  /**
   * \ingroup Painting
   * Paints the default cell borders.
   * \param painter the used painter
   * \param paintRegion the portion of the canvas that is actually in view
   * \param cellRegion the cell's dimension (should be replaced with paintCoordinate!)
   * \param cellCoordinate the cell position (should be removed!)
   * \param paintBorder the borders, that should be painted (should be removed???)
   * \param cellRange the cell range, that is painted (should be removed!)
   * \param cell the Cell (should be removed!)
   * \param sheetView the SheetView (should be removed!)
   */
  void paintDefaultBorders( QPainter& painter, const QRectF &paintRegion,
                            const QRectF &cellRegion, const QPoint &cellCoordinate,
                            Borders paintBorder, const QRect& cellRange,
                            const Cell& cell, SheetView* sheetView );

  /**
   * \return width of the text
   */
  double textWidth() const;

  /**
   * \return height of the text
   */
  double textHeight() const;

  QString testAnchor( const Cell& cell, double x, double y ) const;

    /**
     * \return the size of the obscured cell range
     * \note Used by SheetView to destroy the obscured CellViews.
     */
    QSize obscuredRange() const;

    QPoint obscuringCell() const;

    bool isObscured() const;
    bool obscuresCells() const;

    double cellHeight() const;
    double cellWidth() const;

    bool dimensionFits() const;

private:
    /**
     * \ingroup Layout
     * Calculates the layout of the cell.
     *
     * I.e. recalculates the text to be shown, its dimensions, its offset,
     * breaks lines of the text to fit it into the cell, obscures neighbouring
     * cells, if necessary.
     */
    void makeLayout( SheetView* sheetView, const Cell& cell );

  /**
   * \ingroup Layout
   * Determines the space needed for the text to be displayed.
   *
   * This depends on the following variables:
   * \li text direction (horizontal or vertical)
   * \li text angle
   * \li wether the text is underlined
   * \li vertical alignment
   *
   * There's a special treatment for bottom aligned, underlined text. For
   * all other combinations of these two variables the dimension is the same.
   *
   * \internal Called from makeLayout().
   */
  void textSize( const QFontMetrics& fontMetrics );

  /**
   * \ingroup Layout
   * Determines the text offset relative to the cell's top left corner.
   *
   * This depends on the following variables:
   * \li horizontal alignment
   * \li vertical alignment
   * \li text angle
   * \li text direction (horizontal or vertical)
   * \li text width and height
   * \li single or multiple rows
   * \li cell width and height, including obscured cells
   * \li thickness of the border pens
   *
   * \internal Called from makeLayout().
   */
  void textOffset( const QFontMetrics& fontMetrics, const Cell& cell );

  /**
   * \ingroup Layout
   * Checks wether the cell text could be wrapped
   * and does so, if it's necessary.
   *
   * \internal Called from makeLayout().
   */
  void breakLines( const QFontMetrics& fontMetrics );

  /**
   * \ingroup Layout
   * Determines the dimension of this cell including any merged cells.
   * Does NOT consider space, which is available from empty neighbours,
   * and would be able to be obscured.
   *
   * \internal Called from makeLayout().
   */
  void calculateCellDimension( const Cell& cell );

  /**
   * \ingroup Layout
   * Checks, wether horizontal neighbours could be obscured
   * and does so, if necessary.
   *
   * \internal Called from makeLayout().
   */
  void obscureHorizontalCells( SheetView* sheetView, const Cell& cell );

  /**
   * \ingroup Layout
   * Checks, wether vertical neighbours could be obscured
   * and does so, if necessary.
   *
   * \internal Called from makeLayout().
   */
  void obscureVerticalCells( SheetView* sheetView, const Cell& cell );

  /**
   * \ingroup Layout
   * \ingroup Painting
   * Determines the font to be used by evaluating the font settings.
   *
   * This depends on the following variables:
   * \li font family
   * \li font size
   * \li font flags
   * \li conditional formatting
   *
   * \return the effective font
   *
   * \internal
   */
  QFont effectiveFont( QPaintDevice* paintDevice ) const;

  /**
   * \ingroup Painting
   * Adjust the output text, so that it only holds the part that can be
   * displayed. The altered text is only temporary stored. It is restored
   * after the paint event has been processed.
   *
   * This depends on the following variables:
   * \li horizontal alignment
   * \li value format
   * \li text direction (horizontal or vertical)
   * \li indentation
   *
   * \internal Called from paintText().
   */
  QString textDisplaying( const QFontMetrics& fontMetrics, const Cell& cell );

  /**
   * \ingroup Painting
   * helper function for paintCell() function
   * @see paintCell()
   * @internal
   */
  void paintCustomBorders( QPainter& painter, const QRectF &paintRegion,
                           const QRectF &cellRegion, const QPoint& cellCoordinate,
                           Borders paintBorder );

  /**
   * \ingroup Painting
   * helper function for paintCell() function
   * @see paintCell()
   * @internal
   */
  void paintPageBorders( QPainter& painter, const QRectF &cellRect,
                         const QPoint &cellRef, Borders paintBorder, const Cell& cell );

  /**
   * \ingroup Painting
   * helper function for paintCell() function
   * @see paintCell()
   * @internal
   */
  void paintText( QPainter& painter, const QRectF &cellRect,
                  const QPoint &cellRef, QPaintDevice* paintDevice, const Cell& cell );

  /**
   * \ingroup Painting
   * helper function for paintCell() function
   * @see paintCell()
   * @internal
   */
  void paintMoreTextIndicator( QPainter& painter, const QRectF &cellRect );

  /**
   * \ingroup Painting
   * helper function for paintCell() function
   * @see paintCell()
   * @internal
   */
  void paintCommentIndicator( QPainter& painter, const QRectF &cellRect,
                              const Cell& cell );

  /**
   * \ingroup Painting
   * helper function for paintCell() function
   * @see paintCell()
   * @internal
   */
  void paintFormulaIndicator( QPainter& painter, const QRectF &cellRect, const Cell& cell );

  /**
   * \ingroup Painting
   * helper function for paintCell() function
   * @see paintCell()
   * @internal
   */
  void paintMatrixElementIndicator( QPainter& painter, const QRectF &cellRect, const Cell& cell );

  /**
   * \ingroup Painting
   * helper function for paintCell() function
   * @see paintCell()
   * @internal
   */
  void paintCellDiagonalLines( QPainter& painter, const QRectF &cellRect );

    /**
     * Tells this view that the Cell at \p col , \p row obscures this one.
     * If this view is destructed, the SheetView deletes the obscuring CellView.
     * If the obscuring CellView is destructed, the SheetView deletes this view.
     * \note obscuring is not the same as merging
     * \internal
     */
    void obscure( int col, int row );

    void drawText( QPainter& painter, const QFont& font,
                   const QPointF& location, const QString& text,
                   const Cell& cell ) const;

    /**
     * Default CellView used by SheetView.
     */
    CellView( SheetView* sheetView );

    // not allowed
    CellView& operator=( const CellView& );

    class Private;
    QSharedDataPointer<Private> d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(CellView::Borders)

} // namespace KSpread

Q_DECLARE_TYPEINFO( KSpread::CellView, Q_MOVABLE_TYPE );

#endif
