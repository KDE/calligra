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
class QDomElement;
class QDomDocument;

class KSParseNode;

#include <qstring.h>
#include <qpen.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qrect.h>
#include <qlist.h>
#include <qstrlist.h>
#include <qobject.h>
#include <qdatetime.h>

#include "kspread_layout.h"
#include "kspread_global.h"


/**
 */

struct KSpreadConditional
{
 double val1;
 double val2;
 QColor colorcond;
 QFont fontcond;
 Conditional m_cond;
};

struct KSpreadValidity
{
 QString message;
 QString title;
 double valMin;
 double valMax;
 Conditional m_cond;
 Action m_action;
 Allow m_allow;
 QTime  timeMin;
 QTime  timeMax;
 QDate  dateMin;
 QDate  dateMax;
};

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

    /**
     * Used in @ref KSpreadCell::copyAll to make a copy
     * of some cell.
     */
    virtual KSpreadCellPrivate* copy( KSpreadCell* cell ) = 0;

protected:
    KSpreadCell* m_pCell;
};

class SelectPrivate : public KSpreadCellPrivate
{
    Q_OBJECT
public:
    SelectPrivate( KSpreadCell* _cell ) : KSpreadCellPrivate( _cell ) { m_iIndex = -1; }
    virtual ~SelectPrivate() { }

    KSpreadCellPrivate* copy( KSpreadCell* cell );

    QString text() const;

    void parse( const QString& _text );

    QStringList m_lstItems;
    int m_iIndex;

public slots:
    void slotItemSelected( int _id );
};

/**
 * For every cell in the spread sheet there is a KSpreadCell object.
 *
 * KSpreadCell contains layout information and algorithm and it
 * contains the calculation algorithm.
 *
 * However, all empty cells are represented by one instace, called the
 * default cell. @ref #isDefault tells wether a cell is the default one
 * or not.
 */
class KSpreadCell : public KSpreadLayout
{
  friend class SelectPrivate;

public:
    enum Content { Text, RichText, Formula, VisualFormula };
    enum Style { ST_Normal, ST_Button, ST_Undef, ST_Select };

    KSpreadCell( KSpreadTable *_table, int _column, int _row );
    /**
     * @see #tableDies
     */
    ~KSpreadCell();

    KSpreadCell* previousCell() { return m_previousCell; }
    KSpreadCell* nextCell() { return m_nextCell; }
    void setPreviousCell( KSpreadCell* c ) { m_previousCell = c; }
    void setNextCell( KSpreadCell* c ) { m_nextCell = c; }

    /**
     * Moves around the cell. It cares about obscured and obscuring cells and
     * forces, relayout, calculation and redrawing of the cell.
     */
    void move( int column, int row );

    /**
     * The @ref KSpreadTable calls this method if the table becomes deleted.
     * At the time this method is called other cells may already be deleted or
     * in some inconsistent state.
     *
     * The purpose of this method is to clear up some variables so that the destructor
     * runs without any crash. Especially all variables referencing other cells
     * must be voided. This method may not call anz other method since the whole table
     * and all remaining cells are in an inconsistent state now.
     */
    void tableDies();

    virtual QDomElement save( QDomDocument& doc, int _x_offset = 0, int _y_offset = 0, bool fallBack=false );
    bool load( const QDomElement& cell, int _xshift, int _yshift, PasteMode pm = Normal,
	       Operation op = OverWrite );
    QTime toTime(QDomElement &element) ;
    QDate toDate(QDomElement &element);
    /**
    * save layout parameters when you copy cell
    * used fallback
    */
    QDomElement saveParameters( QDomDocument& doc ) const;

    /**
     * Copyies the layout from the cell at the position (_column|_row).
     *
     * @see #copyAll
     */
    void copyLayout( int _column, int _row );
    /**
     * A convenience function.
     *
     * @see #copyAll
     */
    void copyLayout( KSpreadCell *_cell );
    void copyContent( KSpreadCell *_cell );
    /**
     * Copies the layout and the content. It does not copy the @ref #m_row and @ref #m_column attributes.
     * Besides that all persistent attributes are copied. @ref #setCellText is called to set the real
     * content.
     *
     * @see #copyLayout
     */
    void copyAll( KSpreadCell *cell);

    /**
     * Paints the cell.
     */
    void paintCell( const QRect& _rect, QPainter &_painter, int _tx, int _ty,
                    int _col, int _row, ColumnLayout *cl, RowLayout *rl,
                    QRect *_prect = 0L, bool override_obscured = false );

    /**
     * A convenience function
     */
    void paintCell( const QRect & _ev, QPainter &_painter,
                    int _col, int _row,
                    QRect *_prect = 0L );

    /**
     * @return the column this cell is in. May return 0 if the cell is the default cell.
     */
    int column() const { return m_iColumn; }
    /**
     * @return the row this cell is in. May return 0 if the cell is the default cell.
     */
    int row() const { return m_iRow; }

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
     * @reimp
     */
    bool isDefault() const;

    /**
     * Tells whether this cell has any content.
     * An cell has no content if is has no text and no formula.
     *
     * @return TRUE if there is no content.
     */
    bool isEmpty() const;
    /**
     * @return TRUE if the cell should be printed in a print out.
     *         That si the case if it has any content, border, backgroundcolor,
     *         or background brush.
     *
     * @see KSpreadTable::print
     */
    bool needsPrinting() const;

    /**
     * Tells whether the cell contains, text, a formula, richtext or a visual formula.
     */
    Content content() const { return m_content; }

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
     * When we are in the progress of loading, then this function will only store the
     * text. KSpreadDoc::completeLoading takes care of updating dependencies etc.
     * For this, we call it with updateDepends=false.
     * Most of the time, call this with updateDepends=true so that dependencies are updated.
     */
    void setCellText( const QString& _text, bool updateDepends = true );

    void setDisplayText( const QString& _text, bool updateDepends = true );
    /**
     * @return the text the user entered.
     */
    QString text() const { return m_strText; }

    void setAction( const QString& _action ) { m_strAction = _action; }

    ////////////////////////////////
    //
    // Methods for querying layout stuff.
    //
    ////////////////////////////////

    /**
     * @reimp
     */
    const QPen& leftBorderPen( int col, int row ) const;

    /**
     * @reimp
     */
    const QPen& topBorderPen( int col, int row ) const;

    /**
     * @reimp
     */
    const QPen& rightBorderPen( int col, int row ) const;

    /**
     * @reimp
     */
    const QPen& bottomBorderPen( int col, int row ) const;

    /**
     * @reimp
     */
    const QColor& bgColor( int col, int row ) const;

    /**
     * @reimp
     */
    const QBrush& backGroundBrush( int col, int row ) const;

    ////////////////////////////////
    //
    // Methods for setting layout stuff.
    //
    ////////////////////////////////

    /**
     * @reimp
     */
    void setLeftBorderPen( const QPen& p );

    /**
     * @reimp
     */
    void setTopBorderPen( const QPen& p );

    /**
     * @reimp
     */
    void setRightBorderPen( const QPen& p );

    /**
     * @reimp
     */
    void setBottomBorderPen( const QPen& p );

    //////////////////////
    //
    // Other stuff
    //
    //////////////////////

    /**
     * @see #setStyle
     * @see #m_style
     */
    Style style() const { return m_style; }

    void setStyle( Style _s );
    /**
     * @see #setAction
     * @see #m_strAction
     */
    QString action() const { return m_strAction; }

    bool isValue() const { return m_bValue; }
    bool isBool() const {  return m_bBool; }
    bool valueBool() const { return ( m_dValue != 0.0 ); }
    bool isDate() const {  return m_bDate; }
    bool isTime() const {  return m_bTime; }
    double valueDouble() const { return m_dValue; }
    QString valueString() const;
    void setValue( double _d );
    QDate valueDate() const {return m_Date;}
    QTime valueTime() const {return m_Time;}

    /**
     * return size of the text
     */
    int textWidth() {return m_iOutTextWidth; }
    int textHeight() {return m_iOutTextHeight; }
    int richTextWidth() {return m_richWidth; }
    int richTextHeight() {return m_richHeight;}

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

    /**
    * Refreshing chart
    * bool refresh is default true
    * when it's false it's just for test
    * it's used when you paste cell
    */
    bool updateChart(bool refresh=true);

    QString testAnchor( int _x, int _y );

    /**
     * Called if the user clicks on a cell. If the cell is for example a button, then
     * @ref #m_strAction is executed.
     */
    void clicked( KSpreadCanvas *_canvas );

    /**
     * Starts calculating.
     * If a table is ok and you change this cell only, then you don't need to
     * calculate all cells this one depends on. If you don't know whether all cells
     * are calculated properly right now, you must set '_makedepend' to TRUE.
     * If cell c1 changed you can call 'c1->calc(FALSE)', but all cells depending
     * on c1 must be called with dep_on_c1->calc(TRUE) because the table is not ok
     * any more.
     *
     * @param _makedepend tells whether all other cells are calculated properly or not.
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
     * some cell specific layout value like font or text change.
     */
    virtual void setLayoutDirtyFlag();
    bool layoutDirtyFlag() const { return m_bLayoutDirtyFlag; }

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
    // virtual void print( QPainter &_painter, int _tx, int _ty, int _col, int _row,
    // ColumnLayout *_cl, RowLayout *_rl, bool _only_left,
    // bool _only_top, const QPen& _grid_pen );

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
    bool isObscured() const { return ( m_pObscuringCell != 0L ); }
    /**
     * If obscuring is forced then the marker may never reside on this cell.
     *
     * @return TRUE if the obscuring cell is forced to obscure this one.
     */
    bool isObscuringForced();

    /**
     * @return the column of the obscuring cell.
     */
    int obscuringCellsColumn() const { return m_iObscuringCellsColumn; }
    /**
     * @return the row of the obscuring cell.
     */
    int obscuringCellsRow() const { return m_iObscuringCellsRow; }

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
    bool isForceExtraCells() const { return m_bForceExtraCells; }

    /**
     * @return the amount of obscured cells in the horizontal direction
     */
    int extraXCells() const { return m_iExtraXCells; }
    /**
     * @return the amount of obscured cells in the vertical direction
     */
    int extraYCells() const { return m_iExtraYCells; }
    int extraWidth() const { return m_iExtraWidth; }
    int extraHeight() const { return m_iExtraHeight; }

    bool isFormular() const { return m_content == Formula; }

    QString encodeFormular( int _col = -1, int _row = -1 );
    QString decodeFormular( const char *_text, int _col = -1, int _row = -1 );

    /**
     * Merges the @p new_text with @p old_text during a paste operation.
     * If both texts represent doubles, then the operation is performed on both
     * values and the result is returned. If both texts represents a formular or
     * one a formular and the other a double value, then a formular is returned.
     * In all other cases @p new_text is returned.
     *
     * @return the merged text.
     */
    QString pasteOperation( QString new_text, QString old_text, Operation op );

    /**
     * @return TRUE if the cell contains a formula that could not
     *         be evaluated. These cells usually appear with "####" on the screen.
     */
    bool hasError() const { return m_bError; }

    /**
     * Calculates the layout of the cell.
     */
    virtual void makeLayout( QPainter &_painter, int _col, int _row );

    /**
     * Parses the formula.
     * Fills @ref #dependList and @ref #formula.
     * @return FALSE on error.
     */
    bool makeFormular();


    void defaultStyle();


    //valid or not conditionIsTrue
    void verifyCondition();
    KSpreadConditional * getFirstCondition(int newStruct=-1)
    	{
    	if((m_firstCondition==0)&&(newStruct==-1))
    		m_firstCondition=new KSpreadConditional;
    	return  m_firstCondition;
    	}

    KSpreadConditional * getSecondCondition(int newStruct=-1)
    	{
    	if((m_secondCondition==0)&&(newStruct==-1))
    		m_secondCondition=new KSpreadConditional;
    	return m_secondCondition;
    	}

    KSpreadConditional * getThirdCondition(int newStruct=-1)
    	{
    	if((m_thirdCondition==0)&&(newStruct==-1))
    		m_thirdCondition=new KSpreadConditional;
    	return  m_thirdCondition;
    	}
    void removeThirdCondition()
    	{
    	if(m_thirdCondition!=0)
    		delete m_thirdCondition;
    	m_thirdCondition=0;
    	}

    void removeSecondCondition()
    	{
    	if(m_secondCondition!=0)
    		delete m_secondCondition;
    	m_secondCondition=0;
    	}
    void removeFirstCondition()
    	{
    	if(m_firstCondition!=0)
    		delete m_firstCondition;
    	m_firstCondition=0;
    	}

    void verifyValidity();
    KSpreadValidity * getValidity(int newStruct=-1)
    	{
    	if((m_Validity==0)&&(newStruct==-1))
    		m_Validity=new KSpreadValidity;
    	return  m_Validity;
    	}
    void removeValidity()
    	{
    	if(m_Validity!=0)
    		delete m_Validity;
    	m_Validity=0;
    	}

     /**
     * return true if value is good
     * else show a messagebox
     */
     bool  testValidity();

    void conditionAlign(QPainter &painter,int _col,int _row);

    /**
    * return align X when align is undefined
    */
    int defineAlignX();


    /**
     * Used for comparing cells (when sorting)
     */
    bool operator > ( const KSpreadCell & ) const;
    bool operator < ( const KSpreadCell & ) const;

    /**
     * Reimplemented since obscured cells tell that they have
     * almost no property, even if they physically have.
     */
    virtual bool hasProperty( Properties p ) const;


    void freeAllObscuredCells();

protected:
    /**
     * @reimp
     */
    void layoutChanged();
    /**
     * @reimp
     */
    KSpreadLayout* fallbackLayout( int col, int row );
    /**
     * @reimp
     */
    const KSpreadLayout* fallbackLayout( int col, int row ) const;

    QString createFormat( double value, int col, int row );

    /**
     * When you insert a cell at bottom or right
     * and the size is not the same so text offset
     * will not good => recalc offset
     */
    void offsetAlign( int _col, int _row );

    /**
     * Called from @ref #makeLayout to determine the space
     * needed for the text.
     */
    void textSize( QPainter &_paint );

    /**
    *  Called from @ref #paintCell to determine the text
     * wich can be displaying.
    */
    QString textDisplaying( QPainter &painter);

    /**
     * Cleans up formula stuff.
     * Call this before you store a new formula or to delete the
     * formula.
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
     * Automatically chooses between a number format and
     * a scientific format (if the number is too big)
     */
    void checkNumberFormat();

    /**
     * This cells row.
     * If it is 0, this is the default cell and its row/column can
     * not be determined.
     *
     * @persistent
     */
    int m_iRow;
    /**
     * This cells column.
     * If it is 0, this is the default cell and its row/column can
     * not be determined.
     *
     * @persistent
     */
    int m_iColumn;

    /**
     * Holds the users input
     *
     * @persistent
     */
    QString m_strText;

    /**
     * This is the text we want to print
     */
    QString m_strOutText;
    int m_iOutTextWidth;
    int m_iOutTextHeight;
    int m_iTextX, m_iTextY;
    /**
     * Result of "fm.ascent()" in makeLayout.
     * used in offsetAlign.
     */
    int m_fmAscent;

    double m_dValue;
    bool m_bValue;
    bool m_bBool;
    bool m_bError;
    bool m_bDate;
    bool m_bTime;
    QDate m_Date;
    QTime m_Time;
    /**
     * Flag showing whether the current layout is OK.
     * If you change for example the fonts point size, set this flag. When the cell
     * must draw itself on the screen it will first recalculate its layout.
     */
    bool m_bLayoutDirtyFlag;

    /**
     * Shows whether recalculation is neccessary.
     * If this cell must be recalculated for some reason, for example the user entered
     * a new formula, then this flag is set. If @ref #bFormular is FALSE nothing will happen
     * at all.
     */
    bool m_bCalcDirtyFlag;

    QList<KSpreadDepend> m_lstDepends;

    /**
     * The value we got from calculation.
     * If @ref #isFormular is TRUE, @ref #makeLayout will use @ref #m_strFormularOut
     * instead of @ref m_strText since m_strText stores the formula the user entered.
     */
    QString m_strFormularOut;

    /**
     * Tells whether this cell it currently under calculation.
     * If a cell thats 'progressFlag' is set is told to calculate we
     * have detected a circular reference and we must stop calulating.
     */
    bool m_bProgressFlag;

    /**
     * If this flag is set, then it is known that this cell has to be updated
     * on the display. This means that somewhere in the calling stack there is a
     * function which will call @ref KSpreadTable::updateCell once it retains
     * the control. If a function changes the contents/layout of this cell and this
     * flag is not set, then the function must set it at once. After the changes are
     * done the function must call <tt>m_pTable->updateCell(...).
     * The flag is cleared by the function m_pTable->updateCell.
     */
    bool m_bDisplayDirtyFlag;

    /**
     * The amount of additional cells horizontal
     *
     * @persistent
     */
    int m_iExtraXCells;
    /**
     * The amount of additional cells vertical
     *
     * @persistent
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
     * Tells whether the cell is forced to exceed its size.
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

    /**
     * Tells wether the cell is a button, combobox etc.
     *
     * @persistent
     */
    Style m_style;

    /**
     * Used for example if the cell is displayed as a button.
     * It tells which command has to be executed.
     *
     * @see #action
     * @see #setAction
     *
     * @persistent
     */
    QString m_strAction;

    KSpreadCellPrivate *m_pPrivate;

    /**
     * Tells which kind of content the cell holds.
     *
     * @see #content
     * @see #setCellText
     */
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

    /**
     * A pointer to the decimal separator
     */
    static QChar decimal_point;

    /**
     * Width of richText
     */
    int m_richWidth;
    /**
     * Height of richText
     */
    int m_richHeight;

    /**
     * Set to TRUE if one of the conditions apply.
     *
     * @see #m_firstCondition
     * @see #m_secondCondition
     * @see #m_thirdCondition
     * @see #m_numberOfCond
     */
    bool m_conditionIsTrue;

    /**
     * Pointer to the first condition. May be 0.
     *
     * @persistent
     */
    KSpreadConditional *m_firstCondition;
    /**
     * Pointer to the second condition. May be 0.
     *
     * @persistent
     */
    KSpreadConditional *m_secondCondition;
    /**
     * Pointer to the third condition. May be 0.
     *
     * @persistent
     */
    KSpreadConditional *m_thirdCondition;
    /**
     * If a condition apples, then this variable tells which.
     * The value if always in the range of 0..2 if a condition
     * applies and undefined otherwise.
     */
    int m_numberOfCond;

    /**
    * Store the number of line when you used multirow
    * default is 0
    */
    int m_nbLines;

    KSpreadValidity * m_Validity;

    /**
    * When it's True displays **
    * it's true when text size is bigger that cell size
    * and when Align is center or left
    */
    bool m_bCellTooShort;

    KSpreadCell* m_nextCell;
    KSpreadCell* m_previousCell;

};

#endif
