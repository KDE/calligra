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
#include <QLinkedList>
#include <QList>
#include <QSharedDataPointer>

#include <KoXmlReader.h>

#include "Condition.h"
#include "Global.h"
#include "Format.h"

class QDomElement;
class QDomDocument;
class QFont;
class QFontMetrics;
class QPainter;
class QRect;
class QPoint;

class KLocale;
class KoXmlWriter;
class KoGenStyles;
class KoGenStyle;
class KSParseNode;
class KoOasisStyles;
class KoOasisLoadingContext;

namespace KSpread
{
class CellView;
class Canvas;
class ConditionalDialog;
class Doc;
class Formula;
class GenValidationStyles;
class Sheet;
class Validity;
class Value;
class View;

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
     * \note Accessing the sheet(), column() or row() of this cell will fail.
     */
    Cell();

    /**
     * Constructor.
     * Creates a cell in \p sheet at position \p col , \p row .
     */
    Cell( const Sheet* sheet, int column, int row );

    /**
     * Constructor.
     * Creates a cell in \p sheet at position \p pos .
     */
    Cell( const Sheet* sheet, const QPoint& pos );

    /**
     * Copy constructor.
     */
    Cell( const Cell& other );

    /**
     * @see #sheetDies
     */
    ~Cell();

    /**
     * \return the sheet this cell belongs to
     */
    Sheet* sheet() const;

    /**
     * \return the document this cell belongs to
     */
    Doc* doc() const;

    /**
     * Returns the locale setting of this cell.
     */
    KLocale* locale() const;

    /**
     * Returns true if this is a default cell (with row and column equal to zero).
     * Normally, cell constructed within a sheet can't be a default cell.
     */
    bool isDefault() const;

    /**
     * Returns true if this cell has no content, i.e no text and no formula.
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
     * Returns the cell's column. This could be 0 if the cell is the default cell.
     */
    int column() const;

    /**
     * Returns the cell's row. This could be 0 if the cell is the default cell.
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
    static QString name( int col, int row );

    /**
     * Given the sheet and cell position, this static function returns the full name
     * of the cell, i.e. with the name of the sheet.
     */
    static QString fullName( const Sheet *s, int col, int row );

    /**
     * Given the column number, this static function returns the corresponding
     * column name, i.e. the first column is "A", the second is "B", and so on.
     */
    static QString columnName( uint column );

    /**
     * Return the text the user entered. This could be a value (e.g. "14.03")
     * or a formula (e.g. "=SUM(A1:A10)")
     */
    QString inputText() const;

    /**
     * \return the output text, e.g. the result of a formula
     */
    QString displayText() const;

    /**
     * \return the comment associated with this cell
     */
    QString comment() const;

    void setComment( const QString& comment );

    /**
     * \return the conditions associated with this cell
     */
    Conditions conditions() const;

    void setConditions( const Conditions& conditions );

    /**
     * The cell's formula. Usable to analyze the formula's tokens.
     * \return pointer to the cell's formula object
     */
    Formula formula() const;

    /**
     * Sets \p formula as associated formula of this cell.
     */
    void setFormula( const Formula& formula );

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
    void setLink( const QString& link );

    /**
     * \return the Style associated with this Cell
     */
    Style style() const;

    void setStyle( const Style& style );

    /**
     * \return the validity checks associated with this cell
     */
    Validity validity() const;

    void setValidity( Validity validity );

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
     */
    void setValue( const Value& value );

    /**
     * Sets the value for this cell and its formatting and input text, if
     * appropriate. Can therefore be used as a replacement for setCellText,
     * if we don't need to parse.
     *
     * If \p inputText is empty and the cell has NO formula, the input text
     * is created from \p value .
     *
     * \param value the new cell value
     * \param fmtType the formatting type
     * \param inputText the new input text
     *
     * \note Calls setValue() after setting the formatting and input text.
     */
    void setCellValue (const Value& value, Format::Type fmtType = Format::None,
                       const QString& inputText = QString());

    /**
     * Parses \p text and sets the appropriate formatting.
     * The high-level method for setting text, when the user inputs it.
     * It will revert back to the old text, if testValidity() returns action==stop.
     */
    void setCellText( const QString& text );

    /**
     * Moves around the cell. It cares about obscured and obscuring cells and
     * forces, relayout, calculation and redrawing of the cell.
     */
    void move( int column, int row );

    /**
     * \ingroup NativeFormat
     */
    bool load( const KoXmlElement& cell,
               int _xshift, int _yshift,
               Paste::Mode pm = Paste::Normal,
               Paste::Operation op = Paste::OverWrite,
               bool paste = false );

    /**
     * \ingroup NativeFormat
     * Save this cell.
     * @param doc document to save cell in
     * @param _x_offset x offset
     * @param _y_offset y offset
     * @param force if set to true, all the properties of the format are stored (used for "Copy"),
     *              otherwise only the non-default properties will be stored.
     *              Set this to false if you want smaller files.
     * @param copy if set to true, all cell formats will be copied instead of referencing the format (style name),
     *             thus resulting in larger output (files).
     *             Set this to false if you want smaller files.
     * @param era set this to true if you want to encode relative references as absolutely (they will be switched
     *            back to relative references during decoding) - is used for cutting to clipboard
     *            Usually this is false, to only store the properties explicitly set.
     */
    QDomElement save( QDomDocument& doc,
                      int _x_offset = 0, int _y_offset = 0,
                      bool force = false, bool copy = false, bool era = false );

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
     * @param oasisContext The loading context assoiated with the XML element
     */
    bool loadOasis( const KoXmlElement& element, KoOasisLoadingContext& oasisContext );

    /**
     * \ingroup OpenDocument
     */
    bool saveOasis( KoXmlWriter& xmlwriter, KoGenStyles& mainStyles,
                    int row, int column, int &repeated,
                    GenValidationStyles &valStyle );

    /**
     * Copies the format from \p cell .
     *
     * @see copyAll(Cell *cell)
     */
    void copyFormat( const Cell& cell );

    /**
     * Copies the content from \p cell .
     *
     * @see copyAll(Cell *cell)
     */
    void copyContent( const Cell& cell );

    /**
     * Copies the format and the content. It does not copy the row and column indices.
     * Besides that all persistent attributes are copied. setCellText() is called to set the real
     * content.
     *
     * @see copyFormat( const Cell& cell )
     */
    void copyAll( const Cell& cell);

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

    /**
     * Increases the precision of the
     * value displayed. Precision means here the amount of
     * digits behind the dot. If the current precision is the
     * default of -1, then it is set to the number of digits
     * behind the dot plus 1.
     */
    void incPrecision();

    /**
     * Decreases the precision of the
     * value displayed. Precision means here the amount of
     * digits behind the dot. If the current precision is the
     * default of -1, then it is set to the number of digits
     * behind the dot minus 1.
     */
    void decPrecision();

    //////////////////////
    //
    // Other stuff
    //
    //////////////////////

    /**
     * Return the format of this cell.
     * Convenience method for Format::getFormatType
     * Note that this is "how the user would like the data to be displayed if possible".
     * If he selects a date format, and the cell contains a string, we won't apply that format.
     */
    Format::Type formatType() const;

    /** returns true, if cell format is of date type or content is a date */
    bool isDate() const;
    /** returns true, if cell format is of time type or content is a time */
    bool isTime() const;

    /**
    * Refreshing chart
    * @param refresh is default true
    * when it's false it's just for test
    * it's used when you paste cell
    */
    bool updateChart(bool refresh=true);

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
     * \return the merging cell (might be null)
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
    void mergeCells( int _col, int _row, int _x, int _y );

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
     * Encodes a formula into a text representation.
     *
     * @param _era encode relative references absolutely (this is used for copying
     *             a cell to make the paste operation create a formula that points
     *             to the original cells, not the cells at the same relative position)
     * @param _col row the formula is in
     * @param _row column the formula is in
     *
     * \todo possibly rewrite to make use of the formula tokenizer
     */
    QString encodeFormula( bool _era = false, int _col = -1, int _row = -1 ) const;

    /**
     * inverse operation to encodeFormula()
     * \see encodeFormula()
     *
     * \todo possibly rewrite to make use of the formula tokenizer
     */
    QString decodeFormula( const QString &_text, int _col = -1, int _row = -1 ) const;

    /**
     * Merges the @p new_text with @p old_text during a paste operation.
     * If both texts represent doubles, then the operation is performed on both
     * values and the result is returned. If both texts represents a formula or
     * one a formula and the other a double value, then a formula is returned.
     * In all other cases @p new_text is returned.
     *
     * @return the merged text.
     */
    QString pasteOperation( const QString &new_text, const QString &old_text, Paste::Operation op );

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

    /**
     * Resets the used style to the default. Clears any conditional styles and
     * any content validity checks.
     */
    void defaultStyle();

    //
    //END 
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN 
    //

    /**
     * return align X when align is undefined
     */
    int defineAlignX();

    //
    //END 
    //
    //////////////////////////////////////////////////////////////////////////
    //
    //BEGIN Operators
    //

    /**
     * Assignment.
     */
    Cell& operator=( const Cell& other );

    /**
     * Tests wether this cell's location is less than the \p other 's.
     * (QMap support)
     * \note Does not compare the cell attributes/data.
     */
    bool operator<( const Cell& other ) const;

    /**
     * Tests for equality with \p other 's location only.
     * (QHash support)
     * \note Does not compare the cell attributes/data.
     */
    bool operator==( const Cell& other ) const;

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
    bool compareData( const Cell& other ) const;

protected:
    /**
     * \ingroup OpenDocument
     * Load the text paragraphs from an OASIS XML cell description.
     * @param parent The DOM element representing the cell.
     */
    void loadOasisCellText( const KoXmlElement& parent );

    /**
     * \ingroup OpenDocument
     */
    void loadOasisObjects( const KoXmlElement& e, KoOasisLoadingContext& oasisContext );

    /**
     * \ingroup OpenDocument
     */
    void saveOasisAnnotation( KoXmlWriter &xmlwriter );

private:
    class Private;
    QSharedDataPointer<Private> d;

    /**
     * Triggers the events, that are necessary after a value change.
     */
    void valueChanged();

    /**
     * \ingroup NativeFormat
     */
    bool loadCellData(const KoXmlElement &text, Paste::Operation op);

    /**
     * \ingroup NativeFormat
     */
    bool saveCellResult( QDomDocument& doc, QDomElement& result, QString str );

    /**
     * \ingroup OpenDocument
     * \todo TODO Stefan: merge this into Oasis::decodeFormula
     */
    void checkForNamedAreas( QString & formula ) const;

    /**
     * \ingroup OpenDocument
     */
    void saveOasisValue (KoXmlWriter &xmlWriter);

    /**
     * \ingroup OpenDocument
     * @return the OASIS style's name
     */
    QString saveOasisCellStyle( KoGenStyle &currentCellStyle, KoGenStyles &mainStyles );

    /**
     * Sets the input text.
     * If it's a formula, creates a formula object and sets the input text as
     * its expression. Otherwise, sets the input text as string value.
     * \internal
     */
    void setInputText( const QString& string );
};

inline uint qHash( const Cell& cell )
{
    return ( static_cast<uint>( cell.column() ) << 16 ) + static_cast<uint>( cell.row() );
}

} // namespace KSpread

Q_DECLARE_TYPEINFO( KSpread::Cell, Q_MOVABLE_TYPE );

#endif  // KSPREAD_CELL
