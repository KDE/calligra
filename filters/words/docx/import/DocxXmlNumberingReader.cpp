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

#include "DocxXmlNumberingReader.h"

#include "DocxDebug.h"

#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <MsooXmlUnits.h>

#define MSOOXML_CURRENT_NS "w"
#define MSOOXML_CURRENT_CLASS MsooXmlNumberingReader
#define BIND_READ_CLASS MSOOXML_CURRENT_CLASS

#include <MsooXmlReader_p.h>


class DocxXmlNumberingReader::Private
{
public:
    Private() {
    }
    ~Private() {
    }
};

DocxXmlNumberingReader::DocxXmlNumberingReader(KoOdfWriters *writers)
    : DocxXmlDocumentReader(writers)
    , d(new Private)
{
    init();
}

DocxXmlNumberingReader::~DocxXmlNumberingReader()
{
    delete d;
}

void DocxXmlNumberingReader::init()
{
    m_currentVMLProperties.insideGroup = false;
    m_outputFrames = false;
}

KoFilter::ConversionStatus DocxXmlNumberingReader::read(MSOOXML::MsooXmlReaderContext* context)
{
    m_context = static_cast<DocxXmlDocumentReaderContext*>(context);

    readNext();
    if (!isStartDocument()) {
        return KoFilter::WrongFormat;
    }
    readNext();
    debugDocx << *this << namespaceUri();
    if (!expectEl(QList<QByteArray>() << "w:numbering")) {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::wordprocessingml)) {
        return KoFilter::WrongFormat;
    }
    QXmlStreamNamespaceDeclarations namespaces(namespaceDeclarations());
    /*    for (int i = 0; i < namespaces.count(); i++) {
        debugDocx << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }*/
    //! @todo find out whether the namespace returned by namespaceUri()
    //!       is exactly the same ref as the element of namespaceDeclarations()

    if (!namespaces.contains(QXmlStreamNamespaceDeclaration("w", MSOOXML::Schemas::wordprocessingml))) {
        raiseError(i18n("Namespace \"%1\" not found", QLatin1String(MSOOXML::Schemas::wordprocessingml)));
        return KoFilter::WrongFormat;
    }

    const QString qn(qualifiedName().toString());

    RETURN_IF_ERROR(read_numbering())

    if (!expectElEnd(qn)) {
        return KoFilter::WrongFormat;
    }
    debugDocx << "===========finished============";
    return KoFilter::OK;
}

#undef CURRENT_EL
#define CURRENT_EL abstractNum
//! w:abstractNum handler (Abstract Numbering)
/*!

 Parent elements:
 - [done] numbering (§17.9.17)

 Child elements:
 - [done] lvl (Numbering Level Definition) §17.9.7
 - multiLevelType (Abstract Numbering Definition Type) §17.9.13
 - name (Abstract Numbering Definition Name) §17.9.14
 - nsid (Abstract Numbering Definition Identifier) §17.9.15
 - numStyleLink (Numbering Style Reference) §17.9.22
 - styleLink (Numbering Style Definition) §17.9.28
 - tmpl (Numbering Template Code) §17.9.30

//! @todo: Handle all children
*/
KoFilter::ConversionStatus DocxXmlNumberingReader::read_abstractNum()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());
    TRY_READ_ATTR(abstractNumId)

    m_currentBulletList.clear();

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (name() == "lvl") {
                m_currentBulletProperties.clear();
                TRY_READ(lvl)
                m_currentBulletList.append(m_currentBulletProperties);
            }
            SKIP_UNKNOWN
        }
    }

    m_abstractListStyles[abstractNumId] = m_currentBulletList;

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lvlOverride
//! w:lvlOverride handler (Numbering Level Definition Override)
/*!
 Parent elements:
 - [done] num (§17.9.16)

 Child elements:
 - [done] lvl (Numbering Level Override Definition) §17.9.6
 - startOverride (Numbering Level Starting Value Override) §17.9.27

*/
KoFilter::ConversionStatus DocxXmlNumberingReader::read_lvlOverride()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(ilvl)
    int level = 0;
    STRING_TO_INT(ilvl, level, QString("w:lvlOverride"));
    level++;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (name() == "lvl") {
                m_currentBulletProperties.clear();
                TRY_READ(lvl)
                int index = 0;
                while (index < m_currentBulletList.size()) {
                    // Overriding lvl information
                    if (m_currentBulletList.at(index).m_level == m_currentBulletProperties.m_level) {
                        m_currentBulletList.replace(index, m_currentBulletProperties);
                        break;
                    }
                    ++index;
                }
            }
            else if (name() == "startOverride") {
                int index = 0;
                while (index < m_currentBulletList.size()) {
                    if (m_currentBulletList.at(index).m_level == level)
                    {
                        const QXmlStreamAttributes attrs2(attributes());
                        QString val( attrs2.value(QUALIFIED_NAME(val)).toString() );
                        if (!val.isEmpty()) {
                            m_currentBulletList[index].setStartValue(val);
                        }
                        m_currentBulletList[index].setStartOverride(true);
                        break;
                    }
                    ++index;
                }
            }
            SKIP_UNKNOWN
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lvl
//! w:lvl handler (Level)
/*!

 Parent elements:
 - [done] abstractNum (§17.9.1)

 Child elements:
 - isLgl (Display All Levels Using Arabic Numerals) §17.9.4
 - legacy (Legacy Numbering Level Properties) §17.9.5
 - [done] lvlJc (Justification) §17.9.8
 - [done] lvlPicBulletId (Picture Numbering Symbol Definition Reference) §17.9.10
 - lvlRestart (Restart Numbering Level Symbol) §17.9.11
 - [done] lvlText (Numbering Level Text) §17.9.12
 - [done] numFmt (Numbering Format) §17.9.18
 - [done] pPr (Numbering Level Associated Paragraph Properties) §17.9.23
 - pStyle (Paragraph Style's Associated Numbering Level) §17.9.24
 - [done] rPr (Numbering Symbol Run Properties) §17.9.25
 - [done] start (Starting Value) §17.9.26
 - suff (Content Between Numbering Symbol and Paragraph Text) §17.9.29

//! @todo: Handle all children
*/
KoFilter::ConversionStatus DocxXmlNumberingReader::read_lvl()
{
    READ_PROLOGUE

    m_currentTextStyle = KoGenStyle(KoGenStyle::TextStyle, "text");
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(ilvl)
    if (!ilvl.isEmpty()) {
        m_currentBulletProperties.m_level = ilvl.toInt() + 1;
    }

    m_bulletCharacter.clear();
    m_bulletStyle = false;

    bool pictureType = false;

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(start)
            ELSE_TRY_READ_IF(numFmt)
            ELSE_TRY_READ_IF(lvlText)
            ELSE_TRY_READ_IF(lvlJc)
            ELSE_TRY_READ_IF(suff)
            else if (name() == "lvlPicBulletId") {
                TRY_READ(lvlPicBulletId)
                pictureType = true;
            }
            else if (name() == "pPr") {
                TRY_READ(pPr_numbering)
            }
            else if (name() == "rPr") {
                TRY_READ(rPr)
            }
            SKIP_UNKNOWN
        }
    }

    if (!pictureType && m_bulletStyle && !m_bulletCharacter.isEmpty()) {
        m_currentBulletProperties.setBulletChar(m_bulletCharacter);
    }
    m_currentBulletProperties.setTextStyle(m_currentTextStyle);

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL numPicBullet
//! w:numPicBullet handler (Number picture bullet)
/*!

 Parent elements:

 Child elements:
*/
KoFilter::ConversionStatus DocxXmlNumberingReader::read_numPicBullet()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    READ_ATTR(numPicBulletId)

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(pict)
            SKIP_UNKNOWN
        }
    }

    m_picBulletPaths[numPicBulletId] = m_currentVMLProperties.imagedataPath;

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL numbering
//! w:numbering handler (Numbering)
/*!

 Parent elements:

 Child elements:
//! @todo: Handle all children
*/
KoFilter::ConversionStatus DocxXmlNumberingReader::read_numbering()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            TRY_READ_IF(abstractNum)
            ELSE_TRY_READ_IF(numPicBullet)
            ELSE_TRY_READ_IF(num)
            SKIP_UNKNOWN
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL numFmt
//! w:numfmt handler (Number format)
/*!

 Parent elements:

 Child elements:
//! @todo: Handle all children
*/
KoFilter::ConversionStatus DocxXmlNumberingReader::read_numFmt()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(val)
    if (!val.isEmpty()) {
        if (val == "lowerRoman") {
            m_currentBulletProperties.setNumFormat("i");
        }
        else if (val == "lowerLetter") {
            m_currentBulletProperties.setNumFormat("a");
        }
        else if (val == "decimal") {
            m_currentBulletProperties.setNumFormat("1");
        }
        else if (val == "upperRoman") {
            m_currentBulletProperties.setNumFormat("I");
        }
        else if (val == "upperLetter") {
            m_currentBulletProperties.setNumFormat("A");
        }
        else if (val == "bullet") {
            m_bulletStyle = true;
        }
        else if (val == "ordinal") {
            // in ooxml this means having 1st, 2nd etc. but currently there's no real support for it
            m_currentBulletProperties.setNumFormat("1");
            m_currentBulletProperties.setSuffix(".");
        }
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL start
//! w:start handler (Start)
/*!

 Parent elements:

 Child elements:
//! @todo: Handle all children
*/
KoFilter::ConversionStatus DocxXmlNumberingReader::read_start()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(val)
    if (!val.isEmpty()) {
        m_currentBulletProperties.setStartValue(val);
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lvlText
//! w:lvlText handler (Level Text)
/*!

 Parent elements:

 Child elements:
//! @todo: Handle all children
*/
KoFilter::ConversionStatus DocxXmlNumberingReader::read_lvlText()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(val)
    if (!val.isEmpty()) {
        if (!m_bulletStyle) {
            if (val.at(0) == '%' && val.length() == 2) {
                m_currentBulletProperties.setSuffix("");
            }
            else {
                m_currentBulletProperties.setSuffix(val.right(1));
            }
        }
        else {
            m_bulletCharacter = val;
        }
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL num
//! w:num handler (Number)
/*!

 Parent elements:
 - [done] numbering (§17.9.17)

 Child elements:
 - [done] abstractNumId (Abstract Numbering Definition Reference) §17.9.2
 - [done] lvlOverride (Numbering Level Definition Override) §17.9.9
*/
KoFilter::ConversionStatus DocxXmlNumberingReader::read_num()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(numId)

    m_currentListStyle = KoGenStyle(KoGenStyle::ListStyle);

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (name() == "abstractNumId") {
               TRY_READ(abstractNumId)
               m_currentBulletList = m_abstractListStyles[m_currentAbstractId];
               m_context->m_abstractNumIDs[numId] = m_currentAbstractId;
            }
            // lvlOverride may modify the bulletlist which we get above
            ELSE_TRY_READ_IF(lvlOverride)
            ELSE_WRONG_FORMAT
        }
    }

    m_context->m_bulletStyles[numId] = m_currentBulletList;

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL pPr
//! w:pPr handler (Numbering Level Associated Paragraph Properties)
/*! ECMA-376, §17.9.23, p.808

 Parent elements:
 - [done] lvl (§17.9.6)
 - [done] lvl (§17.9.7)

 Child element NOT provided compared to pPr (17.3.1.26):
 - rPr (Run Properties for the Paragraph Mark)
 - sectPr (Section Properties)

 Child elements:
 - adjustRightInd (Automatically Adjust Right Indent When Using Document Grid) §17.3.1.1
 - autoSpaceDE (Automatically Adjust Spacing of Latin and East Asian Text) §17.3.1.2
 - autoSpaceDN (Automatically Adjust Spacing of East Asian Text and Numbers) §17.3.1.3
 - bidi (Right to Left Paragraph Layout) §17.3.1.6
 - cnfStyle (Paragraph Conditional Formatting) §17.3.1.8
 - contextualSpacing (Ignore Spacing Above and Below When Using Identical Styles) §17.3.1.9
 - divId (Associated HTML div ID) §17.3.1.10
 - framePr (Text Frame Properties) §17.3.1.11
 - [done] ind (Paragraph Indentation) §17.3.1.12
 - jc (Paragraph Alignment) §17.3.1.13
 - keepLines (Keep All Lines On One Page) §17.3.1.14
 - keepNext (Keep Paragraph With Next Paragraph) §17.3.1.15
 - kinsoku (Use East Asian Typography Rules for First and Last Character per Line) §17.3.1.16
 - mirrorIndents (Use Left/Right Indents as Inside/Outside Indents) §17.3.1.18
 - numPr (Numbering Definition Instance Reference) §17.3.1.19
 - outlineLvl (Associated Outline Level) §17.3.1.20
 - overflowPunct (Allow Punctuation to Extend Past Text Extents) §17.3.1.21
 - pageBreakBefore (Start Paragraph on Next Page) §17.3.1.23
 - pBdr (Paragraph Borders) §17.3.1.24
 - pPrChange (Revision Information for Paragraph Properties) §17.13.5.29
 - pStyle (Referenced Paragraph Style) §17.3.1.27
 - shd (Paragraph Shading) §17.3.1.31
 - snapToGrid (Use Document Grid Settings for Inter-Line Paragraph Spacing) §17.3.1.32
 - spacing (Spacing Between Lines and Above/Below Paragraph) §17.3.1.33
 - suppressAutoHyphens (Suppress Hyphenation for Paragraph) §17.3.1.34
 - suppressLineNumbers (Suppress Line Numbers for Paragraph) §17.3.1.35
 - suppressOverlap (Prevent Text Frames From Overlapping) §17.3.1.36
 - tabs (Set of Custom Tab Stops) §17.3.1.38
 - textAlignment (Vertical Character Alignment on Line) §17.3.1.39
 - textboxTightWrap (Allow Surrounding Paragraphs to Tight Wrap to Text Box Contents) §17.3.1.40
 - textDirection (Paragraph Text Flow Direction) §17.3.1.41
 - topLinePunct (Compress Punctuation at Start of a Line) §17.3.1.43
 - widowControl (Allow First/Last Line to Display on a Separate Page) §17.3.1.44
 - wordWrap (Allow Line Breaking At Character Level) §17.3.1.45

//! @todo: Handle all children
*/
KoFilter::ConversionStatus DocxXmlNumberingReader::read_pPr_numbering()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (qualifiedName() == QLatin1String("w:ind")) {
                TRY_READ(ind_numbering)
            }
            //TODO: tabs are important
            SKIP_UNKNOWN
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL abstractNumId
//! w:abstractNumId handler (Abstract Number Id)
/*!

 Parent elements:
 - [donee] num (§17.9.16)

 Child elements:
 - none
*/
KoFilter::ConversionStatus DocxXmlNumberingReader::read_abstractNumId()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(val)
    if (!val.isEmpty()) {
        m_currentAbstractId = val;
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lvlPicBulletId
//! w:lvlPicBulletID (Picture bullet id)
/*!
 Parent elements:

 Child elements:
*/
KoFilter::ConversionStatus DocxXmlNumberingReader::read_lvlPicBulletId()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(val)
    if (!val.isEmpty()) {
        m_currentBulletProperties.setPicturePath(m_picBulletPaths.value(val));
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL lvlJc
//! w:lvlJc handler (Level justification)
/*!

 Parent elements:

 Child elements:
//! @todo: Handle all children
*/
KoFilter::ConversionStatus DocxXmlNumberingReader::read_lvlJc()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(val)
    if (!val.isEmpty()) {
        m_currentBulletProperties.setAlign(val);
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL suff
//! suff (Content Between Numbering Symbol and Paragraph Text)
/*! ECMA-376, §17.9.29, p.817

 Parent elements:
 - [done] lvl (§17.9.6)
 - [done] lvl (§17.9.7)

 Child elements:
*/
KoFilter::ConversionStatus DocxXmlNumberingReader::read_suff()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(val)
    if (!val.isEmpty()) {
        m_currentBulletProperties.setFollowingChar(val);
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL ind
//! w:ind handler (Indentation)
/*!

 Parent elements:

 Child elements:

//! @todo: Handle all attributes!
*/
KoFilter::ConversionStatus DocxXmlNumberingReader::read_ind_numbering()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(left)

    bool ok = false;
    const qreal leftInd = qreal(TWIP_TO_POINT(left.toDouble(&ok)));
    if (ok) {
        m_currentBulletProperties.setMargin(leftInd);
    }

    TRY_READ_ATTR(firstLine)
    TRY_READ_ATTR(hanging)

    if (!hanging.isEmpty()) {
        const qreal firstInd = qreal(TWIP_TO_POINT(hanging.toDouble(&ok)));
        if (ok) {
           m_currentBulletProperties.setIndent(-firstInd);
        }
    }
    else if (!firstLine.isEmpty()) {
        const qreal firstInd = qreal(TWIP_TO_POINT(firstLine.toDouble(&ok)));
        if (ok) {
           m_currentBulletProperties.setIndent(firstInd);
        }
    }

    readNext();
    READ_EPILOGUE
}

// #undef CURRENT_EL
// #define CURRENT_EL rPr
//! w:rPr handler (Numbering Symbol Run Properties)
/*! ECMA-376, §17.9.25, p.812

 Parent elements:
 - [done] lvl (§17.9.6)
 - [done] lvl (§17.9.7)

 Child element NOT provided compared to rPr (§17.3.2.28):
 - del (Deleted Paragraph) §17.13.5.15
 - ins (Inserted Paragraph) §17.13.5.20
 - moveFrom (Move Source Paragraph) §17.13.5.21
 - moveTo (Move Destination Paragraph) §17.13.5.26

 Child elements:
 - b (Bold) §17.3.2.1
 - bCs (Complex Script Bold) §17.3.2.2
 - bdr (Text Border) §17.3.2.4
 - caps (Display All Characters As Capital Letters) §17.3.2.5
 - color (Run Content Color) §17.3.2.6
 - cs (Use Complex Script Formatting on Run) §17.3.2.7
 - dstrike (Double Strikethrough) §17.3.2.9
 - eastAsianLayout (East Asian Typography Settings) §17.3.2.10
 - effect (Animated Text Effect) §17.3.2.11
 - em (Emphasis Mark) §17.3.2.12
 - emboss (Embossing) §17.3.2.13
 - fitText (Manual Run Width) §17.3.2.14
 - highlight (Text Highlighting) §17.3.2.15
 - i (Italics) §17.3.2.16
 - iCs (Complex Script Italics) §17.3.2.17
 - imprint (Imprinting) §17.3.2.18
 - kern (Font Kerning) §17.3.2.19
 - lang (Languages for Run Content) §17.3.2.20
 - noProof (Do Not Check Spelling or Grammar) §17.3.2.21
 - oMath (Office Open XML Math) §17.3.2.22
 - outline (Display Character Outline) §17.3.2.23
 - position (Vertically Raised or Lowered Text) §17.3.2.24
 - rFonts (Run Fonts) §17.3.2.26
 - rPrChange (Revision Information for Run Properties) §17.13.5.31
 - rStyle (Referenced Character Style) §17.3.2.29
 - rtl (Right To Left Text) §17.3.2.30
 - shadow (Shadow) §17.3.2.31
 - shd (Run Shading) §17.3.2.32
 - smallCaps (Small Caps) §17.3.2.33
 - snapToGrid (Use Document Grid Settings For Inter-Character Spacing) §17.3.2.34
 - spacing (Character Spacing Adjustment) §17.3.2.35
 - specVanish (Paragraph Mark Is Always Hidden) §17.3.2.36
 - strike (Single Strikethrough) §17.3.2.37
 - sz (Non-Complex Script Font Size) §17.3.2.38
 - szCs (Complex Script Font Size) §17.3.2.39
 - u (Underline) §17.3.2.40
 - vanish (Hidden Text) §17.3.2.41
 - vertAlign (Subscript/Superscript Text) §17.3.2.42
 - w (Expanded/Compressed Text) §17.3.2.43
 - webHidden (Web Hidden Text) §17.3.2.44

//! @todo: Handle all children
*/
// KoFilter::ConversionStatus DocxXmlNumberingReader::read_rPr_numbering()
// {
//     READ_PROLOGUE

//     while (!atEnd()) {
//         readNext();
//         BREAK_IF_END_OF(CURRENT_EL)
//         if (isStartElement()) {
//             if (qualifiedName() == QLatin1String("w:rFonts")) {
//                 TRY_READ(rFonts_numbering)
//             }
//             else if (qualifiedName() == QLatin1String("w:color")) {
//                 TRY_READ(color_numbering)
//             }
//             SKIP_UNKNOWN
//         }
//     }

//     READ_EPILOGUE
// }

// #undef CURRENT_EL
// #define CURRENT_EL color
// //! w:color handler (bullet color)
// KoFilter::ConversionStatus DocxXmlNumberingReader::read_color_numbering()
// {
//     READ_PROLOGUE
//     const QXmlStreamAttributes attrs(attributes());

//     TRY_READ_ATTR(val)

//     if (!val.isEmpty())
//     {
//         m_currentBulletProperties.setBulletColor(QString("#").append(val));
//     }

//     readNext();
//     READ_EPILOGUE
// }

// #undef CURRENT_EL
// #define CURRENT_EL rFonts
//! w:rFonts handler (Run Fonts)
/*!

 Parent elements:

 Child elements:
//! @todo: Handle all children
*/
// KoFilter::ConversionStatus DocxXmlNumberingReader::read_rFonts_numbering()
// {
//     READ_PROLOGUE

//     const QXmlStreamAttributes attrs(attributes());

//     TRY_READ_ATTR(ascii)

//     if (!ascii.isEmpty()) {
//         m_currentBulletProperties.setBulletFont(ascii);
//     }

//     readNext();
//     READ_EPILOGUE
// }

