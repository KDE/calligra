/* This file is part of the KDE project

   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_CELL
#define KSPREAD_CELL

#include <QDate>
#include <QLinkedList>
#include <QList>

#include <KoXmlReader.h>

#include "Condition.h"
#include "Global.h"

class QDomElement;
class QDomDocument;
class QFont;
class QFontMetrics;
class QPainter;

class KLocale;
class KoXmlWriter;
class KoGenStyles;
class KoGenStyle;
class KSParseNode;
class KoRect;
class KoPoint;
class KoOasisStyles;
class KoOasisLoadingContext;

namespace KSpread
{
class CellView;
class Canvas;
class ConditionalDialog;
class Format;
class Formula;
class GenValidationStyles;
class Sheet;
class Validity;
class Value;
class View;

/**
 * For every cell in the spread sheet there is a Cell object.
 *
 * Cell contains format information and algorithm and it
 * contains the calculation algorithm.
 *
 * However, all empty cells are represented by one instace, called the
 * default cell. @ref #isDefault can be used to determine whether or not a Cell object represents
 * the default one.
 */
class KSPREAD_EXPORT Cell
{
  friend class CellView;
  friend class Conditions;
public:

  Cell (Sheet *_sheet, int _column, int _row);
  Cell (Sheet *_sheet, Style * _style, int _column, int _row);

    /**
     * @see #sheetDies
     */
    ~Cell();

    CellView* cellView() const;

    /**
     * Returns the worksheet which owns this cell.
     */
    Sheet* sheet() const;

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
     * Returns the locale setting of this cell.
     */
    KLocale* locale() const;
    /**
     * Returns true if this cell holds a formula.
     */
    bool isFormula() const;

    /**
     * Return the text the user entered. This could be a value (e.g. "14.03")
     * or a formula (e.g. "=SUM(A1:A10)")
     */
    QString text() const;

    QString strOutText() const;

    /**
     * The cell's formula. Usable to analyze the formula's tokens.
     * \return point to the cell's formula object
     */
    const Formula* formula() const;

    /** Returns the format object of this cell */
    Format* format() const;

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
     */
    void setValue( const Value& value );

    /**
     * Like setValue(), but also sets formatting type \p fmtType and
     * input text \p txt. Can therefore be used as a replacement for
     * setCellText(), if we don't need to parse.
     * If \p txt is not given, the input text is derived from the value.
     * \note Calls setValue() after setting the formatting and input text.
     */
    void setCellValue (const Value &v, FormatType fmtType = No_format,
                       const QString &txt = QString::null);

    Cell* previousCell() const;
    Cell* nextCell() const;
    void setPreviousCell( Cell* c );
    void setNextCell( Cell* c );

    /**
     * Moves around the cell. It cares about obscured and obscuring cells and
     * forces, relayout, calculation and redrawing of the cell.
     */
    void move( int column, int row );

    /**
     * This method notifies the cell that the parent sheet is being deleted.
     */
    // Note:  This used to remove any links from this cell to other cells.  However, this caused a problem
    // in other parts of the code which relied upon walking from one cell to the next using
    // nextCell().
    void sheetDies();

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
     * \ingroup OpenDocument
     */
    bool saveOasis( KoXmlWriter& xmlwriter, KoGenStyles& mainStyles,
                    int row, int column, int &repeated,
                    GenValidationStyles &valStyle );

    /**
     * \ingroup OpenDocument
     */
    void saveOasisValue (KoXmlWriter &xmlWriter);

    /**
     * \ingroup OpenDocument
     * @return the OASIS style's name
     */
    QString saveOasisCellStyle( KoGenStyle &currentCellStyle,KoGenStyles &mainStyles );

    /**
     * \ingroup OpenDocument
     * Loads a cell from an OASIS XML element.
     * @param element An OASIS XML element
     * @param oasisContext The loading context assoiated with the XML element
     * @param style preloaded cell style
     */
    bool loadOasis( const KoXmlElement & element, KoOasisLoadingContext &oasisContext , Style* style);


    QTime toTime(const KoXmlElement &element);
    QDate toDate(const KoXmlElement &element);

    /**
     * Copies the format from the Cell @p cell .
     *
     * @see copyAll(Cell *cell)
     */
    void copyFormat( const Cell* cell );

    void copyContent( const Cell* cell );

    /**
     * Copies the format and the content. It does not copy the row and column indices.
     * Besides that all persistent attributes are copied. setCellText() is called to set the real
     * content.
     *
     * @see copyFormat( const Cell* cell )
     * @see copyFormat( const int column, const int row )
     */
    void copyAll(Cell *cell);


    /**
     * @param _col the column this cell is assumed to be in.
     *             This parameter defaults to the return value of @ref #column.
     *
     * @return the width of this cell as int
     */
    int width( int _col = -1 ) const;

    /**
     * @param _row the row this cell is assumed to be in.
     *
     * @return the height of this cell as int
     */
    int height( int _row = -1 ) const;

    /**
     * @param _col the column this cell is assumed to be in.
     *             This parameter defaults to the return value of @ref #column.
     *
     * @return the width of this cell as double
     */
    double dblWidth( int _col = -1 ) const;

    /**
     * @param _row the row this cell is assumed to be in.
     *
     * @return the height of this cell as double
     */
    double dblHeight( int _row = -1 ) const;

    // TODO Stefan: Replace with a position() method!!!
    /**
     * @return a QRect for this cell (i.e., a 1x1 rect).  @see zoomedCellRect
     */
    QRect cellRect();

    /**
     * @return true if the cell should be printed in a print out.
     *         That si the case if it has any content, border, backgroundcolor,
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

    /**
     * The high-level method for setting text, when the user inputs it.
     * It will revert back to the old text, if testValidity() returns action==stop.
     */
    void setCellText( const QString& _text, bool asString = false );

    /**
     * Sets a link for this cell. For example, setLink( "mailto:joe@somewhere.com" )
     * will open a new e-mail if this cell is clicked.
     * Possible choices for link are URL (web, ftp), e-mail address, local file,
     * or another cell.
     */
    void setLink( const QString& link );

    /**
     * Returns the link associated with cell. It is empty if this cell
     * contains no link.
     */
    QString link() const;

    ////////////////////////////////
    //
    // Methods for querying format stuff.
    //
    ////////////////////////////////

    /**
     * @return effective pen for the left border
     * If this cell is merged by another cell, the other cell's
     * left border pen. If this cell's conditional formatting contains
     * a left border pen and the condition is matched, the conditional
     * formatting's pen. Otherwise, its own left border pen.
     */
    const QPen & effLeftBorderPen( int col, int row ) const;
    /**
     * @return effective pen for the top border
     * @see effLeftBorderPen
     */
    const QPen & effTopBorderPen( int col, int row ) const;
    /**
     * @return effective pen for the right border
     * @see effLeftBorderPen
     */
    const QPen & effRightBorderPen( int col, int row ) const;
    /**
     * @return effective pen for the bottom border
     * @see effLeftBorderPen
     */
    const QPen & effBottomBorderPen( int col, int row ) const;
    /**
     * @return effective pen for the go up diagonal border
     * If this cell's conditional formatting contains a go up diagonal pen and
     * the condition is matched, the conditional formatting's pen. Otherwise,
     * its own go up diagonal pen.
     */
    const QPen & effGoUpDiagonalPen( int col, int row ) const;
    /**
     * @return effective pen for the go up diagonal border
     * @see effGoUpDiagonalPen
     */
    const QPen & effFallDiagonalPen( int col, int row ) const;
    const QColor effTextColor( int col, int row ) const;

    /**
     * @return "worth" of the effective bottom border pen
     * @see Style::calculateValue
     * @see effLeftBorderPen
     */
    uint effBottomBorderValue( int col, int row ) const;
    /**
     * @return "worth" of the effective right border pen
     * @see Style::calculateValue
     * @see effLeftBorderPen
     */
    uint effRightBorderValue( int col, int row ) const;
    /**
     * @return "worth" of the effective left border pen
     * @see Style::calculateValue
     * @see effLeftBorderPen
     */
    uint effLeftBorderValue( int col, int row ) const;
    /**
     * @return "worth" of the effective top border pen
     * @see Style::calculateValue
     * @see effLeftBorderPen
     */
    uint effTopBorderValue( int col, int row ) const;

    /**
     * @see Format::leftBorderPen
     */
    const QPen& leftBorderPen( int col, int row ) const;

    /**
     * @see Format::topBorderPen
     */
    const QPen& topBorderPen( int col, int row ) const;

    /**
     * @see Format::rightBorderPen
     */
    const QPen& rightBorderPen( int col, int row ) const;

    /**
     * @see Format::bottomBorderPen
     */
    const QPen& bottomBorderPen( int col, int row ) const;

    /**
     * @see Format::bgColor
     */
    const QColor bgColor( int col, int row ) const;

    /**
     * @see Format::backGroundBrush
     */
    const QBrush& backGroundBrush( int col, int row ) const;

    ////////////////////////////////
    //
    // Methods for setting format stuff.
    //
    ////////////////////////////////

    /**
     * @see Format::setLeftBorderPen
     */
    void setLeftBorderPen( const QPen& p );

    /**
     * @see Format::setTopBorderPen
     */
    void setTopBorderPen( const QPen& p );

    /**
     * @see Format::setRightBorderPen
     */
    void setRightBorderPen( const QPen& p );

    /**
     * @see Format::setBottomBorderPen
     */
    void setBottomBorderPen( const QPen& p );

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
    FormatType formatType() const;

    /** returns true, if cell format is of date type or content is a date */
    bool isDate() const;
    /** returns true, if cell format is of time type or content is a time */
    bool isTime() const;

    void setNumber( double number );

    /** return the cell's value as a double */
    double getDouble ();

    /** converts content to double format */
    void convertToDouble ();
    /** converts content to percentageformat */
    void convertToPercent ();
    /** converts content to money format */
    void convertToMoney ();
    /** converts content to time format */
    void convertToTime ();
    /** converts content to date format */
    void convertToDate ();


    /**
    * Refreshing chart
    * @param refresh is default true
    * when it's false it's just for test
    * it's used when you paste cell
    */
    bool updateChart(bool refresh=true);

    /**
     * Starts calculating.
     * @param delay true if you want to check for delay condition in doc()
     *         false if you really have to calculate the value right now
     *         e.g. if you sort with formula as key
     *
     * @return true on success and false on error.
     */
    bool calc(bool delay = true);

    /**
     * Causes the format to be recalculated when the cell is drawn next time.
     * This flag is for example set if the width of the column changes or if
     * some cell specific format value like font or text change.
     */
    void setLayoutDirtyFlag( bool format = false );

    /**
     * Tells this cell that the Cell @p cell obscures this one.
     * If this cell has to be redrawn, then the obscuring cell is redrawn instead.
     *
     * @param cell the obscuring cell
     * @param isForcing whether this is a forced obscuring (merged cells) or
     *                  just a temporary obscure (text overlap).
     */
    void obscure( Cell *cell, bool isForcing = false);

    /**
     * Tells this cell that it is no longer obscured.
     *
     * @param cell the cell that is no longer obscuring this one.
     */
    void unobscure(Cell* cell);

    /**
     * @return true if this cell is obscured by another.
     */
    bool isObscured() const;

    /**
     * If this cell is part of a merged cell, then the marker may
     * never reside on this cell.
     *
     * @return true if another cell has this one merged into itself.
     */
    bool isPartOfMerged() const;

    /**
     * Return the cell that is obscuring this one (merged cells only).
     * If no obscuring, return the cell itself.
     *
     * @return the cell that decides the format for the cell in question.
     */
    Cell *ultimateObscuringCell() const;

    /**
     * @return the obscuring cell list (might be empty)
     */
    QList<Cell*> obscuringCells() const;

    void clearObscuringCells();


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

    /**
     * @return the amount of obscured cells in the horizontal direction
     */
    int extraXCells() const;

    /**
     * @return the amount of obscured cells in the vertical direction
     */
    int extraYCells() const;

    double extraWidth() const;
    double extraHeight() const;

    /**
     * encode a formula into a text representation
     *
     * @param _era encode relative references absolutely (this is used for copying
     *             a cell to make the paste operation create a formula that points
     *             to the original cells, not the cells at the same relative position)
     * @param _col row the formula is in
     * @param _row column the formula is in
     */
    QString encodeFormula( bool _era = false, int _col = -1, int _row = -1 ) const;
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

    /**
     * \return \c true if the cell contains a formula, that could not
     *         be evaluated, has a circular dependecency or an invalidated
     *         dependecency. These cells usually appear  on the screen with
     *         "#Parse!", "#Circle!" or "#Depend!", respectively.
     */
    bool hasError() const;

    /**
     * Clear all error flags from the cell.
     */
    void clearAllErrors();

    /**
     * Parses the formula.
     * @return @c false on error.
     */
    bool makeFormula();


    void defaultStyle();

    /**
     * Gets a copy of the list of current conditions
     */
    QLinkedList<Conditional> conditionList() const;

    /**
     * Replace the old set of conditions with a new one
     */
    void setConditionList(const QLinkedList<Conditional> &newList);

    Validity* validity( bool create = false );

    void removeValidity();

    /**
     * return true if value is good
     * else show a messagebox
     */
    bool testValidity() const;

    /**
     * return align X when align is undefined
     */
    int defineAlignX();




    /**
     * Used for comparing cells (when sorting)
     */
    bool operator > ( const Cell & ) const;
    bool operator < ( const Cell & ) const;

    bool operator==( const Cell& other ) const;
    inline bool operator!=( const Cell& other ) const { return !operator==( other ); }

    void freeAllObscuredCells();

    /**
     * Cell status flags.
     */
    enum StatusFlag
    {
      /**
       * Flag_LayoutDirty
       * Flag showing whether the current layout is OK.
       * If you change for example the fonts point size, set this flag. When the
       * cell must draw itself on the screen it will first recalculate its layout.
       */
      Flag_LayoutDirty           = 0x0001,
      /**
       * CalcDirty
       * Shows whether recalculation is necessary.
       * If this cell must be recalculated for some reason, for example the user
       * entered a new formula, then this flag is set. If isFormula() is false
       * nothing will happen at all.
       */
      Flag_CalcDirty             = 0x0002,
      /**
       * CalculatingCell
       * Tells whether this cell it currently under calculation.
       * If a cell thats 'progressFlag' is set is told to calculate we
       * have detected a circular reference and we must stop calulating.
       */
      Flag_CalculatingCell       = 0x0004,
      /**
       * UpdatingDeps
       * Tells whether we've already calculated the reverse dependancies for this
       * cell.  Similar to the Progress flag but it's for when we are calculating
       * in the reverse direction.
       * \todo TODO Stefan: never set so far
       */
      Flag_UpdatingDeps          = 0x0008,
      /**
       * Merged
       * Tells whether the cell is merged with other cells.  Cells may
       * occupy other cells space on demand. You may force a cell to
       * do so by setting this flag. Merging the cell with 0 in both
       * directions, will disable this flag!
       */
      Flag_Merged                = 0x0010,
      /**
       * CellTooShortX
       * When it's True displays ** and/or the red triangle and when the
       * mouse is over it, the tooltip displays the full value
       * it's true when text size is bigger that cell size
       * and when Align is center or left
       */
      Flag_CellTooShortX         = 0x0020,
      /**
       * CellTooShortY
       * When it's True when mouseover it, the tooltip displays the full value
       * it's true when text size is bigger that cell height
       */
      Flag_CellTooShortY         = 0x0040,
      /**
       * ParseError
       * True if the cell is calculated and there was an error during calculation
       * In that case the cell usually displays "#Parse!"
       */
      Flag_ParseError            = 0x0080,
      /**
       * CircularCalculation
       * True if the cell is calculated and there was an error during calculation
       * In that case the cell usually displays "#Circle!"
       */
      Flag_CircularCalculation   = 0x0100,
      /**
       * DependencyError
       * \todo TODO Stefan: never set so far
       */
      Flag_DependencyError       = 0x0200,
      /**
       * PaintingCell
       * Set during painting
       */
      Flag_PaintingCell          = 0x0400,
      /**
       * TextFormatDirty
       * \todo TODO Stefan: difference to Flag_LayoutDirty?
       */
      Flag_TextFormatDirty       = 0x0800
    };
    Q_DECLARE_FLAGS(StatusFlags, StatusFlag)

    void clearFlag( StatusFlag flag );
    void setFlag( StatusFlag flag );
    bool testFlag( StatusFlag flag ) const;

protected:
    /**
     * Cleans up formula stuff.
     * Call this before you store a new formula or to delete the
     * formula.
     */
    void clearFormula();

    /**
     * Sets the text in the cell when the user inputs it.
     * Will determine the type of contents automatically.
     * \internal Called by setCellText().
     */
    void setDisplayText( const QString& _text );

    /**
     * Check the input from the user, and determine the contents of the cell accordingly
     * (in particular the data type).
     * This is to be called only when m_content == Text.
     *
     * Input: m_strText
     * Output: m_dataType
     */
    void checkTextInput();

    /**
     * Automatically chooses between a number format and
     * a scientific format (if the number is too big)
     */
    void checkNumberFormat();

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
    void loadOasisValidation( const QString& validationName );

    /**
     * \ingroup OpenDocument
     */
    void loadOasisValidationCondition( QString &valExpression );

    /**
     * \ingroup OpenDocument
     */
    void saveOasisAnnotation( KoXmlWriter &xmlwriter );

    /**
     * \ingroup OpenDocument
     */
    void loadOasisConditional( KoXmlElement * style );

private:
    class Extra;
    class Private;
    Private * const d;
    // static const char* s_dataTypeToString[];


  /** handle the fact that a cell has been updated - calls cellUpdated()
  in the parent Sheet object */
  void valueChanged ();

  /**
   * Determines the text to be displayed.
   *
   * This depends on the following variables:
   * \li wether the value or the formula should be shown
   *
   * \see ValueFormatter::formatText
   */
  void setOutputText();


  /* helper functions to the load/save routines */
  bool loadCellData(const KoXmlElement &text, Paste::Operation op);
  bool saveCellResult( QDomDocument& doc, QDomElement& result,
                       QString str );
  int effAlignX();

  /**
   * \ingroup OpenDocument
   * \todo TODO Stefan: merge this into Oasis::decodeFormula
   */
  void checkForNamedAreas( QString & formula ) const;

  /**
   * \ingroup OpenDocument
   */
  void loadOasisValidationValue( const QStringList &listVal );
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Cell::StatusFlags)

} // namespace KSpread

#endif  // KSPREAD_CELL
