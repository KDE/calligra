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

#ifndef __kspread_table_h__
#define __kspread_table_h__

class ColumnLayout;
class RowLayout;
class KSpreadCell;
class KSpreadTable;
class KSpreadView;
class KSpreadMap;
class KSpreadCanvas;
class KSpreadDoc;
class KoDocumentEntry;

class QWidget;
class QPainter;
class QDomElement;

class DCOPObject;
class KPrinter;

#include <koDocument.h>
#include <koDocumentChild.h>

#include <qpen.h>
#include <qlist.h>
#include <qintdict.h>
#include <qarray.h>
#include <qrect.h>
#include <qwidget.h>

#define BORDER_SPACE 1

#include "kspread_autofill.h"
#include "kspread_layout.h"
#include "kspread_cell.h"
#include "kspread_global.h"
#include "kspread_cluster.h"

/********************************************************************
 *
 * CellBinding
 *
 ********************************************************************/

/**
 * @short This is an abstract base class only.
 */
class CellBinding : public QObject
{
    Q_OBJECT
public:
    CellBinding( KSpreadTable *_table, const QRect& _area );
    virtual ~CellBinding();

    bool contains( int _x, int _y );
    /**
     * Call this function if one of the cells covered by this binding ( @see #rect )
     * has changed. This will in turn force for example a chart to update.
     *
     * @param _obj may by 0L. In this case all cells may have changed.
     */
    virtual void cellChanged( KSpreadCell *_obj );

    virtual void setIgnoreChanges( bool _ignore ) { m_bIgnoreChanges = _ignore; }

    virtual QRect& dataArea() { return m_rctDataArea; }
    virtual void setDataArea( const QRect _rect ) { m_rctDataArea = _rect; }

    KSpreadTable* table() { return m_pTable; }

signals:
    void changed( KSpreadCell *_obj );

protected:
    QRect m_rctDataArea;
    KSpreadTable *m_pTable;
    bool m_bIgnoreChanges;
};

/********************************************************************
 *
 * KSpreadChild
 *
 ********************************************************************/

/**
 * Holds an embedded object.
 */
class KSpreadChild : public KoDocumentChild
{
public:
  KSpreadChild( KSpreadDoc *parent, KSpreadTable *_table, KoDocument* doc, const QRect& geometry );
  KSpreadChild( KSpreadDoc *parent, KSpreadTable *_table );
  ~KSpreadChild();

  KSpreadDoc* parent() { return (KSpreadDoc*)parent(); }
  KSpreadTable* table() { return m_pTable; }

protected:
  KSpreadTable *m_pTable;
};

/********************************************************************
 *
 * Charts
 *
 ********************************************************************/

class ChartChild;
namespace KoChart { class Part; }

class ChartBinding : public CellBinding
{
    Q_OBJECT
public:

    ChartBinding( KSpreadTable *_table, const QRect& _area, ChartChild *_child );
    virtual ~ChartBinding();

    virtual void cellChanged( KSpreadCell *_obj );

private:
    ChartChild* m_child;
};

class ChartChild : public KSpreadChild
{
public:
    ChartChild( KSpreadDoc *_spread, KSpreadTable *_table, KoDocument* doc, const QRect& _rect );
    ChartChild( KSpreadDoc *_spread, KSpreadTable *_table );
    ~ChartChild();

    void setDataArea( const QRect& _data );
    void update();

    /**
     * @reimp
     */
    bool load( const QDomElement& element );
    /**
     * @reimp
     */
    QDomElement save( QDomDocument& doc );

    /**
     * @reimp
     */
    bool loadDocument( KoStore* _store );

    KoChart::Part* chart();

private:
    ChartBinding *m_pBinding;
};

/********************************************************************
 *
 * Table
 *
 ********************************************************************/

/**
 */
class KSpreadTable : public QObject
{
    friend class KSpreadCell;

    Q_OBJECT
public:
    enum SortingOrder{ Increase, Decrease };
    enum ChangeRef { ColumnInsert, ColumnRemove, RowInsert, RowRemove };

    KSpreadTable( KSpreadMap *_map, const QString &tableName, const char *_name=0L );
    ~KSpreadTable();

    virtual bool isEmpty( unsigned long int x, unsigned long int y );

    /**
     * @return the name of this table.
     *
     * @see #setTableName
     */
    QString tableName() { return m_strName; }
    /**
     * Renames a table. This will automatically adapt all formulars
     * in all tables and all cells to reflect the new name.
     *
     * If the name really changed then @ref #sig_nameChanged is emitted
     * and the GUI will reflect the change. In addition a @ref KSpreadUndoSetTableName
     * object will be created to implement undo.
     *
     * @param init If set to TRUE then no formula will be changed and no signal
     *             will be emitted and no undo action created. Usually you dont
     *             want to do that.
     *
     *
     * @return FALSE if the table could not be renamed. Usually the reason is
     *         that this name is already used.
     *
     * @see #changeCellTabName
     * @see KSpreadTabBar::renameTab
     * @see #tableName
     */
    bool setTableName( const QString& name, bool init = FALSE,bool makeUndo=true );

    virtual QDomElement save( QDomDocument& );
    virtual bool loadXML( const QDomElement& );
    virtual bool loadChildren( KoStore* _store );

    virtual bool saveChildren( KoStore* _store, const QString &_path );

    bool isLoading();

    const ColumnLayout* columnLayout( int _column ) const;
    ColumnLayout* columnLayout( int _column );
    /**
     * If no special @ref ColumnLayout exists for this column, then a new one is created.
     *
     * @return a non default ColumnLayout for this column.
     */
    ColumnLayout* nonDefaultColumnLayout( int _column, bool force_creation = TRUE );

    const RowLayout* rowLayout( int _row ) const;
    RowLayout* rowLayout( int _row );
    /**
     * If no special @ref RowLayout exists for this row, then a new one is created.
     *
     * @return a non default RowLayout for this row.
     */
    RowLayout* nonDefaultRowLayout( int _row, bool force_creation = TRUE );

    /**
     * @return the first cell of this table. Next cells can
     * be retrieved by calling @ref KSpreadCell::nextCell.
     */
    KSpreadCell* firstCell();

    RowLayout* firstRow();

    ColumnLayout* firstCol();

    const KSpreadCell* cellAt( int _column, int _row ) const;
    /**
     * @param _no_scrollbar_update won't change the scrollbar if set to true disregarding
     *                             whether _column/_row are bigger than
     *                             m_iMaxRow/m_iMaxColumn. May be overruled by
     *                             @ref #m_bScrollbarUpdates.
     */
    KSpreadCell* cellAt( int _column, int _row, bool _no_scrollbar_update = false );
    /**
     * A convenience function.
     */
    KSpreadCell* cellAt( const QPoint& _point, bool _no_scrollbar_update = false )
      { return cellAt( _point.x(), _point.y(), _no_scrollbar_update ); }
    /**
     * @returns the pointer to the cell that is visible at a certain position. That means If the cell
     *          at this position is obscured then the obscuring cell is returned.
     *
     * @param _no_scrollbar_update won't change the scrollbar if set to true disregarding
     *                             whether _column/_row are bigger than
     *                             m_iMaxRow/m_iMaxColumn. May be overruled by
     *                             @ref #m_bScrollbarUpdates.
     */
    KSpreadCell* visibleCellAt( int _column, int _row, bool _no_scrollbar_update = false );
    /**
     * If no special KSpreadCell exists for this position then a new one is created.
     *
     * @param _no_scrollbar_update won't change the scrollbar if set to true disregarding
     *                             whether _column/_row are bigger than
     *                             m_iMaxRow/m_iMaxColumn. May be overruled by
     *                             @ref #m_bScrollbarUpdates.
     *
     * @return a non default KSpreadCell for the position.
     */
    KSpreadCell* nonDefaultCell( int _column, int _row, bool _no_scrollbar_update = false );

    KSpreadCell* defaultCell() { return m_pDefaultCell; }

    KSpreadLayout* defaultLayout() { return m_defaultLayout; };
    const KSpreadLayout* defaultLayout() const { return m_defaultLayout; }

    int topRow( int _ypos, int &_top, KSpreadCanvas *_canvas = 0L );
    int bottomRow( int _ypos, KSpreadCanvas *_canvas = 0L );
    int leftColumn( int _xpos, int &_left, KSpreadCanvas *_canvas = 0L );
    int rightColumn( int _xpos, KSpreadCanvas *_canvas = 0L );

    /**
     * @retrun the left corner of the column.
     *
     * @param _canvas If not 0 then the returned position is in screen
     *                coordinates. Otherwise the point (0|0) is in the upper
     *               left corner of the table.
     */
    int columnPos( int _col, KSpreadCanvas *_canvas = 0L );
    int rowPos( int _row, KSpreadCanvas *_canvas = 0L );

    /**
     * Sets the @ref KSpreadCell::layoutDirtyFlag in all cells.
     */
    void setLayoutDirtyFlag();
    /**
     * Sets the @ref KSpreadCell::calcDirtyFlag in all cells.
     * That means that the cells are marked dirty and will recalculate
     * if requested. This function does only MARK, it does NOT actually calculate.
     * Use @ref #recalc to recaculate dirty values.
     */
    void setCalcDirtyFlag();
    /**
     * Recalculates the current table. If you want to recalculate EVERYTHING, then
     * call @ref Table::setCalcDirtyFlag for all tables in the @ref #m_pMap to make
     * sure that no invalid values in other tables make you trouble.
     */
    void recalc(bool mdepend=false);

    /**
     * Sets the contents of the cell at row,column to text
     * @param updateDepends set to false to disable updating the dependencies
     */
    void setText( int row, int column, const QString& text, bool updateDepends = true );

    /**
     * @return the rectangle of the choose selection.
     *
     * @see #setChooseRect
     */
    QRect chooseRect() const { return m_chooseRect; }
    /**
     * Set the rectangle of the choose selection. This will trigger
     * the signal @ref #sig_changeChooseSelection.
     *
     * @see #chooseRect
     */
    void setChooseRect( const QRect& rect );

    QRect markerRect() const;
    /**
     * Usually this rect contains only one cell, but if the current
     * cell is a multicol/span cell, then the returned rectangle covers
     * all obscured cells, too.
     *
     * However, it is save to assume that marker.topleft() returns the
     * real cursor position.
     */
    QRect marker() const;
    QRect selectionRect() const { return m_rctSelection; }

    void setSelection( const QRect &_rect, KSpreadCanvas *_canvas = 0L );
    void setSelection( const QRect &_rect, const QPoint& marker, KSpreadCanvas *_canvas = 0L );
    void setMarker( const QPoint& _point, KSpreadCanvas *_canvas = 0L );

    void setSelectionFont( const QPoint &_marker, const char *_font = 0L, int _size = -1,
                           signed char _bold = -1, signed char _italic = -1, signed char _underline = -1,
                           signed char _strike = -1 );
    void setSelectionMoneyFormat( const QPoint &_marker,bool b );
    void setSelectionAlign( const QPoint &_marker, KSpreadLayout::Align _align );
    void setSelectionAlignY( const QPoint &_marker, KSpreadLayout::AlignY _alignY );
    void setSelectionPrecision( const QPoint &_marker, int _delta );
    void setSelectionPercent( const QPoint &_marker, bool b );
    void setSelectionMultiRow( const QPoint &_marker, bool enable );

    /**
    * setSelectionSize increase or decrease font size
    */
    void setSelectionSize( const QPoint &_marker,int _size );

    /**
     *change string to upper case if _type equals 1
     * or change string to lower if _type equals -1
     */
    void setSelectionUpperLower( const QPoint &_marker,int _type );

    void setSelectionfirstLetterUpper( const QPoint &_marker);

    void setSelectionVerticalText( const QPoint &_marker,bool _b);

    void setSelectionComment( const QPoint &_marker,QString _comment);
    void setSelectionRemoveComment( const QPoint &_marker);

    void setSelectionAngle( const QPoint &_marker,int _value);

    void setSelectionTextColor( const QPoint &_marker, const QColor &tbColor );
    void setSelectionbgColor( const QPoint &_marker, const QColor &bg_Color );
    void setSelectionBorderColor( const QPoint &_marker, const QColor &bd_Color );

    /**
     * @param _marker is used if there is no selection currently.
     *                In this case the cell on which the marker is will
     *                be deleted.
     */
    void deleteSelection( const QPoint &_marker );
    /**
     * @param _marker is used if there is no selection currently.
     *                In this case the cell on which the marker is will
     *                be copied.
     */
    void copySelection( const QPoint &_marker );
    /**
     * @param _marker is used if there is no selection currently.
     *                In this case the cell on which the marker is will
     *                be cut.
     */
    void cutSelection( const QPoint &_marker );
    /**
     * @param _marker is used if there is no selection currently.
     *                In this case the cell on which the marker is will
     *                be cleared.
     */
    void clearTextSelection(const QPoint &_marker );

    void clearValiditySelection(const QPoint &_marker );

    void clearConditionalSelection(const QPoint &_marker );


    void setWordSpelling(const QPoint &_marker,const QString _listWord );

    QString getWordSpelling(const QPoint &_marker );

    /**
     * A convenience function which retrieves the data to be pasted
     * from the clipboard.
     */
    void paste( const QPoint &_marker,bool makeUndo=true, PasteMode=Normal, Operation=OverWrite,bool insert=false,int insertTo=0 );
    void paste( const QByteArray& data, const QPoint &_marker,bool makeUndo=false, PasteMode=Normal, Operation=OverWrite,bool insert=false,int insertTo=0 );
    void defaultSelection( const QPoint &_marker );

    /**
     * A function which allows to paste a text plain from the clipboard
     */
    void pasteTextPlain( QMimeSource * _mime, const QPoint &_marker);

    /**
     * Find support.
     */
    void find( const QPoint &_marker, QString _find, long options );

    /**
     * Find'n'Replace support.
     */
    void replace( const QPoint &_marker, QString _find, QString _replace, long options );

    void sortByRow( int ref_row, SortingOrder = Increase );
    void sortByColumn( int ref_column, SortingOrder = Increase );
    void swapCells( int x1, int y1, int x2, int y2 );
    void setSeries( const QPoint &_marker,int start,int end,int step,Series mode,Series type );

    /**
     * Moves all cells of the row _marker.y() which are in
     * the column _marker.x() or right hand of that one position
     * to the right.
     *
     * @return TRUE if the shift was possible, or false otherwise.
     *         A reason for returning FALSE is that there was a cell
     *         in the right most position.
     */
    bool shiftRow(const QRect &_rect,bool makeUndo=true );
    bool shiftColumn( const QRect& rect,bool makeUndo=true );

    void unshiftColumn( const QRect& rect,bool makeUndo=true );
    void unshiftRow( const QRect& rect,bool makeUndo=true );

    /**
     * Moves all columns which are >= @p col one position to the right and
     * inserts a new and empty column. After this the table is redrawn.
     * nbCol is the number of column which are installing
     */
    bool insertColumn( int col,int nbCol=0,bool makeUndo=true );
    /**
     * Moves all rows which are >= @p row one position down and
     * inserts a new and empty row. After this the table is redrawn.
     */
    bool insertRow( int row,int nbRow=0 ,bool makeUndo=true);

    /**
     * Deletes the column @p col and redraws the table.
     */
    void removeColumn( int col,int nbCol=0,bool makeUndo=true );
    /**
     * Deletes the row @p row and redraws the table.
     */
    void removeRow( int row,int nbRow=0,bool makeUndo=true );
    /**
    * hide row
    */
    void hideRow( int row,int nbRow=0,QValueList<int>list=QValueList<int>() );
    void showRow( int row,int NbRow=0,QValueList<int>list=QValueList<int>() );
    /**
    * hide column
    */
    void hideColumn( int col,int NbCol=0,QValueList<int>list=QValueList<int>() );
    void showColumn( int col,int NbCol=0,QValueList<int>list=QValueList<int>() );

    int adjustColumn( const QPoint &_marker, int _col = -1 );
    int adjustRow( const QPoint &_marker, int _row = -1 );

    /**
     * Install borders
     */
    void borderLeft( const QPoint &_marker,const QColor &_color );
    void borderTop( const QPoint &_marker,const QColor &_color );
    void borderOutline( const QPoint &_marker,const QColor &_color );
    void borderAll( const QPoint &_marker,const QColor &_color );
    void borderRemove( const QPoint &_marker );
    void borderBottom( const QPoint &_marker,const QColor &_color );
    void borderRight( const QPoint &_marker,const QColor &_color );

    void setConditional( const QPoint &_marker,KSpreadConditional tmp[3] );

    void setValidity( const QPoint &_marker,KSpreadValidity tmp );

    bool getShowGrid() {return m_bShowGrid;}

    void setShowGrid(bool _showGrid) {m_bShowGrid=_showGrid;}

    bool getShowFormular() {return m_bShowFormular;}

    void setShowFormular(bool _showFormular) {m_bShowFormular=_showFormular;}

    bool getLcMode() {return m_bLcMode;}

    void setLcMode(bool _lcMode) {m_bLcMode=_lcMode;}

    bool getAutoCalc() {return m_bAutoCalc;}

    void setAutoCalc(bool _AutoCalc) {m_bAutoCalc=_AutoCalc;}

    bool getShowColumnNumber() {return m_bShowColumnNumber;}

    void setShowColumnNumber(bool _showColumnNumber) {m_bShowColumnNumber=_showColumnNumber;}

    bool getHideZero() {return m_bHideZero;}

    void setHideZero(bool _hideZero) {m_bHideZero=_hideZero;}

    bool getFirstLetterUpper() {return m_bFirstLetterUpper;}

    void setFirstLetterUpper(bool _firstUpper) {m_bFirstLetterUpper=_firstUpper;}

    void mergeCell( const QPoint &_marker, bool makeUndo=true );
    void dissociateCell( const QPoint &_marker, bool makeUndo=true );
    void changeMergedCell( int m_iCol, int m_iRow, int m_iExtraX, int m_iExtraY);

    void increaseIndent( const QPoint &_marker );
    void decreaseIndent( const QPoint &_marker );

    bool areaIsEmpty() ;

    void refreshPreference() ;

    void hideTable(bool _hide);

    void removeTable();

    void setActiveTable();
    QPoint getOldPos() {return m_oldPos;}

    int getScrollPosX() {return m_iScrollPosX;}
    void setScrollPosX( int _scrollX) { m_iScrollPosX=_scrollX;}

    int getScrollPosY() {return m_iScrollPosY;}
    void setScrollPosY( int _scrollY) { m_iScrollPosY=_scrollY;}



    QRect selectionCellMerged(const QRect &_sel);
    /**
     * Change name of reference when the user inserts or removes a column,
     * a row or a cell (= insertion of a row [or column] on a single column [or row]).
     * For example the formula =Table1!A1 is changed into =Table1!B1 if a Column
     * is inserted before A.
     *
     * @param pos the point of insertion (only one coordinate may be used, depending
     * on the other paramaters).
     * @param fullRowOrColumn if true, a whole row or column has been inserted/removed.
     *                        if false, we inserted or removed a cell
     * @param ref see ChangeRef
     * @param tabname completes the pos specification by giving the table name
     */
    void changeNameCellRef(const QPoint & pos, bool fullRowOrColumn, ChangeRef ref,QString tabname,int NbCol=1);


    void refreshRemoveAreaName(const QString &_areaName);


    /**
     * Update chart when you insert or remove row or column
     *
     * @param pos the point of insertion (only one coordinate may be used, depending
     * on the other paramaters).
     * @param fullRowOrColumn if true, a whole row or column has been inserted/removed.
     *                        if false, we inserted or removed a cell
     * @param ref see ChangeRef
     * @param tabname completes the pos specification by giving the table name
     */
    void refreshChart(const QPoint & pos, bool fullRowOrColumn, ChangeRef ref);
    /**
     * Refresh merged cell when you insert or remove row or column
     */
    void refreshMergedCell();

    /**
     * @return true if this table is hidden
     */
    bool isHidden() { return m_bTableHide; }
    /**
     * Hides or shows this tables
     */
    void setHidden(bool hidden) { m_bTableHide=hidden; }

    /**
     * Unselects all selected columns/rows/cells and redraws these cells.
     */
    void unselect();

    /**
     * For internal use only.
     */
    void setMap( KSpreadMap* _map ) { m_pMap = _map; }

    KSpreadDoc* doc() { return m_pDoc; }
    KSpreadMap* map() { return m_pMap; }

    /**
     * @return a painter for the hidden widget ( @ref #widget ).
     *
     * This function is useful while making layouts where you
     * need some QPainter related functions.
     */
    QPainter& painter() { return *m_pPainter; }
    /**
     * @return a hidden widget.
     *
     * @see #painter
     */
    QWidget* widget() { return m_pWidget; }

    /**
     * @return a flag that indicates whether the table should paint the page breaks.
     *
     * @see #setShowPageBorders
     * @see #bShowPageBorders
     */
    bool isShowPageBorders() { return m_bShowPageBorders; }

    /**
     * Turns the page break lines on or off.
     *
     * @see #isShowPageBorders
     * @see #bShowPageBorders
     */
    void setShowPageBorders( bool _b );

    /**
     * Tests whether _column is the first column of a new page. In this
     * case the left border of this column may be drawn highlighted to show
     * that this is a page break.
     */
    bool isOnNewPageX( int _column );

    /**
     * Tests whether _row is the first row of a new page. In this
     * case the top border of this row may be drawn highlighted to show
     * that this is a page break.
     */
    bool isOnNewPageY( int _row );

    void addCellBinding( CellBinding *_bind );
    void removeCellBinding( CellBinding *_bind );
    CellBinding* firstCellBinding() { return m_lstCellBindings.first(); }
    CellBinding* nextCellBinding() { return m_lstCellBindings.next(); }

    /**
     * Used by the 'chart' to get the table on which the chart is build.
     * The cells we are interested in are in the rectangle '_range'.
     * The cells are stored row after row in '_list'.
     */
    bool getCellRectangle( const QRect &_range, QList<KSpreadCell> &_list );

    /**
     * A convenience function that finds a table by its name.
     */
    KSpreadTable *findTable( const QString & _name );

    /**
     * Used by Undo.
     *
     * @see KSpreadUndoDeleteColumn
     */
    void insertCell( KSpreadCell *_cell );
    /**
     * Used by Undo.
     *
     * @see KSpreadUndoDeleteColumn
     */
    void insertColumnLayout( ColumnLayout *_l );
    /**
     * Used by Undo.
     *
     * @see KSpreadUndoDeleteRow
     */
    void insertRowLayout( RowLayout *_l );

    /**
     * @see #copy
     */
    QDomDocument saveCellRect( const QRect& );

    /**
    * insertTo defined if you insert to the bottom or right
    * insert to bottom if insertTo==1
    * insert to right if insertTo ==-1
    * insertTo used just for insert/paste an area
     * @see #paste
     */
    bool loadSelection( const QDomDocument& doc, int _xshift, int _yshift,bool makeUndo,PasteMode = Normal, Operation = OverWrite,bool insert=false,int insertTo=0 );

    void loadSelectionUndo( const QDomDocument & doc,int _xshift, int _yshift,bool insert,int insertTo);

    /**
    *  Used when you insert and paste cell
    * return true if it's a area
    * false if it's a column/row
    * it's used to select if you want to insert at the bottom or right
    * @see #paste
     */
    bool testAreaPasteInsert();

    /**
     * Deletes all cells in the given rectangle.
     * The display is NOT updated by this function.
     * This function can be used to clear an area before you paste something from the clipboard
     * in this area.
     *
     * @see #loadCells
     */
    void deleteCells( const QRect& rect );


    /**
     * Return TRUE if there are text value in cell
     * so you can create list selection
     */
    bool testListChoose(const QPoint &_marker);


    /**
     * Assume that the retangle 'src' was already selected. Then the user clicked on the
     * lower right corner of the marker and resized the area ( left mouse button ).
     * Once he releases the mouse we have to autofill the region 'dest'. Mention that
     * src.left() == dest.left() and src.top() == dest.top().
     *
     * @see #mouseReleaseEvent
     */
    void autofill( QRect &src, QRect &dest );

    void print( QPainter &painter, KPrinter *_printer );

    /**
     * Deletes a child object. That will cause all views to update
     * accordingly. Do not use this child object afterwards.
     *
     * @ref #insertChild
     */
    void deleteChild( KSpreadChild *_child );
    /**
     * @ref #deleteChild
     */
    void insertChild( const QRect& _geometry, KoDocumentEntry& );
    /**
     * A convenience function around @ref #insertChild.
     */
    void insertChart( const QRect& _geometry, KoDocumentEntry&, const QRect& _data );
    void changeChildGeometry( KSpreadChild *_child, const QRect& _geometry );

    void update();

    const QColorGroup& colorGroup() { return m_pWidget->colorGroup(); }

    int id() { return m_id; }

    int maxColumn() { return m_iMaxColumn; }
    int maxRow() { return m_iMaxRow; }
    void enableScrollBarUpdates( bool _enable );

    virtual DCOPObject* dcopObject();

    static KSpreadTable* find( int _id );

    /**
     * Calculates the cell if necessary, makes its layout if necessary,
     * and force redraw.
     * Then it sets the cell's @ref KSpreadCell::m_bDisplayDirtyFlag to false.
     */
    void updateCell( KSpreadCell* _cell, int _col, int _row );

    /**
     * used to refresh cells when you make redodelete
     */
    void refreshView(const QRect& rect);
    /**
     * Repaint cells in @p rect.
     * Called by KSpreadUndoCellLayout::undo() and KSpreadUndoCellLayout::redo()
     */
    void updateView(const QRect& rect);

    void emit_updateRow( RowLayout *_layout, int _row );
    void emit_updateColumn( ColumnLayout *_layout, int _column );
    void emit_polygonInvalidated( const QPointArray& );

    /**
     * Needed for @ref KSpreadCell::leftBorderPen and friends, since we can not
     * have a static pen object.
     *
     * The returned pen has pen style NoPen set.
     */
    const QPen& emptyPen() const { return m_emptyPen; }
    const QBrush& emptyBrush() const { return m_emptyBrush; }
    const QColor& emptyColor() const { return m_emptyColor; }

private slots:

    void highlight( const QString &text, int matchingIndex, int matchedLength, const QRect &cellRect );
    void replace( const QString &newText, int index, int replacedLength, const QRect &cellRect );

signals:
    void sig_updateView( KSpreadTable *_table );
    void sig_updateView( KSpreadTable *_table, const QRect& );
    void sig_unselect( KSpreadTable *_table, const QRect& );
    void sig_updateHBorder( KSpreadTable *_table );
    void sig_updateVBorder( KSpreadTable *_table );
    void sig_changeSelection( KSpreadTable *_table, const QRect &_old, const QRect &_old_marker );
    void sig_changeChooseSelection( KSpreadTable *_table, const QRect &_old, const QRect &_new );
    void sig_updateChildGeometry( KSpreadChild *_child );
    void sig_removeChild( KSpreadChild *_child );
    void sig_maxColumn( int _max_column );
    void sig_maxRow( int _max_row );
    /**
     * @see #setTableName
     */
    void sig_nameChanged( KSpreadTable* table, const QString& old_name );
    /**
     * Emitted if a certain area of some table has to be redrawn.
     * That is for example the case when a new child is inserted.
     */
    void sig_polygonInvalidated( const QPointArray& );

    void sig_TableHidden( KSpreadTable* table);
    void sig_TableShown( KSpreadTable* table);
    void sig_TableRemoved( KSpreadTable* table);
    void sig_TableActivated( KSpreadTable* );
    void sig_RefreshView( KSpreadTable* );

protected:
    /**
     * Change the name of a table in all formulas.
     * When you change name table Table1 -> Price
     * for all cell which refere to Table1, this function changes the name.
     */
    void changeCellTabName(QString old_name,QString new_name);

    void insertChild( KSpreadChild *_child );

    /**
     * Prints the page specified by 'page_range'.
     *
     * @return the last vertical line which was printed plus one.
     *
     * @paran _page_rangs QRect defines a rectangle of cells which should be painted
     *                    to the device 'prn'.
     */
    void printPage( QPainter &_painter, const QRect& page_range, const QRect& view );

    /**
     * @see #autofill
     */
    void fillSequence( QList<KSpreadCell>& _srcList, QList<KSpreadCell>& _destList, QList<AutoFillSequence>& _seqList );

    KSpreadCluster m_cells;
    KSpreadRowCluster m_rows;
    KSpreadColumnCluster m_columns;

    KSpreadCell* m_pDefaultCell;
    RowLayout* m_pDefaultRowLayout;
    ColumnLayout* m_pDefaultColumnLayout;

    /**
     * The name of the table. This name shows in the tab bar on the bottom of the window.
     */
    QString m_strName;

    /**
     * The rectangular area that is currently selected.
     * If all 4 coordinates are 0 then no selection is made at all.
     * But testing only selection.left() == 0 will tell you whether a selection
     * is currently active or not.
     * If complete columns are selected, then selection.bottom() == 0x7FFF.
     * If complete rows are selected, then selection.right() == 0x7FFF.
     */
    QRect m_rctSelection;
    QRect m_marker;

    /**
     * Contains the selection of a choose. If @ref QRect::left() returns 0, then
     * there is no selection.
     *
     * @ref #chooseRect
     * @ref #setChooseRect
     */
    QRect m_chooseRect;

    /**
     * Indicates whether the table should paint the page breaks.
     * Doing so costs some time, so by default it should be turned off.
     */
    bool m_bShowPageBorders;

    /**
     * List of all cell bindings. For example charts use bindings to get
     * informed about changing cell contents.
     *
     * @see #addCellBinding
     * @see #removeCellBinding
     */
    QList<CellBinding> m_lstCellBindings;

    /**
     * The label returned by @ref #columnLabel
     */
    char m_arrColumnLabel[20];

    /**
     * The map to which this table belongs.
     */
    KSpreadMap *m_pMap;
    KSpreadDoc *m_pDoc;

    /**
     * Needed to get infos about font metrics.
     */
    QPainter *m_pPainter;
    /**
     * Used for @ref #m_pPainter
     */
    QWidget *m_pWidget;

    /**
     * List of all embedded objects.
     */
    // QList<KSpreadChild> m_lstChildren;

    int m_id;

    /**
     * The highest row ever accessed by the user.
     */
    int m_iMaxRow;
    /**
     * The highest column ever accessed by the user.
     */
    int m_iMaxColumn;
    bool m_bScrollbarUpdates;

    DCOPObject* m_dcop;
    bool m_bTableHide;

    static int s_id;
    static QIntDict<KSpreadTable>* s_mapTables;

    bool m_bShowGrid;
    bool m_bShowFormular;
    bool m_bAutoCalc;
    bool m_bLcMode;
    bool m_bShowColumnNumber;
    bool m_bHideZero;
    bool m_bFirstLetterUpper;

    KSpreadLayout* m_defaultLayout;

    /**
     * @see #emptyPen
     */
    QPen m_emptyPen;
    QBrush m_emptyBrush;
    QColor m_emptyColor;

    QPoint m_oldPos;
    int m_iScrollPosX;
    int m_iScrollPosY;
public:
    // see kspread_table.cc for an explanation of this
    // this is for type B and also for type A (better use CellWorkerTypeA for that)
    struct CellWorker {
	const bool create_if_default;
	const bool emit_signal;
	const bool type_B;

	CellWorker( bool cid=true, bool es=true, bool tb=true ) : create_if_default( cid ), emit_signal( es ), type_B( tb ) { }
	virtual ~CellWorker() { }

	virtual class KSpreadUndoAction* createUndoAction( KSpreadDoc* doc, KSpreadTable* table, QRect& r ) =0;

	// these are only needed for type A
	virtual bool testCondition( RowLayout* ) { return false; }
	virtual void doWork( RowLayout* ) { }
	virtual void doWork( ColumnLayout* ) { }
	virtual void prepareCell( KSpreadCell* ) { }

	// these are needed in all CellWorkers
	virtual bool testCondition( KSpreadCell* cell ) =0;
	virtual void doWork( KSpreadCell* cell, bool cellRegion, int x, int y ) =0;
    };

    // this is for type A (surprise :))
    struct CellWorkerTypeA : public CellWorker {
	CellWorkerTypeA( ) : CellWorker( true, true, false ) { }
	virtual QString getUndoTitle( ) =0;
	class KSpreadUndoAction* createUndoAction( KSpreadDoc* doc, KSpreadTable* table, QRect& r );
    };

protected:
    typedef enum { CompleteRows, CompleteColumns, CellRegion } SelectionType;

    SelectionType workOnCells( const QPoint& _marker, CellWorker& worker );
};

#endif
