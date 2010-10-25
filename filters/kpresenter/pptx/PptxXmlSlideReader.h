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

class KoXmlWriter;
class PptxXmlSlideReaderContext;
class PptxImport;
namespace MSOOXML
{
class MsooXmlRelationships;
class TableStyleList;
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

//! Styles valid for current slide
class PptxActualSlideProperties
{
public:
    //! Map of paragraph-styles with the styleId as outer-key and the listlevel as inner-key.
    QMap<QString, QMap<int,KoGenStyle> > styles;

    //! Map of text-styles with the styleId as outer-key and the listlevel as inner-key.
    QMap<QString, QMap<int,KoGenStyle> > textStyles;

    //! Map of list-styles with the styleId as outer-key and the listlevel as inner-key.
    QMap<QString, QMap<int,MSOOXML::Utils::ParagraphBulletProperties> > listStyles;
};

//! Data structure collecting information about single slide/master slide
class PptxSlideProperties
{
public:
    PptxSlideProperties();
    ~PptxSlideProperties();
    //! Shapes ordered by position
    QList<PptxShapeProperties*> shapes;
    //! Shapes map addressed by type
    QMap<QString, PptxShapeProperties*> shapesMap;
    //! Clear the shapes and shapesMap lists.
    void clear();
private:
    Q_DISABLE_COPY(PptxSlideProperties)
};

//! Properties of a single placeholder for shapes defined by layouts
class PptxPlaceholder
{
public:
    PptxPlaceholder();
    PptxPlaceholder(const PptxShapeProperties &other);
    ~PptxPlaceholder();
    int x;
    int y;
    int width;
    int height;
    int rot;
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
    //! The presentation:presentation-page-layout-name
    QString pageLayoutStyleName;
    //! Map of paragraph-styles with the styleId as outer-key and the listlevel as inner-key.
    QMap<QString, QMap<int,KoGenStyle> > styles;

    //! Map of text-styles with the styleId as outer-key and the listlevel as inner-key.
    QMap<QString, QMap<int,KoGenStyle> > textStyles;

    //! Map of list-styles with the styleId as outer-key and the listlevel as inner-key.
    QMap<QString, QMap<int,MSOOXML::Utils::ParagraphBulletProperties> > listStyles;

    //! Position of the text
    QMap<QString, QString> textShapePositions;

    //! Borders in the frame
    QMap<QString, QString> textLeftBorders;
    QMap<QString, QString> textRightBorders;
    QMap<QString, QString> textBottomBorders;
    QMap<QString, QString> textTopBorders;

    // Extras frames such as pictures from layout, which should be put to the slide.
    QVector<QString> layoutFrames;

    KoGenStyle m_drawingPageProperties;

    // Name of the slidemaster to be used with this layout
    QString m_slideMasterName;
private:

    Q_DISABLE_COPY(PptxSlideLayoutProperties)
};

//! Data structure collecting information about master slide
class PptxSlideMasterPageProperties
{
public:
    PptxSlideMasterPageProperties();
    void clear();

    QMap<QString, QString> colorMap;

    //! Map of paragraph-styles with the styleId as outer-key and the listlevel as inner-key.
    QMap<QString, QMap<int,KoGenStyle> > styles;

    //! Map of text-styles with the styleId as outer-key and the listlevel as inner-key.
    QMap<QString, QMap<int,KoGenStyle> > textStyles;

    //! Map of list-styles with the styleId as outer-key and the listlevel as inner-key.
    QMap<QString, QMap<int,MSOOXML::Utils::ParagraphBulletProperties> > listStyles;

    //! Position of the text
    QMap<QString, QString> textShapePositions;

    //! Borders in the frame
    QMap<QString, QString> textLeftBorders;
    QMap<QString, QString> textRightBorders;
    QMap<QString, QString> textBottomBorders;
    QMap<QString, QString> textTopBorders;

    // Them to which this masterslide refers to.
    MSOOXML::DrawingMLTheme theme;

    KoGenStyle m_drawingPageProperties;
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
    KoFilter::ConversionStatus read_bgRef();
    KoFilter::ConversionStatus read_spTree();
    KoFilter::ConversionStatus read_nvPr();
    KoFilter::ConversionStatus read_ph();
    KoFilter::ConversionStatus read_txBody();
    KoFilter::ConversionStatus read_graphicFrame();
    KoFilter::ConversionStatus read_nvGraphicFramePr();

    KoFilter::ConversionStatus read_xfrm_p();
    KoFilter::ConversionStatus read_cNvPr_p();
    KoFilter::ConversionStatus read_oleObj();

    KoFilter::ConversionStatus read_clrMap();
    KoFilter::ConversionStatus read_clrMapOvr();

//    KoGenStyle m_currentPageStyle;
    PptxXmlSlideReaderContext* m_context;
    PptxShapeProperties* m_currentShapeProperties;

    KoGenStyle m_currentMasterPageStyle; //!< set by read_sp()
    KoGenStyle m_currentPresentationPageLayoutStyle; //!< set by read_sp() for placeholders (KoGenStyle::PresentationPageLayoutStyle)
    KoGenStyle m_currentPresentationStyle; //!< KoGenStyle::PresentationAutoStyle

    KoXmlWriter* m_placeholderElWriter;

    // Saves current text and paragraph styles to slide(layout/master) memory where they
    // can be used later for inheritance purposes
    void saveCurrentStyles();

    void saveCurrentListStyles();

    // Copies 9 lvls of text and paragraph styles to current styles
    void inheritAllTextAndParagraphStyles();

    // Inherits liststyle to m_currentCombinedBulletProperties
    void inheritListStyles();

    // Inherits default styles to m_currentCombinedBulletProperties
    void inheritDefaultListStyles();

    // inherits default text style to target
    void inheritDefaultTextStyle(KoGenStyle& targetStyle);

    // Inherits correct textstyle to m_currentTextStyle
    // First from default, then from master, layout, slide using those lvls which are needed
    void inheritTextStyle(KoGenStyle& targetStyle);

    // Inherits default paragraphStyle
    void inheritDefaultParagraphStyle(KoGenStyle& targetStyle);

    // Inherits correct paragraph styles to m_currentParagraphStyle
    void inheritParagraphStyle(KoGenStyle& targetStyle);

    // Inherits default body properties
    void inheritDefaultBodyProperties();

    // Inherit correct body properties from slideMaster/slideLayout if needed
    void inheritBodyProperties();

    // Saves current body properties
    void saveBodyProperties();

private:
    void init();
    class Private;
    Private* const d;

#include <MsooXmlCommonReaderMethods.h>
#include <MsooXmlCommonReaderDrawingMLMethods.h>
#include <MsooXmlDrawingReaderTableMethods.h>
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
        MSOOXML::DrawingMLTheme* _themes,
        PptxXmlSlideReader::Type _type, PptxSlideProperties* _slideProperties,
        PptxSlideLayoutProperties* _slideLayoutProperties,
        PptxSlideMasterPageProperties* _slideMasterPageProperties,
        MSOOXML::MsooXmlRelationships& _relationships,
        QMap<int, QString> _commentAuthors,
        MSOOXML::TableStyleList *tableStyleList,
        QMap<QString, QString> masterColorMap,
        QMap<QString, QString> _oleReplacements);

    PptxImport* import;
    const QString path;
    const QString file;
    const uint slideNumber;
    MSOOXML::DrawingMLTheme* themes;
    PptxXmlSlideReader::Type type;
    PptxSlideProperties* slideProperties;
    PptxSlideLayoutProperties* slideLayoutProperties;
    PptxSlideMasterPageProperties* slideMasterPageProperties;
    PptxActualSlideProperties currentSlideStyles;

    // There could potentially be multiple of these...todo
    QString pageDrawStyleName; //!< written in read_sldInternal()
    QVector<QString> pageFrames; //! Frames which go to masterslide
    QMap<int, QString> commentAuthors;
    MSOOXML::TableStyleList *tableStyleList;

    // This value is always initialized with values from master slide, but it is possible
    // that slide/layout override it with custom map
    QMap<QString, QString> colorMap;

    QMap<QString, QString> oleReplacements;

    // Used to keep track, whether we should skip elements
    // currently we need to read some slides twice
    // This because some elements from the later part of the document are needed
    // to fully understand cSld element
    bool firstReadingRound;

    // These have to be in context, because each slide/layout/master may define their own colormap
    // therefore the way default text is interpreted cannot be static
    QVector<KoGenStyle> defaultTextStyles;
    QVector<KoGenStyle> defaultParagraphStyles;
    QVector<MSOOXML::Utils::ParagraphBulletProperties> defaultListStyles;
};

#endif
