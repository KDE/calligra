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
class AutoFillDeltaSequence;
class KSpreadMap;
class ChartCellBinding;
class KSpreadCanvas;
class KSpreadDoc;
class KoDocumentEntry;

class QWidget;
class QPainter;

#include <iostream.h>
#include <komlParser.h>
#include <komlMime.h>
#include <koDocument.h>

#include <kscript_context.h>

#include <qpen.h>
#include <qlist.h>
#include <qintdict.h>
#include <qarray.h>
#include <qrect.h>
#include <qpalette.h>

#define MM_TO_POINT 2.83465
#define POINT_TO_MM 0.3527772388

#define BORDER_SPACE 1

#include "kspread_autofill.h"
#include "kspread_layout.h"
#include "kspread_cell.h"
#include "kspread_dlg_layout.h"
#include "kspread.h"

#include <chart.h>

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
  KSpreadChild( KSpreadDoc *_spread, KSpreadTable *_table, const QRect& _rect, KOffice::Document_ptr _doc );
  KSpreadChild( KSpreadDoc *_spread, KSpreadTable *_table );
  ~KSpreadChild();
  
  KSpreadDoc* parent() { return m_pDoc; }
  KSpreadTable* table() { return m_pTable; }

protected:
  KSpreadDoc *m_pDoc;
  KSpreadTable *m_pTable;
};

/********************************************************************
 *
 * Charts
 *
 ********************************************************************/

class ChartChild;

class ChartBinding : public CellBinding
{
  Q_OBJECT
public:

  ChartBinding( KSpreadTable *_table, const QRect& _area, ChartChild *_child );
  virtual ~ChartBinding();

  void setChart( Chart::SimpleChart_ptr _chart ) { m_vChart = Chart::SimpleChart::_duplicate( _chart ); }

  virtual void cellChanged( KSpreadCell *_obj );

protected:
  ChartChild *m_pChild;
  Chart::SimpleChart_var m_vChart;
};

class ChartChild : public KSpreadChild
{
public:
  ChartChild( KSpreadDoc *_spread, KSpreadTable *_table, const QRect& _rect, KOffice::Document_ptr _doc );
  ChartChild( KSpreadDoc *_spread, KSpreadTable *_table );
  ~ChartChild();

  void setChart( Chart::SimpleChart_ptr );
  void setDataArea( const QRect& _data );
  void update();
  
  virtual bool loadDocument( KOStore::Store_ptr _store, const char *_format );
  virtual bool load( KOMLParser& parser, vector<KOMLAttrib>& _attribs );
  virtual bool save( ostream& out );
  
protected:
  ChartBinding *m_pBinding;
};

/********************************************************************
 *
 * Table
 *
 ********************************************************************/

/**
 */
class KSpreadTable : public QObject,
		     virtual public KSpread::Table_skel
{
    friend KSpreadCell;
  
    Q_OBJECT
public:    
    // C++
    KSpreadTable( KSpreadDoc *_doc, const char *_name );
    ~KSpreadTable();

    // IDL
    virtual KSpread::Book_ptr book();
    virtual KSpread::Range* range( CORBA::ULong left, CORBA::ULong top,
				  CORBA::ULong right, CORBA::ULong bottom );
    virtual KSpread::Range* rangeFromString( const char* str );
    virtual KSpread::Range* rangeFromCells( const KSpread::Cell& topleft,
					   const KSpread::Cell& bottomright );
    virtual KSpread::Cell* cellFromString( const char* str );
    virtual void setValue( const KSpread::Cell& cell, CORBA::Double value );
    virtual void setStringValue( const KSpread::Cell& cell, const char* value );
    virtual CORBA::Double value( const KSpread::Cell& cell );
    virtual char* stringValue( const KSpread::Cell& cell );
    /**
     * Deletes the column '_column' and redraws the table.
     */
    virtual void deleteColumn( CORBA::ULong col );
    /**
     * Moves all columns which are >= _column one position to the right and
     * inserts a new and empty column. After this the table is redrawn.
     */
    virtual void insertColumn( CORBA::ULong col );
    /**
     * Deletes the row '_ow' and redraws the table.
     */
    virtual void deleteRow( CORBA::ULong row );
    /**
     * Moves all rows which are >= _row one position down and
     * inserts a new and empty row. After this the table is redrawn.
     */
    virtual void insertRow( CORBA::ULong row );
    virtual void setSelection( const KSpread::Range& sel );
    virtual KSpread::Range* selection();
    virtual void copySelection();
    virtual void cutSelection();
    virtual void pasteSelection( const KSpread::Cell& cell );
    virtual CORBA::Boolean isEmpty( CORBA::ULong x, CORBA::ULong y );
  
    char* tableName() { return CORBA::string_dup( m_strName.data() ); }

    // C++
    virtual bool save( ostream& );
    virtual bool load( KOMLParser&, vector<KOMLAttrib>& );
    virtual bool loadChildren( KOStore::Store_ptr _store );
 
    virtual void makeChildList( KOffice::Document_ptr _doc, const char *_path );
    /*
     * @return true if one of the direct children wants to
     *              be saved embedded. If there are no children or if
     *              every direct child saves itself into its own file
     *              then false is returned.
     * 
     */
    virtual bool hasToWriteMultipart();

    bool isLoading();
  
    /**
     * This event handler is called if the table becomes the active table,
     * that means that the table becomes visible and may fill the GUIs 
     * widget with new values ( for example the @ref EditWindow ) and
     * the table has to hide/show its parts.
     *
     * @param _status is TRUE if the table became active or FALSE if it became
     *                inactive.
     */
    // void activeKSpreadTableEvent( bool _status );
    
    ColumnLayout* columnLayout( int _column );
    /**
     * If no special @ref ColumnLayout exists for this column, then a new one is created.
     *
     * @return a non default ColumnLayout for this column.
     */
    ColumnLayout* nonDefaultColumnLayout( int _column );
    RowLayout* rowLayout( int _row );
    /**
     * If no special @ref RowLayout exists for this row, then a new one is created.
     *
     * @return a non default RowLayout for this row.
     */
    RowLayout* nonDefaultRowLayout( int _row );
    /**
     * @param _no_scrollbar_update wont change the scrollbar if set to true disregarding
     *                             wether _column/_row are bigger than
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
     * @param _no_scrollbar_update wont change the scrollbar if set to true disregarding
     *                             wether _column/_row are bigger than
     *                             m_iMaxRow/m_iMaxColumn. May be overruled by
     *                             @ref #m_bScrollbarUpdates.
     */
    KSpreadCell* visibleCellAt( int _column, int _row, bool _no_scrollbar_update = false );
    /**
     * If no special KSpreadCell exists for this position then a new one is created.
     *
     * @param _no_scrollbar_update wont change the scrollbar if set to true disregarding
     *                             wether _column/_row are bigger than
     *                             m_iMaxRow/m_iMaxColumn. May be overruled by
     *                             @ref #m_bScrollbarUpdates.
     *
     * @return a non default KSpreadCell for the position.
     */
    KSpreadCell* nonDefaultCell( int _column, int _row, bool _no_scrollbar_update = false );

    KSpreadCell* defaultCell() { return m_pDefaultCell; }
  
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
     * shure that no invalid values in other tables make you trouble.
     */
    void recalc();
  
    /**
     * Sets the contents of the cell at row,column to text
     */
    void setText( int row, int column, const QString& text );

    /**
     * @return the name of this table.
     */
    QString name() { return m_strName; }
    void setName( QString _name ) { m_strName = _name; }
  
    QRect& selectionRect() { return m_rctSelection; }
    void setSelection( const QRect &_rect, KSpreadCanvas *_canvas = 0L );
      
    void setSelectionFont( const QPoint &_marker, const char *_font = 0L, int _size = -1,
			   signed char _bold = -1, signed char _italic = -1 );
    void setSelectionMoneyFormat( const QPoint &_marker );
    void setSelectionAlign( const QPoint &_marker, KSpreadLayout::Align _align );
    void setSelectionPrecision( const QPoint &_marker, int _delta );
    void setSelectionPercent( const QPoint &_marker );
    void setSelectionMultiRow( const QPoint &_marker );

    void setSelectionTextColor( const QPoint &_marker, QColor tbColor );
    void setSelectionbgColor( const QPoint &_marker, QColor bg_Color );
    void deleteSelection( const QPoint &_marker );
    void copySelection( const QPoint &_marker );
    void cutSelection( const QPoint &_marker );
    enum Special_paste { ALL,Formula,Format,Wborder,Link,ALL_trans,Formula_trans,Format_trans,Wborder_trans,Link_trans};
    void paste( const QPoint &_marker,Special_paste=ALL );
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
    
    QPainter& painter() { return *m_pPainter; }
    QWidget* widget() { return m_pWidget; }
  
    /**
     * @return a flag that indicates wether the table should paint the page breaks.
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
     * Tests wether _column is the first column of a new page. In this
     * case the left border of this column may be drawn highlighted to show
     * that this is a page break.
     */
    bool isOnNewPageX( int _column );

    /**
     * Tests wether _row is the first row of a new page. In this
     * case the top border of this row may be drawn highlighted to show
     * that this is a page break.
     */
    bool isOnNewPageY( int _row );

    /**
     * convert the column from int to ascii format
     * (e.g. 1 -> 'A', 27 -> 'AA' , ...)
     */
    const char *columnLabel( int _column );         

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
    KSpreadTable *findTable( const char *_name );

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
     * @see #paste
     */
    bool loadSelection( istream& _in, int _xshift, int _yshift,Special_paste = ALL );
    
    /**
     * Deletes all cells in the given rectangle.
     * The display is NOT updated by this function.
     * This function can be used to clear an area before you paste something from the clipboard
     * in this area.
     *
     * @see #loadCells
     */
    void deleteCells( int _left, int _top, int _right, int _bottom );

    /**
     * Assume that the retangle 'src' was already selected. Then the user clicked on the
     * lower right corner of the marker and resized the area ( left mouse button ).
     * Once he releases the mouse we have to autofill the region 'dest'. Mention that
     * src.left() == dest.left() and src.top() == dest.top().
     *
     * @see #mouseReleaseEvent
     */
    void autofill( QRect &src, QRect &dest );

    void print( QPainter &painter, QPrinter *_printer );

    /**
     * Needed for the printing Extension KOffice::Print
     */
    void draw( QPaintDevice* _dev, CORBA::Long _width, CORBA::Long _height,
	       CORBA::Float _scale );

    void insertChart( const QRect& _geometry, KoDocumentEntry&, const QRect& _data );
    void insertChild( const QRect& _geometry, KoDocumentEntry& );
    void changeChildGeometry( KSpreadChild *_child, const QRect& _geometry );
    QListIterator<KSpreadChild> childIterator();

    void update();
  
    const QColorGroup& colorGroup() { return m_pWidget->colorGroup(); }

    int id() { return m_id; }

    int maxColumn() { return m_iMaxColumn; }
    int maxRow() { return m_iMaxRow; }
    void enableScrollBarUpdates( bool _enable );

    /**
     * @return a context that can be used for evaluating formulars.
     *         This function does remove any exception from the context.
     */
    KSContext& context() { m_context.setException( 0 ); return m_context; }

    static KSpreadTable* find( int _id );
  
    /**
     * Emits the signal @ref #sig_updateCell and sets the cells @ref KSpreadCell::m_bDisplayDirtyFlag to false.
     */
    void emit_updateCell( KSpreadCell* _cell, int _col, int _row );
    void emit_updateRow( RowLayout *_layout, int _row );
    void emit_updateColumn( ColumnLayout *_layout, int _column );

signals:
    void sig_updateView( KSpreadTable *_table );
    void sig_updateView( KSpreadTable *_table, const QRect& );
    void sig_updateCell( KSpreadTable *_table, KSpreadCell* _cell, int _col, int _row );
    void sig_unselect( KSpreadTable *_table, const QRect& );
    void sig_updateHBorder( KSpreadTable *_table );
    void sig_updateVBorder( KSpreadTable *_table );
    void sig_changeSelection( KSpreadTable *_table, const QRect &_old, const QRect &_new );
    void sig_insertChild( KSpreadChild *_child );
    void sig_updateChildGeometry( KSpreadChild *_child );
    void sig_removeChild( KSpreadChild *_child );
    void sig_maxColumn( int _max_column );
    void sig_maxRow( int _max_row );
  
protected:
    void insertChild( KSpreadChild *_child );
  
    /**
     * Prints the page specified by 'page_range'.
     *
     * @paran _page_rangs QRect defines a rectangle of cells which should be painted
     *                    to the device 'prn'.
     */
    void printPage( QPainter &_painter, QRect *page_range, const QPen& _grid_pen );

    /**
     * @see #autofill
     */
    void fillSequence( QList<KSpreadCell>& _srcList, QList<KSpreadCell>& _destList, QList<AutoFillSequence>& _seqList );

    bool saveCellRect( ostream&, const QRect& );

    /**
     * Initialized @ref #m_module and @ref #m_context.
     */
    void initInterpreter();

    /**
     * This module is used to execute formulas of this table.
     */
    KSModule::Ptr m_module;
    /**
     * This context is used to execute formulas of this table.
     */
    KSContext m_context;

    QIntDict<KSpreadCell> m_dctCells;
    QIntDict<RowLayout> m_dctRows;
    QIntDict<ColumnLayout> m_dctColumns;
    
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
     * But testing only selection.left() == 0 will tell you wether a selection
     * is currently active or not.
     * If complete columns are selected, then selection.bottom() == 0x7FFF.
     * If complete rows are selected, then selection.right() == 0x7FFF.
     */
    QRect m_rctSelection;

    /**
     * Indicates wether the table should paint the page breaks.
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
    QList<KSpreadChild> m_lstChildren;

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
  
    static int s_id;
    static QIntDict<KSpreadTable>* s_mapTables;
};

#endif
