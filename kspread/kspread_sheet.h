/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
             (C) 1999-2002 The KSpread Team
                           www.koffice.org/kspread

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

#ifndef KSPREAD_SHEET
#define KSPREAD_SHEET

class KSpreadSheet;
class KSpreadSheetPrint;

class ColumnFormat;
class RowFormat;
class KSpreadCell;
class KSpreadStyle;
class KSpreadView;
class KSpreadPoint;
class KSpreadMap;
class KSpreadCanvas;
class KSpreadDoc;
class KSpreadSelection;
class KSpreadUndoInsertRemoveAction;
class KoDocumentEntry;
class KoStyleStack;
class KoGenStyles;
class KSpreadGenValidationStyles;
class QWidget;
class QPainter;
class QDomElement;

class DCOPObject;
class KPrinter;
class KoOasisSettings;

#include <koDocument.h>
#include <koDocumentChild.h>

#include <qpen.h>
#include <qptrlist.h>
#include <qintdict.h>
#include <qmemarray.h>
#include <qrect.h>
#include <qwidget.h>
#include <qdragobject.h>
#include <koOasisStyles.h>
#include <koxmlwriter.h>

#include "kspread_autofill.h"
#include "kspread_format.h"
#include "kspread_cell.h"
#include "kspread_global.h"

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
    CellBinding( KSpreadSheet *_table, const QRect& _area );
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

    KSpreadSheet* table()const { return m_pTable; }

signals:
    void changed( KSpreadCell *_obj );

protected:
    QRect m_rctDataArea;
    KSpreadSheet *m_pTable;
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
  KSpreadChild( KSpreadDoc *parent, KSpreadSheet *_table, KoDocument* doc, const QRect& geometry );
  KSpreadChild( KSpreadDoc *parent, KSpreadSheet *_table );
  ~KSpreadChild();

  KSpreadDoc* parent()const { return (KSpreadDoc*)parent(); }
  KSpreadSheet* table()const { return m_pTable; }

protected:
  KSpreadSheet *m_pTable;
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

    ChartBinding( KSpreadSheet *_table, const QRect& _area, ChartChild *_child );
    virtual ~ChartBinding();

    virtual void cellChanged( KSpreadCell *_obj );

private:
    ChartChild* m_child;
};

class ChartChild : public KSpreadChild
{
    Q_OBJECT
public:
    ChartChild( KSpreadDoc *_spread, KSpreadSheet *_table, KoDocument* doc, const QRect& _rect );
    ChartChild( KSpreadDoc *_spread, KSpreadSheet *_table );
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
 * KSpreadTextDrag
 *
 ********************************************************************/

/**
 * @short This is a class for handling clipboard data
 */

class KSpreadTextDrag : public QTextDrag
{
    Q_OBJECT

public:
    KSpreadTextDrag( QWidget * dragSource = 0L, const char * name = 0L );
    virtual ~KSpreadTextDrag();

    void setPlain( QString const & _plain ) { setText( _plain ); }
    void setKSpread( QByteArray const & _kspread ) { m_kspread = _kspread; }

    virtual QByteArray encodedData( const char * mime ) const;
    virtual const char* format( int i ) const;

    static bool canDecode( QMimeSource * e );

    static const char * selectionMimeType();

protected:
    QByteArray m_kspread;
};


/********************************************************************
 *
 * Table
 *
 ********************************************************************/

class SheetPrivate;


/**
 */
class KSpreadSheet : public QObject
{
    friend class KSpreadCell;

    Q_OBJECT


    Q_PROPERTY( QString sheetName READ sheetName )
    Q_PROPERTY( bool autoCalc READ getAutoCalc WRITE setAutoCalc );
    Q_PROPERTY( bool showGrid READ getShowGrid WRITE setShowGrid );

public:
    enum Direction { Right, Left, Up, Down };
    enum SortingOrder{ Increase, Decrease };
    enum ChangeRef { ColumnInsert, ColumnRemove, RowInsert, RowRemove };
    enum TestType { Text, Validity, Comment, ConditionalCellAttribute };
    
    enum LayoutDirection { LeftToRight, RightToLeft };

    KSpreadSheet( KSpreadMap *_map, const QString &tableName, const char *_name=0L );
    ~KSpreadSheet();

    virtual bool isEmpty( unsigned long int x, unsigned long int y ) const;

    /**
     * Return the name of this sheet.
     */
    QString sheetName() const;

    /**
     * \deprecated Use sheetName().
     */
    QString tableName() const { return sheetName(); }

    /**
     * Renames a table. This will automatically adapt all formulas
     * in all tables and all cells to reflect the new name.
     *
     * If the name really changed then @ref #sig_nameChanged is emitted
     * and the GUI will reflect the change.
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
    bool setTableName( const QString& name, bool init = FALSE, bool makeUndo=true );

    /**
     * Saves the table and all it's children in XML format
     */
    virtual QDomElement saveXML( QDomDocument& );
    /**
     * Loads the table and all it's children in XML format
     */
    virtual bool loadXML( const QDomElement& );

    virtual bool loadOasis( const QDomElement& table, const KoOasisStyles& oasisStyles );

    virtual bool saveOasis( KoXmlWriter & xmlWriter, KoGenStyles &mainStyles, KSpreadGenValidationStyles &valStyle );
    void saveOasisHeaderFooter( KoXmlWriter &xmlWriter ) const;

    void loadOasisSettings( const KoOasisSettings &settings );
    void saveOasisSettings( KoXmlWriter &settingsWriter, const QPoint& marker );
    void saveOasisPrintStyleLayout( KoGenStyle &style );

    /**
     * Saves a children
     */
    virtual bool saveChildren( KoStore* _store, const QString &_path );
    /**
     * Loads a children
     */
    virtual bool loadChildren( KoStore* _store );

    bool isLoading();
    
    /**
     * Returns the layout direction of the sheet.
     */
    LayoutDirection layoutDirection() const;
    
    /**
     * Sets the layout direction of the sheet. For example, for Arabic or Hebrew
     * documents, it is possibly to layout the sheet from right to left.
     */
    void setLayoutDirection( LayoutDirection dir );
    
    /**
     * \deprecated Use direction().
     */
    bool isRightToLeft() const;

    void password( QCString & passwd ) const ;
    bool isProtected() const;
    void setProtected( QCString const & passwd );
    bool checkPassword( QCString const & passwd ) const;

    void setDefaultHeight( double height );
    void setDefaultWidth( double width );

    const ColumnFormat* columnFormat( int _column ) const;
    ColumnFormat* columnFormat( int _column );
    /**
     * If no special @ref ColumnFormat exists for this column, then a new one is created.
     *
     * @return a non default ColumnFormat for this column.
     */
    ColumnFormat* nonDefaultColumnFormat( int _column, bool force_creation = TRUE );

    const RowFormat* rowFormat( int _row ) const;
    RowFormat* rowFormat( int _row );
    /**
     * If no special @ref RowFormat exists for this row, then a new one is created.
     *
     * @return a non default RowFormat for this row.
     */
    RowFormat* nonDefaultRowFormat( int _row, bool force_creation = TRUE );

    /**
     * @return the first cell of this table. Next cells can
     * be retrieved by calling @ref KSpreadCell::nextCell.
     */
    KSpreadCell* firstCell() const;

    RowFormat* firstRow() const;

    ColumnFormat* firstCol() const;

    KSpreadCell* cellAt( int _column, int _row ) const;
    /**
     * @param _scrollbar_update will change the scrollbar if set to true disregarding
     *                          whether _column/_row are bigger than
     *                          m_iMaxRow/m_iMaxColumn. May be overruled by
     *                          @ref #m_bScrollbarUpdates.
     */
    KSpreadCell* cellAt( int _column, int _row, bool _scrollbar_update = false );
    /**
     * A convenience function.
     */
    KSpreadCell* cellAt( const QPoint& _point, bool _scrollbar_update = false )
      { return cellAt( _point.x(), _point.y(), _scrollbar_update ); }
    /**
     * @returns the pointer to the cell that is visible at a certain position. That means If the cell
     *          at this position is obscured then the obscuring cell is returned.
     *
     * @param _scrollbar_update will change the scrollbar if set to true disregarding
     *                          whether _column/_row are bigger than
     *                          m_iMaxRow/m_iMaxColumn. May be overruled by
     *                          @ref #m_bScrollbarUpdates.
     */
    KSpreadCell* visibleCellAt( int _column, int _row, bool _scrollbar_update = false );
    /**
     * If no special KSpreadCell exists for this position then a new one is created.
     *
     * @param _scrollbar_update will change the scrollbar if set to true disregarding
     *                          whether _column/_row are bigger than
     *                          m_iMaxRow/m_iMaxColumn. May be overruled by
     *                          @ref #m_bScrollbarUpdates.
     *
     * @return a non default KSpreadCell for the position.
     */
    KSpreadCell* nonDefaultCell( int _column, int _row, bool _scrollbar_update = false, KSpreadStyle * _style = 0 );
    KSpreadCell* nonDefaultCell( QPoint const & cellRef, bool scroll = false )
      { return nonDefaultCell( cellRef.x(), cellRef.y(), scroll ); }

    KSpreadCell* defaultCell() const;

    KSpreadFormat* defaultFormat();
    const KSpreadFormat* defaultFormat() const;

    QRect visibleRect( KSpreadCanvas const * const _canvas ) const;
    int topRow( double _ypos, double &_top, const KSpreadCanvas *_canvas = 0L ) const;
    int bottomRow( double _ypos, const KSpreadCanvas *_canvas = 0L ) const;
    int leftColumn( double _xpos, double &_left, const KSpreadCanvas *_canvas = 0L ) const;
    int rightColumn( double _xpos, const KSpreadCanvas *_canvas = 0L ) const;

    /**
     * @return the left corner of the column as int.
     *
     * @param _canvas If not 0 then the returned position is in screen
     *                coordinates. Otherwise the point (0|0) is in the upper
     *                left corner of the table.
     */
    int columnPos( int _col, const KSpreadCanvas *_canvas = 0L ) const;
    /**
     * @return the left corner of the column as double.
     * Use this method, when you later calculate other positions depending on this one
     * to avoid rounding problems
     *
     * @param _canvas If not 0 then the returned position is in screen
     *                coordinates. Otherwise the point (0|0) is in the upper
     *                left corner of the table.
     */
    double dblColumnPos( int _col, const KSpreadCanvas *_canvas = 0L ) const;
    /**
     * @return the top corner of the row as int.
     *
     * @param _canvas If not 0 then the returned position is in screen
     *                coordinates. Otherwise the point (0|0) is in the upper
     *                top corner of the table.
     */
    int rowPos( int _row, const KSpreadCanvas *_canvas = 0L ) const;
    /**
     * @return the top corner of the row as double.
     * Use this method, when you later calculate other positions depending on this one
     * to avoid rounding problems
     *
     * @param _canvas If not 0 then the returned position is in screen
     *                coordinates. Otherwise the point (0|0) is in the upper
     *                top corner of the table.
     */
    double dblRowPos( int _row, const KSpreadCanvas *_canvas = 0L ) const;

    /**
     * @return the maximum size of the column range
     */
    double sizeMaxX() const ;
    /**
     * @return the maximum size of the row range
     */
    double sizeMaxY() const;

    /**
     * Adjusts the internal reference of the sum of the widths of all columns.
     * Used in resizing of columns.
     */
    void adjustSizeMaxX ( double _x );

    /**
     * Adjusts the internal reference of the sum of the heights of all rows.
     * Used in resizing of rows.
     */
    void adjustSizeMaxY ( double _y );

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
     * Calculates all cells in the table with the CalcDirtyFlag.
     */
  //why on earth would we want to do this?
//    void calc();

    /**
     * Recalculates the current table. If you want to recalculate EVERYTHING, then
     * call @ref Table::setCalcDirtyFlag for all tables in the @ref #m_pMap to make
     * sure that no invalid values in other tables make you trouble.
     */
    void recalc();
    
    /** handles the fact that a cell has been changed - updates
    things that need to be updated */
    void valueChanged (KSpreadCell *cell);

    /**
     * Sets the contents of the cell at row,column to text
     * @param updateDepends set to false to disable updating the dependencies
     */
    void setText( int row, int column, const QString& text,
                  bool asString = false );



    void setSelectionFont( KSpreadSelection* selectionInfo,
                           const char *_font = 0L, int _size = -1,
                           signed char _bold = -1, signed char _italic = -1,
                           signed char _underline = -1,
                           signed char _strike = -1 );

    void setSelectionMoneyFormat( KSpreadSelection* selectionInfo, bool b );
    void setSelectionAlign( KSpreadSelection* selectionInfo,
                            KSpreadFormat::Align _align );
    void setSelectionAlignY( KSpreadSelection* selectionInfo,
                             KSpreadFormat::AlignY _alignY );
    void setSelectionPrecision( KSpreadSelection* selectionInfo, int _delta );
    void setSelectionPercent( KSpreadSelection* selectionInfo, bool b );
    void setSelectionMultiRow( KSpreadSelection* selectionInfo, bool enable );
    void setSelectionStyle( KSpreadSelection* selectionInfo, KSpreadStyle * style );

    /**
    * setSelectionSize increase or decrease font size
    */
    void setSelectionSize( KSpreadSelection* selectionInfo, int _size );

    /**
     *change string to upper case if _type equals 1
     * or change string to lower if _type equals -1
     */
    void setSelectionUpperLower( KSpreadSelection* selectionInfo, int _type );

    void setSelectionfirstLetterUpper( KSpreadSelection* selectionInfo);

    void setSelectionVerticalText( KSpreadSelection* selectionInfo, bool _b);

    void setSelectionComment( KSpreadSelection* selectionInfo,
                              const QString &_comment);
    void setSelectionRemoveComment(KSpreadSelection* selectionInfo);

    void setSelectionAngle(KSpreadSelection* selectionInfo, int _value );

    void setSelectionTextColor( KSpreadSelection* selectionInfo,
                                const QColor &tbColor );
    void setSelectionbgColor( KSpreadSelection* selectionInfo,
                              const QColor &bg_Color );
    void setSelectionBorderColor( KSpreadSelection* selectionInfo,
                                  const QColor &bd_Color );

    /**
     * @param _marker is used if there is no selection currently.
     *                In this case the cell on which the marker is will
     *                be deleted.
     *
     */
    void deleteSelection( KSpreadSelection* selectionInfo, bool undo = true );

    /**
     * @param _marker is used if there is no selection currently.
     *                In this case the cell on which the marker is will
     *                be copied.
     */
    void copySelection( KSpreadSelection* selectionInfo );
    /**
     * @param _marker is used if there is no selection currently.
     *                In this case the cell on which the marker is will
     *                be cut.
     */
    void cutSelection( KSpreadSelection* selectionInfo );
    /**
     * @param _marker is used if there is no selection currently.
     *                In this case the cell on which the marker is will
     *                be cleared.
     */
    void clearTextSelection( KSpreadSelection* selectionInfo );

    void clearValiditySelection(KSpreadSelection* selectionInfo );

    void clearConditionalSelection(KSpreadSelection* selectionInfo );

    void fillSelection( KSpreadSelection * selectionInfo, int direction );

    void setWordSpelling(KSpreadSelection* selectionInfo,const QString _listWord );

    QString getWordSpelling(KSpreadSelection* selectionInfo );

    /**
     * A convenience function which retrieves the data to be pasted
     * from the clipboard.
     */
    void paste( const QRect & pasteArea, bool makeUndo = true, PasteMode = Normal,
                Operation = OverWrite, bool insert = false, int insertTo = 0,
                bool pasteFC = false );
    void paste( const QByteArray & data, const QRect & pasteArea,
                bool makeUndo = false, PasteMode= Normal, Operation = OverWrite,
                bool insert = false, int insertTo = 0, bool pasteFC = false );
    void defaultSelection( KSpreadSelection* selectionInfo );

    /**
     * A function which allows to paste a text plain from the clipboard
     */
    void pasteTextPlain( QString &_text, QRect pasteArea);

    void sortByRow( const QRect &area, int ref_row, SortingOrder );
    void sortByRow( const QRect &area, int key1, int key2, int key3,
                    SortingOrder order1, SortingOrder order2, SortingOrder order3,
                    QStringList const * firstKey, bool copyFormat, bool headerRow,
                    KSpreadPoint const & outputPoint, bool respectCase );
    void sortByColumn( const QRect &area, int ref_column, SortingOrder );
    void sortByColumn( const QRect &area, int key1, int key2, int key3,
                       SortingOrder order1, SortingOrder order2, SortingOrder order3,
                       QStringList const * firstKey, bool copyFormat, bool headerRow,
                       KSpreadPoint const & outputPoint, bool respectCase );
    void swapCells( int x1, int y1, int x2, int y2, bool cpFormat );

    /**
     * @param x1, y1: values from source cell,
     * @param x2, y2: values from target cell
     * @param cpFormat: if true: cell format gets copied, too
     */
    void copyCells( int x1, int y1, int x2, int y2, bool cpFormat );
    void setSeries( const QPoint &_marker, double start, double end, double step, Series mode, Series type );

    /**
     * Moves all cells of the row _marker.y() which are in
     * the column _marker.x() or right hand of that one position
     * to the right.
     *
     * @return TRUE if the shift was possible, or false otherwise.
     *         A reason for returning FALSE is that there was a cell
     *         in the right most position.
     */
    bool shiftRow( const QRect &_rect, bool makeUndo=true );
    bool shiftColumn( const QRect& rect, bool makeUndo=true );

    void unshiftColumn( const QRect& rect, bool makeUndo=true );
    void unshiftRow( const QRect& rect, bool makeUndo=true );

    /**
     * Moves all columns which are >= @p col one position to the right and
     * inserts a new and empty column. After this the table is redrawn.
     * nbCol is the number of column which are installing
     */
    bool insertColumn( int col, int nbCol=0, bool makeUndo=true );
    /**
     * Moves all rows which are >= @p row one position down and
     * inserts a new and empty row. After this the table is redrawn.
     */
    bool insertRow( int row, int nbRow=0, bool makeUndo=true );

    /**
     * Deletes the column @p col and redraws the table.
     */
    void removeColumn( int col, int nbCol=0, bool makeUndo=true );
    /**
     * Deletes the row @p row and redraws the table.
     */
    void removeRow( int row, int nbRow=0, bool makeUndo=true );

    /**
    * hide row
    */
    void hideRow( int row, int nbRow=0, QValueList<int>list=QValueList<int>() );
    void emitHideRow();
    void showRow( int row, int NbRow=0, QValueList<int>list=QValueList<int>() );

    /**
    * hide column
    */
    void hideColumn( int col, int NbCol=0, QValueList<int>list=QValueList<int>() );
    void emitHideColumn();
    void showColumn( int col, int NbCol=0, QValueList<int>list=QValueList<int>() );

    int adjustColumn( KSpreadSelection* selectionInfo, int _col = -1 );
    int adjustRow( KSpreadSelection* selectionInfo, int _row = -1 );

    /**
     * Install borders
     */
    void borderLeft( KSpreadSelection* selectionInfo, const QColor &_color );
    void borderTop( KSpreadSelection* selectionInfo, const QColor &_color );
    void borderOutline( KSpreadSelection* selectionInfo, const QColor &_color );
    void borderAll( KSpreadSelection* selectionInfo, const QColor &_color );
    void borderRemove( KSpreadSelection* selectionInfo );
    void borderBottom( KSpreadSelection* selectionInfo, const QColor &_color );
    void borderRight( KSpreadSelection* selectionInfo, const QColor &_color );

    void setConditional( KSpreadSelection* selectionInfo,
			 QValueList<KSpreadConditional> const & newConditions );

    void setValidity( KSpreadSelection* selectionInfo,KSpreadValidity tmp );

    /**
     * Returns, if the grid shall be shown on the screen
     */
    bool getShowGrid() const;

    /**
     * Sets, if the grid shall be shown on the screen
     */
    void setShowGrid( bool _showGrid );

    /**
     * Sets, if formula shall be shown instead of the result
     */
    bool getShowFormula() const;

    void setShowFormula(bool _showFormula);

    /**
     * Sets, if indicator must be shown when the cell holds a formula
     */
    bool getShowFormulaIndicator() const;

    void setShowFormulaIndicator(bool _showFormulaIndicator);

    bool getLcMode() const;

    void setLcMode(bool _lcMode);

    bool getAutoCalc() const;

    void setAutoCalc(bool _AutoCalc);

    bool getShowColumnNumber() const;

    void setShowColumnNumber(bool _showColumnNumber);

    bool getHideZero() const;

    void setHideZero(bool _hideZero);

    bool getFirstLetterUpper() const;

    void setFirstLetterUpper(bool _firstUpper);

    void mergeCells( const QRect &area );
    void dissociateCell( const QPoint &cellRef );
    void changeMergedCell( int m_iCol, int m_iRow, int m_iExtraX, int m_iExtraY);

    void increaseIndent( KSpreadSelection* selectionInfo );
    void decreaseIndent( KSpreadSelection* selectionInfo );

    bool areaIsEmpty(const QRect &area, TestType _type = Text) ;

    void refreshPreference() ;

    void hideTable(bool _hide);

    void removeTable();

    QRect selectionCellMerged(const QRect &_sel);
    /**
     * Change name of reference when the user inserts or removes a column,
     * a row or a cell (= insertion of a row [or column] on a single column [or row]).
     * For example the formula =Table1!A1 is changed into =Table1!B1 if a Column
     * is inserted before A.
     *
     * @param pos the point of insertion (only one coordinate may be used, depending
     * on the other parameters).
     * @param fullRowOrColumn if true, a whole row or column has been inserted/removed.
     *                        if false, we inserted or removed a cell
     * @param ref see ChangeRef
     * @param tabname completes the pos specification by giving the table name
     * @param undo is the handler of the undo class in case of lost cell references
     */
    void changeNameCellRef( const QPoint & pos, bool fullRowOrColumn,
                            ChangeRef ref, QString tabname, int NbCol = 1,
                            KSpreadUndoInsertRemoveAction * undo = 0 );


    void refreshRemoveAreaName(const QString &_areaName);
    void refreshChangeAreaName(const QString &_areaName);


    /**
     * Update chart when you insert or remove row or column
     *
     * @param pos the point of insertion (only one coordinate may be used, depending
     * on the other parameters).
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
    bool isHidden()const;
    /**
     * Hides or shows this tables
     */
    void setHidden( bool hidden );

    /**
     * For internal use only.
     */
    void setMap( KSpreadMap* _map );

    KSpreadDoc* doc()const ;
    KSpreadMap* map()const ;

    /**
     * @return a painter for the hidden widget ( @ref #widget ).
     *
     * This function is useful while making formats where you
     * need some QPainter related functions.
     */
    QPainter& painter();
    /**
     * @return a hidden widget.
     *
     * @see #painter
     */
    QWidget* widget()const;

    /**
     * @return a flag that indicates whether the table should paint the page breaks.
     *
     * @see #setShowPageBorders
     * @see #bShowPageBorders
     */
    bool isShowPageBorders() const;

    /**
     * Turns the page break lines on or off.
     *
     * @see #isShowPageBorders
     * @see #bShowPageBorders
     */
    void setShowPageBorders( bool _b );

    void addCellBinding( CellBinding *_bind );
    void removeCellBinding( CellBinding *_bind );
    CellBinding* firstCellBinding();
    CellBinding* nextCellBinding();

    /**
     * Used by the 'chart' to get the table on which the chart is build.
     * The cells we are interested in are in the rectangle '_range'.
     * The cells are stored row after row in '_list'.
     */
    bool getCellRectangle( const QRect &_range, QPtrList<KSpreadCell> &_list );

    /**
     * A convenience function that finds a table by its name.
     */
    KSpreadSheet *findTable( const QString & _name );

    /**
     * Inserts the @p _cell into the table.
     * All cells depending on this cell will be actualized.
     * The border range will be actualized, when the cell is out of current range.
     */
    void insertCell( KSpreadCell *_cell );
    /**
     * Used by Undo.
     *
     * @see KSpreadUndoDeleteColumn
     */
    void insertColumnFormat( ColumnFormat *_l );
    /**
     * Used by Undo.
     *
     * @see KSpreadUndoDeleteRow
     */
    void insertRowFormat( RowFormat *_l );

    /**
     * @see #copy
     *
     * @param era set this to true if you want to encode relative references absolutely (they will
     *            be switched back to relative references during decoding) - used for cut to clipboard
     */
    QDomDocument saveCellRect( const QRect &, bool copy = false, bool era = false );

    /**
    * insertTo defined if you insert to the bottom or right
    * insert to bottom if insertTo==1
    * insert to right if insertTo ==-1
    * insertTo used just for insert/paste an area
     * @see #paste
     */
    bool loadSelection( const QDomDocument& doc, const QRect &pasteArea,
                        int _xshift, int _yshift, bool makeUndo,
                        PasteMode = Normal, Operation = OverWrite,
                        bool insert = false, int insertTo = 0, bool paste = false );

    void loadSelectionUndo( const QDomDocument & doc, const QRect &loadArea,
                            int _xshift, int _yshift,bool insert,int insertTo);

    /**
    *  Used when you insert and paste cell
    * return true if it's a area
    * false if it's a column/row
    * it's used to select if you want to insert at the bottom or right
    * @see #paste
     */
    bool testAreaPasteInsert()const;

    /**
     * Deletes all cells in the given rectangle.
     * The display is NOT updated by this function.
     * This function can be used to clear an area before you paste something from the clipboard
     * in this area.
     *
     * @param rect The rectangle that contains the cells that should be deleted
     *
     * @see #loadCells
     */
    void deleteCells( const QRect& rect );


    /**
     * Return TRUE if there are text value in cell
     * so you can create list selection
     */
    bool testListChoose(KSpreadSelection* selectionInfo);

    /**
     * returns the text to be copied to the clipboard
     */
    QString copyAsText(KSpreadSelection* selection);

    /**
     * Assume that the retangle 'src' was already selected. Then the user clicked on the
     * lower right corner of the marker and resized the area ( left mouse button ).
     * Once he releases the mouse we have to autofill the region 'dest'. Mention that
     * src.left() == dest.left() and src.top() == dest.top().
     *
     * @see #mouseReleaseEvent
     */
    void autofill( QRect &src, QRect &dest );

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

    const QColorGroup& colorGroup() { return widget()->colorGroup(); }

    int id() const;

    /**
     * Return the currently maximum defined column of the horizontal scrollbar.
     * It's always 10 times higher than the maximum access column.
     * In an empty table it starts with 256.
     */
    int maxColumn() const ;

    /**
     * Checks if the argument _column is out of the current maximum range of the vertical border
     * If this is the case, the current maximum value m_iMaxColumn is adjusted and the vertical border
     * is resized.
     * Use this function with care, as it involves a repaint of the border, when it is out of range.
     */
    void checkRangeHBorder ( int _column );

    /**
     * Return the currently maximum defined row of the vertical scrollbar.
     * It's always 10 times higher than the maximum access row.
     * In an empty table it starts with 256.
     */
    int maxRow() const ;

    /**
     * Checks if the argument _row is out of the current maximum range of the horizontal border
     * If this is the case, the current maximum value m_iMaxRow is adjusted and the horizontal border
     * is resized.
     * Use this function with care, as it involves a repaint of the border, when it is out of range.
     */
    void checkRangeVBorder ( int _row );


    void enableScrollBarUpdates( bool _enable );

    virtual DCOPObject* dcopObject();

    static KSpreadSheet* find( int _id );

#ifndef NDEBUG
    void printDebug();
#endif

    /**
     * Calculates the cell if necessary, makes its layout if necessary,
     * and force redraw.
     * Then it sets the cell's @ref KSpreadCell::m_bDisplayDirtyFlag to false.
     */
    void updateCell( KSpreadCell* _cell, int _col, int _row );

    /**
     * Like updateCell except it works on a range of cells.  Use this function
     * rather than calling updateCell(..) on several adjacent cells so there
     * will be one paint event instead of several
     */
    void updateCellArea(const QRect &cellArea);

    /**
     * Updates every cell on the table
     */
    void update();

    /**
     * repaints all visible cells
     */
    void updateView();

    /**
     * repaints all visible cells in the given rect
     */
    void updateView( QRect const & rect );

    /**
     * used to refresh cells when you make redodelete
     */
    void refreshView(const QRect& rect);

    void emit_updateRow( RowFormat *_format, int _row );
    void emit_updateColumn( ColumnFormat *_format, int _column );

    /**
     * Needed for @ref KSpreadCell::leftBorderPen and friends, since we can not
     * have a static pen object.
     *
     * The returned pen has pen style NoPen set.
     */
    const QPen& emptyPen() const ;
    const QBrush& emptyBrush() const;
    const QColor& emptyColor() const;

    void updateLocale();


  /**
   * set a region of the spreadsheet to be 'paint dirty' meaning it
   * needs repainted.  This is not a flag on the cell itself since quite
   * often this needs set on a default cell
   */
  void setRegionPaintDirty(QRect const & region);

  /**
   * Remove all records of 'paint dirty' cells
   */
  void clearPaintDirtyData();

  /**
   * Test whether a cell needs repainted
   */
  bool cellIsPaintDirty(QPoint const & cell);

  /**
   * Retrieve the first used cell in a given column.  Can be used in conjunction
   * with getNextCellDown to loop through a column.
   *
   * @param col The column to get the first cell from
   *
   * @return Returns a pointer to the cell, or NULL if there are no used cells
   *         in this column
   */
  KSpreadCell* getFirstCellColumn(int col) const;

  /**
   * Retrieve the last used cell in a given column.  Can be used in conjunction
   * with getNextCellUp to loop through a column.
   *
   * @param col The column to get the cell from
   *
   * @return Returns a pointer to the cell, or NULL if there are no used cells
   *         in this column
   */
  KSpreadCell* getLastCellColumn(int col) const;

  /**
   * Retrieve the first used cell in a given row.  Can be used in conjunction
   * with getNextCellRight to loop through a row.
   *
   * @param row The row to get the first cell from
   *
   * @return Returns a pointer to the cell, or NULL if there are no used cells
   *         in this row
   */
  KSpreadCell* getFirstCellRow(int row) const;

  /**
   * Retrieve the last used cell in a given row.  Can be used in conjunction
   * with getNextCellLeft to loop through a row.
   *
   * @param row The row to get the last cell from
   *
   * @return Returns a pointer to the cell, or NULL if there are no used cells
   *         in this row
   */
  KSpreadCell* getLastCellRow(int row) const;

  /**
   * Retrieves the next used cell above the given col/row pair.  The given
   * col/row pair does not need to reference a used cell.
   *
   * @param col column to start looking through
   * @param row the row above which to start looking.
   *
   * @return Returns the next used cell above this one, or NULL if there are none
   */
  KSpreadCell* getNextCellUp(int col, int row) const;

  /**
   * Retrieves the next used cell below the given col/row pair.  The given
   * col/row pair does not need to reference a used cell.
   *
   * @param col column to start looking through
   * @param row the row below which to start looking.
   *
   * @return Returns the next used cell below this one, or NULL if there are none
   */
  KSpreadCell* getNextCellDown(int col, int row) const;

  /**
   * Retrieves the next used cell to the right of the given col/row pair.
   * The given col/row pair does not need to reference a used cell.
   *
   * @param col the column after which should be searched
   * @param row the row to search through
   *
   * @return Returns the next used cell to the right of this one, or NULL if
   * there are none
   */
  KSpreadCell* getNextCellLeft(int col, int row) const;

  /**
   * Retrieves the next used cell to the left of the given col/row pair.
   * The given col/row pair does not need to reference a used cell.
   *
   * @param col the column before which should be searched
   * @param row the row to search through
   *
   * @return Returns the next used cell to the left of this one, or NULL if
   * there are none
   */
  KSpreadCell* getNextCellRight(int col, int row) const;

  KSpreadSheetPrint * print() const;


signals:
    void sig_refreshView();
    void sig_updateView( KSpreadSheet *_table );
    void sig_updateView( KSpreadSheet *_table, const QRect& );
    void sig_updateHBorder( KSpreadSheet *_table );
    void sig_updateVBorder( KSpreadSheet *_table );
    void sig_updateChildGeometry( KSpreadChild *_child );
    void sig_removeChild( KSpreadChild *_child );
    void sig_maxColumn( int _max_column );
    void sig_maxRow( int _max_row );
    /**
     * @see #setTableName
     */
    void sig_nameChanged( KSpreadSheet* table, const QString& old_name );
    /**
     * Emitted if a certain area of some table has to be redrawn.
     * That is for example the case when a new child is inserted.
     */
    void sig_polygonInvalidated( const QPointArray& );

    void sig_TableHidden( KSpreadSheet* table);
    void sig_TableShown( KSpreadSheet* table);
    void sig_TableRemoved( KSpreadSheet* table);
    void sig_TableActivated( KSpreadSheet* );
    void sig_RefreshView( KSpreadSheet* );

protected:
    /**
     * Change the name of a table in all formulas.
     * When you change name table Table1 -> Price
     * for all cell which refere to Table1, this function changes the name.
     */
    void changeCellTabName( QString const & old_name,QString const & new_name );

    bool loadRowFormat( const QDomElement& row, int &rowIndex, const KoOasisStyles& oasisStyles, bool isLast );
    bool loadColumnFormat(const QDomElement& row, const KoOasisStyles& oasisStyles, int & indexCol );
    bool loadTableStyleFormat( QDomElement *style );
    void loadOasisMasterLayoutPage( KoStyleStack &styleStack );

    QString saveOasisTableStyleName( KoGenStyles &mainStyles );
    void saveOasisColRowCell( KoXmlWriter& xmlWriter, KoGenStyles &mainStyles, int maxCols, int maxRows, KSpreadGenValidationStyles &valStyle );
    void saveOasisCells(  KoXmlWriter& xmlWriter, KoGenStyles &mainStyles, int row, int maxCols, KSpreadGenValidationStyles &valStyle );
    void convertPart( const QString & part, KoXmlWriter & writer ) const;
    void addText( const QString & text, KoXmlWriter & writer ) const;

    void maxRowCols( int & maxCols, int & maxRows );

    QString getPart( const QDomNode & part );
    void replaceMacro( QString & text, const QString & old, const QString & newS );

    void insertChild( KSpreadChild *_child );

    /**
     * @see #autofill
     */
    void fillSequence( QPtrList<KSpreadCell>& _srcList, QPtrList<KSpreadCell>& _destList, QPtrList<AutoFillSequence>& _seqList, bool down = true );

    static int s_id;
    static QIntDict<KSpreadSheet>* s_mapTables;

public:
    // see kspread_sheet.cc for an explanation of this
    // this is for type B and also for type A (better use CellWorkerTypeA for that)
    struct CellWorker {
	const bool create_if_default;
	const bool emit_signal;
	const bool type_B;

	CellWorker( bool cid=true, bool es=true, bool tb=true ) : create_if_default( cid ), emit_signal( es ), type_B( tb ) { }
	virtual ~CellWorker() { }

	virtual class KSpreadUndoAction* createUndoAction( KSpreadDoc* doc, KSpreadSheet* table, QRect& r ) =0;

	// these are only needed for type A
	virtual bool testCondition( RowFormat* ) { return false; }
	virtual void doWork( RowFormat* ) { }
	virtual void doWork( ColumnFormat* ) { }
	virtual void prepareCell( KSpreadCell* ) { }

	// these are needed in all CellWorkers
	virtual bool testCondition( KSpreadCell* cell ) =0;
	virtual void doWork( KSpreadCell* cell, bool cellRegion, int x, int y ) =0;
    };

    // this is for type A (surprise :))
    struct CellWorkerTypeA : public CellWorker {
	CellWorkerTypeA( ) : CellWorker( true, true, false ) { }
	virtual QString getUndoTitle( ) =0;
	class KSpreadUndoAction* createUndoAction( KSpreadDoc* doc, KSpreadSheet* table, QRect& r );
    };
    static QString translateOpenCalcPoint( const QString & str );
protected:
    typedef enum { CompleteRows, CompleteColumns, CellRegion } SelectionType;
    SelectionType workOnCells( KSpreadSelection* selectionInfo,
                               CellWorker& worker );

private:
    bool FillSequenceWithInterval (QPtrList<KSpreadCell>& _srcList,
				   QPtrList<KSpreadCell>& _destList,
				   QPtrList<AutoFillSequence>& _seqList,
                                   bool down);

    void FillSequenceWithCopy (QPtrList<KSpreadCell>& _srcList,
			       QPtrList<KSpreadCell>& _destList,
                               bool down);

    void convertObscuringBorders();
    void checkCellContent(KSpreadCell * cell1, KSpreadCell * cell2, int & ret);
    int  adjustColumnHelper( KSpreadCell * c, int _col, int _row );
    void checkContentDirection( QString const & name );

    SheetPrivate* d;

    // don't allow copy or assignment
    KSpreadSheet( const KSpreadSheet& );
    KSpreadSheet& operator=( const KSpreadSheet& );
};

// for compatibility only, remove in the future
typedef KSpreadSheet KSpreadTable;

#endif  // KSPREAD_SHEET
