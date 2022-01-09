/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_CELL_BASE_STORAGE
#define CALLIGRA_SHEETS_CELL_BASE_STORAGE

#include "sheets_engine_export.h"
#include "PointStorage.h"


#ifdef CALLIGRA_SHEETS_MT
#include <QReadWriteLock>
#endif


namespace Calligra
{
namespace Sheets
{

class Binding;
class BindingStorage;
class Formula;
class FormulaStorage;
class Region;
class SheetBase;
class UserInputStorage;
class Value;
class ValueStorage;
class Validity;
class ValidityStorage;


/**
 * \ingroup Storage
 * The cell storage.
 * A wrapper around a couple of storages, which hold the cell data.
 * Provides methods to iterate over the non-empty cells.
 * Emits Damages on changes.
 * Capable of recording the old data for undoing.
 *
 * \author Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 *
 * \note If you fill the storage, do it row-wise. That's more performant.
 */
class CALLIGRA_SHEETS_ENGINE_EXPORT CellBaseStorage {
public:
    /**
     * Constructor.
     * Creates an empty storage for \p sheet.
     */
    explicit CellBaseStorage(SheetBase *sheet);

    /**
     * Copy constructor.
     * \note Take care: does not perform a deep copy!
     */
    CellBaseStorage(const CellBaseStorage& other);

    /**
     * Copy constructor.
     * Creates a CellBaseStorage for \p sheet and copies the data from \p other.
     */
    CellBaseStorage(const CellBaseStorage& other, SheetBase* sheet);

    /**
     * Destructor.
     */
    virtual ~CellBaseStorage();

    /**
     * \return the sheet this CellBaseStorage is for.
     */
    SheetBase* sheet() const;

    /**
     * Removes all data at \p col , \p row .
     */
//    virtual void take(int col, int row);


    /**
     * \return the formula associated with the Cell at \p column , \p row .
     */
    Formula formula(int column, int row) const;
    void setFormula(int column, int row, const Formula& formula);

    /**
     * \return the user input associated with the Cell at \p column , \p row .
     */
    QString userInput(int column, int row) const;
    void setUserInput(int column, int row, const QString& input);

    /**
     * \return the value associated with the Cell at \p column , \p row .
     */
    Value value(int column, int row) const;

    /**
     * Creates a value array containing the values in \p region.
     */
    Value valueRegion(const Region& region) const;
    void setValue(int column, int row, const Value& value);

    /**
     * \return the binding associated with the Cell at \p column , \p row .
     */
    Binding binding(int column, int row) const;
    void setBinding(const Region& region, const Binding& binding);
    void removeBinding(const Region& region, const Binding& binding);

    /**
     * \return the validity checks associated with the Cell at \p column , \p row .
     */
    Validity validity(int column, int row) const;
    void setValidity(const Region& region, Validity validity);




    /**
     * Insert \p number columns at \p position .
     * \return the data, that became out of range (shifted over the end)
     */
    void insertColumns(int position, int number = 1);

    /**
     * Removes \p number columns at \p position .
     * \return the removed data
     */
    void removeColumns(int position, int number = 1);

    /**
     * Insert \p number rows at \p position .
     * \return the data, that became out of range (shifted over the end)
     */
    void insertRows(int position, int number = 1);

    /**
     * Removes \p number rows at \p position .
     * \return the removed data
     */
    void removeRows(int position, int number = 1);

    /**
     * Shifts the data right of \p rect to the left by the width of \p rect .
     * The data formerly contained in \p rect becomes overridden.
     */
    void removeShiftLeft(const QRect& rect);

    /**
     * Shifts the data in and right of \p rect to the right by the width of \p rect .
     */
    void insertShiftRight(const QRect& rect);

    /**
     * Shifts the data below \p rect to the top by the height of \p rect .
     * The data formerly contained in \p rect becomes overridden.
     */
    void removeShiftUp(const QRect& rect);

    /**
     * Shifts the data in and below \p rect to the bottom by the height of \p rect .
     */
    void insertShiftDown(const QRect& rect);



    const BindingStorage* bindingStorage() const;
    const FormulaStorage* formulaStorage() const;
    const UserInputStorage* userInputStorage() const;
    const ValidityStorage* validityStorage() const;
    const ValueStorage* valueStorage() const;
protected:
    void fillStorages();

private:
    class Private;
    Private * const d;
};

class CALLIGRA_SHEETS_ENGINE_EXPORT UserInputStorage : public PointStorage<QString>
{
public:
    UserInputStorage& operator=(const PointStorage<QString>& o) {
        PointStorage<QString>::operator=(o);
        return *this;
    }
};


} // namespace Sheets
} // namespace Calligra

#endif   // CALLIGRA_SHEETS_CELL_BASE_STORAGE
