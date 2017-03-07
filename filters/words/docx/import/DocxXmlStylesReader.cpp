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

#include "DocxXmlStylesReader.h"

#include "DocxImport.h"
#include "DocxDebug.h"

#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <MsooXmlUnits.h>

#define MSOOXML_CURRENT_NS "w"
#define BIND_READ_CLASS MSOOXML::MsooXmlStylesReader
#define MSOOXML_CURRENT_CLASS DocxXmlStylesReader

#include <MsooXmlReader_p.h>
#include <MsooXmlDrawingTableStyle.h>


//#include <MsooXmlCommonReaderImpl.h> // this adds w:pPr, etc.

DocxXmlStylesReader::DocxXmlStylesReader(KoOdfWriters *writers)
        : DocxXmlDocumentReader(writers)
{
}

DocxXmlStylesReader::~DocxXmlStylesReader()
{
    qDeleteAll(m_defaultStyles);
//    delete m_characterStyle;
}

void DocxXmlStylesReader::init()
{
//already done in DocxXmlDocumentReader:    initInternal(); // MsooXmlCommonReaderImpl.h
    m_defaultNamespace = QLatin1String(MSOOXML_CURRENT_NS ":");
}

void DocxXmlStylesReader::createDefaultStyle(KoGenStyle::Type type, const char* family)
{
    KoGenStyle *style = new KoGenStyle(type, family);
    style->setDefaultStyle(true);
    m_defaultStyles.insert(family, style);
}

//! @todo support latentStyles child (Latent Style Information) §17.7.4.5
KoFilter::ConversionStatus DocxXmlStylesReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = static_cast<DocxXmlDocumentReaderContext*>(context);
    debugDocx << "=============================";
    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    //w:document
    readNext();
    debugDocx << *this << namespaceUri();

    if (!expectEl("w:styles")) {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::wordprocessingml)) {
        return KoFilter::WrongFormat;
    }
    /*
        const QXmlStreamAttributes attrs( attributes() );
        for (int i=0; i<attrs.count(); i++) {
            debugDocx << "1 NS prefix:" << attrs[i].name() << "uri:" << attrs[i].namespaceUri();
        }*/

    QXmlStreamNamespaceDeclarations namespaces(namespaceDeclarations());
    for (int i = 0; i < namespaces.count(); i++) {
        debugDocx << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }
//! @todo find out whether the namespace returned by namespaceUri()
//!       is exactly the same ref as the element of namespaceDeclarations()
    if (!namespaces.contains(QXmlStreamNamespaceDeclaration("w", MSOOXML::Schemas::wordprocessingml))) {
        raiseNSNotFoundError(MSOOXML::Schemas::wordprocessingml);
        return KoFilter::WrongFormat;
    }
//! @todo expect other namespaces too...

//! @todo use KoStyleManager::saveOdfDefaultStyles()
    qDeleteAll(m_defaultStyles);
    m_defaultStyles.clear();

    createDefaultStyle(KoGenStyle::TableStyle, "table");
//     createDefaultStyle(KoGenStyle::ParagraphStyle, "paragraph");
//     createDefaultStyle(KoGenStyle::TextStyle, "text");
    //createDefaultStyle(KoGenStyle::GraphicStyle, "graphic");
    //createDefaultStyle(KoGenStyle::TableRowStyle, "table-row");
    //createDefaultStyle("numbering");

    while (!atEnd()) {
        readNext();
        debugDocx << *this;
        BREAK_IF_END_OF(styles)
        if (isStartElement()) {
            TRY_READ_IF(docDefaults)
            ELSE_TRY_READ_IF(style)
            SKIP_UNKNOWN
            //! @todo add ELSE_WRONG_FORMAT
        }
    }

    if (!expectElEnd("w:styles")) {
        return KoFilter::WrongFormat;
    }

    // add default styles:
    for (QMap<QByteArray, KoGenStyle*>::ConstIterator it(m_defaultStyles.constBegin());
         it != m_defaultStyles.constEnd(); ++it)
    {
        debugDocx << it.key();
        mainStyles->insert(*it.value());
    }
    qDeleteAll(m_defaultStyles);
    m_defaultStyles.clear();

    debugDocx << "===========finished============";
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL docDefaults
//! w:docDefaults handler (Document Default Paragraph and Run Properties)
/*! ECMA-376, 17.5.5.1, p.723.

 Document Defaults

 The first formatting information which is applied to all regions of text in a
 WordprocessingML document when that document is displayed is the document
 defaults.  The document defaults specify the default set of properties which
 shall be inherited by every paragraph and run of text within all stories of
 the current WordprocessingML document.  If no other formatting information was
 referenced by that text, these properties would solely define the formatting
 of the resulting text.

 Parent elements:
 - [done] styles (§17.7.4.18)

 Child elements:
 - [done] pPrDefault
 - [done] rPrDefault

 CASE #850
*/
KoFilter::ConversionStatus DocxXmlStylesReader::read_docDefaults()
{
    READ_PROLOGUE

    m_currentParagraphStyle = KoGenStyle(KoGenStyle::ParagraphStyle, "paragraph");
    m_currentTextStyle = KoGenStyle(KoGenStyle::TextStyle, "text");

    while (!atEnd()) {
        readNext();
        debugDocx << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(pPrDefault)
            ELSE_TRY_READ_IF(rPrDefault)
            ELSE_WRONG_FORMAT
        }
    }

    // color (Run Content Color), ECMA-376, 17.3.2.6, p.284.
    // If this element is not present, the default value is to leave the
    // formatting applied at previous level in the style hierarchy.  If this
    // element is never applied in the style hierarchy, then the characters are
    // set to allow the consumer to automatically choose an appropriate color
    // based on the background color behind the run's content.
    //
    if (m_currentTextStyle.property("fo:color").isEmpty()) {
        m_currentTextStyle.addProperty("style:use-window-font-color", "true");
    }

    if (!m_currentTextStyle.property("fo:font-size").isEmpty()) {
        m_context->m_defaultFontSizePt = m_currentTextStyle.property("fo:font-size");
    }

    KoGenStyle::copyPropertiesFromStyle(m_currentTextStyle, m_currentParagraphStyle, KoGenStyle::TextType);

    if (m_currentParagraphStyle.property("fo:line-height").isEmpty() &&
        m_currentParagraphStyle.property("style:line-height-at-least").isEmpty()) {
        m_currentParagraphStyle.addProperty("fo:line-height", "100%");
    }

    if (m_context->import->documentSettings().contains("defaultTabStop")) {
        QString val = m_context->import->documentSetting("defaultTabStop").toString();
        m_currentParagraphStyle.addPropertyPt("style:tab-stop-distance", TWIP_TO_POINT(val.toDouble()));
    }

    m_currentParagraphStyle.setDefaultStyle(true);
    mainStyles->insert(m_currentParagraphStyle);

    m_currentTextStyle.setDefaultStyle(true);
    mainStyles->insert(m_currentTextStyle);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL pPrDefault
//! w:pPrDefault handler (Default Paragraph Properties))
/*! ECMA-376, 17.7.5.3, p.726.
 Parent elements:
 - [done] docDefault
 Child elements:
 - [done] pPr

 CASE #850
*/
KoFilter::ConversionStatus DocxXmlStylesReader::read_pPrDefault()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        debugDocx << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(pPr)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL rPrDefault
//! w:rPrDefault handler (Default Run Properties)
/*! ECMA-376, 17.7.5.5, p.729.
 Parent elements:
 - [done] docDefault
 Child elements:
 - [done] rPr

 CASE #850
*/
KoFilter::ConversionStatus DocxXmlStylesReader::read_rPrDefault()
{
    READ_PROLOGUE
    while (!atEnd()) {
        readNext();
        debugDocx << *this;
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(rPr)
            ELSE_WRONG_FORMAT
        }
    }
    READ_EPILOGUE
}

//! @todo use  themeFontName:
#if 0
// CASE #1200
static QString themeFontName(const QString& asciiTheme)
{
    if (asciiTheme.contains(QLatin1String("minor"))) {
    } else if (asciiTheme.contains(QLatin1String("major"))) {
    }
    return QString();
}
#endif

//! Converts ST_StyleType to ODF's style family
/*! Style family can be paragraph, text, section, table, table-column, table-row, table-cell,
    table-page, chart, default, drawing-page, graphic, presentation, control and ruby
    but not all of these are used after converting ST_StyleType. */
static QString ST_StyleType_to_ODF(const QString& type)
{
    if (type == QLatin1String("paragraph"))
        return type;
    if (type == QLatin1String("character"))
        return QLatin1String("text");
    if (type == QLatin1String("table"))
        return type;
    //! @todo ?
//    if (type == QLatin1String("numbering"))
//        return QLatin1String("paragraph");
    return QString();
}

#undef CURRENT_EL
#define CURRENT_EL style
//! style handler (Style Definition)
/*! ECMA-376, 17.7.4.17, p.714.

 This element specifies the definition of a single style within a WordprocessingML document.
 A style is a predefined set of table, numbering, paragraph, and/or character properties which
 can be applied to regions within a document.

 The style definition for any style definition can be divided into three segments:
 - General style properties
 - Style type
 - Style type-specific properties

 Parent elements:
 - [done] styles (§17.7.4.18)

 Child elements:
 - aliases (Alternate Style Names) §17.7.4.1
 - autoRedefine (Automatically Merge User Formatting Into Style Definition) §17.7.4.2
 - [done] basedOn (Parent Style ID) §17.7.4.3
 - hidden (Hide Style From User Interface) §17.7.4.4
 - link (Linked Style Reference) §17.7.4.6
 - locked (Style Cannot Be Applied) §17.7.4.7
 - [done] name (Primary Style Name) §17.7.4.9
 - [done] next (Style For Next Paragraph) §17.7.4.10
 - personal (E-Mail Message Text Style) §17.7.4.11
 - personalCompose (E-Mail Message Composition Style) §17.7.4.12
 - personalReply (E-Mail Message Reply Style) §17.7.4.13
 - [done] pPr (Style Paragraph Properties) §17.7.8.2
 - qFormat (Primary Style) §17.7.4.14
 - [done] rPr (Run Properties) §17.7.9.1
 - rsid (Revision Identifier for Style Definition) §17.7.4.15
 - semiHidden (Hide Style From Main User Interface) §17.7.4.16
 - [done] tblPr (Style Table Properties) §17.7.6.4
 - [done] tblStylePr (Style Conditional Table Formatting Properties) §17.7.6.6
 - [done] tcPr (Style Table Cell Properties) §17.7.6.9
 - trPr (Style Table Row Properties) §17.7.6.11
 - uiPriority (Optional User Interface Sorting Order) §17.7.4.19
 - unhideWhenUsed (Remove Semi-Hidden Property When Style Is Used) §17.7.4.20

 @todo support all elements
*/
KoFilter::ConversionStatus DocxXmlStylesReader::read_style()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    m_name.clear();

    //! 17.18.83 ST_StyleType (Style Types)
    READ_ATTR(type)
    //! @todo numbering style
    if (type == QLatin1String("numbering")) {
        return KoFilter::OK; // give up
    }
    const QString odfType(ST_StyleType_to_ODF(type));
    if (odfType.isEmpty()) {
        return KoFilter::WrongFormat;
    }

    QString styleName;
    READ_ATTR_INTO(styleId, styleName)

    debugDocx << "Reading style of family:" << odfType << "[" << styleName << "]";

    // Specifies that this style is the default for this style type.  This
    // property is used in conjunction with the type attribute to determine the
    // style which is applied to objects that do not explicitly declare a
    // style.  If this attribute is not specified for any style, then no
    // properties shall be applied to objects of the specified style type.  If
    // this attribute is specified by multiple styles, then the last instance
    // of a style with this property shall be used.  ECMA-376, 17.7.4.17, p.707
    const bool isDefault = readBooleanAttr("w:default");

    if (type == "table") {
        m_currentStyle = new MSOOXML::DrawingTableStyle;
    } else {
        m_currentTextStyle = KoGenStyle(KoGenStyle::TextStyle, "text");
        m_currentParagraphStyle = KoGenStyle(KoGenStyle::ParagraphStyle, "paragraph");
    }

    QString nextStyleName;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            const QXmlStreamAttributes attrs(attributes());
            TRY_READ_IF(name)
            else if (name() == "rPr") {
                if (type == "table") {
                    m_currentTextStyle = KoGenStyle(KoGenStyle::TextStyle, "text");
                    m_currentTableStyleProperties = m_currentStyle->properties(MSOOXML::DrawingTableStyle::WholeTbl);
                    if (m_currentTableStyleProperties == 0) {
                        m_currentTableStyleProperties = new MSOOXML::TableStyleProperties;
                    }
                }
                TRY_READ(rPr)
                if (type == "table") {
                    m_currentTableStyleProperties->textStyle = m_currentTextStyle;
                    m_currentStyle->addProperties(MSOOXML::DrawingTableStyle::WholeTbl, m_currentTableStyleProperties);
                    m_currentTableStyleProperties = 0;
                }
            }
            else if (name() == "pPr") {
                if (type == "table") {
                    m_currentParagraphStyle = KoGenStyle(KoGenStyle::ParagraphStyle, "paragraph");
                    m_currentTableStyleProperties = m_currentStyle->properties(MSOOXML::DrawingTableStyle::WholeTbl);
                    if (m_currentTableStyleProperties == 0) {
                        m_currentTableStyleProperties = new MSOOXML::TableStyleProperties;
                    }
                }
                TRY_READ(pPr)
                if (type == "table") {
                    m_currentTableStyleProperties->paragraphStyle = m_currentParagraphStyle;
                    m_currentStyle->addProperties(MSOOXML::DrawingTableStyle::WholeTbl, m_currentTableStyleProperties);
                    m_currentTableStyleProperties = 0;
                }
            }
            else if (name() == "tblPr") {
                m_currentTableStyleProperties = m_currentStyle->properties(MSOOXML::DrawingTableStyle::WholeTbl);
                if (m_currentTableStyleProperties == 0) {
                    m_currentTableStyleProperties = new MSOOXML::TableStyleProperties;
                }
                m_tableMainStyle = KoTblStyle::create();
                TRY_READ(tblPr)
                m_currentStyle->mainStyle = m_tableMainStyle;
                m_currentStyle->addProperties(MSOOXML::DrawingTableStyle::WholeTbl, m_currentTableStyleProperties);
                m_currentTableStyleProperties = 0;
            }
            else if (name() == "tcPr") {
                m_currentTableStyleProperties = m_currentStyle->properties(MSOOXML::DrawingTableStyle::WholeTbl);
                if (m_currentTableStyleProperties == 0) {
                    m_currentTableStyleProperties = new MSOOXML::TableStyleProperties;
                }
                TRY_READ(tcPr)
                m_currentStyle->addProperties(MSOOXML::DrawingTableStyle::WholeTbl, m_currentTableStyleProperties);
                m_currentTableStyleProperties = 0;
            }
            ELSE_TRY_READ_IF(tblStylePr)
            else if (QUALIFIED_NAME_IS(basedOn)) {
                READ_ATTR(val)
                if (type == "character") {
                    m_currentTextStyle.setParentName(val);
                }
                else if (type == "paragraph") {
                    m_currentParagraphStyle.setParentName(val);
                }
            }
            else if (QUALIFIED_NAME_IS(next)) {
                READ_ATTR_INTO(val, nextStyleName)
            }
            SKIP_UNKNOWN
            //! @todo add ELSE_WRONG_FORMAT
        }
    }
    KoGenStyles::InsertionFlags insertionFlags = KoGenStyles::DontAddNumberToName;
    if (styleName.isEmpty()) {
        styleName = m_name;
        if (styleName.isEmpty()) {
            // allow for numbering for generated style names
            styleName = odfType;
            insertionFlags = KoGenStyles::NoFlag;
        }
    }

    // When reading from styles, we allow duplicates
    insertionFlags = insertionFlags | KoGenStyles::AllowDuplicates;

    // style:class - A style may belong to an arbitrary class of styles.  The
    // class is an arbitrary string.  The class has no meaning within the file
    // format itself, but it can for instance be evaluated by user interfaces
    // to show a list of styles where the styles are grouped by its name.
    //
    // ! @todo oo.o converter defines these classes: list, extra, index,
    // ! chapter
    if (type == "character") {
        m_currentTextStyle.addAttribute("style:class", "text");
        styleName = mainStyles->insert(m_currentTextStyle, styleName, insertionFlags);
    }
    else if (type == "paragraph") {
        m_currentParagraphStyle.addAttribute("style:class", "text");
        KoGenStyle::copyPropertiesFromStyle(m_currentTextStyle, m_currentParagraphStyle, KoGenStyle::TextType);
        styleName = mainStyles->insert(m_currentParagraphStyle, styleName, insertionFlags);
    }
    else if (type == "table") {
        m_context->m_tableStyles.insert(styleName, m_currentStyle);
    }

    if (!nextStyleName.isEmpty()) {
        mainStyles->insertStyleRelation(styleName, nextStyleName, "style:next-style-name");
    }

    if (isDefault) {
        m_context->m_namedDefaultStyles.insertMulti(odfType, styleName);
    }

    m_currentParagraphStyle = KoGenStyle();
    m_currentTextStyle = KoGenStyle();

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tblStylePr
//! tblStylePr  (Style Conditional Table Formatting Properties)
/*! ECMA-376, 17.7.6.6, p.731

  This element specifies a set of formatting properties which shall be
  conditionally applied to the parts of a table which match the requirement
  specified on the type attribute.

  Parent elements:
  - [done] style (§17.7.4.17)

  Child elements:
  - [done] pPr (Table Style Conditional Formatting Paragraph Properties) §17.7.6.1
  - [done] rPr (Table Style Conditional Formatting Run Properties) §17.7.6.2
  - [done] tblPr (Table Style Conditional Formatting Table Properties) §17.7.6.3
  - [done] tcPr (Table Style Conditional Formatting Table Cell Properties) §17.7.6.8
  - trPr (Table Style Conditional Formatting Table Row Properties) §17.7.6.10
 */
KoFilter::ConversionStatus DocxXmlStylesReader::read_tblStylePr()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    READ_ATTR(type)

    m_currentTableStyleProperties = new MSOOXML::TableStyleProperties;
    m_currentParagraphStyle = KoGenStyle(KoGenStyle::ParagraphAutoStyle, "paragraph");
    m_currentTextStyle = KoGenStyle(KoGenStyle::TextAutoStyle, "text");

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(tcPr)
            ELSE_TRY_READ_IF(rPr)
            ELSE_TRY_READ_IF(pPr)
            ELSE_TRY_READ_IF(tblPr)
            //TODO: Add trPr and test for regressions!
            SKIP_UNKNOWN
            //! @todo add ELSE_WRONG_FORMAT
        }
    }

    m_currentTableStyleProperties->textStyle = m_currentTextStyle;
    m_currentTableStyleProperties->paragraphStyle = m_currentParagraphStyle;

    if (type == "firstRow") {
        // In docx predefined styles for first row, even though it may define
        // insideV to be 0 and bottom border to have something, it in reality
        // wishes insideV to also contain the bottom data
//         if (m_currentTableStyleProperties->insideH.innerPen.widthF() == 0) {
//             m_currentTableStyleProperties->insideH = m_currentTableStyleProperties->bottom;
//         }
        if (m_currentTableStyleProperties->target == MSOOXML::TableStyleProperties::Table) {
            m_currentTableStyleProperties->target = MSOOXML::TableStyleProperties::TableRow;
        }
        m_currentStyle->addProperties(MSOOXML::DrawingTableStyle::FirstRow, m_currentTableStyleProperties);
    }
    else if (type == "lastRow") {
        if (m_currentTableStyleProperties->target == MSOOXML::TableStyleProperties::Table) {
            m_currentTableStyleProperties->target = MSOOXML::TableStyleProperties::TableRow;
        }
        m_currentStyle->addProperties(MSOOXML::DrawingTableStyle::LastRow, m_currentTableStyleProperties);
    }
    else if (type == "band1Horz") {
        m_currentStyle->addProperties(MSOOXML::DrawingTableStyle::Band1Horizontal, m_currentTableStyleProperties);
    }
    else if (type == "band2Horz") {
        m_currentStyle->addProperties(MSOOXML::DrawingTableStyle::Band2Horizontal, m_currentTableStyleProperties);
    }
    else if (type == "band1Vert") {
        m_currentStyle->addProperties(MSOOXML::DrawingTableStyle::Band1Vertical, m_currentTableStyleProperties);
    }
    else if (type == "band2Vert") {
        m_currentStyle->addProperties(MSOOXML::DrawingTableStyle::Band2Vertical, m_currentTableStyleProperties);
    }
    else if (type == "firstCol") {
//         if (m_currentTableStyleProperties->insideV.innerPen.widthF() == 0) {
//             m_currentTableStyleProperties->insideV = m_currentTableStyleProperties->right;
//         }
        if (m_currentTableStyleProperties->target == MSOOXML::TableStyleProperties::Table) {
            m_currentTableStyleProperties->target = MSOOXML::TableStyleProperties::TableColumn;
        }
        m_currentStyle->addProperties(MSOOXML::DrawingTableStyle::FirstCol, m_currentTableStyleProperties);
    }
    else if (type == "lastCol") {
        if (m_currentTableStyleProperties->target == MSOOXML::TableStyleProperties::Table) {
            m_currentTableStyleProperties->target = MSOOXML::TableStyleProperties::TableColumn;
        }
        m_currentStyle->addProperties(MSOOXML::DrawingTableStyle::LastCol, m_currentTableStyleProperties);
    }
    else if (type == "nwCell") {
        m_currentStyle->addProperties(MSOOXML::DrawingTableStyle::NwCell, m_currentTableStyleProperties);
    }
    else if (type == "neCell") {
        m_currentStyle->addProperties(MSOOXML::DrawingTableStyle::NeCell, m_currentTableStyleProperties);
    }
    else if (type == "swCell") {
        m_currentStyle->addProperties(MSOOXML::DrawingTableStyle::SwCell, m_currentTableStyleProperties);
    }
    else if (type == "seCell") {
        m_currentStyle->addProperties(MSOOXML::DrawingTableStyle::SeCell, m_currentTableStyleProperties);
    }

    m_currentTableStyleProperties = 0;

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL name
//! 17.7.4.9 name (Primary Style Name)
/*
 Parent elements:
 - style (§17.7.4.17)

 Child elements:
 - none
*/
KoFilter::ConversionStatus DocxXmlStylesReader::read_name()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_INTO(val, m_name)
    m_name.replace(QLatin1Char(' '), QLatin1Char('_'));
    readNext();
    READ_EPILOGUE
}
