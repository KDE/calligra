/*
 * This file is part of Office 2007 Filters for KOffice
 *
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

#ifndef PPTXXMLSLIDEREADER_H
#define PPTXXMLSLIDEREADER_H

#include <MsooXmlThemesReader.h>
#include <MsooXmlCommonReader.h>

#include <KoGenStyle.h>
#include <styles/KoCharacterStyle.h>

class KoXmlWriter;
class PptxXmlSlideReaderContext;
class PptxImport;
namespace MSOOXML
{
class MsooXmlRelationships;
}

//! Data structure collecting information about single slide's shape (p:sp)
class PptxShapeProperties
{
public:
    PptxShapeProperties();
    PptxShapeProperties(const PptxShapeProperties &other);
    PptxShapeProperties& operator=(const PptxShapeProperties &other);

    //! p:spPr/a:xfrm/a:off@x value in EMU
    int x;
    //! p:spPr/a:xfrm/a:off@y value in EMU
    int y;
    //! p:spPr/a:xfrm/a:ext@cx value in EMU
    int width;
    //! p:spPr/a:xfrm/a:ext@cy value in EMU
    int height;
    //! p:spPr/a:xfrm@rot value in 1/60,000th of a degree.
    int rot;
    //! p:nvPr/p:ph
    bool isPlaceHolder;
};

//! Data structure collecting information about single slide/master slide
class PptxSlideProperties
{
public:
    PptxSlideProperties();
    ~PptxSlideProperties();
    //! Shapes ordered by position
    QList<PptxShapeProperties*> shapes;
    QMap<QString, PptxShapeProperties*> shapesMap;

    void clear();
};

//! Properties of a single placeholder for shapes defined by layouts
class PptxPlaceholder
{
public:
    PptxPlaceholder();
    PptxPlaceholder(const PptxShapeProperties &other);
    ~PptxPlaceholder();
    //! Writers placeholder's attributes (svg:x, etc.) to @a writer
    void writeAttributes(KoXmlWriter* writer);
    QString x;
    QString y;
    QString width;
    QString height;
};

//! Data structure collecting information about single layout
class PptxSlideLayoutProperties
{
public:
    PptxSlideLayoutProperties();
    ~PptxSlideLayoutProperties();
    //! Shapes ordered by position
    QList<PptxShapeProperties*> shapes;
    //! Shapes map addressed by type
    QMap<QString, PptxShapeProperties*> shapesMap;
    //! placeholders objects are owned by this object
    QMap<QString, PptxPlaceholder*> placeholders;
    QString styleName;
    QMap<QString, KoGenStyle> styles;
};

//! Data structure collecting information about single text style for one list level defined by master slide
//! This can be bodyStyle, otherStyle, tileStyle
//! Used by PptxSlideMasterTextStyle
//! Attributes here are of type QVariant, so it is possible to test if these are present.
class PptxSlideMasterListLevelTextStyle
{
public:
    PptxSlideMasterListLevelTextStyle();
    QVariant sz;
};

//! Data structure collecting information about single text style defined by master slide
//! This can be bodyStyle, otherStyle, tileStyle
class PptxSlideMasterTextStyle
{
public:
    PptxSlideMasterTextStyle();
    ~PptxSlideMasterTextStyle();
    //! @return text style for list level @a level
    //! @par level can be 1..9, otherwise 0 is returned.
    //! Returned object is owned by PptxSlideMasterTextStyle.
    PptxSlideMasterListLevelTextStyle *listStyle(uint level);
private:
    QVector<PptxSlideMasterListLevelTextStyle*> m_listStyles;
};

//! Data structure collecting information about master slide
class PptxSlideMasterPageProperties
{
public:
    PptxSlideMasterPageProperties();
    void addDrawingPageProperty(const QByteArray& property, const QByteArray& value);
    void saveDrawingPageProperties(KoGenStyle* style);

    PptxSlideMasterTextStyle* textStyle(const QString& style);
    PptxSlideMasterTextStyle titleStyle;
    PptxSlideMasterTextStyle bodyStyle;
    PptxSlideMasterTextStyle otherStyle;
    
    //! Shapes map addressed by type
    QMap<QString, PptxShapeProperties*> shapes;

private:
    QMap<QByteArray, QByteArray> m_drawingPageProperties;
};

//! A class reading MSOOXML PPTX markup - ppt/slides/slide*.xml part.
class PptxXmlSlideReader : public MSOOXML::MsooXmlCommonReader
{
public:
    //! Mode of operation.
    enum Type {
        Slide,
        SlideLayout,
        SlideMaster
    };

    explicit PptxXmlSlideReader(KoOdfWriters *writers);

    virtual ~PptxXmlSlideReader();

    //! Reads/parses the XML.
    virtual KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0);

protected:

    KoFilter::ConversionStatus read_titleStyle();
    KoFilter::ConversionStatus read_bodyStyle();
    KoFilter::ConversionStatus read_otherStyle();
    KoFilter::ConversionStatus read_txStyles();
    KoFilter::ConversionStatus readInternal();
    KoFilter::ConversionStatus read_sld();
    KoFilter::ConversionStatus read_sldInternal();
    KoFilter::ConversionStatus read_sldMaster(); //!< For SlideMaster mode
    KoFilter::ConversionStatus read_sldLayout(); //!< For SlideLayout mode
    KoFilter::ConversionStatus read_cSld();
    KoFilter::ConversionStatus read_bg();
    KoFilter::ConversionStatus read_bgPr();
    KoFilter::ConversionStatus read_spTree();
    KoFilter::ConversionStatus read_nvPr();
    KoFilter::ConversionStatus read_ph();
    KoFilter::ConversionStatus read_txBody();
    KoFilter::ConversionStatus read_graphicFrame();
    KoFilter::ConversionStatus read_xfrm_p();
    KoFilter::ConversionStatus read_oleObj();

//    KoGenStyle m_currentPageStyle;
    PptxXmlSlideReaderContext* m_context;
    PptxShapeProperties* m_currentShapeProperties;

    KoGenStyle m_currentMasterPageStyle; //!< set by read_sp()
    KoGenStyle m_currentPresentationPageLayoutStyle; //!< set by read_sp() for placeholders (KoGenStyle::PresentationPageLayoutStyle)
    KoGenStyle m_currentPresentationStyle; //!< KoGenStyle::PresentationAutoStyle

    KoXmlWriter* m_placeholderElWriter;

private:
    void init();
    class Private;
    Private* const d;

#include <MsooXmlCommonReaderMethods.h>
#include <MsooXmlCommonReaderDrawingMLMethods.h>
};

//! Context for PptxXmlSlideReader
class PptxXmlSlideReaderContext : public MSOOXML::MsooXmlReaderContext
{
public:
    //! Creates the context object.
    /*! @param _slideProperties is written for SlideMaster type and read for Slide type. */
    PptxXmlSlideReaderContext(
        PptxImport& _import, const QString& _path, const QString& _file,
        uint _slideNumber,
        const QMap<QString, MSOOXML::DrawingMLTheme*>& _themes,
        PptxXmlSlideReader::Type _type, PptxSlideProperties* _slideProperties,
        PptxSlideLayoutProperties* _slideLayoutProperties,
        PptxSlideMasterPageProperties* _slideMasterPageProperties,
        MSOOXML::MsooXmlRelationships& _relationships,
        QMap<int, QString> _commentsAuthors);
    PptxImport* import;
    const QString path;
    const QString file;
    const uint slideNumber;
    const QMap<QString, MSOOXML::DrawingMLTheme*>* themes;
    PptxXmlSlideReader::Type type;
    PptxSlideProperties* slideProperties;
    PptxSlideLayoutProperties* slideLayoutProperties;
    PptxSlideMasterPageProperties* slideMasterPageProperties;
    QString pageDrawStyleName; //!< written in read_sldInternal()
    QMap<int, QString> commentsAuthors;
};

#endif
