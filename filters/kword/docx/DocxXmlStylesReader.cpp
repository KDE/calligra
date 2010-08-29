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

#include "DocxXmlStylesReader.h"
#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <MsooXmlUnits.h>
#include <KoXmlWriter.h>

#define MSOOXML_CURRENT_NS "w"
#define BIND_READ_CLASS MSOOXML::MsooXmlStylesReader
#define MSOOXML_CURRENT_CLASS DocxXmlStylesReader

#include <MsooXmlReader_p.h>

//#include <MsooXmlCommonReaderImpl.h> // this adds w:pPr, etc.

DocxXmlStylesReader::DocxXmlStylesReader(KoOdfWriters *writers)
        : DocxXmlDocumentReader(writers), m_context(NoContext) //, m_characterStyle(0)
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
    Q_UNUSED(context)
    kDebug() << "=============================";
    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }

    //w:document
    readNext();
    kDebug() << *this << namespaceUri();

    if (!expectEl("w:styles")) {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::wordprocessingml)) {
        return KoFilter::WrongFormat;
    }
    /*
        const QXmlStreamAttributes attrs( attributes() );
        for (int i=0; i<attrs.count(); i++) {
            kDebug() << "1 NS prefix:" << attrs[i].name() << "uri:" << attrs[i].namespaceUri();
        }*/

    QXmlStreamNamespaceDeclarations namespaces(namespaceDeclarations());
    for (int i = 0; i < namespaces.count(); i++) {
        kDebug() << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
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

    createDefaultStyle(KoGenStyle::ParagraphStyle, "paragraph");
    createDefaultStyle(KoGenStyle::TextStyle, "text");
    createDefaultStyle(KoGenStyle::TableStyle, "table");
    //createDefaultStyle(KoGenStyle::GraphicStyle, "graphic");
    //createDefaultStyle(KoGenStyle::TableRowStyle, "table-row");
    //createDefaultStyle("numbering");

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        BREAK_IF_END_OF(styles)
        if (isStartElement()) {
            TRY_READ_IF(docDefaults)
            ELSE_TRY_READ_IF(style)
            else {
                m_context = NoContext;
            }
            //! @todo add ELSE_WRONG_FORMAT
        }
    }

    if (!expectElEnd("w:styles")) {
        return KoFilter::WrongFormat;
    }

    // add default styles:
    for (QMap<QByteArray, KoGenStyle*>::ConstIterator it(m_defaultStyles.constBegin());
         it!=m_defaultStyles.constEnd(); ++it)
    {
        kDebug() << it.key();
        mainStyles->insert(*it.value());
    }
    qDeleteAll(m_defaultStyles);
    m_defaultStyles.clear();

    kDebug() << "===========finished============";
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL docDefaults
//! w:docDefaults handler (Document Default Paragraph and Run Properties)
/*! ECMA-376, 17.5.5.1, p.723.

 Document Defaults

 The first formatting information which is applied to all regions of text in a WordprocessingML
 document when that document is displayed is the document defaults. The document defaults specify
 the default set of properties which shall be inherited by every paragraph and run of text within
 all stories of the current WordprocessingML document. If no other formatting information
 was referenced by that text, these properties would solely define the formatting  of the resulting
 text.

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
    m_context = DocDefaultsContext;

    m_currentTextStyle = KoGenStyle();
    m_currentParagraphStyle = KoGenStyle(KoGenStyle::ParagraphStyle, "paragraph");

    while (!atEnd()) {
        readNext();
        kDebug() << *this;
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF(pPrDefault)
            ELSE_TRY_READ_IF(rPrDefault)
            ELSE_WRONG_FORMAT
        }
    }
    m_defaultStyle = m_currentTextStyle;
    m_currentTextStyle = KoGenStyle();
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
        kDebug() << *this;
        BREAK_IF_END_OF(CURRENT_EL);
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
        kDebug() << *this;
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            TRY_READ_IF_IN_CONTEXT(rPr)
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
 - pPr (Style Paragraph Properties) §17.7.8.2
 - qFormat (Primary Style) §17.7.4.14
 - rPr (Run Properties) §17.7.9.1
 - rsid (Revision Identifier for Style Definition) §17.7.4.15
 - semiHidden (Hide Style From Main User Interface) §17.7.4.16
 - [done] tblPr (Style Table Properties) §17.7.6.4
 - [done] tblStylePr (Style Conditional Table Formatting Properties) §17.7.6.6
 - tcPr (Style Table Cell Properties) §17.7.6.9
 - trPr (Style Table Row Properties) §17.7.6.11
 - uiPriority (Optional User Interface Sorting Order) §17.7.4.19
 - unhideWhenUsed (Remove Semi-Hidden Property When Style Is Used) §17.7.4.20

 @todo support all elements
*/
KoFilter::ConversionStatus DocxXmlStylesReader::read_style()
{
    READ_PROLOGUE
    m_context = StyleContext;

    const QXmlStreamAttributes attrs(attributes());
    m_name.clear();

    //! 17.18.83 ST_StyleType (Style Types)
    READ_ATTR(type)
    //! @todo numbering style
    if (type == QLatin1String("numbering"))
        return KoFilter::OK; // give up
    const QString odfType(ST_StyleType_to_ODF(type));
    if (odfType.isEmpty()) {
        return KoFilter::WrongFormat;
    }

    QString styleName;
    READ_ATTR_INTO(styleId, styleName)
    //! @todo should we skip "Normal" style?

    // w:default specifies that this style is the default for this style type.
    // This property is used in conjunction with the type attribute to determine the style which
    // is applied to objects that do not explicitly declare a style.
    const bool isDefault = readBooleanAttr("w:default");
    if (isDefault) {
        if (!m_defaultStyles.contains(odfType.toLatin1())) {
            raiseUnexpectedAttributeValueError(odfType, "w:type");
            return KoFilter::WrongFormat;
        }
        kDebug() << "Setting default style of family" << odfType << "...";
        if (type == "character") {
            m_currentTextStyle = *m_defaultStyles.value(odfType.toLatin1());
        }
        else if (type == "paragraph") {
            m_currentParagraphStyle = *m_defaultStyles.value(odfType.toLatin1());
        }
    }
    else {
        if (type == "character") {
            m_currentTextStyle = KoGenStyle(KoGenStyle::TextStyle, odfType.toLatin1());
        }
        else if (type == "paragraph") {
            m_currentParagraphStyle = KoGenStyle(KoGenStyle::ParagraphStyle, odfType.toLatin1());
        }
        else if (type == "table") {
            m_currentTableStyle = KoGenStyle(KoGenStyle::TableStyle, odfType.toLatin1());

            // These are needed because ooxml stores table styles differently from odf
            m_currentTableCellStyleLeft = KoGenStyle(KoGenStyle::TableCellStyle, "table-cell");
            m_currentTableCellStyleRight = KoGenStyle(KoGenStyle::TableCellStyle, "table-cell");
            m_currentTableCellStyleTop = KoGenStyle(KoGenStyle::TableCellStyle, "table-cell");
            m_currentTableCellStyleBottom = KoGenStyle(KoGenStyle::TableCellStyle, "table-cell");
            m_currentTableCellStyleInsideV = KoGenStyle(KoGenStyle::TableCellStyle, "table-cell");
            m_currentTableCellStyleInsideH = KoGenStyle(KoGenStyle::TableCellStyle, "table-cell");
        }
    }
    MSOOXML::Utils::Setter<bool> currentTextStylePredefinedSetter(&m_currentTextStylePredefined, false);
    MSOOXML::Utils::Setter<bool> currentParagraphStylePredefinedSetter(&m_currentParagraphStylePredefined, false);
    m_currentTextStylePredefined = true;
    m_currentParagraphStylePredefined = true;

    QString nextStyleName;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            const QXmlStreamAttributes attrs(attributes());
            TRY_READ_IF(name)
            ELSE_TRY_READ_IF_IN_CONTEXT(rPr)
            ELSE_TRY_READ_IF(pPr)
            ELSE_TRY_READ_IF(tblPr)
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
            //! @todo add ELSE_WRONG_FORMAT
        }
    }
    KoGenStyles::InsertionFlags insertionFlags = KoGenStyles::DontAddNumberToName;
    if (styleName.isEmpty()) {
        styleName = m_name.replace(" ", "_");
        if (styleName.isEmpty()) {
            // allow for numbering for generated style names
            styleName = odfType;
            insertionFlags = KoGenStyles::NoFlag;
        }
    }

    // When reading from styles, we allow duplicates
    insertionFlags = insertionFlags | KoGenStyles::AllowDuplicates;

    m_currentTextStylePredefined = false;
    m_currentParagraphStylePredefined = false;

    // insert style
    if (isDefault) {
        // do not insert, will be inserted at the end
        kDebug() << "Default style of family" << odfType << "created";
        if (type == "character") {
            *m_defaultStyles.value(odfType.toLatin1()) = m_currentTextStyle;
        }
        else if (type == "paragraph") {
            *m_defaultStyles.value(odfType.toLatin1()) = m_currentParagraphStyle;
        }
    }
    else {
        // Style class: A style may belong to an arbitrary class of styles.
        // The class is an arbitrary string. The class has no meaning within the file format itself,
        // but it can for instance be evaluated by user interfaces to show a list of styles where
        // the styles are grouped by its name.
        //! @todo oo.o converter defines these classes: list, extra, index, chapter
        if (type == "character") {
            m_currentTextStyle.addAttribute("style:class", "text");

            styleName = mainStyles->insert(m_currentTextStyle, styleName, insertionFlags);
        }
        else if (type == "paragraph") {
            m_currentParagraphStyle.addAttribute("style:class", "text");

            MSOOXML::Utils::copyPropertiesFromStyle(m_currentTextStyle, m_currentParagraphStyle, KoGenStyle::TextType);
            styleName = mainStyles->insert(m_currentParagraphStyle, styleName, insertionFlags);
        }
        else if (type == "table") {
            styleName = mainStyles->insert(m_currentTableStyle, styleName, insertionFlags);

            // The names here are partly hard coded, it means in practise that if there was a style called
            // 'style'-cellsLeft, then this logic would not work, however it is not very likely.
            mainStyles->insert(m_currentTableCellStyleLeft, QString("%1-%2").arg(styleName).arg("cellsLeft"), insertionFlags);
            mainStyles->insert(m_currentTableCellStyleRight, QString("%1-%2").arg(styleName).arg("cellsRight"), insertionFlags);
            mainStyles->insert(m_currentTableCellStyleTop, QString("%1-%2").arg(styleName).arg("cellsTop"), insertionFlags);
            mainStyles->insert(m_currentTableCellStyleBottom, QString("%1-%2").arg(styleName).arg("cellsBottom"), insertionFlags);
            mainStyles->insert(m_currentTableCellStyleInsideV, QString("%1-%2").arg(styleName).arg("cellsInsideV"), insertionFlags);
            mainStyles->insert(m_currentTableCellStyleInsideH, QString("%1-%2").arg(styleName).arg("cellsInsideH"), insertionFlags);

        }
        if (!nextStyleName.isEmpty()) {
            mainStyles->insertStyleRelation(styleName, nextStyleName, "style:next-style-name");
        }
    }

    m_currentParagraphStyle = KoGenStyle();
    m_currentTextStyle = KoGenStyle();

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL tblStylePr
//! tbleStylePr (table style handler)
KoFilter::ConversionStatus DocxXmlStylesReader::read_tblStylePr()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(type)
    if (!type.isEmpty()) {

    }

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL);
        if (isStartElement()) {
            //TRY_READ_IF(name)
            //! @todo add ELSE_WRONG_FORMAT
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL name
//! 17.7.4.9 name (Primary Style Name)
KoFilter::ConversionStatus DocxXmlStylesReader::read_name()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());
    READ_ATTR_INTO(val, m_name)
    SKIP_EVERYTHING
    READ_EPILOGUE
}
