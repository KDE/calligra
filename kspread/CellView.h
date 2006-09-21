/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
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
   License as published by the Free Software Foundation; either
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

// KOffice
#include <KoPoint.h>

class QRectF;

namespace KSpread
{
class Cell;
class View;

class CellView
{
public:
  CellView( Cell* cell );
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
   * \ingroup Painting
   * Paints the cell.
   *
   * @param rect the portion of the canvas that is actually in view
   * @param painter the painter object to paint on
   * @param view the view of this data.  This may be 0, but no selection
   *        will be included with the painting.
   * @param coordinate coordinates on the painter where the top left corner
   *                    of the cell should be painted plus width and height
   * @param cellRef the column/row coordinates of the cell.
   * @param paintBorder a combination of flags from the Cell::Border
   *                    enum which specifies which cell borders to paint
   * @param rightPen pen to use to draw the right border
   *                 if @p paintBorder includes the RightBorder flag
   * @param bottomPen pen to use to draw the bottom border
   *                  if @p paintBorder includes the BottomBorder flag
   * @param leftPen pen to use to draw the left border
   *                if @p paintBorder includes the LeftBorder flag
   * @param topPen pen to use to draw the top border
   *               if @p paintBorder includes the TopBorder flag
   * @param mergedCellsPainted list of merged cells being painted
   */
  void paintCell( const QRectF & rect, QPainter & painter,
                  View * view, const KoPoint & coordinate,
                  const QPoint & cellRef, Borders paintBorder,
                  QPen& rightPen, QPen& bottomPen,
                  QPen& leftPen,  QPen& topPen,
                  QLinkedList<QPoint> &mergedCellsPainted );

  /**
   * \ingroup Layout
   * Calculates the layout of the cell.
   *
   * I.e. recalculates the text to be shown, its dimensions, its offset,
   * breaks lines of the text to fit it into the cell, obscures neighbouring
   * cells, if necessary.
   */
  void makeLayout( int _col, int _row );

  /**
   * \ingroup Layout
   * Calculates the text dimensions and the offset
   * for the current displayed text.
   */
  void calculateTextParameters();

  /**
   * \return width of the text
   */
  double textWidth() const;

  /**
   * \return height of the text
   */
  double textHeight() const;

  QString testAnchor( double x, double y ) const;

private:
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
   * \internal Called from makeLayout() and calculateTextParameters().
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
   * \internal Called from makeLayout() and calculateTextParameters().
   */
  void textOffset( const QFontMetrics& fontMetrics );

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
  void calculateCellDimension() const;

  void checkTextFitting();

  /**
   * \ingroup Layout
   * Checks, wether horizontal neighbours could be obscured
   * and does so, if necessary.
   *
   * \internal Called from makeLayout().
   */
  void obscureHorizontalCells();

  /**
   * \ingroup Layout
   * Checks, wether vertical neighbours could be obscured
   * and does so, if necessary.
   *
   * \internal Called from makeLayout().
   */
  void obscureVerticalCells();

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
  QFont effectiveFont( int _col, int _row ) const;

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
  QString textDisplaying( const QFontMetrics& fontMetrics );

  /**
   * \ingroup Painting
   * helper function for paintCell() function
   * @see paintCell()
   * @internal
   */
  void paintCellBorders( QPainter& painter, const QRectF &rect,
                         const QRectF &cellRect, const QPoint &cellRef,
                         Borders paintBorder,
                         QPen & rightPen, QPen & bottomPen,
                         QPen & leftPen, QPen & topPen );

  /**
   * \ingroup Painting
   * helper function for paintCell() function
   * @see paintCell()
   * @internal
   */
  void paintPageBorders( QPainter& painter, const QRectF &cellRect,
                         const QPoint &cellRef, Borders paintBorder );

  /**
   * \ingroup Painting
   * helper function for paintCell() function
   * @see paintCell()
   * @internal
   */
  void paintText( QPainter& painter, const QRectF &cellRect,
                  const QPoint &cellRef );

  /**
   * \ingroup Painting
   * helper function for paintCell() function
   * @see paintCell()
   * @internal
   */
  void paintMoreTextIndicator( QPainter& painter, const QRectF &cellRect,
                               QColor &backgroundColor );

  /**
   * \ingroup Painting
   * helper function for paintCell() function
   * @see paintCell()
   * @internal
   */
  void paintCommentIndicator( QPainter& painter, const QRectF &cellRect,
                              const QPoint &cellRef, QColor &backgroundColor );

  /**
   * \ingroup Painting
   * helper function for paintCell() function
   * @see paintCell()
   * @internal
   */
  void paintFormulaIndicator( QPainter& painter, const QRectF &cellRect,
                              QColor &backgroundColor );

  /**
   * \ingroup Painting
   * helper function for paintCell() function
   * @see paintCell()
   * @internal
   */
  void paintDefaultBorders( QPainter& painter, const QRectF &rect,
                            const QRectF &cellRect, const QPoint &cellRef,
                            Borders paintBorder,
                            QPen const & rightPen, QPen const & bottomPen,
                            QPen const & leftPen, QPen const & topPen );

  /**
   * \ingroup Painting
   * helper function for paintCell() function
   * @see paintCell()
   * @internal
   */
  void paintBackground( QPainter& painter, const QRectF &cellRect,
                        const QPoint &cellRef, bool selected,
                        QColor &backgroundColor );

  /**
   * \ingroup Painting
   * helper function for paintCell() function
   * @see paintCell()
   * @internal
   */
  void paintObscuredCells( const QRectF& rect, QPainter& painter,
                           View* view, const QRectF &cellRect,
                           const QPoint &cellRef, Borders paintBorder,
                           QPen & rightPen, QPen & bottomPen,
                           QPen & leftPen, QPen & topPen,
                           QLinkedList<QPoint> &mergedCellsPainted );

  /**
   * \ingroup Painting
   * helper function for paintCell() function
   * @see paintCell()
   * @internal
   */
  void paintCellDiagonalLines( QPainter& painter, const QRectF &cellRect,
                               const QPoint &cellRef );

  class Private;
  Private * const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(CellView::Borders)

} // namespace KSpread

#endif
