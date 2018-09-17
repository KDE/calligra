/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
 * Contact: Suresh Chande suresh.chande@nokia.com
 * Copyright (C) 2011 Matus Uzak <matus.uzak@ixonos.com>
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

#include "PptxXmlSlideReader.h"

#include "PptxImport.h"
#include "PptxDebug.h"

#include "Charting.h"
#include "KoOdfChartWriter.h"
#include "XlsxXmlChartReader.h"
#include "PptxXmlCommentsReader.h"

#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <MsooXmlRelationships.h>
#include <MsooXmlUnits.h>
#include <MsooXmlDrawingTableStyle.h>
#include <MsooXmlDrawingTableStyleReader.h>
#include <MsooXmlTheme.h>

#include <KoXmlWriter.h>
#include <KoGenStyles.h>
#include <KoOdfGraphicStyles.h>

#include <QBrush>

#define MSOOXML_CURRENT_NS "p"
#define MSOOXML_CURRENT_CLASS PptxXmlSlideReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS
#define PPTXXMLSLIDEREADER_CPP

#define PPTX_INHERIT_CURRENT_SHAPE_PROPERTIES
#define PPTX_INHERIT_CURRENT_NOTE_PROPERTIES
//#define PPTX_DEBUG_LIST_STYLES
//#define PPTX_DEBUG_TEXT_STYLES

#include <MsooXmlReader_p.h>
#include <MsooXmlContentTypes.h>

#include <KoTable.h>
#include <KoRow.h>
#include <KoColumnStyle.h>
#include <KoColumn.h>
#include <KoCell.h>
#include <KoRawCellChild.h>


PptxShapeProperties::PptxShapeProperties()
{
    x = 0;
    y = 0;
    width = -1;
    height = -1;
    rot = 0;
}

PptxShapeProperties::PptxShapeProperties(const PptxShapeProperties &other)
{
    *this = other;
}

PptxShapeProperties& PptxShapeProperties::operator=(const PptxShapeProperties &other)
{
    x = other.x;
    y = other.y;
    width = other.width;
    height = other.height;
    rot = other.rot;
    return *this;
}

// -------------------

PptxPlaceholder::PptxPlaceholder()
{
}

PptxPlaceholder::PptxPlaceholder(const PptxShapeProperties &other)
 : x(other.x), y(other.y), width(other.width), height(other.height), rot(other.rot)
{
    debugPptx << x << y << width << height;
}

PptxPlaceholder::~PptxPlaceholder()
{
}

// -------------------

PptxSlideProperties::PptxSlideProperties()
{
    overrideClrMapping = false;
    m_drawingPageProperties = KoGenStyle(KoGenStyle::DrawingPageAutoStyle, "drawing-page");
}

PptxSlideProperties::~PptxSlideProperties()
{
}

// -------------------

PptxXmlSlideReaderContext::PptxXmlSlideReaderContext(
    PptxImport& _import, const QString& _path, const QString& _file,
    uint _slideNumber, MSOOXML::DrawingMLTheme* _themes,
    PptxXmlSlideReader::Type _type,
    PptxSlideProperties* _slideLayoutProperties,
    PptxSlideProperties* _slideMasterProperties,
    PptxSlideProperties* _notesMasterProperties,
    MSOOXML::MsooXmlRelationships& _relationships,
    QMap<int, QString> _commentAuthors,
    QMap<QString, QString> masterColorMap,
    VmlDrawingReader& _vmlReader,
    QString _tableStylesFilePath)
        : MSOOXML::MsooXmlReaderContext(&_relationships),
        import(&_import), path(_path), file(_file),
        slideNumber(_slideNumber), themes(_themes), type(_type),
        slideLayoutProperties(_slideLayoutProperties),
        slideMasterProperties(_slideMasterProperties),
        notesMasterProperties(_notesMasterProperties),
        commentAuthors(_commentAuthors),
        vmlReader(_vmlReader),
        firstReadingRound(false),
        overrideClrMapping(false),
        tableStylesFilePath(_tableStylesFilePath)
{
    colorMap = masterColorMap;
}

void PptxXmlSlideReaderContext::initializeContext(
        const MSOOXML::DrawingMLTheme& theme,
        const QVector<KoGenStyle>& _defaultParagraphStyles,
        const QVector<KoGenStyle>& _defaultTextStyles,
        const QVector<MSOOXML::Utils::ParagraphBulletProperties>& _defaultListStyles,
        const QVector<QString>& _defaultBulletColors,
        const QVector<QString>& _defaultTextColors,
        const QVector<QString>& _defaultLatinFonts)
{
    // Only now, we can fully prepare default text styles, as we know the theme we are using
    // And we have the mapping available
    defaultTextStyles = _defaultTextStyles;
    defaultParagraphStyles = _defaultParagraphStyles;
    defaultListStyles = _defaultListStyles;
    defaultBulletColors = _defaultBulletColors;
    defaultTextColors = _defaultTextColors;
    defaultLatinFonts = _defaultLatinFonts;
    int defaultIndex = 0;

    // NOTE: Workaround!  The color mapping changed compared to slideMaster.
    // Let's use theme specific default colors until we get correct
    // style:use-window-font-color support.
    QMap<QString, QString> colorMapBkp;
    if (type == PptxXmlSlideReader::Slide && slideLayoutProperties->overrideClrMapping) {
        colorMapBkp = colorMap;
        colorMap = slideLayoutProperties->colorMap;
    }

    while (defaultIndex < defaultTextStyles.size()) {
        if (!defaultTextColors.at(defaultIndex).isEmpty()) {
            QString valTransformed = colorMap.value(defaultTextColors.at(defaultIndex));
            MSOOXML::DrawingMLColorSchemeItemBase *colorItem = theme.colorScheme.value(valTransformed);
            QColor col = Qt::black;
            if (colorItem) {
                col = colorItem->value();
            }
            defaultTextStyles[defaultIndex].addProperty("fo:color", col.name());
        }
        if (!defaultLatinFonts.at(defaultIndex).isEmpty()) {
            QString face = defaultLatinFonts.at(defaultIndex);
            if (face.startsWith(QLatin1String("+mj"))) {
                face = theme.fontScheme.majorFonts.latinTypeface;
            }
            else if (face.startsWith(QLatin1String("+mn"))) {
                face = theme.fontScheme.minorFonts.latinTypeface;
            }
            defaultTextStyles[defaultIndex].addProperty("fo:font-family", face);
        }
        if (!defaultBulletColors.at(defaultIndex).isEmpty()) {
            QString valTransformed = colorMap.value(defaultBulletColors.at(defaultIndex));
            MSOOXML::DrawingMLColorSchemeItemBase *colorItem = theme.colorScheme.value(valTransformed);
            QColor col = Qt::black;
            if (colorItem) {
                col = colorItem->value();
            }
            defaultListStyles[defaultIndex].setBulletColor(col.name());
        }
        ++defaultIndex;
    }

    // NOTE: Workaround Part!
    if (type == PptxXmlSlideReader::Slide && slideLayoutProperties->overrideClrMapping) {
        colorMap = colorMapBkp;
    }
}

class PptxXmlSlideReader::Private
{
public:
    Private() : tableStyleList(0) {
    }
    ~Private() {
        delete tableStyleList;
    }
    KoXmlWriter *body; //!< Backup body pointer for SlideMaster mode
    //! Used to index shapes in master slide when inheriting properties
    bool phRead;
    QString qualifiedNameOfMainElement;
    QString phType; //!< set by read_ph()
    QString phIdx; //!< set by read_ph()

    //!set by read_t as true whenever some characters are copied to a textbox,
    //!used to figure out if a shape is a placeholder or not
    bool textBoxHasContent;

    QMap<QString, MSOOXML::DrawingTableStyle*>* tableStyleList;
};

PptxXmlSlideReader::PptxXmlSlideReader(KoOdfWriters *writers)
        : MSOOXML::MsooXmlCommonReader(writers)
        , m_context(0)
        , m_currentShapeProperties(0)
        , m_placeholderElWriter(0)
        , d(new Private)
{
    init();
}

PptxXmlSlideReader::~PptxXmlSlideReader()
{
    doneInternal(); // MsooXmlCommonReaderImpl.h
    delete d;
}

void PptxXmlSlideReader::init()
{
    initInternal(); // MsooXmlCommonReaderImpl.h
    initDrawingML();
    m_defaultNamespace = QLatin1String(MSOOXML_CURRENT_NS ":");
}

KoFilter::ConversionStatus PptxXmlSlideReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = dynamic_cast<PptxXmlSlideReaderContext*>(context);
    Q_ASSERT(m_context);
    switch (m_context->type) {
    case Slide:
        d->qualifiedNameOfMainElement = "p:sld";
        break;
    case SlideLayout:
        d->qualifiedNameOfMainElement = "p:sldLayout";
        break;
    case SlideMaster:
        d->qualifiedNameOfMainElement = "p:sldMaster";
        break;
    case NotesMaster:
        d->qualifiedNameOfMainElement = "p:notesMaster";
        break;
    case Notes:
        d->qualifiedNameOfMainElement = "p:notes";
        break;
    }
    const KoFilter::ConversionStatus result = readInternal();
    m_context = 0;

    return result;
}

KoFilter::ConversionStatus PptxXmlSlideReader::readInternal()
{
    debugPptx << "=============================";
    QBuffer masterBuffer;
    if (m_context->type == SlideMaster) {
        //! Clear body pointer for SlideMaster mode: avoid writing to body by mistake in this mode
        d->body = body;
        // We do not want to write to the main body in slidemaster, so we use secondary body,
        // the old body is
        body = new KoXmlWriter(&masterBuffer);
    }
    else if (m_context->type == NotesMaster) {
        // For now, we read placeholders etc. from notesmaster but don't output anything
        d->body = body;
        body = new KoXmlWriter(&masterBuffer);
    }

    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    // p:sld{Master}
    readNext();
    debugPptx << *this << namespaceUri();

    if (!expectEl(d->qualifiedNameOfMainElement)) {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::presentationml)) {
        return KoFilter::WrongFormat;
    }
    /*
        const QXmlStreamAttributes attrs( attributes() );
        for (int i=0; i<attrs.count(); i++) {
            debugPptx << "1 NS prefix:" << attrs[i].name() << "uri:" << attrs[i].namespaceUri();
        }*/

    QXmlStreamNamespaceDeclarations namespaces(namespaceDeclarations());
    for (int i = 0; i < namespaces.count(); i++) {
        debugPptx << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }
//! @todo find out whether the namespace returned by namespaceUri()
//!       is exactly the same ref as the element of namespaceDeclarations()
    if (!namespaces.contains(QXmlStreamNamespaceDeclaration("p", MSOOXML::Schemas::presentationml))) {
        raiseError(i18n("Namespace \"%1\" not found", QLatin1String(MSOOXML::Schemas::presentationml)));
        return KoFilter::WrongFormat;
    }
//! @todo expect other namespaces too...

    switch (m_context->type) {
    case Slide:
        TRY_READ(sld)
        break;
    case SlideLayout:
        TRY_READ(sldLayout)
        break;
    case SlideMaster:
        TRY_READ(sldMaster)
        break;
    case NotesMaster:
        TRY_READ(notesMaster)
        break;
    case Notes:
        TRY_READ(notes)
        break;
    }

     if (m_context->type == SlideMaster) {
        QString elementContents = QString::fromUtf8(masterBuffer.buffer(), masterBuffer.buffer().size());
        m_context->pageFrames.push_back(elementContents);

        // write the contents here to pageFrames
        delete body;
        body = d->body;
    }
    else if (m_context->type == NotesMaster) {
        delete body;
        body = d->body;
    }

    debugPptx << "===========finished============";
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL notes
//! notes handler (Notes Slide)
/*!

 Child elements:
    - clrMapOvr (Color Scheme Map Override) §19.3.1.7
    - [done] cSld (Common Slide Data) §19.3.1.16
    - extLst (Extension List with Modification Flag) §19.3.1.20
    - timing (Slide Timing Information for a Slide Layout) §19.3.1.48
    - transition (Slide Transition for a Slide Layout) §19.3.1.50
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_notes()
{
    READ_PROLOGUE
    RETURN_IF_ERROR( read_sldInternal() )
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL sld
//! sld handler (Presentation Slide)
/*! This element specifies a slide.
 ECMA-376, 19.3.1.38, p. 2849.
 Root element of PresentationML Slide part.

 Child elements:
    - clrMapOvr (Color Scheme Map Override) §19.3.1.7
    - [done] cSld (Common Slide Data) §19.3.1.16
    - extLst (Extension List with Modification Flag) §19.3.1.20
    - timing (Slide Timing Information for a Slide Layout) §19.3.1.48
    - transition (Slide Transition for a Slide Layout) §19.3.1.50
*/
//! @todo support all child elements
//! CASE #P300
KoFilter::ConversionStatus PptxXmlSlideReader::read_sld()
{
    READ_PROLOGUE
    RETURN_IF_ERROR( read_sldInternal() )
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL notesMaster
//! notesMaster handler (Notes Master)
/*! ECMA-376, 19.3.1.42, p. 2853.
 This element specifies an instance of a slide master slide.

 Child elements:
    - [done] clrMap (Color Scheme Map) §19.3.1.6
    - [done] cSld (Common Slide Data) §19.3.1.16
    - extLst (Extension List with Modification Flag) §19.3.1.20
    - hf (Header/Footer information for a master) §19.3.1.25
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_notesMaster()
{
    READ_PROLOGUE
    RETURN_IF_ERROR( read_sldInternal() )
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL sldMaster
//! sldMaster handler (Slide Master)
/*! ECMA-376, 19.3.1.42, p. 2853.
 This element specifies an instance of a slide master slide.

 Child elements:
    - [done] clrMap (Color Scheme Map) §19.3.1.6
    - [done] cSld (Common Slide Data) §19.3.1.16
    - extLst (Extension List with Modification Flag) §19.3.1.20
    - hf (Header/Footer information for a slide master) §19.3.1.25
    - sldLayoutIdLst (List of Slide Layouts) §19.3.1.41
    - timing (Slide Timing Information for a Slide Layout) §19.3.1.48
    - transition (Slide Transition for a Slide Layout) §19.3.1.50
    - [done] txStyles (Slide Master Text Styles) §19.3.1.52
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_sldMaster()
{
    READ_PROLOGUE
    RETURN_IF_ERROR( read_sldInternal() )
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL sldLayout
//! sldLayout handler (Slide Layout)
/*! ECMA-376, 19.3.1.39, p. 2851.
 This element specifies an instance of a slide layout. The slide layout contains
 in essence a template slide design that can be applied to any existing slide.
 When applied to an existing slide all corresponding content should be mapped to the new slide layout.

 Root element of PresentationML Slide Layout part.

 Child elements:
 - clrMapOvr (Color Scheme Map Override) §19.3.1.7
 - [done] cSld (Common Slide Data) §19.3.1.16
 - extLst (Extension List with Modification Flag) §19.3.1.20
 - hf (Header/Footer information for a slide master) §19.3.1.25
 - timing (Slide Timing Information for a Slide Layout) §19.3.1.48
 - transition (Slide Transition for a Slide Layout) §19.3.1.50
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_sldLayout()
{
    READ_PROLOGUE
    RETURN_IF_ERROR( read_sldInternal() )
    READ_EPILOGUE
}

//! CASE #P300
KoFilter::ConversionStatus PptxXmlSlideReader::read_sldInternal()
{
    QXmlStreamNamespaceDeclarations namespaces = namespaceDeclarations();
    for (int i = 0; i < namespaces.count(); i++) {
        debugPptx << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }

    // m_currentDrawStyle defined in "MsooXmlCommonReader.h"
    m_currentDrawStyle = new KoGenStyle(KoGenStyle::DrawingPageAutoStyle, "drawing-page"); // CASE #P109

    MSOOXML::Utils::XmlWriteBuffer drawPageBuf; // buffer this draw:page, because we have to compute

    bool showMasterShapes = true;
    bool showSlide = true;

    if (!m_context->firstReadingRound) {

        //TODO: attributes support
        const QXmlStreamAttributes attrs(attributes());

        TRY_READ_ATTR_WITHOUT_NS(showMasterSp)
        if (!showMasterSp.isEmpty()) {
            STRING_TO_INT(showMasterSp, showMasterShapes, "attr:showMasterSp")
        }

        if (m_context->type == SlideLayout) {
            m_currentPresentationPageLayoutStyle = KoGenStyle(KoGenStyle::PresentationPageLayoutStyle);
        }

        if ((m_context->type == SlideMaster) || (m_context->type == SlideLayout)) {
            //TODO: Is the presentation:visibility attribute required?
            m_currentDrawStyle->addProperty("presentation:visibility", "visible");
            if (!showMasterSp.isEmpty()) {
                m_currentDrawStyle->addProperty("presentation:background-objects-visible", showMasterShapes);
            }
        }
        else if (m_context->type == Slide) {

            TRY_READ_ATTR_WITHOUT_NS(show)
            if (!show.isEmpty()) {
                STRING_TO_INT(show, showSlide, "attr:show")
            }

            // Inherit drawing-page-properties from SlideMaster/slideLayout.
            KoGenStyle::copyPropertiesFromStyle(m_context->slideMasterProperties->m_drawingPageProperties,
                                                *m_currentDrawStyle, KoGenStyle::DrawingPageType);

            KoGenStyle::copyPropertiesFromStyle(m_context->slideLayoutProperties->m_drawingPageProperties,
                                                *m_currentDrawStyle, KoGenStyle::DrawingPageType);

            // Now we have the info if placeholders from SlideMaster should be
            // displayed.  Have to process the info if SlideLayout placeholders
            // should be displayed separately.
            m_showSlideLayoutShapes = true;

            //Presentation Slide overrides.
            if (!showMasterSp.isEmpty()) {
                m_currentDrawStyle->addProperty("presentation:background-objects-visible", showMasterShapes);
                m_showSlideLayoutShapes = showMasterShapes;
            }
            if (!show.isEmpty()) {
                m_currentDrawStyle->addProperty("presentation:visibility", showSlide ? "visible" : "hidden");
            }

            // Default values for a presentation slide
            if ((m_currentDrawStyle->property("presentation:background-objects-visible")).isEmpty()) {
                m_currentDrawStyle->addProperty("presentation:background-objects-visible", "true");
            }
            if ((m_currentDrawStyle->property("presentation:visibility")).isEmpty()) {
                m_currentDrawStyle->addProperty("presentation:visibility", "visible");
            }

            // style before style name is known
            body = drawPageBuf.setWriter(body);
        }
    }

    while (!atEnd()) {
        readNext();
        if (isStartElement()) {
            // This effectively means that in the first round of reading slideMaster, we ignore cSld
            if (QUALIFIED_NAME_IS(cSld)) {
                if (!m_context->firstReadingRound) {
                    TRY_READ(cSld)
                }
                else {
                    skipCurrentElement();
                }
            }
            else if (m_context->type == NotesMaster && QUALIFIED_NAME_IS(notesStyles)) {
                if (m_context->firstReadingRound) {
                    TRY_READ(notesStyle)
                }
                else {
                   skipCurrentElement();
                }
            }
            else if (m_context->type == SlideMaster && QUALIFIED_NAME_IS(txStyles)) {
                if (m_context->firstReadingRound) {
                    TRY_READ(txStyles)
                }
                else {
                    skipCurrentElement();
                }
            }
            else if ((m_context->type == NotesMaster || m_context->type == SlideMaster) &&
                     QUALIFIED_NAME_IS(clrMap))
            {
                if (m_context->firstReadingRound) {
                    TRY_READ(clrMap)
                }
                else {
                    skipCurrentElement();
                }
            }
            else if (m_context->type != SlideMaster && QUALIFIED_NAME_IS(clrMapOvr)) {
                if (m_context->firstReadingRound) {
                    TRY_READ(clrMapOvr)
                }
                else {
                    skipCurrentElement();
                }
            }
//! @todo add ELSE_WRONG_FORMAT
        }
        if (isEndElement()) {
            if (d->qualifiedNameOfMainElement == qualifiedName()) {
                break;
            }
        }
    }

    if (m_context->type == Slide && !m_context->firstReadingRound) {

        body = drawPageBuf.originalWriter();

        body->startElement("draw:page"); // CASE #P300
        QString masterName =  m_context->slideLayoutProperties->m_slideMasterName;
        masterName.chop(4); //removes .xml
        masterName.remove(0, masterName.lastIndexOf("slideMaster"));
        body->addAttribute("draw:master-page-name", masterName);
        //! @todo draw:name can be pulled out of docProps/app.xml (TitlesOfParts)
        body->addAttribute("draw:name", i18n("Slide %1",m_context->slideNumber+1)); //optional; CASE #P303
        body->addAttribute("draw:id", QString("pid%1").arg(m_context->slideNumber)); //optional; unique ID; CASE #P305, #P306
        body->addAttribute("xml:id", QString("pid%1").arg(m_context->slideNumber)); //optional; unique ID; CASE #P305, #P306

        const QString currentPageStyleName(mainStyles->insert(*m_currentDrawStyle, "dp"));
        body->addAttribute("draw:style-name", currentPageStyleName); // CASE #P302
        debugPptx << "currentPageStyleName:" << currentPageStyleName;

        if (!m_context->slideLayoutProperties->pageLayoutStyleName.isEmpty()) {
            // CASE #P308
            debugPptx << "presentation:presentation-page-layout-name=" <<
                        m_context->slideLayoutProperties->pageLayoutStyleName;

            body->addAttribute("presentation:presentation-page-layout-name",
                                m_context->slideLayoutProperties->pageLayoutStyleName);
        }

        (void)drawPageBuf.releaseWriter();

        // Read notes
        {
            QString notesTarget = m_context->relationships->targetForType(m_context->path, m_context->file,
                "http://schemas.openxmlformats.org/officeDocument/2006/relationships/notesSlide");
            if (!notesTarget.isEmpty()) {
                body->startElement("presentation:notes");

                QString notesPath, notesFile;
                MSOOXML::Utils::splitPathAndFile(notesTarget, &notesPath, &notesFile);

                QMap<int, QString> dummyAuthors;
                VmlDrawingReader vmlreader(this);
                QString vmlTarget = m_context->relationships->targetForType(notesPath, notesFile,
                    "http://schemas.openxmlformats.org/officeDocument/2006/relationships/vmlDrawing");

                if (!vmlTarget.isEmpty()) {
                    QString errorMessage, vmlPath, vmlFile;

                    QString fileName = vmlTarget;
                    fileName.remove(0, m_context->path.length());
                    MSOOXML::Utils::splitPathAndFile(vmlTarget, &vmlPath, &vmlFile);

                    VmlDrawingReaderContext vmlContext(*m_context->import,
                        vmlPath, vmlFile, *m_context->relationships);

                   const KoFilter::ConversionStatus status =
                       m_context->import->loadAndParseDocument(&vmlreader, vmlTarget, errorMessage, &vmlContext);
                   if (status != KoFilter::OK) {
                       vmlreader.raiseError(errorMessage);
                   }
                }

                PptxXmlSlideReaderContext context(
                    *m_context->import,
                    notesPath, notesFile,
                    0,
                    &m_context->notesMasterProperties->theme,
                    PptxXmlSlideReader::Notes,
                    0,
                    0,
                    m_context->notesMasterProperties,
                    *m_context->relationships,
                    dummyAuthors,
                    m_context->notesMasterProperties->colorMap,
                    vmlreader
                );

                // In first round we only read possible colorMap override
                PptxXmlSlideReader slideReader(this);
                context.firstReadingRound = true;

                KoFilter::ConversionStatus status = m_context->import->loadAndParseDocument(&slideReader, notesTarget, &context);
                if (status != KoFilter::OK) {
                    debugPptx << slideReader.errorString();
                    return status;
                }

                context.initializeContext(m_context->notesMasterProperties->theme, m_context->defaultParagraphStyles,
                    m_context->defaultTextStyles, m_context->defaultListStyles, m_context->defaultBulletColors,
                    m_context->defaultTextColors, m_context->defaultLatinFonts);

                // In this round we read rest
                context.firstReadingRound = false;
                status = m_context->import->loadAndParseDocument(&slideReader, notesTarget, &context);
                if (status != KoFilter::OK) {
                    debugPptx << slideReader.errorString();
                    return status;
                }
                body->endElement(); // presentation:notes
            }
        }
        // Read comments
        {
            PptxXmlCommentsReader commentsReader(this);
            const QString filepath = m_context->relationships->targetForType(m_context->path, m_context->file, MSOOXML::Relationships::comments);
            PptxXmlCommentsReaderContext commentsContext;
            commentsContext.authors = m_context->commentAuthors;
            (void)m_context->import->loadAndParseDocument(&commentsReader, filepath, &commentsContext);
        }

        body->endElement(); //draw:page
    }
    else if (m_context->type == SlideMaster && !m_context->firstReadingRound) {
        if (!m_currentDrawStyle->isEmpty()) {
            KoGenStyle::copyPropertiesFromStyle(*m_currentDrawStyle,
                                                m_context->slideMasterProperties->m_drawingPageProperties,
                                                KoGenStyle::DrawingPageType);

            m_currentDrawStyle->setAutoStyleInStylesDotXml(true);
            m_context->pageDrawStyleName = mainStyles->insert(*m_currentDrawStyle, "dp");

            debugPptx << "m_context->pageDrawStyleName:" << m_context->pageDrawStyleName <<
                        "m_context->type:" << m_context->type;
        }
    }
    else if (m_context->type == SlideLayout && !m_context->firstReadingRound) {
        if (!m_currentDrawStyle->isEmpty()) {
            KoGenStyle::copyPropertiesFromStyle(*m_currentDrawStyle,
                                                m_context->slideLayoutProperties->m_drawingPageProperties,
                                                KoGenStyle::DrawingPageType);

            m_context->pageDrawStyleName = mainStyles->insert(*m_currentDrawStyle, "dp");
        }
        m_context->slideLayoutProperties->pageLayoutStyleName =
            mainStyles->insert(m_currentPresentationPageLayoutStyle);

        debugPptx << "slideLayoutProperties->styleName:" << m_context->slideLayoutProperties->pageLayoutStyleName;
    }

    delete m_currentDrawStyle;
    m_currentDrawStyle = 0;

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL txStyles
//! txStyles handler (Slide Master Text Styles)
/*! ECMA-376, 19.3.1.52, p. 2862.

 Parent elements:
    - [done] sldMaster (§19.3.1.42)

 Child elements:
    - [done] bodyStyle (Slide Master Body Text Style)   §19.3.1.5
    - extLst (Extension List)                    §19.2.1.12
    - [done] otherStyle (Slide Master Other Text Style) §19.3.1.35
    - [done] titleStyle (Slide Master Title Text Style) §19.3.1.49
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_txStyles()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        debugPptx << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(bodyStyle)
            ELSE_TRY_READ_IF(titleStyle)
            ELSE_TRY_READ_IF(otherStyle)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL notesStyle
//! mptesStyle handler (Notes Master Text)
/*!
 Parent elements:
    - [done] notesMaster (§19.3.1.52)

 Child elements:
    - defPPr (Default Paragraph Style)  §21.1.2.2.2
    - extLst (Extension List)           §20.1.2.2.15
    - [done] lvl1pPr (List Level 1 Text Style) §21.1.2.4.13
    - [done] lvl2pPr (List Level 2 Text Style) §21.1.2.4.14
    - [done] lvl3pPr (List Level 3 Text Style) §21.1.2.4.15
    - [done] lvl4pPr (List Level 4 Text Style) §21.1.2.4.16
    - [done] lvl5pPr (List Level 5 Text Style) §21.1.2.4.17
    - [done] lvl6pPr (List Level 6 Text Style) §21.1.2.4.18
    - [done] lvl7pPr (List Level 7 Text Style) §21.1.2.4.19
    - [done] lvl8pPr (List Level 8 Text Style) §21.1.2.4.20
    - [done] lvl9pPr (List Level 9 Text Style) §21.1.2.4.21
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_notesStyle()
{
    READ_PROLOGUE

    d->phType = "notes";

    m_currentCombinedBulletProperties.clear();

    while (!atEnd()) {
        readNext();
        debugPptx << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF_NS(a, lvl1pPr)
            ELSE_TRY_READ_IF_NS(a, lvl2pPr)
            ELSE_TRY_READ_IF_NS(a, lvl3pPr)
            ELSE_TRY_READ_IF_NS(a, lvl4pPr)
            ELSE_TRY_READ_IF_NS(a, lvl5pPr)
            ELSE_TRY_READ_IF_NS(a, lvl6pPr)
            ELSE_TRY_READ_IF_NS(a, lvl7pPr)
            ELSE_TRY_READ_IF_NS(a, lvl8pPr)
            ELSE_TRY_READ_IF_NS(a, lvl9pPr)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    saveCurrentListStyles();
    saveCurrentStyles();

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL bodyStyle
//! bodyStyle handler (Slide Master Body Text)
/*!
 Parent elements:
    - [done] txStyles (§19.3.1.52)

 Child elements:
    - defPPr (Default Paragraph Style)  §21.1.2.2.2
    - extLst (Extension List)           §20.1.2.2.15
    - [done] lvl1pPr (List Level 1 Text Style) §21.1.2.4.13
    - [done] lvl2pPr (List Level 2 Text Style) §21.1.2.4.14
    - [done] lvl3pPr (List Level 3 Text Style) §21.1.2.4.15
    - [done] lvl4pPr (List Level 4 Text Style) §21.1.2.4.16
    - [done] lvl5pPr (List Level 5 Text Style) §21.1.2.4.17
    - [done] lvl6pPr (List Level 6 Text Style) §21.1.2.4.18
    - [done] lvl7pPr (List Level 7 Text Style) §21.1.2.4.19
    - [done] lvl8pPr (List Level 8 Text Style) §21.1.2.4.20
    - [done] lvl9pPr (List Level 9 Text Style) §21.1.2.4.21
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_bodyStyle()
{
    READ_PROLOGUE

    d->phType = "body";

    m_currentCombinedBulletProperties.clear();

    while (!atEnd()) {
        readNext();
        debugPptx << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF_NS(a, lvl1pPr)
            ELSE_TRY_READ_IF_NS(a, lvl2pPr)
            ELSE_TRY_READ_IF_NS(a, lvl3pPr)
            ELSE_TRY_READ_IF_NS(a, lvl4pPr)
            ELSE_TRY_READ_IF_NS(a, lvl5pPr)
            ELSE_TRY_READ_IF_NS(a, lvl6pPr)
            ELSE_TRY_READ_IF_NS(a, lvl7pPr)
            ELSE_TRY_READ_IF_NS(a, lvl8pPr)
            ELSE_TRY_READ_IF_NS(a, lvl9pPr)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    saveCurrentListStyles();
    saveCurrentStyles();

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL controls
//! controls handler (List of controls)
/*!
 Parent elements:

 Child elements:
 - [done] control (Embedded Control) §19.3.2.1
*/
KoFilter::ConversionStatus PptxXmlSlideReader::read_controls()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(control)
            ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL control
//! control handler (Embedded Control)
/*!
 Parent elements:
 - [done] controls (§19.3.1.15)

 Child elements:
 - extLst (Extension List) §19.2.1.12
 - pic (Picture) §19.3.1.37
*/
KoFilter::ConversionStatus PptxXmlSlideReader::read_control()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR_WITHOUT_NS(spid)
    spid = "_x0000_s" + spid;

    QString frameBeing = m_context->vmlReader.frames().value(spid);
    // Replacement image
    if (!frameBeing.isEmpty()) {
        body->addCompleteElement(frameBeing.toUtf8());
        body->startElement("draw:image");
        body->addAttribute("xlink:type", "simple");
        body->addAttribute("xlink:show", "embed");
        body->addAttribute("xlink:actuate", "onLoad");
        body->addAttribute("xlink:href", m_context->vmlReader.content().value(spid));
        body->endElement(); // draw:image
        body->addCompleteElement("</draw:frame>");
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL oleObj
//! oleObj handler (Global Element for Embedded objects and Controls)
/*!
 Parent elements:
 - [done] cSld (§19.3.1.16)

 Child elements:
 - embed (Embedded Object or Control) §19.3.2.2
 - link (Linked Object or Control) §19.3.2.3
 - pic (Picture) §19.3.1.37

*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_oleObj()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITH_NS(r, id);
    TRY_READ_ATTR_WITHOUT_NS(imgW);
    TRY_READ_ATTR_WITHOUT_NS(imgH);
    TRY_READ_ATTR_WITHOUT_NS(progId);
    TRY_READ_ATTR_WITHOUT_NS(name);
    TRY_READ_ATTR_WITHOUT_NS(spid)

    /*
    if(!imgW.isEmpty()) m_svgWidth = imgW.toInt();
    if(!imgH.isEmpty()) m_svgHeight = imgH.toInt();
    */

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    if (!r_id.isEmpty()) {
        QString sourceName(m_context->relationships->target(m_context->path, m_context->file, r_id));
        if (sourceName.isEmpty()) {
            return KoFilter::FileNotFound;
        }

        QString destinationName = QLatin1String("") + sourceName.mid(sourceName.lastIndexOf('/') + 1);
        KoFilter::ConversionStatus stat = m_context->import->copyFile(sourceName, destinationName, false );
        // In case the file could not be find due to it being external we can at least do draw:image from below
        if (stat == KoFilter::OK) {
            body->startElement("draw:object-ole");
            addManifestEntryForFile(destinationName);
            body->addAttribute("xlink:href", destinationName);
            body->addAttribute("xlink:type", "simple");
            body->endElement(); // draw:object-ole
        }

        // Replacement
        body->startElement("draw:image");
        body->addAttribute("xlink:type", "simple");
        body->addAttribute("xlink:show", "embed");
        body->addAttribute("xlink:actuate", "onLoad");
        body->addAttribute("xlink:href", m_context->vmlReader.content().value(spid));
        body->endElement(); // draw:image
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL titleStyle
//! titleStyle handler (Slide Master Title Text)
/*!
 Parent elements:
    - [done] txStyles (§19.3.1.52)

 Child elements:
    - defPPr (Default Paragraph Style)  §21.1.2.2.2
    - extLst (Extension List)           §20.1.2.2.15
    - [done] lvl1pPr (List Level 1 Text Style) §21.1.2.4.13
    - [done] lvl2pPr (List Level 2 Text Style) §21.1.2.4.14
    - [done] lvl3pPr (List Level 3 Text Style) §21.1.2.4.15
    - [done] lvl4pPr (List Level 4 Text Style) §21.1.2.4.16
    - [done] lvl5pPr (List Level 5 Text Style) §21.1.2.4.17
    - [done] lvl6pPr (List Level 6 Text Style) §21.1.2.4.18
    - [done] lvl7pPr (List Level 7 Text Style) §21.1.2.4.19
    - [done] lvl8pPr (List Level 8 Text Style) §21.1.2.4.20
    - [done] lvl9pPr (List Level 9 Text Style) §21.1.2.4.21
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_titleStyle()
{
    READ_PROLOGUE

    d->phType = "title";

    m_currentCombinedBulletProperties.clear();

    while (!atEnd()) {
        readNext();
        debugPptx << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF_NS(a, lvl1pPr)
            ELSE_TRY_READ_IF_NS(a, lvl2pPr)
            ELSE_TRY_READ_IF_NS(a, lvl3pPr)
            ELSE_TRY_READ_IF_NS(a, lvl4pPr)
            ELSE_TRY_READ_IF_NS(a, lvl5pPr)
            ELSE_TRY_READ_IF_NS(a, lvl6pPr)
            ELSE_TRY_READ_IF_NS(a, lvl7pPr)
            ELSE_TRY_READ_IF_NS(a, lvl8pPr)
            ELSE_TRY_READ_IF_NS(a, lvl9pPr)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    saveCurrentListStyles();
    saveCurrentStyles();

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL otherStyle
//! otherStyle handler (Slide Master Other Text)
/*! ECMA-376,19.3.1.35, p. 2847.

  This element specifies the text formatting style for the all other text
  within a master slide.  This formatting is used on all text not covered by
  the titleStyle or bodyStyle elements within related presentation slides.  The
  text formatting is specified by utilizing the DrawingML framework just as
  within a regular presentation slide.  Within the otherStyle element there can
  be many different style types defined as there are different kinds of text
  stored within a slide.

  Parent elements:
    - [done] txStyles (§19.3.1.52)

  Child elements:
    - defPPr (Default Paragraph Style)  §21.1.2.2.2
    - extLst (Extension List)           §20.1.2.2.15
    - [done] lvl1pPr (List Level 1 Text Style) §21.1.2.4.13
    - [done] lvl2pPr (List Level 2 Text Style) §21.1.2.4.14
    - [done] lvl3pPr (List Level 3 Text Style) §21.1.2.4.15
    - [done] lvl4pPr (List Level 4 Text Style) §21.1.2.4.16
    - [done] lvl5pPr (List Level 5 Text Style) §21.1.2.4.17
    - [done] lvl6pPr (List Level 6 Text Style) §21.1.2.4.18
    - [done] lvl7pPr (List Level 7 Text Style) §21.1.2.4.19
    - [done] lvl8pPr (List Level 8 Text Style) §21.1.2.4.20
    - [done] lvl9pPr (List Level 9 Text Style) §21.1.2.4.21
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_otherStyle()
{
    READ_PROLOGUE

    d->phType = "other";

    m_currentCombinedBulletProperties.clear();

    while (!atEnd()) {
        readNext();
        debugPptx << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF_NS(a, lvl1pPr)
            ELSE_TRY_READ_IF_NS(a, lvl2pPr)
            ELSE_TRY_READ_IF_NS(a, lvl3pPr)
            ELSE_TRY_READ_IF_NS(a, lvl4pPr)
            ELSE_TRY_READ_IF_NS(a, lvl5pPr)
            ELSE_TRY_READ_IF_NS(a, lvl6pPr)
            ELSE_TRY_READ_IF_NS(a, lvl7pPr)
            ELSE_TRY_READ_IF_NS(a, lvl8pPr)
            ELSE_TRY_READ_IF_NS(a, lvl9pPr)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    saveCurrentListStyles();
    saveCurrentStyles();

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL cSld
//! cSld handler (Common Slide Data)
/*! ECMA-376, 19.3.1.16, p. 2833.
 This element specifies a container for slide information that is relevant to all of the slide types.

 Parent elements:
    - [done] presentation (§19.2.1.26)
    - handoutMaster (§19.3.1.24)
    - [done] notes (§19.3.1.26)
    - [done] notesMaster (§19.3.1.27)
    - [done] sld (§19.3.1.38)
    - [done] sldLayout (§19.3.1.39)
    - [done] sldMaster (§19.3.1.42)

 Child elements:
    - [done] bg (Slide Background) §19.3.1.1
    - [done] controls (List of controls) §19.3.1.15
    - custDataLst (Customer Data List) §19.3.1.18
    - extLst (Extension List) §19.2.1.12
    - [done] spTree (Shape Tree) §19.3.1.45
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_cSld()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        debugPptx << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(bg)
            ELSE_TRY_READ_IF(spTree)
            ELSE_TRY_READ_IF(controls)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL clrMap
//! clrMap handler (Color Scheme Map)
/*! This element specifies the mapping layer that transforms one color scheme
 definition to another.  Each attribute represents a color name that can be
 referenced in this master, and the value is the corresponding color in the
 theme.

 Parent elements:
 - handoutMaster (§19.3.1.24)
 - [done] notesMaster (§19.3.1.27)
 - [done] sldMaster (§19.3.1.42)

 Child elements:
 - extLst (Extension List) §20.1.2.2.15

*/
KoFilter::ConversionStatus PptxXmlSlideReader::read_clrMap()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    int index = 0;
    while (index < attrs.size()) {
        const QString handledAttr = attrs.at(index).name().toString();
        const QString attrValue = attrs.value(handledAttr).toString();
        m_context->colorMap[handledAttr] = attrValue;
        if (m_context->type == SlideMaster) {
            m_context->slideMasterProperties->colorMap[handledAttr] = attrValue;
        }
        else if (m_context->type == NotesMaster) {
            m_context->notesMasterProperties->colorMap[handledAttr] = attrValue;
        }
        ++index;
    }

    SKIP_EVERYTHING
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL clrMapOvr
// clrMapOvr handler (Color Scheme Map Override)
/*
 Parent elements:
 - notes (§19.3.1.26)
 - [done] sld (§19.3.1.38)
 - [done] sldLayout (§19.3.1.39)

 Child elements:
 - [done] masterClrMapping (Master Color Mapping) §20.1.6.6
 - [done] overrideClrMapping (Override Color Mapping) §20.1.6.8
*/
KoFilter::ConversionStatus PptxXmlSlideReader::read_clrMapOvr()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF_NS(a, overrideClrMapping)
            ELSE_TRY_READ_IF_NS(a, masterClrMapping)
            ELSE_WRONG_FORMAT
//             SKIP_UNKNOWN
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL bg
// bg handler (Slide Background)
/*! ECMA-376, 19.3.1.1, p. 2815.
 This element specifies the background appearance information for a
 slide. The slide background covers the entire slide and is visible
 where no objects exist and as the background for transparent objects.

 Parent elements:
    - [done] cSld (§19.3.1.16)
 Attributes:
    - bwMode (Black and White Mode)
 Child elements:
    - [done] bgPr (Background Properties) §19.3.1.2
    - [done] bgRef (Background Style Reference) §19.3.1.3
*/
//! @todo support all elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_bg()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        debugPptx << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(bgPr)
            ELSE_TRY_READ_IF(bgRef)
            ELSE_WRONG_FORMAT
        }
    }

    if (!m_currentDrawStyle->isEmpty()) {
        if (m_context->type == NotesMaster) {
            KoGenStyle::copyPropertiesFromStyle(*m_currentDrawStyle,
                                                m_context->notesMasterProperties->m_drawingPageProperties,
                                                KoGenStyle::DrawingPageType);
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL bgRef
//! bgRef Handler (Background style reference)
/*! This element specifies the slide background is to use a fill style defined in the style matrix. The idx attribute
 refers to the index of a background fill style or fill style within the presentation's style matrix, defined by the
 fmtScheme element. A value of 0 or 1000 indicates no background, values 1-999 refer to the index of a fill style
 within the fillStyleLst element, and values 1001 and above refer to the index of a background fill style within

 Parent elements:
 - [done] bg (§19.3.1.1)

 Child elements:
 - hslClr (Hue, Saturation, Luminance Color Model) §20.1.2.3.13
 - prstClr (Preset Color) §20.1.2.3.22
 - [done] schemeClr (Scheme Color) §20.1.2.3.29
 - [done] scrgbClr (RGB Color Model - Percentage Variant) §20.1.2.3.30
 - [done] srgbClr (RGB Color Model - Hex Variant) §20.1.2.3.32
 - [done] sysClr (System Color) §20.1.2.3.33
*/
//! @todo support all elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_bgRef()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR_WITHOUT_NS(idx)
    int index = idx.toInt();

    m_currentColor = QColor();
    while (!atEnd()) {
        readNext();
        debugPptx << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF_NS(a, schemeClr)
            ELSE_TRY_READ_IF_NS(a, srgbClr)
            ELSE_TRY_READ_IF_NS(a, sysClr)
            ELSE_TRY_READ_IF_NS(a, scrgbClr)
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    MSOOXML::DrawingMLFillBase *fillBase = m_context->themes->formatScheme.fillStyles.value(index);
    if (fillBase) {
        fillBase->writeStyles(*mainStyles, m_currentDrawStyle, m_currentColor);
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL bgPr
//! 19.3.1.2 bgPr (Background Properties)
/*! ECMA-376, 19.3.1.2, p. 2815.
 This element specifies visual effects used to render the slide
 background. This includes any fill, image, or effects that are to
 make up the background of the slide.

 Parent elements:
 - [done] bg (§19.3.1.1)
 Attributes:
 - shadeToTitle
 Child elements:
 - [done] blipFill (Picture Fill) §20.1.8.14
 - effectDag (Effect Container) §20.1.8.25
 - [done] effectLst (Effect Container) §20.1.8.26
 - extLst (Extension List) §19.2.1.12
 - [done] gradFill (Gradient Fill) §20.1.8.33
 - grpFill (Group Fill) §20.1.8.35
 - [done] noFill (No Fill) §20.1.8.44
 - pattFill (Pattern Fill) §20.1.8.47
 - [done] solidFill (Solid Fill) §20.1.8.54
*/
//! @todo support all elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_bgPr()
{
    READ_PROLOGUE

    QString fillImageName;
    m_currentColor = QColor();

    while (!atEnd()) {
        readNext();
        debugPptx << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (qualifiedName() == QLatin1String("a:solidFill")) {
                TRY_READ_IF_NS(a, solidFill)
                if (m_currentColor != QColor()) {
                    m_currentDrawStyle->addProperty("draw:fill", QLatin1String("solid"));
                    m_currentDrawStyle->addProperty("draw:fill-color", m_currentColor.name());
                }
                if (m_currentAlpha > 0) {
                    m_currentDrawStyle->addProperty("draw:opacity", QString("%1%").arg(m_currentAlpha));
                }
            }
            else if (qualifiedName() == QLatin1String("a:effectLst")) {
                TRY_READ(effectLst)
            }
            else if (qualifiedName() == QLatin1String("a:noFill")) {
                m_currentDrawStyle->addProperty("draw:fill", constNone);
            }
            else if (qualifiedName() == QLatin1String("a:blipFill")) {
                TRY_READ_IF_NS_IN_CONTEXT(a, blipFill)
                if (!m_xlinkHref.isEmpty()) {
                    KoGenStyle fillStyle = KoGenStyle(KoGenStyle::FillImageStyle);
                    fillStyle.addProperty("xlink:href", m_xlinkHref);
                    fillStyle.addProperty("xlink:type", "simple");
                    fillStyle.addProperty("xlink:actuate", "onLoad");
                    const QString imageName = mainStyles->insert(fillStyle);
                    m_currentDrawStyle->addProperty("draw:fill", "bitmap");
                    m_currentDrawStyle->addProperty("draw:fill-image-name", imageName);
                    m_xlinkHref.clear();
                }
            }
            else if (qualifiedName() == QLatin1String("a:gradFill")) {
                m_currentGradientStyle = KoGenStyle(KoGenStyle::LinearGradientStyle);
                TRY_READ_IF_NS(a, gradFill)
                m_currentDrawStyle->addProperty("draw:fill", "gradient");
                const QString gradName = mainStyles->insert(m_currentGradientStyle);
                m_currentDrawStyle->addProperty("draw:fill-gradient-name", gradName);
            }

/*            else if (qualifiedName() == QLatin1String("a:tile")) {
                TRY_READ(tile)
                foundTile = true;
            }*/
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL spTree
//! spTree handler (Shape Tree)
/*! ECMA-376, 19.3.1.45, p. 2856

 This element specifies all shape-based objects, either grouped or not, that
 can be referenced on a given slide.

 Parent elements:
    - [done] cSld (§19.3.1.16)
 Child elements:
    - contentPart (Content Part) §19.3.1.14
    - [done] cxnSp (Connection Shape) §19.3.1.19
    - extLst (Extension List with Modification Flag) §19.3.1.20
    - [done] graphicFrame (Graphic Frame) §19.3.1.21
    - [done] grpSp (Group Shape) §19.3.1.22
    - grpSpPr (Group Shape Properties) §19.3.1.23
    - nvGrpSpPr (Non-Visual Properties for a Group Shape) §19.3.1.31
    - [done] pic (Picture) §19.3.1.37
    - [done] sp (Shape) §19.3.1.43
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_spTree()
{
    READ_PROLOGUE

    // Adding extra 'inherited' frames from layout
    if (m_context->type == Slide && m_showSlideLayoutShapes) {
        int index = 0;
        while (index < m_context->slideLayoutProperties->layoutFrames.size()) {
            body->addCompleteElement(m_context->slideLayoutProperties->layoutFrames.at(index).toUtf8());
            ++index;
        }
    }

    QByteArray placeholderEl;
    QBuffer placeholderElBuffer(&placeholderEl);
    placeholderElBuffer.open(QIODevice::WriteOnly);
    m_placeholderElWriter = new KoXmlWriter(&placeholderElBuffer, 0/*indentation*/);
    bool potentiallyAddToLayoutFrames = false;

    QBuffer* shapeBuf = 0;
    KoXmlWriter *shapeWriter = 0;
    KoXmlWriter *bodyBackup = body;

    while (!atEnd()) {
        readNext();
        debugPptx << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (m_context->type == SlideLayout) {
                shapeBuf = new QBuffer;
                shapeWriter = new KoXmlWriter(shapeBuf);
                body = shapeWriter;
            }
            if (qualifiedName() == "p:sp") {
                TRY_READ(sp)
                potentiallyAddToLayoutFrames = true;
            }
            else if (qualifiedName() == "p:grpSp") {
                TRY_READ(grpSp)
                potentiallyAddToLayoutFrames = true;
            }
            else if (qualifiedName() == "p:pic") {
                TRY_READ(pic)
                potentiallyAddToLayoutFrames = true;
            }
            else if (qualifiedName() == "p:graphicFrame") {
                TRY_READ(graphicFrame)
                potentiallyAddToLayoutFrames = true;
            }
            else if (qualifiedName() == "p:cxnSp") {
                TRY_READ(cxnSp)
                potentiallyAddToLayoutFrames = true;
            }
            else if (qualifiedName() == "mc:AlternateContent") {
                TRY_READ(AlternateContent)
                potentiallyAddToLayoutFrames = true;
            }
            else {
                potentiallyAddToLayoutFrames = false;
            }
            if (m_context->type == SlideLayout) {
                // Checking, whether we are in layout. If yes, we may have to forward some shapes to slides.
                // An alternative approach is to put these to masterslides, but it could in practice mean
                // that there are slidemaster * slideLayout masterslides, ie ~40, and it's bit trickier
                if (potentiallyAddToLayoutFrames) {
                    potentiallyAddToLayoutFrames = false;
                    if (!d->phRead) {
                        const QString elementContents = QString::fromUtf8(shapeBuf->buffer(), shapeBuf->buffer().size());
                        m_context->slideLayoutProperties->layoutFrames.push_back(elementContents);
                    }
                }
                delete shapeBuf;
                delete shapeWriter;
            }
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    if (m_context->type == SlideLayout) {
        body = bodyBackup;
    }

    placeholderElBuffer.close();
    m_currentPresentationPageLayoutStyle.addProperty(QString(), QString::fromUtf8(placeholderEl), KoGenStyle::StyleChildElement);

    delete m_placeholderElWriter;
    m_placeholderElWriter = 0;

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL ph
//! ph handler (Placeholder Shape)
/*! ECMA-376, 19.3.1.36, p. 2848
 This element specifies that the corresponding shape should be represented
 by the generating application as a placeholder.

 Parent elements:
 - [done] nvPr (§19.3.1.33)

 Child elements:
 - extLst (Extension List with Modification Flag) §19.3.1.20

 Attributes:
 - hasCustomPrompt (Placeholder has custom prompt)
 - [done] idx (Placeholder Index)
 - orient (Placeholder Orientation)
 - [done] sz (Placeholder Size)
 - [done] type (Placeholder Type)
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_ph()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    d->phRead = true;

    // Specifies the placeholder index. This is used when applying templates or changing
    // layouts to match a placeholder on one template/master to another.
    TRY_READ_ATTR_WITHOUT_NS_INTO(idx, d->phIdx)
    debugPptx << "idx:" << d->phIdx;

    // Specifies the size of a placeholder.
    // The possible values for this attribute are defined by the ST_PlaceholderSize simple type (§19.7.9), p.2987.
    TRY_READ_ATTR_WITHOUT_NS(size)
    debugPptx << "size:" << size;

    // Specifies what content type a placeholder is intended to contain.
    // The possible values for this attribute are defined by the ST_PlaceholderType simple type (§19.7.10), p.2988.
    TRY_READ_ATTR_WITHOUT_NS_INTO(type, d->phType)
    debugPptx << "type:" << d->phType;
    // There is a hardcoded behaviour in MSoffice that ctrTitle refers also to
    // "title".
    if (d->phType == "ctrTitle") {
        d->phType = "title";
    }

    // NOTE: in testing phase
    if (d->phType == "subTitle") {
        d->phType = "body";
    }

    // According to forums, if there is idx, but no type, then type should be default to be body
    if (!d->phIdx.isEmpty() && d->phType.isEmpty() &&
        (m_context->type == SlideLayout || m_context->type == Slide || m_context->type == Notes))
    {
        d->phType = "body";
    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            // TRY_READ_IF(extLst)
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL txBody
//! txBody handler (Shape Text Body)
//! ECMA-376, 19.3.1.51, p. 2862
/*! This element specifies the existence of text to be contained
 within the corresponding shape.

 Parent elements:
 - [done] sp (§19.3.1.43)

 Child elements:
 - [done] bodyPr (Body Properties) §21.1.2.1.1
 - [done] lstStyle (Text List Styles) §21.1.2.4.12
 - [done] p (Text Paragraphs) §21.1.2.2.6

 Only used for Slide type.
*/
//! @todo support all child elements
//! CASE #P526
KoFilter::ConversionStatus PptxXmlSlideReader::read_txBody()
{
    READ_PROLOGUE
    debugPptx << "m_context->type:" << m_context->type;

    m_prevListLevel = 0;
    m_currentListLevel = 0;
    m_pPr_lvl = 0;
    m_continueListNumbering.clear();
    m_prevListStyleName.clear();

    MSOOXML::Utils::XmlWriteBuffer listBuf;
    body = listBuf.setWriter(body);

    while (!atEnd()) {
        readNext();
        debugPptx << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF_NS(a, bodyPr)
            ELSE_TRY_READ_IF_NS(a, lstStyle)
            else if (qualifiedName() == QLatin1String("a:p")) {
                TRY_READ(DrawingML_p);
            }
            ELSE_WRONG_FORMAT
        }
    }

    if (m_prevListLevel > 0) {
        // Ending our current level
        body->endElement(); // text:list
        // Ending any additional levels needed
        for(; m_prevListLevel > 1; --m_prevListLevel) {
            body->endElement(); // text:list-item
            body->endElement(); // text:list
        }
        m_prevListLevel = 0;
    }

    body = listBuf.originalWriter();
    bool createTextBox = false;

    if (m_contentType == "rect" || m_contentType.isEmpty() || unsupportedPredefinedShape()) {
        createTextBox = true;
    }

    if (createTextBox) {
        body->startElement("draw:text-box"); // CASE #P436
    }

    // NOTE: Workaround!  Only in case of a textshape the placeholder flag does
    // hide the placeholder text => Ignoring the placeholder text in case of
    // other shapes (Unspecified presentation shapes are fine).
    if (!createTextBox && !d->phType.isEmpty() &&
        (m_context->type == SlideMaster || m_context->type == SlideLayout))
    {
        listBuf.clear();
    } else {
        body = listBuf.releaseWriter();
    }

    if (createTextBox) {
        body->endElement(); // draw:text-box
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL graphicFrame
//! graphicFrame
//! ECMA-376, 19.3.1.21, p.2835 (PresentationML)
/*! This element specifies the existence of a graphics frame. This
  frame contains a graphic that was generated by an external source
  and needs a container in which to be displayed on the slide surface.

  Parent Elements:
  - [done] grpSp (§4.4.1.19)
  - [done] spTree (§4.4.1.42)

  Child Elements:
  - extLst (Extension List with Modification Flag) (§19.3.1.20)
  - [done] graphic (Graphic Object) (§20.1.2.2.16)
  - [done] nvGraphicFramePr (Non-Visual Properties for a Graphic Frame) (§19.3.1.30)
  - [done] xfrm (2D Transform for Graphic Frame) (§19.3.1.53)
*/
KoFilter::ConversionStatus PptxXmlSlideReader::read_graphicFrame()
{
    READ_PROLOGUE
    m_svgX = m_svgY = m_svgWidth = m_svgHeight = 0;

    MSOOXML::Utils::XmlWriteBuffer buffer;
    body = buffer.setWriter(body);

    // Create a new drawing style for this element
    pushCurrentDrawStyle(new KoGenStyle(KoGenStyle::GraphicAutoStyle, "graphic"));

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF_NS(a, graphic)
            ELSE_TRY_READ_IF(nvGraphicFramePr)
            else if (qualifiedName() == "p:xfrm") {
                read_xfrm_p();
            }
//! @todo add ELSE_WRONG_FORMAT
        }
    }

    body = buffer.originalWriter();

    if (m_context->graphicObjectIsGroup) {
        body->startElement("draw:g");
    } else {
        body->startElement("draw:frame");
    }

    if (m_context->type == SlideMaster || m_context->type == NotesMaster) {
        m_currentDrawStyle->setAutoStyleInStylesDotXml(true);
    }
    const QString styleName(mainStyles->insert(*m_currentDrawStyle, "gr"));
    body->addAttribute("draw:style-name", styleName);

    popCurrentDrawStyle();

    body->addAttribute("draw:name", m_cNvPrName);
    body->addAttribute("svg:y", EMU_TO_CM_STRING(m_svgY));
    if (!m_context->graphicObjectIsGroup) {
        // draw:g has no draw:layer, svg:x, svg:width or svg:height
        body->addAttribute("draw:layer", "layout");
        body->addAttribute("svg:x", EMU_TO_CM_STRING(m_svgX));
        body->addAttribute("svg:width", EMU_TO_CM_STRING(m_svgWidth));
        body->addAttribute("svg:height", EMU_TO_CM_STRING(m_svgHeight));
    }

    (void)buffer.releaseWriter();

    body->endElement(); //draw:g/draw:frame

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL nvGraphicFramePr
//! graphicFramePr handler
/*
 Parent elements:
 - [done] graphicFrame (§19.3.1.21)

 Child elements:
 - cNvGraphicFramePr (Non-Visual Graphic Frame Drawing Properties) §19.3.1.9
 - cNvPr (Non-Visual Drawing Properties) §19.3.1.12
 - [done] nvPr (Non-Visual Properties) §19.3.1.33

*/
KoFilter::ConversionStatus PptxXmlSlideReader::read_nvGraphicFramePr()
{
    READ_PROLOGUE

    // Added to make sure these are 0 for tables etc.
    d->phType.clear();
    d->phIdx.clear();

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (qualifiedName() == "p:cNvPr") {
                read_cNvPr_p();
            }
            // commented atm. for tables because it is not clear what idx type would mean for table
            // ie. if this is uncommented then styles could be taken from bodystyle and that is not wanted
            //ELSE_TRY_READ_IF(nvPr)
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL nvPr
//! nvPr handler (Non-Visual Properties)
/*! ECMA-376, 19.3.1.33, p. 2845
 This element specifies non-visual properties for objects.

 Parent elements:
    - nvCxnSpPr (§19.3.1.29)
    - nvGraphicFramePr (§19.3.1.30)
    - nvGrpSpPr (§19.3.1.31)
    - [done] nvPicPr (§19.3.1.32)
    - [done] nvSpPr (§19.3.1.34)

 Child elements:
    - audioCd (Audio from CD) §20.1.3.1
    - audioFile (Audio from File) §20.1.3.2
    - custDataLst (Customer Data List) §19.3.1.18
    - extLst (Extension List) §19.2.1.12
    - [done] ph (Placeholder Shape) §19.3.1.36
    - quickTimeFile (QuickTime from File) §20.1.3.4
    - videoFile (Video from File) §20.1.3.6
    - wavAudioFile (Audio from WAV File) §20.1.3.7
*/
//! @todo support all child elements
KoFilter::ConversionStatus PptxXmlSlideReader::read_nvPr()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    d->phRead = false;
    d->phType.clear();
    d->phIdx.clear();
    while (!atEnd()) {
        readNext();
        debugPptx << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(ph)
            SKIP_UNKNOWN
//! @todo add ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "p"

#undef CURRENT_EL
#define CURRENT_EL cNvPr
//! p:nvPr handler
KoFilter::ConversionStatus PptxXmlSlideReader::read_cNvPr_p()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    READ_ATTR_WITHOUT_NS_INTO(id, m_cNvPrId)
    TRY_READ_ATTR_WITHOUT_NS_INTO(name, m_cNvPrName)

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL xfrm
//! p:xfrm handler that redirects to a:xfrm
KoFilter::ConversionStatus PptxXmlSlideReader::read_xfrm_p()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (qualifiedName() == QLatin1String("a:off")) {
                TRY_READ(off);
            } else if (qualifiedName() == QLatin1String("a:ext")) {
                TRY_READ(ext);
            }
        }
    }
    READ_EPILOGUE
}

void PptxXmlSlideReader::saveCurrentListStyles()
{
    if (m_currentCombinedBulletProperties.isEmpty()) {
        return;
    }

    if (m_context->type == SlideMaster) {
        //primary style - any other style is applied of top of this one
        if (!d->phType.isEmpty()) {
            if (!m_context->slideMasterProperties->listStyles.contains(d->phType) || d->phIdx.isEmpty()) {
                m_context->slideMasterProperties->listStyles[d->phType] = m_currentCombinedBulletProperties;
            }
        }
        //placeholder style
        if (!d->phIdx.isEmpty()) {
            m_context->slideMasterProperties->listStyles[d->phIdx] = m_currentCombinedBulletProperties;
        }
    }
    else if (m_context->type == SlideLayout) {
        //primary style modification
        if (!d->phType.isEmpty() && d->phIdx.isEmpty()) {
            m_context->slideLayoutProperties->listStyles[d->phType] = m_currentCombinedBulletProperties;
        }
        //placeholder style modification
        if (!d->phIdx.isEmpty()) {
            m_context->slideLayoutProperties->listStyles[d->phIdx] = m_currentCombinedBulletProperties;
        }
    }
#ifdef PPTX_INHERIT_CURRENT_SHAPE_PROPERTIES
    else if (m_context->type == Slide) {
        QString slideIdentifier = d->phType + d->phIdx;
        if (!slideIdentifier.isEmpty()) {
            m_context->currentSlideStyles.listStyles[slideIdentifier] = m_currentCombinedBulletProperties;
        }
    }
#endif
    else if (m_context->type == NotesMaster) {
        if (!d->phType.isEmpty()) {
            m_context->notesMasterProperties->listStyles[d->phType] = m_currentCombinedBulletProperties;
        }
        if (!d->phIdx.isEmpty()) {
            m_context->notesMasterProperties->listStyles[d->phIdx] = m_currentCombinedBulletProperties;
        }
    }
#ifdef PPTX_INHERIT_CURRENT_NOTE_PROPERTIES
    else if (m_context->type == Notes) {
        QString slideIdentifier = d->phType + d->phIdx;
        if (!slideIdentifier.isEmpty()) {
            m_context->currentNotesStyles.listStyles[slideIdentifier] = m_currentCombinedBulletProperties;
        }
    }
#endif
}

void PptxXmlSlideReader::saveCurrentStyles()
{
    if (m_currentCombinedTextStyles.isEmpty()) {
        return;
    }
    if (m_context->type == SlideMaster) {
        //primary style
        if (!d->phType.isEmpty()) {
            if (!m_context->slideMasterProperties->textStyles.contains(d->phType) || d->phIdx.isEmpty()) {
                m_context->slideMasterProperties->textStyles[d->phType] = m_currentCombinedTextStyles;
                m_context->slideMasterProperties->styles[d->phType] = m_currentCombinedParagraphStyles;
            }
        }
        //placeholder style
        if (!d->phIdx.isEmpty()) {
            m_context->slideMasterProperties->textStyles[d->phIdx] = m_currentCombinedTextStyles;
            m_context->slideMasterProperties->styles[d->phIdx] = m_currentCombinedParagraphStyles;
        }
    }
    else if (m_context->type == SlideLayout) {
        //primary style modification
        if (!d->phType.isEmpty() && d->phIdx.isEmpty()) {
            m_context->slideLayoutProperties->textStyles[d->phType] = m_currentCombinedTextStyles;
            m_context->slideLayoutProperties->styles[d->phType] = m_currentCombinedParagraphStyles;
        }
        //placeholder style modification
        if (!d->phIdx.isEmpty()) {
            m_context->slideLayoutProperties->textStyles[d->phIdx] = m_currentCombinedTextStyles;
            m_context->slideLayoutProperties->styles[d->phIdx] = m_currentCombinedParagraphStyles;
        }
    }
#ifdef PPTX_INHERIT_CURRENT_SHAPE_PROPERTIES
    else if (m_context->type == Slide) {
        QString slideIdentifier = d->phType + d->phIdx;
        if (!slideIdentifier.isEmpty()) {
            m_context->currentSlideStyles.textStyles[slideIdentifier] = m_currentCombinedTextStyles;
            m_context->currentSlideStyles.styles[slideIdentifier] = m_currentCombinedParagraphStyles;
        }
    }
#endif
    else if (m_context->type == NotesMaster) {
        if (!d->phType.isEmpty()) {
            m_context->notesMasterProperties->textStyles[d->phType] = m_currentCombinedTextStyles;
            m_context->notesMasterProperties->styles[d->phType] = m_currentCombinedParagraphStyles;
        }
        if (!d->phIdx.isEmpty()) {
            m_context->notesMasterProperties->textStyles[d->phIdx] = m_currentCombinedTextStyles;
            m_context->notesMasterProperties->styles[d->phIdx] = m_currentCombinedParagraphStyles;
        }
    }
#ifdef PPTX_INHERIT_CURRENT_NOTE_PROPERTIES
    else if (m_context->type == Notes) {
        QString slideIdentifier = d->phType + d->phIdx;
        if (!slideIdentifier.isEmpty()) {
            m_context->currentNotesStyles.textStyles[slideIdentifier] = m_currentCombinedTextStyles;
            m_context->currentNotesStyles.styles[slideIdentifier] = m_currentCombinedParagraphStyles;
        }
    }
#endif
}

void PptxXmlSlideReader::saveBodyPropertiesHelper(const QString &id, PptxSlideProperties* slideProperties)
{
    slideProperties->textShapePositions[id] = m_shapeTextPosition;
    slideProperties->textLeftBorders[id] = m_shapeTextLeftOff;
    slideProperties->textRightBorders[id] = m_shapeTextRightOff;
    slideProperties->textTopBorders[id] = m_shapeTextTopOff;
    slideProperties->textBottomBorders[id] = m_shapeTextBottomOff;
    slideProperties->m_textAutoFit[id] = m_normAutofit;
}

void PptxXmlSlideReader::saveBodyProperties()
{
    // Todo: extend this in the future to save other peroperties too
    if (m_context->type == SlideMaster) {
        if (!d->phIdx.isEmpty()) {
            saveBodyPropertiesHelper(d->phIdx, m_context->slideMasterProperties);
        }
        if (!d->phType.isEmpty()) {
            saveBodyPropertiesHelper(d->phType, m_context->slideMasterProperties);
        }
    }
    else if (m_context->type == NotesMaster) {
        if (!d->phIdx.isEmpty()) {
            saveBodyPropertiesHelper(d->phIdx, m_context->notesMasterProperties);
        }
        if (!d->phType.isEmpty()) {
            saveBodyPropertiesHelper(d->phType, m_context->notesMasterProperties);
        }
    }
    else if (m_context->type == SlideLayout) {
        if (!d->phIdx.isEmpty()) {
            saveBodyPropertiesHelper(d->phIdx, m_context->slideLayoutProperties);
        }
        if (!d->phType.isEmpty()) {
            saveBodyPropertiesHelper(d->phType, m_context->slideLayoutProperties);
        }
    }
}

void PptxXmlSlideReader::saveCurrentGraphicStyles()
{
    if (m_context->type == SlideLayout) {
        if (!d->phType.isEmpty()) {
            m_context->slideLayoutProperties->graphicStyles[d->phType] = *m_currentDrawStyle;
        }
        if (!d->phIdx.isEmpty()) {
            m_context->slideLayoutProperties->graphicStyles[d->phIdx] = *m_currentDrawStyle;
        }
    }
    else if (m_context->type == SlideMaster) {
        if (!d->phType.isEmpty()) {
            m_context->slideMasterProperties->graphicStyles[d->phType] = *m_currentDrawStyle;
        }
        if (!d->phIdx.isEmpty()) {
            m_context->slideMasterProperties->graphicStyles[d->phIdx] = *m_currentDrawStyle;
        }
    }
    else if (m_context->type == NotesMaster) {
        if (!d->phType.isEmpty()) {
            m_context->notesMasterProperties->graphicStyles[d->phType] = *m_currentDrawStyle;
        }
        if (!d->phIdx.isEmpty()) {
            m_context->notesMasterProperties->graphicStyles[d->phIdx] = *m_currentDrawStyle;
        }
    }
}

void PptxXmlSlideReader::inheritBodyPropertiesHelper(const QString &id, PptxSlideProperties* slideProperties)
{
    QString position, left, right, top, bottom;

    if (!id.isEmpty()) {
        if (slideProperties->textShapePositions.contains(id)) {
            position = slideProperties->textShapePositions.value(id);
            if (!position.isEmpty()) {
                m_shapeTextPosition = position;
            }
        }
        if (slideProperties->textLeftBorders.contains(id)) {
            left = slideProperties->textLeftBorders.value(id);
            if (!left.isEmpty()) {
                m_shapeTextLeftOff = left;
            }
        }
        if (slideProperties->textRightBorders.contains(id)) {
            right = slideProperties->textRightBorders.value(id);
            if (!right.isEmpty()) {
                m_shapeTextRightOff = right;
            }
        }
        if (slideProperties->textTopBorders.contains(id)) {
            top = slideProperties->textTopBorders.value(id);
            if (!top.isEmpty()) {
                m_shapeTextTopOff = top;
            }
        }
        if (slideProperties->textBottomBorders.contains(id)) {
            bottom = slideProperties->textBottomBorders.value(id);
            if (!bottom.isEmpty()) {
                m_shapeTextBottomOff = bottom;
            }
        }
        if (slideProperties->m_textAutoFit.contains(id)) {
            if (slideProperties->m_textAutoFit.value(id) != MSOOXML::Utils::autoFitUnUsed) {
                if (m_normAutofit == MSOOXML::Utils::autoFitUnUsed) {
                    m_normAutofit = slideProperties->m_textAutoFit.value(id);
                }
            }
        }
    }
}

void PptxXmlSlideReader::inheritBodyProperties()
{
    // TODO: This might not be 100% correct, it is here only temporary until it
    // is figured out what is the correct action plan in the following case:
    // slide phIdx = "2", no bodyPr, slideLayout phIdx = "2", no BodyPr,
    // slideMaster phIdx = "2" phType = "dt", bodyPr For looking at msoffice
    // behavior, the bodyPr from master was not used in the actual slide It was
    // also noted that bullet character that was used in the slide, was either
    // MSOffice default (mentioned nowhere) or somehow gotten from bodyStyles
    // in slidemaster (also not mentioned anywhere why this would happen)
    if (d->phType.isEmpty()) {
        return;
    }

    if (m_context->type == SlideMaster || m_context->type == NotesMaster) {
        return; // Nothing needed for slidemaster / notesmaster
    }

    if (m_context->type == Notes) {
        inheritBodyPropertiesHelper(d->phIdx, m_context->notesMasterProperties);
        inheritBodyPropertiesHelper(d->phType, m_context->notesMasterProperties);
        return;
    }

    // In all non notes cases, we take them first from masterslide
    inheritBodyPropertiesHelper(d->phIdx, m_context->slideMasterProperties);
    inheritBodyPropertiesHelper(d->phType, m_context->slideMasterProperties);


    if (m_context->type == SlideLayout) {
        return; // No further actions needed for layout
    }

    inheritBodyPropertiesHelper(d->phType, m_context->slideLayoutProperties);
    inheritBodyPropertiesHelper(d->phIdx, m_context->slideLayoutProperties);
}

void PptxXmlSlideReader::inheritDefaultListStyles()
{
    int index = 0;
    while (index <  m_context->defaultListStyles.size()) {
#ifdef PPTX_DEBUG_LIST_STYLES
        debugPptx << "==>Inheriting defaultListStyles!";
        debugPptx << "marL:" << m_context->defaultListStyles.at(index).margin();
        debugPptx << "indent:" << m_context->defaultListStyles.at(index).indent();
        debugPptx << "bulletChar" << m_context->defaultListStyles.at(index).bulletChar();
#endif
        m_currentCombinedBulletProperties.insert(index + 1, m_context->defaultListStyles.at(index));
        ++index;
    }
}

void PptxXmlSlideReader::inheritDefaultParagraphStyle(KoGenStyle& targetStyle)
{
    const int copyLevel = qMax(1, m_currentListLevel); // if m_currentListLevel==0 then use level1

    if (m_context->defaultParagraphStyles.size() >= copyLevel) {
        KoGenStyle::copyPropertiesFromStyle(m_context->defaultParagraphStyles[copyLevel-1],
                                                targetStyle, KoGenStyle::ParagraphType);
    }
}

void PptxXmlSlideReader::inheritParagraphStyle(KoGenStyle& targetStyle)
{
    const int copyLevel = qMax(1, m_currentListLevel); // if m_currentListLevel==0 then use level1

    QMap<QString, QMap<int,KoGenStyle> >* map = 0;
    QString id = d->phIdx;
    QString type = d->phType;

    if (id.isEmpty() && type.isEmpty()) {
        //NOTE: Commented out for now, as it seems these properties do not
        //behave the same way
//         type = "other";
    }

    //TODO: Figure out the logic when to inherit from defaults.
    inheritDefaultParagraphStyle(targetStyle);

    if (m_context->type == NotesMaster || m_context->type == Notes) {
        map = &m_context->notesMasterProperties->styles;
        // In all cases, we take them first from masterslide
        if (!id.isEmpty()) {
            if (map->contains(id) && map->value(id).contains(copyLevel)) {
                KoGenStyle::copyPropertiesFromStyle(map->value(id)[copyLevel],
                                                    targetStyle, KoGenStyle::ParagraphType);
            }
        }
        // In all cases, we take them first from masterslide
        if (!type.isEmpty()) {
            if (map->contains(type) && map->value(type).contains(copyLevel)) {
                KoGenStyle::copyPropertiesFromStyle(map->value(type)[copyLevel],
                                                    targetStyle, KoGenStyle::ParagraphType);
            }
        }
#ifdef PPTX_INHERIT_CURRENT_NOTE_PROPERTIES
        if (m_context->type == Notes) {
            QString slideIdentifier = type + id;
            map = &m_context->currentNotesStyles.styles;
            if (!slideIdentifier.isEmpty()) {
                if (map->contains(slideIdentifier) && map->value(slideIdentifier).contains(copyLevel)) {
                    KoGenStyle::copyPropertiesFromStyle(map->value(slideIdentifier)[copyLevel],
                                                        targetStyle, KoGenStyle::ParagraphType);
                }
            }
        }
#endif
        return; // no further actions are needed for notes
    }

    //TODO: Add correct logic here
    // In all cases, we take them first from masterslide
    if (!id.isEmpty()) {
        map = &m_context->slideMasterProperties->styles;
        if (map->contains(id) && map->value(id).contains(copyLevel)) {
            KoGenStyle::copyPropertiesFromStyle(map->value(id)[copyLevel],
                                                targetStyle, KoGenStyle::ParagraphType);
        }
    }
    // In all cases, we take them first from masterslide
    if (!type.isEmpty()) {
        map = &m_context->slideMasterProperties->styles;
        if (map->contains(type) && map->value(type).contains(copyLevel)) {
	    KoGenStyle::copyPropertiesFromStyle(map->value(type)[copyLevel],
                                                targetStyle, KoGenStyle::ParagraphType);
        }
    }
    if (m_context->type == Slide || m_context->type == SlideLayout) {
        map = &m_context->slideLayoutProperties->styles;
        // Perhaps we need to get the properties from layout Slidelayout needs
        // to be here in case there was also lvl1ppr defined
        if (!type.isEmpty()) {
            if (map->contains(type) && map->value(type).contains(copyLevel)) {
                KoGenStyle::copyPropertiesFromStyle(map->value(type)[copyLevel],
                                                    targetStyle, KoGenStyle::ParagraphType);
            }
        }
        // Perhaps we need to get the properties from layout Slidelayout needs
        // to be here in case there was also lvl1ppr defined
        if (!id.isEmpty()) {
            if (map->contains(id) && map->value(id).contains(copyLevel)) {
                KoGenStyle::copyPropertiesFromStyle(map->value(id)[copyLevel],
                                                    targetStyle, KoGenStyle::ParagraphType);
            }
        }
    }
#ifdef PPTX_INHERIT_CURRENT_SHAPE_PROPERTIES
    if (m_context->type == Slide) {
        QString slideIdentifier = type + id;
        map = &m_context->currentSlideStyles.styles;
        if (!slideIdentifier.isEmpty()) {
            if (map->contains(slideIdentifier) && map->value(slideIdentifier).contains(copyLevel)) {
                KoGenStyle::copyPropertiesFromStyle(map->value(slideIdentifier)[copyLevel],
                                                    targetStyle, KoGenStyle::ParagraphType);
            }
        }
    }
#endif
}

void PptxXmlSlideReader::inheritAllTextAndParagraphStyles()
{
    int temp = m_currentListLevel;
    m_currentListLevel = 1;
    KoGenStyle currentTextStyle;
    KoGenStyle currentParagraphStyle;
    while (m_currentListLevel < 10) {
        currentTextStyle = KoGenStyle(KoGenStyle::TextAutoStyle);
        currentParagraphStyle = KoGenStyle(KoGenStyle::ParagraphAutoStyle);
        inheritParagraphStyle(currentParagraphStyle);
        inheritTextStyle(currentTextStyle);
        m_currentCombinedTextStyles[m_currentListLevel] = currentTextStyle;
        m_currentCombinedParagraphStyles[m_currentListLevel] = currentParagraphStyle;
        ++m_currentListLevel;
    }
    m_currentListLevel = temp;
}

void PptxXmlSlideReader::inheritListStyles()
{
    QMap<QString, QMap<int,MSOOXML::Utils::ParagraphBulletProperties> >* map = 0;
    QString id = d->phIdx;
    QString type = d->phType;

    //TODO: Check if this logic also applies for Notes and if to inherit from
    //Defaults at all.
    if (id.isEmpty() && type.isEmpty()) {
        inheritDefaultListStyles();
        type = "other";
    }

#ifdef PPTX_DEBUG_LIST_STYLES
    debugPptx << "==> id:" << id << "| type:" << type;
#endif

    if (m_context->type == NotesMaster || m_context->type == Notes) {
        map = &m_context->notesMasterProperties->listStyles;
        if (!type.isEmpty() && map->contains(type)) {
            QMapIterator<int, MSOOXML::Utils::ParagraphBulletProperties> i(map->value(type));
            while (i.hasNext()) {
                i.next();
                if (i.value().isEmpty()) {
                    m_currentCombinedBulletProperties.insert(i.key(), i.value());
                }
                else {
                    m_currentCombinedBulletProperties[i.key()].addInheritedValues(i.value());
                }
            }
        }
        if (!id.isEmpty() && map->contains(id)) {
            QMapIterator<int, MSOOXML::Utils::ParagraphBulletProperties> i(map->value(id));
            while (i.hasNext()) {
                i.next();
                if (i.value().isEmpty()) {
                    m_currentCombinedBulletProperties.insert(i.key(), i.value());
                }
                else {
                    m_currentCombinedBulletProperties[i.key()].addInheritedValues(i.value());
                }
            }
        }
#ifdef PPTX_INHERIT_CURRENT_NOTE_PROPERTIES
        if (m_context->type == Notes) {
            QString slideIdentifier = type + id;
            map = &m_context->currentNotesStyles.listStyles;
            if (!slideIdentifier.isEmpty() && map->contains(slideIdentifier)) {
                QMapIterator<int, MSOOXML::Utils::ParagraphBulletProperties> i(map->value(slideIdentifier));
                while (i.hasNext()) {
                    i.next();
                    if (i.value().isEmpty()) {
                        m_currentCombinedBulletProperties.insert(i.key(), i.value());
                    }
                    else {
                        m_currentCombinedBulletProperties[i.key()].addInheritedValues(i.value());
                    }
                }
            }
        }
#endif
        return;
    }

    // Masterslide layer
    if (!type.isEmpty() &&
        m_context->slideMasterProperties->listStyles.contains(type))
    {
#ifdef PPTX_DEBUG_LIST_STYLES
        debugPptx << "==> [MasterSlide] type:" << type << "| contains:" <<
                    m_context->slideMasterProperties->listStyles.contains(type);
#endif
        QMapIterator<int, MSOOXML::Utils::ParagraphBulletProperties> i(m_context->slideMasterProperties->listStyles[type]);
        while (i.hasNext()) {
            i.next();
            m_currentCombinedBulletProperties[i.key()].addInheritedValues(i.value());
#ifdef PPTX_DEBUG_LIST_STYLES
            debugPptx << "==>inheriting! | key:" << i.key();
            debugPptx << "| marL:" << m_currentCombinedBulletProperties[i.key()].margin();
            debugPptx << "| indent:" << m_currentCombinedBulletProperties[i.key()].indent();
            debugPptx << "| bulletChar" << m_currentCombinedBulletProperties[i.key()].bulletChar();
            debugPptx << "| bulletColor" << m_currentCombinedBulletProperties[i.key()].bulletColor();
#endif
        }
    }
    if (!id.isEmpty() &&
        m_context->slideMasterProperties->listStyles.contains(id))
    {
#ifdef PPTX_DEBUG_LIST_STYLES
        debugPptx << "==> [MasterSlide] id:" <<  id << "| contains:" <<
                    m_context->slideMasterProperties->listStyles.contains(id);
#endif
        QMapIterator<int, MSOOXML::Utils::ParagraphBulletProperties> i(m_context->slideMasterProperties->listStyles[id]);
        while (i.hasNext()) {
            i.next();
            m_currentCombinedBulletProperties[i.key()].addInheritedValues(i.value());
#ifdef PPTX_DEBUG_LIST_STYLES
            debugPptx << "==>inheriting!" << "| key:" << i.key();
            debugPptx << "| marL:" << m_currentCombinedBulletProperties[i.key()].margin();
            debugPptx << "| indent:" << m_currentCombinedBulletProperties[i.key()].indent();
            debugPptx << "| bulletChar" << m_currentCombinedBulletProperties[i.key()].bulletChar();
            debugPptx << "| bulletColor" << m_currentCombinedBulletProperties[i.key()].bulletColor();
#endif
        }
    }
    // Layout layer
    if (!type.isEmpty()) {
        if ((m_context->type == SlideLayout || m_context->type == Slide) &&
            m_context->slideLayoutProperties->listStyles.contains(type))
        {
#ifdef PPTX_DEBUG_LIST_STYLES
            debugPptx << "=====> [SlideLayout] type:" << type << "| contains:" <<
                        m_context->slideLayoutProperties->listStyles.contains(type);
#endif
            QMapIterator<int, MSOOXML::Utils::ParagraphBulletProperties> i(m_context->slideLayoutProperties->listStyles[type]);
            while (i.hasNext()) {
                i.next();
                m_currentCombinedBulletProperties[i.key()].addInheritedValues(i.value());
#ifdef PPTX_DEBUG_LIST_STYLES
                debugPptx << "====>inheriting!" << "| key:" << i.key();
                debugPptx << "| marL:" << m_currentCombinedBulletProperties[i.key()].margin();
                debugPptx << "| indent:" << m_currentCombinedBulletProperties[i.key()].indent();
                debugPptx << "| bulletChar" << m_currentCombinedBulletProperties[i.key()].bulletChar();
                debugPptx << "| bulletColor" << m_currentCombinedBulletProperties[i.key()].bulletColor();
#endif
            }
        }
    }
    if (!id.isEmpty()) {
        if ((m_context->type == SlideLayout || m_context->type == Slide) &&
            m_context->slideLayoutProperties->listStyles.contains(id))
        {
#ifdef PPTX_DEBUG_LIST_STYLES
            debugPptx << "====> [SlideLayout] id:" << id << "| contains:" <<
                        m_context->slideLayoutProperties->listStyles.contains(id);
#endif
            QMapIterator<int, MSOOXML::Utils::ParagraphBulletProperties> i(m_context->slideLayoutProperties->listStyles[id]);
            while (i.hasNext()) {
                i.next();
                m_currentCombinedBulletProperties[i.key()].addInheritedValues(i.value());
#ifdef PPTX_DEBUG_LIST_STYLES
                debugPptx << "====>inheriting!" << "| key:" << i.key();
                debugPptx << "| marL:" << m_currentCombinedBulletProperties[i.key()].margin();
                debugPptx << "| indent:" << m_currentCombinedBulletProperties[i.key()].indent();
                debugPptx << "| bulletChar" << m_currentCombinedBulletProperties[i.key()].bulletChar();
                debugPptx << "| bulletColor" << m_currentCombinedBulletProperties[i.key()].bulletColor();
#endif
            }
        }
    }

#ifdef PPTX_INHERIT_CURRENT_SHAPE_PROPERTIES
    if (m_context->type == Slide) {
        QString slideIdentifier = type + id;
        map = &m_context->currentSlideStyles.listStyles;
        // Slide layer
        if (!slideIdentifier.isEmpty() && map->contains(slideIdentifier)) {
#ifdef PPTX_DEBUG_LIST_STYLES
            debugPptx << "==> [Slide] identifier:" << slideIdentifier;
#endif
            QMapIterator<int, MSOOXML::Utils::ParagraphBulletProperties> i(map->value(slideIdentifier));
            while (i.hasNext()) {
                i.next();
                m_currentCombinedBulletProperties[i.key()].addInheritedValues(i.value());
#ifdef PPTX_DEBUG_LIST_STYLES
                debugPptx << "====>inheriting!" << "| key:" << i.key();
                debugPptx << "| marL:" << m_currentCombinedBulletProperties[i.key()].margin();
                debugPptx << "| indent:" << m_currentCombinedBulletProperties[i.key()].indent();
                debugPptx << "| bulletChar" << m_currentCombinedBulletProperties[i.key()].bulletChar();
#endif
            }
        }
    }
#endif
}

void PptxXmlSlideReader::inheritDefaultTextStyle(KoGenStyle& targetStyle)
{
    const int listLevel = qMax(1, m_currentListLevel);
    if (m_context->defaultTextStyles.size() >= listLevel) {
        KoGenStyle::copyPropertiesFromStyle(m_context->defaultTextStyles[listLevel-1],
                                                targetStyle, KoGenStyle::TextType);
    }
}

void PptxXmlSlideReader::inheritShapePosition()
{
    //TODO: Why DO NOT overwrite props by phIdx?

    QMap<QString, PptxShapeProperties*>* map = 0;

    // Inheriting shape placement information
    if (!m_xfrm_read) {
        PptxShapeProperties* props = 0;

        // Loading from notes master
        if (m_context->type == Notes) {
            props = m_context->notesMasterProperties->shapesMap.value(d->phType);
            if (!props) {
                props = m_context->notesMasterProperties->shapesMap.value(d->phIdx);
            }
        }
        // Loading from SlideLayout
        if (m_context->type == Slide) {
            map = &m_context->slideLayoutProperties->shapesMap;
#ifdef PPTX_DEBUG_LIST_STYLES
            debugPptx << "==> [SlideLayout] type:" << d->phType << "| contains:" << map->contains(d->phType);
#endif
            if (map->contains(d->phType)) {
                props = map->value(d->phType);
            }
            if (!props) {
#ifdef PPTX_DEBUG_LIST_STYLES
                debugPptx << "==> [SlideLayout] id:" << d->phIdx << "| contains:" << map->contains(d->phIdx);
#endif
                if (map->contains(d->phIdx)) {
                    props = map->value(d->phIdx);
                }
            }
        }
        // Loading from MasterSlide if needed
        if (m_context->type == Slide || m_context->type == SlideLayout) {
            map = &m_context->slideMasterProperties->shapesMap;
            if (!props) {
#ifdef PPTX_DEBUG_LIST_STYLES
                debugPptx << "==> [MasterSlide] type:" << d->phType << "| contains:" << map->contains(d->phType);
#endif
                if (map->contains(d->phType)) {
                    props = map->value(d->phType);
                }
                if (!props) {
#ifdef PPTX_DEBUG_LIST_STYLES
                    debugPptx << "==> [MasterSlide] id:" << d->phIdx << "| contains:" << map->contains(d->phIdx);
#endif
                    if (map->contains(d->phIdx)) {
                        props = map->value(d->phIdx);
                    }
                }
            }
        }
        if (props) {
            m_svgX = props->x;
            m_svgY = props->y;
            m_svgWidth = props->width;
            m_svgHeight = props->height;
            m_rot = props->rot;
            debugPptx << "Copied from PptxShapeProperties:"
                     << "m_svgWidth:" << m_svgWidth << "m_svgHeight:" << m_svgHeight
                     << "m_svgX:" << m_svgX << "m_svgY:" << m_svgY;
        }
    }
}

void PptxXmlSlideReader::inheritShapeGeometry()
{
    // Inheriting shape geometry type (not extends yet)
    if (m_contentType.isEmpty()) {
        if (m_context->type == Notes) {
            m_contentType = m_context->notesMasterProperties->contentTypeMap.value(d->phType);
            if (m_contentType == "custom") {
                m_customEquations = m_contentType = m_context->notesMasterProperties->contentEquations.value(d->phType);
                m_customPath = m_contentType = m_context->notesMasterProperties->contentPath.value(d->phType);
            }
            if (m_contentType.isEmpty()) {
                m_contentType = m_context->notesMasterProperties->contentTypeMap.value(d->phIdx);
                if (m_contentType == "custom") {
                    m_customEquations = m_contentType = m_context->notesMasterProperties->contentEquations.value(d->phIdx);
                    m_customPath = m_contentType = m_context->notesMasterProperties->contentPath.value(d->phIdx);
                }
            }
        }
        if (m_context->type == Slide) {
            m_contentType = m_context->slideLayoutProperties->contentTypeMap.value(d->phType);
            if (m_contentType == "custom") {
                m_customEquations = m_contentType = m_context->slideLayoutProperties->contentEquations.value(d->phType);
                m_customPath = m_contentType = m_context->slideLayoutProperties->contentPath.value(d->phType);
            }
            if (m_contentType.isEmpty()) {
                m_contentType = m_context->slideLayoutProperties->contentTypeMap.value(d->phIdx);
                if (m_contentType == "custom") {
                    m_customEquations = m_contentType = m_context->slideLayoutProperties->contentEquations.value(d->phIdx);
                    m_customPath = m_contentType = m_context->slideLayoutProperties->contentPath.value(d->phIdx);
                }
            }
        }
        if (m_context->type == Slide || m_context->type == SlideLayout) {
            if (m_contentType.isEmpty()) {
                m_contentType = m_context->slideMasterProperties->contentTypeMap.value(d->phType);
                if (m_contentType == "custom") {
                    m_customEquations = m_contentType = m_context->slideMasterProperties->contentEquations.value(d->phType);
                    m_customPath = m_contentType = m_context->slideMasterProperties->contentPath.value(d->phType);
                }
                if (m_contentType.isEmpty()) {
                    m_contentType = m_context->slideMasterProperties->contentTypeMap.value(d->phIdx);
                    if (m_contentType == "custom") {
                        m_customEquations = m_contentType = m_context->slideMasterProperties->contentEquations.value(d->phIdx);
                        m_customPath = m_contentType = m_context->slideMasterProperties->contentPath.value(d->phIdx);
                    }
                }
            }
        }
    }

    bool drawingStyleInherited = false;
    KoGenStyle inheritedStyle;
    // Inheriting drawing style, this can be outline or fill style
    if (m_context->type == Notes) {
        inheritedStyle = m_context->notesMasterProperties->graphicStyles.value(d->phType);
        if (!inheritedStyle.isEmpty()) {
            KoGenStyle::copyPropertiesFromStyle(inheritedStyle, *m_currentDrawStyle, KoGenStyle::GraphicType);
        }
        else {
            inheritedStyle = m_context->notesMasterProperties->graphicStyles.value(d->phIdx);
            if (!inheritedStyle.isEmpty()) {
                KoGenStyle::copyPropertiesFromStyle(inheritedStyle, *m_currentDrawStyle, KoGenStyle::GraphicType);
            }
        }
    }
    if (m_context->type == Slide) {
        inheritedStyle = m_context->slideLayoutProperties->graphicStyles.value(d->phType);
        if (!inheritedStyle.isEmpty()) {
            drawingStyleInherited = true;
            KoGenStyle::copyPropertiesFromStyle(inheritedStyle, *m_currentDrawStyle, KoGenStyle::GraphicType);
        }
        else {
            inheritedStyle = m_context->slideLayoutProperties->graphicStyles.value(d->phIdx);
            if (!inheritedStyle.isEmpty()) {
                drawingStyleInherited = true;
                KoGenStyle::copyPropertiesFromStyle(inheritedStyle, *m_currentDrawStyle, KoGenStyle::GraphicType);
            }
        }
    }
    if ((!drawingStyleInherited && m_context->type == Slide) || m_context->type == SlideLayout) {
        inheritedStyle = m_context->slideMasterProperties->graphicStyles.value(d->phType);
        if (!inheritedStyle.isEmpty()) {
            KoGenStyle::copyPropertiesFromStyle(inheritedStyle, *m_currentDrawStyle, KoGenStyle::GraphicType);
        }
        else {
            inheritedStyle = m_context->slideMasterProperties->graphicStyles.value(d->phIdx);
            if (!inheritedStyle.isEmpty()) {
                KoGenStyle::copyPropertiesFromStyle(inheritedStyle, *m_currentDrawStyle, KoGenStyle::GraphicType);
            }
        }
    }
}

void PptxXmlSlideReader::inheritTextStyle(KoGenStyle& targetStyle)
{
    const int listLevel = qMax(1, m_currentListLevel); // if m_currentListLevel==0 then use level1

    QMap<QString, QMap<int,KoGenStyle> >* map = 0;
    QString id = d->phIdx;
    QString type = d->phType;

    if (id.isEmpty() && type.isEmpty()) {
        inheritDefaultTextStyle(targetStyle);
        type = "other";
    }

    if (m_context->type == Notes || m_context->type == NotesMaster) {
        map = &m_context->notesMasterProperties->textStyles;
        if (!id.isEmpty()) {
            if (map->contains(id) && map->value(id).contains(listLevel)) {
                KoGenStyle::copyPropertiesFromStyle(map->value(id)[listLevel],
                                                    targetStyle, KoGenStyle::TextType);
            }
        }
        if (!type.isEmpty()) {
            if (map->contains(type) && map->value(type).contains(listLevel)) {
                KoGenStyle::copyPropertiesFromStyle(map->value(type)[listLevel],
                                                    targetStyle, KoGenStyle::TextType);
            }
        }
#ifdef PPTX_INHERIT_CURRENT_NOTE_PROPERTIES
        if (m_context->type == Notes) {
            QString slideIdentifier = type + id;
            map = &m_context->currentNotesStyles.textStyles;
            if (!slideIdentifier.isEmpty()) {
                if (map->contains(slideIdentifier) && map->value(slideIdentifier).contains(listLevel)) {
                    KoGenStyle::copyPropertiesFromStyle(map->value(slideIdentifier)[listLevel],
                                                        targetStyle, KoGenStyle::TextType);
                }
            }
        }
#endif
        return;
    }

    // We must apply properties outside rpr, since it is possible that we do
    // not enter rpr at all.

    if (!type.isEmpty()) {
#ifdef PPTX_DEBUG_TEXT_STYLES
	debugPptx << "==> [MasterSlide] type:" << type << "| contains:" <<
                    m_context->slideMasterProperties->textStyles.contains(type);
#endif
        if (m_context->slideMasterProperties->textStyles.contains(type)) {
#ifdef PPTX_DEBUG_TEXT_STYLES
            debugPptx << "listLevel:" << listLevel << "| contains:" <<
                        m_context->slideMasterProperties->textStyles[type].contains(listLevel);
#endif
            if (m_context->slideMasterProperties->textStyles[type].contains(listLevel)) {
                KoGenStyle::copyPropertiesFromStyle(m_context->slideMasterProperties->textStyles[type][listLevel],
                                                targetStyle, KoGenStyle::TextType);
#ifdef PPTX_DEBUG_TEXT_STYLES
                KoGenStyle* style = &targetStyle;
                debugPptx << "font-size:" << style->property("fo:font-size", KoGenStyle::TextType);
                debugPptx << "font-style:" << style->property("fo:font-style", KoGenStyle::TextType);
                debugPptx << "font-variant:" << style->property("fo:font-variant", KoGenStyle::TextType);
                debugPptx << "font-weight:" << style->property("fo:font-weight", KoGenStyle::TextType);
#endif
	    }
        }
    }
    if (!id.isEmpty()) {
#ifdef PPTX_DEBUG_TEXT_STYLES
	debugPptx << "==> [MasterSlide] id:" << id << "| contains:" <<
                    m_context->slideMasterProperties->textStyles.contains(id);
#endif
        if (m_context->slideMasterProperties->textStyles.contains(id)) {
#ifdef PPTX_DEBUG_TEXT_STYLES
            debugPptx << "listLevel:" << listLevel << "| contains:" <<
                        m_context->slideMasterProperties->textStyles[id].contains(listLevel);
#endif
            if (m_context->slideMasterProperties->textStyles[id].contains(listLevel)) {
                KoGenStyle::copyPropertiesFromStyle(m_context->slideMasterProperties->textStyles[id][listLevel],
                                                targetStyle, KoGenStyle::TextType);
#ifdef PPTX_DEBUG_TEXT_STYLES
                KoGenStyle* style = &targetStyle;
                debugPptx << "font-size:" << style->property("fo:font-size", KoGenStyle::TextType);
                debugPptx << "font-style:" << style->property("fo:font-style", KoGenStyle::TextType);
                debugPptx << "font-variant:" << style->property("fo:font-variant", KoGenStyle::TextType);
                debugPptx << "font-weight:" << style->property("fo:font-weight", KoGenStyle::TextType);
#endif
	    }
        }
    }

    // NOTE: Workaround!  Themes support is not perfect at the moment so DO NOT
    // inherit fo:color from SlideMaster when a color mapping override was
    // applied.  The p:txStyles element content requires an update.
    if ((m_context->type == Slide || m_context->type == SlideLayout) &&
        (m_context->slideLayoutProperties->overrideClrMapping)) {
        targetStyle.removeProperty("fo:color", KoGenStyle::TextType);

        // Theme specific default colors should be used until we get correct
        // style:use-window-font-color support.
        if (m_context->type == Slide) {
            const int listLevel = qMax(1, m_currentListLevel);
            if (m_context->defaultTextStyles.size() >= listLevel) {
                QString textColor = m_context->defaultTextStyles[listLevel-1].property("fo:color", KoGenStyle::TextType);
                if (!textColor.isEmpty()) {
                    targetStyle.addProperty("fo:color", textColor, KoGenStyle::TextType);
                }
            }
        }
    }

    //Reset the text formatting inherited from the Master Slide.
    if (!type.isEmpty()) {
        if (m_context->type == Slide || m_context->type == SlideLayout) {
#ifdef PPTX_DEBUG_TEXT_STYLES
            debugPptx << "==> [SlideLayout] type:" << type << "| contains:" <<
                        m_context->slideLayoutProperties->textStyles.contains(type);
#endif
            if (m_context->slideLayoutProperties->textStyles.contains(type)) {
#ifdef PPTX_DEBUG_TEXT_STYLES
                debugPptx << "listLevel:" << listLevel << "| contains:" <<
                            m_context->slideLayoutProperties->textStyles[type].contains(listLevel);
#endif
                if (m_context->slideLayoutProperties->textStyles[type].contains(listLevel)) {
                    KoGenStyle::copyPropertiesFromStyle(m_context->slideLayoutProperties->textStyles[type][listLevel],
                                                    targetStyle, KoGenStyle::TextType);
#ifdef PPTX_DEBUG_TEXT_STYLES
                    KoGenStyle* style = &targetStyle;
                    debugPptx << "font-size:" << style->property("fo:font-size", KoGenStyle::TextType);
                    debugPptx << "font-style:" << style->property("fo:font-style", KoGenStyle::TextType);
                    debugPptx << "font-variant:" << style->property("fo:font-variant", KoGenStyle::TextType);
                    debugPptx << "font-weight:" << style->property("fo:font-weight", KoGenStyle::TextType);
#endif
                }
            }
            else if ((type == "body") &&
                     m_context->slideMasterProperties->textStyles.contains(type))
            {
                if (m_context->slideMasterProperties->textStyles[type].contains(listLevel)) {
                    targetStyle = m_context->slideMasterProperties->textStyles[type][listLevel];

#ifdef PPTX_DEBUG_TEXT_STYLES
                    debugPptx << "==> [RESET/SlideMaster] type:" << type;
                    KoGenStyle* style = &targetStyle;
                    debugPptx << "font-size:" << style->property("fo:font-size", KoGenStyle::TextType);
                    debugPptx << "font-style:" << style->property("fo:font-style", KoGenStyle::TextType);
                    debugPptx << "font-variant:" << style->property("fo:font-variant", KoGenStyle::TextType);
                    debugPptx << "font-weight:" << style->property("fo:font-weight", KoGenStyle::TextType);
#endif
                }
            }
        }
    }

    if (!id.isEmpty()) {
        if (m_context->type == Slide || m_context->type == SlideLayout) {
#ifdef PPTX_DEBUG_TEXT_STYLES
            debugPptx << "==> [SlideLayout] id:" << id << "| contains:" <<
                        m_context->slideLayoutProperties->textStyles.contains(id);
#endif
            if (m_context->slideLayoutProperties->textStyles.contains(id)) {
#ifdef PPTX_DEBUG_TEXT_STYLES
                debugPptx << "listLevel:" << listLevel << "| contains:" <<
                            m_context->slideLayoutProperties->textStyles[id].contains(listLevel);
#endif
                if (m_context->slideLayoutProperties->textStyles[id].contains(listLevel)) {
                    KoGenStyle::copyPropertiesFromStyle(m_context->slideLayoutProperties->textStyles[id][listLevel],
                                            targetStyle, KoGenStyle::TextType);
#ifdef PPTX_DEBUG_TEXT_STYLES
                    KoGenStyle* style = &targetStyle;
                    debugPptx << "font-size:" << style->property("fo:font-size", KoGenStyle::TextType);
                    debugPptx << "font-style:" << style->property("fo:font-style", KoGenStyle::TextType);
                    debugPptx << "font-variant:" << style->property("fo:font-variant", KoGenStyle::TextType);
                    debugPptx << "font-weight:" << style->property("fo:font-weight", KoGenStyle::TextType);
#endif
                }
            }
        }
    }

    // NOTE: Workaround!  Themes support is not perfect at the moment so DO NOT
    // inherit fo:color from slideMaster/slideLayout when a color mapping
    // override was applied. (disabled at the moment)
//     if (m_context->type == Slide && m_context->overrideClrMapping) {
//         targetStyle.removeProperty("fo:color", KoGenStyle::TextType);

//         //  default text color
//         const int listLevel = qMax(1, m_currentListLevel);
//         if (m_context->defaultTextStyles.size() >= listLevel) {
//             QString textColor = m_context->defaultTextStyles[listLevel-1].property("fo:color", KoGenStyle::TextType);
//             if (!textColor.isEmpty()) {
//                targetStyle.addProperty("fo:color", KoGenStyle::TextType);
//             }
//         }
//     }

#ifdef PPTX_INHERIT_CURRENT_SHAPE_PROPERTIES
    if (m_context->type == Slide) {
        QString slideIdentifier = type + id;
        map = &m_context->currentSlideStyles.textStyles;
        if (!slideIdentifier.isEmpty()) {
            if (map->contains(slideIdentifier) && map->value(slideIdentifier).contains(listLevel)) {
                KoGenStyle::copyPropertiesFromStyle(map->value(slideIdentifier)[listLevel],
                                                    targetStyle, KoGenStyle::TextType);
            }
        }
    }
#endif
}

KoFilter::ConversionStatus PptxXmlSlideReader::generatePlaceHolderSp()
{
    debugPptx << "d->phType:" << d->phType << "d->phIdx:" << d->phIdx;

    if (m_context->type == SlideLayout) {
        PptxShapeProperties* masterShapeProperties = 0;
        masterShapeProperties = m_context->slideMasterProperties->shapesMap.value(d->phType);
        if (!masterShapeProperties) {
            masterShapeProperties = m_context->slideMasterProperties->shapesMap.value(d->phIdx);
        }
        debugPptx << "masterShapeProperties:" << masterShapeProperties;

        if (masterShapeProperties) {
            m_currentShapeProperties = new PptxShapeProperties(*masterShapeProperties);
        } else { // Case where it was not present in master slide at all
            m_currentShapeProperties = new PptxShapeProperties;
        }
        // If element was present, then we can use values from the slidelayout
        if (m_xfrm_read) {
            m_currentShapeProperties->x = m_svgX;
            m_currentShapeProperties->y = m_svgY;
            m_currentShapeProperties->width = m_svgWidth;
            m_currentShapeProperties->height = m_svgHeight;
            m_currentShapeProperties->rot = m_rot;
        }
        //primary style modification
        if (!d->phType.isEmpty() && d->phIdx.isEmpty()) {
            m_context->slideLayoutProperties->shapesMap[d->phType] = m_currentShapeProperties;
        }
        //TODO: Add the logic here!
        if (!d->phType.isEmpty()) {
            m_context->slideLayoutProperties->contentTypeMap[d->phType] = m_contentType;
            m_context->slideLayoutProperties->contentPath[d->phType] = m_customPath;
            m_context->slideLayoutProperties->contentEquations[d->phType] = m_customEquations;
        }
        //placeholder style
        if (!d->phIdx.isEmpty()) {
            m_context->slideLayoutProperties->shapesMap[d->phIdx] = m_currentShapeProperties;
            m_context->slideLayoutProperties->contentTypeMap[d->phIdx] = m_contentType;
            m_context->slideLayoutProperties->contentPath[d->phIdx] = m_customPath;
            m_context->slideLayoutProperties->contentEquations[d->phIdx] = m_customEquations;
        }
        // presentation:placeholder
        Q_ASSERT(m_placeholderElWriter);
        QString presentationObject = MSOOXML::Utils::ST_PlaceholderType_to_ODF(d->phType);

        m_placeholderElWriter->startElement("presentation:placeholder");
        m_placeholderElWriter->addAttribute("presentation:object", presentationObject);
        if (m_rot == 0) {
            m_placeholderElWriter->addAttribute("svg:x", EMU_TO_CM_STRING(m_svgX));
            m_placeholderElWriter->addAttribute("svg:y", EMU_TO_CM_STRING(m_svgY));
        }
        m_placeholderElWriter->addAttribute("svg:width", EMU_TO_CM_STRING(m_svgWidth));
        m_placeholderElWriter->addAttribute("svg:height", EMU_TO_CM_STRING(m_svgHeight));
        if (m_rot != 0) {
            qreal angle, xDiff, yDiff;
            MSOOXML::Utils::rotateString(m_rot, m_svgWidth, m_svgHeight, angle, xDiff, yDiff);
            QString rotString = QString("rotate(%1) translate(%2cm %3cm)")
                                .arg(angle).arg((m_svgX + xDiff)/360000).arg((m_svgY + yDiff)/360000);
            m_placeholderElWriter->addAttribute("draw:transform", rotString);

        }

        m_placeholderElWriter->endElement();
    }
    else if (m_context->type == SlideMaster) {
        // If element was present, then we can use values from the actual slidemaster
        if (m_xfrm_read) {
            m_currentShapeProperties->x = m_svgX;
            m_currentShapeProperties->y = m_svgY;
            m_currentShapeProperties->width = m_svgWidth;
            m_currentShapeProperties->height = m_svgHeight;
            m_currentShapeProperties->rot = m_rot;
        }
	//primary style
        if (!d->phType.isEmpty()) {
            if (!m_context->slideMasterProperties->shapesMap.contains(d->phType)) {
                m_context->slideMasterProperties->shapesMap[d->phType] = m_currentShapeProperties;
            }
            m_context->slideMasterProperties->contentTypeMap[d->phType] = m_contentType;
            m_context->slideMasterProperties->contentPath[d->phType] = m_customPath;
            m_context->slideMasterProperties->contentEquations[d->phType] = m_customEquations;
        }
        //placeholder style
        if (!d->phIdx.isEmpty()) {
            m_context->slideMasterProperties->shapesMap[d->phIdx] = m_currentShapeProperties;
            m_context->slideMasterProperties->contentTypeMap[d->phIdx] = m_contentType;
            m_context->slideMasterProperties->contentPath[d->phIdx] = m_customPath;
            m_context->slideMasterProperties->contentEquations[d->phIdx] = m_customEquations;
        }
    }
    else if (m_context->type == NotesMaster) {
        if (m_xfrm_read) {
            m_currentShapeProperties->x = m_svgX;
            m_currentShapeProperties->y = m_svgY;
            m_currentShapeProperties->width = m_svgWidth;
            m_currentShapeProperties->height = m_svgHeight;
            m_currentShapeProperties->rot = m_rot;
        }
        if (!d->phType.isEmpty()) {
            m_context->notesMasterProperties->shapesMap[d->phType] = m_currentShapeProperties;
            m_context->notesMasterProperties->contentTypeMap[d->phType] = m_contentType;
            m_context->notesMasterProperties->contentPath[d->phType] = m_customPath;
            m_context->notesMasterProperties->contentEquations[d->phType] = m_customEquations;
        }
        if (!d->phIdx.isEmpty()) {
            m_context->notesMasterProperties->shapesMap[d->phIdx] = m_currentShapeProperties;
            m_context->notesMasterProperties->contentTypeMap[d->phIdx] = m_contentType;
            m_context->notesMasterProperties->contentPath[d->phIdx] = m_customPath;
            m_context->notesMasterProperties->contentEquations[d->phIdx] = m_customEquations;
        }
    }

    m_currentShapeProperties = 0; // Making sure that nothing uses them.
    return KoFilter::OK;
}

qreal
PptxXmlSlideReader::processParagraphSpacing(const qreal margin, const qreal fontSize)
{
    // MS PowerPoint specific: font-independent-line-spacing is used, which
    // means that line height is calculated only from the font height as
    // specified by the font size properties.  If a number of font sizes are
    // used in a paragraph, then use the minimum.
    //
    // lineHeight = fontSize + (1/4 * fontSize);
    //
    // margin-top/margin-bottom are calculated based on the lineHeight.
    //
    qreal lineHeight = fontSize + (0.25 * fontSize);
    return (margin * lineHeight) / 100;
}

// END NAMESPACE p

#undef MSOOXML_CURRENT_NS
#define MSOOXML_CURRENT_NS "a"

#include <MsooXmlCommonReaderImpl.h> // this adds a:p, a:pPr, a:t, a:r, etc.

#define DRAWINGML_NS "a"
#define DRAWINGML_PIC_NS "p" // DrawingML/Picture

#include <MsooXmlCommonReaderDrawingMLImpl.h> // this adds p:pic, etc.
#include <MsooXmlDrawingReaderTableImpl.h> //this adds a:tbl
