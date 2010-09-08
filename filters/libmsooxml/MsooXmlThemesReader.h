/*
 * This file is part of Office 2007 Filters for KOffice
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MSOOXMLTHEMESREADER_H
#define MSOOXMLTHEMESREADER_H

#include <QHash>
#include <QColor>
#include <QMap>

#include <KoGenStyles.h>

#include "MsooXmlCommonReader.h"
#include "MsooXmlImport.h"

namespace MSOOXML
{

class DrawingMLColorSchemeItem;
class DrawingMLColorSchemeSystemItem;

//! @todo add other classes inheriting DrawingMLColorSchemeItemBase
class MSOOXML_EXPORT DrawingMLColorSchemeItemBase
{
public:
    DrawingMLColorSchemeItemBase();
    virtual ~DrawingMLColorSchemeItemBase();
    DrawingMLColorSchemeItem* toColorItem();
    DrawingMLColorSchemeSystemItem* toSystemItem();
    virtual QColor value() const = 0;
};

class MSOOXML_EXPORT DrawingMLColorSchemeItem : public DrawingMLColorSchemeItemBase
{
public:
    DrawingMLColorSchemeItem();
    virtual QColor value() const { return color; }
    QColor color;
};

class MSOOXML_EXPORT DrawingMLColorSchemeSystemItem : public DrawingMLColorSchemeItemBase
{
public:
    DrawingMLColorSchemeSystemItem(bool spreadMode);
    virtual QColor value() const;

    QColor lastColor;
    QString systemColor; //!< ST_SystemColorVal (§20.1.10.58).
    bool spreadsheetMode;
};

typedef QHash<QString, DrawingMLColorSchemeItemBase*> DrawingMLColorSchemeItemHash;

//! Implements color scheme, based on hash. All items are owned by this object.
class MSOOXML_EXPORT DrawingMLColorScheme : public DrawingMLColorSchemeItemHash
{
public:
    DrawingMLColorScheme();
    ~DrawingMLColorScheme();

    DrawingMLColorSchemeItemBase* value(const QString& name) const { return DrawingMLColorSchemeItemHash::value(name); }

    /*! @return color value for index. Needed because while PPTX uses lookup by name: value(QString&),
                XLSX uses lookup by index. When index is invalid, 0 is returned. */
    DrawingMLColorSchemeItemBase* value(int index) const;

    //! Name of the color scheme
    QString name;
};

//! Font set for majorFont and minorFont.
//! @todo add more support for latin, ea and cs: charser, panose, pitchfamily attributes (21.1.2.3.3)
class MSOOXML_EXPORT DrawingMLFontSet
{
public:
    DrawingMLFontSet();
    //! A (script->typeface) hash with font definitions (20.1.4.1.16.)
    QHash<QString, QString> typefacesForScripts;

    //! Specifies that a Latin font be used for a specific run of text.
    QString latinTypeface;

    //! The possible values for this attribute are defined by the ST_TextTypeface simple type
    //! (§20.1.10.81).
    QString eaTypeface;
    //! The possible values for this attribute are defined by the ST_TextTypeface simple type
    //! (§20.1.10.81).
    QString csTypeface;
};

//! Defines the font scheme within the theme
//! The font scheme consists of a pair of major and minor fonts for which to use in a document.s
class MSOOXML_EXPORT DrawingMLFontScheme
{
public:
    DrawingMLFontScheme();
    DrawingMLFontSet majorFonts;
    DrawingMLFontSet minorFonts;
    QString name;
};


class MSOOXML_EXPORT DrawingMLFillBase
{
public:
    virtual ~DrawingMLFillBase();
    // This function will create the fill style and fill the approriate styles
    // and filePath if needed.
    // Number is used to index to correct style, color is the color which should be used when making the styles
    virtual void writeStyles(KoGenStyles& styles, KoGenStyle *graphicStyle, KoGenStyle *drawStyle, QColor color) = 0;
};

class MSOOXML_EXPORT DrawingMLSolidFill : public DrawingMLFillBase
{
public:
    void writeStyles(KoGenStyles& styles, KoGenStyle *graphicStyle, KoGenStyle *drawStyle, QColor color);
};

class MSOOXML_EXPORT DrawingMLBlipFill : public DrawingMLFillBase
{
public:
    DrawingMLBlipFill(QString filePath);
    void writeStyles(KoGenStyles& styles, KoGenStyle *graphicStyle, KoGenStyle *drawStyle, QColor color);
private:
    QString m_filePath;
};

class MSOOXML_EXPORT DrawingMLFormatScheme
{
public:

    ~DrawingMLFormatScheme();
    QString name;

    QMap<int, DrawingMLFillBase*> fillStyles;
};

//! Defines a single DrawingML theme.
//! @todo support objectDefaults and extraClrSchemeLst
class MSOOXML_EXPORT DrawingMLTheme
{
public:
    DrawingMLTheme();
    QString name;
    DrawingMLColorScheme colorScheme;
    DrawingMLFontScheme fontScheme;
    DrawingMLFormatScheme formatScheme;
};

//! Context for MsooXmlThemesReader::read()
class MSOOXML_EXPORT MsooXmlThemesReaderContext : public MsooXmlReaderContext
{
public:
    MsooXmlThemesReaderContext(DrawingMLTheme& t, MSOOXML::MsooXmlRelationships* rel, MSOOXML::MsooXmlImport* imp,
        QString pathName, QString fileName);
    DrawingMLTheme * const theme;
    bool spreadMode;
    MSOOXML::MsooXmlRelationships* relationships;
    MSOOXML::MsooXmlImport* import;
    QString path;
    QString file;
};

//! A class reading MSOOXML themes markup - theme/theme1.xml.
/*! @todo generalize for other MSOOXML subformats.
 */
class MSOOXML_EXPORT MsooXmlThemesReader : public MSOOXML::MsooXmlCommonReader
{
public:
    //! Creates MsooXmlThemesReader object.
    //! On successful reading, @a theme will be written with theme definition.
    MsooXmlThemesReader(KoOdfWriters *writers);

    virtual ~MsooXmlThemesReader();

    //! Reads/parses the file. The output goes mainly to KoGenStyles* KoOdfWriters::mainStyles
    virtual KoFilter::ConversionStatus read(MsooXmlReaderContext* context = 0);

protected:
    KoFilter::ConversionStatus readInternal();
    KoFilter::ConversionStatus read_theme();
    KoFilter::ConversionStatus read_themeElements();
    //! @todo no CASE
    KoFilter::ConversionStatus read_objectDefaults();
    KoFilter::ConversionStatus read_custClrLst();
    KoFilter::ConversionStatus read_extLst();
    KoFilter::ConversionStatus read_extraClrSchemeLst();
    KoFilter::ConversionStatus read_extraClrScheme();
    KoFilter::ConversionStatus read_clrScheme();
    KoFilter::ConversionStatus read_color(); //!< helper
    KoFilter::ConversionStatus read_srgbClr();
    KoFilter::ConversionStatus read_sysClr();
    DrawingMLColorSchemeItemBase* m_currentColor; //!< used by *Clr()
    KoFilter::ConversionStatus read_fmtScheme();
    KoFilter::ConversionStatus read_fontScheme();
    KoFilter::ConversionStatus read_clrMap();
    KoFilter::ConversionStatus read_bgFillStyleLst();

    //! Used for skipping a subtree - kust reads and shows each element.
    //! called by BIND_READ_SKIP() macro.
    KoFilter::ConversionStatus read_SKIP();

    //! Used for skipping a subtree - kust reads and shows each element.
    //! called by BIND_READ_SKIP() macro.
    KoFilter::ConversionStatus read_SKIP(QColor&);

private:
    void init();

    MsooXmlThemesReaderContext* m_context;

    typedef KoFilter::ConversionStatus(MsooXmlThemesReader::*ReadMethod)();
    QHash<QString, ReadMethod> m_readMethods;
    QHash<QString, QString> m_colorSchemeIndices;
    bool m_clrScheme_initialized;
    bool m_color_initialized;
    bool m_spreadMode;

    MSOOXML::MsooXmlRelationships* m_relationships;
    MSOOXML::MsooXmlImport* m_import;
    QString m_path;
    QString m_file;
};

} // namespace MSOOXML

#endif //MSOOXMLTHEMESREADER_H
