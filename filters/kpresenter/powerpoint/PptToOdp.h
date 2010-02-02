/* This file is part of the KDE project
   Copyright (C) 2005 Yolla Indria <yolla.indria@gmail.com>
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>
   Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
   Contact: Amit Aggarwal <amitcs06@gmail.com>

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

#include "mso/simpleParser.h"
#include "DateTimeFormat.h"
#include "ParsedPresentation.h"

#include <KoFilter.h>
#include <KoGenStyles.h>
#include <KoStore.h>

#include <QtCore/QStack>
#include <QtCore/QPair>
#include <QtCore/QRectF>
#include <QtGui/QColor>

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
public:
    /**
     * Constructs a converter.
     */
    PptToOdp();
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
private:
    /**
     * Function that does the actual conversion.
     *
     * It is shared by the two convert() functions.
     * @param input an open OLE container that contains the ppt data.
     * @param output an open KoStore to write the odp into.
     * @return result code of the conversion.
     */
    KoFilter::ConversionStatus doConversion(POLE::Storage& input,
                                            KoStore* output);

    /**
     * Helper class that for writing xml.
     *
     * Besides containing KoXmlWriter, this class keeps track of the coordinate
     * system. It has convenience functions for writing lengths in physical
     * dimensions (currently only mm).
     */
    class Writer
    {
    private:
        qreal xOffset;
        qreal yOffset;
        qreal scaleX;
        qreal scaleY;
    public:
        /**
         * Xml writer that writes into content.xml.
         */
        KoXmlWriter& xml;

        /**
         * Construct a new Writer.
         *
         * @param xmlWriter The xml writer that writes content.xml
         */
        Writer(KoXmlWriter& xmlWriter);
        /**
         * Create a new writer with a new coordinate system.
         *
         * In different contexts in drawings in PPT files, different coordinate
         * systems are used. These are defined by specifying a rectangle in the
         * old coordinate system and the equivalent in the new coordinate
         * system.
         */
        Writer transform(const QRectF& oldCoords, const QRectF &newCoords) const;
        /**
         * Convert local length to global length string.
         *
         * A length without unit in the local coordinate system is converted
         * to a global length with a unit.
         * @param length a local length.
         * @return string of the global length with "mm" appended.
         */
        QString vLength(qreal length);
        /**
         * @see vLength
         */
        QString hLength(qreal length);
        /**
         * @see vLength
         */
        QString vOffset(qreal offset);
        /**
         * @see vLength
         */
        QString hOffset(qreal offset);
    };

    /**
      * @brief Simple pre parsed hyperlink range.
      *
      * PPT file format describes hyperlinks with 3 structures:
      * 1) Range of text the hyperlink replaces
      * 2) Hyperlink action
      * 3) Hyperlink location and name
      *
      * This struct contains structures 1 and 2 to simplify applying hyperlinks.
      */
    typedef struct {
        /**
          * @brief Start index of text that is to be replaced with hyperlink's
          * user readable name
          *
          */
        int start;

        /**
          * @brief End index of text that is to be replaced with hyperlink's
          * user readable name
          *
          */
        int end;

        /**
          * @brief ID of the hyperlink we want to this specific range
          */
        int id;
    }HyperlinkRange;

    void createMainStyles(KoGenStyles& styles);

    template <typename T>
    void processGraphicStyle(KoGenStyle& style, T& o);
    void processSlideForStyle(int slideNo, KoGenStyles &styles);
    void processObjectForStyle(const PPT::OfficeArtSpgrContainerFileBlock& of, KoGenStyles &styles);
    void processObjectForStyle(const PPT::OfficeArtSpgrContainer& o, KoGenStyles &styles);
    void processObjectForStyle(const PPT::OfficeArtSpContainer& o, KoGenStyles &styles);
    void processDrawingObjectForStyle(const PPT::OfficeArtSpContainer& o, KoGenStyles &styles);
    void processTextObjectForStyle(const PPT::OfficeArtSpContainer& o, const PPT::TextContainer& tc, KoGenStyles &styles);


    QByteArray createContent(KoGenStyles& styles);
    void processSlideForBody(unsigned slideNo, KoXmlWriter& xmlWriter);
    void processObjectForBody(const PPT::OfficeArtSpgrContainerFileBlock& o, Writer& out);
    void processObjectForBody(const PPT::OfficeArtSpgrContainer& o, Writer& out);
    void processObjectForBody(const PPT::OfficeArtSpContainer& o, Writer& out);
    void processDrawingObjectForBody(const PPT::OfficeArtSpContainer& o, Writer& out);
    void processTextObjectForBody(const PPT::OfficeArtSpContainer& o, const PPT::TextContainer& tc, Writer& out);

    /**
    * @brief Write styles (KoGenStyle& style) meant for the whole presentation
    * @param KoGenStyle& style To represent the style
    * @param Slide master - master slide
    */
    void processContentStyles(KoGenStyles &styles);

     /**
     * @brief Write declaration in the content body presentation
     * @param xmlWriter XML writer to write
     */
     void processDeclaration(KoXmlWriter* xmlWriter);

     /**
     * @brief Write Frame element.
     * @param KoGenStyle& style To represent the style
     * @param Writer& out writer xml.
     * @param presentatonName represent the class for ex: page-number etc
     * @param QRect -  co-ordinates of the frame
     * @param mStyle - presentation style
     * @param pStyle - paragraph style
     * @param tStyle - text style
     */
     void addFrame(KoGenStyle& style, Writer& out, const char* presentationName,
            const QRect& rect, QString mStyle, QString pStyle, QString tStyle);

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
    QPair<QString, QString> findHyperlink(const unsigned int id);

     /**
      * @brief Find the next hyperlink range that starts after current position
      * @param text TextContainer to search hyperlinks from
      * @param currentPos current position on the text
      * @return Range for next hyperlink, or a invalid range (where values are
      * negative) if none found.
      */
    HyperlinkRange findNextHyperlinkStart(const PPT::TextContainer& text,
                                          const int currentPos);

    /**
      * @brief Write a text span and replace vertical tabs and carriage returns.
      * @param xmlWriter writer to write with
      * @param text text to write
      */
    void writeTextSpan(KoXmlWriter& xmlWriter, const QString &text);

    /**
      * @brief Write a text span while replacing text with possible hyperlinks
      * @param text TextContainer to write
      * @param xmlWriter XML writer to write with
      * @param start Starting index of the textObject's text that is written
      * @param end Ending index of the textObject's text that is written
      */
    void writeTextSpanWithHyperlinks(KoXmlWriter& xmlWriter,
                                     const PPT::TextContainer& text,
                                     const int start,
                                     const int end);

    /**
    * @brief Write text deindentations the specified amount. Actually it just
    * closes elements.
    *
    * Doesn't close the last text:list-item though.
    * @param xmlWriter XML writer to write closing tags
    * @param count how many lists and list items to leave open
    * @param levels the list of levels to remove from
    */
    void writeTextObjectDeIndent(KoXmlWriter& xmlWriter,
                                 const unsigned int count,
                                 QStack<QString>& levels);

    /**
    * @brief Write text all text within specified paragraph
    *
    * @param xmlWriter XML writer to write with
    * @param pf Text paragraph exception to define style for the text paragraph
    * @param textObject text object whose contents to write
    * @param textPos position of current paragraph within all the text in
    * textObject
    * @param levels   stack of current list levels by name
    */
    void writeTextPFException(KoXmlWriter& xmlWriter,
                              const PPT::TextPFRun *pf,
                              const PPT::TextContainer& text,
                              const QString& intext,
                              const unsigned int textPos,
                              QStack<QString>& levels);

    /**
    * @brief Write specified line within specified paragraph with xmlWriter
    * @param xmlWriter XML writer to write with
    * @param pf Text paragraph exception to define paragraph style for the line
    * @param textObject text object whose contents to write
    * @param text Line to write
    * @param linePosition what is the index of the whole text (within textObject)
    * that this line starts from
    */
    void writeTextLine(KoXmlWriter& xmlWriter,
                       const PPT::StyleTextPropAtom& style,
                       const PPT::TextPFException* pf,
                       const PPT::TextContainer& tc,
                       const QString& text,
                       const unsigned int linePosition);

    /**
    * @brief write part of a line (bound by the same text character exception)
    * @param xmlWriter XML writer to write with
    * @param cf character exception that applies to text we are about to write
    * @param pf paragraph exception that applies to text we are about to write
    * @param textPosition starting index of the text line within all text
    * in the same text bytes/chars struct
    * @param textLength length of text in characters
    */
    void writeTextCFException(KoXmlWriter& xmlWriter,
                              const PPT::TextCFException *cf,
                              const PPT::TextPFException *pf,
                              const PPT::TextContainer& tc,
                              const unsigned int textPosition,
                              const unsigned int textLength);

    /**
    * @brief Parse all styles from given PPT::TextCFRun and PPT::TextPFRun pair
    * @param cf PPT::TextCFRun that provides character style
    * @param pf PPT::TextPFRun that provides paragraph style
    * @param styles KoGenStyles to store styles to
    * @param textObject TextObject to cache style names to
    */
    void processTextExceptionsForStyle(const PPT::TextCFRun *cf,
                                       const PPT::TextPFRun *pf,
                                       KoGenStyles &styles,
                                       const PPT::TextContainer& tc);

    /**
    * @brief Helper method to find specified TextParagraphException from
    * MainMasterContainer
    * @param text type of the text whose style to get. See TextTypeEnum in
    * [MS-PPT].pdf
    * @param level TextMasterStyleLevel index (indentation level)
    */
    const PPT::TextPFException *masterTextPFException(quint16 type, quint16 level);

    /**
    * @brief Helper method to find specified TextCharacterException from
    * MainMasterContainer
    * @param text type of the text whose style to get. See TextTypeEnum in
    * [MS-PPT].pdf
    * @param level TextMasterStyleLevel index (indentation level)
    */
    const PPT::TextCFException *masterTextCFException(int type, unsigned int level);

    /**
    * @brief Convert paraspacing value to centimeters
    *
    * ParaSpacing is a 2-byte signed integer that specifies text paragraph
    * spacing. It MUST be a value from the following table:
    * Range                     Meaning
    * 0 to 13200, inclusive.    The value specifies spacing as a percentage of the
    *                           text line height.
    * Less than 0.              The absolute value specifies spacing in master
    *                           units.
    *
    * master unit: A unit of linear measurement that is equal to 1/576 inch.
    *
    * @brief value Value to convert
    * @return value converted to centimeters
    */
    QString paraSpacingToCm(int value) const;

    /**
    * @brief Convert TextAlignmentEnum value to a string from ODF specification
    * An enumeration that specifies paragraph alignments.
    * Name                              Value       Meaning
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
    QString pptMasterUnitToCm(unsigned int value) const;

    /**
    * @brief Converts ColorIndexStruct to QColor
    *
    * Color index struct can either contain rgb values or a index to color
    * in MainMasterContainer's SlideSchemeColorSchemeAtom. This method
    * returns the rgb values the specified struct refers to.
    * @param color Color to convert
    * @return QColor value, may be undefined
    */
    QColor toQColor(const PPT::ColorIndexStruct &color);
    QColor toQColor(const PPT::OfficeArtCOLORREF& c);

    /**
    * TextAutoNumberSchemeEnum
    * Referenced by: TextAutoNumberScheme
    * An enumeration that specifies the character sequence and delimiters to use for automatic
    * numbering.
    */
    enum {
        ANM_AlphaLcPeriod,                  //0x0000     Example: a., b., c., ...Lowercase Latin character followed by a period.
        ANM_AlphaUcPeriod ,                 //0x0001     Example: A., B., C., ...Uppercase Latin character followed by a period.
        ANM_ArabicParenRight,               //0x0002     Example: 1), 2), 3), ...Arabic numeral followed by a closing parenthesis.
        ANM_ArabicPeriod,                   //0x0003     Example: 1., 2., 3., ...Arabic numeral followed by a period.
        ANM_RomanLcParenBoth,               //0x0004     Example: (i), (ii), (iii), ...Lowercase Roman numeral enclosed in parentheses.
        ANM_RomanLcParenRight,              //0x0005     Example: i), ii), iii), ... Lowercase Roman numeral followed by a closing parenthesis.
        ANM_RomanLcPeriod,                  //0x0006     Example: i., ii., iii., ...Lowercase Roman numeral followed by a period.
        ANM_RomanUcPeriod ,                 //0x0007     Example: I., II., III., ...Uppercase Roman numeral followed by a period.
        ANM_AlphaLcParenBoth,               //0x0008     Example: (a), (b), (c), ...Lowercase alphabetic character enclosed in parentheses.
        ANM_AlphaLcParenRight,              //0x0009     Example: a), b), c), ...Lowercase alphabetic character followed by a closing
        ANM_AlphaUcParenBoth,               //0x000A     Example: (A), (B), (C), ...Uppercase alphabetic character enclosed in parentheses.
        ANM_AlphaUcParenRight,              //0x000B     Example: A), B), C), ...Uppercase alphabetic character followed by a closing
        ANM_ArabicParenBoth,                //0x000C     Example: (1), (2), (3), ...Arabic numeral enclosed in parentheses.
        ANM_ArabicPlain,                    //0x000D     Example: 1, 2, 3, ...Arabic numeral.
        ANM_RomanUcParenBoth,               //0x000E     Example: (I), (II), (III), ...Uppercase Roman numeral enclosed in parentheses.
        ANM_RomanUcParenRight,              //0x000F     Example: I), II), III), ...Uppercase Roman numeral followed by a closing parenthesis.
        //Future
    }TextAutoNumberSchemeEnum;

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
    * ClientPlacementId
    * Referenced by: ClientTextData
    * Used in master,notes placement id to define the default location
    */
    enum ClientPlacementId {
        None = 0,
        MasterTitle = 1,
        MasterBody = 2,
        MasterCenteredTitle = 3,
        MasterNotesSlideImage,
        MasterNotesBodyImage,
        //Missing
        MasterDate = 7 ,
        MasterSlideNumber = 8,
        MasterFooter = 9,
        MasterHeader = 10,
        MasterSubtitle,
        GenericTextObject,
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

    /**
    * QHash ClientPlacementId, QRect
    * Referenced by: masterObects
    * Store the masterObjects placement co-ordinates with placementId.
    */
    QHash<ClientPlacementId/*placementId*/,QRect>masterObjects;

    /**
    * @brief processTextAutoNumberScheme : process the Textautoscheme to display the Bullet and numbering.
    * @param TextAutomNumberSchemeEnum - enum values of textautoscheme.
    * @param numFormat - Format of the bulletand numbering scheme
    * @param numSuffix - Suffix of the numFormat
    * @param numPrefix - suffix of prefix
    * @return none
    */
    void processTextAutoNumberScheme(int val, QString& numFormat, QString& numSuffix, QString& numPrefix);


    void processEllipse(const PPT::OfficeArtSpContainer& fsp, Writer& out);
    void processRectangle(const PPT::OfficeArtSpContainer& o, Writer& out);
    void processRoundRectangle(const PPT::OfficeArtSpContainer& o, Writer& out);
    void processDiamond(const PPT::OfficeArtSpContainer& o, Writer& out);
    void processTriangle(const PPT::OfficeArtSpContainer& o, Writer& out);
    void processTrapezoid(const PPT::OfficeArtSpContainer& o, Writer& out);
    void processParallelogram(const PPT::OfficeArtSpContainer& o, Writer& out);
    void processHexagon(const PPT::OfficeArtSpContainer& o, Writer& out);
    void processOctagon(const PPT::OfficeArtSpContainer& o, Writer& out);
    void processArrow(const PPT::OfficeArtSpContainer& o, Writer& out);
    void processLine(const PPT::OfficeArtSpContainer& o, Writer& out);
    void processSmiley(const PPT::OfficeArtSpContainer& o, Writer& out);
    void processHeart(const PPT::OfficeArtSpContainer& o, Writer& out);
    void processFreeLine(const PPT::OfficeArtSpContainer& o, Writer& out);
    void processPictureFrame(const PPT::OfficeArtSpContainer& o, Writer& out);

    /**
    * @brief process the mainMasterSlideContainer
    *
    * From [MS-PPT].pdf
    * @param -  KoGenStyles &styles
    * @param -  KoXmlWriter* xmlWriter
    * @return - None
    */
    void processMainMasterSlide(KoGenStyles &styles, Writer& out);

    /**
    * @brief process the main Master DrawingObject.
    * ShapeGroupContainer as per the MS-PPT spec
    *
    * From [MS-PPT].pdf
    * @param -  OfficeArtSpgrContainer
    * @param -   Writer& out
    * @return - None
    */
    void processMainMasterDrawingObject(const PPT::OfficeArtSpgrContainer& o, Writer& out);

    /**
    * @brief process the main Master DrawingObject FileBlock Container.
    * SpgrContainerFileBlock as per the MS-PPT spec
    *
    * From [MS-PPT].pdf
    * @param -  OfficeArtSpgrContainerFileBlock
    * @param -  Writer& out
    * @return - None
    */
    void processMainMasterDrawingObject(const PPT::OfficeArtSpgrContainerFileBlock& of, Writer& out);

    /**
    * @brief process the main Master DrawingObject.
    * ShapeContainer as per the MS-PPT spec
    *
    * From [MS-PPT].pdf
    * @param -  OfficeArtSpContainer
    * @param -  Writer& out
    * @return - None
    */
    void processMainMasterDrawingObject(const PPT::OfficeArtSpContainer& o, Writer& out);

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
    *
    *
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

    QMap<QByteArray, QString> pictureNames;
    DateTimeFormat dateTime;
    QString masterStyleName;
    typedef QPair<const PPT::TextCFException*, const PPT::TextPFException*> StyleKey;
    QMap<StyleKey, StyleName> textStyles;
    QMap<const PPT::OfficeArtSpContainer*, QString> graphicStyles;

    const ParsedPresentation* p;

    const PPT::SlideContainer* currentSlide;
    const PPT::SlideListWithTextSubContainerOrAtom* currentSlideTexts;

    bool parse(POLE::Storage& storage);

    /**
     * There is at most one SlideHeadersFootersContainer, but for some slides
     * it is in a strange positions. This convenience function returns a pointer
     * to the SlideHeadersFootersContainer or NULL if there is none.
     **/
    const PPT::SlideHeadersFootersContainer* getSlideHF() const {
        return (p->documentContainer->slideHF)
               ? p->documentContainer->slideHF.data()
               : p->documentContainer->slideHF2.data();
    }
    void setGraphicStyleName(const PPT::OfficeArtSpContainer& o, const QString& name) {
        graphicStyles[&o] = name;
    }

    /**
      *Return the name of the style associated with this object.
      * If no style is present, create one.
      **/
    QString getGraphicStyleName(const PPT::OfficeArtSpContainer& o) {
        return graphicStyles.value(&o);
    }
    void addStyleNames(const PPT::TextCFException *cf, const PPT::TextPFException *pf,
                       const QString& text, const QString& paragraph, const QString& list) {
        textStyles[StyleKey(cf, pf)] = StyleName(text, paragraph, list);
    }

    /**
      *Return the name of the style associated with these objects.
      * If no style is present, create one.
      **/
    QString getTextStyleName(const PPT::TextCFException *cf, const PPT::TextPFException *pf) {
        return textStyles.value(StyleKey(cf, pf)).text;
    }
    /**
      *Return the name of the style associated with these objects.
      * If no style is present, create one.
      **/
    QString getListStyleName(const PPT::TextCFException *cf, const PPT::TextPFException *pf) {
        return textStyles.value(StyleKey(cf, pf)).list;
    }
    /**
      *Return the name of the style associated with these objects.
      * If no style is present, create one.
      **/
    QString getParagraphStyleName(const PPT::TextCFException *cf, const PPT::TextPFException *pf) {
        return textStyles.value(StyleKey(cf, pf)).paragraph;
    }

    /**
      * Look in blipStore for the id mapping to this object
      **/
    QByteArray getRgbUid(quint16 pib) const {
        // return 16 byte rgbuid for this given blip id
        if (p->documentContainer->drawingGroup.OfficeArtDgg.blipStore) {
            const PPT::OfficeArtBStoreContainer* b
            = p->documentContainer->drawingGroup.OfficeArtDgg.blipStore.data();
            if (pib < b->rgfb.size()
                    && b->rgfb[pib].anon.is<PPT::OfficeArtFBSE>()) {
                return b->rgfb[pib].anon.get<PPT::OfficeArtFBSE>()->rgbUid;
            }
        }
        return QByteArray();
    }
    const PPT::FontEntityAtom*
    getFont(quint16 fontRef) {
        const PPT::FontCollectionContainer* f =
            p->documentContainer->documentTextInfo.fontCollection.data();
        if (f && f->rgFontCollectionEntry.size() > fontRef) {
            return &f->rgFontCollectionEntry[fontRef].fontEntityAtom;
        }
        return 0;
    }

    void createFillImages(KoGenStyles& styles);
    QString getPicturePath(int pib) const;

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
};

#endif // POWERPOINTIMPORT_H
