/*
 * This file is part of Office 2007 Filters for KOffice
 * Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>
 * Copyright (c) 2003 Lukas Tinkl <lukas@kde.org>
 * Copyright (C) 2003 David Faure <faure@kde.org>
 * Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef MSOOXML_UTILS_H
#define MSOOXML_UTILS_H

#include "msooxml_export.h"

#include <QColor>
#include <QBuffer>
#include <KoFilterChain.h>
#include <KoXmlReader.h>
#include <KDebug>
#include <KoGenStyle.h>

class QLocale;
class QDomElement;
class QDomDocument;
class QSize;
class KZip;
struct KoOdfWriters;
class KoCharacterStyle;
class KoStyleStack;
class KoXmlWriter;

//! Returns from the current block if the result of @a call is not equal to KoFilter::OK
#define RETURN_IF_ERROR( call ) \
    { \
        const KoFilter::ConversionStatus result = call; \
        if (result != KoFilter::OK) \
            return result; \
    }

//! Common utilities for handling MSOOXML formats
namespace MSOOXML
{

class MsooXmlReader;
class MsooXmlReaderContext;

namespace Utils {

class ParagraphBulletProperties
{
public:

    ParagraphBulletProperties();

    void clear();

    QString convertToListProperties() const;

    bool isEmpty() const;

    enum ParagraphBulletType {BulletType, NumberType};
    ParagraphBulletType m_type;
    QString m_bulletChar;
    QString m_numbering;
    int m_level;
    int m_startValue;
};

//! Container autodeleter. Works for QList, QHash and QMap.
//! @todo move to more generic place
template <typename T>
class ContainerDeleter
{
public:
    ContainerDeleter(T& container) : m_container(&container) {}
    ~ContainerDeleter() {
        qDeleteAll(*m_container); m_container->clear();
    }
private:
    T* const m_container;
};

//! Helper that sets given variable to specified value on destruction
//! Object of type Setter are supposed to be created on the stack.
//! @todo Copied from koffice/kexi/kexiutils/utils.h; replace with a shared code
template <typename T>
class Setter
{
public:
    //! Creates a new setter object for variable @a var,
    //! which will be set to value @a val on setter's destruction.
    Setter(T* var, const T& val)
            : m_var(var), m_value(val) {
    }
    ~Setter() {
        if (m_var)
            *m_var = m_value;
    }
    //! Clears the assignment, so the setter
    //! will not alter the variable on destruction
    void clear() {
        m_var = 0;
    }
private:
    T* m_var;
    const T m_value;
};

//! Helper that works like the @ref Setter class but also has behaviour of std::auto_ptr.
//! When std::auto_ptr is used, the pointer of type T* is not set back to 0 on desctruction.
//! @todo replace with a shared code
template <typename T>
class AutoPtrSetter
{
public:
    //! Creates a new auto-ptr setter object for variable pointer of type T* @a ptr,
    //! which will be set to 0 setter's destruction, unless release() was called.
    explicit AutoPtrSetter(T* ptr)
            : m_pptr(&ptr) {
    }
    ~AutoPtrSetter() {
        if (m_pptr && *m_pptr) {
            delete *m_pptr;
            *m_pptr = 0;
        }
    }
    //! Bypasses the smart pointer, and returns it, so on destruction
    //! of the AutoPtrSetter object the pointed object will not be deleted
    //! (so it is the behaviour like std::auto__ptr::release())
    //! but also the pointer of type T* will not be cleared.
    T* release() {
        T* p = m_pptr ? *m_pptr : 0;
        m_pptr = 0;
        return p;
    }
private:
    T** m_pptr;
};

//! Copies properties from one KoGenStyle to another
MSOOXML_EXPORT void copyPropertiesFromStyle(const KoGenStyle& sourceStyle, KoGenStyle& targetStyle, KoGenStyle::PropertyType type);

//! Decodes boolean attribute @a value. If unspecified returns @a defaultValue.
//! @return true unless @a value is equal to "false", "off" or "0".
MSOOXML_EXPORT bool convertBooleanAttr(const QString& value, bool defaultValue = false);

//! Loads content types from "[Content_Types].xml"
/*! Based on information from ECMA-376, Part 1: "11.2 Package Structure".
 @return status: KoFilter::OK on success or KoFilter::WrongFormat when any unexpected and critical incompatibility occurs.
*/
//! @todo create whole class keeping the data
MSOOXML_EXPORT KoFilter::ConversionStatus loadContentTypes(const KoXmlDocument& contentTypesXML,
        QMultiHash<QByteArray, QByteArray>& contentTypes);

//! @return device for file @a fileName of @a zip archive. Status @a status is written on error.
//! The device is already opened for reading and should be deleted after use.
MSOOXML_EXPORT QIODevice* openDeviceForFile(const KZip* zip,
        QString& errorMessage,
        const QString& fileName,
        KoFilter::ConversionStatus& status);

//! QXmlStreamReader-based generic loading/parsing into @a doc KoXmlDocument
MSOOXML_EXPORT KoFilter::ConversionStatus loadAndParse(QIODevice* io, KoXmlDocument& doc,
        QString& errorMessage, const QString & fileName);

//! @see KoOdfReadStore::loadAndParse(QIODevice* fileDevice, KoXmlDocument& doc, QString& errorMessage, const QString& fileName)
MSOOXML_EXPORT KoFilter::ConversionStatus loadAndParse(KoXmlDocument& doc,
        const KZip* zip,
        QString& errorMessage,
        const QString& fileName);

//! QXmlStreamReader-based loading/parsing for document.xml
MSOOXML_EXPORT KoFilter::ConversionStatus loadAndParseDocument(MsooXmlReader* reader,
        const KZip* zip,
        KoOdfWriters* writers,
        QString& errorMessage,
        const QString& fileName,
        MsooXmlReaderContext* context = 0);

/*! Copies file @a sourceName from zip archive @a zip to @a outputStore store
 under @a destinationName name. If @a size is not 0, *size is set to size of the image
 @return KoFilter::OK on success.
 On failure @a errorMessage is set. */
KoFilter::ConversionStatus copyFile(const KZip* zip, QString& errorMessage,
                                    const QString& sourceName,
                                    KoStore *outputStore,
                                    const QString& destinationName, bool oleType=false);

/*! @return size of image file @a sourceName read from zip archive @a zip.
 Size of the image is returned in @a size.
 @return KoFilter::OK on success.
 On failure @a errorMessage is set. */
KoFilter::ConversionStatus imageSize(const KZip* zip, QString& errorMessage,
                                     const QString& sourceName, QSize* size);

//! Loads a thumbnail.
/*! @return conversion status
    @todo Thumbnails are apparently used only by PowerPoint or templates for now.
          Implement it, for now this feature is not needed for docx. */
MSOOXML_EXPORT KoFilter::ConversionStatus loadThumbnail(QImage& thumbnail, KZip* zip);

// -- conversions ---

//! Handles ST_Lang value @a value (Language Reference) (SharedML, 22.9.2.6)
/*! The value specifies that its contents contains a language identifier as defined by RFC 4646/BCP 47.
    Sets up @a language and @a country based on @a value that is of format {langugage}-{country}
    @return true on success. */
MSOOXML_EXPORT bool ST_Lang_to_languageAndCountry(const QString& value, QString& language, QString& country);

//! @return QColor value for ST_HexColorRGB (Hexadecimal Color Value) (SharedML, 22.9.2.5)
//!         or invalid QColor if @a color is not in the expected format.
//! @par val color value in RRGGBB hexadecimal format
inline QColor ST_HexColorRGB_to_QColor(const QString& color)
{
    if (color.length() != 6)
        return QColor();
    bool ok;
    const uint rgb = color.toUInt(&ok, 16);
    return ok ? QColor(QRgb(rgb)) : QColor(); // alpha ignored
//    return QColor(QRgb(0xff000000 | color.toInt(0, 16)));
}

//! @return QBrush value for ST_HighlightColor
//! The brush is built out of solid color.
//! If colorName is not supported by the standard, QBrush() is returned.
//! @par colorName named text highlight color like "black", "blue" (17.18.40)
MSOOXML_EXPORT QBrush ST_HighlightColor_to_QColor(const QString& colorName);

//! @return QColor value for DefaultIndexColor
MSOOXML_EXPORT QColor defaultIndexedColor( int index );

//! @return QLocale for the give language id
MSOOXML_EXPORT QLocale localeForLangId( int langid );

//! Converts value for 22.9.2.9 ST_Percentage (Percentage Value with Sign) from string
//! Sets @arg ok to true on success.
MSOOXML_EXPORT qreal ST_Percentage_to_double(const QString& val, bool& ok);

//! Converts value for 22.9.2.9 ST_Percentage (Percentage Value with Sign) from string
//! If "%" suffix is not present (MSOOXML violation of OOXML), the format is expected to be int({ST_Percentage}*1000).
//! Sets @arg ok to true on success.
MSOOXML_EXPORT qreal ST_Percentage_withMsooxmlFix_to_double(const QString& val, bool& ok);

struct MSOOXML_EXPORT DoubleModifier {
    DoubleModifier(qreal v) : value(v), valid(true) {}
    DoubleModifier() : value(0.0), valid(false) {}
    qreal value;
    bool valid;
};

//! Converts color string to rgb color string #xxYYZZ
MSOOXML_EXPORT QString rgbColor(const QString& color);

MSOOXML_EXPORT QColor colorForLuminance(const QColor& color,
    const DoubleModifier& modulation, const DoubleModifier& offset);

MSOOXML_EXPORT void modifyColor(QColor& color, qreal tint, qreal shade, qreal satMod);

//! Converts shape types from ECMA-376 to ODF.
/*! @return "Common Presentation Shape Attribute" value (ODF 1.1., 9.6.1)
            for presentation shape converted from ECMA-376 19.7.10
            ST_PlaceholderType (Placeholder ID) value, p. 2987.
    @param ecmaType ECMA-376 shape type
    The conversion is useful e.g. for presentation:class attribute of draw:frame
    out of ECMA-376's ph@type attribute.
    By default (and for empty argument), "outline" is returned.
*/
//! @todo or "object"?  ST_PlaceholderType docs day the default is "obj".
//! CASE #P500
MSOOXML_EXPORT QString ST_PlaceholderType_to_ODF(const QString& ecmaType);

//! Sets up @a textStyleProperties with underline style matching MSOOXML name @a msooxmlName.
//! Based on 17.18.99 ST_Underline (Underline Patterns), WML ECMA-376 p.1681
//! and on 20.1.10.82 ST_TextUnderlineType (Text Underline Types), DrawingML ECMA-376 p.3450 (merged)
MSOOXML_EXPORT void setupUnderLineStyle(const QString& msooxmlName, KoCharacterStyle* textStyleProperties);

//! @return the symbolic name of column @a column (counted from 0)
//! This is similar to the notation of spreadsheet's column, e.g. 0th column is "A", 1st is "B", 26th is "AA".
MSOOXML_EXPORT QString columnName(uint column);

//! Splits @a pathAndFile into path and file parts. Path does not end with '/'.
MSOOXML_EXPORT void splitPathAndFile(const QString& pathAndFile, QString* path, QString* file);

//! Returns calculated angle and xDiff, yDiff, caller has to apply these to style
MSOOXML_EXPORT void rotateString(const qreal rotation, const qreal width, const qreal height, qreal& angle, qreal& xDiff, qreal& yDiff,
    bool flipH, bool flipV);

//! A helper allowing to buffer xml streams and writing them back later
/*! This class is useful when information that has to be written in advance is based
    on XML elements parsed later. In such case the information cannot be saved in one pass.
    Example of this is paragraphs style name: is should be written to style:name attribute but
    relevant XML elements (that we use for building the style) are appearing later.
    So we first output created XML to a buffer, then save the parent element with the style name
    and use KoXmlWriter::addCompleteElement() to redirect the buffer contents as a subelement.

     Example use:
     @code
     KoXmlWriter *body = ...;
     XmlWriteBuffer buf;
     body = buf.setWriter(body);
     // ...
     // buf.originalWriter() can be used here ...
     // ...
     // Use the new buffered body writer here, e.g.:
     body->startElement("text:span", false);
     body->addAttribute("text:style-name", currentTextStyleName);
     body->addTextSpan(text);
     body->endElement();
     // We are done with the buffered body writer, now release it and restore the original body writer.
     // This inserts all the XML buffered by buf into the original body writer
     // (internally using KoXmlWriter::addCompleteElement()).
     body = buf.releaseWriter();
     @endcode */
class MSOOXML_EXPORT XmlWriteBuffer
{
public:
    //! Constructor; no writer is set initially.
    XmlWriteBuffer();

    //! Destructor, releases writer if there is any set.
    ~XmlWriteBuffer();

    //! Assigns writer @a writer to this buffer.
    /*! From now any output directed to @a writer is written to a buffer instead.
     Use releaseWriter() to write the changes back through the original writer.
     @return the newly created writer, which usually should be assigned
             to the variable passed as @a writer. */
    KoXmlWriter* setWriter(KoXmlWriter* writer);

    //! Releases the original writer set before using setWriter(KoXmlWriter*&).
    /*! This inserts all the XML buffered by buffer into the original body writer passed in setWriter()
     (internally using KoXmlWriter::addCompleteElement()).
     @return the original writer set in setWriter();
             this writer usually should be assigned back to the variable
             altered by the recent use of setWriter(). */
    KoXmlWriter* releaseWriter();

    //! @return the original writer set in setWriter(). Does not change the state of the buffer.
    /*! Use this method when you need to access the remembered writer without releasing it. */
    KoXmlWriter* originalWriter() const {
        return m_origWriter;
    }

    //! Clears this buffer without performing any output to the writer.
    void clear();
private:
    //! Internal, used in releaseWriter() and the destructor; Does not assert when there's nothing to release.
    KoXmlWriter* releaseWriterInternal();

    QBuffer m_buffer;
    KoXmlWriter* m_origWriter;
    KoXmlWriter* m_newWriter;
};

} // Utils namespace

} // MSOOXML namespace

#endif /* MSOOXML_UTILS_H */
