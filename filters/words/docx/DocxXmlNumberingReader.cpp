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
#include <MsooXmlSchemas.h>
#include <MsooXmlUtils.h>
#include <KoXmlWriter.h>
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
    kDebug() << *this << namespaceUri();
    if (!expectEl(QList<QByteArray>() << "w:numbering")) {
        return KoFilter::WrongFormat;
    }
    if (!expectNS(MSOOXML::Schemas::wordprocessingml)) {
        return KoFilter::WrongFormat;
    }
    QXmlStreamNamespaceDeclarations namespaces(namespaceDeclarations());
    /*    for (int i = 0; i < namespaces.count(); i++) {
        kDebug() << "NS prefix:" << namespaces[i].prefix() << "uri:" << namespaces[i].namespaceUri();
    }*/
    //! @todo find out whether the namespace returned by namespaceUri()
    //!       is exactly the same ref as the element of namespaceDeclarations()

    if (!namespaces.contains(QXmlStreamNamespaceDeclaration("w", MSOOXML::Schemas::wordprocessingml))) {
        raiseError(i18n("Namespace \"%1\" not found", MSOOXML::Schemas::wordprocessingml));
        return KoFilter::WrongFormat;
    }

    const QString qn(qualifiedName().toString());

    RETURN_IF_ERROR(read_numbering())

    if (!expectElEnd(qn)) {
        return KoFilter::WrongFormat;
    }
    kDebug() << "===========finished============";
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

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(ilvl)
    if (!ilvl.isEmpty()) {
        m_currentBulletProperties.m_level = ilvl.toInt() + 1;
    }

    m_bulletCharacter = QString();
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
            else if (name() == "lvlPicBulletId") {
                TRY_READ(lvlPicBulletId)
                pictureType = true;
            }
            else if (name() == "pPr") {
                TRY_READ(pPr_numbering)
            }
            else if (name() == "rPr") {
                TRY_READ(rPr_numbering)
            }
            SKIP_UNKNOWN
        }
    }

    if (!pictureType && m_bulletStyle && !m_bulletCharacter.isEmpty()) {
        m_currentBulletProperties.setBulletChar(m_bulletCharacter);
    }

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
#define CURRENT_EL rPr
//! w:rpr handler (Run Properties)
/*!

 Parent elements:

 Child elements:
//! @todo: Handle all children
*/
KoFilter::ConversionStatus DocxXmlNumberingReader::read_rPr_numbering()
{
    READ_PROLOGUE

    while (!atEnd()) {
        readNext();
        BREAK_IF_END_OF(CURRENT_EL)
        if (isStartElement()) {
            if (qualifiedName() == QLatin1String("w:rFonts")) {
                TRY_READ(rFonts_numbering)
            }
            else if (qualifiedName() == QLatin1String("w:color")) {
                TRY_READ(color_numbering)
            }
            SKIP_UNKNOWN
        }
    }

    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL pPr
//! w:ppr handler (Paragraph Properties)
/*!

 Parent elements:

 Child elements:
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
#define CURRENT_EL ind
//! w:ind handler (Indentation)
/*!

 Parent elements:

 Child elements:
//! @todo: Handle all children
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

#undef CURRENT_EL
#define CURRENT_EL color
//! w:color handler (bullet color)
KoFilter::ConversionStatus DocxXmlNumberingReader::read_color_numbering()
{
    READ_PROLOGUE
    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(val)

    if (!val.isEmpty())
    {
        m_currentBulletProperties.setBulletColor(QString("#").append(val));
    }

    readNext();
    READ_EPILOGUE
}

#undef CURRENT_EL
#define CURRENT_EL rFonts
//! w:rFonts handler (Run Fonts)
/*!

 Parent elements:

 Child elements:
//! @todo: Handle all children
*/
KoFilter::ConversionStatus DocxXmlNumberingReader::read_rFonts_numbering()
{
    READ_PROLOGUE

    const QXmlStreamAttributes attrs(attributes());

    TRY_READ_ATTR(ascii)

    if (!ascii.isEmpty()) {
        m_currentBulletProperties.setBulletFont(ascii);
    }

    readNext();
    READ_EPILOGUE
}

