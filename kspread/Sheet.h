/* This file is part of the KDE project
   Copyright 1998, 1999 Torben Weis <weis@kde.org>
   Copyright 1999- 2006 The KSpread Team <koffice-devel@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_SHEET
#define KSPREAD_SHEET

#include <QClipboard>
#include <QHash>
#include <QList>
#include <QLinkedList>
#include <QPen>
#include <QPixmap>
#include <QRect>
//#include <QWidget>

#include <KoDocument.h>
#include <KoDocumentChild.h>
#include <KoOasisSettings.h> // for KoOasisSettings::NamedMap
#include <KoXmlReader.h>

//#include "AutoFill.h"
#include "Cell.h"
//#include "Format.h"
#include "Style.h"
#include "Global.h"
//#include "Object.h"

class QWidget;
class QPainter;
class QDomElement;
class KPrinter;
class KoDocumentEntry;
class KoStyleStack;
class KoGenStyles;
class KoOasisLoadingContext;
class KoOasisSettings;
class KoOasisStyles;
class KCommand;
class KoPicture;
class KoXmlWriter;

namespace KoChart
{
class Part;
}

namespace KSpread
{
class AutoFillSequence;
class Canvas;
class Cell;
class ColumnFormat;
class RowFormat;
class EmbeddedChart;
class Doc;
class Format;
class GenValidationStyles;
class Map;
class Point;
class Region;
class Selection;
class Sheet;
class SheetPrint;
class Style;
class UndoInsertRemoveAction;
class Validity;
class View;
class EmbeddedKOfficeObject;
class EmbeddedObject;
class SheetAdaptor;

/********************************************************************
 *
 * CellBinding
 *
 ********************************************************************/

/**
 * \ingroup Embedding
 * @short This is an abstract base class only.
 */
class CellBinding : public QObject
{
    Q_OBJECT
public:
    CellBinding( Sheet *_sheet, const QRect& _area );
    virtual ~CellBinding();

    bool contains( int _x, int _y );
    /**
     * Call this function if one of the cells covered by this binding ( @see rect )
     * has changed. This will in turn force for example a chart to update.
     *
     * @param _obj may by 0. In this case all cells may have changed.
     */
    virtual void cellChanged( Cell *_obj );

    virtual void setIgnoreChanges( bool _ignore ) { m_bIgnoreChanges = _ignore; }

    virtual QRect& dataArea() { return m_rctDataArea; }
    virtual void setDataArea( const QRect _rect ) { m_rctDataArea = _rect; }

    Sheet* sheet()const { return m_pSheet; }

signals:
    void changed( Cell *_obj );

protected:
    QRect m_rctDataArea;
    Sheet *m_pSheet;
    bool m_bIgnoreChanges;
};

/**
 * \ingroup Embedding
 */
class ChartBinding : public CellBinding
{
    Q_OBJECT
public:

    ChartBinding( Sheet *_sheet, const QRect& _area, EmbeddedChart *_child );
    virtual ~ChartBinding();

    virtual void cellChanged( Cell *_obj );

private:
    EmbeddedChart* m_child;
};


/********************************************************************
 *
 * Sheet
 *
 ********************************************************************/

/**
 * A sheet contains several cells.
 */
class KSPREAD_EXPORT Sheet : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString sheetName READ sheetName )
    Q_PROPERTY( bool autoCalc READ getAutoCalc WRITE setAutoCalc )
    Q_PROPERTY( bool showGrid READ getShowGrid WRITE setShowGrid )

public:
    enum Direction       { Right, Left, Up, Down };
    enum SortingOrder    { Increase, Decrease };
    enum ChangeRef       { ColumnInsert, ColumnRemove, RowInsert, RowRemove };
    enum TestType        { Text, Validity, Comment, ConditionalCellAttribute };
    enum LayoutDirection { LeftToRight, RightToLeft };

    /**
     * Creates a sheet in \p map with the name \p sheetName . The internal
     * name, for scripting, is set by \p name .
     */
    Sheet ( Map* map, const QString &sheetName, const char* name = 0 );

    /**
     * Destructor.
     */
    ~Sheet();

    /**
     * \return the map this sheet belongs to
     */
    Map* workbook() const;

    /**
     * \return the document this sheet belongs to
     */
    Doc* doc() const;

    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Methods related to sheet properties
    //

    /**
     * \return the name of this sheet
     */
    QString sheetName() const;

    /**
     * Renames a sheet. This will automatically adapt all formulas
     * in all sheets and all cells to reflect the new name.
     *
     * If the name really changed then sig_nameChanged is emitted
     * and the GUI will reflect the change.
     *
     * @param name The new sheet name.
     * @param init If set to true then no formula will be changed and no signal
     *             will be emitted and no undo action created. Usually you dont
     *             want to do that.
     * @param makeUndo If set to @p true , the renaming is inserted in the
     *                 undo list.
     *
     * @return @c true if the sheet was renamed successfully
     * @return @c false if the sheet could not be renamed. Usually the reason is
     * that this name is already used.
     *
     * @see changeCellTabName
     * @see TabBar::renameTab
     * @see sheetName
     */
    bool setSheetName( const QString& name, bool init = false, bool makeUndo=true );

    /**
     * \return \c true , if a document is currently loading
     */
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

    /**
     * Returns true if comment indicator is visible.
     */
    bool getShowCommentIndicator() const;

    /**
     * If b is true, comment indicator is visible, otherwise
     * it will be hidden.
     */
    void setShowCommentIndicator( bool b );

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

    /**
     * @return true if this sheet is hidden
     */
    bool isHidden()const;

    /**
     * Hides or shows this sheets
     */
    void setHidden( bool hidden );

    /**
     * @return a flag that indicates whether the sheet should paint the page breaks.
     *
     * @see setShowPageBorders
     * @see Sheet::Private::showPageBorders
     */
    bool isShowPageBorders() const;

    /**
     * Turns the page break lines on or off.
     *
     * @see isShowPageBorders
     * @see Sheet::Private::showPageBorders
     */
    void setShowPageBorders( bool _b );

    //
    //END Methods related to sheet properties
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Methods related to KSpread's old file format
    //

    /**
     * \ingroup NativeFormat
     * Saves the sheet and all it's children in XML format
     */
    QDomElement saveXML( QDomDocument& );

    /**
     * \ingroup NativeFormat
     * Loads the sheet and all it's children in XML format
     */
    bool loadXML( const KoXmlElement& );

    /**
     * \ingroup NativeFormat
     * Saves a children
     */
    bool saveChildren( KoStore* _store, const QString &_path );

    /**
     * \ingroup NativeFormat
     * Loads a children
     */
    bool loadChildren( KoStore* _store );

    //
    //END Methods related to KSpread's old file format
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Methods related to the OpenDocument file format
    //

    /**
     * \ingroup OpenDocument
     */
    bool loadOasis( const KoXmlElement& sheet,
                    KoOasisLoadingContext& oasisContext,
                    const Styles& styleMap );

    /**
     * \ingroup OpenDocument
     */
    bool saveOasis( KoXmlWriter& xmlWriter, KoGenStyles& mainStyles,
                    GenValidationStyles& valStyle, KoStore* store,
                    KoXmlWriter* manifestWriter,
                    int& indexObj, int& partIndexObj );

    /**
     * \ingroup OpenDocument
     */
    void saveOasisHeaderFooter( KoXmlWriter &xmlWriter ) const;

    /**
     * \ingroup OpenDocument
     */
    void loadOasisObjects( const KoXmlElement& e, KoOasisLoadingContext& oasisContext );

    /**
     * \ingroup OpenDocument
     */
    void loadOasisSettings( const KoOasisSettings::NamedMap &settings );

    /**
     * \ingroup OpenDocument
     */
    void saveOasisSettings( KoXmlWriter &settingsWriter ) const;

    /**
     * \ingroup OpenDocument
     */
    void saveOasisPrintStyleLayout( KoGenStyle &style ) const;

    /**
     * \ingroup OpenDocument
     */
    bool saveOasisObjects( KoStore *store, KoXmlWriter &xmlWriter, KoGenStyles& mainStyles, int & indexObj, int &partIndexObj );

    //
    //END Methods related to the OpenDocument file format
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Methods related to embedded objects
    //

    /**
     * \ingroup Embedding
     * @brief Get a list of all selected objects
     *
     * @return list of selected objets.
     */
    QList<EmbeddedObject*> getSelectedObjects();

    /**
     * \ingroup Embedding
     * @brief get the rect for the objects
     *
     * @param all true if the rect for all objects should be returned
     *        false if only the rect for selected objects sould be returned
     *
     * @return rect of the objects
     */
    KoRect getRealRect( bool all );

    /**
     * \ingroup Embedding
     * Moves the selected object.
     * \return command for undo
     */
    KCommand *moveObject(View *_view, double diffx, double diffy);

    /**
     * \ingroup Embedding
     * Moves the selected object.
     * \return command for undo
     */
    KCommand *moveObject(View *m_view,const KoPoint &_move,bool key);

    /**
     * \ingroup Embedding
     * @brief Create a unique name for an object.
     *
     * Create a unique name for the object. If no name is set for the object
     * a name according to its type is created. If the name already exists
     * append ' (x)'. // FIXME: not allowed by I18N
     *
     * @param object to work on
     */
    void unifyObjectName( EmbeddedObject *object );

    /**
     * \ingroup Embedding
     */
    bool insertChild( const KoRect& _geometry, KoDocumentEntry&, QWidget* );

    /**
     * \ingroup Embedding
     */
    bool insertChart( const KoRect& _geometry, KoDocumentEntry&, const QRect& _data, QWidget* );

    /**
     * \ingroup Embedding
     * Creates a new embedded picture object and inserts it into the sheet next to the currently
     * selected cell.
     *
     * TODO:  Remove this method in future and provide a better way of opening pictures and inserting
     * them into the sheet.
     *
     * @param file The URL of the file to insert.
     * @param point The the top-left point in the sheet where the picture should be inserted.
     */
    bool insertPicture( const KoPoint& point , const KUrl& file );

    /**
     * \ingroup Embedding
     * Creates a new embedded picture object and inserts it into the sheet at the specified position.
     *
     * @param point The top-left position for the new picture object in the sheet
     * @param pixmap The source pixmap for the new picture
     */
    bool insertPicture( const KoPoint& point, const QPixmap& pixmap );

    /**
     * \ingroup Embedding
     */
    void changeChildGeometry( EmbeddedKOfficeObject *_child, const KoRect& _geometry );

    /**
     * \ingroup Embedding
     * @brief Get the amount of selected objects that belong to this sheet
     *
     * @return the amount of select objects in this sheet
     */
    int numberSelectedObjects() const;

    //
    //END Methods related to embedded objects
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Methods related to protection
    //

    /**
     * \ingroup Protection
     */
    void password( QByteArray & passwd ) const ;

    /**
     * \ingroup Protection
     */
    bool isProtected() const;

    /**
     * \ingroup Protection
     */
    void setProtected( QByteArray const & passwd );

    /**
     * \ingroup Protection
     */
    bool checkPassword( QByteArray const & passwd ) const;

    //
    //END Methods related to protection
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Methods related to row formats
    //

    /**
     * \return the row format of row \p _row . The default row format,
     * if no special one exists.
     */
    const RowFormat* rowFormat( int _row ) const;

    /**
     * \return the row format of row \p _row . The default row format,
     * if no special one exists.
     */
    RowFormat* rowFormat( int _row );

    /**
     * If no special RowFormat exists for this row, then a new one is created.
     *
     * @return a non default RowFormat for this row.
     */
    RowFormat* nonDefaultRowFormat( int _row, bool force_creation = true );

    /**
     * \return the first non-default row format
     */
    RowFormat* firstRow() const;

    void setDefaultHeight( double height );

    //
    //END Methods related to row formats
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Methods related to column formats
    //

    /**
     * \return the column format of column \p _column . The default column format,
     * if no special one exists.
     */
    const ColumnFormat* columnFormat( int _column ) const;

    /**
     * \return the column format of column \p _column . The default column format,
     * if no special one exists.
     */
    ColumnFormat* columnFormat( int _column );

    /**
     * If no special ColumnFormat exists for this column, then a new one is created.
     *
     * @return a non default ColumnFormat for this column.
     */
    ColumnFormat* nonDefaultColumnFormat( int _column, bool force_creation = true );

    /**
     * \return the first non-default row format
     */
    ColumnFormat* firstCol() const;

    void setDefaultWidth( double width );

    //
    //END Methods related to column formats
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Methods related to accessing cells
    //

    /**
     * @return the first non-default cell
     */
    Cell* firstCell() const;

    /**
     * @param column the cell's column index
     * @param row the cell's row index
     */
    Cell* cellAt( int _column, int _row ) const;

    /**
     * @param column the cell's column index
     * @param row the cell's row index
     * @param scrollbar_update will change the scrollbar if set to true disregarding
     *                          whether _column/_row are bigger than
     *                          m_iMaxRow/m_iMaxColumn. May be overruled by
     *                          Sheet::Private::scrollbarUpdates.
     */
    Cell* cellAt( int column, int row, bool scrollbar_update = false );

    /**
     * A convenience function.
     * @param point the cell's coordinates
     * @param scrollbar_update will change the scrollbar if set to true disregarding
     *                          whether _column/_row are bigger than
     *                          m_iMaxRow/m_iMaxColumn. May be overruled by
     *                          Sheet::Private::scrollbarUpdates .
     */
    Cell* cellAt( const QPoint& point, bool scrollbar_update = false )
      { return cellAt( point.x(), point.y(), scrollbar_update ); }

    /**
     * @return the pointer to the cell that is visible at a certain position.
     *         That means If the cell at this position is obscured then the
     *         obscuring cell is returned
     * @param _column the cell's column index
     * @param _row the cell's row index
     * @param _scrollbar_update will change the scrollbar if set to true disregarding
     *                          whether _column/_row are bigger than
     *                          m_iMaxRow/m_iMaxColumn. May be overruled by
     *                          Sheet::Private::scrollbarUpdates .
     */
    Cell* visibleCellAt( int _column, int _row, bool _scrollbar_update = false );

    /**
     * If no special Cell exists for this position then a new one is created.
     *
     * @param _column the cell's column index
     * @param _row the cell's row index
     * @param _style if a new cell is created, this Style is used
     * @return a non default Cell for the position.
     */
    Cell* nonDefaultCell( int _column, int _row, Style* _style = 0 );

    /**
     * Convenience function.
     * \param cellRef the cell's location
     * \see nonDefaultCell(int, int, Style*)
     */
    Cell* nonDefaultCell( QPoint const & cellRef )
      { return nonDefaultCell( cellRef.x(), cellRef.y() ); }

    /**
     * Retrieve the first used cell in a given column.  Can be used in conjunction
     * with getNextCellDown to loop through a column.
     *
     * @param col The column to get the first cell from
     *
     * @return Returns a pointer to the cell, or 0 if there are no used cells
     *         in this column
     */
    Cell* getFirstCellColumn(int col) const;

    /**
     * Retrieve the last used cell in a given column.  Can be used in conjunction
     * with getNextCellUp to loop through a column.
     *
     * @param col The column to get the cell from
     *
     * @return Returns a pointer to the cell, or 0 if there are no used cells
     *         in this column
     */
    Cell* getLastCellColumn(int col) const;

    /**
     * Retrieve the first used cell in a given row.  Can be used in conjunction
     * with getNextCellRight to loop through a row.
     *
     * @param row The row to get the first cell from
     *
     * @return Returns a pointer to the cell, or 0 if there are no used cells
     *         in this row
     */
    Cell* getFirstCellRow(int row) const;

    /**
     * Retrieve the last used cell in a given row.  Can be used in conjunction
     * with getNextCellLeft to loop through a row.
     *
     * @param row The row to get the last cell from
     *
     * @return Returns a pointer to the cell, or 0 if there are no used cells
     *         in this row
     */
    Cell* getLastCellRow(int row) const;

    /**
     * Retrieves the next used cell above the given col/row pair.  The given
     * col/row pair does not need to reference a used cell.
     *
     * @param col column to start looking through
     * @param row the row above which to start looking.
     *
     * @return Returns the next used cell above this one, or 0 if there are none
     */
    Cell* getNextCellUp(int col, int row) const;

    /**
     * Retrieves the next used cell below the given col/row pair.  The given
     * col/row pair does not need to reference a used cell.
     *
     * @param col column to start looking through
     * @param row the row below which to start looking.
     *
     * @return Returns the next used cell below this one, or 0 if there are none
     */
    Cell* getNextCellDown(int col, int row) const;

    /**
     * Retrieves the next used cell to the right of the given col/row pair.
     * The given col/row pair does not need to reference a used cell.
     *
     * @param col the column after which should be searched
     * @param row the row to search through
     *
     * @return Returns the next used cell to the right of this one, or 0 if
     * there are none
     */
    Cell* getNextCellLeft(int col, int row) const;

    /**
     * Retrieves the next used cell to the left of the given col/row pair.
     * The given col/row pair does not need to reference a used cell.
     *
     * @param col the column before which should be searched
     * @param row the row to search through
     *
     * @return Returns the next used cell to the left of this one, or 0 if
     * there are none
     */
    Cell* getNextCellRight(int col, int row) const;

    /**
     * \return the default cell
     */
    Cell* defaultCell() const;

    //
    //END Methods related to accessing cells
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN UNSORTED METHODS !!!
    //

    /**
     * \return the default
     */
    Format* defaultFormat();

    /**
     * \return the default
     */
    const Format* defaultFormat() const;

    /** retrieve a value */
    Value value (int col, int row) const;

    /** retrieve a range of values */
    Value valueRange (int col1, int row1, int col2, int row2) const;

    /**
     * Sets the contents of the cell at row,column to text
     */
    void setText( int row, int column, const QString& text,
                  bool asString = false );

    /**
     * Determines the row for a given position \p _ypos . If the position is
     * on the border between two cells, the upper row is returned. Also, the offset
     * between the coordinate system root and the upper row border is determined.
     *
     * \param _ypos the position for which the row should be determined
     * \param _top the offset between the coordinate system root and the upper row border
     * \param canvas If not 0, the position and offset are treated as document coordinates
     *               (unzoomed). Otherwise screen coordinates are assumed.
     *
     * \return the row for the given position \p _ypos
     */
    int topRow( double _ypos, double &_top, const Canvas *_canvas = 0 ) const;

    /**
     * Determines the row for a given position \p _ypos . If the position is
     * on the border between two cells, the lower row is returned.
     *
     * \param _ypos the position for which the row should be determined
     * \param _canvas If not 0, the position and offset are treated as document coordinates
     *                (unzoomed). Otherwise screen coordinates are assumed.
     *
     * \return the row for the given position \p _ypos
     */
    int bottomRow( double _ypos, const Canvas *_canvas = 0 ) const;

    /**
     * Determines the column for a given position \p _xpos . If the position is
     * on the border between two cells, the left column is returned. Also, the offset
     * between the coordinate system root and the left column border is determined.
     *
     * \param _xpos the position for which the column should be determined
     * \param _left the offset between the coordinate system root and the left column border
     * \param canvas If not 0, the position and offset are treated as document coordinates
     *               (unzoomed). Otherwise screen coordinates are assumed.
     *
     * \return the column for the given position \p _xpos
     */
    int leftColumn( double _xpos, double &_left, const Canvas *_canvas = 0 ) const;

    /**
     * Determines the column for a given position \p _xpos . If the position is
     * on the border between two cells, the right column is returned.
     *
     * \param _xpos the position for which the column should be determined
     * \param canvas If not 0, the position and offset are treated as document coordinates
     *               (unzoomed). Otherwise screen coordinates are assumed.
     *
     * \return the column for the given position \p _xpos
     */
    int rightColumn( double _xpos, const Canvas *_canvas = 0 ) const;

    /**
     * @return the left corner of the column as int.
     * @param col the column's index
     * @param canvas If not 0 then the returned position is in screen
     *               coordinates. Otherwise the point (0|0) is in the upper
     *               left corner of the sheet.
     */
    int columnPos( int col, const Canvas *canvas = 0 ) const;

    /**
     * @return the left corner of the column as double.
     * Use this method, when you later calculate other positions depending on this one
     * to avoid rounding problems
     * @param col the column's index
     * @param canvas If not 0 then the returned position is in screen
     *               coordinates. Otherwise the point (0|0) is in the upper
     *               left corner of the sheet.
     */
    double dblColumnPos( int col, const Canvas *canvas = 0 ) const;

    /**
     * @return the top corner of the row as int.
     * @param _row the row's index
     * @param _canvas If not 0 then the returned position is in screen
     *                coordinates. Otherwise the point (0|0) is in the upper
     *                top corner of the sheet.
     */
    int rowPos( int _row, const Canvas *_canvas = 0 ) const;

    /**
     * @return the top corner of the row as double.
     * Use this method, when you later calculate other positions depending on this one
     * to avoid rounding problems
     * @param _row the row's index
     * @param _canvas If not 0 then the returned position is in screen
     *                coordinates. Otherwise the point (0|0) is in the upper
     *                top corner of the sheet.
     */
    double dblRowPos( int _row, const Canvas *_canvas = 0 ) const;

    /**
     * @return the maximum horizontal size
     */
    double sizeMaxX() const;

    /**
     * @return the maximum vertical size
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
     * Sets the Cell::Flag_LayoutDirty in all cells.
     */
    void setLayoutDirtyFlag();

    /**
     * Recalculates the current sheet.
     *
     * Recalc will do nothing, if automatic calculation is disabled (via Sheet::setAutoCalc)
     * unless the force flag is set to true.  Automatic recalculation is enabled by default.
     *
     * @param force If false, the sheet will be recalculated if automatic calculation is enabled.
     * If true, the sheet will be recalculated regardless of the automatic calculation setting.
     */
    void recalc( bool force = false );

    /**
     * Handles the fact that a cell has been changed.
     * Recalculates dependant cells.
     */
    void valueChanged (Cell *cell);

    /**
     * Handles the fact, that a formula has been changed.
     * Updates the dependencies accordingly.
     */
    void formulaChanged(Cell *cell);

    /**
     * Attempts to guess the title (or 'header') of a column, within a given area of the sheet
     * This is used, for example, by the Data Sort dialog, to guess the names of columns
     * within the selected area.  An empty string may be returned if guessColumnTitle does not think
     * that column @p col has a title.
     * @param area The area within the sheet to guess from
     * @param col The column to find the title (or 'header') for.
     */
    QString guessColumnTitle(QRect& area, int col);

    /**
     * Attempts to guess the title (or 'header') of a row, within a given area of the sheet
     * This is used, for example, by the Data Sort dialog, to guess the names of rows within the selected area.
     * An empty string may be returned if guessRowTitle does not think that row @p row has a title.
     * @param area The area within the sheet to guess from
     * @param row The row to find the title (or 'header') for.
     */
    QString guessRowTitle(QRect& area, int row);

    //
    //END UNSORTED METHODS
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Methods related to manipulations of selected cells
    //

    void setArrayFormula (Selection* selection, const QString &_text);

    /**
     * @param selection the selection of cells to work on
     * @param undo create undo entry, if @c true
     */
    void deleteSelection( Selection* selection, bool undo = true );

    /**
     * @param selection the selection of cells to work on
     */
    void copySelection( Selection* selection );
    /**
     * @param selection the selection of cells to work on
     */
    void cutSelection( Selection* selection );

    void setWordSpelling(Selection* selection,const QString _listWord );

    /**
     * @param selection the selection of cells to work on
     */
    QString getWordSpelling(Selection* selection );

    /**
     * @return @c true if there are text value in cell
     * so you can create list selection
     * @param selection the selection of cells to work on
     */
    bool testListChoose(Selection* selection);

    /**
     * returns the text to be copied to the clipboard
     * @param selection the selection of cells to work on
     */
    QString copyAsText(Selection* selection);

    /**
     * @param region the region to merge
     * @param hor merge horizontally
     * @param ver merge vertically
     */
    void mergeCells( const Region& region, bool hor = false, bool ver = false );
    void dissociateCells( const Region &region );

    bool areaIsEmpty(const Region& area, TestType _type = Text) ;

    /**
     * Deletes all cells in the given rectangle.
     * The display is NOT updated by this function.
     * This function can be used to clear an area before you paste something from the clipboard
     * in this area.
     *
     * @param region The region that contains the cells that should be deleted
     */
    void deleteCells(const Region& region);

    //
    //END Methods related to manipulations of selected cells
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Methods related to cut & paste
    //

    /**
     * A convenience function which retrieves the data to be pasted
     * from the clipboard.
     */
    void paste( const QRect & pasteArea, bool makeUndo = true,
                Paste::Mode = Paste::Normal, Paste::Operation = Paste::OverWrite,
                bool insert = false, int insertTo = 0, bool pasteFC = false,
                QClipboard::Mode clipboardMode = QClipboard::Clipboard );

    void paste( const QByteArray & data, const QRect & pasteArea,
                bool makeUndo = false, Paste::Mode= Paste::Normal, Paste::Operation = Paste::OverWrite,
                bool insert = false, int insertTo = 0, bool pasteFC = false );

    /**
     * A function which allows to paste a text plain from the clipboard
     */
    void pasteTextPlain( QString &_text, QRect pasteArea);

    //
    //END Methods related to cut & paste
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Methods related to column/row operations
    //

    /**
     * Moves all cells of the row _marker.y() which are in
     * the column _marker.x() or right hand of that one position
     * to the right.
     *
     * @return true if the shift was possible, or false otherwise.
     *         A reason for returning false is that there was a cell
     *         in the right most position.
     */
    bool shiftRow( const QRect &_rect, bool makeUndo=true );
    bool shiftColumn( const QRect& rect, bool makeUndo=true );

    void unshiftColumn( const QRect& rect, bool makeUndo=true );
    void unshiftRow( const QRect& rect, bool makeUndo=true );

    /**
     * Moves all columns which are >= @p col one position to the right and
     * inserts a new and empty column. After this the sheet is redrawn.
     * nbCol is the number of column which are installing
     */
    bool insertColumn( int col, int nbCol=0, bool makeUndo=true );
    /**
     * Moves all rows which are >= @p row one position down and
     * inserts a new and empty row. After this the sheet is redrawn.
     */
    bool insertRow( int row, int nbRow=0, bool makeUndo=true );

    /**
     * Deletes the column @p col and redraws the sheet.
     */
    void removeColumn( int col, int nbCol=0, bool makeUndo=true );
    /**
     * Deletes the row @p row and redraws the sheet.
     */
    void removeRow( int row, int nbRow=0, bool makeUndo=true );

    /**
     * Updates vertical border and view.
     */
    void emitHideRow();

    /**
     * Updates horizontal border and view.
     */
    void emitHideColumn();

    //
    //END Methods related column/row operations
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN UNSORTED METHODS !!!
    //

    /**
     * \todo TODO Stefan: remove after Undo.cpp|h and Commands.cpp|h are obsolete
     * \deprecated use MergeManipulator
     */
    void changeMergedCell( int /*m_iCol*/, int /*m_iRow*/, int /*m_iExtraX*/, int /*m_iExtraY*/) {}

    void refreshPreference() ;

    void hideSheet(bool _hide);

    void removeSheet();

    /**
     * Change name of reference when the user inserts or removes a column,
     * a row or a cell (= insertion of a row [or column] on a single column [or row]).
     * For example the formula =Sheet1!A1 is changed into =Sheet1!B1 if a Column
     * is inserted before A.
     *
     * @param pos the point of insertion (only one coordinate may be used, depending
     * on the other parameters).
     * @param fullRowOrColumn if true, a whole row or column has been inserted/removed.
     *                        if false, we inserted or removed a cell
     * @param ref see ChangeRef
     * @param tabname completes the pos specification by giving the sheet name
     * @param NbCol number of columns which were inserted
     * @param undo is the handler of the undo class in case of lost cell references
     */
    void changeNameCellRef( const QPoint & pos, bool fullRowOrColumn,
                            ChangeRef ref, QString tabname, int NbCol = 1,
                            UndoInsertRemoveAction * undo = 0 );


    void refreshRemoveAreaName(const QString &_areaName);
    void refreshChangeAreaName(const QString &_areaName);


    /**
     * \ingroup Embedding
     * Update chart when you insert or remove row or column
     *
     * @param pos the point of insertion (only one coordinate may be used, depending
     * on the other parameters).
     * @param fullRowOrColumn if true, a whole row or column has been inserted/removed.
     *                        if false, we inserted or removed a cell
     * @param ref see ChangeRef
     */
    void refreshChart(const QPoint & pos, bool fullRowOrColumn, ChangeRef ref);

    /**
     * Refresh merged cell when you insert or remove row or column
     */
    void refreshMergedCell();

    /**
     * Adds the CellBinding @p bind to the sheet's list of bindings.
     */
    void addCellBinding( CellBinding* bind );

    /**
     * Removes the CellBinding @p bind to the sheet's list of bindings.
     */
    void removeCellBinding( CellBinding* bind );

    /**
     * @return the sheet's list of bindings
     */
    const QList<CellBinding*>& cellBindings() const;

    /**
     * Used by the 'chart' to get the sheet on which the chart is build.
     * The cells we are interested in are in the rectangle '_range'.
     * The cells are stored row after row in '_list'.
     */
    bool getCellRectangle( const QRect &_range, QList<Cell*> &_list );

    /**
     * A convenience function that finds a sheet by its name.
     */
    Sheet *findSheet( const QString & _name );

    /**
     * Inserts the \p cell into the sheet.
     * All cells depending on this cell will be actualized.
     * The max. scroll range will be actualized, when the cell exceeds the
     * current max. range.
     * \warning If you process many cells, you should disable the scroll
     *          range update by using enableScrollBarUpdates() and update
     *          the range explicitly by using checkRangeHBorder() and
     *          checkRangeVBorder() ONCE after the processing is done.
     */
    void insertCell( Cell *_cell );

    /**
     * Used by Undo.
     * @see UndoDeleteColumn
     */
    void insertColumnFormat( ColumnFormat *_l );

    /**
     * Used by Undo.
     * @see UndoDeleteRow
     */
    void insertRowFormat( RowFormat *_l );

    /**
     * @param era set this to true if you want to encode relative references
     *            absolutely (they will be switched back to relative
     *            references during decoding) - used for cut to clipboard
     */
    QDomDocument saveCellRegion(const Region&, bool copy = false, bool era = false);

    /**
     * insertTo defined if you insert to the bottom or right
     * insert to bottom if insertTo==1
     * insert to right if insertTo ==-1
     * insertTo used just for insert/paste an area
     * @see paste
     */
    bool loadSelection( const KoXmlDocument& doc, const QRect &pasteArea,
                        int _xshift, int _yshift, bool makeUndo,
                        Paste::Mode = Paste::Normal, Paste::Operation = Paste::OverWrite,
                        bool insert = false, int insertTo = 0, bool paste = false );

    void loadSelectionUndo( const KoXmlDocument & doc, const QRect &loadArea,
                            int _xshift, int _yshift,bool insert,int insertTo);

    /**
     * Used when you insert and paste cell
     * return true if it's a area
     * false if it's a column/row
     * it's used to select if you want to insert at the bottom or right
     * @see paste
     */
    bool testAreaPasteInsert()const;

    /**
     * Assume that the retangle 'src' was already selected. Then the user clicked on the
     * lower right corner of the marker and resized the area ( left mouse button ).
     * Once he releases the mouse we have to autofill the region 'dest'. Mention that
     * src.left() == dest.left() and src.top() == dest.top().
     *
     * @see mouseReleaseEvent
     */
    void autofill( QRect &src, QRect &dest );


    //
    //END UNSORTED METHODS
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Methods related to scrollbars
    //

    /**
     * Returns the maximum column ever accessed.
     * In an empty sheet it starts with 256.
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
     * Returns the maximum row ever accessed.
     * In an empty sheet it starts with 256.
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

    //
    //END Methods related to scrollbars
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Methods related to painting
    //

    /**
     * \ingroup Painting
     * set a region of the spreadsheet to be 'paint dirty' meaning it
     * needs repainted.  This is not a flag on the cell itself since quite
     * often this needs set on a default cell
     */
    void setRegionPaintDirty(const Region & region);

    /**
     * \ingroup Painting
     * Remove all records of 'paint dirty' cells
     */
    void clearPaintDirtyData();

    /**
     * \ingroup Painting
     * Test whether a cell needs repainted
     */
    const Region& paintDirtyData() const;

    /**
     * \ingroup Painting
     * Marks the Cell at @p col , @p row as dirty.
     * \deprecated use setRegionPaintDirty
     */
    void updateCell( Cell* cell, int col, int row );

    /**
     * \ingroup Painting
     * Like updateCell except it works on a range of cells.  Use this function
     * rather than calling updateCell(..) on several adjacent cells so there
     * will be one paint event instead of several
     * \deprecated use setRegionPaintDirty
     */
    void updateCellArea(const Region& cellArea);

    /**
     * \ingroup Painting
     * Updates every cell on the sheet
     * \deprecated use setRegionPaintDirty
     */
    void update();

    /**
     * \ingroup Painting
     * repaints all visible cells
     */
    void updateView();

    /**
     * \ingroup Painting
     * repaints all visible cells in \p region
     */
    void updateView(const Region& region);

    //
    //END Methods related to painting
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN UNSORTED METHODS !!!
    //

    /**
     * used to refresh cells when you make redodelete
     * \note Deletes (!) the cells in the region.
     */
    void refreshView(const Region& region);

    void emit_updateRow( RowFormat *_format, int _row, bool repaint = true );
    void emit_updateColumn( ColumnFormat *_format, int _column );

    /**
     * Needed for Cell::leftBorderPen and friends, since we can not
     * have a static pen object.
     *
     * The returned pen has pen style NoPen set.
     */
    const QPen& emptyPen() const ;
    const QBrush& emptyBrush() const;
    const QColor& emptyColor() const;

    void updateLocale();


  SheetPrint * print() const;

#ifndef NDEBUG
    void printDebug();
#endif

    //
    //END UNSORTED METHODS
    //
    //////////////////////////////////////////////////////////////////////////

signals:
    void sig_refreshView();
    void sig_updateView( Sheet *_sheet );
    void sig_updateView( Sheet *_sheet, const Region& );
    void sig_updateView( EmbeddedObject *obj );
    void sig_updateHBorder( Sheet *_sheet );
    void sig_updateVBorder( Sheet *_sheet );
    void sig_updateChildGeometry( EmbeddedKOfficeObject *_child );
    void sig_maxColumn( int _max_column );
    void sig_maxRow( int _max_row );
    /**
     * @see setSheetName
     */
    void sig_nameChanged( Sheet* sheet, const QString& old_name );

    void sig_SheetHidden( Sheet* sheet);
    void sig_SheetShown( Sheet* sheet);
    void sig_SheetRemoved( Sheet* sheet);
    void sig_SheetActivated( Sheet* );
    void sig_RefreshView( Sheet* );

protected slots:
  /** react on modification (add/remove) of a named area */
  void slotAreaModified (const QString &name);

protected:
    /**
     * Change the name of a sheet in all formulas.
     * When you change name sheet Sheet1 -> Price
     * for all cell which refere to Sheet1, this function changes the name.
     */
    void changeCellTabName( QString const & old_name,QString const & new_name );

    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Methods related to the OpenDocument file format
    //

    /**
     * \ingroup OpenDocument
     */
    bool loadRowFormat( const KoXmlElement& row, int &rowIndex,
                        KoOasisLoadingContext& oasisContext,
                        const Styles& styleMap );

    /**
     * \ingroup OpenDocument
     * Loads the properties of a column from a table:table-column element in an OASIS XML file
     * defaultColumnCellStyles is a map from column indicies to the default cell style for that column
     */
    bool loadColumnFormat(const KoXmlElement& row,
                          const KoOasisStyles& oasisStyles, int & indexCol,
                          const Styles& styleMap);

    /**
     * \ingroup OpenDocument
     */
    bool loadSheetStyleFormat( KoXmlElement *style );

    /**
     * \ingroup OpenDocument
     */
    void loadOasisMasterLayoutPage( KoStyleStack &styleStack );

    /**
     * \ingroup OpenDocument
     */
    QString saveOasisSheetStyleName( KoGenStyles &mainStyles );

    /**
     * \ingroup OpenDocument
     */
    void saveOasisColRowCell( KoXmlWriter& xmlWriter, KoGenStyles &mainStyles,
                              int maxCols, int maxRows, GenValidationStyles &valStyle );

    /**
     * \ingroup OpenDocument
     */
    void saveOasisCells( KoXmlWriter& xmlWriter, KoGenStyles &mainStyles,
                         int row, int maxCols, GenValidationStyles &valStyle );

    /**
     * \ingroup OpenDocument
     */
    void convertPart( const QString & part, KoXmlWriter & writer ) const;

    /**
     * \ingroup OpenDocument
     */
    void addText( const QString & text, KoXmlWriter & writer ) const;

    /**
     * \ingroup OpenDocument
     */
    void maxRowCols( int & maxCols, int & maxRows );

    /**
     * \ingroup OpenDocument
     */
    bool compareRows( int row1, int row2, int & maxCols ) const;

    /**
     * \ingroup OpenDocument
     */
    QString getPart( const KoXmlNode & part );

    /**
     * \ingroup OpenDocument
     */
    void replaceMacro( QString & text, const QString & old, const QString & newS );

    //
    //END Methods related to the OpenDocument file format
    //
    //////////////////////////////////////////////////////////////////////////
    //

    /**
     * \ingroup Embedding
     */
    void insertObject( EmbeddedObject *_obj );

    /**
     * @see autofill
     */
    void fillSequence( const QList<Cell*>& _srcList,
                       const QList<Cell*>& _destList,
                       const QList<AutoFillSequence*>& _seqList,
                       bool down = true );


    static Sheet* find( int _id );
    static int s_id;
    static QHash<int,Sheet*>* s_mapSheets;

private:
    /**
     * \ingroup Embedding
     * Inserts a picture into the sheet and the given position.  The picture should be added to the
     * document's picture collection before calling this method.
     */
    bool insertPicture( const KoPoint& point, KoPicture& picture );

    /**
     * \ingroup Embedding
     */
    bool objectNameExists( EmbeddedObject *object, QList<EmbeddedObject*> &list );

    bool fillSequenceWithInterval( const QList<Cell*>& _srcList,
                                   const QList<Cell*>& _destList,
                                   const QList<AutoFillSequence*>& _seqList,
                                   bool down );

    void fillSequenceWithCopy( const QList<Cell*>& _srcList,
                               const QList<Cell*>& _destList,
                               bool down);

    /**
     * \ingroup NativeFormat
     */
    void convertObscuringBorders();

    /**
     * \ingroup NativeFormat
     */
    void checkContentDirection( QString const & name );

    class Private;
    Private* d;

    // don't allow copy or assignment
    Sheet( const Sheet& );
    Sheet& operator=( const Sheet& );
};

} // namespace KSpread

#endif  // KSPREAD_SHEET
