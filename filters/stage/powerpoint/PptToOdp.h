/* This file is part of the KDE project
   Copyright (C) 2005 Yolla Indria <yolla.indria@gmail.com>
   Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
   Contact: Amit Aggarwal <amitcs06@gmail.com>
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>
   Copyright (C) 2010, 2011 Matus Uzak <matus.uzak@ixonos.com>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef PPTTOODP_H
#define PPTTOODP_H

#include "generated/simpleParser.h"
#include "pptstyle.h"
#include "drawstyle.h"
#include "writer.h"
#include "DateTimeFormat.h"
#include "ParsedPresentation.h"

#include <KoFilter.h>
#include <KoStore.h>

#include <QStack>
#include <QPair>
#include <QRectF>
#include <QColor>

class KoGenStyles;
class KoGenStyle;

class PowerPointImport;
class ODrawToOdf;

/**
 * Converts PPT files to ODP files.
 * This internal class is there to separate the koconverter plugin interface
 * from the koconverter plugin. This allows the conversion code to be reused
 * in other programs without plugin overhead.
 * The converter is re-entrant: if you want to conversions in parallel,
 * you need multiple instances of PptToOdp.
 */
class PptToOdp
{
private:
    class DrawClient;
public:
    /**
     * Constructs a converter.
     *
     * @param address of the filter instance
     * @param address of the setProgress member f. of the filter instance
     */
    PptToOdp(PowerPointImport* filter, void (PowerPointImport::*setProgress)(const int));
    /**
     * Destroy the converter.
     */
    ~PptToOdp();
    /**
     * Convert a ppt file to an odp file.
     *
     * @param inputfile PowerPoint file to read.
     * @param ODF file to write.
     * @param storeType type of storage format the ODF file should be stored in.
     * @return result code of the conversion.
     */
    KoFilter::ConversionStatus convert(const QString& inputfile,
                                       const QString& outputfile,
                                       KoStore::Backend storeType);
    /**
     * Convert ppt data to odp data.
     *
     * @param input an open OLE container that contains the ppt data.
     * @param output an open KoStore to write the odp into.
     * @return result code of the conversion.
     */
    KoFilter::ConversionStatus convert(POLE::Storage& input,
                                       KoStore* output);

    /**
     * Get path to an already stored picture based on its identifier.
     *
     * @param pib specifies which BLIP to display in the picture shape.
     * @return path
     */
    QString getPicturePath(const quint32 pib) const;

    class TextListTag;
    typedef QStack<TextListTag> ListStack;
private:

    /**
    * @brief Struct that contains precalculated style names based on
    * TextCFException and TextPFException combinations.
    *
    * For each individual character in this object's text three styles apply:
    * Paragraph style, List style and Character style. These are parsed from
    * TextCFException and TextPFException. For each character there is a
    * corresponding pair of TextCFException and TextPFException.
    *
    * Saving of styles is done before saving text contents so we'll cache
    * the style names and pairs of TextCFException and TextPFException.
    */
    class StyleName
    {
    public:
        /**
        * @brief Text style name (e.g. T1)
        *
        */
        QString text;

        /**
        * @brief Paragraph style (e.g. P1)
        *
        */
        QString paragraph;

        /**
        * @brief List style (e.g. L1)
        *
        */
        QString list;
        StyleName() {}
        StyleName(const QString& t, const QString& p, const QString& l)
                : text(t), paragraph(p), list(l) {}
    };

    /**
     * Structure that influences all information that affects the style of type
     * TextStyle.  This is a convenience container for passing this
     * information.
     **/
    struct ListStyleInput {
    public:
        const PptTextPFRun& pf;
        const PptTextCFRun& cf; //deprecated
        const MSO::TextCFException* cf_; //deprecated
        const MSO::TextCFException9* cf9;
        const MSO::TextCFException10* cf10;
        const MSO::TextSIException* si;

        ListStyleInput(const PptTextPFRun& pf, const PptTextCFRun& cf)
            :pf(pf), cf(cf), cf_(0), cf9(0), cf10(0), si(0) {}
    };

    /**
      * @brief An enumeration that specifies an action that can be performed
      * when interacting with an object during a slide show.
      */
    enum {
        /**
          * @brief No effect.
          */
        II_NoAction,

        /**
          * @brief A macro is executed.
          */
        II_MacroAction,

        /**
          * @brief A program is run.
          */
        II_RunProgramAction,

        /**
          * @brief The current presentation slide of the slide show jumps to
          * another presentation slide in the same presentation.
          */
        II_JumpAction,

        /**
          * @brief A URL is executed.
          */
        II_HyperlinkAction,

        /**
          * @brief An OLE action (only valid if the object this applies to is an
          * OLE embedded object).
          */
        II_OLEAction,

        /**
          * @brief A media object is played.
          */
        II_MediaAction,

        /**
          * @brief A named show is displayed.
          */
        II_CustomShowAction,
    }InteractiveInfoActionEnum;

    /**
    * Declaration Type
    * Referenced by: Declaration Type
    * A declaration type ex:- Header,Footer,DateTime
    */
    enum DeclarationType {
        Footer,
        Header,
        DateTime
    };

    /**
     * Function that does the actual conversion.
     *
     * It is shared by the two convert() functions.
     * @param output an open KoStore to write the odp into.
     * @return result code of the conversion.
     */
    KoFilter::ConversionStatus doConversion(KoStore* output);

    /**
     * TODO:
     */
    bool parse(POLE::Storage& storage);

    void createMainStyles(KoGenStyles& styles);
    void defineDefaultTextStyle(KoGenStyles& styles);
    void defineDefaultParagraphStyle(KoGenStyles& styles);
    void defineDefaultSectionStyle(KoGenStyles& styles);
    void defineDefaultRubyStyle(KoGenStyles& styles);
    void defineDefaultTableStyle(KoGenStyles& styles);
    void defineDefaultTableColumnStyle(KoGenStyles& styles);
    void defineDefaultTableRowStyle(KoGenStyles& styles);
    void defineDefaultTableCellStyle(KoGenStyles& styles);
    void defineDefaultGraphicStyle(KoGenStyles& styles);
    void defineDefaultPresentationStyle(KoGenStyles& styles);
    void defineDefaultDrawingPageStyle(KoGenStyles& styles);
    void defineDefaultChartStyle(KoGenStyles& styles);

    /**
     * define automatic styles for text, paragraphs, graphic and presentation
     * families
     */
    void defineMasterStyles(KoGenStyles& styles);
    void defineAutomaticDrawingPageStyles(KoGenStyles& styles);

    // we assume that these functions are the same for all style families
    void defineDefaultTextProperties(KoGenStyle& style);
    void defineDefaultParagraphProperties(KoGenStyle& style);
    void defineDefaultGraphicProperties(KoGenStyle& style, KoGenStyles& styles);

    /**
     * Extract data from TextCFException into the style
     * @param KoGenStyle of type TextAutoStyle/TextStyle
     * @param PptTextCFRun address
     * @param TextCFException9 address
     * @param TextCFException10 address
     * @param TextSIException address
     * @param chunk might represent a symbol
     */
    void defineTextProperties(KoGenStyle& style,
                              const PptTextCFRun& cf,
                              const MSO::TextCFException9* cf9,
                              const MSO::TextCFException10* cf10,
                              const MSO::TextSIException* si,
                              const bool isSymbol = false);

    /**
     * Extract data from TextPFException into the style
     * @param style KoGenStyle of type ParagraphAutoStyle/ParagraphStyle
     * @param pf PptTextPFRun
     * @param fs minimal size of the font used in the paragraph
     */
    void defineParagraphProperties(KoGenStyle& style,
                                   const PptTextPFRun& pf,
                                   const quint16 fs);

    /**
     * Extract data into the drawing-page style
     * @param KoGenStyle
     * @param DrawStyle
     * @param KoGenStyles
     * @param ODrawToOdf
     * @param pointer to a HeadersFootersAtom
     * @param pointer to SlideFlags of presentation or notes slide
     */
    void defineDrawingPageStyle(KoGenStyle& style, const DrawStyle& ds, KoGenStyles& styles,
                                ODrawToOdf& odrawtoodf,
                                const MSO::HeadersFootersAtom* hf,
                                const MSO::SlideFlags* sf = nullptr);

    /* Extract data into the style of type ListAutoStyle/ListStyle */
    void defineListStyle(KoGenStyle& style,
                         const quint32 textType,
                         const MSO::TextMasterStyleAtom& levels,
                         const MSO::TextMasterStyle9Atom* levels9 = 0,
                         const MSO::TextMasterStyle10Atom* levels10 = 0);

    void defineListStyle(KoGenStyle& style,
                         const quint32 textType,
                         const quint16 indentLevel,
                         const MSO::TextMasterStyleLevel* level = 0,
                         const MSO::TextMasterStyle9Level* level9 = 0,
                         const MSO::TextMasterStyle10Level* level10 = 0);

    void defineListStyle(KoGenStyle& style,
                         const quint16 indentLevel,
                         const ListStyleInput& info);

    void defineListStyleProperties(KoXmlWriter& out, bool imageBullet,
                                   const QString& bulletSize,
                                   const PptTextPFRun& pf);

    void defineListStyleTextProperties(KoXmlWriter& out_,
                                       const QString& bulletSize,
                                       const PptTextPFRun& pf);

    /**
     * TODO:
     * @param
     * @param
     * @param
     * @return name of the created style as stored in the styles collection
     */
    QString defineAutoListStyle(Writer& out, const PptTextPFRun& pf, const PptTextCFRun& cf);

    const MSO::StyleTextProp9* getStyleTextProp9(quint32 slideIdRef,
                                                 quint32 textType, quint8 pp9rt);

    const MSO::TextContainer* getTextContainer(const MSO::PptOfficeArtClientTextBox* ctb,
                                               const MSO::PptOfficeArtClientData* cd) const;
    quint32 getTextType(const MSO::PptOfficeArtClientTextBox* ctb,
                        const MSO::PptOfficeArtClientData* cd) const;

    void addPresentationStyleToDrawElement(Writer& out, const MSO::OfficeArtSpContainer& o);

    /**
     * Create office:document-content XML tree to be saved into the content.xml file.
     */
    QByteArray createContent(KoGenStyles& styles);

    /**
     * Create office:document-meta XML tree to be saved into the meta.xml file.
     */
    QByteArray createMeta();

    void processSlideForBody(unsigned slideNo, Writer& out);

    /**
     * Process the content of a TextContainer.
     *
     * Each shape type allowed to contain text should have a TextContainer in
     * its OfficeArtSpContainer.  TextContainer stores the text, its formatting
     * properties and everything else related to the text like bokkmarks, etc.
     * TextContainer is our own convenience structure to group all the smaller
     * containers providing text related data.
     *
     * @param out Writer
     * @param cd provides access to additional text formatting in StyleTextProp9Atom
     * @param tc provides access to text formatting in MasterTextPropAtom and TextType
     * @param tr specifies tabbing, horizontal margins, and indentation for text
     * @return 0 (OK), -1 (TextContainer missing)
     */
    int processTextForBody(Writer& out,
                           const MSO::OfficeArtClientData* cd,
                           const MSO::TextContainer* tc,
                           const MSO::TextRuler* tr,
                           const bool isPlaceholder);

    /**
     * Add a text:list-item into a newly created text:list with corresponding
     * number of levels and set automatic numbering related attributes.
     */
    void addListElement(KoXmlWriter& out,
                        const QString& listStyle,
                        ListStack& levels,
                        quint16 level,
                        const PptTextPFRun &pf);

    /**
     * Process a span or the smallest run of text having it's own formatting.
     *
     * @return x > 0 (num. of processed characters), -1 (Error)
     */
    int processTextSpan(Writer& out,
                        PptTextCFRun& cf,
                        const MSO::TextContainer* tc,
                        const QString& text,
                        const int start, int end,
                        quint16* p_fs);

    /**
     * Process all spans or parts of the run of text representing a paragraph
     * with different formatting.
     *
     * @return 0 (OK), x < 0 (Error)
     */
    int processTextSpans(Writer& out,
                         PptTextCFRun& cf,
                         const MSO::TextContainer* tc,
			 const QString& text,
                         const int start, int end,
                         quint16* p_fs);

    /**
     * Process the run of text which represents the content of a paragraph.
     *
     * @param out Writer
     * @param levels provides info about each level of indentation
     * @param cd provides access to additional text formatting in StyleTextProp9Atom
     * @param tc provides access to text formatting in MasterTextPropAtom and TextType
     * @param tr specifies tabbing, horizontal margins, and indentation for text
     * @param isPlaceHolder specifies if the shape is a placeholder
     * @param text contains the text of the slide
     * @param start specifies begging of the paragraph in text
     * @param end specifies end of the paragraph in text
     */
    void processParagraph(Writer& out,
                          ListStack& levels,
                          const MSO::OfficeArtClientData* cd,
                          const MSO::TextContainer* tc,
                          const MSO::TextRuler* tr,
                          const bool isPlaceHolder,
                          const QString& text,
                          const int start,
                          int end);

    /**
     * @brief Write declaration in the content body presentation
     * @param xmlWriter XML writer to write
     */
    void processDeclaration(KoXmlWriter* xmlWriter);

    /**
      * @brief Converts vector of quint16 to String
      *
      * Powerpoint files have text as utf16.
      * @param data Vector to convert
      * @return data as string
      */
    inline QString utf16ToString(const QVector<quint16> &data);

    /**
      * @brief Find hyperlink with specified id
      *
      * @param id Id of the hyperlink to find
      * @return QPair where first element is hyperlink's target, second is
      * user readable name. If both are empty, hyperlink is not found
      */
    QPair<QString, QString> findHyperlink(const quint32 id);

    /**
     * @brief Convert paraspacing value
     *
     * ParaSpacing is a 2-byte signed integer that specifies text paragraph
     * spacing.  It MUST be a value from the following intervals:
     *
     * x = value; x in <0, 13200>, specifies spacing as a percentage of the text
     * line height.  x < 0, the absolute value specifies spacing in master units.
     *
     * master unit: A unit of linear measurement that is equal to 1/576 inch.
     *
     * @param value to convert
     * @param size of the font
     * @param percentage is preferred in case x in <0, 13200>
     * @return processed value
     */
    QString processParaSpacing(const int value,
                               const quint16 fs,
                               const bool percentage = false) const;

    /**
    * @brief Convert TextAlignmentEnum value to a string from ODF specification
    * An enumeration that specifies paragraph alignments.
    * Name                      Value       Meaning
    * Tx_ALIGNLeft              0x0000    For horizontal text, left aligned.
    *                                     For vertical text, top aligned.
    * Tx_ALIGNCenter            0x0001    For horizontal text, centered.
    *                                     For vertical text, middle aligned.
    * Tx_ALIGNRight             0x0002    For horizontal text, right aligned.
    *                                     For vertical text, bottom aligned.
    * Tx_ALIGNJustify           0x0003    For horizontal text, flush left and
    *                                     right.
    *                                     For vertical text, flush top and bottom.
    * Tx_ALIGNDistributed       0x0004    Distribute space between characters.
    * Tx_ALIGNThaiDistributed   0x0005    Thai distribution justification.
    * Tx_ALIGNJustifyLow        0x0006    Kashida justify low.
    *
    * NOTE values Tx_ALIGNDistributed, Tx_ALIGNThaiDistributed and
    * Tx_ALIGNJustifyLow were not found in ODF specification v1.1 and are
    * ignored at the moment. Values "end" and "start" from ODF specification v1.1
    * were not found in [MS-PPT].pdf and are also ignored.
    *
    * @param value TextAlignmentEnum value to convert
    * @return value as string from ODF specification or an empty string if
    * conversion failed
    */
    QString textAlignmentToString(unsigned int value) const;

    /**
    * @brief convert Master Unit values to centimeters
    *
    * From [MS-PPT].pdf:
    * master unit: A unit of linear measurement that is equal to 1/576 inch.
    *
    * @param value master unit value to convert
    * @return value as centimeters with cm appended to the end
    */
    QString pptMasterUnitToCm(qint16 value) const;

    /**
     * @brief Converts ColorIndexStruct to QColor
     *
     * ColorIndexStruct can either contain rgb values or an index into
     * SlideSchemeColorSchemeAtom.  The main master/title master slide or notes
     * master slide will be checked to choose the correct color scheme.  The
     * presentation slide or notes slide will be checked if applicable.  This
     * method returns the rgb values the specified struct refers to.
     *
     * @param color Color to convert
     * @return QColor value, may be undefined
     */
    QColor toQColor(const MSO::ColorIndexStruct &color);

    /**
     * @brief Converts OfficeArtCOLORREF to QColor
     *
     * OfficeArtCOLORREF struct can contain either rgb values or an index into
     * SlideSchemeColorSchemeAtom.  The main master/title master slide or notes
     * master slide MUST be provided to choose the correct color scheme.  The
     * presentation slide or notes slide MUST be provided if applicable.  This
     * method returns the rgb values the specified struct refers to.
     *
     * @param color stored as OfficeArtCOLORREF to convert
     * @param pointer to a MainMasterContainer or NotesContainer
     * @param pointer to a SlideContainer or NotesContainer
     * @return QColor value, may be undefined
     */
    QColor toQColor(const MSO::OfficeArtCOLORREF& color,
                    const MSO::StreamOffset* master = nullptr,
                    const MSO::StreamOffset* common = nullptr);

    /**
    * @brief processTextAutoNumberScheme : process the Textautoscheme to display the Bullet and numbering.
    * @param TextAutomNumberSchemeEnum - enum values of textautoscheme.
    * @param numFormat - Format of the bulletand numbering scheme
    * @param numSuffix - Suffix of the numFormat
    * @param numPrefix - suffix of prefix
    * @return none
    */
    void processTextAutoNumberScheme(int val, QString& numFormat, QString& numSuffix, QString& numPrefix);

    /**
    * @brief find the text from  Declaration.
    * @return pointer of the use name.
    */
    QString findDeclaration(DeclarationType type, const QString &text) const;
    /**
    * @brief find the text from  notesDeclaration.
    * @return pointer of the use notes name.
    */
    QString findNotesDeclaration(DeclarationType type, const QString &) const;

    /**
    * @brief insert the text into  Declaration.
    * @param QString declaration use name string ex: ftr1
    * @param QString declaration text to displayed.
    */
    void insertDeclaration(DeclarationType type, const QString &name, const QString &text);
    /**
    * @brief insert the text into  notesDeclaration.
    * @param QString notes declaration use name string ex: ftr2
    * @param QString notes text to displayed.
    */
    void insertNotesDeclaration(DeclarationType type, const QString &name, const QString &text);

    /**
     * @brief retrieve the OfficeArtSpContainer of the master shape
     * @param spid identifier of the master shape
     * @return pointer to the OfficeArtSpContainer
     */
    const MSO::OfficeArtSpContainer* retrieveMasterShape(quint32 spid) const;

    /**
     * There is at most one SlideHeadersFootersContainer, but for some slides
     * it is in a strange positions. This convenience function returns a pointer
     * to the SlideHeadersFootersContainer or nullptr if there is none.
     **/
    const MSO::SlideHeadersFootersContainer* getSlideHF() const
    {
        return (p->documentContainer->slideHF)
               ? p->documentContainer->slideHF.data()
               : p->documentContainer->slideHF2.data();
    }
    const MSO::PerSlideHeadersFootersContainer*
    getPerSlideHF(const MSO::SlideContainer* slide) const
    {
        const MSO::PerSlideHeadersFootersContainer* hf = 0;
        const MSO::MasterOrSlideContainer* master = p->getMaster(slide);
        const MSO::MainMasterContainer* m1 =
                (master) ?master->anon.get<MSO::MainMasterContainer>() :0;
        const MSO::SlideContainer* m2 =
                (master) ?master->anon.get<MSO::SlideContainer>() :0;
        if (slide && slide->perSlideHFContainer) {
            hf = slide->perSlideHFContainer.data();
        } else if (m1 && m1->perSlideHeadersFootersContainer) {
            hf = m1->perSlideHeadersFootersContainer.data();
        } else if (m2 && m2->perSlideHFContainer) {
            hf = m2->perSlideHFContainer.data();
        }
        return hf;
    }

    const MSO::FontEntityAtom* getFont(quint16 fontRef)
    {
        const MSO::FontCollectionContainer* f =
            p->documentContainer->documentTextInfo.fontCollection.data();
        if (f && f->rgFontCollectionEntry.size() > fontRef) {
            return &f->rgFontCollectionEntry[fontRef].fontEntityAtom;
        }
        return 0;
    }

    /**
     * Pointer to the parser
     */
    const ParsedPresentation* p;

    /**
     * Pointer to the filter (KoFilter child)
     */
    PowerPointImport* m_filter;

    /**
     * Pointer to the progress indication f. defined in PowerPointImport.cpp
     */
    void (PowerPointImport::*m_setProgress)(const int);

    /**
     * Whether to propagate progress updates to the filter
     */
    bool m_progress_update;

    /**
     * name for to use in the style:page-layout-name attribute for master
     * slides (style:master-page)
     */
    QString slidePageLayoutName;
    /**
     * name for to use in the style:page-layout-name attribute for notes
     * and handout slides (presentation:notes and style:handout-master)
     */
    QString notesPageLayoutName;

    //Pointers to ppt specific information, try to avoid using those.
    const MSO::SlideListWithTextSubContainerOrAtom* m_currentSlideTexts;
    const MSO::MasterOrSlideContainer* m_currentMaster;
    const MSO::SlideContainer* m_currentSlide;
    bool m_processingMasters; //false - processing presentation slides

    QMap<QByteArray, QString> pictureNames;
    QMap<quint16, QString> bulletPictureNames;
    DateTimeFormat dateTime;
    QString declarationStyleName;

    QMap<const void*, QString> presentationPageLayouts;
    QMap<const void*, QString> drawingPageStyles;
    typedef QMap<const MSO::MasterOrSlideContainer*, QMap<int, QString> > MasterStyles;
    MasterStyles masterGraphicStyles;
    MasterStyles masterPresentationStyles;
    QMap<const MSO::MasterOrSlideContainer*, QString> masterNames;
    QString notesMasterName;

    quint16 m_firstChunkFontSize;
    quint16 m_firstChunkFontRef;
    bool m_firstChunkSymbolAtStart;

    bool m_isList; //true - processing a list, false - processing a paragraph
    quint16 m_previousListLevel;

    // true - continue numbered list, false - restart numbering
    QMap<quint16, bool> m_continueListNumbering;

    // Map of level keys and xml:id values of text:list elements to continue
    // automatic numbering.
    QMap<quint16, QString> m_lvlXmlIdMap;

    /**
    * @brief An usedDeclaration.
    * settings for slideNo &  usedeclaration name.
    */
    QHash<unsigned int/*slideNo*/,QString /*usedDeclarationName*/>usedFooterDeclaration;

    /**
    * @brief An usedDeclaration.
    * settings for slideNo &  usedeclaration name.
    */
    QHash<unsigned int/*slideNo*/,QString/*usedDeclarationName*/>usedHeaderDeclaration;

    /**
    * @brief An usedDeclaration.
    * settings for slideNo &  usedeclaration name.
    */
    QHash<unsigned int/*slideNo*/,QString/*usedDeclarationName*/>usedDateTimeDeclaration;

    /**
    * @brief An declaration.
    * settings for declaration text and usedeclaration name.
    */
    QHash<DeclarationType/*type*/,QPair<QString/*declarationName*/,QString/*text*/> >declaration;

    /**
    * @brief An notesDeclaration.
    * settings for notes declaration  text and usenotes declaration name.
    */
    QHash<DeclarationType/*type*/,QPair<QString /*declarationName*/,QString/*text*/> >notesDeclaration;
};

#endif // PPTTOODP_H
