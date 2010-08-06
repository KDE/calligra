/* This file is part of the KDE project
   Copyright 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2004 Tomas Mecir <mecirt@gmail.com>
   Copyright 1999-2002,2004 Laurent Montel <montel@kde.org>
   Copyright 2002,2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2003 Stefan Hetzl <shetzl@chello.at>
   Copyright 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002 Harri Porten <porten@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 1999-2001 David Faure <faure@kde.org>
   Copyright 2000-2001 Werner Trobin <trobin@kde.org>
   Copyright 2000 Simon Hausmann <hausmann@kde.org
   Copyright 1998-1999 Torben Weis <weis@kde.org>
   Copyright 1999 Michael Reiher <michael.reiher@gmx.de>
   Copyright 1999 Reginald Stadlbauer <reggie@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_CELL
#define KSPREAD_CELL

#include <QDate>
#include <QSharedDataPointer>
#include <QSharedPointer>
#include <QTextDocument>

#include "Condition.h"
#include "Global.h"
#include "Format.h"

#include "database/Database.h"

class QDomElement;
class QDomDocument;
class QFont;
class QRect;
class QPoint;

class KLocale;
class KoXmlWriter;
class KoGenStyles;
class KoGenStyle;
class KoOdfLoadingContext;

namespace KSpread
{
class Doc;
class Formula;
class OdfLoadingContext;
class OdfSavingContext;
class Sheet;
class Validity;
class Value;

/**
 * An accessor to the actual cell data.
 * The Cell object acts as accessor to the actual data stored in the separate
 * storages in CellStorage. It provides methods to alter and retrieve this data
 * and methods related to loading and saving the contents.
 */
class KSPREAD_EXPORT Cell
{
public:
    /**
     * Constructor.
     * Creates the null cell.
     * \note Accessing the sheet(), column() or row() methods or any related method, that makes use
     * of the former, will fail.
     */
    Cell();

    /**
     * Constructor.
     * Creates a Cell for accessing the data in \p sheet at position \p col , \p row .
     */
    Cell(const Sheet* sheet, int column, int row);

    /**
     * Constructor.
     * Creates a Cell for accessing the data in \p sheet at position \p pos .
     */
    Cell(const Sheet* sheet, const QPoint& pos);

    /**
     * Copy constructor.
     */
    Cell(const Cell& other);

    /**
     * Destructor.
     */
    ~Cell();

    /**
     * \return the sheet this cell belongs to
     */
    Sheet* sheet() const;

    /**
     * Returns the locale setting of this cell.
     */
    KLocale* locale() const;

    /**
     * Returns true, if this is a default cell, i.e. if the cell has no value, formula, link and
     * does not merge any other cells, and has no custom style.
     */
    bool isDefault() const;

    /**
     * Returns true, if this is a cell with default content, i.e. if the cell has no value, formula, link and
     * does not merge any other cells. This is the same as isDefault, except that the style
     * is not taken into account here.
     */
    bool hasDefaultContent() const;

    /**
     * Returns true, if this cell has no content, i.e no value and no formula.
     */
    bool isEmpty() const;

    /**
     * Returns true if this cell is the null cell.
     */
    bool isNull() const;

    /**
     * Returns true if this cell holds a formula.
     */
    bool isFormula() const;

    /**
     * Returns the cell's column.
     */
    int column() const;

    /**
     * Returns the cell's row.
     */
    int row() const;

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
    static QString fullName(const Sheet *s, int col, int row);

    /**
     * Given the column number, this static function returns the corresponding
     * column name, i.e. the first column is "A", the second is "B", and so on.
     */
    static QString columnName(uint column);

    /**
     * \return the output text, e.g. the result of a formula
     */
    QString displayText() const;

    /**
     * \return the comment associated with this cell
     */
    QString comment() const;

    void setComment(const QString& comment);

    /**
     * \return the conditions associated with this cell
     */
    Conditions conditions() const;

    void setConditions(const Conditions& conditions);

    /**
     * \return the database associated with this cell
     */
    Database database() const;

    /**
     * The cell's formula. Usable to analyze the formula's tokens.
     * \return pointer to the cell's formula object
     */
    Formula formula() const;

    /**
     * Sets \p formula as associated formula of this cell.
     */
    void setFormula(const Formula& formula);

    /**
     * Returns the link associated with cell. It is empty if this cell
     * contains no link.
     */
    QString link() const;

    /**
     * Sets a link for this cell. For example, setLink( "mailto:joe@somewhere.com" )
     * will open a new e-mail if this cell is clicked.
     * Possible choices for link are URL (web, ftp), e-mail address, local file,
     * or another cell.
     */
    void setLink(const QString& link);

    /**
     * \return the Style associated with this Cell
     */
    Style style() const;

    /**
     * The effective style takes conditional style attributes into account.
     * \return the effective Style associated with this Cell
     */
    Style effectiveStyle() const;

    void setStyle(const Style& style);

    /**
     * \return the validity checks associated with this cell
     */
    Validity validity() const;

    void setValidity(Validity validity);

    /**
     * Returns the value that this cell holds. It could be from the user
     * (i.e. when s/he enters a value) or a result of formula.
     */
    const Value value() const;

    /**
     * Sets the value for this cell.
     * It also clears all errors, if the value itself is not an error.
     * In addition to this, it calculates the outstring and sets the dirty
     * flags so that a redraw is forced.
     * \param value the new value
     *
     * \see setUserInput, parseUserInput
     */
    void setValue(const Value& value);

    /**
     * Returns the richtext that this cell holds.
     */
    QSharedPointer<QTextDocument> richText() const;

    /**
     * Sets the richtext for this cell.
     * If \p text is empty, richtext is removed.
     *
     * \param text the new richtext
     */
    void setRichText(QSharedPointer<QTextDocument> text);

    /**
     * Return the text the user entered. This could be a value (e.g. "14.03")
     * or a formula (e.g. "=SUM(A1:A10)")
     */
    QString userInput() const;

    /**
     * Sets the user input without parsing it.
     * If \p text is a formula, creates a formula object and sets \p text as
     * its expression. Otherwise, simply stores \p text as user input.
     *
     * \see parseUserInput, setValue
     */
    void setUserInput(const QString& text);

    /**
     * Sets the user input and parses it.
     * If \p text is a formula, creates a formula object and sets \p text as
     * its expression. Otherwise, parses \p text, creates an appropriate value,
     * including the proper type, validates the value and, if accepted, stores
     * \p text as the user input and the value as the cell's value.
     *
     * \see setUserInput, setValue
     */
    void parseUserInput(const QString& text);

    /**
     * \ingroup NativeFormat
     */
    bool load(const KoXmlElement& cell,
              int _xshift, int _yshift,
              Paste::Mode pm = Paste::Normal,
              Paste::Operation op = Paste::OverWrite,
              bool paste = false);

    /**
     * \ingroup NativeFormat
     * Save this cell.
     * @param doc document to save cell in
     * @param xOffset x offset
     * @param yOffset y offset
     * @param era set this to true if you want to encode relative references as absolutely (they will be switched
     *            back to relative references during decoding) - is used for cutting to clipboard
     *            Usually this is false, to only store the properties explicitly set.
     */
    QDomElement save(QDomDocument& doc, int xOffset = 0, int yOffset = 0, bool era = false);

    /**
     * \ingroup NativeFormat
     * Decodes a string into a time value.
     */
    QTime toTime(const KoXmlElement &element);

    /**
     * \ingroup NativeFormat
     * Decodes a string into a date value.
     */
    QDate toDate(const KoXmlElement &element);

    /**
     * \ingroup OpenDocument
     * Loads a cell from an OASIS XML element.
     * @param element An OASIS XML element
     * @param tableContext The loading context assoiated with the XML element
     */
    bool loadOdf(const KoXmlElement& element, OdfLoadingContext& tableContext, const Style& style);

    /**
     * \ingroup OpenDocument
     */
    bool saveOdf(KoXmlWriter& xmlwriter, KoGenStyles& mainStyles,
                 int row, int column, int &repeated,
                 OdfSavingContext& savingContext);

    /**
     * Copies the format from \p cell .
     *
     * @see copyAll(Cell *cell)
     */
    void copyFormat(const Cell& cell);

    /**
     * Copies the content from \p cell .
     *
     * @see copyAll(Cell *cell)
     */
    void copyContent(const Cell& cell);

    /**
     * Copies the format and the content from \p cell .
     *
     * @see copyContent( const Cell& cell )
     * @see copyFormat( const Cell& cell )
     */
    void copyAll(const Cell& cell);

    /**
     * @return the width of this cell as double
     */
    double width() const;

    /**
     * @return the height of this cell as double
     */
    double height() const;

    /**
     * \return the position of this cell
     */
    QPoint cellPosition() const;

    /**
     * @return true if the cell should be printed in a print out.
     *         That's the case, if it has any content, border, backgroundcolor,
     *         or background brush.
     *
     * @see Sheet::print
     */
    bool needsPrinting() const;

    //
    //END
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Merging
    //

    /**
     * If this cell is part of a merged cell, then the marker may
     * never reside on this cell.
     *
     * @return true if another cell has this one merged into itself.
     */
    bool isPartOfMerged() const;

    /**
     * \return the merging cell (might be this cell)
     */
    Cell masterCell() const;

    /**
     * Merge a number of cells, i.e. force the cell to occupy other
     * cells space.  If '_x' and '_y' are 0 then the merging is
     * disabled.
     *
     * @param _col is the column this cell is assumed to be in.
     * @param _row is the row this cell is assumed to be in.
     * @param _x tells to occupy _x additional cells in the horizontal
     * @param _y tells to occupy _y additional cells in the vertical
     *
     */
    void mergeCells(int _col, int _row, int _x, int _y);

    /**
     * @return true if the cell is forced to obscure other cells.
     */
    bool doesMergeCells() const;

    /**
     * @return the number of obscured cells in the horizontal direction as a
     *         result of cell merging (forced obscuring)
     */
    int mergedXCells() const;

    /**
     * @return the number of obscured cells in the vertical direction as a
     *         result of cell merging (forced obscuring)
     */
    int mergedYCells() const;

    //
    //END Merging
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Matrix locking
    //

    bool isLocked() const;
    QRect lockedCells() const;

    //
    //END Matrix locking
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Cut & paste
    //

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
    QString decodeFormula(const QString& text) const;

    /**
     * Merges the @p new_text with @p old_text during a paste operation.
     * If both texts represent doubles, then the operation is performed on both
     * values and the result is returned. If both texts represents a formula or
     * one a formula and the other a double value, then a formula is returned.
     * In all other cases @p new_text is returned.
     *
     * @return the merged text.
     */
    QString pasteOperation(const QString &new_text, const QString &old_text, Paste::Operation op);

    //
    //END Cut & paste
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN
    //

    /**
     * Parses the formula.
     * @return @c false on error.
     */
    bool makeFormula();

    //
    //END
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Effective style attributes
    //

    /** returns horizontal alignment, depending on style and value type */
    int effectiveAlignX() const;
    /** returns true, if cell format is of date type or content is a date */
    bool isDate() const;
    /** returns true, if cell format is of time type or content is a time */
    bool isTime() const;
    /** returns true, if cell format is of text type */
    bool isText() const;

    //
    //END Effective style attributes
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Operators
    //

    /**
     * Assignment.
     */
    Cell& operator=(const Cell& other);

    /**
     * Tests whether this cell's location is less than the \p other 's.
     * (QMap support)
     * \note Does not compare the cell attributes/data.
     */
    bool operator<(const Cell& other) const;

    /**
     * Tests for equality with \p other 's location only.
     * (QHash support)
     * \note Does not compare the cell attributes/data.
     */
    bool operator==(const Cell& other) const;

    /**
     * Is null.
     */
    bool operator!() const;

    //
    //END Operators
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN
    //

    /**
     * Tests for equality of all cell attributes/data to those in \p other .
     */
    bool compareData(const Cell& other) const;

protected:
    /**
     * \ingroup OpenDocument
     * Load the text paragraphs from an OASIS XML cell description.
     * @param parent The DOM element representing the cell.
     */
    void loadOdfCellText(const KoXmlElement& parent, OdfLoadingContext& tableContext, const Style& style);

    /**
     * \ingroup OpenDocument
     */
    void loadOdfObjects(const KoXmlElement& e, OdfLoadingContext& tableContext);

    /**
     * \ingroup OpenDocument
     */
    void saveOdfAnnotation(KoXmlWriter &xmlwriter);

private:
    class Private;
    QSharedDataPointer<Private> d;

    /**
     * \ingroup NativeFormat
     */
    bool loadCellData(const KoXmlElement &text, Paste::Operation op, const QString &dataType = QString());

    /**
     * \ingroup NativeFormat
     */
    bool saveCellResult(QDomDocument& doc, QDomElement& result, QString str);

    /**
     * \ingroup OpenDocument
     */
    void saveOdfValue(KoXmlWriter &xmlWriter);

    /**
     * \ingroup OpenDocument
     * @return the OASIS style's name
     */
    QString saveOdfCellStyle(KoGenStyle &currentCellStyle, KoGenStyles &mainStyles);
};

inline uint qHash(const Cell& cell)
{
    return (static_cast<uint>(cell.column()) << 16) + static_cast<uint>(cell.row());
}

} // namespace KSpread

Q_DECLARE_TYPEINFO(KSpread::Cell, Q_MOVABLE_TYPE);


/***************************************************************************
  kDebug support
****************************************************************************/

inline kdbgstream operator<<(kdbgstream str, const KSpread::Cell& cell)
{
    return str << qPrintable(KSpread::Cell::columnName(cell.column()) + QString::number(cell.row()));
}

#endif  // KSPREAD_CELL
