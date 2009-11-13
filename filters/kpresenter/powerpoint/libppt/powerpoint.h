/* libppt - library to read PowerPoint presentation
   Copyright (C) 2005 Yolla Indria <yolla.indria@gmail.com>

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

#ifndef LIBPPT_POWERPOINT
#define LIBPPT_POWERPOINT

#include <iostream>
#include "ustring.h"
#include <QtCore/QSharedDataPointer>
#include <QtGui/QColor>
#include "slide.h"

namespace Libppt
{

// forward declaration
class Presentation;

class Record
{
public:

    /**
      Static ID of the record. Subclasses should override this value
      with the id of the record they handle.
    */
    static const unsigned int id;

    virtual unsigned int rtti() {
        return this->id;
    }

    /**
      Creates a new generic record.
    */
    Record();

    /**
      Destroys the record.
    */
    virtual ~Record();

    /**
     * Record factory, create a new record of specified type.
     */
    static Record* create(unsigned type);

    /**
     * Returns true if this is a Container.
     */
    virtual bool isContainer() const {
        return false;
    }

    void setParent(Record* parent);

    const Record* parent() const;

    /**
      Sets the data for this record.
     */
    virtual void setData(unsigned size, const unsigned char* data);

    /**
      Sets the position of the record in the OLE stream.
     */
    void setPosition(unsigned pos);

    /**
      Gets the position of this record in the OLE stream.
     */
    unsigned position() const;

    void setInstance(unsigned inst);
    unsigned instance() const;

    /**
      Returns the name of the record. For debugging only.
     */
    virtual const char* name() const {
        return "Unknown";
    }

    /**
      Dumps record information to output stream. For debugging only.
     */
    virtual void dump(std::ostream& out) const;

protected:

    // position of this record in the OLE stream
    unsigned stream_position;

    unsigned record_instance;

    Record* record_parent;

private:
    // no copy or assign
    Record(const Record&);
    Record& operator=(const Record&);
};

// Container is a special record, it is for holding another records
class Container: public Record
{
public:
    Container();
    virtual bool isContainer() const {
        return true;
    }
    const char* name() const {
        return "Container";
    }

private:
    // no copy or assign
    Container(const Container&);
    Container& operator=(const Container&);
};


class BookmarkCollectionContainer: public Container
{
public:
    static const unsigned int id;
    BookmarkCollectionContainer();
    const char* name() const {
        return "BookmarkCollectionContainer";
    }

private:
    // no copy or assign
    BookmarkCollectionContainer(const BookmarkCollectionContainer&);
    BookmarkCollectionContainer& operator=(const BookmarkCollectionContainer&);
};

class DocumentContainer: public Container
{
public:
    static const unsigned int id;
    DocumentContainer();
    const char* name() const {
        return "DocumentContainer";
    }

private:
    // no copy or assign
    DocumentContainer(const DocumentContainer&);
    DocumentContainer& operator=(const DocumentContainer&);
};

class DocumentTextInfoContainer: public Container
{
public:
    static const unsigned int id;
    DocumentTextInfoContainer();
    const char* name() const {
        return "DocumentTextInfoContainer";
    }

private:
    // no copy or assign
    DocumentTextInfoContainer(const DocumentTextInfoContainer&);
    DocumentTextInfoContainer& operator=(const DocumentTextInfoContainer&);
};

class ExObjListContainer: public Container
{
public:
    static const unsigned int id;
    ExObjListContainer();
    const char* name() const {
        return "ExObjListContainer";
    }

private:
    // no copy or assign
    ExObjListContainer(const ExObjListContainer&);
    ExObjListContainer& operator=(const ExObjListContainer&);
};

class ExHyperlinkContainer : public Container
{
public:
    static const unsigned int id;
    ExHyperlinkContainer();
    const char* name() const {
        return "ExHyperlinkContainer ";
    }

private:
    // no copy or assign
    ExHyperlinkContainer(const ExHyperlinkContainer &);
    ExHyperlinkContainer & operator=(const ExHyperlinkContainer &);
};

class ExEmbedContainer : public Container
{
public:
    static const unsigned int id;
    ExEmbedContainer();
    const char* name() const {
        return "ExEmbedContainer ";
    }

private:
    // no copy or assign
    ExEmbedContainer(const ExEmbedContainer &);
    ExEmbedContainer & operator=(const ExEmbedContainer &);
};

class ExLinkContainer : public Container
{
public:
    static const unsigned int id;
    ExLinkContainer();
    const char* name() const {
        return "ExLinkContainer ";
    }

private:
    // no copy or assign
    ExLinkContainer(const ExLinkContainer &);
    ExLinkContainer & operator=(const ExLinkContainer &);
};

class RunArrayContainer: public Container
{
public:
    static const unsigned int id;
    RunArrayContainer();

    const char* name() const {
        return "RunArrayContainer";
    }

private:
    // no copy or assign
    RunArrayContainer(const RunArrayContainer&);
    RunArrayContainer& operator=(const RunArrayContainer&);
};

class ExOleObjStgContainer: public Container
{
public:
    static const unsigned int id;
    ExOleObjStgContainer();
    const char* name() const {
        return "ExOleObjStgContainer";
    }

private:
    // no copy or assign
    ExOleObjStgContainer(const ExOleObjStgContainer&);
    ExOleObjStgContainer& operator=(const ExOleObjStgContainer&);
};

class FontCollectionContainer: public Container
{
public:
    static const unsigned int id;
    FontCollectionContainer();
    const char* name() const {
        return "FontCollectionContainer";
    }

private:
    // no copy or assign
    FontCollectionContainer(const FontCollectionContainer&);
    FontCollectionContainer& operator=(const FontCollectionContainer&);
};

class HandoutContainer: public Container
{
public:
    static const unsigned int id;
    HandoutContainer();
    const char* name() const {
        return "HandoutContainer";
    }

private:
    // no copy or assign
    HandoutContainer(const HandoutContainer&);
    HandoutContainer& operator=(const HandoutContainer&);
};

class HeadersFootersContainer: public Container
{
public:
    static const unsigned int id;
    HeadersFootersContainer();
    const char* name() const {
        return "HeadersFootersContainer";
    }

private:
    // no copy or assign
    HeadersFootersContainer(const DocumentContainer&);
    HeadersFootersContainer& operator=(const DocumentContainer&);
};

class ListContainer: public Container
{
public:
    static const unsigned int id;
    ListContainer();
    const char* name() const {
        return "ListContainer";
    }

private:
    // no copy or assign
    ListContainer(const ListContainer&);
    ListContainer& operator=(const ListContainer&);
};


/**
* @brief A structure that specifies a color in the sRGB color space as specified
* in [IEC-RGB].
*
*/
class ColorStruct
{
public:
    ColorStruct();
    ColorStruct(const ColorStruct &other);
    ~ColorStruct();

    /**
    * @brief Parse data for this object
    * @param data Data to parse
    */
    void setData(const unsigned char *data);

    /**
    * @brief Return this ColorStruct as QColor
    * @return Color as QColor
    */
    QColor color();
private:
    class Private;
    QSharedDataPointer<Private> d;
};

/**
* A structure that specifies an index in the color scheme, or a color in the
* sRGB color space as specified in [IEC-RGB]. Color schemes are specified by the
* SlideSchemeColorSchemeAtom record.
*/
class ColorIndexStruct
{
public:
    ColorIndexStruct();
    ColorIndexStruct(const ColorIndexStruct &color);
    ~ColorIndexStruct();

    /**
    * @brief Parse data for this struct
    * @param data pointer to data to parse values from, array must be at least 4
    * bytes long
    */
    void setData(const unsigned char *data);

    /**
    * @brief Get red value
    *
    */
    unsigned int red() const;

    /**
    * @brief Get green value
    *
    */
    unsigned int green() const;

    /**
    * @brief Get blue value
    *
    */
    unsigned int blue() const;

    /**
    * @brief An unsigned integer that specifies the index in the color scheme.
    * It MUST be a value from the following table:
    * 0x00 Background color
    * 0x01 Text color
    * 0x02 Shadow color
    * 0x03 Title text color
    * 0x04 Fill color
    * 0x05 Accent 1 color
    * 0x06 Accent 2 color
    * 0x07 Accent 3 color
    * 0xFE Color is an sRGB value specified by red, green, and blue fields.
    * 0xFF Color is undefined.
    */
    unsigned int index() const;

private:
    class Private;
    Private* d;
};

/**
* @brief A structure that specifies character-level style and formatting, font
* information, coloring and positioning.
*/
class TextCFException
{
public:
    TextCFException(const TextCFException &exception);
    TextCFException();
    ~TextCFException();


    void dump(std::ostream& out) const;

    /**
    * @brief Parse data for this class
    * @param size size of the data to read
    * @param data Data to read
    * @return number of bytes read
    */
    unsigned int setData(unsigned int size, const unsigned char *data);

    /**
    * @brief does this class contain font definition
    * @return true if this class contains font definition
    */
    bool hasFont();

    /**
    * @brief does this class contain font size definition
    * @return true if this class contains font size definition
    */
    bool hasFontSize();

    /**
    * @brief does this class contain color definition
    * @return true if this class contains color definition
    */
    bool hasColor();

    /**
    * @brief does this class contain italic definition
    * @return true if this class contains italic definition
    */
    bool hasItalic();

    /**
    * @brief does this class contain bold definition
    * @return true if this class contains bold definition
    */
    bool hasBold();

    /**
    * @brief does this class contain underline definition
    * @return true if this class contains underline definition
    */
    bool hasUnderline();

    /**
    * @brief does this class contain position definition
    * @return true if this class contains position definition
    */
    bool hasPosition();

    /**
    * @brief does this class contain shadow definition
    * @return true if this class contains shadow definition
    */
    bool hasShadow();

    /**
    * @brief does this class contain emboss definition
    * @return true if this class contains emboss definition
    */
    bool hasEmboss();

    /**
    * @brief Is the character italic
    *
    * NOTE: only valid if hasItalic() is true
    * @return true if the character is italic
    */
    bool italic();

    /**
    * @brief Is the character text bold
    *
    * NOTE: only valid if hasBold() is true
    * @return true if the character is bold
    */
    bool bold();

    /**
    * @brief Is the character text underlined
    *
    * NOTE: only valid if hasUnderline() is true
    * @return true if the character is underlined
    */
    bool underline();

    /**
    * @brief Is the character text embossed
    *
    * NOTE: only valid if hasEmboss() is true
    * @return true if the character is embossed
    */
    bool emboss();
    /**
    * @brief Does the character text have a shadow
    *
    * NOTE: only valid if hasShadow() is true
    * @return true if the character has a shadow
    */
    bool shadow();

    /**
    * @brief Get position of the text
    *
    * NOTE: only valid if hasPosition() is true
    * @return position of the text
    */
    int position();

    /**
    * @brief Get font reference for the character
    *
    * NOTE: only valid if hasFont() is true
    * @return font reference for the character
    */
    unsigned int fontRef();

    /**
    * @brief Get font size for the character
    *
    * NOTE: only valid if hasFontSize() is true
    * @return font size for the character
    */
    int fontSize();

    /**
    * @brief Get color for the character
    *
    * NOTE: only valid if hasColor is true
    * @return color for the character
    */
    ColorIndexStruct color();

    /**
    * @brief An unsigned integer that specifies the run grouping of additional
    * text properties in StyleTextProp9Atom record.
    *
    */
    unsigned int pp9rt();
private:
    class Private;
    QSharedDataPointer<Private> d;

};

/**
* @brief A structure that specifies paragraph-level formatting.
*
*/
class TextPFException
{

public:
    TextPFException();
    TextPFException(const TextPFException &exception);
    ~TextPFException();

    /**
    * @brief Parse TextPFException from given data
    * @param size size of data in bytes
    * @param data Data to parse from
    * @return number of bytes read
    */
    unsigned int setData(unsigned int size, const unsigned char *data);

    /**
    * @brief Does this paragraph have a bullet
    * @return true if this paragraph has a bullet
    */
    bool hasBullet();

    /**
    * @brief Does this exception contain a font definition for the bullet
    * @return true if this exception contains a font definition for the bullet
    */
    bool hasBulletFont();

    /**
    * @brief Does this exception need a font definition for the bullet
    * @return true if this exception need a font definition for the bullet
    */
    bool needsBulletFont();

    /**
    * @brief Does this exception contain a font definition for bullet color
    * @return true if this exception contains a font definition for bullet color
    */
    bool hasBulletColor();

    /**
    * @brief Does this exception need a font definition for bullet color
    * @return true if this exception need a font definition for bullet color
    */
    bool needsBulletColor();

    /**
    * @brief A bit that specifies whether the bulletChar field of the
    * TextPFException structure that contains this PFMasks exists.
    * @return true if this exception contains a bullet char definition
    */
    bool hasBulletChar();

    /**
    * @brief A bit that specifies whether the bulletSize field of the
    * TextPFException structure that contains this PFMasks exists.
    * @return true if this exception contains a bullet size definition
    */
    bool hasBulletSize();

    /**
    * @brief A bit that specifies whether the bulletChar field of the
    * TextPFException structure that contains this PFMasks exists.
    * @return true if this exception contains a left margin definition
    */
    bool hasLeftMargin();

    /**
    * @brief A bit that specifies whether the spaceBefore field of the
    * TextPFException that contains this PFMasks exists.
    * @return true if this exception contains a space before definition
    */
    bool hasSpaceBefore();

    /**
    * @brief A bit that specifies whether the spaceAfter field of the
    * TextPFException structure that contains this PFMasks exists.
    * @return true if this exception contains a space after definition
    */
    bool hasSpaceAfter();

    /**
    * @brief A bit that specifies whether the indent field of the TextPFException
    * structure that contains this PFMasks exists.
    * @return true if this exception contains indentation definition
    */
    bool hasIndent();

    /**
    * @brief A bit that specifies whether the textAlignment field of the
    * TextPFException structure that contains this PFMasks exists.
    * @return true if this exception contains alignment definition
    */
    bool hasAlign();

    /**
    * An optional FontIndexRef that specifies the font to use for the bullet.
    * It MUST exist if and only if masks.bulletFont is TRUE. This field is valid
    * if and only if bulletFlags.fBulletHasFont is TRUE.
    *
    */
    unsigned int bulletFontRef();

    /**
    * An optional ColorIndexStruct structure that specifies the color of a bullet.
    * This field exists if and only if masks.bulletColor is TRUE. This field is
    * valid if and only if bulletFlags.fBulletHasColor is TRUE.
    *
    */
    ColorIndexStruct bulletColor();

    /**
    * A bit that specifies whether the bulletFlags field of the TextPFException
    * structure that contains this PFMasks exists and whether
    * bulletFlags.fHasBullet is valid.
    *
    */
    bool bullet();

    /**
    * An optional BulletSize that specifies the size of the bullet. It MUST exist
    * if and only if masks.bulletSize is TRUE. This field is valid if and only if
    * bulletFlags.fBulletHasSize is TRUE.
    *
    */
    int bulletSize();

    /**
    * An optional signed integer that specifies a UTF-16 Unicode [RFC2781]
    * character to display as the bullet. The character MUST NOT be the NUL
    * character 0x0000. It MUST exist if and only if masks.bulletChar is TRUE.
    *
    */
    QChar bulletChar();

    /**
    * An optional MarginOrIndent that specifies the left margin of the
    * paragraph. It MUST exist if and only if masks.leftMargin is TRUE.
    *
    */
    int leftMargin();

    /**
    * An optional ParaSpacing that specifies the size of the spacing before
    * the paragraph. It MUST exist if and only if masks.spaceBefore is TRUE.
    *
    */
    int spaceBefore();

    /**
    * An optional ParaSpacing that specifies the size of the spacing after the
    * paragraph. It MUST exist if and only if masks.spaceAfter is TRUE.
    *
    */
    int spaceAfter();

    /**
    * An optional MarginOrIndent that specifies the indentation of the paragraph.
    * It MUST exist if and only if masks.indent is TRUE.
    *
    */
    int indent();

    /**
    * An optional TextAlignmentEnum enumeration that specifies the
    * alignment of the paragraph. It MUST exist if and only if masks.align is
    * TRUE.
    *
    */
    unsigned int textAlignment();

    void dump(std::ostream& out) const;

private:
    class Private;
    QSharedDataPointer<Private> d;

};

/**
* @brief A structure that specifies character-level and paragraph
* level-formatting for a style level.
*/
class TextMasterStyleLevel
{
public:
    TextMasterStyleLevel();
    TextMasterStyleLevel(const TextMasterStyleLevel &level);
    ~TextMasterStyleLevel();

    /**
    * @param parse data for this class
    * @param size size of the data to parse
    * @param data Data to parse
    * @return number of bytes read
    */
    unsigned int setData(unsigned int size, const unsigned char *data);

    /**
    * @brief set level for this
    */
    void setLevel(unsigned int value);

    /**
    * @brief An optional unsigned integer that specifies to what style level this
    * TextMasterStyleLevel applies.
    */
    unsigned int level();

    /**
    * @brief get text paragraph exception for this level
    * @return text paragraph exception
    */
    TextPFException *pf();

    /**
    * @brief get text character exception for this level
    * @return text character exception
    */
    TextCFException *cf();

    void dump(std::ostream& out) const;
private:
    class Private;
    QSharedDataPointer<Private> d;


};


/**
* An atom record that specifies the character-level and paragraph-level
* formatting of a main master slide. If this TextMasterStyleAtom is contained in
* a MainMasterContainer record, character-level and paragraph-level formatting
* not specified by this TextMasterStyleAtom record inherit from the
* TextMasterStyleAtom record contained in the DocumentTextInfoContainer record.
*
*/
class TextMasterStyleAtom  : public Record
{
public:
    static const unsigned int id;
    TextMasterStyleAtom();
    TextMasterStyleAtom(const TextMasterStyleAtom &other);
    ~TextMasterStyleAtom();

    void setTextType(unsigned int type);
    int textType();

    const char* name() const {
        return "TextMasterStyleAtom  ";
    }
    void dump(std::ostream& out) const;

    /**
    * @brief Parse data for this class
    * @param size size of the given data
    * @param data data to parse
    */
    void setDataWithInstance(const unsigned int size,
                             const unsigned char* data,
                             unsigned int recInstance);

    /**
    * @brief get the amount of levels within this atom
    * @return amount of levels
    */
    unsigned int levelCount();

    /**
    * @brief get specific level within this atom
    * @param index index of the level
    * @return specified TextMasterStyleLevel or 0 if none found
    */
    TextMasterStyleLevel *level(int index);
private:
    // no copy or assign
    TextMasterStyleAtom  & operator=(const TextMasterStyleAtom  &);
    //TextMasterStyleAtom(const TextMasterStyleAtom  &other);

    class Private;
    QSharedDataPointer<Private> d;
};

/**
* @brief A structure that specifies additional text properties
*
*/
class TextSIException
{
public:
    TextSIException();
    unsigned int setData(unsigned int size, const unsigned char* data);
    void dump(std::ostream& out) const;
private:
    class Private;
    QSharedDataPointer<Private> d;
};

/**
* @brief A structure that specifies additional character-level formatting.
*
*/
class TextCFException9
{
public:
    TextCFException9();
    unsigned int setData(unsigned int size, const unsigned char* data);
    void dump(std::ostream& out) const;
private:
    class Private;
    QSharedDataPointer<Private> d;
};

/**
* @brief A structure that specifies additional paragraph-level formatting.
*
*/
class TextPFException9
{
public:
    TextPFException9();
    unsigned int setData(unsigned int size, const unsigned char* data);
    void dump(std::ostream& out) const;
    bool bulletBlip();
    bool bulletHasScheme();
    bool bulletScheme();

    int bulletBlipRef();
    int fBulletHasAutoNumber();
    unsigned int scheme();
    int startNum();
private:
    class Private;
    QSharedDataPointer<Private> d;
};

/**
* @brief A structure that specifies additional paragraph-level formatting,
* character-level formatting, and text properties for a text run.
*
*/
class StyleTextProp9
{
public:
    StyleTextProp9();
    unsigned int setData(unsigned int size, const unsigned char* data);
    void dump(std::ostream& out) const;
    TextPFException9 *pf9();
    TextCFException9 *cf9();
    TextSIException *si();
private:
    class Private;
    QSharedDataPointer<Private> d;
};


/**
* @brief An atom record that specifies additional text formatting.
*
* When this record is contained in an OutlineTextProps9Entry structure, let the
* corresponding text be as specified in the OutlineTextPropsHeaderExAtom record
* contained in the OutlineTextProps9Entry structure that contains this
* StyleTextProp9Atom record.
*
* When this record is contained in a
* PP9ShapeBinaryTagExtension record, let the corresponding text be specified by
* the TextHeaderAtom record contained in the OfficeArtSpContainer ([MS-ODRAW]
* section 2.2.14) that contains this StyleTextProp9Atom record.
*
* Let the corresponding shape be as specified in the corresponding text.
*
* Let the corresponding main master be as specified in the corresponding text.
*
* If the corresponding shape is a placeholder shape, character-level and
* paragraph-level formatting not specified by this StyleTextProp9Atom record
* inherit from the TextMasterStyle9Atom records contained in the corresponding
* main master.
*/
class StyleTextProp9Atom
{
public:
    StyleTextProp9Atom();
    StyleTextProp9Atom(const StyleTextProp9Atom &atom);
    ~StyleTextProp9Atom();
    unsigned int setData(unsigned int size, const unsigned char* data);
    void dump(std::ostream& out) const;
    int styleTextProp9Count();
    StyleTextProp9 *styleTextProp9(unsigned int index);
private:
    class Private;
    QSharedDataPointer<Private> d;
};

/**
* @brief An atom record that specifies a reference to text contained in the
* SlideListWithTextContainer record.
*
* Let the corresponding slide persist be specified by the SlidePersistAtom
* record contained in the SlideListWithTextContainer record whose slideId field
* is equal to slideIdRef. Let the corresponding text be specified by the
* TextHeaderAtom record referenced by rh.recInstance.
*
*
*/
class OutlineTextPropsHeaderExAtom
{
public:
    OutlineTextPropsHeaderExAtom();
    unsigned int setData(unsigned int size,
                         const unsigned char* data);
    void dump(std::ostream& out) const;
    unsigned int instance();
    unsigned int slideIdRef();
    unsigned int txType();
private:
    class Private;
    QSharedDataPointer<Private> d;
};

/**
* @brief A structure that specifies additional text properties for a single
* placeholder shape position on a slide.
*
*/
class OutlineTextProps9Entry
{
public:
    OutlineTextProps9Entry();
    unsigned int setData(unsigned int size, const unsigned char* data);
    void dump(std::ostream& out) const;
    OutlineTextPropsHeaderExAtom * outlineTextHeaderAtom();
    StyleTextProp9Atom *styleTextProp9Atom();
private:
    class Private;
    QSharedDataPointer<Private> d;
};


/**
* @brief A container record that specifies additional text properties for
* outline text.
*
*/
class OutlineTextProps9Container
{
public:
    OutlineTextProps9Container();
    unsigned int setData(unsigned int size, const unsigned char* data);
    void dump(std::ostream& out) const;
    int OutlineTextProps9EntryCount();
    OutlineTextProps9Entry *entry(unsigned int index);
    StyleTextProp9Atom *styleTextProp9Atom(unsigned int slideId,
                                           unsigned int textIndex);
private:
    class Private;
    QSharedDataPointer<Private> d;
};



/**
* @brief Specifies a SlideSchemeColorSchemeAtom or
* SchemeListElementColorSchemeAtom.
*
* SlideSchemeColorSchemeAtom:
* A container record that specifies the color scheme used by a slide.
*
* SchemeListElementColorSchemeAtom:
* A container record that specifies a color scheme in a list of available color
* schemes.
*/
class ColorSchemeAtom : public Record
{
public:
    static const unsigned int id;
    ColorSchemeAtom();
    ~ColorSchemeAtom();

    int background() const;
    void setBackground(int background);
    int textAndLines() const;
    void setTextAndLines(int textAndLines);
    int shadows() const;
    void setShadows(int shadows);
    int titleText() const;
    void setTitleText(int titleText);
    int fills() const;
    void setFills(int fills);
    int accent() const;
    void setAccent(int accent);
    int accentAndHyperlink() const;
    void setAccentAndHyperlink(int accentAndHyperlink);
    int accentAndFollowedHyperlink() const;
    void setAccentAndFollowedHyperlink(int accentAndFollowedHyperlink);
    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "ColorSchemeAtom ";
    }
    void dump(std::ostream& out) const;

    /**
    * @brief Get color as QColor with
    *
    */
    QColor getColor(unsigned int index);
private:
    QColor intToQColor(unsigned int value);
    // no copy or assign

    ColorSchemeAtom & operator=(const ColorSchemeAtom &);
    ColorSchemeAtom(const ColorSchemeAtom &);

    class Private;
    Private *d;
};

/**
* @brief A container record that specifies a main master slide.
*/
class MainMasterContainer: public Container
{
public:
    static const unsigned int id;
    MainMasterContainer();
    ~MainMasterContainer();
    const char* name() const {
        return "MainMasterContainer";
    }
    /**
    * @brief add a new ColorSchemeAtom
    * @param color new atom to add
    */
    void addSchemeListElementColorScheme(ColorSchemeAtom *color);

    /**
    * @brief add new text master style
    * @param textMasterStyleAtom text master style to add
    */
    void addTextMasterStyle(TextMasterStyleAtom *textMasterStyleAtom);

    /**
    * @brief Get specified TextMasterStyleAtom
    * @param index index of the atom to get
    * @return specified TextMasterStyleAtom or 0 if none found
    */
    TextMasterStyleAtom *textMasterStyleAtom(int index);

    /**
    * @brief Get TextMasterStyleAtom for specified text type
    * @param type type of text to find TextMasterStyleAtom for. See TextTypeEnum
    * in [MS-PPT].pdf
    * @return specified TextMasterStyleAtom or 0 if none found
    */
    TextMasterStyleAtom *textMasterStyleAtomForTextType(int type);

    /**
    * @brief Get pointer to slideSchemeColorSchemeAtom
    * @return pointer to slideSchemeColorSchemeAtom
    */
    ColorSchemeAtom *getSlideSchemeColorSchemeAtom();

    /**
    * @brief Get the amount of text master styles we have
    *
    */
    unsigned int textMasterStyleCount();
private:
    class Private;
    Private* d;
    // no copy or assign
    MainMasterContainer(const MainMasterContainer&);
    MainMasterContainer& operator=(const MainMasterContainer&);
};

class NotesContainer: public Container
{
public:
    static const unsigned int id;
    NotesContainer();
    const char* name() const {
        return "NotesContainer";
    }

private:
    // no copy or assign
    NotesContainer(const NotesContainer&);
    NotesContainer& operator=(const NotesContainer&);
};

class OutlineViewInfoContainer : public Container
{
public:
    static const unsigned int id;
    OutlineViewInfoContainer();
    const char* name() const {
        return "OutlineViewInfoContainer ";
    }

private:
    // no copy or assign
    OutlineViewInfoContainer(const OutlineViewInfoContainer &);
    OutlineViewInfoContainer & operator=(const OutlineViewInfoContainer &);
};

class PPDrawingContainer : public Container
{
public:
    static const unsigned int id;
    PPDrawingContainer();
    const char* name() const {
        return "PPDrawingContainer ";
    }

private:
    // no copy or assign
    PPDrawingContainer(const PPDrawingContainer &);
    PPDrawingContainer & operator=(const PPDrawingContainer &);
};

class PPDrawingGroupContainer : public Container
{
public:
    static const unsigned int id;
    PPDrawingGroupContainer();
    const char* name() const {
        return "PPDrawingGroupContainer ";
    }

private:
    // no copy or assign
    PPDrawingGroupContainer(const PPDrawingGroupContainer &);
    PPDrawingGroupContainer & operator=(const PPDrawingGroupContainer &);
};

class ProgBinaryTagContainer: public Container
{
public:
    static const unsigned int id;
    ProgBinaryTagContainer();
    const char* name() const {
        return "ProgBinaryTagContainer";
    }
private:
    // no copy or assign
    ProgBinaryTagContainer(const ProgBinaryTagContainer&);
    ProgBinaryTagContainer& operator=(const ProgBinaryTagContainer&);
};

class ProgStringTagContainer: public Container
{
public:
    static const unsigned int id;
    ProgStringTagContainer();
    const char* name() const {
        return "ProgStringTagContainer";
    }

private:
    // no copy or assign
    ProgStringTagContainer(const ProgStringTagContainer&);
    ProgStringTagContainer& operator=(const ProgStringTagContainer&);
};

class ProgTagsContainer : public Container
{
public:
    static const unsigned int id;
    ProgTagsContainer();
    const char* name() const {
        return "ProgTagsContainer ";
    }

private:
    // no copy or assign
    ProgTagsContainer(const ProgTagsContainer &);
    ProgTagsContainer & operator=(const ProgTagsContainer &);
};

class SlideContainer: public Container
{
public:
    static const unsigned int id;
    SlideContainer();
    const char* name() const {
        return "SlideContainer";
    }

private:
    // no copy or assign
    SlideContainer(const SlideContainer&);
    SlideContainer& operator=(const SlideContainer&);
};

class SlideBaseContainer: public Container
{
public:
    static const unsigned int id;
    SlideBaseContainer();
    const char* name() const {
        return "SlideBaseContainer";
    }

private:
    // no copy or assign
    SlideBaseContainer(const SlideBaseContainer&);
    SlideBaseContainer& operator=(const SlideBaseContainer&);
};

class SlideListWithTextContainer: public Container
{
public:
    static const unsigned int id;
    SlideListWithTextContainer();
    const char* name() const {
        return "SlideListWithTextContainer";
    }

private:
    // no copy or assign
    SlideListWithTextContainer(const SlideListWithTextContainer&);
    SlideListWithTextContainer& operator=(const SlideListWithTextContainer&);
};

class SlideViewInfoContainer: public Container
{
public:
    static const unsigned int id;
    SlideViewInfoContainer();
    const char* name() const {
        return "SlideViewInfoContainer";
    }

private:
    // no copy or assign
    SlideViewInfoContainer(const SlideViewInfoContainer&);
    SlideViewInfoContainer& operator=(const SlideViewInfoContainer&);
};

class SorterViewInfoContainer : public Container
{
public:
    static const unsigned int id;
    SorterViewInfoContainer();
    const char* name() const {
        return "SorterViewInfoContainer ";
    }

private:
    // no copy or assign
    SorterViewInfoContainer(const SorterViewInfoContainer &);
    SorterViewInfoContainer & operator=(const SorterViewInfoContainer &);
};

class SummaryContainer : public Container
{
public:
    static const unsigned int id;
    SummaryContainer();
    const char* name() const {
        return "SummaryContainer ";
    }

private:
    // no copy or assign
    SummaryContainer(const SummaryContainer &);
    SummaryContainer & operator=(const SummaryContainer &);
};

class SrKinsokuContainer: public Container
{
public:
    static const unsigned int id;
    SrKinsokuContainer();
    const char* name() const {
        return "SrKinsokuContainer";
    }

private:
    // no copy or assign
    SrKinsokuContainer(const SrKinsokuContainer&);
    SrKinsokuContainer& operator=(const SrKinsokuContainer&);
};

class VBAInfoContainer: public Container
{
public:
    static const unsigned int id;
    VBAInfoContainer();
    const char* name() const {
        return "VBAInfoContainer";
    }

private:
    // no copy or assign
    VBAInfoContainer(const VBAInfoContainer&);
    VBAInfoContainer& operator=(const VBAInfoContainer&);
};

class ViewInfoContainer: public Container
{
public:
    static const unsigned int id;
    ViewInfoContainer();
    const char* name() const {
        return "ViewInfoContainer";
    }

private:
    // no copy or assign
    ViewInfoContainer(const ViewInfoContainer&);
    ViewInfoContainer& operator=(const ViewInfoContainer&);
};

class msofbtDgContainer: public Container
{
public:
    static const unsigned int id;
    msofbtDgContainer();
    const char* name() const {
        return "msofbtDgContainer";
    }

private:
    // no copy or assign
    msofbtDgContainer(const msofbtDgContainer&);
    msofbtDgContainer& operator=(const msofbtDgContainer&);
};

class msofbtSpContainer: public Container
{
public:
    static const unsigned int id;
    msofbtSpContainer();
    const char* name() const {
        return "msofbtSpContainer";
    }

private:
    // no copy or assign
    msofbtSpContainer(const msofbtSpContainer&);
    msofbtSpContainer& operator=(const msofbtSpContainer&);
};

class msofbtSpgrContainer: public Container
{
public:
    static const unsigned int id;
    msofbtSpgrContainer();
    const char* name() const {
        return "msofbtSpgrContainer";
    }

private:
    // no copy or assign
    msofbtSpgrContainer(const msofbtSpgrContainer&);
    msofbtSpgrContainer& operator=(const msofbtSpgrContainer&);
};

class msofbtDggContainer: public Container
{
public:
    static const unsigned int id;
    msofbtDggContainer();
    const char* name() const {
        return "msofbtDggContainer";
    }

private:
    // no copy or assign
    msofbtDggContainer(const msofbtDggContainer&);
    msofbtDggContainer& operator=(const msofbtDggContainer&);
};

class msofbtBstoreContainer: public Container
{
public:
    static const unsigned int id;
    msofbtBstoreContainer();
    const char* name() const {
        return "msofbtBstoreContainer";
    }

private:
    // no copy or assign
    msofbtBstoreContainer(const msofbtBstoreContainer&);
    msofbtBstoreContainer& operator=(const msofbtBstoreContainer&);
};

class msofbtSolverContainer: public Container
{
public:
    static const unsigned int id;
    msofbtSolverContainer();
    const char* name() const {
        return "msofbtSolverContainer";
    }

private:
    // no copy or assign
    msofbtSolverContainer(const msofbtSolverContainer&);
    msofbtSolverContainer& operator=(const msofbtSolverContainer&);
};

class BinaryTagExtension: public Record
{
public:
    static const unsigned int id;
    BinaryTagExtension();
    const char* name() const {
        return "BinaryTagExtension";
    }

private:
    // no copy or assign
    BinaryTagExtension(const BinaryTagExtension&);
    BinaryTagExtension& operator=(const BinaryTagExtension&);
};


class BookmarkEntityAtom : public Record
{
public:
    static const unsigned int id;
    BookmarkEntityAtom();
    ~BookmarkEntityAtom();

    int bookmarkID() const;
    void setBookmarkID(int bookmarkID);
    int bookmarkName() const;
    void setBookmarkName(int bookmarkName);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "BookmarkEntityAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    BookmarkEntityAtom(const BookmarkEntityAtom &);
    BookmarkEntityAtom & operator=(const BookmarkEntityAtom &);

    class Private;
    Private *d;
};

class CStringAtom: public Record
{
public:
    static const unsigned int id;
    CStringAtom();
    ~CStringAtom();

    QString string() const;
    void setString(const QString& str);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "CStringAtom";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    CStringAtom(const CStringAtom&);
    CStringAtom& operator=(const CStringAtom&);

    class Private;
    QSharedDataPointer<Private> d;
};



class CurrentUserAtom : public Record
{
public:
    static const unsigned int id;
    CurrentUserAtom();
    ~CurrentUserAtom();

    int size() const;
    void setSize(int size);
    int magic() const;
    void setMagic(int magic);
    int offsetToCurrentEdit() const;
    void setOffsetToCurrentEdit(int offsetToCurrentEdit);
    int lenUserName() const;
    void setLenUserName(int lenUserName);
    int docFileVersion() const;
    void setDocFileVersion(int docFileVersion);
    int majorVersion() const;
    void setMajorVersion(int majorVersion);
    int minorVersion() const;
    void setMinorVersion(int minorVersion);
    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "ColorSchemeAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    CurrentUserAtom(const CurrentUserAtom &);
    CurrentUserAtom & operator=(const CurrentUserAtom &);

    class Private;
    Private *d;
};

class DocumentAtom : public Record
{
public:
    static const unsigned int id;
    DocumentAtom();
    ~DocumentAtom();

    int slideWidth() const;
    void setSlideWidth(int w);
    int slideHeight() const;
    void setSlideHeight(int h);
    int notesWidth() const;
    void setNotesWidth(int nw);
    int notesHeight() const;
    void setNotesHeight(int nh);
    int zoomNumer() const;
    void setZoomNumer(int numer);
    int zoomDenom() const;
    void setZoomDenom(int denom);

    int notesMasterPersist() const;
    void setNotesMasterPersist(int notesMasterPersist);

    int handoutMasterPersist() const;
    void setHandoutMasterPersist(int handoutMasterPersist);

    int firstSlideNum() const;
    void setFirstSlideNum(int firstSlideNum);

    int slideSizeType() const;
    void setSlideSizeType(int slideSizeType);

    int saveWithFonts() const;
    void setSaveWithFonts(int saveWithFonts);

    int omitTitlePlace() const;
    void setOmitTitlePlace(int omitTitlePlace);

    int rightToLeft() const;
    void setRightToLeft(int rightToLeft);

    int showComments() const;
    void setShowComments(int showComments);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "DocumentAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    DocumentAtom(const DocumentAtom &);
    DocumentAtom & operator=(const DocumentAtom &);

    class Private;
    Private *d;
};

class EndDocumentAtom: public Record
{
public:
    static const unsigned int id;
    EndDocumentAtom();
    const char* name() const {
        return "EndDocumentAtom";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    EndDocumentAtom(const EndDocumentAtom&);
    EndDocumentAtom& operator=(const EndDocumentAtom&);
};

class ExObjListAtom : public Record
{
public:
    static const unsigned int id;
    ExObjListAtom();
    ~ExObjListAtom();

    int objectIdSeed() const;
    void setObjectIdSeed(int objectIdSeed);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "ExObjListAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    ExObjListAtom(const ExObjListAtom &);
    ExObjListAtom & operator=(const ExObjListAtom &);

    class Private;
    Private *d;
};

class ExHyperlinkAtom : public Record
{
public:
    static const unsigned int id;
    ExHyperlinkAtom();
    ~ExHyperlinkAtom();

    int objID() const;
    void setObjID(int objID);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "ExHyperlinkAtom   ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    ExHyperlinkAtom(const ExHyperlinkAtom   &);
    ExHyperlinkAtom   & operator=(const ExHyperlinkAtom   &);

    class Private;
    Private *d;
};

class ExLinkAtom : public Record
{
public:
    static const unsigned int id;
    ExLinkAtom();
    ~ExLinkAtom();

    int exObjId() const;
    void setExObjId(int exObjId);
    int flags() const;
    void setFlags(int flags);
    int unavailable() const;
    void setUnavailable(int unavailable);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "ExLinkAtom";
    }
    void dump(std::ostream& out) const;
private:
    // no copy or assign
    ExLinkAtom(const ExLinkAtom  &);
    ExLinkAtom  & operator=(const ExLinkAtom  &);

    class Private;
    Private *d;
};

class ExOleObjAtom  : public Record
{
public:
    static const unsigned int id;
    ExOleObjAtom();
    ~ExOleObjAtom();

    int drawAspect() const;
    void setDrawAspect(int drawAspect);
    int type() const;
    void setType(int type);
    int objID() const;
    void setObjID(int objID);
    int subType() const;
    void setSubType(int subType);
    int objStgDataRef() const;
    void setObjStgDataRef(int objStgDataRef);
    int isBlank() const;
    void setIsBlank(int isBlank);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "ExOleObjAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    ExOleObjAtom(const ExOleObjAtom  &);
    ExOleObjAtom  & operator=(const ExOleObjAtom  &);

    class Private;
    Private *d;
};

class ExEmbedAtom  : public Record
{
public:
    static const unsigned int id;
    ExEmbedAtom();
    ~ExEmbedAtom();

    int followColorScheme() const;
    void setFollowColorScheme(int followColorScheme);
    int cantLockServerB() const;
    void setCantLockServerB(int cantLockServerB);
    int noSizeToServerB() const;
    void setNoSizeToServerB(int noSizeToServerB);
    int isTable() const;
    void setIsTable(int isTable);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "ExEmbedAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    ExEmbedAtom(const ExEmbedAtom  &);
    ExEmbedAtom  & operator=(const ExEmbedAtom  &);

    class Private;
    Private *d;
};

/**
* @brief An atom record that specifies the information needed to define the
* attributes of a font, such as typeface name, character set, and so forth, and
* corresponds in part to a Windows Logical Font (LOGFONT) structure
* [MC-LOGFONT].
*
*/
class FontEntityAtom : public Record
{
public:
    static const unsigned int id;
    FontEntityAtom();
    FontEntityAtom(const FontEntityAtom &other);
    ~FontEntityAtom();

    QString typeface() const;
    void setTypeface(const QString& typeface);
    int charset() const;
    void setCharset(int charset) ;
    int clipPrecision() const;
    void setClipPrecision(int clipPrecision);
    int quality() const;
    void setQuality(int quality);
    int pitchAndFamily() const;
    void setPitchAndFamily(int pitchAndFamily);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "FontEntityAtom";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    FontEntityAtom & operator=(const FontEntityAtom &);

    class Private;
    QSharedDataPointer<Private> d;
};

class GuideAtom  : public Record
{
public:
    static const unsigned int id;
    GuideAtom();
    ~GuideAtom();

    int type() const;
    void setType(int type);
    int pos() const;
    void setPos(int pos);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "GuideAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    GuideAtom(const GuideAtom  &);
    GuideAtom  & operator=(const GuideAtom  &);

    class Private;
    Private *d;
};

class HeadersFootersAtom  : public Record
{
public:
    static const unsigned int id;
    HeadersFootersAtom();
    ~HeadersFootersAtom();

    int formatId() const;
    void setFormatId(int slideId);
    int flags() const;
    void setFlags(int flags);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "HeadersFootersAtom  ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    HeadersFootersAtom(const HeadersFootersAtom  &);
    HeadersFootersAtom  & operator=(const HeadersFootersAtom  &);

    class Private;
    Private *d;
};

class NotesAtom : public Record
{
public:
    static const unsigned int id;
    NotesAtom();
    ~NotesAtom();

    int slideId() const;
    void setSlideId(int slideId);
    int flags() const;
    void setFlags(int flags);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "NotesAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    NotesAtom(const NotesAtom &);
    NotesAtom & operator=(const NotesAtom &);

    class Private;
    Private *d;
};

class PersistIncrementalBlockAtom : public Record
{
public:
    static const unsigned int id;
    PersistIncrementalBlockAtom();
    ~PersistIncrementalBlockAtom();

    unsigned entryCount() const;
    unsigned long reference(unsigned index) const;
    unsigned long offset(unsigned index) const;

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "PersistIncrementalBlockAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    PersistIncrementalBlockAtom(const PersistIncrementalBlockAtom &);
    PersistIncrementalBlockAtom & operator=(const PersistIncrementalBlockAtom &);

    class Private;
    Private *d;
};

class Record1043 : public Record
{
public:
    static const unsigned int id;
    Record1043();

// void setData( unsigned size, const unsigned char* data );
    const char* name() const {
        return "Record1043 ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    Record1043(const Record1043 &);
    Record1043 & operator=(const Record1043 &);

    class Private;
    Private *d;
};

class Record1044 : public Record
{
public:
    static const unsigned int id;
    Record1044();

// void setData( unsigned size, const unsigned char* data );
    const char* name() const {
        return "Record1044 ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    Record1044(const Record1044 &);
    Record1044 & operator=(const Record1044 &);

    class Private;
    Private *d;
};

class SSlideLayoutAtom  : public Record
{
public:
    static const unsigned int id;
    SSlideLayoutAtom();
    ~SSlideLayoutAtom();

    int geom() const;
    void setGeom(int geom);
    int placeholderId() const;
    void setPlaceholderId(int placeholderId);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "SSlideLayoutAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    SSlideLayoutAtom(const SSlideLayoutAtom  &);
    SSlideLayoutAtom  & operator=(const SSlideLayoutAtom  &);

    class Private;
    Private *d;
};

class SlideViewInfoAtom  : public Record
{
public:
    static const unsigned int id;
    SlideViewInfoAtom();
    ~SlideViewInfoAtom();

    int showGuides() const;
    void setShowGuides(int showGuides);
    int snapToGrid() const;
    void setSnapToGrid(int snapToGrid);
    int snapToShape() const;
    void setSnapToShape(int snapToShape);
    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "SlideViewInfoAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    SlideViewInfoAtom(const SlideViewInfoAtom  &);
    SlideViewInfoAtom  & operator=(const SlideViewInfoAtom  &);

    class Private;
    Private *d;
};

class SlidePersistAtom   : public Record
{
public:
    static const unsigned int id;
    SlidePersistAtom();
    virtual ~SlidePersistAtom();

    int psrReference() const;
    void setPsrReference(int psrReference);
    int flags() const;
    void setFlags(int flags);
    int numberTexts() const;
    void setNumberTexts(int numberTexts);
    int slideId() const;
    void setSlideId(int slideId);
    int reserved() const;
    void setReserved(int reserved);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "SlidePersistAtom  ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    SlidePersistAtom(const SlidePersistAtom  &);
    SlidePersistAtom  & operator=(const SlidePersistAtom  &);

    class Private;
    Private *d;
};

class SSDocInfoAtom : public Record
{
public:
    static const unsigned int id;
    SSDocInfoAtom();
    ~SSDocInfoAtom();

    int penColorRed() const;
    void setPenColorRed(int penColorRed);
    int penColorGreen() const;
    void setPenColorGreen(int penColorGreen);
    int penColorBlue() const;
    void setPenColorBlue(int penColorBlue);
    int penColorIndex() const;
    void setPenColorIndex(int penColorIndex);
    int restartTime() const;
    void setRestartTime(int restartTime);
    int startSlide() const;
    void setStartSlide(int startSlide);
    int endSlide() const;
    void setEndSlide(int endSlide);
    int namedShow() const;
    void setNamedShow(int namedShow);
    int flags() const;
    void setFlags(int flags);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "SSDocInfoAtom  ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    SSDocInfoAtom(const SSDocInfoAtom  &);
    SSDocInfoAtom  & operator=(const SSDocInfoAtom  &);

    class Private;
    Private *d;
};






/**
* @brief A structure that specifies the character-level formatting of a run of
* text.
*
* Let the corresponding text be as specified in the StyleTextPropAtom record
* that contains this TextCFRun structure.
*
*/
class TextCFRun
{
public:
    TextCFRun();
    TextCFRun(const TextCFRun &other);
    ~TextCFRun();

    /**
    * @brief get the amount of characters this style applies to
    * @return amount of characters
    */
    unsigned int count();

    /**
    * @brief set the amount of characters this style applies to
    * @param count character count
    */
    void setCount(unsigned int count);

    /**
    * @brief Get TextException that applies to a run of characters
    * @return TextException that applies to a run of characters
    */
    TextCFException *textCFException();
private:
    class Private;
    QSharedDataPointer<Private> d;
};

/**
* @brief A structure that specifies the paragraph-level formatting of a run of text.
*
* Let the corresponding text be as specified in the StyleTextPropAtom record
* that contains this TextPFRun structure.
*
*/
class TextPFRun
{
public:
    TextPFRun();
    TextPFRun(const TextPFRun &other);
    ~TextPFRun();

    /**
    * @brief get the amount of characters this style applies to
    * @return amount of characters
    */
    void setCount(unsigned int count);

    /**
    * @brief Set indentation level for this paragraph
    *
    */
    void setIndentLevel(unsigned int level);

    /**
    * @brief get the count of characters this style applies to
    * @return count of characters this style applies to
    */
    unsigned int count();

    /**
    * @brief Get indentation level for this paragraph
    * @return indentation level for this paragraph
    *
    */
    unsigned int indentLevel();

    /**
    * @brief Get TextPFException that applies to this paragraph
    * @return TextPFException that applies to this paragraph
    */
    TextPFException *textPFException();
private:
    class Private;
    QSharedDataPointer<Private> d;

};

/**
* @brief An atom record that specifies character-level and paragraph-level
* formatting.
*
* Let the corresponding text be specified by the TextHeaderAtom record that most
* closely precedes this record. Let the corresponding shape be as specified in
* the corresponding text. Let the corresponding main master be as specified in
* the corresponding text. If the corresponding shape is a placeholder shape,
* character-level and paragraph-level formatting not specified by this
* StyleTextPropAtom record inherit from the TextMasterStyleAtom records
* contained in the corresponding main master.
*
*/
class StyleTextPropAtom : public Record
{
public:
    static const unsigned int id;
    StyleTextPropAtom();
    StyleTextPropAtom(const StyleTextPropAtom &atom);
    ~StyleTextPropAtom();

    /**
    * @brief Get the amount of TextCFRuns this atom contains
    * @return amount of TextCFRuns this atom contains
    */
    unsigned int textCFRunCount();

    /**
    * @brief Get the amount of TextPFRuns this atom contains
    * @return amount of TextPFRuns this atom contains
    */

    unsigned int textPFRunCount();

    /**
    * @brief Get specific TextCFRun
    * @param index index of the TextCFRun to get
    * @return specified TextCFRun or 0 if none found
    */
    TextCFRun *textCFRun(unsigned int index);

    /**
    * @brief Get specific TextPFRun
    * @param index index of the TextPFRun to get
    * @return specified TextPFRun or 0 if none found
    */
    TextPFRun *textPFRun(unsigned int index);

    /**
    * @brief Find a TextCFRun that applies to specified position in text
    * @param position index of the text (character) that we want to find
    * TextCFRun to
    * @return TextCFRun or 0 if none applies
    */
    TextCFRun *findTextCFRun(unsigned int pos);

    /**
    * @brief Find a TextPFRun that applies to specified position in text
    * @param position index of the text (character) that we want to find
    * TextPFRun to
    * @return TextPFRun or 0 if none applies
    */
    TextPFRun *findTextPFRun(unsigned int pos);

    /**
    * @brief Parse data for this atom
    * @param size size of the data to parse
    * @param data Data to parse
    * @param lastSize Size of previous textbytes/chars atom that this style applies
    * to
    */
    void setDataWithSize(unsigned size, const unsigned char* data, unsigned lastSize);
    const char* name() const {
        return "StyleTextPropAtom   ";
    }
private:

    // no copy or assign
    //StyleTextPropAtom   ( const StyleTextPropAtom   & );
    StyleTextPropAtom   & operator=(const StyleTextPropAtom   &);

    class Private;
    QSharedDataPointer<Private> d;
};

class SlideAtom: public Record
{
public:
    static const unsigned int id;
    SlideAtom();
    ~SlideAtom();

    int layoutGeom() const;
    void setLayoutGeom(int layoutGeom);
// see OEPlaceHolderAtom
    int layoutPlaceholderId() const;
// void setLayoutPlaceholderId(int layoutPlaceholderId);
    void setLayoutPlaceholderId(int layoutPlaceholderId1, int layoutPlaceholderId2, int layoutPlaceholderId3, int layoutPlaceholderId4, int layoutPlaceholderId5, int layoutPlaceholderId6, int layoutPlaceholderId7, int layoutPlaceholderId8);
    int masterId() const;
    void setMasterId(int masterId);
    int notesId() const;
    void setNotesId(int notesId);
    int flags() const;
    void setFlags(int flags);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "SlideAtom";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    SlideAtom(const SlideAtom&);
    SlideAtom& operator=(const SlideAtom&);

    class Private;
    Private *d;
};

class SSSlideInfoAtom: public Record
{
public:
    static const unsigned int id;
    SSSlideInfoAtom();
    ~SSSlideInfoAtom();

    int transType() const;
    void settransType(int transType);
    int speed() const;
    void setspeed(int speed);
    int direction() const;
    void setdirection(int direction);
    int slideTime() const;
    void setslideTime(int slideTime);
    int buildFlags() const;
    void setbuildFlags(int buildFlags);
    int soundRef() const;
    void setsoundRef(int soundRef);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "SSSlideInfoAtom";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    SSSlideInfoAtom(const SSSlideInfoAtom&);
    SSSlideInfoAtom& operator=(const SSSlideInfoAtom&);

    class Private;
    Private *d;
};

class SrKinsokuAtom : public Record
{
public:
    static const unsigned int id;
    SrKinsokuAtom();
    ~SrKinsokuAtom();

// void setData( unsigned size, const unsigned char* data );
    const char* name() const {
        return "SrKinsokuAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    SrKinsokuAtom(const SrKinsokuAtom &);
    SrKinsokuAtom & operator=(const SrKinsokuAtom &);

    class Private;
    Private *d;
};



class TextCFExceptionAtom   : public Record
{
public:
    static const unsigned int id;
    TextCFExceptionAtom();
    ~TextCFExceptionAtom();

    TextCFException *textCFException();
    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "TextCFExceptionAtom   ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    TextCFExceptionAtom(const TextCFExceptionAtom   &);
    TextCFExceptionAtom   & operator=(const TextCFExceptionAtom   &);

    class Private;
    QSharedDataPointer<Private> d;
};


class TextCharsAtom   : public Record
{
public:
    static const unsigned int id;
    TextCharsAtom();
    ~TextCharsAtom();

    QString text() const;
    void setText(QString text);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "TextCharsAtom   ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    TextCharsAtom(const TextCharsAtom   &);
    TextCharsAtom   & operator=(const TextCharsAtom   &);

    class Private;
    Private *d;
};

class TextPFExceptionAtom : public Record
{
public:
    static const unsigned int id;
    TextPFExceptionAtom();
    ~TextPFExceptionAtom();

    TextPFException *textPFException();
    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "TextPFExceptionAtom   ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    TextPFExceptionAtom(const TextPFExceptionAtom  &);
    TextPFExceptionAtom   & operator=(const TextPFExceptionAtom  &);

    class Private;
    QSharedDataPointer<Private> d;
};

class TxSIStyleAtom    : public Record
{
public:
    static const unsigned int id;
    TxSIStyleAtom();
    ~TxSIStyleAtom();

    const char* name() const {
        return "TxSIStyleAtom    ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    TxSIStyleAtom(const TxSIStyleAtom   &);
    TxSIStyleAtom    & operator=(const TxSIStyleAtom   &);

    class Private;
    Private *d;
};

class TextHeaderAtom : public Record
{
public:
    static const unsigned int id;
    TextHeaderAtom();
    ~TextHeaderAtom();

    int textType() const;
    void setTextType(int type);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "TextHeaderAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    TextHeaderAtom(const TextHeaderAtom &);
    TextHeaderAtom & operator=(const TextHeaderAtom &);

    class Private;
    Private *d;
};

class TextSpecInfoAtom  : public Record
{
public:
    static const unsigned int id;
    TextSpecInfoAtom();
    ~TextSpecInfoAtom();

    int charCount() const;
    void setCharCount(int txSpecInfo);
    int flags() const;
    void setFlags(int flags);


    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "TextSpecInfoAtom  ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    TextSpecInfoAtom(const TextSpecInfoAtom &);
    TextSpecInfoAtom & operator=(const TextSpecInfoAtom &);

    class Private;
    Private *d;
};

class TextBookmarkAtom: public Record
{
public:
    static const unsigned int id;
    TextBookmarkAtom();
    ~TextBookmarkAtom();

    int begin() const;
    void setBegin(int begin);
    int end() const;
    void setEnd(int end);
    int bookmarkID() const;
    void setBookmarkID(int bookmarkID);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "TextBookmarkAtom";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    TextBookmarkAtom(const TextBookmarkAtom&);
    TextBookmarkAtom& operator=(const TextBookmarkAtom&);

    class Private;
    Private *d;
};

class TextBytesAtom : public Record
{
public:
    static const unsigned int id;
    TextBytesAtom();
    ~TextBytesAtom();

    QString text() const;
    void setText(QString text);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "TextBytesAtom   ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    TextBytesAtom(const TextBytesAtom   &);
    TextBytesAtom   & operator=(const TextBytesAtom   &);

    class Private;
    Private *d;
};

class UserEditAtom: public Record
{
public:
    static const unsigned int id;
    UserEditAtom();
    ~UserEditAtom();

    int lastSlideId() const;
    void setLastSlideId(int id);
    int majorVersion() const;
    void setMajorVersion(int majorVersion);
    int minorVersion() const;
    void setMinorVersion(int minorVersion);

    unsigned long offsetLastEdit() const;
    void setOffsetLastEdit(unsigned long ofs);
    unsigned long offsetPersistDir() const;
    void setOffsetPersistDir(unsigned long ofs) const;
    unsigned long documentRef() const;
    void setDocumentRef(unsigned long ref) const;

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "UserEditAtom";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    UserEditAtom(const UserEditAtom&);
    UserEditAtom& operator=(const UserEditAtom&);

    class Private;
    Private *d;
};

class ViewInfoAtom : public Record
{
public:
    static const unsigned int id;
    ViewInfoAtom();
    ~ViewInfoAtom();

    int curScaleXNum() const;
    void setCurScaleXNum(int curScaleXNum);
    int curScaleXDen() const;
    void setCurScaleXDen(int curScaleXDen);
    int curScaleYNum() const;
    void setCurScaleYNum(int curScaleYNum);
    int curScaleYDen() const;
    void setCurScaleYDen(int curScaleYDen);
    int prevScaleXNum() const;
    void setPrevScaleXNum(int prevScaleXNum);
    int prevScaleXDen() const;
    void setPrevScaleXDen(int prevScaleXDen);
    int prevScaleYNum() const;
    void setPrevScaleYNum(int prevScaleYNum);
    int prevScaleYDen() const;
    void setPrevScaleYDen(int prevScaleYDen);
    int viewSizeX() const;
    void setViewSizeX(int viewSizeX);
    int viewSizeY() const;
    void setViewSizeY(int viewSizeY);
    int originX() const;
    void setOriginX(int originX);
    int originY() const;
    void setOriginY(int originY);
    int varScale() const;
    void setVarScale(int varScale);
    int draftMode() const;
    void setDraftMode(int draftMode);
    int padding() const;
    void setPadding(int padding);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "ViewInfoAtom  ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    ViewInfoAtom(const ViewInfoAtom   &);
    ViewInfoAtom   & operator=(const ViewInfoAtom   &);

    class Private;
    Private *d;
};

class msofbtDgAtom : public Record
{
public:
    static const unsigned int id;
    msofbtDgAtom();
    ~msofbtDgAtom();

// void setData( unsigned size, const unsigned char* data );
    const char* name() const {
        return "msofbtDgAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    msofbtDgAtom(const msofbtDgAtom  &);
    msofbtDgAtom & operator=(const msofbtDgAtom  &);

    class Private;
    Private *d;
};

class msofbtSpgrAtom : public Record
{
public:
    static const unsigned int id;
    msofbtSpgrAtom();
    ~msofbtSpgrAtom();

    double x() const;
    void setX(double x);
    double y() const;
    void setY(double y);
    double width() const;
    void setWidth(double w);
    double height() const;
    void setHeight(double h);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "msofbtSpgrAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    msofbtSpgrAtom(const msofbtSpgrAtom  &);
    msofbtSpgrAtom & operator=(const msofbtSpgrAtom  &);

    class Private;
    Private *d;
};

class msofbtSpAtom : public Record
{
public:
    enum {
        msosptMin = 0,
        msosptNotPrimitive = msosptMin,
        msosptRectangle = 1,
        msosptRoundRectangle = 2,
        msosptEllipse = 3,
        msosptDiamond = 4,
        msosptIsoscelesTriangle = 5,
        msosptRightTriangle = 6,
        msosptParallelogram = 7,
        msosptTrapezoid = 8,
        msosptHexagon = 9,
        msosptOctagon = 10,
        msosptPlus = 11,
        msosptStar = 12,
        msosptArrow = 13,
        msosptThickArrow = 14,
        msosptHomePlate = 15,
        msosptCube = 16,
        msosptBalloon = 17,
        msosptSeal = 18,
        msosptArc = 19,
        msosptLine = 20,
        msosptPlaque = 21,
        msosptCan = 22,
        msosptDonut = 23,
        msosptTextSimple = 24,
        msosptTextOctagon = 25,
        msosptTextHexagon = 26,
        msosptTextCurve = 27,
        msosptTextWave = 28,
        msosptTextRing = 29,
        msosptTextOnCurve = 30,
        msosptTextOnRing = 31,
        msosptStraightConnector1 = 32,
        msosptBentConnector2 = 33,
        msosptBentConnector3 = 34,
        msosptBentConnector4 = 35,
        msosptBentConnector5 = 36,
        msosptCurvedConnector2 = 37,
        msosptCurvedConnector3 = 38,
        msosptCurvedConnector4 = 39,
        msosptCurvedConnector5 = 40,
        msosptCallout1 = 41,
        msosptCallout2 = 42,
        msosptCallout3 = 43,
        msosptAccentCallout1 = 44,
        msosptAccentCallout2 = 45,
        msosptAccentCallout3 = 46,
        msosptBorderCallout1 = 47,
        msosptBorderCallout2 = 48,
        msosptBorderCallout3 = 49,
        msosptAccentBorderCallout1 = 50,
        msosptAccentBorderCallout2 = 51,
        msosptAccentBorderCallout3 = 52,
        msosptRibbon = 53,
        msosptRibbon2 = 54,
        msosptChevron = 55,
        msosptPentagon = 56,
        msosptNoSmoking = 57,
        msosptSeal8 = 58,
        msosptSeal16 = 59,
        msosptSeal32 = 60,
        msosptWedgeRectCallout = 61,
        msosptWedgeRRectCallout = 62,
        msosptWedgeEllipseCallout = 63,
        msosptWave = 64,
        msosptFoldedCorner = 65,
        msosptLeftArrow = 66,
        msosptDownArrow = 67,
        msosptUpArrow = 68,
        msosptLeftRightArrow = 69,
        msosptUpDownArrow = 70,
        msosptIrregularSeal1 = 71,
        msosptIrregularSeal2 = 72,
        msosptLightningBolt = 73,
        msosptHeart = 74,
        msosptPictureFrame = 75,
        msosptQuadArrow = 76,
        msosptLeftArrowCallout = 77,
        msosptRightArrowCallout = 78,
        msosptUpArrowCallout = 79,
        msosptDownArrowCallout = 80,
        msosptLeftRightArrowCallout = 81,
        msosptUpDownArrowCallout = 82,
        msosptQuadArrowCallout = 83,
        msosptBevel = 84,
        msosptLeftBracket = 85,
        msosptRightBracket = 86,
        msosptLeftBrace = 87,
        msosptRightBrace = 88,
        msosptLeftUpArrow = 89,
        msosptBentUpArrow = 90,
        msosptBentArrow = 91,
        msosptSeal24 = 92,
        msosptStripedRightArrow = 93,
        msosptNotchedRightArrow = 94,
        msosptBlockArc = 95,
        msosptSmileyFace = 96,
        msosptVerticalScroll = 97,
        msosptHorizontalScroll = 98,
        msosptCircularArrow = 99,
        msosptNotchedCircularArrow = 100,
        msosptUturnArrow = 101,
        msosptCurvedRightArrow = 102,
        msosptCurvedLeftArrow = 103,
        msosptCurvedUpArrow = 104,
        msosptCurvedDownArrow = 105,
        msosptCloudCallout = 106,
        msosptEllipseRibbon = 107,
        msosptEllipseRibbon2 = 108,
        msosptFlowChartProcess = 109,
        msosptFlowChartDecision = 110,
        msosptFlowChartInputOutput = 111,
        msosptFlowChartPredefinedProcess = 112,
        msosptFlowChartInternalStorage = 113,
        msosptFlowChartDocument = 114,
        msosptFlowChartMultidocument = 115,
        msosptFlowChartTerminator = 116,
        msosptFlowChartPreparation = 117,
        msosptFlowChartManualInput = 118,
        msosptFlowChartManualOperation = 119,
        msosptFlowChartConnector = 120,
        msosptFlowChartPunchedCard = 121,
        msosptFlowChartPunchedTape = 122,
        msosptFlowChartSummingJunction = 123,
        msosptFlowChartOr = 124,
        msosptFlowChartCollate = 125,
        msosptFlowChartSort = 126,
        msosptFlowChartExtract = 127,
        msosptFlowChartMerge = 128,
        msosptFlowChartOfflineStorage = 129,
        msosptFlowChartOnlineStorage = 130,
        msosptFlowChartMagneticTape = 131,
        msosptFlowChartMagneticDisk = 132,
        msosptFlowChartMagneticDrum = 133,
        msosptFlowChartDisplay = 134,
        msosptFlowChartDelay = 135,
        msosptTextPlainText = 136,
        msosptTextStop = 137,
        msosptTextTriangle = 138,
        msosptTextTriangleInverted = 139,
        msosptTextChevron = 140,
        msosptTextChevronInverted = 141,
        msosptTextRingInside = 142,
        msosptTextRingOutside = 143,
        msosptTextArchUpCurve = 144,
        msosptTextArchDownCurve = 145,
        msosptTextCircleCurve = 146,
        msosptTextButtonCurve = 147,
        msosptTextArchUpPour = 148,
        msosptTextArchDownPour = 149,
        msosptTextCirclePour = 150,
        msosptTextButtonPour = 151,
        msosptTextCurveUp = 152,
        msosptTextCurveDown = 153,
        msosptTextCascadeUp = 154,
        msosptTextCascadeDown = 155,
        msosptTextWave1 = 156,
        msosptTextWave2 = 157,
        msosptTextWave3 = 158,
        msosptTextWave4 = 159,
        msosptTextInflate = 160,
        msosptTextDeflate = 161,
        msosptTextInflateBottom = 162,
        msosptTextDeflateBottom = 163,
        msosptTextInflateTop = 164,
        msosptTextDeflateTop = 165,
        msosptTextDeflateInflate = 166,
        msosptTextDeflateInflateDeflate = 167,
        msosptTextFadeRight = 168,
        msosptTextFadeLeft = 169,
        msosptTextFadeUp = 170,
        msosptTextFadeDown = 171,
        msosptTextSlantUp = 172,
        msosptTextSlantDown = 173,
        msosptTextCanUp = 174,
        msosptTextCanDown = 175,
        msosptFlowChartAlternateProcess = 176,
        msosptFlowChartOffpageConnector = 177,
        msosptCallout90 = 178,
        msosptAccentCallout90 = 179,
        msosptBorderCallout90 = 180,
        msosptAccentBorderCallout90 = 181,
        msosptLeftRightUpArrow = 182,
        msosptSun = 183,
        msosptMoon = 184,
        msosptBracketPair = 185,
        msosptBracePair = 186,
        msosptSeal4 = 187,
        msosptDoubleWave = 188,
        msosptActionButtonBlank = 189,
        msosptActionButtonHome = 190,
        msosptActionButtonHelp = 191,
        msosptActionButtonInformation = 192,
        msosptActionButtonForwardNext = 193,
        msosptActionButtonBackPrevious = 194,
        msosptActionButtonEnd = 195,
        msosptActionButtonBeginning = 196,
        msosptActionButtonReturn = 197,
        msosptActionButtonDocument = 198,
        msosptActionButtonSound = 199,
        msosptActionButtonMovie = 200,
        msosptHostControl = 201,
        msosptTextBox = 202,
        msosptMax,
        msosptNil = 0x0FFF
    } ;

    static const unsigned int id;
    msofbtSpAtom();
    ~msofbtSpAtom();

    unsigned long shapeId() const;
    void setShapeId(unsigned long id);
    const char* shapeTypeAsString() const;
    unsigned long persistentFlag() const;
    void setPersistentFlag(unsigned long persistentFlag);

    bool isBackground() const;
    void setBackground(bool bg);
    bool isVerFlip() const;
    void setVerFlip(bool vFlip);
    bool isHorFlip() const;
    void setHorFlip(bool hFlip);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "msofbtSpAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    msofbtSpAtom(const msofbtSpAtom  &);
    msofbtSpAtom & operator=(const msofbtSpAtom  &);

    class Private;
    Private *d;
};

class msofbtOPTAtom : public Record
{
public:
    enum {               // PID
        Rotation = 4,
        Pib = 260,
        FillType = 384,
        FillColor = 385,
        LineColor = 448,
        LineOpacity = 449,
        LineBackColor = 450,
        LineType = 452,
        LineWidth =  459,
        LineDashing = 462,
        LineStartArrowhead = 464,
        LineEndArrowhead = 465,
        LineStartArrowWidth = 466,
        LineStartArrowLength = 467,
        LineEndArrowWidth = 468,
        LineEndArrowLength = 469,
        FlagNoLineDrawDash = 511,
        ShadowColor = 513,
        ShadowOpacity =  516,
        ShadowOffsetX = 517,
        ShadowOffsetY = 518
    };

    enum {
        FillSolid,             // Fill with a solid color
        FillPattern,           // Fill with a pattern (bitmap)
        FillTexture,           // A texture (pattern with its own color map)
        FillPicture,           // Center a picture in the shape
        FillShade,             // Shade from start to end points
        FillShadeCenter,       // Shade from bounding rectangle to end point
        FillShadeShape,        // Shade from shape outline to end point
        FillShadeScale,        // Similar to msofillShade, but the fillAngle
        FillShadeTitle,        // special type - shade to title ---  for PP
        FillBackground         // Use the background fill color/pattern
    }; // MSOFILLTYPE

    enum {
        LineSolid,              // Solid (continuous) pen           0
        LineDashSys,            // PS_DASH system   dash style      1
        LineDotSys,             // PS_DOT system   dash style       2
        LineDashDotSys,         // PS_DASHDOT system dash style      3
        LineDashDotDotSys,      // PS_DASHDOTDOT system dash style   4
        LineDotGEL,             // square dot style                  5
        LineDashGEL,            // dash style                       6
        LineLongDashGEL,        // long dash style                       7
        LineDashDotGEL,         // dash short dash                 8
        LineLongDashDotGEL,     // long dash short dash             9
        LineLongDashDotDotGEL   // long dash short dash short dash        10
    }; // MSOLINEDASHING


    enum {
        LineNoEnd,
        LineArrowEnd,
        LineArrowStealthEnd,
        LineArrowDiamondEnd,
        LineArrowOvalEnd,
        LineArrowOpenEnd
    };  // MSOLINEEND - line end effect


    enum {
        LineNarrowArrow,
        LineMediumWidthArrow,
        LineWideArrow
    }; // MSOLINEENDWIDTH - size of arrowhead

    enum {
        LineShortArrow,
        LineMediumLenArrow,
        LineLongArrow
    };   // MSOLINEENDLENGTH - size of arrowhead

    static const unsigned int id;
    msofbtOPTAtom();
    ~msofbtOPTAtom();

    unsigned propertyCount() const;
    unsigned propertyId(unsigned index) const;
    unsigned long propertyValue(unsigned index) const;
    void setProperty(unsigned id, unsigned long value);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "msofbtOPTAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    msofbtOPTAtom(const msofbtOPTAtom  &);
    msofbtOPTAtom & operator=(const msofbtOPTAtom  &);

    class Private;
    Private *d;
};

class msofbtChildAnchorAtom : public Record
{
public:
    static const unsigned int id;
    msofbtChildAnchorAtom();
    ~msofbtChildAnchorAtom();

    int left() const;
    void setLeft(int left);
    int top() const;
    void setTop(int top);
    int right() const;
    void setRight(int right);
    int bottom() const;
    void setBottom(int bottom);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "msofbtChildAnchorAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    msofbtChildAnchorAtom(const msofbtChildAnchorAtom  &);
    msofbtChildAnchorAtom & operator=(const msofbtChildAnchorAtom  &);

    class Private;
    Private *d;
};

class msofbtClientAnchorAtom : public Record
{
public:
    static const unsigned int id;
    msofbtClientAnchorAtom();
    ~msofbtClientAnchorAtom();

    int left() const;
    void setLeft(int left);
    int top() const;
    void setTop(int top);
    int right() const;
    void setRight(int right);
    int bottom() const;
    void setBottom(int bottom);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "msofbtClientAnchorAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    msofbtClientAnchorAtom(const msofbtClientAnchorAtom  &);
    msofbtClientAnchorAtom & operator=(const msofbtClientAnchorAtom  &);

    class Private;
    Private *d;
};

class msofbtClientDataAtom : public Record
{
public:
    enum { None = 0,
           MasterTitle,
           MasterBody,
           MasterCenteredTitle,
           MasterNotesSlideImage,
           MasterNotesBodyImage,
           MasterDate,
           MasterSlideNumber,
           MasterFooter,
           MasterHeader,
           MasterSubtitle,
           Generic,
           Title,
           Body,
           NotesBody,
           CenteredTitle,
           Subtitle,
           VerticalTextTitle,
           VerticalTextBody,
           NotesSlideImage,
           Object,
           Graph,
           Table,
           ClipArt,
           OrganizationChart,
           MediaClip
         };

    static const unsigned int id;
    msofbtClientDataAtom();
    ~msofbtClientDataAtom();

    unsigned placementId() const;
    void setPlacementId(unsigned id);
    unsigned placeholderId() const;
    void setPlaceholderId(unsigned id);
    const char* placeholderIdAsString() const;

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "msofbtClientDataAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    msofbtClientDataAtom(const msofbtClientDataAtom  &);
    msofbtClientDataAtom & operator=(const msofbtClientDataAtom  &);

    class Private;
    Private *d;
};

class msofbtClientTextBox : public Container
{
public:
    static const unsigned int id;
    msofbtClientTextBox();
    ~msofbtClientTextBox();

private:
    // no copy or assign
    msofbtClientTextBox(const msofbtClientTextBox  &);
    msofbtClientTextBox & operator=(const msofbtClientTextBox  &);

    class Private;
    Private *d;
};

class msofbtOleObjectAtom : public Record
{
public:
    static const unsigned int id;
    msofbtOleObjectAtom();
    ~msofbtOleObjectAtom();
// void setData( unsigned size, const unsigned char* data );
    const char* name() const {
        return "msofbtOleObjectAtom ";
    }
    void dump(std::ostream& out) const;
private:
    // no copy or assign
    msofbtOleObjectAtom(const msofbtOleObjectAtom  &);
    msofbtOleObjectAtom & operator=(const msofbtOleObjectAtom  &);
    class Private;
    Private *d;
};

class msofbtDeletedPsplAtom : public Record
{
public:
    static const unsigned int id;
    msofbtDeletedPsplAtom();
    ~msofbtDeletedPsplAtom();
// void setData( unsigned size, const unsigned char* data );
    const char* name() const {
        return "msofbtDeletedPsplAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    msofbtDeletedPsplAtom(const msofbtDeletedPsplAtom  &);
    msofbtDeletedPsplAtom & operator=(const msofbtDeletedPsplAtom  &);
    class Private;
    Private *d;

};

class msofbtDggAtom : public Record
{
public:
    static const unsigned int id;
    msofbtDggAtom();
    ~msofbtDggAtom();
// void setData( unsigned size, const unsigned char* data );
    const char* name() const {
        return "msofbtDggAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    msofbtDggAtom(const msofbtDggAtom  &);
    msofbtDggAtom & operator=(const msofbtDggAtom  &);

    class Private;
    Private *d;
};

class msofbtColorMRUAtom : public Record
{
public:
    static const unsigned int id;
    msofbtColorMRUAtom();
    ~msofbtColorMRUAtom();
// void setData( unsigned size, const unsigned char* data );
    const char* name() const {
        return "msofbtColorMRUAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    msofbtColorMRUAtom(const msofbtColorMRUAtom  &);
    msofbtColorMRUAtom & operator=(const msofbtColorMRUAtom  &);

    class Private;
    Private *d;
};

class msofbtSplitMenuColorsAtom : public Record
{
public:
    static const unsigned int id;
    msofbtSplitMenuColorsAtom();
    ~msofbtSplitMenuColorsAtom();

    unsigned fillColor() const;
    void setFillColor(unsigned fillColor);
    unsigned lineColor() const;
    void setLineColor(unsigned lineColor);
    unsigned shadowColor() const;
    void setShadowColor(unsigned shadowColor);
    unsigned threeDColor() const;
    void setThreeDColor(unsigned threeDColor);

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "msofbtSplitMenuColorsAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    msofbtSplitMenuColorsAtom(const msofbtSplitMenuColorsAtom  &);
    msofbtSplitMenuColorsAtom & operator=(const msofbtSplitMenuColorsAtom  &);

    class Private;
    Private *d;
};

class msofbtBSEAtom : public Record
{
public:
    static const unsigned int id;
    msofbtBSEAtom();
    ~msofbtBSEAtom();

// void setData( unsigned size, const unsigned char* data );
    const char* name() const {
        return "msofbtBSEAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    msofbtBSEAtom(const msofbtBSEAtom  &);
    msofbtBSEAtom & operator=(const msofbtBSEAtom  &);

    class Private;
    Private *d;
};

class msofbtCLSIDAtom : public Record
{
public:
    static const unsigned int id;
    msofbtCLSIDAtom();
    ~msofbtCLSIDAtom();

// void setData( unsigned size, const unsigned char* data );
    const char* name() const {
        return "msofbtCLSIDAtom ";
    }
    void dump(std::ostream& out) const;


private:
    // no copy or assign
    msofbtCLSIDAtom(const msofbtCLSIDAtom  &);
    msofbtCLSIDAtom & operator=(const msofbtCLSIDAtom  &);

    class Private;
    Private *d;
};

class msofbtRegroupItemsAtom : public Record
{
public:
    static const unsigned int id;
    msofbtRegroupItemsAtom();
    ~msofbtRegroupItemsAtom();

// void setData( unsigned size, const unsigned char* data );
    const char* name() const {
        return "msofbtRegroupItemsAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    msofbtRegroupItemsAtom(const msofbtRegroupItemsAtom  &);
    msofbtRegroupItemsAtom & operator=(const msofbtRegroupItemsAtom  &);

    class Private;
    Private *d;
};

class msofbtColorSchemeAtom : public Record
{
public:
    static const unsigned int id;
    msofbtColorSchemeAtom();
    ~msofbtColorSchemeAtom();

// void setData( unsigned size, const unsigned char* data );
    const char* name() const {
        return "msofbtColorSchemeAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    msofbtColorSchemeAtom(const msofbtColorSchemeAtom  &);
    msofbtColorSchemeAtom & operator=(const msofbtColorSchemeAtom  &);

    class Private;
    Private *d;
};

class msofbtAnchorAtom : public Record
{
public:
    static const unsigned int id;
    msofbtAnchorAtom();
    ~msofbtAnchorAtom();

// void setData( unsigned size, const unsigned char* data );
    const char* name() const {
        return "msofbtAnchorAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    msofbtAnchorAtom(const msofbtAnchorAtom  &);
    msofbtAnchorAtom & operator=(const msofbtAnchorAtom  &);

    class Private;
    Private *d;
};

class msofbtConnectorRuleAtom : public Record
{
public:
    static const unsigned int id;
    msofbtConnectorRuleAtom();
    ~msofbtConnectorRuleAtom();

// void setData( unsigned size, const unsigned char* data );
    const char* name() const {
        return "msofbtConnectorRuleAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    msofbtConnectorRuleAtom(const msofbtConnectorRuleAtom  &);
    msofbtConnectorRuleAtom & operator=(const msofbtConnectorRuleAtom  &);

    class Private;
    Private *d;
};

class msofbtAlignRuleAtom : public Record
{
public:
    static const unsigned int id;
    msofbtAlignRuleAtom();
    ~msofbtAlignRuleAtom();

    int ruid() const; // rule ID
    void setRuid(int ruid);
    int align() const;
    void setAlign(int align);  // alignment
    int cProxies() const;
    void setCProxies(int cProxies);  // number of shapes governed by rule

    void setData(unsigned size, const unsigned char* data);
    const char* name() const {
        return "msofbtAlignRuleAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    msofbtAlignRuleAtom(const msofbtAlignRuleAtom  &);
    msofbtAlignRuleAtom & operator=(const msofbtAlignRuleAtom  &);

    class Private;
    Private *d;
};

class msofbtArcRuleAtom : public Record
{
public:
    static const unsigned int id;
    msofbtArcRuleAtom();
    ~msofbtArcRuleAtom();

// void setData( unsigned size, const unsigned char* data );
    const char* name() const {
        return "msofbtArcRuleAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    msofbtArcRuleAtom(const msofbtArcRuleAtom  &);
    msofbtArcRuleAtom & operator=(const msofbtArcRuleAtom  &);

    class Private;
    Private *d;
};

class msofbtClientRuleAtom : public Record
{
public:
    static const unsigned int id;
    msofbtClientRuleAtom();
    ~msofbtClientRuleAtom();

// void setData( unsigned size, const unsigned char* data );
    const char* name() const {
        return "msofbtClientRuleAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    msofbtClientRuleAtom(const msofbtClientRuleAtom  &);
    msofbtClientRuleAtom & operator=(const msofbtClientRuleAtom  &);

    class Private;
    Private *d;
};

class msofbtCalloutRuleAtom : public Record
{
public:
    static const unsigned int id;
    msofbtCalloutRuleAtom();
    ~msofbtCalloutRuleAtom();

// void setData( unsigned size, const unsigned char* data );
    const char* name() const {
        return "msofbtCalloutRuleAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    msofbtCalloutRuleAtom(const msofbtCalloutRuleAtom  &);
    msofbtCalloutRuleAtom & operator=(const msofbtCalloutRuleAtom  &);

    class Private;
    Private *d;
};

class msofbtSelectionAtom : public Record
{
public:
    static const unsigned int id;
    msofbtSelectionAtom();
    ~msofbtSelectionAtom();

// void setData( unsigned size, const unsigned char* data );
    const char* name() const {
        return "msofbtSelectionAtom ";
    }
    void dump(std::ostream& out) const;

private:
    // no copy or assign
    msofbtSelectionAtom(const msofbtSelectionAtom  &);
    msofbtSelectionAtom & operator=(const msofbtSelectionAtom  &);

    class Private;
    Private *d;
};

class PPTReader
{
public:
    PPTReader();
    virtual ~PPTReader();
    bool load(Presentation* pr, const char* filename);

protected:

    void loadUserEdit();
    void loadMaster();
    void loadSlides();
    void loadDocument();
    int indexPersistence(unsigned long offset);

    void loadRecord(Record* parent);
    void handleRecord(Record* record, int type);
    void handleContainer(Container* container, int type, unsigned size);

    void handleDocumentAtom(DocumentAtom* r);
    void handleSlidePersistAtom(SlidePersistAtom* r);
    void handleTextHeaderAtom(TextHeaderAtom* r);
    void handleTextCharsAtom(TextCharsAtom* r);
    void handleTextBytesAtom(TextBytesAtom* r);
    void handleStyleTextPropAtom(StyleTextPropAtom* r);
    void handleColorSchemeAtom(ColorSchemeAtom* r);
    void handleTextPFExceptionAtom(TextPFExceptionAtom* r);
    void handleTextCFExceptionAtom(TextCFExceptionAtom* r);

    void handleDrawingContainer(msofbtDgContainer* r, unsigned size);
    void handleEscherGroupContainer(msofbtSpgrContainer* r, unsigned size);
    void handleSPContainer(msofbtSpContainer* r, unsigned size);
    void handleEscherTextBox(msofbtClientTextBox* r, unsigned size);
    void handleEscherGroupAtom(msofbtSpgrAtom* r);
    void handleEscherSpAtom(msofbtSpAtom* r);
    void handleEscherPropertiesAtom(msofbtOPTAtom* atom);
    void handleEscherClientDataAtom(msofbtClientDataAtom* r);
    void handleEscherClientAnchorAtom(msofbtClientAnchorAtom* r);
    void handleEscherChildAnchorAtom(msofbtChildAnchorAtom* r);
    void handleProgBinaryTagContainer(ProgBinaryTagContainer* r,
                                      unsigned int size);


    void loadMainMasterContainer(MainMasterContainer *container);

    /**
    * @brief Increase position in documentstream until specified record is found
    *
    * Position is always set to the start of the record header
    * @param wantedType What is the record type that we are looking for
    * @param max how many records can we go through to find this one
    * @param count of records skipped or -1 if failed
    */
    int fastForwardRecords(unsigned int wantedType, unsigned int max);

    /**
    * @brief Handle font entity by creating a TextFont and storing it to
    * this presentation's font collection.
    * @param r FontEntityAtom to create font from
    */
    void handleFontEntityAtom(FontEntityAtom* r);
private:
    // no copy or assign
    PPTReader(const PPTReader&);
    PPTReader& operator=(const PPTReader&);

    class Private;
    Private* d;
};




}

#endif /* LIBPPT_POWERPOINT */
