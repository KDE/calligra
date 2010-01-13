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

#ifndef LIBPPT_OBJECTS
#define LIBPPT_OBJECTS

#include <string>
#include <QString>
#include <QColor>
#include "powerpoint.h"

namespace Libppt
{

class Color
{
public:
    unsigned red, green, blue;
    Color() {
        red = green = blue = 0;
    }
    Color(unsigned r, unsigned g, unsigned b) {
        red = r; green = g; blue = b;
    }
    Color(const Color& c) {
        red = c.red; green = c.green; blue = c.blue;
    }
    Color& operator=(const Color& c) {
        red = c.red; green = c.green; blue = c.blue; return *this;
    }
};

class Object
{
public:
    Object();
    Object(Object* o);
    virtual ~Object();

    int id() const;
    void setId(int id);

    virtual bool isText() const {
        return false;
    }
    virtual bool isPicture() const {
        return false;
    }
    virtual bool isGroup() const {
        return false;
    }
    virtual bool isDrawing() const {
        return false;
    }

    // all is in mm

    double top() const;
    double left() const;
    double width() const;
    double height() const;
    void setTop(double top);
    void setLeft(double left);
    void setWidth(double width);
    void setHeight(double height);

    bool isBackground() const;
    void setBackground(bool bg);


    bool hasProperty(std::string name) const;

    void setProperty(std::string name, int value);
    void setProperty(std::string name, double value);
    void setProperty(std::string name, std::string value);
    void setProperty(std::string name, bool value);
    void setProperty(std::string name, Color value);
    void setProperty(std::string name, const char* value) {
        setProperty(name, std::string(value));
    }

    int getIntProperty(std::string name) const;
    double getDoubleProperty(std::string name) const;
    bool getBoolProperty(std::string name) const;
    std::string getStrProperty(std::string name) const;
    Color getColorProperty(std::string name) const;

    /**
    * @brief Set the graphic style name
    */
    void setGraphicStyleName(const QString& name);

    /**
    * @brief Get the graphic style name
    */
    QString graphicStyleName() const;

private:
    // no copy or assign
    Object(const Object&);
    Object& operator=(const Object&);

    class Private;
    Private* const d;
};


/**
* @brief Class that hold text objects from the slide and style information
*
*/
class TextObject: public Object
{
public:

    enum {
        Title       = 0,
        Body        = 1,
        Notes       = 2,
        NotUsed     = 3,
        Other       = 4,  // text in a shape
        CenterBody  = 5,  // subtitle in title slide
        CenterTitle = 6,  // title in title slide
        HalfBody    = 7,  // body in two-column slide
        QuarterBody = 8   // body in four-body slide
    };

    TextObject();
    TextObject(Object*);
    virtual ~TextObject();
    virtual bool isText() const {
        return true;
    }
    unsigned type() const;
    void setType(unsigned type);
    const char* typeAsString() const;

    /**
    * @brief Assign a style atom for this text object
    * @param atom Style atom
    * @param atom9 Extended style atom
    */
    void setStyleTextProperty(const StyleTextPropAtom *atom,
                              const StyleTextProp9Atom *atom9);

    /**
    * @brief Get text (e.g. T1) style that applies to given pair of
    * text cf/pf exceptions
    * @param cf TextCFException that applies to the text
    * @param pf TextPFException that applies to the text
    */
    QString textStyleName(const TextCFException *cf,
                          const TextPFException *pf) const;

    /**
    * @brief Get paragraph (e.g. P1) style that applies to given pair of
    * text cf/pf exceptions
    * @param cf TextCFException that applies to the text
    * @param pf TextPFException that applies to the text
    */
    QString paragraphStyleName(const TextCFException *cf,
                               const TextPFException *pf) const;

    /**
    * @brief Get list (e.g. L1) style that applies to given pair of
    * text cf/pf exceptions
    * @param cf TextCFException that applies to the text
    * @param pf TextPFException that applies to the text
    */
    QString listStyleName(const TextCFException *cf,
                          const TextPFException *pf) const;

    /**
    * @brief Add pre parsed style names
    * @param cf TextCFException that applies corresponds to style names
    * @param pf TextPFException that applies corresponds to style names
    * @param text Name of text style that applies to both cf and pf
    * @param paragraph Name of paragraph style that applies to both cf and pf
    * @param list Name of list style that applies to both cf and pf
    */
    void addStylenames(const TextCFException *cf,
                       const TextPFException *pf,
                       const QString &text,
                       const QString &paragraph,
                       const QString &list);

    /**
    * @brief Get text style atom
    * @return This object's StyleTextPropAtom
    */
    StyleTextPropAtom *styleTextProperty();

    /**
    * @brief Get text extendend style atom
    * @return This object's StyleTextProp9Atom
    */
    StyleTextProp9Atom *styleTextProperty9();

    /**
    * @brief Find StyleTextProp9 that matches the specified TextCFException
    *
    * [MS-PPT].pdf 2.9.67 StyleTextProp9Atom says the following:
    * An array of StyleTextProp9 structures that specifies additional formatting
    * for the corresponding text. Each item in the array specifies formatting
    * for a sequence of consecutive character runs of the corresponding text
    * that share the same value of the fontStyle.pp9rt field of the
    * TextCFException record. If a TextCFException record does not specify a
    * fontStyle.pp9rt field, its value is assumed to be 0x0000. An item at index
    * i MUST be ignored if i % 16 is not equal to the value of the
    * fontstyle.pp9rt field of the next such sequence. The length, in bytes, of
    * the array is specified by rh.recLen.
    */
    StyleTextProp9 *findStyleTextProp9(TextCFException *cf);

    /**
    * @brief Get the actual text
    *
    * @return text
    */
    QString text() const;

    /**
    * @brief Set text for this object
    *
    * @param text text to set
    */
    void setText(const QString& text);
private:

    /**
    * @brief Find index of StyleName array that has specified cf and pf values
    * @param cf Text character exception to find
    * @param pf Text paragraph exception to find
    * @return index within Stylename array that contains both cf and pf
    */
    int findStyle(const TextCFException *cf, const TextPFException *pf) const;
    // no copy or assign
    TextObject(const TextObject&);
    TextObject& operator=(const TextObject&);

    class Private;
    Private* const d;
};

class GroupObject: public Object
{
public:
    GroupObject();
    virtual ~GroupObject();
    virtual bool isGroup() const {
        return true;
    }
    unsigned objectCount() const;
    Object* object(unsigned index);
    const Object* object(unsigned index) const;
    void addObject(Object* object);
    void takeObject(Object* object);

    /**
     * @brief Set the dimension of the viewport of this group.
     **/
    void setViewportDimensions(double x, double y, double width, double height);
    /**
     * @brief Set the dimension in the coordinate space of the parent.
     **/
    void setDimensions(double x, double y, double width, double height);
    /**
     * @brief Get the x offset in the coordinate system of this group
     * @return The x offset in the coordinate system of this group
     */
    double getXOffset() const;
    /**
     * @brief Get the y offset in the coordinate system of this group
     * @return The y offset in the coordinate system of this group
     */
    double getYOffset() const;
    /**
     * @brief Get the x scaling factor in the coordinate system of this group
     * @return The x scaling factor in the coordinate system of this group
     */
    double getXScale() const;
    /**
     * @brief Get the y scaling factor in the coordinate system of this group
     * @return The y scaling factor in the coordinate system of this group
     */
    double getYScale() const;
private:
    // no copy or assign
    GroupObject(const GroupObject&);
    GroupObject& operator=(const GroupObject&);

    class Private;
    Private* const d;

};

class DrawObject: public Object
{
public:

    enum {
        None = 0,
        Rectangle,
        RoundRectangle,
        Circle,
        Ellipse,
        Diamond,
        RightArrow,
        LeftArrow,
        UpArrow,
        DownArrow,
        IsoscelesTriangle,
        RightTriangle,
        Parallelogram,
        Trapezoid,
        Hexagon,
        Octagon,
        Line,
        Smiley,
        Heart,
        FreeLine,
        PictureFrame
    };

    DrawObject();
    virtual ~DrawObject();
    virtual bool isDrawing() const {
        return true;
    }

    unsigned shape() const;
    void setShape(unsigned s);

    bool isVerFlip() const;
    void setVerFlip(bool vFlip);
    bool isHorFlip() const;
    void setHorFlip(bool hFlip);
    /**
    * @brief Set this DrawObject's style name
    *
    * @param name Style name
    */
    void setStyleName(const QString &name);

    /**
    * @brief Get this DrawObject's style name
    *
    * @return Style name
    */
    QString styleName() const;

private:
    // no copy or assign
    DrawObject(const DrawObject&);
    DrawObject& operator=(const DrawObject&);

    class Private;
    Private* const d;
};

/**
* @brief A very simple class to hold basic information about fonts
*
*/
class TextFont
{
public:
    TextFont();
    TextFont(const TextFont &source);
    TextFont(const QString &fontName,
             int charset,
             int clipPrecision,
             int quality,
             int pitchAndFamily);

    ~TextFont();

    /**
    * @brief Get Font's family name
    * @return font's family name
    */
    QString name() const;

    /**
    * @brief Get font's charset
    * See http://msdn.microsoft.com/en-us/library/dd145037(VS.85).aspx
    * for more information
    * @return font's charset
    */
    int charset() const;

    /**
    * @brief Get font's clip precision
    * See http://msdn.microsoft.com/en-us/library/dd145037(VS.85).aspx
    * for more information
    * @return font's clip precision
    */
    int clipPrecision() const;

    /**
    * @brief Get font's quality
    * See http://msdn.microsoft.com/en-us/library/dd145037(VS.85).aspx
    * for more information
    * @return font's quality
    */
    int quality() const;

    /**
    * @brief Get font's pitch and family
    * See http://msdn.microsoft.com/en-us/library/dd145037(VS.85).aspx
    * for more information
    * @return font's pitch and family
    */
    int pitchAndFamily() const;
private:
    class Private;
    QSharedDataPointer<Private> d;
};

/**
* @brief Text Font Collection
*
* Simple class to hold TextFont information
*/
class TextFontCollection
{
public:
    TextFontCollection();
    ~TextFontCollection();

    /**
    * @brief Get the amount of fonts held by this collection
    * @return amount of fonts held by this collection
    */
    unsigned int listSize() const;

    /**
    * @brief add new font to the collection
    * @param font new font to add
    */
    void addFont(const TextFont &font);

    /**
    * @brief Get a specific font
    * @param index Index of the font to get
    * @return specified font
    */
    TextFont* getFont(unsigned int index);
private:
    class Private;
    Private* const d;
};

}

#endif /* LIBPPT_OBJECTS */
