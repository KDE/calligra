/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>, Torben Weis <weis@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef format_h
#define format_h

#include <qstring.h>
#include <qcolor.h>

#include <qtextstream.h>
#include <koStream.h>

class KWFormat;
class KWordDocument;
class KWUserFont;
class KWDisplayFont;

/******************************************************************/
/* Class: KWFormat                                                */
/******************************************************************/

/**
 * This class is used to change the texts format within a paragraph.
 *
 * @see KWParagraph
 */
class KWFormat
{
public:
    enum VertAlign { VA_NORMAL, VA_SUB, VA_SUPER };
    enum Attributes {
        FontFamily = 1,
        FontSize = 2,
        Color = 4,
        Weight = 8,
        Italic = 16,
        Underline = 32,
        Vertalign = 64,
        All = FontFamily | FontSize | Color | Weight | Italic | Underline | Vertalign
    };
    /**
     * Creates a new KWFormat instance.
     *
     * @param _color The color to use for the text. If this color is not not valid
     *               ( find out with QColor::isValid ) then the color does not change.
     *               Pass <TT>QColor()</TT> if you dont want to change the color at all.
     * @param _font A pointer to the font family. if this pointer is 0L that means
     *              that the text does not change its current font family.
     * @param _font_size The size of the font to use or -1 if this value does not change.
     * @param _weight The fonts weight or -1 if the value should not change
     * @param _italic 1 to enable italic font, 0 to disable and -1 for no change.
     * @param _underline 1 to enable underline font, 0 to disable and -1 for no change.
     * @param _math 1 to enable math mode, 0 to disable and -1 for no change.
     * @param _direct 1 to enable direct mode, 0 to disable and -1 for no change.
     *
     * @see KWUserFont
     */
    KWFormat( KWordDocument *_doc, const QColor& _color, KWUserFont *_font = 0L, int _font_size = -1, int _weight = -1,
              char _italic = -1, char _underline = -1, VertAlign _vertAlign = VA_NORMAL );

    /**
     * Creates a new KWFormat instance. This instance has set all values to
     * 'dont change'.
     */
    KWFormat( KWordDocument *_doc );
    KWFormat() : userFont(0L), ptFontSize(-1), weight(-1), italic(-1), underline(-1),
        vertAlign(VA_NORMAL), ref(0), doc(0L) {}

    /**
     * Creates a new KWFormat instance.
     *
     * @param _format It copies its state from this KWFormat instance.
     */
    KWFormat( KWordDocument *_doc, const KWFormat& _format );

    virtual ~KWFormat() {}

    virtual KWFormat& operator=( const KWFormat& _format );

    virtual bool operator==( const KWFormat & _format ) const;
    virtual bool operator!=( const KWFormat & _format ) const;

    virtual void apply( const KWFormat &_format );

    /**
     * @return The color to use. The color may be not valid ( test with <TT>QColor::isValid()</TT>.
     *         In this case you shoud not use the color returned.
     */
    QColor getColor() const { return color; }

    /**
     * @return The font family to use. The return value may be 0L if the font should not change.
     */
    KWUserFont* getUserFont() const { return userFont; }

    KWDisplayFont* loadFont( KWordDocument *_doc );

    /**
     * @return The font size to use in points. The return value may be -1 if the size should not change.
     */
    int getPTFontSize() const { return ptFontSize; }

    /**
     * @return The fonts weight to use. The return value may be -1 if the weight should not change.
     */
    int getWeight() const { return weight; }

    /**
     * @return The italic mode. The return value may be -1 if the italic mode should not change.
     */
    char getItalic() const { return italic; }

    /**
     * @return The underline mode. The return value may be -1 if the underline mode should not change.
     */
    char getUnderline() const { return underline; }

    VertAlign getVertAlign() const { return vertAlign; }

    /**
     * Fills all values with defaults. No value will remain in the 'dont change' state.
     * You may want to use this function to get a default font & color.
     *
     * @param _doc is the document this format belongs to. The document provides information
     *             on the default font for example.
     *
     * @see KWordDocument
     */
    void setDefaults( KWordDocument *_doc );

    /**
     * Set the color to use.
     * @param _color The color to use for the text. If this color is not not valid
     *               ( find out with QColor::isValid ) then the color does not change.
     *               Pass <TT>QColor()</TT> if you dont want to change the color at all.
     *
     * @see $QColor
     */
    void setColor( const QColor& _c ) { color = _c; }

    /**
     * Sets the font to use.
     *
     * @param _font A pointer to the font family. if this pointer is 0L that means
     *              that the text does not change its current font family.
     *
     * @see KWUserFont
     */
    void setUserFont( KWUserFont* _font ) { userFont = _font; }

    /**
     * Sets the unzoomed font size to use.
     *
     * @param _font_size The size of the font to use or -1 if this value does not change.
     */
    void setPTFontSize( int _size ) { ptFontSize = _size; }

    /**
     * Sets the fonts weight to use.
     *
     * @param _weight The fonts weight or -1 if the value should not change
     */
    void setWeight( int _weight ) { weight = _weight; }

    /**
     * Sets the fonts italic mode.
     *
     * @param _italic 1 to enable italic font, 0 to disable and -1 for no change.
     */
    void setItalic( char _italic ) { italic = _italic; }

    /**
     * Sets the fonts underline mode.
     *
     * @param _underline 1 to enable underline font, 0 to disable and -1 for no change.
     */
    void setUnderline( char _underline ) { underline = _underline; }

    void setVertAlign( VertAlign _vertAlign ) { vertAlign = _vertAlign; }

    void incRef();
    void decRef();
    int refCount() { return ref; }

    KWordDocument *getDocument() { return doc; }

    void save( QTextStream&out );
    void load( KOMLParser&, QValueList<KOMLAttrib>&, KWordDocument* );

protected:
    /**
     * Pointer to the font we have to use. If this value is 0L we are told
     * not to change the current font family.
     * @see KWUserFont
     */
    KWUserFont *userFont;
    /**
     * The size of the font in points. If this is -1 then we are told not
     * to change the fonts size.
     */
    int ptFontSize;
    /**
     * The weight of the font. A value of -1 tells us not to change the fonts weight.
     */
    int weight;
    /**
     * Indicates wether to use italic or not. A value of 0 tells us 'no',
     * a value of 1 tells 'yes' and a value of -1 tells us 'dont change
     * the mode'.
     */
    char italic;
    /**
     * Indicates wether to use underline or not. A value of 0 tells us 'no',
     * a value of 1 tells 'yes' and a value of -1 tells us 'dont change
     * the mode'.
     */
    char underline;
    /**
     * The color to use for the text. If this color is not not valid
     * ( find out with QColor::isValid ) then the color does not change.
     */
    QColor color;

    VertAlign vertAlign;

    int ref;
    KWordDocument *doc;

private:
    // disabe normal copy constructor
    KWFormat( const KWFormat& _format );

};

#endif


