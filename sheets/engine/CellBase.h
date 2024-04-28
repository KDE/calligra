/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2021 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __CELLBASE_H__
#define __CELLBASE_H__

#include "sheets_engine_export.h"

#include <QDebug>
#include <QPoint>
#include <QRect>
#include <QSharedDataPointer>

namespace Calligra
{
namespace Sheets
{

class Formula;
class SheetBase;
class Value;
class Validity;

class CALLIGRA_SHEETS_ENGINE_EXPORT CellBase
{
public:
    /**
     * Constructor.
     * Creates the null cell.
     * \note Accessing many methods will fail as the sheet is not set.
     */
    CellBase();

    /**
     * Constructor.
     * Creates a CellBase for accessing the data in \p sheet at position \p col , \p row.
     */
    CellBase(SheetBase *sheet, unsigned int column, unsigned int row);

    /**
     * Constructor.
     * Creates a Cell for accessing the data in \p sheet at position \p pos .
     */
    CellBase(SheetBase *sheet, const QPoint &pos);

    /**
     * Copy constructor.
     */
    CellBase(const CellBase &other);

    /**
     * Destructor.
     */
    virtual ~CellBase();

    /**
     * Returns true if this cell is the null cell.
     */
    bool isNull() const;

    /**
     * \return the sheet this cell belongs to
     */
    SheetBase *sheet() const;

    /**
     * Returns the cell's column.
     */
    int column() const;

    /**
     * Returns the cell's row.
     */
    int row() const;

    /**
     * \return the position of this cell
     */
    QPoint cellPosition() const;

    /**
     * Returns the name of the cell. For example, the cell in first column and
     * first row is "A1".
     */
    QString name() const;

    /**
     * Returns the full name of the cell, i.e. including the worksheet name.
     * Example: "Sheet1!A1"
     */
    QString fullName() const;

    /**
     * Returns the column name of the cell.
     */
    QString columnName() const;

    /**
     * Given the cell position, this static function returns the name of the cell.
     * Example: name(5,4) will return "E4".
     */
    static QString name(int col, int row);

    /**
     * Given the sheet and cell position, this static function returns the full name
     * of the cell, i.e. with the name of the sheet.
     */
    static QString fullName(const SheetBase *s, int col, int row);

    /**
     * Returns the value that this cell holds. It could be from the user
     * (i.e. when s/he enters a value) or a result of formula.
     */
    const Value value() const;

    /**
     * Sets the value for this cell.
     * It also clears all errors, if the value itself is not an error.
     * \param value the new value
     *
     * \see setUserInput, parseUserInput
     */
    void setValue(const Value &value);

    /**
     * The cell's formula. Usable to analyze the formula's tokens.
     * \return pointer to the cell's formula object
     */
    Formula formula() const;

    /**
     * Sets \p formula as associated formula of this cell.
     */
    void setFormula(const Formula &formula);

    /**
     * Returns true, if this cell has no content, i.e no value and no formula.
     */
    bool isEmpty() const;

    /**
     * Returns true if this cell holds a formula.
     */
    bool isFormula() const;

    /**
     * Sets a cell value and generates the user input accordingly. Wrapper for setValue + setUserInput.
     */
    void setCellValue(const Value &value);

    /**
     * Return the text the user entered. This could be a value (e.g. "14.03")
     * or a formula (e.g. "=SUM(A1:A10)")
     */
    QString userInput() const;

    /**
     * Sets the user input without parsing it. Formulas are still processed.
     *
     * \see parseUserInput, setValue
     */
    virtual void setUserInput(const QString &text);

    /**
     * Sets the user input and parses it.
     *
     * \see setUserInput, setValue
     */
    void parseUserInput(const QString &text);

    virtual Value parsedUserInput(const QString &text);

    /**
     * Sets the user input without parsing it, without clearing existing formulas
     * userinput or rich text. Used during loading of documents only!
     * If \p text is a formula, creates a formula object and sets \p text as
     * its expression. Otherwise, simply stores \p text as user input.
     *
     * \see parseUserInput, setValue
     */
    void setRawUserInput(const QString &text);

    /**
     * \return the comment associated with this cell
     */
    QString comment() const;

    void setComment(const QString &comment);

    /**
     * \return the validity checks associated with this cell
     */
    Validity validity() const;

    void setValidity(Validity validity);

    /**
     * Encodes the cell's formula into a text representation.
     *
     * \param fixedReferences encode relative references absolutely (this is used for copying
     *             a cell to make the paste operation create a formula that points
     *             to the original cells, not the cells at the same relative position)
     * \see decodeFormula()
     */
    QString encodeFormula(bool fixedReferences = false) const;

    /**
     * Decodes a text representation \p text into a formula expression.
     *
     * \see encodeFormula()
     */
    QString decodeFormula(const QString &text) const;

    /**
     * Given the column number, this static function returns the corresponding
     * column name, i.e. the first column is "A", the second is "B", and so on.
     */
    static QString columnName(unsigned int column);

    //////////////////////////////////////////////////////////////////////////
    //
    // BEGIN Matrix locking
    //

    bool isLocked() const;
    QRect lockedCells() const;

    //
    // END Matrix locking
    //
    //////////////////////////////////////////////////////////////////////////
    //
    // BEGIN Operators
    //

    /**
     * Assignment.
     */
    CellBase &operator=(const CellBase &other);

    /**
     * Tests whether this cell's location is less than the \p other 's.
     * (QMap support)
     * \note Does not compare the cell attributes/data.
     */
    bool operator<(const CellBase &other) const;

    /**
     * Tests for equality with \p other 's location only.
     * (QHash support)
     * \note Does not compare the cell attributes/data.
     */
    bool operator==(const CellBase &other) const;

    /**
     * Is null.
     */
    bool operator!() const;

    //
    // END Operators

private:
    class Private;
    QSharedDataPointer<Private> d;
};

inline size_t qHash(const CellBase &cell, size_t seed = 0)
{
    return ::qHash((static_cast<uint>(cell.column()) << 16) + static_cast<uint>(cell.row()), seed);
}

} // namespace Sheets
} // namespace Calligra

/***************************************************************************
  QDebug support
****************************************************************************/

inline QDebug operator<<(QDebug str, const Calligra::Sheets::CellBase &cell)
{
    if (!cell) {
        return str << qPrintable(QStringLiteral("CellBase::isNull"));
    }
    return str << qPrintable(QString("%1%2").arg(Calligra::Sheets::CellBase::columnName(cell.column())).arg(QString::number(cell.row())));
}

#endif
