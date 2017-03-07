/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "DocxXmlSettingsReader.h"

#include "DocxDebug.h"

#define MSOOXML_CURRENT_NS "w"
#define MSOOXML_CURRENT_CLASS MsooXmlSettingsReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlSchemas.h>
#include <MsooXmlReader_p.h>


DocxXmlSettingsReaderContext::DocxXmlSettingsReaderContext(QMap<QString, QVariant>& _documentSettings)
    : documentSettings(_documentSettings)
{
}

class DocxXmlSettingsReader::Private
{
public:
    Private() {
    }
    ~Private() {
    }
};

DocxXmlSettingsReader::DocxXmlSettingsReader(KoOdfWriters *writers)
    : MSOOXML::MsooXmlCommonReader(writers)
    , d(new Private)
{
    init();
}

DocxXmlSettingsReader::~DocxXmlSettingsReader()
{
    delete d;
}

void DocxXmlSettingsReader::init()
{
}

KoFilter::ConversionStatus DocxXmlSettingsReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = static_cast<DocxXmlSettingsReaderContext*>(context);

    debugDocx << "=============================";
    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }
    readNext();

    debugDocx << *this << namespaceUri();
    if (!expectEl(QList<QByteArray>() << "w:settings")) {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::wordprocessingml)) {
        return KoFilter::WrongFormat;
    }

    QXmlStreamNamespaceDeclarations namespaces(namespaceDeclarations());

    //! @todo find out whether the namespace returned by namespaceUri()
    //!       is exactly the same ref as the element of namespaceDeclarations()
    if (!namespaces.contains(QXmlStreamNamespaceDeclaration("w", MSOOXML::Schemas::wordprocessingml))) {
        raiseError(i18n("Namespace \"%1\" not found", QLatin1String(MSOOXML::Schemas::wordprocessingml)));
        return KoFilter::WrongFormat;
    }

    const QString qn(qualifiedName().toString());

    RETURN_IF_ERROR(read_settings())

    if (!expectElEnd(qn)) {
        return KoFilter::WrongFormat;
    }
    debugDocx << "===========finished============";

    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL settings
//! w:footnotes handler (Document footnotes)
/*!

 Parent elements:
 - [done] root element of Wordprocessing Header part

 Child elements:
 - activeWritingStyle (Grammar Checking Settings) §17.15.1.1
 - alignBordersAndEdges (Align Paragraph and Table Borders with Page Border) §17.15.1.2
 - alwaysMergeEmptyNamespace (Do Not Mark Custom XML Elements With No Namespace As Invalid) §17.15.1.3
 - alwaysShowPlaceholderText (Use Custom XML Element Names as Default Placeholder Text) §17.15.1.4
 - attachedSchema (Attached Custom XML Schema) §17.15.1.5
 - attachedTemplate (Attached Document Template) §17.15.1.6
 - autoFormatOverride (Allow Automatic Formatting to Override Formatting Protection Settings) §17.15.1.9
 - autoHyphenation (Automatically Hyphenate Document Contents When Displayed) §17.15.1.10
 - bookFoldPrinting (Book Fold Printing) §17.15.1.11
 - bookFoldPrintingSheets (Number of Pages Per Booklet) §17.15.1.12
 - bookFoldRevPrinting (Reverse Book Fold Printing) §17.15.1.13
 - bordersDoNotSurroundFooter (Page Border Excludes Footer) §17.15.1.14
 - bordersDoNotSurroundHeader (Page Border Excludes Header) §17.15.1.15
 - captions (Caption Settings) §17.15.1.17
 - characterSpacingControl (Character-Level Whitespace Compression) §17.15.1.18
 - clickAndTypeStyle (Paragraph Style Applied to Automatically Generated Paragraphs) §17.15.1.19
 - clrSchemeMapping (Theme Color Mappings) §17.15.1.20
 - compat (Compatibility Settings) §17.15.1.21
 - consecutiveHyphenLimit (Maximum Number of Consecutively Hyphenated Lines) §17.15.1.22
 - decimalSymbol (Radix Point for Field Code Evaluation) §17.15.1.23
 - defaultTableStyle (Default Table Style for Newly Inserted Tables) §17.15.1.24
 - [done] defaultTabStop (Distance Between Automatic Tab Stops) §17.15.1.25
 - [done] displayBackgroundShape (Display Background Objects When Displaying Document) §17.15.1.26
 - displayHorizontalDrawingGridEvery (Distance between Horizontal Gridlines) §17.15.1.27
 - displayVerticalDrawingGridEvery (Distance between Vertical Gridlines) §17.15.1.28
 - documentProtection (Document Editing Restrictions) §17.15.1.29
 - documentType (Document Classification) §17.15.1.30
 - docVars (Document Variables) §17.15.1.32
 - doNotAutoCompressPictures (Do Not Automatically Compress Images) §17.15.1.33
 - doNotDemarcateInvalidXml (Do Not Show Visual Indicator For Invalid Custom XML Markup) §17.15.1.34
 - doNotDisplayPageBoundaries (Do Not Display Visual Boundary For Header/Footer or Between Pages) §17.15.1.35
 - doNotEmbedSmartTags (Remove Smart Tags When Saving) §17.15.1.36
 - doNotHyphenateCaps (Do Not Hyphenate Words in ALL CAPITAL LETTERS) §17.15.1.37
 - doNotIncludeSubdocsInStats (Do Not Include Content in Text Boxes, Footnotes, and Endnotes in Document Statistics) §17.15.1.38
 - doNotShadeFormData (Do Not Show Visual Indicator For Form Fields) §17.15.1.39
 - doNotTrackFormatting (Do Not Track Formatting Revisions When Tracking Revisions) §17.15.1.40
 - doNotTrackMoves (Do Not Use Move Syntax When Tracking Revisions) §17.15.1.41
 - doNotUseMarginsForDrawingGridOrigin (Do Not Use Margins for Drawing Grid Origin) §17.15.1.42
 - doNotValidateAgainstSchema (Do Not Validate Custom XML Markup Against Schemas) §17.15.1.43
 - drawingGridHorizontalOrigin (Drawing Grid Horizontal Origin Point) §17.15.1.44
 - drawingGridHorizontalSpacing (Drawing Grid Horizontal Grid Unit Size) §17.15.1.45
 - drawingGridVerticalOrigin (Drawing Grid Vertical Origin Point) §17.15.1.46
 - drawingGridVerticalSpacing (Drawing Grid Vertical Grid Unit Size) §17.15.1.47
 - embedSystemFonts (Embed Common System Fonts) §17.8.3.7
 - embedTrueTypeFonts (Embed TrueType Fonts) §17.8.3.8
 - endnotePr (Document-Wide Endnote Properties) §17.11.4
 - evenAndOddHeaders (Different Even/Odd Page Headers and Footers) §17.10.1
 - footnotePr (Document-Wide Footnote Properties) §17.11.12
 - forceUpgrade (Upgrade Document on Open) §17.15.1.48
 - formsDesign (Structured Document Tag Placeholder Text Should be Resaved) §17.15.1.49
 - gutterAtTop (Position Gutter At Top of Page) §17.15.1.50
 - hideGrammaticalErrors (Do Not Display Visual Indication of Grammatical Errors) §17.15.1.51
 - hideSpellingErrors (Do Not Display Visual Indication of Spelling Errors) §17.15.1.52
 - hyphenationZone (Hyphenation Zone) §17.15.1.53
 - ignoreMixedContent (Ignore Mixed Content When Validating Custom XML Markup) §17.15.1.54
 - linkStyles (Automatically Update Styles From Document Template) §17.15.1.55
 - listSeparator (List Separator for Field Code Evaluation) §17.15.1.56
 - mailMerge (Mail Merge Settings) §17.14.20
 - mathPr (Math Properties) §22.1.2.62
 - mirrorMargins (Mirror Page Margins) §17.15.1.57
 - noLineBreaksAfter (Custom Set of Characters Which Cannot End a Line) §17.15.1.58
 - noLineBreaksBefore (Custom Set Of Characters Which Cannot Begin A Line) §17.15.1.59
 - noPunctuationKerning (Never Kern Punctuation Characters) §17.15.1.60
 - printFormsData (Only Print Form Field Content) §17.15.1.61
 - printFractionalCharacterWidth (Print Fractional Character Widths) §17.15.1.62
 - printPostScriptOverText (Print PostScript Codes With Document Text) §17.15.1.63
 - printTwoOnOne (Print Two Pages Per Sheet) §17.15.1.64
 - proofState (Spelling and Grammatical Checking State) §17.15.1.65
 - readModeInkLockDown (Freeze Document Layout) §17.15.1.66
 - removeDateAndTime (Remove Date and Time from Annotations) §17.15.1.67
 - removePersonalInformation (Remove Personal Information from Document Properties) §17.15.1.68
 - revisionView (Visibility of Annotation Types) §17.15.1.69
 - rsids (Listing of All Revision Save ID Values) §17.15.1.72
 - saveFormsData (Only Save Form Field Content) §17.15.1.73
 - saveInvalidXml (Allow Saving Document As XML File When Custom XML Markup Is Invalid) §17.15.1.74
 - savePreviewPicture (Generate Thumbnail For Document On Save) §17.15.1.75
 - saveSubsetFonts (Subset Fonts When Embedding) §17.8.3.15
 - saveThroughXslt (Custom XSL Transform To Use When Saving As XML File) §17.15.1.76
 - saveXmlDataOnly (Only Save Custom XML Markup) §17.15.1.77
 - schemaLibrary (Embedded Custom XML Schema Supplementary Data) §23.2.2
 - showEnvelope (Show E-Mail Message Header) §17.15.1.79
 - showXMLTags (Show Visual Indicators for Custom XML Markup Start/End Locations) §17.15.1.80
 - smartTagType (Supplementary Smart Tag Information) §17.15.1.81
 - strictFirstAndLastChars (Use Strict Kinsoku Rules for Japanese Text) §17.15.1.82
 - styleLockQFSet (Prevent Replacement of Styles Part) §17.15.1.83
 - styleLockTheme (Prevent Modification of Themes Part) §17.15.1.84
 - stylePaneFormatFilter (Suggested Filtering for List of Document Styles) §17.15.1.85
 - stylePaneSortMethod (Suggested Sorting for List of Document Styles) §17.15.1.86
 - summaryLength (Percentage of Document to Use When Generating Summary) §17.15.1.87
 - themeFontLang (Theme Font Languages) §17.15.1.88
 - trackRevisions (Track Revisions to Document) §17.15.1.89
 - updateFields (Automatically Recalculate Fields on Open) §17.15.1.90
 - useXSLTWhenSaving (Save Document as XML File through Custom XSL Transform) §17.15.1.91
 - view (Document View Setting) §17.15.1.92
 - writeProtection (Write Protection) §17.15.1.93
 - zoom (Magnification Setting) §17.15.1.94
*/
KoFilter::ConversionStatus DocxXmlSettingsReader::read_settings()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(defaultTabStop)
            ELSE_TRY_READ_IF(displayBackgroundShape)
            ELSE_TRY_READ_IF(clrSchemeMapping)
            SKIP_UNKNOWN
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL defaultTabStop
//! w:defaultTabStop handler (Default Tab Stop)
/*!

 Parent elements:
 - [done] settings (§17.15.1.78)

 Child elements:
 - none
*/
KoFilter::ConversionStatus DocxXmlSettingsReader::read_defaultTabStop()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(val)

    m_context->documentSettings["defaultTabStop"] = val;

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL displayBackgroundShape
//! w:displayBackgroundShape handler (Display background shape)
/*!

 Parent elements:
 - [done] settings (§17.15.1.78)

 Child elements:
 - none
*/
KoFilter::ConversionStatus DocxXmlSettingsReader::read_displayBackgroundShape()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(val)

    m_context->documentSettings["diplayBackgroundShape"] = val;

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL clrSchemeMapping
//! w:clrSchemeMapping handler (Color scheme mapping)
/*!

 Parent elements:
 - [done] settings (§17.15.1.78)

 Child elements:
 - none
*/
KoFilter::ConversionStatus DocxXmlSettingsReader::read_clrSchemeMapping()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    int index = 0;
    while (index < attrs.size()) {
        QString handledAttr = attrs.at(index).name().toString();
        // There is a mismatch between the settings file and the theme file
        // created with the naming of the theme colors, here we make sure
        // that they match.
        if (handledAttr == "t1") {
            handledAttr = "tx1";
        }
        else if (handledAttr == "t2") {
            handledAttr = "tx2";
        }
        else if (handledAttr == "hyperlink") {
            handledAttr = "hlink";
        }
        else if (handledAttr == "followedHyperlink") {
            handledAttr = "folHlink";
        }
        QString attrValue = attrs.value(handledAttr).toString();
        if (attrValue == "light1") {
            attrValue = "lt1";
        }
        else if (attrValue == "light2") {
            attrValue = "lt2";
        }
        else if (attrValue == "dark1") {
            attrValue = "dk1";
        }
        else if (attrValue == "dark2") {
            attrValue = "dk2";
        }
        else if (attrValue == "hyperlink") {
            attrValue = "hlink";
        }
        else if (attrValue == "followedHyperlink") {
            attrValue = "folHlink";
        }
        m_context->colorMap[handledAttr] = attrValue;
        ++index;
    }

    readNext();
    READ_EPILOGUE
}

