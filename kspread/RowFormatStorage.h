#ifndef KSPREAD_ROWFORMATSTORAGE_H
#define KSPREAD_ROWFORMATSTORAGE_H

#include <Qt>

#include "kspread_export.h"

namespace KSpread {

class Sheet;

/** first and last row are both inclusive in all functions */
class KSPREAD_EXPORT RowFormatStorage
{
public:
    RowFormatStorage(Sheet* sheet);
    ~RowFormatStorage();

    Sheet* sheet() const;

    qreal rowHeight(int row, int* lastRow = 0, int* firstRow = 0) const;
    void setRowHeight(int firstRow, int lastRow, qreal height);

    qreal visibleHeight(int row, int* lastRow = 0, int* firstRow = 0) const;

    qreal totalRowHeight(int firstRow, int lastRow) const;
    qreal totalVisibleRowHeight(int firstRow, int lastRow) const;

    bool isHidden(int row, int* lastRow = 0, int* firstRow = 0) const;
    void setHidden(int firstRow, int lastRow, bool hidden);

    bool isFiltered(int row, int* lastRow = 0, int* firstRow = 0) const;
    void setFiltered(int firstRow, int lastRow, bool filtered);

    bool isHiddenOrFiltered(int row, int* lastRow = 0, int* firstRow = 0) const;

    bool hasPageBreak(int row, int* lastRow = 0, int* firstRow = 0) const;
    void setPageBreak(int firstRow, int lastRow, bool pageBreak);

    int lastNonDefaultRow() const;
    bool rowsAreEqual(int row1, int row2) const;

    bool isDefaultRow(int row, int* lastRow = 0, int* firstRow = 0) const;
    void setDefault(int firstRow, int lastRow);

    /**
     * Insert \p number of rows at position \p row.
     * Also updates the sheets documentHeight property
     */
    void insertRows(int row, int number);

    /**
     * Removes \p number of rows starting at position \p row.
     * Also updates the sheets documentHeight property
     */
    void removeRows(int row, int number);

    RowFormatStorage& operator=(const RowFormatStorage& r);
private:
    RowFormatStorage(const RowFormatStorage&);
    class Private;
    Private * const d;
};


} // namespace KSpread

#endif // KSPREAD_ROWFORMATSTORAGE_H

