/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __kspread_cell_h__
#define __kspread_cell_h__

class KSpreadCell;
class KSpreadTable;
class KSpreadCanvas;

class KFormula;

class QSimpleRichText;

class KSParseNode;

#include <iostream.h>

#include <qstring.h>
#include <qpen.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qrect.h>
#include <qlist.h>
#include <qstrlist.h>
#include <qobject.h>
#include <qdom.h>

#include "kspread_layout.h"

/**
 */
struct KSpreadDepend
{
  int m_iColumn;
  int m_iRow;
  KSpreadTable *m_pTable;
  /**
   * Column of second corner
   *  If this is set to -1, no second corner is defined.
   */
  int m_iColumn2;
  int m_iRow2;
};

class KSpreadCellPrivate : public QObject
{
  Q_OBJECT
public:
  KSpreadCellPrivate( KSpreadCell* _cell ) { m_pCell = _cell; }
  virtual ~KSpreadCellPrivate() { }

protected:
  KSpreadCell* m_pCell;
};

class SelectPrivate : public KSpreadCellPrivate
{
  Q_OBJECT
public:
  SelectPrivate( KSpreadCell* _cell ) : KSpreadCellPrivate( _cell ) { m_iIndex = -1; }
  virtual ~SelectPrivate() { }

  const char* text();

  void parse( const char* _text );

  QStrList m_lstItems;
  int m_iIndex;

public slots:
   void slotItemSelected( int _id );
};

/**
 */
class KSpreadCell : public KSpreadLayout
{
  friend SelectPrivate;

public:
    enum Style { ST_Normal, ST_Button, ST_Undef, ST_Select };
    enum Content { Text, RichText, Formula, VisualFormula };

    KSpreadCell( KSpreadTable *_table, int _column, int _row, const char* _text = 0L );
    ~KSpreadCell();

    virtual QDomElement save( QDomDocument&, int _x_offset = 0, int _y_offset = 0 );
    virtual bool load( const QDomElement& e ) { return load( e, 0, 0 ); }
    virtual bool load( const QDomElement&, int _xshift, int _yshift );

    /**
     * Copyies the layout from the cell at the position (_column|_row).
     */
    void copyLayout( int _column, int _row );
    /**
     * A convenience function.
     */
    void copyLayout( KSpreadCell *_cell ) { copyLayout( _cell->column(), _cell->row() ); }

    /**
     * Paints the cell.
     */
    virtual void paintEvent( KSpreadCanvas *_canvas, const QRect& _rect, QPainter &_painter, int _tx, int _ty,
			     int _col, int _row, ColumnLayout *cl, RowLayout *rl, QRect *_prect = 0L );

    /**
     * A convenience function
     */
    virtual void paintEvent( KSpreadCanvas *_canvas, const QRect & _ev, QPainter &_painter, int _col, int _row,
			     QRect *_prect = 0L );

    /**
     * @return the column this cell is in. May return 0 if the cell is the default cell.
     */
    int column() { return m_iColumn; }
    /**
     * @return the row this cell is in. May return 0 if the cell is the default cell.
     */
    int row() { return m_iRow; }

    /**
     * @param _col the column this cell is assumed to be in.
     *             This parameter defaults to the return value of @ref #column.
     *
     * @return the width of this cell
     */
    int width( int _col = -1, KSpreadCanvas *_canvas = 0L );

    /**
     * @param _row the row this cell is assumed to be in.
     *
     * @return the height of this cell
     */
    int height( int _row = -1, KSpreadCanvas *_canvas = 0L );

    /**
     * @return TRUE if this cell is the default cell.
     */
    virtual bool isDefault() { return ( m_iColumn == 0 ); }

    /**
     * Tells wether this cell has any content.
     * An cell has no content if is has no text and no formular.
     *
     * @return TRUE if there is no content.
     */
    virtual bool isEmpty();

    /**
     * Tells wether the cell contains, text, a formula, richtext or a visual formula.
     */
    Content content() { return m_content; }

    /**
     * @return the text the user entered.
     */
    QString text() { return m_strText; }

    /**
     * Increases the precison of the
     * value displayed. Precision means here the amount of
     * digits behind the dot. If the current precision is the
     * default of -1, then it is set to the number of digits
     * behind the dot plus 1.
     */
    void incPrecision();
    /**
     * Decreases the precison of the
     * value displayed. Precision means here the amount of
     * digits behind the dot. If the current precision is the
     * default of -1, then it is set to the number of digits
     * behind the dot minus 1.
     */
    void decPrecision();

    /**
     * Set the column this cell is now in. This function is usually used after the 'paste'
     * command.
     */
    void setColumn( int _c ) { m_iColumn = _c; }
    /**
     * Set the row this cell is now in. This function is usually used after the 'paste'
     * command.
     */
    void setRow( int _r ) { m_iRow = _r; }

    /**
     * When we are in the progress of loading, then this function will only store the
     * text. Call @ref #initAfterLoading afterwards to complete this functions job.
     */
    void setText( const QString& _text );
    void setAlign( Align _align ) { m_eAlign = _align; m_bLayoutDirtyFlag = TRUE; }
    void setFaktor( double _d ) { m_dFaktor = _d; m_bLayoutDirtyFlag = TRUE; }

    void setPrefix( const char * _prefix );
    void setPostfix( const char * _postfix );
    void setPrecision( int _p ) { m_bLayoutDirtyFlag = TRUE; m_iPrecision = _p; }

    void setLeftBorderStyle( Qt::PenStyle _s ) { m_leftBorderPen.setStyle( _s ); m_bLayoutDirtyFlag = TRUE; }
    void setTopBorderStyle( Qt::PenStyle _s ) { m_topBorderPen.setStyle( _s ); m_bLayoutDirtyFlag = TRUE; }
    void setRightBorderStyle( Qt::PenStyle _s );
    void setBottomBorderStyle( Qt::PenStyle _s );

    void setLeftBorderColor( const QColor & _c ) { m_leftBorderPen.setColor( _c ); m_bLayoutDirtyFlag = TRUE; }
    void setTopBorderColor( const QColor & _c ) { m_topBorderPen.setColor( _c ); m_bLayoutDirtyFlag = TRUE; }
    void setRightBorderColor( const QColor & _c );
    void setBottomBorderColor( const QColor & _c );

    void setLeftBorderWidth( int _w ) { m_iLeftBorderWidth = _w; m_bLayoutDirtyFlag = TRUE; }
    void setTopBorderWidth( int _w ) { m_iTopBorderWidth = _w; m_bLayoutDirtyFlag = TRUE; }
    void setRightBorderWidth( int _w );
    void setBottomBorderWidth( int _w );

    virtual void setTextFontSize( int _s ) { m_textFont.setPointSize( _s ); m_bLayoutDirtyFlag = TRUE; }
    virtual void setTextFontFamily( const char *_f )
    { m_textFont.setFamily( _f ); m_bLayoutDirtyFlag = TRUE; }
    virtual void setTextFontBold( bool _b )
    { m_textFont.setBold( _b ); m_bLayoutDirtyFlag = TRUE; }
    virtual void setTextFontItalic( bool _i )
    { m_textFont.setItalic( _i ); m_bLayoutDirtyFlag = TRUE; }
    void setTextFont( const QFont& _f ) { m_textFont = _f; m_bLayoutDirtyFlag = TRUE; }
    void setTextColor( const QColor & _c ) { m_textColor = _c; m_bLayoutDirtyFlag = TRUE; }
    void setBgColor( const QColor & _c ) { m_bgColor = _c; m_bLayoutDirtyFlag = TRUE; }

    void setFloatFormat( FloatFormat _f ) { m_eFloatFormat = _f; m_bLayoutDirtyFlag = TRUE; }
    void setFloatColor( FloatColor _c ) { m_eFloatColor = _c; m_bLayoutDirtyFlag = TRUE; }

    void setMultiRow( bool _b ) { m_bMultiRow = _b; m_bLayoutDirtyFlag = TRUE; }

    void setStyle( Style _s );
    void setAction( const char* _action ) { m_strAction = _action; }

    /**
     * Since the GUI supports zooming, you can get the value zoomed
     * or not scaled. The not scaled value may be of interest in a
     * layout dialog for example.
     *
     * @param _col the column this cell is assumed to be in
     * @param _row the row this cell is assumed to be in
     *
     * @return the border width of the left border
     */
    int leftBorderWidth( int _col, int _row, KSpreadCanvas *_canvas = 0L );
    int topBorderWidth( int _col, int _row, KSpreadCanvas *_canvas = 0L );
    int rightBorderWidth( int _col, int _row, KSpreadCanvas *_canvas = 0L );
    int bottomBorderWidth( int _col, int _row, KSpreadCanvas *_canvas = 0L );

    /**
     * @param _col the column this cell is assumed to be in
     * @param _row the row this cell is assumed to be in
     *
     * @return the style used to draw the left border
     */
    Qt::PenStyle leftBorderStyle( int _col, int _row );
    Qt::PenStyle topBorderStyle( int _col, int _row );
    Qt::PenStyle rightBorderStyle( int _col, int _row );
    Qt::PenStyle bottomBorderStyle( int _col, int _row );

    /**
     * @param _col the column this cell is assumed to be in
     * @param _row the row this cell is assumed to be in
     *
     * @return the background color.
     */
    const QColor& bgColor( int _col, int _row );

    Style style() { return m_style; }
    QString action() { return m_strAction; }

    /**
     * @param _col the column this cell is assumed to be in
     * @param _row the row this cell is assumed to be in
     *
     * @return the color of the left color.
     */
    const QColor& leftBorderColor( int _col, int _row );
    const QColor& topBorderColor( int _col, int _row );
    const QColor& rightBorderColor( int _col, int _row );
    const QColor& bottomBorderColor( int _col, int _row );

    bool isValue() const { return m_bValue; }
    bool isBool() const {  return m_bBool; }
    bool valueBool() const { return ( m_dValue != 0.0 ); }
    double valueDouble() const { return m_dValue; }
    QString valueString();
    void setValue( double _d );

    /**
     * Like @ref updateDepending, but the cells content will be refreshed
     * on all views.
     */
    void update();
    /**
     * If the contents of this cell changed, then this function has
     * to be called so that all depending cells/charts etc.
     * become updated. This does even work if the cell was already
     * removed from the table.
     *
     * The cells content is not redisplayed and no flag of this cell is altered.
     */
    void updateDepending();

    QString testAnchor( int _x, int _y, QWidget* _canvas );

    /**
     * Called if the user clicks on a cell. If the cell is for example a button, then
     * @ref #m_strAction is executed.
     */
    void clicked( KSpreadCanvas *_canvas );

    /**
     * Starts calculating.
     * If a table is ok and you change this cell only, then you dont need to
     * calculate all cells this one depends on. If you dont know wether all cells
     * are caculated properly right now, you must set '_makedepend' to TRUE.
     * If cell c1 changed you can call 'c1->calc(FALSE)', but all cells depending
     * on c1 muts be called with dep_on_c1->calc(TRUE) because the table is not ok
     * any more.
     *
     * @param _makedepend tells wether all other cells are calculated properly or not.
     *
     * @return TRUE on success and FALSE on error.
     */
    bool calc( bool _makedepend = FALSE );

    /**
     * Set the calcDirtyFlag
     */
    void setCalcDirtyFlag() { if ( m_content == Formula ) m_bCalcDirtyFlag = TRUE; }
    bool calcDirtyFlag() { if ( m_content == Formula ) return false; return m_bCalcDirtyFlag; }

    /**
     * Sets the calcDirtyFlag if this cell depends on a given cell.
     * If cell (_column|_row) in table '_table' changed and we depend on this cells value, the
     * 'calcDirtyFlag' will be set, otherwise not.
     * After this ALL cells will be called with:
     * <TT>"cell->setCalcDirtyFlag( column, row)"</TT>.
     *
     * @param _table the table in which a cell changed its content
     * @param _column the column of the cell that changed its content
     * @param _row the row of the cell that changed its content
     */
    void setCalcDirtyFlag( KSpreadTable* _table, int _column, int _row );

    /**
     * Causes the layout to be recalculated when the cell is drawn next time.
     * This flag is for example set if the width of the column changes or if
     * some cell specific layout valued like font or text change.
     */
    virtual void setLayoutDirtyFlag();

    void clearDisplayDirtyFlag() { m_bDisplayDirtyFlag = false; }
    void setDisplayDirtyFlag() { m_bDisplayDirtyFlag = true ; }

    /**
     * Print the cell.
     *
     * @param _tx is the left offset.
     * @param _ty is the top offset.
     * @param _col the column this cell is assumed to be in
     * @param _row the row this cell is assumed to be in
     * @param _cl the @ref ColunmKSpreadLayout for this cell
     * @param _rl the @ref RowLayout for this cell
     * @param _only_left_border' set to draw only the left border. Since every cell paints its
     *                           left and top border only, you need to paint the right cells
     *                           left border to get the final right border.
     * @param _only_top_border' set to draw only the top border.
     */
    virtual void print( QPainter &_painter, int _tx, int _ty, int _col, int _row,
			ColumnLayout *_cl, RowLayout *_rl, bool _only_left,
			bool _only_top, const QPen& _grid_pen );

    /**
     * Tells this cell that the @ref KSpreadCell '_cell' obscures this one.
     * If this cell has to be redrawn, then the obscuring cell is redrawn instead.
     *
     * @param _cell the obscuring cell
     * @param _col is the column of the obscuring cell.
     * @param _row is the row of the obscuring cell.
     */
    void obscure( KSpreadCell *_cell, int _col, int _row );
    /**
     * Tells this cell that it is no longer obscured.
     */
    void unobscure();
    /**
     * @return TRUE if this cell is obscured by another.
     */
    bool isObscured() { return ( m_pObscuringCell != 0L ); }
    /**
     * If obscuring is forced then the marker may never reside on this cell.
     *
     * @return TRUE if the obscuring cell is forced to obscure this one.
     */
    bool isObscuringForced();

    /**
     * @return the column of the obscuring cell.
     */
    int obscuringCellsColumn() { return m_iObscuringCellsColumn; }
    /**
     * @return the row of the obscuring cell.
     */
    int obscuringCellsRow() { return m_iObscuringCellsRow; }

    /**
     * Force the cell to occupy other cells space.
     * If '_x' and '_y' are 0 then the forcing is disabled.
     *
     * @param _col is the column this cell is assumed to be in.
     * @param _row is the row this cell is assumed to be in.
     * @param _x tells to occupy _x additional cells in the horizontal
     * @param _y tells to occupy _y additional cells in the horizontal
     *
     */
    void forceExtraCells( int _col, int _row, int _x, int _y );

    /**
     * @return TRUE if the cell is forced to obscure other cells.
     */
    bool isForceExtraCells() { return m_bForceExtraCells; }
    /**
     * @return the amount of obscured cells in the horizontal direction
     */
    int extraXCells() { return m_iExtraXCells; }
    /**
     * @return the amount of obscured cells in the vertical direction
     */
    int extraYCells() { return m_iExtraYCells; }

    bool isFormular() { return m_content == Formula; }

    QString encodeFormular( int _col = -1, int _row = -1 );
    QString decodeFormular( const char *_text, int _col = -1, int _row = -1 );

    /**
     * @return TRUE if the cell contains a formula that could not
     *         be evaluated. These cells usually appear with "####" on the screen.
     */
    bool hasError() { return m_bError; }

protected:

    virtual void makeLayout( QPainter &_painter, int _col, int _row );
    /**
     * Parses the formular.
     * Fills @ref #dependList and @ref #formular.
     * @return FALSE on error.
     */
    bool makeFormular();

    /**
     * Cleans up formular stuff.
     * Call this before you store a new formular or to delete the
     * formular.
     */
    void clearFormular();

    /**
     * Parses a cell description ( like $A1 or so ).
     * If '_second' is set, it will store the data in '**_dep' as second corner.
     * Make shure that '**_dep' is a valid address in this case.
     * Otherwise a new KSpreadDepend will be created and the data is stored as first corner.
     * A pointer to the new KSpreadDepend will be stored in '**_dep*'.
     *
     * @return FALSE on error, what means that the string starting at '_p' is not a valid
     *         cell description. If an error is detected, '**dep' nor '*dep' get modified.
     */
    // bool makeDepend( const char *_p, KSpreadDepend ** _dep, bool _second = FALSE );

    /**
     * Set the @ref #m_bValue flag.
     * If the cell holds a numerical value ( calculated or as a constant value )
     * @ref #m_bValue is set to TRUE otherwise to FALSE. If the @ref #m_bValue changed, the
     * @ref #m_bLayoutDirtyFlag is set to TRUE, since this will cause the alignment
     * to change ( if align == KSpreadCell::Undefined ).
     * If @ref #m_bValue is set, a new value for @ref m_dValue is calculated.
      */
    void checkValue();

    /**
     * This cells row.
     * If it is 0, this is the default cell and its row/column can
     * not be determined.
     */
    int m_iRow;
    /**
     * This cells column.
     * If it is 0, this is the default cell and its row/column can
     * not be determined.
     */
    int m_iColumn;

    /**
     * Holds the users input
     */
    QString m_strText;

    /**
     * This is the text we want to print
     */
    QString m_strOutText;
    int m_iOutTextWidth;
    int m_iOutTextHeight;
    int m_iTextX, m_iTextY;

    double m_dValue;
    bool m_bValue;
    bool m_bBool;
    bool m_bError;

    /**
     * Flag showing wether the current layout is OK.
     * If you change for example the fonts point size, set this flag. When the cell
     * must draw itself on the screen it will first recalculate its layout.
     */
    bool m_bLayoutDirtyFlag;

    /**
     * Shows wether recalculation is neccessary.
     * If this cell must be recalculated for some reason, for example the user entered
     * a new formular, then this flag is set. If @ref #bFormular is FALSE nothing will happen
     * at all.
     */
    bool m_bCalcDirtyFlag;

    QList<KSpreadDepend> m_lstDepends;
    /**
     * The value we got from calculation.
     * If @ref #isFormular is TRUE, @ref #makeLayout will use @ref #m_strFormularOut
     * instead of @ref m_strText since m_strText stores the formular the user entered.
     */
    QString m_strFormularOut;

    /**
     * Tells wether this cell it currently under calculation.
     * If a cell thats 'progressFlag' is set is told to calculate we
     * have detected a circel reference and we must stop calulating.
     */
    bool m_bProgressFlag;

    /**
     * If this flag is set, then it is known that this cell has to be updated
     * on the display. This means that somewhere in the calling stack there is a
     * function which will call @ref KSpreadTable::emit_updateCell once it retaines
     * the control. If a function changes the contents/layout of this cell and this
     * flag is not set, then the function must set it at once. After the changes are
     * done the function must call <tt>m_pTable->emit_updateCell(...).
     * The flag is cleared by the function m_pTable->emit_updateCell.
     */
    bool m_bDisplayDirtyFlag;

    /**
     * The amount of additional cells horizontal
     */
    int m_iExtraXCells;
    /**
     * The amount of additional cells vertical
     */
    int m_iExtraYCells;

    /**
     * If a cell overlapps other cells, then we have the cells width stored here.
     * This value does not mean anything unless @ref #m_iExtraXCells is different from 0.
     */
    int m_iExtraWidth;
    /**
     * If a cell overlapps other cells, then we have the cells height stored here.
     * This value does not mean anything unless @ref m_iExtraYCells is different from 0.
     */
    int m_iExtraHeight;

    /**
     * Tells wether the cell is forced to exceed its size.
     * Cells may occupy other cells space on demand. But you may force
     * a cell to do so by setting this flag. Forcing the cell to have
     * no extra size will disable this flag!
     */
    bool m_bForceExtraCells;

    /**
     * The @ref KSpreadCell that obscures this one.
     * If this pointer is not NULL, then this cell is obscured by another
     * enlarged object. This means that we have to call this object in order
     * of painting it for example instead of painting 'this'.
     */
    KSpreadCell *m_pObscuringCell;
    /**
     * The column of the cell obscuring this one
     */
    int m_iObscuringCellsColumn;
    /**
     * The row of the cell obscuring this one
     */
    int m_iObscuringCellsRow;

    Style m_style;
    QString m_strAction;

    KSpreadCellPrivate *m_pPrivate;

    Content m_content;

    /**
     * Perhaps this cell contains QML ?
     */
    QSimpleRichText *m_pQML;
    /**
     * Perhaps this cell contains a visual formula ?
     */
    KFormula* m_pVisualFormula;

    /**
     * The parse tree of the real formula (e.g: "=A1*A2").
     */
    KSParseNode* m_pCode;
};

#endif
