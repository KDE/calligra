/* Sidewinder - Portable library for spreadsheet
   Copyright (C) 2003-2006 Ariya Hidayat <ariya@kde.org>
   Copyright (C) 2006 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

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
 * Boston, MA 02110-1301, USA
*/

#ifndef SWINDER_FORMAT_H
#define SWINDER_FORMAT_H

#include "ustring.h"
#include <cstdio> // for sscanf

namespace Swinder
{

/**
 * @short Provides color based on RGB values.
 *
 * Class Color provides color based on  terms of RGB (red, green and blue)
 * components.
 *
 */
class Color
{
public:

    unsigned red, green, blue;

    /**
     * Constructs a default color with the RGB value (0, 0, 0), i.e black.
     */
    Color() {
        red = green = blue = 0;
    }

    /**
     * Creates a copy of another color.
     */
    Color(const Color& c) {
        red = c.red; green = c.green; blue = c.blue;
    }

    /**
     * Creates a color based on given red, green and blue values.
     */
    Color(unsigned r, unsigned g, unsigned b) {
        red = r; green = g; blue = b;
    }

    /**
     * Creates a color based on given red, green and blue values, encoded as \#RRGGBB in a string.
     */
    explicit Color(const char* c) {
        std::sscanf(c, "#%2x%2x%2x", &red, &green, &blue);
    }

    friend inline bool operator==(const Color&, const Color&);
    friend inline bool operator!=(const Color&, const Color&);
};

/**
    Returns true if c1 is equal to c2; otherwise returns false.
*/
inline bool operator==(const Color& c1, const Color& c2)
{
    return c1.red == c2.red && c1.green == c2.green && c1.blue == c2.blue;
}

/**
    Returns true if c1 is not equal to c2; otherwise returns false.
*/
inline bool operator!=(const Color& c1, const Color& c2)
{
    return c1.red != c2.red || c1.green != c2.green || c1.blue != c2.blue;
}

class Pen
{
public:
    unsigned style;
    float width;
    Color color;

    enum {
        NoLine,         // no line at all
        SolidLine,      // a simple solid line
        DashLine,       // dashes separated by a few pixels
        DotLine,        // dots separated by a few pixels
        DashDotLine,    // alternate dots and dashes
        DashDotDotLine, // one dash, two dots, one dash, two dots
        DoubleLine      // two simple solid lines
    };

    Pen(): style(SolidLine), width(0) {}

    friend inline bool operator==(const Pen&, const Pen&);
    friend inline bool operator!=(const Pen&, const Pen&);
};

/**
    Returns true if p1 is equal to p2; otherwise returns false.
*/
inline bool operator==(const Pen& p1, const Pen& p2)
{
    return p1.style == p2.style && p1.width == p1.width && p1.color == p2.color;
}

/**
    Returns true if p1 is not equal to p2; otherwise returns false.
*/
inline bool operator!=(const Pen& p1, const Pen& p2)
{
    return p1.style != p2.style || p1.width != p1.width || p1.color != p2.color;
}


/**
 * Defines font information for cell format.
 *
 * Class FormatFont defines the font family, size and other attributes
 * for use in cell format.
 *
 */

class FormatFont
{
public:

    /**
     * Creates a default font information.
     */
    FormatFont();

    /**
     * Destroys the font information
     */
    ~FormatFont();

    /**
     * Creates a copy of font information.
     */
    FormatFont(const FormatFont&);

    /**
     * Assigns from another font information.
     */
    FormatFont& operator=(const FormatFont&);

    /**
     * Assigns from another font information.
     */
    FormatFont& assign(const FormatFont&);

    /**
     * Returns true if it is a default font information.
     */
    bool isNull() const;

    /**
     * Returns the name of font family, e.g "Helvetica".
     */
    UString fontFamily() const;

    /**
     * Sets a new family for the font information.
     */
    void setFontFamily(const UString& fontFamily);

    /**
     * Returns the size of font (in points).
     */
    double fontSize() const;

    /**
     * Sets the size of font (in points).
     */
    void setFontSize(double fs);

    /**
     * Returns the color of the font.
     */
    Color color() const;

    /**
     * Sets the color of the font.
     */
    void setColor(const Color& color);

    /**
     * Returns true if bold has been set.
     */
    bool bold() const;

    /**
     * If b is true, bold is set on; otherwise bold is set off.
     */
    void setBold(bool b);

    /**
     * Returns true if italic has been set.
     */
    bool italic() const;

    /**
     * If i is true, italic is set on; otherwise italic is set off.
     */
    void setItalic(bool i);

    /**
     * Returns true if underline has been set.
     */
    bool underline() const;

    /**
     * If u is true, underline is set on; otherwise underline is set off.
     */
    void setUnderline(bool u);

    /**
     * Returns true if strikeout has been set.
     */
    bool strikeout() const;

    /**
     * If s is true, strikeout is set on; otherwise strikeout is set off.
     */
    void setStrikeout(bool s);

    /**
     * Returns true if subscript has been set.
     */
    bool subscript() const;

    /**
     * If s is true, subscript is set on; otherwise subscript is set off.
     */
    void setSubscript(bool s);

    /**
     * Returns true if superscript has been set.
     */
    bool superscript() const;

    /**
     * If s is true, superscript is set on; otherwise superscript is set off.
     */
    void setSuperscript(bool s);

    /**
     * Returns true if this font is equal to f; otherwise returns false.
     */
    bool operator==(const FormatFont& f) const;

    /**
     * Returns true if this font is not equal to f; otherwise returns false.
     */
    bool operator!=(const FormatFont& f) const;

private:
    class Private;
    Private *d;
};

/**
 * Defines alignment information for cell format.
 *
 * Class FormatAlignment defines the horizontal and vertical alignment
 * for the text inside a cell.
 *
 */

class FormatAlignment
{
public:

    /**
     * Creates a default alignment information.
     */
    FormatAlignment();

    /**
     * Destroys the alignment information
     */
    ~FormatAlignment();

    /**
     * Creates a copy of alignment information.
     */
    FormatAlignment(const FormatAlignment&);

    /**
     * Assigns from another alignment information.
     */
    FormatAlignment& operator=(const FormatAlignment&);

    /**
     * Assigns from another alignment information.
     */
    FormatAlignment& assign(const FormatAlignment&);

    /**
     * Returns true if it is a default alignment information.
     */
    bool isNull() const;

    /**
     * Returns horizontal alignment. Possible values are
     * Format::Left, Format::Right and Format::Center.
     *
     * \sa setAlignX
     */
    unsigned alignX() const;

    /**
     * Sets the horizontal alignment.
     *
     * \sa alignX
     */
    void setAlignX(unsigned xa);

    /**
     * Returns vertical alignment. Possible values are
     * Format::Top, Format::Middle and Format::Bottom.
     *
     * \sa setAlignY
     */
    unsigned alignY() const;

    /**
     * Sets the vertical alignment.
     *
     * \sa alignY
     */
    void setAlignY(unsigned xa);

    /**
     * Returns true if the text should be wrapped at right border.
     *
     * \sa setWrap
     */
    bool wrap() const;

    /**
     * Sets whether the text should be wrapped at right border.
     *
     * \sa setWrap
     */
    void setWrap(bool w);

    /**
     * Returns the indentation level.
     *
     * \sa setIndentLevel
     */
    unsigned indentLevel() const;

    /**
     * Sets the indentation level.
     *
     * \sa indentLevel
     */
    void setIndentLevel(unsigned i);

    /**
     * Returns the text rotation angle.
     *
     * \sa setRotationAngle
     */
    unsigned rotationAngle() const;

    /**
     * Sets the text rotation angle.
     *
     * \sa rotationAngle
     */
    void setRotationAngle(unsigned r);

    /**
     * Returns true if the font size should be decreased to make text fit its cell.
     *
     * \sa setShrinkToFit
     */
    bool shrinkToFit() const;

    /**
     * Sets whether the font size should be decreased to make text fit its cell.
     *
     * \sa shrinkToFit
     */
    void setShrinkToFit(bool b);

    /**
     * Returns true if the text should be shown in stacked letters.
     *
     * \sa setStackedLetters
     */
    bool stackedLetters() const;

    /**
     * Sets whether the text should be shown in stacked letters.
     *
     * \sa stackedLetters
     */
    void setStackedLetters(bool b);

    /**
     * Returns true if this alignment is equal to f; otherwise returns false.
     */
    bool operator==(const FormatAlignment& f) const;

    /**
     * Returns true if this alignment is not equal to f; otherwise returns false.
     */
    bool operator!=(const FormatAlignment& f) const;

private:
    class Private;
    Private *d;
};

/**
 * Defines background information for cell.
 *
 */
class FormatBackground
{
public:
    /**
     * Creates a default background information.
     */
    FormatBackground();

    /**
     * Destroys the background information.
     */
    ~FormatBackground();

    /**
     * Creates a copy of background information.
     */
    FormatBackground(const FormatBackground&);

    /**
     * Assigns from another background information.
     */
    FormatBackground& operator=(const FormatBackground&);

    /**
     * Assigns from another background information.
     */
    FormatBackground& assign(const FormatBackground&);

    /**
     * Returns true if it is a default background information.
     */
    bool isNull() const;

    enum FillPattern {
        SolidPattern, ///< solid
        Dense1Pattern, ///< diagonal crosshatch
        Dense2Pattern, ///< thick diagonal crosshatch
        Dense3Pattern, ///< 75% gray
        Dense4Pattern, ///< 50% gray
        Dense5Pattern, ///< 25% gray
        Dense6Pattern, ///< 12.5% gray
        Dense7Pattern, ///< 6.25% gray
        HorPattern,  ///< Horizonatal lines
        VerPattern,  ///< Vertical lines
        CrossPattern, ///< Horizontal and Vertical lines
        BDiagPattern, ///< Left-bottom to right-top diagonal lines
        FDiagPattern, ///< Left-top to right-bottom diagonal lines
        DiagCrossPattern, ///< Crossing diagonal lines
        EmptyPattern ///< Transparent
    };

    /**
     * Returns pattern for this background.
     *
     * \sa setPattern
     */
    unsigned pattern() const;

    /**
     * Set the pattern for this background.
     *
     * \sa pattern
     */
    void setPattern(unsigned);

    /**
     * Returns the background color of the background area.
     *
     * \sa setBackgroundColor
     */
    Color backgroundColor() const;

    /**
     * Set the background color.
     *
     * \sa backgroundColor
     */
    void setBackgroundColor(const Color&);

    /**
     * Returns the foreground color of the background area.
     *
     * \sa setForegroundColor
     */
    Color foregroundColor() const;

    /**
     * Sets the foreground color.
     *
     * \sa foregroundColor
     */
    void setForegroundColor(const Color&);

    /**
     * Returns true if this background is equal to f; otherwise returns false.
     */
    bool operator==(const FormatBackground& f) const;

    /**
     * Returns true if this background is not equal to f; otherwise returns false.
     */
    bool operator!=(const FormatBackground& f) const;

private:
    class Private;
    Private *d;
};

/**
 * Defines borders information for cell.
 *
 */

class FormatBorders
{
public:

    /**
     * Creates a default border information.
     */
    FormatBorders();

    /**
     * Destroys the border information
     */
    ~FormatBorders();

    /**
     * Creates a copy of border information.
     */
    FormatBorders(const FormatBorders&);

    /**
     * Assigns from another border information.
     */
    FormatBorders& operator=(const FormatBorders&);

    /**
     * Assigns from another border information.
     */
    FormatBorders& assign(const FormatBorders&);

    /**
     * Returns true if it is a default border information.
     */
    bool isNull() const;

    /**
     * Returns pen style, width and color for left border.
     *
     * \sa setLeftBorder
     */
    const Pen& leftBorder() const;

    /**
     * Sets pen style, width and color for left border.
     *
     * \sa leftBorder
     */
    void setLeftBorder(const Pen& pen);

    /**
     * Returns pen style, width and color for right border.
     *
     * \sa setRightBorder
     */
    const Pen& rightBorder() const;

    /**
     * Sets pen style, width and color for right border.
     *
     * \sa rightBorder
     */
    void setRightBorder(const Pen& pen);

    /**
     * Returns pen style, width and color for top border.
     *
     * \sa setTopBorder
     */
    const Pen& topBorder() const;

    /**
     * Sets pen style, width and color for top border.
     *
     * \sa topBorder
     */
    void setTopBorder(const Pen& pen);

    /**
     * Returns pen style, width and color for bottom border.
     *
     * \sa setBottomBorder
     */
    const Pen& bottomBorder() const;

    /**
     * Sets pen style, width and color for bottom border.
     *
     * \sa bottomBorder
     */
    void setBottomBorder(const Pen& pen);

    // diagonal borders
    const Pen& topLeftBorder() const;
    void setTopLeftBorder(const Pen& pen);
    const Pen& bottomLeftBorder() const;
    void setBottomLeftBorder(const Pen& pen);

    /**
     * Returns true if this background is equal to f; otherwise returns false.
     */
    bool operator==(const FormatBorders& f) const;

    /**
     * Returns true if this background is not equal to f; otherwise returns false.
     */
    bool operator!=(const FormatBorders& f) const;

private:
    class Private;
    Private *d;
};

/**
 * Defines format of cell.
 *
 * Class Format defines possible formatting for use in cells or ranges.
 * Basically, Format might consist of one or more "pieces". Each piece
 * specifies only one type of formatting, e.g whether the text should
 * be shown in bold or not, which borders should the cells/ranges have,
 * and so on.
 *
 * A complex formatting can be decomposed into different pieces. For example,
 * formatting like "Font is Arial 10 pt, background color is blue,
 " formula is hidden" could be a combination of three simple formatting pieces
 * as: (1) font is "Arial 10pt", (2) background pattern is 100%, blue
 * and (3) cell is protected, formula is hidden. This also means
 * that one format might be applied to another format. An example of this is
 * "Font is Helvetica" format and "Left border, 1pt, blue" format will yields
 * something like "Font is Helvetica, with left border of blue 1pt".
 * Use Format::apply to do such format merging.
 *
 */


class Format
{
public:

    /**
     * Creates a default format.
     */
    Format();

    /**
     * Destroys the format.
     */
    ~Format();

    /**
     * Creates a copy from another format.
     */
    Format(const Format& f);

    /**
     * Assigns from another format.
     */
    Format& operator= (const Format& f);

    /**
     * Assigns from another value.
     */
    Format& assign(const Format& f);

    /**
     * Returns true if it is a default format information.
     */
    bool isNull() const;

    /**
     * Returns a constant reference to the formatting information of this format.
     */
    FormatFont& font() const;

    /**
     * Sets new font information for this format.
     */
    void setFont(const FormatFont& font);

    /**
     * Returns a constant reference to the alignment information of this format.
     */
    FormatAlignment& alignment() const;

    /**
     * Sets new alignment information for this format.
     */
    void setAlignment(const FormatAlignment& alignment);

    /**
     * Returns a reference to the borders information of this format.
     */
    FormatBorders& borders() const;

    /**
     * Sets new borders information for this format.
     */
    void setBorders(const FormatBorders& border);

    /**
     * Retursn a reference to the background information of this format.
     */
    FormatBackground& background() const;

    /**
     * Sets new background information for this format.
     */
    void setBackground(const FormatBackground&);

    /**
     * Returns the formatting string to display the value of this format.
     */
    UString valueFormat() const;

    /**
     * Sets the new formatting string to display the value of this format.
     */
    void setValueFormat(const UString& valueFormat);

    enum { Standard = -1, Left, Center, Right, Justify, Distributed };

    enum { Top, Middle, Bottom, VJustify, VDistributed };

    /**
     * Applies another format to this format. Basically this will merge
     * the formatting information of f into the current format.
     * For example, if current format is "Bold, Italic" and f is
     * "Left border", the current format would become "Bold, Italic, left border".
     *
     * If parts of the formatting information in f are already specified in the
     * current format, then it will override the current format.
     * For example, if current format is "Bold, right-aligned" and f is "Italic",
     * the result is "Italic, right-aligned".
     *
     */
    Format& apply(const Format& f);

    /**
     * Returns true if this format is equal to f; otherwise returns false.
     */
    bool operator==(const Format& f) const;

    /**
     * Returns true if this format is not equal to f; otherwise returns false.
     */
    bool operator!=(const Format& f) const;

private:
    class Private;
    Private* d; // can't never be 0
};

}

#endif // SWINDER_FORMAT_H

