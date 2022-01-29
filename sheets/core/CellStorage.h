/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_CELL_STORAGE
#define CALLIGRA_SHEETS_CELL_STORAGE

#include <QPair>
#include <QRect>
#include <QTextDocument>

#include "engine/CellBaseStorage.h"
#include "engine/calligra_sheets_limits.h"

#include "sheets_core_export.h"
#include "Cell.h"

class KUndo2Command;

namespace Calligra
{
namespace Sheets
{
class Binding;
class BindingStorage;
class Cell;
class Conditions;
class ConditionsStorage;
class Database;
class FusionStorage;
class LinkStorage;
class RichTextStorage;
class Sheet;
class StyleStorage;

/**
 * \ingroup Storage
 * The cell storage.
 * Extends the base storage with more types.
 * A wrapper around a couple of storages, which hold the cell data.
 * Provides methods to iterate over the non-empty cells.
 * Emits Damages on changes.
 * Capable of recording the old data for undoing.
 *
 * \author Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 *
 * \note If you fill the storage, do it row-wise. That's more performant.
 */
class CALLIGRA_SHEETS_CORE_EXPORT CellStorage : public CellBaseStorage, public QObject
{
    Q_OBJECT
public:
    enum Visiting {
        Values          = 0x01,
        Formulas        = 0x02,
        Comments        = 0x04,
        Links           = 0x08,
        Styles          = 0x10,
        ConditionStyles = 0x20,
        Validities      = 0x40,
        VisitContent    = 0x03, ///< just visit the cell contents: values, formulas
        VisitAll        = 0xFF  ///< visit all: cell contents, styles, comments, ...
    };

    /**
     * Constructor.
     * Creates an empty storage for \p sheet.
     */
    explicit CellStorage(Sheet *sheet);

    /**
     * Copy constructor.
     * \note Take care: does not perform a deep copy!
     */
    CellStorage(const CellStorage& other);

    /**
     * Copy constructor.
     * Creates a CellStorage for \p sheet and copies the data from \p other.
     */
    CellStorage(const CellStorage& other, Sheet* sheet);

    /**
     * Destructor.
     */
    ~CellStorage() override;

    /**
     * \return the full sheet this CellStorage is for.
     */
    Sheet* fullSheet() const;

    /**
     * Removes all data at \p col , \p row .
     */
    virtual void take(int col, int row) override;

    /**
     * \return the binding associated with the Cell at \p column , \p row .
     */
    Binding binding(int column, int row) const;
    void setBinding(const Region& region, const Binding& binding);
    void removeBinding(const Region& region, const Binding& binding);

    /**
     * \return the conditional formattings associated with the Cell at \p column , \p row .
     */
    Conditions conditions(int column, int row) const;
    void setConditions(const Region& region, Conditions conditions);

    /**
     * \return the database associated with the Cell at \p column , \p row .
     */
    Database database(int column, int row) const;
    QVector< QPair<QRectF, Database> > databases(const Region& region) const;
    void setDatabase(const Region& region, const Database& database);

    /**
     * \return the hyperlink associated with the Cell at \p column , \p row .
     */
    QString link(int column, int row) const;
    void setLink(int column, int row, const QString& link);

    void emitInsertNamedArea(const Region &region, const QString &namedArea);

    /**
     * \return the Style associated with the Cell at \p column , \p row .
     */
    Style style(int column, int row) const;

    /**
     * \return the Style associated with \p rect.
     */
    Style style(const QRect& rect) const;
    void setStyle(const Region& region, const Style& style);
    void insertSubStyle(const QRect& rect, const SharedSubStyle& subStyle);

    QSharedPointer<QTextDocument> richText(int column, int row) const;
    void setRichText(int column, int row, QSharedPointer<QTextDocument> text);

    /**
     */
    bool doesMergeCells(int column, int row) const;
    bool isPartOfMerged(int column, int row) const;

    /**
     * Merge the cell at \p column, \p row with the \p numXCells adjacent cells in horizontal
     * direction and with the \p numYCells adjacent cells in vertical direction. I.e. the
     * resulting cell spans \p numXCells + 1 columns and \p numYCells + 1 rows. Passing \c 0
     * as \p numXCells and \p numYCells unmerges the cell at \p column, \p row.
     *
     * \param column the master cell's column
     * \param row the master cell's row
     * \param numXCells number of horizontal cells to be merged in
     * \param numYCells number of vertical cells to be merged in
     *
     */
    void mergeCells(int column, int row, int numXCells, int numYCells);
    Cell masterCell(int column, int row) const;
    int mergedXCells(int column, int row) const;
    int mergedYCells(int column, int row) const;
    QList<Cell> masterCells(const Region& region) const;

    /**
     * Retrieve the first used data in \p col .
     * Can be used in conjunction with nextInColumn() to loop through a column.
     * \return the first used data in \p col or the default data, if the column is empty.
     */
    Cell firstInColumn(int col, Visiting visiting = VisitAll) const;

    /**
     * Retrieve the first used data in \p row .
     * Can be used in conjunction with nextInRow() to loop through a row.
     * \return the first used data in \p row or the default data, if the row is empty.
     */
    Cell firstInRow(int row, Visiting visiting = VisitAll) const;

    /**
     * Retrieve the last used data in \p col .
     * Can be used in conjunction with prevInColumn() to loop through a column.
     * \return the last used data in \p col or the default data, if the column is empty.
     */
    Cell lastInColumn(int col, Visiting visiting = VisitAll) const;

    /**
     * Retrieve the last used data in \p row .
     * Can be used in conjunction with prevInRow() to loop through a row.
     * \return the last used data in \p row or the default data, if the row is empty.
     */
    Cell lastInRow(int row, Visiting visiting = VisitAll) const;

    /**
     * Retrieve the next used data in \p col after \p row .
     * Can be used in conjunction with firstInColumn() to loop through a column.
     * \return the next used data in \p col or the default data, there is no further data.
     */
    Cell nextInColumn(int col, int row, Visiting visiting = VisitAll) const;

    /**
     * Retrieve the next used data in \p row after \p col .
     * Can be used in conjunction with firstInRow() to loop through a row.
     * \return the next used data in \p row or the default data, if there is no further data.
     */
    Cell nextInRow(int col, int row, Visiting visiting = VisitAll) const;

    /**
     * Retrieve the previous used data in \p col after \p row .
     * Can be used in conjunction with lastInColumn() to loop through a column.
     * \return the previous used data in \p col or the default data, there is no further data.
     */
    Cell prevInColumn(int col, int row, Visiting visiting = VisitAll) const;

    /**
     * Retrieve the previous used data in \p row after \p col .
     * Can be used in conjunction with lastInRow() to loop through a row.
     * \return the previous used data in \p row or the default data, if there is no further data.
     */
    Cell prevInRow(int col, int row, Visiting visiting = VisitAll) const;

    /**
     * The maximum occupied column, i.e. the horizontal storage dimension.
     * \return the maximum column
     */
    int columns(bool includeStyles = true) const;

    /**
     * The maximum occupied row, i.e. the vertical storage dimension.
     * \return the maximum row
     */
    int rows(bool includeStyles = true) const;

    /**
     * Creates a substorage consisting of the values in \p region.
     * \return a subset of the storage stripped down to the values in \p region
     */
    CellStorage subStorage(const Region& region) const;

    const BindingStorage* bindingStorage() const;
    const ConditionsStorage* conditionsStorage() const;
    const FusionStorage* fusionStorage() const;
    const LinkStorage* linkStorage() const;
    const StyleStorage* styleStorage() const;

    void loadConditions(const QList<QPair<QRegion, Conditions> >& conditions);
    void loadStyles(const QList<QPair<QRegion, Style> >& styles);

    void invalidateStyleCache();

    /**
     * Starts the undo recording.
     * While recording the undo data of each storage operation is saved in
     * an undo command, that can be retrieved when the recording is stopped.
     * \see stopUndoRecording
     */
    void startUndoRecording();

    /**
     * Stops the undo recording.
     * An undo command has to be passed as \p parent command and
     * for each sub-storage an undo-capable command is attached to \p parent.
     * \see startUndoRecording
     */
    void stopUndoRecording(KUndo2Command *parent);

Q_SIGNALS:
    void insertNamedArea(const Region&, const QString&);
    void namedAreaRemoved(const QString&);

protected:
    /** Addition to the inherited fillStorages. We call this in constructors, so cannot override. */
    void fillExtraStorages();
private:
    // do not allow assignment
    CellStorage& operator=(const CellStorage&);

#ifdef CALLIGRA_SHEETS_MT
    QReadWriteLock bigUglyLock;
#endif
    bool undoEnabled;

    class Private;
    Private * const d;
};

class LinkStorage : public PointStorage<QString>
{
public:
    LinkStorage& operator=(const PointStorage<QString>& o) {
        PointStorage<QString>::operator=(o);
        return *this;
    }
};

class RichTextStorage : public PointStorage<QSharedPointer<QTextDocument> >
{
public:
    RichTextStorage& operator=(const PointStorage<QSharedPointer<QTextDocument> >& o) {
        PointStorage<QSharedPointer<QTextDocument> >::operator=(o);
        return *this;
    }
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CELL_STORAGE
