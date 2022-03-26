/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>
   SPDX-FileCopyrightText: 1999-2007 The KSpread Team <calligra-devel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_SHEET
#define CALLIGRA_SHEETS_SHEET

#include <KoShapeBasedDocumentBase.h>
#include <KoShapeUserData.h>

#include "engine/calligra_sheets_limits.h"
#include "engine/SheetBase.h"

#include "sheets_core_export.h"

#include "engine/ProtectableObject.h"

class KoShape;

namespace Calligra
{
namespace Sheets
{
class Cell;
class CellStorage;
class ColFormat;
class ColFormatStorage;
class CommentStorage;
class ConditionsStorage;
class Database;
class FormulaStorage;
class DocBase;
class FusionStorage;
class LinkStorage;
class HeaderFooter;
class Map;
class PrintSettings;
class Region;
class RowFormat;
class RowFormatStorage;
class Sheet;
class SheetModel;
class SheetPrint;
class Style;
class StyleStorage;
class Validity;
class ValidityStorage;
class ValueStorage;
class View;
class SheetTest;

/**
 * A sheet contains several cells.
 */
class CALLIGRA_SHEETS_CORE_EXPORT Sheet : public KoShapeUserData, public SheetBase, public KoShapeBasedDocumentBase,
        public ProtectableObject
{
    Q_OBJECT
public:
    enum TestType        { Text, Validity, Comment, ConditionalCellAttribute };

    /**
     * Creates a sheet in \p map with the name \p sheetName.
     */
    Sheet(Map* map, const QString& sheetName);

    /**
     * Copy constructor.
     * Creates a sheet with the contents and the settings of \p other.
     */
    Sheet(const Sheet& other);

    /**
     * Destructor.
     */
    ~Sheet() override;

    /**
     * \return the full map this sheet belongs to. This is the inherited version of map().
     */
    Map* fullMap() const;

    /**
     * \return the document this sheet belongs to.
     */
    DocBase* doc() const;

    SheetModel *model() const;

    // KoShapeBasedDocumentBase interface
    void addShape(KoShape* shape) override;
    void removeShape(KoShape* shape) override;

    /**
     * Deletes all shapes without emitting shapeRemoved()
     */
    void deleteShapes();

    /**
     * \ingroup Embedding
     * Returns the sheet's shapes.
     * \return the shapes this sheet contains
     */
    QList<KoShape*> shapes() const;

    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Methods related to sheet properties
    //

    /**
     * Renames a sheet. This will automatically adapt all formulas
     * in all sheets and all cells to reflect the new name.
     *
     * If the name really changed then sig_nameChanged is emitted
     * and the GUI will reflect the change.
     *
     * @param name The new sheet name.
     *
     * @return @c true if the sheet was renamed successfully
     * @return @c false if the sheet could not be renamed. Usually the reason is
     * that this name is already used.
     *
     * @see changeCellTabName
     * @see TabBar::renameTab
     * @see sheetName
     */
    virtual bool setSheetName(const QString& name) override;

    /**
     * Returns the layout direction of the sheet.
     */
    Qt::LayoutDirection layoutDirection() const;

    /**
     * Sets the layout direction of the sheet. For example, for Arabic or Hebrew
     * documents, it is possibly to layout the sheet from right to left.
     */
    void setLayoutDirection(Qt::LayoutDirection dir);

    /**
     * Returns, if the grid shall be shown on the screen
     */
    bool getShowGrid() const;

    /**
     * Sets, if the grid shall be shown on the screen
     */
    void setShowGrid(bool _showGrid);

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
    void setShowCommentIndicator(bool b);

    bool getLcMode() const;

    void setLcMode(bool _lcMode);

    bool getShowColumnNumber() const;

    void setShowColumnNumber(bool _showColumnNumber);

    bool getHideZero() const;

    void setHideZero(bool _hideZero);

    /** Returns whether a row is hidden. */
    virtual bool rowIsHidden(int row) const override;

    /**
     * @return a flag that indicates whether the sheet should paint the page breaks.
     *
     * @see setShowPageOutline
     * @see Sheet::Private::showPageOutline
     */
    bool isShowPageOutline() const;

    /**
     * Turns the page break lines on or off.
     *
     * @see isShowPageOutline
     * @see Sheet::Private::showPageOutline
     */
    void setShowPageOutline(bool _b);

    struct BackgroundImageProperties {
        BackgroundImageProperties()
        : repeat(Repeat)
        , opacity(1.0)
        , horizontalPosition(HorizontalCenter)
        , verticalPosition(VerticalCenter)
        {}

        enum Repetition {
            NoRepeat,
            Repeat,
            Stretch
        };
        Repetition repeat;

        float opacity;

        enum HorizontalPosition {
            Left,
            HorizontalCenter,
            Right
        };
        HorizontalPosition horizontalPosition;

        enum VerticalPosition {
            Top,
            VerticalCenter,
            Bottom
        };
        VerticalPosition verticalPosition;

        //TODO filterName
    };

    /**
     * Set background image for this sheet
     */
    void setBackgroundImage( const QImage& image );

    /**
     * @return The QImage used as the background picture for this sheet
     */
    QImage backgroundImage() const;

    void setBackgroundImageProperties( const BackgroundImageProperties& properties );

    BackgroundImageProperties backgroundImageProperties() const;

    //
    //END Methods related to sheet properties
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Methods related to row formats
    //

    /**
     * \ingroup ColumnRowFormat
     * \return the row format storage for this sheet.
     */
    const RowFormatStorage* rowFormats() const;
    RowFormatStorage* rowFormats();

    /**
     * \ingroup ColumnRowFormat
     * Sets the format for row \p row
     */
    void setRowFormat(int row, const RowFormat &rowFormat);

    /**
     * \ingroup ColumnRowFormat
     * Removed the row format at \p row (changes the format of that row to be the default format).
     */
    void clearRowFormat(int row);


    //
    //END Methods related to row formats
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Methods related to column formats
    //

    /**
     * \ingroup ColumnRowFormat
     * \return the column format storage for this sheet.
     */
    const ColFormatStorage* columnFormats() const;
    ColFormatStorage* columnFormats();

    /**
     * \ingroup ColumnRowFormat
     * Sets the format for column \p col
     */
    void setColumnFormat(int col, const ColFormat &columnFormat);

    /**
     * \ingroup ColumnRowFormat
     * Deletes the column format at \p column.
     */
    void clearColumnFormat(int column);


    //
    //END Methods related to column formats
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Methods for Storage access
    //

    /**
     * \ingroup Storage
     * \return the full cell storage
     */
    CellStorage* fullCellStorage() const;

    const CommentStorage* commentStorage() const;
    const ConditionsStorage* conditionsStorage() const;
    const FusionStorage* fusionStorage() const;
    const LinkStorage* linkStorage() const;
    const StyleStorage* styleStorage() const;

    /**
     * \ingroup Storage
     * Determines the used area, i.e. the area spanning from A1 to the maximum
     * occupied column and row.
     * \return the used area
     */
    QRect usedArea(bool onlyContent = false) const;

    //
    //END Methods for Storage access
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN UNSORTED METHODS !!!
    //

    /**
     * \ingroup Coordinates
     * Determines the row for a given position \p _ypos . If the position is
     * on the border between two cells, the upper row is returned. Also, the offset
     * between the coordinate system root and the upper row border is determined.
     *
     * \param _ypos the position for which the row should be determined
     * \param _top the offset between the coordinate system root and the upper row border
     *
     * \return the row for the given position \p _ypos
     */
    int topRow(double _ypos, double &_top) const;

    /**
     * \ingroup Coordinates
     * Determines the row for a given position \p _ypos . If the position is
     * on the border between two cells, the lower row is returned.
     *
     * \param _ypos the position for which the row should be determined
     *
     * \return the row for the given position \p _ypos
     */
    int bottomRow(double _ypos) const;

    /**
     * \ingroup Coordinates
     * Determines the column for a given position \p _xpos . If the position is
     * on the border between two cells, the left column is returned. Also, the offset
     * between the coordinate system root and the left column border is determined.
     *
     * \param _xpos the position for which the column should be determined
     * \param _left the offset between the coordinate system root and the left column border
     *
     * \return the column for the given position \p _xpos
     */
    int leftColumn(double _xpos, double &_left) const;

    /**
     * \ingroup Coordinates
     * Determines the column for a given position \p _xpos . If the position is
     * on the border between two cells, the right column is returned.
     *
     * \param _xpos the position for which the column should be determined
     *
     * \return the column for the given position \p _xpos
     */
    int rightColumn(double _xpos) const;

    /**
     * \ingroup Coordinates
     * Calculates the region in document coordinates occupied by a range of cells.
     * \param cellRange the range of cells
     * \return the document area covered by the cells
     */
    QRectF cellCoordinatesToDocument(const QRect& cellRange) const;

    /**
     * \ingroup Coordinates
     * Calculates the cell range covering a document area.
     * \param area the document area
     * \return the cell range covering the area
     */
    QRect documentToCellCoordinates(const QRectF& area) const;

    /**
     * \ingroup Coordinates
     * @return the left corner of the column as double.
     * Use this method, when you later calculate other positions depending on this one
     * to avoid rounding problems
     * @param col the column's index
     */
    double columnPosition(int col) const;

    /**
     * \ingroup Coordinates
     * @return the top corner of the row as double.
     * Use this method, when you later calculate other positions depending on this one
     * to avoid rounding problems
     * @param _row the row's index
     */
    double rowPosition(int _row) const;

    /**
     * \ingroup Coordinates
     * \return the document size
     */
    QSizeF documentSize() const;

    /**
     * \ingroup Coordinates
     * Adjusts the internal reference of the sum of the widths of all columns.
     * Used in resizing of columns.
     */
    void adjustDocumentWidth(double deltaWidth);

    /**
     * \ingroup Coordinates
     * Adjusts the internal reference of the sum of the heights of all rows.
     * Used in resizing of rows.
     */
    void adjustDocumentHeight(double deltaHeight);

    /**
     * Adjusts the position of cell anchored shapes as a result of a column size change/insertion/removal.
     * All cell anchored shapes with x coordinates >= minX and < maxX will be moved by delta.
     */
    void adjustCellAnchoredShapesX(double minX, double maxX, double delta);
    void adjustCellAnchoredShapesX(double delta, int firstCol, int lastCol = KS_colMax);

    /**
     * Adjusts the position of cell anchored shapes as a result of a row size change/insertion/removal.
     * All cell anchored shapes with y coordinates >= minY and < maxY will be moved by delta.
     */
    void adjustCellAnchoredShapesY(double minY, double maxY, double delta);
    void adjustCellAnchoredShapesY(double delta, int firstRow, int lastRow = KS_rowMax);

    /**
     * Called when validation fails.
     */
    virtual bool onValidationFailed(Validity::Action action, const CellBase *cell, const QString &message, const QString &title) const override;

    //
    //END UNSORTED METHODS
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Methods related to manipulations of selected cells
    //

    /**
     * \ingroup Commands
     */
    bool areaIsEmpty(const Region& area, TestType _type = Text) ;
 
    //
    //END Methods related to manipulations of selected cells
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Methods related to column/row operations
    //

    /**
     * \ingroup Commands
     * Helper method.
     * \see ShiftManipulator
     */
    void insertShiftRight(const QRect& rect);

    /**
     * \ingroup Commands
     * Helper method.
     * \see ShiftManipulator
     */
    void insertShiftDown(const QRect& rect);

    /**
     * \ingroup Commands
     * Helper method.
     * \see ShiftManipulator
     */
    void removeShiftUp(const QRect& rect);

    /**
     * \ingroup Commands
     * Helper method.
     * \see ShiftManipulator
     */
    void removeShiftLeft(const QRect& rect);

    /**
     * \ingroup ColumnRowFormat
     * Helper method.
     * \see InsertDeleteColumnManipulator
     * Moves all columns which are >= \p col \p number positions to the right
     * and inserts a new and empty column.
     */
    void insertColumns(int row, int numbers);

    /**
     * Helper method.
     * \see InsertDeleteRowManipulator
     * Moves all rows which are >= \p row \p number positions down
     * and inserts a new and empty row.
     */
    void insertRows(int row, int numbers);

    /**
     * \ingroup ColumnRowFormat
     * Helper method.
     * \see InsertDeleteColumnManipulator
     * Deletes \p number columns beginning at \p col .
     */
    void removeColumns(int row, int numbers);

    /**
     * \ingroup ColumnRowFormat
     * Helper method.
     * \see InsertDeleteRowManipulator
     * Deletes \p number rows beginning at \p row .
     */
    void removeRows(int row, int number);

    //
    //END Methods related column/row operations
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN UNSORTED METHODS !!!
    //

    /**
     * Shows a status \p message in the status bar for \p timeout msecs.
     */
    virtual void showStatusMessage(const QString &message, int timeout = 3000) const override;

    void updateLocale();


    /**
     * \ingroup Page
     * Print settings.
     */
    PrintSettings* printSettings() const;

    SheetPrint* print() const;

    /**
     * \ingroup Page
     * Sets the print settings.
     */
    void setPrintSettings(const PrintSettings& settings, bool forcePaint = false);

    /**
     * \ingroup Page
     * \return the header & footer object
     */
    HeaderFooter *headerFooter() const;

    /**
     * Applies a database filter.
     */
    void applyDatabaseFilter(const Database& database);
#ifndef NDEBUG
    void printDebug();
#endif

    //
    //END UNSORTED METHODS
    //
    //////////////////////////////////////////////////////////////////////////

    /// Return the vertical 'scroll' value for save/load and restore
    /// when sheet is set to active sheet
    int canvasOffsetX() const;
    /// Return the horizontal 'scroll' value for save/load and restore
    /// when sheet is set to active sheet
    int canvasOffsetY() const;

public Q_SLOTS:
    /// Store the vertical 'scroll' value for save/load and restore
    /// when sheet is set to active sheet
    void setCanvasOffsetX(int value);
    /// Store the horizontal 'scroll' value for save/load and restore
    /// when sheet is set to active sheet
    void setCanvasOffsetY(int value);

Q_SIGNALS:
    /**
     * Emitted, if the document size changed.
     * E.g. if some columns were inserted.
     * \param size new size
     */
    void documentSizeChanged(const QSizeF &size);

    /**
     * Emitted, if the visible size changed.
     * E.g. if the document size changed or the user selected an area,
     * which was not visible before.
     */
    void visibleSizeChanged();

    /**
     * Emitted, if a status \p message should be shown in the status bar
     * for \p timeout msecs.
     */
    void statusMessage(const QString& message, int timeout) const;

    /**
     * \ingroup Embedding
     * Emitted, if a \p shape was added.
     * \param sheet this sheet (for the View to determine, if it's the active one)
     * \param shape the shape
     */
    void shapeAdded(Sheet *sheet, KoShape *shape);

    /**
     * \ingroup Embedding
     * Emitted, if a \p shape was removed.
     * \param sheet this sheet (for the View to determine, if it's the active one)
     * \param shape the shape
     */
    void shapeRemoved(Sheet *sheet, KoShape *shape);

    /**
     * Emitted if the sheet name changed.
     * \param oldName the previous sheet name
     * \param name the new sheet name
     */
    void nameChanged(const QString &oldName, const QString &name);

    /**
     * Emitted when one or more columns have been added.
     * \param col first column number
     * \param number number of columns
     */
    void columnsAdded(int col, int number);

    /**
     * Emitted when one or more rows have been added.
     * \param col first row number
     * \param number number of rows
     */
    void rowsAdded(int row, int number);

    /**
     * Emitted when one or more columns have been removed.
     * \param col first column number
     * \param number number of columns
     */
    void columnsRemoved(int col, int number);

    /**
     * Emitted when one or more rows have been removed.
     * \param col first row number
     * \param number number of rows
     */
    void rowsRemoved(int row, int number);

protected:

    //
    //////////////////////////////////////////////////////////////////////////
    //

    /**
     * \ingroup Commands
     * \see areaIsEmpty()
     */
    bool cellIsEmpty(const Cell& cell, TestType _type);

private:

    // disable assignment operator
    void operator=(const Sheet& other);

    friend class SheetTest;

    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

Q_DECLARE_METATYPE(Calligra::Sheets::Sheet*)

#endif  // CALLIGRA_SHEETS_SHEET
