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


class QSimpleRichText;
class QDomElement;
class QDomDocument;

class KSParseNode;


#include <qpainter.h>
#include <qptrlist.h>
#include <qstrlist.h>
#include <qdatetime.h>

#include "kspread_layout.h"
#include "kspread_global.h"
#include "kspread_depend.h"
#include "kspread_condition.h"

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
  friend class KSpreadConditions;
public:
    /** The type of content in the cell */
    enum Content { Text, RichText, Formula, VisualFormula };
    /**
     * The type of data in the cell, for Content == Text or Formula.
     * RichText always is StringData, and VisualFormula is OtherData
     * If you edit this, keep s_dataTypeToString uptodate.
     */
    enum DataType { StringData = 0, BoolData, NumericData, DateData, TimeData,
                    OtherData, // see docu
                    LastDataType = OtherData };
    /** */
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
     * must be voided. This method may not call any other method since the whole table
     * and all remaining cells are in an inconsistent state now.
     */
    void tableDies();

    /**
     * Save this cell.
     * @param _x_offset ...
     * @param _y_offset ...
     * @param force if set to true, all the properties of the layout are stored (used for "Copy")
     * Usually this is false, to only store the properties explicitely set.
     */
    QDomElement save( QDomDocument& doc, int _x_offset = 0, int _y_offset = 0, bool force = false );

    bool load( const QDomElement& cell, int _xshift, int _yshift, PasteMode pm = Normal,
	       Operation op = OverWrite );

    QTime toTime(QDomElement &element) ;
    QDate toDate(QDomElement &element);

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
     *
     * @param rect the portion of the canvas that is actually in view
     * @param painter the painter object to paint on
     * @param corner coordinates on the painter where the top left corner
     *               of the cell should be painted
     * @param cellRef the column/row coordinates of the cell.
     * @param drawCursor whether to draw the cursor and selection or not
     */
    void paintCell( const QRect& rect, QPainter &painter, QPoint corner,
                    QPoint cellRef, bool drawCursor=true );

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
     * Tells what is the content of the cell (its nature).
     * Text, formula, richtext, or visual formula.
     * Set by @ref setDisplayText().
     */
    Content content() const { return m_content; }

    /**
     * Tells what is the type of data in the cell:
     * string, numeric, date, time etc.
     * Set by @ref checkTextInput(), and by @ref load().
     */
    DataType dataType() const { return m_dataType; }

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
     * The high-level method for setting text, when the user inputs it.
     * It will revert back to the old text if testValidity() returns action==stop.
     * @param updateDepends whether to recalculate the dependent cells (always true nowadays)
     */
    void setCellText( const QString& _text, bool updateDepends = true );

    /**
     * Sets the text in the cell when the user inputs it.
     * Will determine the type of contents automatically.
     * Called by setCellText.
     */
    void setDisplayText( const QString& _text, bool updateDepends = true );

    /**
     * @return the text the user entered.
     */
    QString text() const { return m_strText; }

    QString strOutText() const {return m_strOutText;}

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

    /**
     * Return the format of this cell.
     * Convenience method for KSpreadLayout::getFormatType
     * Note that this is "how the user would like the data to be displayed if possible".
     * If he selects a date format, and the cell contains a string, we won't apply that format.
     */
    FormatType formatType() const { return getFormatType( m_iColumn, m_iRow ); }

    bool isString() const { return m_dataType == StringData; }
    bool isNumeric() const { return m_dataType == NumericData; }
    bool isBool() const { return m_dataType == BoolData; }
    bool isDate() const { return m_dataType == DateData; }
    bool isTime() const { return m_dataType == TimeData; }

    bool valueBool() const { return ( m_dValue != 0.0 ); }
    double valueDouble() const { return m_dValue; }
    QString valueString() const;
    QDate valueDate() const { return m_Date; }
    QTime valueTime() const { return m_Time; }

    void setValue( double _d );

    /**
     * return size of the text
     */
    int textWidth() {return m_iOutTextWidth; }
    int textHeight() {return m_iOutTextHeight; }

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
    bool calc();

    /**
     * Set the calcDirtyFlag
     */
    void setCalcDirtyFlag();

    bool calcDirtyFlag();

    /**
     * Test to see if this cell depends on the given cell
     *
     * @param table table of the cell we're curious about
     * @param col column of the cell we're curious about
     * @param row row of the cell we're curious about
     *
     * @return TRUE if this cell depends on the given cell
     */
    bool cellDependsOn(KSpreadTable *table, int col, int row);

    /**
     * Notify this cell that another cell is depending, or no longer depending on this cell's value
     *
     * @param col the column of the cell
     * @param row the row of the cell
     * @param table the table that the cell is on
     * @param isDepending true if the cell is now depending on this one, false if it is not any longer
     *                    depending on it.
     */
    void NotifyDepending( int col, int row, KSpreadTable* table, bool isDepending );

    /**
     * Causes the layout to be recalculated when the cell is drawn next time.
     * This flag is for example set if the width of the column changes or if
     * some cell specific layout value like font or text change.
     */
    virtual void setLayoutDirtyFlag();
    bool layoutDirtyFlag() const;

    void clearDisplayDirtyFlag();
    void setDisplayDirtyFlag();

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
    void obscure( KSpreadCell *_cell );
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
    int obscuringCellsColumn();
    /**
     * @return the row of the obscuring cell.
     */
    int obscuringCellsRow();

    /**
     * Force the cell to occupy other cells space.
     * If '_x' and '_y' are 0 then the forcing is disabled.
     *
     * @param _col is the column this cell is assumed to be in.
     * @param _row is the row this cell is assumed to be in.
     * @param _x tells to occupy _x additional cells in the horizontal
     * @param _y tells to occupy _y additional cells in the vertical
     *
     */
    void forceExtraCells( int _col, int _row, int _x, int _y );

    /**
     * @return TRUE if the cell is forced to obscure other cells.
     */
    bool isForceExtraCells() const;

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

    bool isFormula() const { return m_content == Formula; }

    QString encodeFormula( int _col = -1, int _row = -1 );
    QString decodeFormula( const QString &_text, int _col = -1, int _row = -1 );

    /**
     * Merges the @p new_text with @p old_text during a paste operation.
     * If both texts represent doubles, then the operation is performed on both
     * values and the result is returned. If both texts represents a formula or
     * one a formula and the other a double value, then a formula is returned.
     * In all other cases @p new_text is returned.
     *
     * @return the merged text.
     */
    QString pasteOperation( QString new_text, QString old_text, Operation op );

    /**
     * @return TRUE if the cell contains a formula that could not
     *         be evaluated. These cells usually appear with "####" on the screen.
     */
    bool hasError() const;

    /**
     * Calculates the layout of the cell, i,e, determines what should be shown
     * for this cell, m_strOutText.
     */
    void makeLayout( QPainter &_painter, int _col, int _row );

    /**
     * Parses the formula.
     * Fills @ref #dependList and @ref #formula.
     * @return FALSE on error.
     */
    bool makeFormula();


    void defaultStyle();

    /**
     * Gets a copy of the list of current conditions
     */
    QValueList<KSpreadConditional> GetConditionList();

    /**
     * Replace the old set of conditions with a new one
     */
    void SetConditionList(QValueList<KSpreadConditional> newList);

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

    void freeAllObscuredCells();

    QString dataTypeToString( DataType dt ) const;
    DataType stringToDataType( const QString& str ) const;

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

    /**
     * Format a numeric value (isNumeric()==true) using the user-specified format
     */
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
    void clearFormula();

    /**
     * Check the input from the user, and determine the contents of the cell accordingly
     * (in particular the data type).
     * This is to be called only when m_content == Text.
     *
     * Input: m_strText
     * Output: m_dataType
     */
    void checkTextInput();

    // Try to parse the text as a bool/number/date/time/etc.
    // Helpers for checkTextInput.
    bool tryParseBool( const QString& str );
    bool tryParseNumber( const QString& str );
    bool tryParseDate( const QString& str );
    bool tryParseTime( const QString& str );

    /**
     * Automatically chooses between a number format and
     * a scientific format (if the number is too big)
     */
    void checkNumberFormat();

    void NotifyDependancyList(QPtrList<KSpreadDependency> lst, bool isDepending);

private:
    /**
     * This cell's row.
     * If it is 0, this is the default cell and its row/column can
     * not be determined.
     *
     * @persistent
     */
    int m_iRow;
    /**
     * This cell's column.
     * If it is 0, this is the default cell and its row/column can
     * not be determined.
     *
     * @persistent
     */
    int m_iColumn;

    /**
     * Holds the user's input
     *
     * @persistent
     */
    QString m_strText;

    /**
     * This is the text we want to display
     * Not necessarily the same as m_strText, e.g. m_strText="1" and m_strOutText="1.00".
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


    /* a bit-mask variable for various boolean flags used in the cell
       See the description of flags near the top of kspread_cell.cc
    */
    Q_INT8 m_flagsMask;

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
     * The @ref KSpreadCell that obscures this one.
     * If this pointer is not NULL, then this cell is obscured by another
     * enlarged object. This means that we have to call this object in order
     * of painting it for example instead of painting 'this'.
     */
    KSpreadCell *m_pObscuringCell;

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


    /**
     * list of cells that must be calculated in order to calculate this cell
     */
    QPtrList<KSpreadDependency> m_lstDepends;

    /**
     * list of cells that require this cell's value to be calculated
     */
    QPtrList<KSpreadDependency> m_lstDependingOnMe;


    /**
     * The value we got from calculation.
     * If @ref isFormula() is TRUE, @ref makeLayout() will use @ref m_strFormulaOut
     * instead of @ref m_strText since m_strText stores the formula the user entered.
     */
    QString m_strFormulaOut;

    KSpreadCellPrivate *m_pPrivate;

    /**
     * Tells which kind of content the cell holds.
     *
     * @see #content
     * @see #setCellText
     */
    Content m_content;

    /**
     * Tells which kind of data is present in the cell, assuming m_content == Text
     */
    DataType m_dataType;

    /* no union -- classes can't be in a union */
    double m_dValue;
    QDate m_Date;
    QTime m_Time;

    QSimpleRichText *m_pQML; // Set when the cell contains QML

    /**
     * The parse tree of the real formula (e.g: "=A1*A2").
     */
    KSParseNode* m_pCode;


    KSpreadConditions conditions;

    /**
    * Store the number of line when you used multirow
    * default is 0
    */
    int m_nbLines;

    KSpreadValidity * m_Validity;

    KSpreadCell* m_nextCell;
    KSpreadCell* m_previousCell;

    /**
     * A pointer to the decimal separator
     */
    static QChar decimal_point;
    static const char* s_dataTypeToString[];

  /* helper functions to the paintCell(...) function */
    void paintCellBorders(QPainter& painter, QPoint corner, QPoint cellRef);
    void paintMarker(QPainter& painter, QPoint corner, QPoint cellRef);
    void paintPageBorders(QPainter& painter, QPoint corner, QPoint cellRef);
    void paintText(QPainter& painter, QPoint corner, QPoint cellRef,
                   bool selected);
    void paintMoreTextIndicator(QPainter& painter, QPoint corner,
                                QPoint cellRef);
    void paintCommentIndicator(QPainter& painter, QPoint corner, QPoint cellRef);
    void paintDefaultBorders(QPainter& painter, QPoint corner, QPoint cellRef);
    void paintBackground(QPainter& painter, QPoint corner, QPoint cellRef,
                         bool selected);
    void paintObscuredCells(const QRect& rect, QPainter& painter, QPoint corner,
                            QPoint cellRef);

};

#endif
