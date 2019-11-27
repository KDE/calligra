/*
 * This file is part of Office 2007 Filters for Calligra
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
#include <MsooXmlDrawingTableStyle.h>
#include <VmlDrawingReader.h>
#include <KoGenStyle.h>
#include <KoTable.h>

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

//! Properties of a single placeholder for shapes defined by layouts
class PptxPlaceholder
{
public:
    PptxPlaceholder();
    explicit PptxPlaceholder(const PptxShapeProperties &other);
    ~PptxPlaceholder();
    int x;
    int y;
    int width;
    int height;
    int rot;
};

//! Data structure collecting information about single layout/master
class PptxSlideProperties
{
public:
    PptxSlideProperties();
    ~PptxSlideProperties();

    //! geometry type
    QMap<QString, QString> contentTypeMap;

    //! geometry type
    QMap<QString, QString> contentEquations;

    //! geometry type
    QMap<QString, QString> contentPath;

    //! The presentation:presentation-page-layout-name
    QString pageLayoutStyleName;
    //! Map of paragraph-styles with the styleId as outer-key and the listlevel as inner-key.
    QMap<QString, QMap<int,KoGenStyle> > styles;

    //! Map of text-styles with the styleId as outer-key and the listlevel as inner-key.
    QMap<QString, QMap<int,KoGenStyle> > textStyles;

    //! Map of list-styles with the styleId as outer-key and the listlevel as inner-key.
    QMap<QString, QMap<int,MSOOXML::Utils::ParagraphBulletProperties> > listStyles;

    //! Map of spPr based graphicStyles, these contain fillColor & outline
    QMap<QString, KoGenStyle> graphicStyles;

    //! Position of the text
    QMap<QString, QString> textShapePositions;

    //! Possible text shrinkage
    QMap<QString, MSOOXML::Utils::autoFitStatus> m_textAutoFit;

    //! Borders in the frame
    QMap<QString, QString> textLeftBorders;
    QMap<QString, QString> textRightBorders;
    QMap<QString, QString> textBottomBorders;
    QMap<QString, QString> textTopBorders;

    KoGenStyle m_drawingPageProperties;

    // Layout specific

    // Extras frames such as pictures from layout, which should be put to the slide.
    QVector<QString> layoutFrames;

    // Name of the slidemaster to be used with this layout
    QString m_slideMasterName;

    // Master specific
    MSOOXML::DrawingMLTheme theme;

    //! Shapes map addressed by type
    QMap<QString, PptxShapeProperties*> shapesMap;

    QMap<QString, QString> colorMap;

    // Temporary attribute providing the info that the color mapping changed
    // compared to the slideMaster.
    bool overrideClrMapping;

private:
};

//! A class reading MSOOXML PPTX markup - ppt/slides/slide*.xml part.
class PptxXmlSlideReader : public MSOOXML::MsooXmlCommonReader
{
public:
    //! Mode of operation.
    enum Type {
        Slide,
        SlideLayout,
        SlideMaster,
        NotesMaster,
        Notes
    };

    explicit PptxXmlSlideReader(KoOdfWriters *writers);

    ~PptxXmlSlideReader() override;

    //! Reads/parses the XML.
    KoFilter::ConversionStatus read(MSOOXML::MsooXmlReaderContext* context = 0) override;

protected:

    KoFilter::ConversionStatus read_titleStyle();
    KoFilter::ConversionStatus read_notesStyle();
    KoFilter::ConversionStatus read_bodyStyle();
    KoFilter::ConversionStatus read_otherStyle();
    KoFilter::ConversionStatus read_txStyles();
    KoFilter::ConversionStatus readInternal();
    KoFilter::ConversionStatus read_sld();
    KoFilter::ConversionStatus read_notes();
    KoFilter::ConversionStatus read_sldInternal();
    KoFilter::ConversionStatus read_notesMaster();
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
    KoFilter::ConversionStatus read_controls();
    KoFilter::ConversionStatus read_control();

    KoFilter::ConversionStatus read_xfrm_p();
    KoFilter::ConversionStatus read_cNvPr_p();
    KoFilter::ConversionStatus read_oleObj();

    KoFilter::ConversionStatus read_clrMap();
    KoFilter::ConversionStatus read_clrMapOvr();

    PptxXmlSlideReaderContext* m_context;
    PptxShapeProperties* m_currentShapeProperties;

    KoGenStyle m_currentMasterPageStyle; //!< set by read_sp()
    KoGenStyle m_currentPresentationPageLayoutStyle; //!< set by read_sp() for placeholders (KoGenStyle::PresentationPageLayoutStyle)
    KoGenStyle m_currentPresentationStyle; //!< KoGenStyle::PresentationAutoStyle

    KoXmlWriter* m_placeholderElWriter;

    // Saves current text and paragraph styles to slide(layout/master) memory where they
    // can be used later for inheritance purposes
    void saveCurrentStyles();

    // Saves current list styles
    void saveCurrentListStyles();

    // Saves outline & fill style
    void saveCurrentGraphicStyles();

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

    // Inherit correct body properties from slideMaster/slideLayout if needed
    void inheritBodyProperties();

    // Saves current body properties
    void saveBodyProperties();

    // Saves current shape position etc. so that they can be used by slide/slideLayout
    KoFilter::ConversionStatus generatePlaceHolderSp();

    // Inherits shape x, y, width etc. from slideLayout/slideMaster
    void inheritShapePosition();

    // Inherits shapes geometry, fill color etc. from slideLayout/slideMaster
    void inheritShapeGeometry();

    /**
     * MS PowerPoint specific calculation of the paragraph margin.
     * @param spcAft/spcBef (paragraph spacing) as provided in pptx [%]
     * @param minimum font size detected in the paragraph [point]
     * @return paragraph margin [point]
     */
    qreal processParagraphSpacing(const qreal margin, const qreal fontSize);

private:

    void saveBodyPropertiesHelper(const QString &id, PptxSlideProperties* slideProperties);
    void inheritBodyPropertiesHelper(const QString &id, PptxSlideProperties* slideProperties);

    void init();

    class Private;
    Private* const d;

    bool m_showSlideLayoutShapes;

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
        PptxImport& _import,
        const QString& _path,
        const QString& _file,
        uint _slideNumber,
        MSOOXML::DrawingMLTheme* _themes,
        PptxXmlSlideReader::Type _type,
        PptxSlideProperties* _slideLayoutProperties,
        PptxSlideProperties* _slideMasterProperties,
        PptxSlideProperties* _notesMasterProperties,
        MSOOXML::MsooXmlRelationships& _relationships,
        QMap<int, QString> _commentAuthors,
        QMap<QString, QString> masterColorMap,
        VmlDrawingReader& _vmlReader,
        QString _tableStylesFilePath = QString());

    PptxImport* import;
    const QString path;
    const QString file;
    const uint slideNumber;
    MSOOXML::DrawingMLTheme* themes;
    PptxXmlSlideReader::Type type;
    PptxSlideProperties* slideLayoutProperties;
    PptxSlideProperties* slideMasterProperties;
    PptxSlideProperties* notesMasterProperties;
    PptxActualSlideProperties currentSlideStyles;
    PptxActualSlideProperties currentNotesStyles;

    // There could potentially be multiple of these...todo
    QString pageDrawStyleName; //!< written in read_sldInternal()
    QVector<QString> pageFrames; //! Frames which go to masterslide
    QMap<int, QString> commentAuthors;

    VmlDrawingReader& vmlReader;

    // Used to keep track, whether we should skip elements.  At the moment we
    // need to read some slides twice because some elements from the later part
    // of the document are needed to fully understand cSld element.
    bool firstReadingRound;

    // Temporary attribute providing the info that the color mapping changed
    // compared to the slideMaster.
    bool overrideClrMapping;

    void initializeContext(const MSOOXML::DrawingMLTheme& theme, const QVector<KoGenStyle>& _defaultParagraphStyles,
        const QVector<KoGenStyle>& _defaultTextStyles, const QVector<MSOOXML::Utils::ParagraphBulletProperties>& _defaultListStyles,
        const QVector<QString>& _defaultBulletColors, const QVector<QString>& _defaultTextColors, const QVector<QString>& _defaultLatinFonts);

    // Must be in context, because each slide/layout/master may overwrite the
    // color mapping and the default text colors have to be re-interpreted.
    QVector<KoGenStyle> defaultTextStyles;
    QVector<KoGenStyle> defaultParagraphStyles;
    QVector<MSOOXML::Utils::ParagraphBulletProperties> defaultListStyles;
    QVector<QString> defaultBulletColors;
    QVector<QString> defaultTextColors;
    QVector<QString> defaultLatinFonts;

    // We need to know where to find the table styles when needed
    QString tableStylesFilePath;
};

#endif
