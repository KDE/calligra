/* This file is part of the Calligra project
   Copyright (C) 2002 Werner Trobin <trobin@kde.org>
   Copyright (C) 2002 David Faure <faure@kde.org>
   Copyright (C) 2008 Benjamin Cail <cricketc@gmail.com>
   Copyright (C) 2009 Inge Wallin   <inge@lysator.liu.se>
   Copyright (C) 2010, 2011, 2013 Matus Uzak <matus.uzak@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "texthandler.h"
#include "conversion.h"
#include "NumberFormatParser.h"

#include <wv2/src/styles.h>
#include <wv2/src/paragraphproperties.h>
#include <wv2/src/functor.h>
#include <wv2/src/functordata.h>
#include <wv2/src/ustring.h>
#include <wv2/src/parser.h>
#include <wv2/src/fields.h>

#include <QTime>
#include <QFont>
#include <QUrl>
#include <QBuffer>
#include <QFontInfo>

#include <klocalizedstring.h>

#include <KoGenStyle.h>
#include <KoFontFace.h>

#include "document.h"
#include "msdoc.h"
#include "MsDocDebug.h"

wvWare::U8 WordsReplacementHandler::hardLineBreak()
{
    return '\n';
}

wvWare::U8 WordsReplacementHandler::nonBreakingHyphen()
{
    return '-'; // normal hyphen for now
}

wvWare::U8 WordsReplacementHandler::nonRequiredHyphen()
{
    return 0xad; // soft hyphen, according to words.dtd
}

WordsTextHandler::WordsTextHandler(wvWare::SharedPtr<wvWare::Parser> parser, KoXmlWriter* bodyWriter, KoGenStyles* mainStyles)
    : m_mainStyles(0)
    , m_document(0)
    , m_parser(parser)
    , m_sectionNumber(0)
    , m_tocNumber(0)
    , m_footNoteNumber(0)
    , m_endNoteNumber(0)
    , m_hasStoredDropCap(false)
    , m_breakBeforePage(false)
    , m_insideFootnote(false)
    , m_footnoteWriter(0)
    , m_footnoteBuffer(0)
    , m_insideAnnotation(false)
    , m_annotationWriter(0)
    , m_annotationBuffer(0)
    , m_insideDrawing(false)
    , m_drawingWriter(0)
    , m_paragraph(0)
    , m_currentTable(0)
    , m_tableWriter(0)
    , m_tableBuffer(0)
    , m_currentListLevel(-1)
    , m_currentListID(0)
    , m_fld(new fld_State())
    , m_fldStart(0)
    , m_fldEnd(0)
    , m_fldChp(0)
    , m_textBoxX(0)
    , m_textBoxY(0)
//     , m_index(0)
{
    //set the pointer to bodyWriter for writing to content.xml in office:text
    if (bodyWriter) {
        m_bodyWriter = bodyWriter;
    } else {
        warnMsDoc << "No bodyWriter!";
    }
    //for collecting most of the styles
    if (mainStyles) {
        m_mainStyles = mainStyles;
    } else {
        warnMsDoc << "No mainStyles!";
    }

    //[MS-DOC] — v20101219 - 2.7.2 DopBase
    if ((m_parser->fib().nFib <= 0x00D9) && (m_parser->dop().nfcFtnRef2 == 0)) {
        m_footNoteNumber = m_parser->dop().nFtn - 1;
    }
    //set a unique xml:id of a text:list element
    qsrand(QTime::currentTime().msec());
}

WordsTextHandler::~WordsTextHandler()
{
    delete m_fld;
}

bool WordsTextHandler::stateOk() const
{
    if (m_fldStart != m_fldEnd) {
        return false;
    }
    return true;
}

KoXmlWriter* WordsTextHandler::currentWriter() const
{
    KoXmlWriter* writer = nullptr;

    if (m_insideDrawing) {
        writer = m_drawingWriter;
    }
    else if (m_currentTable && m_currentTable->floating) {
        writer = m_tableWriter;
    }
    else if (document()->writingHeader()) {
        writer = document()->headerWriter();
    }
    else if (m_insideFootnote) {
        writer = m_footnoteWriter;
    }
    else if (m_insideAnnotation) {
        writer = m_annotationWriter;
    }
    else {
        writer = m_bodyWriter;
    }
    return writer;
}

QString WordsTextHandler::paragraphBaseFontColor() const
{
    if (!m_paragraph) return QString();

    const wvWare::StyleSheet& styles = m_parser->styleSheet();
    const wvWare::Style* ps = m_paragraph->paragraphStyle();
    quint16 istdBase = 0x0fff;
    QString color;

    while (!ps->isEmpty()) {
        if (ps->chp().cv != wvWare::Word97::cvAuto) {
            color = QString::number(ps->chp().cv | 0xff000000, 16).right(6).toUpper();
            color.prepend('#');
            break;
        }
        istdBase = ps->m_std->istdBase;
        if (istdBase == 0x0fff) {
            break;
        } else {
            ps = styles.styleByIndex(istdBase);
        }
    }
    return color;
}

//increment m_sectionNumber
void WordsTextHandler::sectionStart(wvWare::SharedPtr<const wvWare::Word97::SEP> sep)
{
    debugMsDoc ;

    m_sectionNumber++;
    m_sep = sep; //store sep for section end

    //type of the section break
    debugMsDoc << "section" << m_sectionNumber << "| sep->bkc:" << sep->bkc;

    //page layout could change
    if (sep->bkc != bkcNewColumn) {
        emit sectionFound(sep);
    }
    //check for a column break
//     if (sep->bkc == bkcNewColumn) {
//     }
    int numColumns = sep->ccolM1 + 1;

    //NOTE: We used to save the content of a section having a "continuous
    //section break" (sep-bkc == bkcContinuous) into the <text:section>
    //element.  We are now creating a <master-page> for it because the page
    //layout or header/footer content could change.
    //
    //But this way the section content is placed at a new page, which is wrong.
    //There's actually no direct support for "continuous section break" in ODF.

    //check to see if we have more than the usual one column
    if ( numColumns > 1 )
    {
        QString sectionStyleName = "Sect";
        sectionStyleName.append(QString::number(m_sectionNumber));
        KoGenStyle sectionStyle(KoGenStyle::SectionAutoStyle, "section");
        //parse column info
        QBuffer buf;
        buf.open(QIODevice::WriteOnly);
        KoXmlWriter writer(&buf);
        writer.startElement("style:columns");
        //ccolM1 = number of columns in section
        debugMsDoc << "ccolM1 = " << sep->ccolM1;
        writer.addAttribute("fo:column-count", numColumns);
        //dxaColumns = distance that will be maintained between columns
        debugMsDoc << "dxaColumns = " << sep->dxaColumns;
        writer.addAttributePt("fo:column-gap", sep->dxaColumns / 20.0);
        //check for vertical separator
        if (sep->fLBetween) {
            writer.startElement("style:column-sep");
            //NOTE: just a random default for now
            writer.addAttribute("style:width", "0.0693in");
            writer.endElement();//style:column-sep
        }
        //individual column styles
        if (numColumns > 1) {
            for (int i = 0; i < numColumns; ++i) {
                writer.startElement("style:column");
                //this should give even columns for now
                writer.addAttribute("style:rel-width", "1*");
                writer.endElement();//style:column
            }
        }
        writer.endElement();//style:columns
        QString contents = QString::fromUtf8(buf.buffer(), buf.buffer().size());
        sectionStyle.addChildElement("style:columns", contents);

//         QString footconfig("<text:notes-configuration ");
//         footconfig.append("style:num-format=\"a\"");
//         footconfig.append("/>");
//         sectionStyle.addChildElement("text:notes-configuration", footconfig);

        //add style to the collection
        sectionStyleName = m_mainStyles->insert(sectionStyle, sectionStyleName,
                                                KoGenStyles::DontAddNumberToName);
        //put necessary tags in the content
        m_bodyWriter->startElement("text:section");
        QString sectionName = "Section";
        sectionName.append(QString::number(m_sectionNumber));
        m_bodyWriter->addAttribute("text:name", sectionName);
        m_bodyWriter->addAttribute("text:style-name", sectionStyleName);
    }
    // if line numbering modulus is not 0, do line numbering
    if (sep->nLnnMod != 0) {
        if (m_mainStyles) {
            // set default line numbers style
            QString lineNumbersStyleName = QString("Standard");
            // if we have m_document
            if (m_document) {
                QString lineNumbersStyleNameTemp = m_document->lineNumbersStyleName();
                // if the style name is not empty, use it
                if (lineNumbersStyleNameTemp.isEmpty() == false) {
                    lineNumbersStyleName = lineNumbersStyleNameTemp;
                }
            }
            // prepare string for line numbering configuration
            QString cfg("<text:linenumbering-configuration text:style-name=\"%1\" "
                        "style:num-format=\"1\" text:number-position=\"left\" text:increment=\"1\"/>");
            if (!sep->lnc) {
                cfg.insert(cfg.length() - 2, " text:restart-on-page=\"true\"");
            }

            m_mainStyles->insertRawOdfStyles(KoGenStyles::DocumentStyles,
                                             cfg.arg(lineNumbersStyleName).toLatin1());

            KoGenStyle *normalStyle = m_mainStyles->styleForModification(QString("Normal"), "paragraph");

            // if got Normal style, insert line numbering configuration in it
            if (normalStyle) {
                normalStyle->addProperty(QString("text:number-lines"), QString("true"));
                normalStyle->addProperty(QString("text:line-number"), QString("0"));
            } else {
                warnMsDoc << "Could not find Normal style, numbering not added!";
            }
        }
    }
} //end sectionStart()

void WordsTextHandler::sectionEnd()
{
    debugMsDoc;

    //check for a table to be parsed and processed
    if (m_currentTable) {
        warnMsDoc << "==> WOW, unprocessed table: ignoring";
    }

    if (m_sep->bkc != bkcNewColumn) {
        emit sectionEnd(m_sep);
    }
    if (m_sep->ccolM1 > 0) {
        m_bodyWriter->endElement();//text:section
    }
}

void WordsTextHandler::pageBreak(void)
{
    m_breakBeforePage = true;
}

//signal that there's another subDoc to parse
void WordsTextHandler::headersFound(const wvWare::HeaderFunctor& parseHeaders)
{
    debugMsDoc;

    if (m_document->omittMasterPage() || m_document->useLastMasterPage()) {
        debugMsDoc << "Processing of headers/footers cancelled, master-page creation omitted.";
        return;
    }
    //NOTE: only parse headers if we're in a section that can have new headers
    //ie. new sections for columns trigger this function again, but we've
    //already parsed the headers
    if (m_sep->bkc != bkcNewColumn) {
        emit headersFound(new wvWare::HeaderFunctor(parseHeaders), 0);
    }
}


//this part puts the marker in the text, and signals for the rest to be parsed later
void WordsTextHandler::footnoteFound(wvWare::FootnoteData data,
                                     wvWare::UString characters,
                                     wvWare::SharedPtr<const wvWare::Word97::SEP> sep,
                                     wvWare::SharedPtr<const wvWare::Word97::CHP> chp,
                                     const wvWare::FootnoteFunctor& parseFootnote)
{
    Q_UNUSED(sep);

    m_insideFootnote = true;

    //create temp writer for footnote content that we'll add to m_paragraph
    m_footnoteBuffer = new QBuffer();
    m_footnoteBuffer->open(QIODevice::WriteOnly);
    m_footnoteWriter = new KoXmlWriter(m_footnoteBuffer);

    m_footnoteWriter->startElement("text:note");
    //set footnote or endnote
    m_footnoteWriter->addAttribute("text:note-class", data.type == wvWare::FootnoteData::Endnote ? "endnote" : "footnote");
    //autonumber or character
    m_footnoteWriter->startElement("text:note-citation");

    //autonumbering: 1,2,3,... for footnote; i,ii,iii,... for endnote

    //NOTE: besides converting the number to text here the format is specified
    //in section-properties -> notes-configuration too
    //
    //The character in the main document at the position specified by the
    //corresponding CP MUST be equal 0x02 and have sprmCFSpec applied with a
    //value of 1.  Let's be a bit more relaxed and follow the behavior of OOo
    //and MS Office 2007.
    if (!chp->fSpec || !(characters[0].unicode() == 2)) {
        warnMsDoc << "Warning: Trying to process a broken footnote/endnote!";
    }

    if ( data.autoNumbered ) {

        int noteNumber = (data.type == wvWare::FootnoteData::Endnote ? ++m_endNoteNumber : ++m_footNoteNumber);
        QString noteNumberString;
        char letter = 'a';
        uint ref = msonfcArabic;

        //TODO: check SEP if required

    //checking DOP - documents default
        if (data.type == wvWare::FootnoteData::Endnote) {
            ref = m_parser->dop().nfcEdnRef2;
        } else {
            ref = m_parser->dop().nfcFtnRef2;
        }
        switch (ref) {
        case msonfcArabic:
            noteNumberString = QString::number(noteNumber);
            break;
        case msonfcUCRoman:
        case msonfcLCRoman:
        {
            QString numDigitsLower[] = {"m", "cm", "d", "cd", "c", "xc", "l", "xl", "x", "ix", "v", "iv", "i" };
            QString numDigitsUpper[] = {"M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I" };
            QString *numDigits = (ref == 1 ? numDigitsUpper : numDigitsLower);
            int numValues[] = {1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1};

            for (int i = 0; i < 13; ++i) {
                while (noteNumber >= numValues[i]) {
                    noteNumber -= numValues[i];
                    noteNumberString += numDigits[i];
                }
            }
            break;
        }
        case msonfcUCLetter:
            letter = 'A';
        case msonfcLCLetter:
        {
            while (noteNumber / 25 > 0) {
                noteNumberString += QString::number(noteNumber / 25);
                noteNumber = noteNumber % 25;
                noteNumberString += QChar(letter - 1 + noteNumber / 25);
            }
            noteNumberString += QChar(letter - 1 + noteNumber);
            break;
        }
        case msonfcChiManSty:
        {
            QChar chicagoStyle[] =  {42, 8224, 8225, 167};
            int styleIndex = (noteNumber - 1) % 4;
            int repeatCount = (noteNumber - 1) / 4;
            noteNumberString = QString(chicagoStyle[styleIndex]);
            while (repeatCount > 0) {
                noteNumberString += QString(chicagoStyle[styleIndex]);
                repeatCount--;
            }
            break;
        }
        default:
            noteNumberString = QString::number(noteNumber);
            break;
        }

        m_footnoteWriter->addTextNode(noteNumberString);
    } else {
        int index = 0;
        int length = characters.length();
        QString customNote;
        while (index != length) {
            customNote.append(characters[index].unicode());
            ++index;
        }
        m_footnoteWriter->addTextNode(customNote);
    }

    m_footnoteWriter->endElement(); //text:note-citation
    //start the body of the footnote
    m_footnoteWriter->startElement("text:note-body");

    //save the state of tables/paragraphs/lists
    saveState();
    //signal Document to parse the footnote
    emit footnoteFound(new wvWare::FootnoteFunctor(parseFootnote), data.type);

//     if (listIsOpen()) {
//         closeList();
//     }
    restoreState();

    //end the elements
    m_footnoteWriter->endElement();//text:note-body
    m_footnoteWriter->endElement();//text:note

    m_insideFootnote = false;

    QString contents = QString::fromUtf8(m_footnoteBuffer->buffer(), m_footnoteBuffer->buffer().size());
    m_paragraph->addRunOfText(contents, 0, QString(""), m_parser->styleSheet());

    //cleanup
    delete m_footnoteWriter;
    m_footnoteWriter = 0;
    delete m_footnoteBuffer;
    m_footnoteBuffer = 0;

    //bool autoNumbered = (character.unicode() == 2);
    //QDomElement varElem = insertVariable( 11 /*Words code for footnotes*/, chp, "STRI" );
    //QDomElement footnoteElem = varElem.ownerDocument().createElement( "FOOTNOTE" );
    //if ( autoNumbered )
    //    footnoteElem.setAttribute( "value", 1 ); // Words will renumber anyway
    //else
    //    footnoteElem.setAttribute( "value", QString(QChar(character.unicode())) );
    //footnoteElem.setAttribute( "notetype", type == wvWare::FootnoteData::Endnote ? "endnote" : "footnote" );
    //footnoteElem.setAttribute( "numberingtype", autoNumbered ? "auto" : "manual" );
    //if ( type == wvWare::FootnoteData::Endnote )
    // Keep name in sync with Document::startFootnote
    //    footnoteElem.setAttribute( "frameset", i18n("Endnote %1", ++m_endNoteNumber ) );
    //else
    // Keep name in sync with Document::startFootnote
    //    footnoteElem.setAttribute( "frameset", i18n("Footnote %1", ++m_footNoteNumber ) );
    //varElem.appendChild( footnoteElem );
} //end footnoteFound()

void WordsTextHandler::bookmarkStart( const wvWare::BookmarkData& data )
{
    KoXmlWriter* writer;
    QBuffer buf;

    if (!m_fld->m_insideField) {
        buf.open(QIODevice::WriteOnly);
        writer = new KoXmlWriter(&buf);
    } else {
        if (!m_fld->m_afterSeparator) {
            warnMsDoc << "bookmark interferes with field instructions, omitting";
            return;
        } else {
            writer = m_fld->m_writer;
        }
    }
    //get the name of the bookmark
    QString bookmarkName;
    int nameIndex = 0;
    int nameLength = data.name.length();
    while (nameIndex != nameLength) {
        bookmarkName.append(data.name[nameIndex].unicode());
        ++nameIndex;
    }
    if ((data.limCP - data.startCP) > 0) {
        writer->startElement("text:bookmark-start");
        writer->addAttribute("text:name", bookmarkName);
        writer->endElement();
    } else {
        writer->startElement("text:bookmark");
        writer->addAttribute("text:name", bookmarkName);
        writer->endElement();
    }

    if (!m_fld->m_insideField) {
        QString content = QString::fromUtf8(buf.buffer(), buf.buffer().size());
        m_paragraph->addRunOfText(content, 0, QString(""), m_parser->styleSheet(), true);
        delete writer;
    }
}

void WordsTextHandler::bookmarkEnd( const wvWare::BookmarkData& data )
{
    KoXmlWriter* writer;
    QBuffer buf;

     if (!m_fld->m_insideField) {
        buf.open(QIODevice::WriteOnly);
        writer = new KoXmlWriter(&buf);
    } else {
        if (!m_fld->m_afterSeparator) {
            warnMsDoc << "bookmark interferes with field instructions, omitting";
            return;
        } else {
            writer = m_fld->m_writer;
        }
    }

    if ((data.limCP - data.startCP)) {
        QString bookmarkName;
        int nameIndex = 0;
        int nameLength = data.name.length();
        while (nameIndex != nameLength) {
            bookmarkName.append(data.name[nameIndex].unicode());
            ++nameIndex;
        }
        writer->startElement("text:bookmark-end");
        writer->addAttribute("text:name", bookmarkName);
        writer->endElement();
    }

    if (!m_fld->m_insideField) {
        QString content = QString::fromUtf8(buf.buffer(), buf.buffer().size());
        m_paragraph->addRunOfText(content, 0, QString(""), m_parser->styleSheet(), true);
        delete writer;
    }
}

void WordsTextHandler::annotationFound( wvWare::UString characters, wvWare::SharedPtr<const wvWare::Word97::CHP> chp,
                                        const wvWare::AnnotationFunctor& parseAnnotation)
{
    Q_UNUSED(characters);
    Q_UNUSED(chp);
    m_insideAnnotation = true;

    m_annotationBuffer = new QBuffer();
    m_annotationBuffer->open(QIODevice::WriteOnly);
    m_annotationWriter = new KoXmlWriter(m_annotationBuffer);

    m_annotationWriter->startElement("office:annotation");
    m_annotationWriter->startElement("dc:creator");
    // XXX: get the creator from the .doc
    m_annotationWriter->endElement();
    m_annotationWriter->startElement("dc:date");
    // XXX: get the date from the .doc
    m_annotationWriter->endElement();

    //save the state of tables/paragraphs/lists
    saveState();
    //signal Document to parse the annotation
    emit annotationFound(new wvWare::AnnotationFunctor(parseAnnotation), 0);

//     if (listIsOpen()) {
//         closeList();
//     }
    restoreState();

    //end the elements
    m_annotationWriter->endElement();//office:annotation

    m_insideAnnotation = false;

    QString contents = QString::fromUtf8(m_annotationBuffer->buffer(), m_annotationBuffer->buffer().size());
    m_paragraph->addRunOfText(contents, 0, QString(""), m_parser->styleSheet());

    //cleanup
    delete m_annotationWriter;
    m_annotationWriter = 0;
    delete m_annotationBuffer;
    m_annotationBuffer = 0;
}

void WordsTextHandler::tableRowFound(const wvWare::TableRowFunctor& functor, wvWare::SharedPtr<const wvWare::Word97::TAP> tap)
{
    debugMsDoc ;

    //odf doesn't support tables in annotations
    if (m_insideAnnotation) {
        return;
    }

    if (!m_currentTable) {
        static int s_tableNumber = 0;
        m_currentTable = new Words::Table();
        m_currentTable->name = i18n("Table %1", ++s_tableNumber);
        m_currentTable->tap = tap;
        //insertAnchor( m_currentTable->name );

        //check if the table is inside of an absolutely positioned frame
        if ( (tap->dxaAbs != 0) || (tap->dyaAbs != 0) ) {
            m_currentTable->floating = true;
        }
    }
//     debugMsDoc << "tap->itcMac:" << tap->itcMac << "tap->rgdxaCenter.size():" << tap->rgdxaCenter.size();

    // NOTE: Number of columns MUST be at least zero, and MUST NOT exceed 63.
    // The rgdxaCenter vector MUST contain exactly one value for every column,
    // incremented by 1.  The values in the vector MUST be in non-decreasing
    // order, [MS-DOC] — v20101219, 514/621
    if ( (tap->itcMac < 0) || (tap->itcMac > 63) ) {
        throw InvalidFormatException("Table row: INVALID num. of columns!");
    }
    if ( tap->rgdxaCenter.empty() ||
         (tap->rgdxaCenter.size() != (quint16)(tap->itcMac + 1)) )
    {
        throw InvalidFormatException("Table row: tap->rgdxaCenter.size() INVALID!");
    }
    for (uint i = 0; i < (quint16) tap->itcMac; i++) {
        if (tap->rgdxaCenter[i] > tap->rgdxaCenter[i + 1]) {
            warnMsDoc << "Warning: tap->rgdxaCenter INVALID, tablehandler will try to fix!";
            break;
        }
    }
    // Add all cell edges to an array where tablehandler will keep them sorted.
    for (int i = 0; i <= tap->itcMac; i++) {
        m_currentTable->cacheCellEdge(tap->rgdxaCenter[ i ]);
    }
    Words::Row row(new wvWare::TableRowFunctor(functor), tap);
    m_currentTable->rows.append(row);
}

void WordsTextHandler::tableEndFound()
{
    debugMsDoc ;

    //odf doesn't support tables in annotations
    if (m_insideAnnotation) {
        return;
    }
    if (!m_currentTable) {
        warnMsDoc << "Looks like we lost a table somewhere: return";
        return;
    }
//     if (listIsOpen()) {
//         //debugMsDoc << "closing list " << m_currentListID;
//         closeList();
//     }
    bool floating = m_currentTable->floating;

    if (floating) {
        m_tableBuffer = new QBuffer();
        m_tableBuffer->open(QIODevice::WriteOnly);
        m_tableWriter = new KoXmlWriter(m_tableBuffer);
    }

    //must delete table in Document!
    emit tableFound(m_currentTable);
    m_currentTable = 0L;

    if (floating) {
        m_floatingTable = QString::fromUtf8(m_tableBuffer->buffer(), m_tableBuffer->buffer().size());
        delete m_tableWriter;
        m_tableWriter = 0;
        delete m_tableBuffer;
        m_tableBuffer = 0;
    }
}

void WordsTextHandler::msodrawObjectFound(const unsigned int globalCP, const wvWare::PictureData* data)
{
    debugMsDoc;
    //inline object should be inside of a pragraph
    Q_ASSERT(m_paragraph);

    //ignore if field instructions are processed
    if (m_fld->m_insideField && !m_fld->m_afterSeparator) {
        warnMsDoc << "Warning: Object located in field instructions, Ignoring!";
        return;
    }

    //save the state of tables/paragraphs/lists (text-box)
    saveState();

    //Create temporary writer for the picture tags.
    KoXmlWriter* writer = 0;
    QBuffer buf;

    buf.open(QIODevice::WriteOnly);
    writer = new KoXmlWriter(&buf);
    m_drawingWriter = writer;
    m_insideDrawing = true;

    //frame or drawing shape acting as a hyperlink
    if (m_fld->m_hyperLinkActive) {
        writer->startElement("draw:a");
        writer->addAttribute("xlink:type", "simple");
        writer->addAttribute("xlink:href", QUrl(m_fld->m_hyperLinkUrl).toEncoded());
    }

    if (data) {
        emit inlineObjectFound(*data, writer);
    } else {
        emit floatingObjectFound(globalCP, writer);
    }

//     if (listIsOpen()) {
//         closeList();
//     }
    if (m_fld->m_hyperLinkActive) {
        writer->endElement();
        m_fld->m_hyperLinkActive = false;
    }
    //cleanup
    delete m_drawingWriter;
    m_drawingWriter = 0;
    m_insideDrawing = false;

    //restore the state
    restoreState();

    //now add content to our current paragraph
    QString contents = QString::fromUtf8(buf.buffer(), buf.buffer().size());
    m_paragraph->addRunOfText(contents, 0, QString(""), m_parser->styleSheet(), true);
}

// Sets m_currentStyle with PAP->istd (index to STSH structure)
void WordsTextHandler::paragraphStart(wvWare::SharedPtr<const wvWare::ParagraphProperties> paragraphProperties, wvWare::SharedPtr<const wvWare::Word97::CHP> chp)
{
    debugMsDoc << "**********************************************";

    //TODO: what has to be done in this situation
    Q_ASSERT(paragraphProperties);
    m_currentPPs = paragraphProperties;

    //check for a table to be parsed and processed
//     if (m_currentTable) {
//         warnMsDoc << "==> WOW, unprocessed table: ignoring";
//     }
    //set correct writer and style location
    KoXmlWriter* writer = currentWriter();
    bool inStylesDotXml = document()->writingHeader();

    const wvWare::StyleSheet& styles = m_parser->styleSheet();
    const wvWare::Style* paragraphStyle = 0;

    // Check list information, because that's bigger than a paragraph, and
    // we'll track that here in the TextHandler.
    //
    //TODO: <text:numbered-paragraph>

    quint16 istd = paragraphProperties->pap().istd;

    //Check for a named style for this paragraph.
    paragraphStyle = styles.styleByIndex(istd);
    if (!paragraphStyle) {
        paragraphStyle = styles.styleByID(stiNormal);
        debugMsDoc << "Invalid reference to paragraph style, reusing Normal";
    }
    Q_ASSERT(paragraphStyle);

    //headings related logic
    uint outlineLevel = 0;
    bool isHeading = false;

    //An unsigned integer that specifies the istd of the parent style from
    //which this style inherits in the style inheritance tree, or 0x0FFF if
    //this style does not inherit from any other style.
    quint16 istdBase = 0x0fff;

    //Applying a heading style => paragraph is a heading.  MS-DOC outline level
    //is ZERO based, whereas ODF has ONE based.
    if ( (istd >= 0x1) && (istd <= 0x9) ) {
        isHeading = true;
        outlineLevel = istd;
    } else {
        const wvWare::Style* ps = paragraphStyle;
        while (!ps->isEmpty()) {
            istdBase = ps->m_std->istdBase;
//             debugMsDoc << "istd:" << hex << istd << "| istdBase:" << istdBase;
            if (istdBase == 0x0fff) {
                break;
            } else if ( (istdBase >= 0x1) && (istdBase <= 0x9) ) {
                isHeading = true;
                outlineLevel = istdBase;
                break;
            } else {
                ps = styles.styleByIndex(istdBase);
            }
        }
    }

    //Lists related logic
    qint16 ilfo = paragraphProperties->pap().ilfo;

    // At the moment each list-item is saved into a separate text:list to make
    // automatic numbering based on text:continue-list work properly.
//     if (ilfo == 0) {
//         //This paragraph is not in a list.
//         if (listIsOpen()) {
//             //debugMsDoc << "closing list " << m_currentListID;
//             closeList();
//         }
//     }
    if (ilfo > 0) {
        // We're in a list in the word document.
        //
        // At the moment <text:numbered-paragraph> is not supported, we process
        // the paragraph as an list-item instead.
        debugMsDoc << "Paragraph in a list or a numbered paragraph";

        // listInfo is our list properties object.
        const wvWare::ListInfo* listInfo = paragraphProperties->listInfo();

        //error (or currently unknown case)
        if (!listInfo) {
            warnMsDoc << "pap.ilfo is non-zero but there's no listInfo!";
            // Try to make it a heading for now.
            outlineLevel = paragraphProperties->pap().ilvl + 1;
            isHeading = true;
        } else if (listInfo->lsid() == 1 && listInfo->numberFormat() == 255) {
            debugMsDoc << "Found a heading, pap().ilvl=" << paragraphProperties->pap().ilvl;
            outlineLevel = paragraphProperties->pap().ilvl + 1;
            isHeading = true;
        } else {
            // List processing
            // This takes care of all the cases:
            //  - A new list
            //  - A list with higher level than before
            //  - A list with lower level than before
            writeListInfo(writer, paragraphProperties->pap(), listInfo);
        }
    } else if (ilfo < 0) {
        //TODO: support required
        debugMsDoc << "Unable to determine which list contains the paragraph";
    }

    // Now that the bookkeeping is taken care of for old paragraphs,
    // then actually create the new one.
    debugMsDoc << "create new Paragraph";
    m_paragraph = new Paragraph(m_mainStyles, m_document->currentBgColor(), inStylesDotXml, isHeading,
                                m_document->writingHeader(), outlineLevel);

    //set current named style in m_paragraph
    m_paragraph->setParagraphStyle(paragraphStyle);
    //set paragraph and character properties of the paragraph
    m_paragraph->setParagraphProperties(paragraphProperties);
    m_paragraph->setCharacterProperties(chp);

    KoGenStyle* style = m_paragraph->koGenStyle();

    //check if the master-page-name attribute is required
    if (document()->writeMasterPageName() && !document()->writingHeader())
    {
        style->addAttribute("style:master-page-name", document()->masterPageName());
        document()->set_writeMasterPageName(false);
    }
    //check if the break-before property is required
    if ( m_breakBeforePage &&
         !document()->writingHeader() &&
         !paragraphProperties->pap().fInTable )
    {
        style->addProperty("fo:break-before", "page", KoGenStyle::ParagraphType);
        m_breakBeforePage = false;
    }

    //insert the floating table at the beginning
    if (!m_floatingTable.isEmpty()) {
        m_paragraph->addRunOfText(m_floatingTable, 0, QString(""), m_parser->styleSheet());
        m_floatingTable.clear();
    }

} //end paragraphStart()

void WordsTextHandler::paragraphEnd()
{
    debugMsDoc << "-----------------------------------------------";

    bool chck_dropcaps = false;

    // If the last paragraph was a drop cap paragraph, combine with this one.
    if (m_hasStoredDropCap) {
        debugMsDoc << "combine paragraphs for drop cap" << m_dropCapString;
        m_paragraph->addDropCap(m_dropCapString, m_dcs_fdct, m_dcs_lines, m_dropCapDistance, m_dropCapStyleName);
    }

    //write some debug messages
    if (m_insideFootnote) {
        debugMsDoc << "writing a footnote";
    } else if (m_insideAnnotation) {
        debugMsDoc << "writing an annotation";
    } else if (m_insideDrawing) {
        debugMsDoc << "writing a drawing";
    } else if (!document()->writingHeader()) {
        debugMsDoc << "writing to body";
        chck_dropcaps = true;
    } else {
        debugMsDoc << "writing a header/footer";
    }

    //set correct writer and style location
    KoXmlWriter* writer = currentWriter();

    //add nested field snippets to this paragraph
    if (m_fld->m_insideField) {

        // text:p allowed as child element of text:index-body
        if ( !m_fld_snippets.isEmpty() || m_fld->m_type == TOC ) {
            writer = m_fld->m_writer;
        }
        while (!m_fld_snippets.isEmpty()) {
            m_paragraph->addRunOfText(m_fld_snippets.takeFirst(), m_fldChp, QString(""),
                                      m_parser->styleSheet(), true);
        }
    }

    bool openTextBox = false;
    if (m_currentPPs->pap().dxaAbs !=  m_textBoxX || m_currentPPs->pap().dyaAbs != m_textBoxY) {
        if ((m_textBoxX || m_textBoxY)) {
            m_textBoxX = 0;
            m_textBoxY = 0;
            // close the previous text box before writing a new one
            writer->endElement(); //draw:text-box
            writer->endElement(); //draw:frame
            writer->endElement(); //text:p
        }

        if (!m_currentPPs->pap().fInTable &&
            (m_currentPPs->pap().dxaAbs != 0 || m_currentPPs->pap().dyaAbs != 0)) {
            m_textBoxX = m_currentPPs->pap().dxaAbs;
            m_textBoxY = m_currentPPs->pap().dyaAbs;
            openTextBox = true;
        }
    }

    //write paragraph content, reuse text/paragraph style name if applicable
    QString styleName = m_paragraph->writeToFile(writer, openTextBox, &m_fld->m_tabLeader);

    //provide the styleName to the current field
    m_fld->m_styleName = styleName;

    if (chck_dropcaps) {
        // If this paragraph is a drop cap, it should be combined with the next
        // paragraph.  So store the drop cap data for future use.  However,
        // only do this if the last paragraph was *not* a dropcap.
        if (!m_hasStoredDropCap && m_paragraph->dropCapStatus() == Paragraph::IsDropCapPara) {
            m_paragraph->getDropCapData(&m_dropCapString, &m_dcs_fdct, &m_dcs_lines,
                                        &m_dropCapDistance, &m_dropCapStyleName);
            m_hasStoredDropCap = true;
            debugMsDoc << "saving drop cap data in texthandler" << m_dropCapString;
        }
        else {
            // Remove the traces of the drop cap for the next round.
            m_hasStoredDropCap = false;
            m_dropCapString.clear();
        }
    }

    // At the moment each list-item is saved into a separate text:list to make
    // automatic numbering based on text:continue-list work properly.
    if (listIsOpen()) {
        closeList();
    }

    //save the font color
    m_paragraphBaseFontColorBkp = paragraphBaseFontColor();

    delete m_paragraph;
    m_paragraph = 0;
}//end paragraphEnd()

void WordsTextHandler::fieldStart(const wvWare::FLD* fld, wvWare::SharedPtr<const wvWare::Word97::CHP> /*chp*/)
{
    //NOTE: The content between fieldStart and fieldSeparator represents field
    //instructions and the content between fieldSeparator and fieldEnd
    //represents the field RESULT [optional].  In most cases the field RESULT
    //stores the complete information (instruction are applied by msword).
    debugMsDoc << "fld->flt:" << fld->flt << "( 0x" << hex << fld->flt << ")";

    //nested field
    if (m_fld->m_insideField) {
        fld_saveState();
    } else {
        delete m_fld;
    }

    m_fld = new fld_State((fldType)fld->flt);
    m_fld->m_insideField = true;
    m_fldStart++;

    switch (m_fld->m_type) {
    case EQ:
        debugMsDoc << "processing field... EQ (Combined Characters)";
        break;
    case REF:
    case CREATEDATE:
    case DATE:
    case HYPERLINK:
    case PAGEREF:
    case SAVEDATE:
    case TIME:
    case TOC:
        debugMsDoc << "Processing only a subset of field instructions!";
        debugMsDoc << "Processing field result.";
        break;
    case LAST_REVISED_BY:
    case NUMPAGES:
    case PAGE:
    case SUBJECT:
    case TITLE:
        warnMsDoc << "Warning: field instructions not supported, storing as ODF field!";
        warnMsDoc << "Warning: ignoring field result!";
        break;
    case MACROBUTTON:
    case SYMBOL:
        warnMsDoc << "Warning: processing only a subset of field instructions!";
        warnMsDoc << "Warning: ignoring field result!";
        break;
    case AUTHOR:
    case AUTOTEXTLIST:
    case EDITTIME:
    case FILENAME:
    case MERGEFIELD:
    case REF_WITHOUT_KEYWORD:
    case SEQ:
    case SHAPE:
        warnMsDoc << "Warning: field instructions not supported!";
        warnMsDoc << "Warning: processing only field result!";
        break;
    case UNSUPPORTED:
        warnMsDoc << "Warning: Fld data missing, ignoring!";
    default:
        warnMsDoc << "Warning: unrecognized field type, ignoring!";
        m_fld->m_type = UNSUPPORTED;
        break;
    }

    switch (m_fld->m_type) {
    case NUMPAGES:
    case PAGE:
        m_paragraph->setContainsPageNumberField(true);
        break;
    case TOC:
        m_tocNumber++;
        break;
    default:
        break;
    }
}//end fieldStart()

void WordsTextHandler::fieldSeparator(const wvWare::FLD* /*fld*/, wvWare::SharedPtr<const wvWare::Word97::CHP> /*chp*/)
{
    debugMsDoc ;
    m_fld->m_afterSeparator = true;
    QString* inst = &m_fld->m_instructions;

    //process field instructions if required
    switch (m_fld->m_type) {
    case HYPERLINK:
    {
        // Syntax: HYPERLINK field-argument [ switches ]
        //
        // When selected, causes control to jump to the location specified by
        // text in field-argument.  That location can be a bookmark or a URL.
        //
        // Field Value: None
        //
        // TODO: add support for all switches
        //
        // \o field-argument - Text in this switch's field-argument specifies
        // the ScreenTip text for the hyperlink.  field-argument which
        // specifies a location in the file, such as bookmark, where this
        // hyperlink will jump.
        //
        // \t field-argument - Text in this switch's field-argument specifies
        // the target to which the link should be redirected.  Use this switch
        // to link from a frames page to a page that you want to appear outside
        // of the frames page.  The permitted values for text are: _top, whole
        // page (the default); _self, same frame; _blank, new window; _parent,
        // parent frame
        //
        // \m - Appends coordinates to a hyperlink for a server-side image map.
        // \n - Causes the destination site to be opened in a new window.
        //
        // \l field-argument - Text in this switch's field-argument specifies a
        // location in the file, such as a bookmark, where to jump.
        //
        QRegExp rx("\\s\\\\l\\s\"(\\S+)\"");
        if (rx.indexIn(*inst) >= 0) {
            // prevent invalid URI
            if (rx.cap(1) != "#") {
                m_fld->m_hyperLinkUrl = rx.cap(1).prepend("#");
            }
        }
        rx = QRegExp("HYPERLINK\\s\"(\\S+)\"");
        if (rx.indexIn(*inst) >= 0) {
            m_fld->m_hyperLinkUrl.prepend(rx.cap(1));
        }

        m_fld->m_hyperLinkActive = true;
        break;
    }
    case REF:
    {
        // Syntax: REF Bookmark [ switches ]
        //
        // TODO: add support for all switches
        //
        // \h - Creates a hyperlink to the bookmarked paragraph.
        //
        QRegExp rx("REF\\s(\\S+)");
        if (rx.indexIn(*inst) >= 0) {
            m_fld->m_hyperLinkUrl = rx.cap(1);
        }
        rx = QRegExp("\\s\\\\h\\s");
        if (rx.indexIn(*inst) >= 0) {
            m_fld->m_hyperLinkActive = true;
            m_fld->m_hyperLinkUrl.prepend("#");
        }
        m_fld->m_refFormat = "text";
        break;
    }
    case PAGEREF:
    {
        // Syntax: PAGEREF field-argument [ switches ]
        //
        // Inserts the number of the page containing the bookmark specified by
        // text in field-argument for a cross-reference.
        //
        // Field Value: The number of the page containing the bookmark.
        //
        // TODO: add support for all switches
        //
        // \p - Causes the field to display its position relative to the source
        // bookmark.  If the PAGEREF field is on the same page as the bookmark,
        // it omits "on page #" and returns "above" or "below" only.  If the
        // PAGEREF field is not on the same page as the bookmark, the string
        // "on page #" is used.
        //
        // \h - Creates a hyperlink to the bookmarked paragraph.
        //
        QRegExp rx("PAGEREF\\s(\\S+)");
        if (rx.indexIn(*inst) >= 0) {
            m_fld->m_hyperLinkUrl = rx.cap(1);
        }
        rx = QRegExp("\\s\\\\h\\s");
        if (rx.indexIn(*inst) >= 0) {
            m_fld->m_hyperLinkActive = true;
            m_fld->m_hyperLinkUrl.prepend("#");
        }
        m_fld->m_refFormat = "page";
        break;
    }
    case TIME:
    case DATE:
    {
        // Extract the interesting format-string. That means we translate
        // something like 'TIME \@ "MMMM d, yyyy"' into 'MMMM d, yyyy' cause
        // the NumberFormatParser doesn't handle it correct else.
        //
        QRegExp rx(".*\"(.*)\".*");
        if (rx.indexIn(*inst) >= 0)
            m_fld->m_instructions = rx.cap(1);
        break;
    }
    default:
        break;
    }
} //end fieldSeparator()

/**
 * Fields which are supported by inline variables can be dealt with by emitting
 * the necessary markup here. For example:
 *
 *    case LAST_REVISED_BY:
 *        writer.startElement("text:creator");
 *        writer.endElement();
 *        break;
 *
 * However, fields which do not enjoy such support are dealt with by emitting
 * the "result" text generated by Word as vanilla text in @ref runOftext.
 */
void WordsTextHandler::fieldEnd(const wvWare::FLD* fld, wvWare::SharedPtr<const wvWare::Word97::CHP> chp)
{
    Q_UNUSED(fld);

//     debugMsDoc << "fDiffer:" << fld->flags.fDiffer <<
//     "fZombieEmbed:" << fld->flags.fZombieEmbed <<
//     "fResultDirty:" << fld->flags.fResultDirty <<
//     "fResultEdited:" << fld->flags.fResultEdited <<
//     "fLocked:" << fld->flags.fLocked <<
//     "fPrivateResult:" << fld->flags.fPrivateResult <<
//     "fNested:" << fld->flags.fNested <<
//     "fHasSep:"<< fld->flags.fHasSep;

    if (!m_fld->m_insideField) {
        debugMsDoc << "End of a broken field detected!";
        return;
    }

    QBuffer buf;
    buf.open(QIODevice::WriteOnly);
    KoXmlWriter writer(&buf);
    QString* inst = &m_fld->m_instructions;
    QString tmp;

    switch (m_fld->m_type) {
    case EQ:
    {
        //TODO: nested fields support required
        //NOTE: actually combined characters stored as 'equation'
        QRegExp rx("eq \\\\o\\(\\\\s\\\\up 36\\(([^\\)]*)\\),\\\\s\\\\do 12\\(([^\\)]*)\\)\\)");
        int where = rx.indexIn(*inst);

        if (where != -1) {
            QString cc = rx.cap(1) + rx.cap(2);
            if (!cc.isEmpty()) {
                m_paragraph->setCombinedCharacters(true);
                m_paragraph->addRunOfText(cc, chp, QString(""), m_parser->styleSheet());
                m_paragraph->setCombinedCharacters(false);
            }
        }
        break;
    }
    case HYPERLINK:
    {
        if (m_fld->m_hyperLinkActive) {
            writer.startElement("text:a", false);
            writer.addAttribute("xlink:type", "simple");
            writer.addAttribute("xlink:href", QUrl(m_fld->m_hyperLinkUrl).toEncoded());
            writer.addCompleteElement(m_fld->m_buffer);
            writer.endElement(); //text:a
        }
        //else a frame or drawing shape acting as a hyperlink already processed
        break;
    }
    case LAST_REVISED_BY:
        writer.startElement("text:creator");
        writer.endElement();
        break;
    case MACROBUTTON:
    {
        QRegExp rx("MACROBUTTON\\s\\s?\\w+\\s\\s?(.+)$");
        if (rx.indexIn(*inst) >= 0) {
            writer.addTextNode(rx.cap(1));
        }
        break;
    }
    case NUMPAGES:
        writer.startElement("text:page-count");
        writer.endElement();
        break;
    case PAGE:
        writer.startElement("text:page-number");
        writer.addAttribute("text:select-page", "current");
        writer.endElement();
        break;
    case REF:
    case PAGEREF:
    {
        if (m_fld->m_hyperLinkActive) {
            writer.startElement("text:a", false);
            writer.addAttribute("xlink:type", "simple");
            writer.addAttribute("xlink:href", QUrl(m_fld->m_hyperLinkUrl).toEncoded());
            writer.addCompleteElement(m_fld->m_buffer);
            writer.endElement(); //text:a
        } else {
            writer.startElement("text:bookmark-ref");
            writer.addAttribute("text:reference-format", m_fld->m_refFormat);
            writer.addAttribute("text:ref-name", QUrl(m_fld->m_hyperLinkUrl).toEncoded());
            writer.addCompleteElement(m_fld->m_buffer);
            writer.endElement(); //text:bookmark-ref
        }
        break;
    }
    case SUBJECT:
        writer.startElement("text:subject");
        writer.endElement();
        break;
    case DATE:
    {
        writer.startElement("text:date");

        KoGenStyle style = NumberFormatParser::parse(m_fld->m_instructions, m_mainStyles, KoGenStyle::NumericDateStyle);
        writer.addAttribute("style:data-style-name", m_mainStyles->insert(style, "N"));

        //writer.addAttribute("text:fixed", "true");
        //writer.addAttribute("text:date-value", "2011-01-14T12:38:31.99");
        writer.addCompleteElement(m_fld->m_buffer); // January 14, 2011
        writer.endElement(); //text:date
        break;
    }
    case TIME:
    {
        writer.startElement("text:time");

        KoGenStyle style = NumberFormatParser::parse(m_fld->m_instructions, m_mainStyles, KoGenStyle::NumericTimeStyle);
        writer.addAttribute("style:data-style-name", m_mainStyles->insert(style, "N"));

        //writer.addAttribute("text:fixed", "true");
        //writer.addAttribute("text:time-value", );
        writer.addCompleteElement(m_fld->m_buffer);
        writer.endElement(); //text:time
        break;
    }
    case CREATEDATE:
    {
        writer.startElement("text:creation-date");
        KoGenStyle style = NumberFormatParser::parse(m_fld->m_instructions, m_mainStyles, KoGenStyle::NumericTimeStyle);
        writer.addAttribute("style:data-style-name", m_mainStyles->insert(style, "N"));
        writer.addCompleteElement(m_fld->m_buffer);
        writer.endElement(); //text:creation-date
        break;
    }
    case SAVEDATE:
    {
        writer.startElement("text:modification-date");
        KoGenStyle style = NumberFormatParser::parse(m_fld->m_instructions, m_mainStyles, KoGenStyle::NumericTimeStyle);
        writer.addAttribute("style:data-style-name", m_mainStyles->insert(style, "N"));
        writer.addCompleteElement(m_fld->m_buffer);
        writer.endElement(); //text:modification-date
        break;
    }
    case SYMBOL:
    {
        QRegExp rx_txt("SYMBOL\\s{2}(\\S+)\\s+.+$");
        QString txt;
        *inst = inst->trimmed();

        //check for text in field instructions
        if (rx_txt.indexIn(*inst) >= 0) {
            txt = rx_txt.cap(1);

            //ascii code
            if (inst->contains("\\a")) {
                QRegExp rx16("0\\D.+");
                bool ok = false;
                int n;

                if (rx16.indexIn(txt) >= 0) {
                    n = txt.toInt(&ok, 16);
                } else {
                    n = txt.toInt(&ok, 10);
                }
                if (ok) {
                    tmp.append((char) n);
                }
            }
            //unicode
            if (inst->contains("\\u")) {
                debugMsDoc << "Warning: unicode symbols not supported!";
            }
        }
        //default value (check the corresponding test)
        if (tmp.isEmpty()) {
            tmp = "###";
        }
        writer.addTextNode(tmp);
        break;
    }
    case TITLE:
        writer.startElement("text:title");
        writer.endElement();
        break;
    case TOC:
    {
        // Syntax: TOC [ switches ]
        //
        // Field Value: The table of contents.
        //
        // TODO:
        //
        // \a field-argument - Includes captioned items, but omits caption
        // labels and numbers.  The identifier designated by text in this
        // switch's field-argument corresponds to the caption label.
        //
        // \b field-argument - Includes entries only from the portion of the
        // document marked by the bookmark named by text in this switch's
        // field-argument.
        //
        // \c field-argument - Includes figures, tables, charts, and other
        // items that are numbered by a SEQ field.  The sequence identifier
        // designated by text in this switch's field-argument, which
        // corresponds to the caption label, shall match the identifier in the
        // corresponding SEQ field.
        //
        // \d field-argument - When used with \s, the text in this switch's
        // field-argument defines the separator between sequence and page
        // numbers.  The default separator is a hyphen (-).
        //
        // \f field-argument - Includes only those TC fields whose identifier
        // exactly matches the text in this switch's field-argument (which is
        // typically a letter).
        //
        // \l field-argument - Includes TC fields that assign entries to one of
        // the levels specified by text in this switch's field-argument as a
        // range having the form startLevel-endLevel, where startLevel and
        // endLevel are integers, and startLevel has a value equal-to or
        // less-than endLevel.  TC fields that assign entries to lower levels
        // are skipped.
        //
        // \s field-argument - For entries numbered with a SEQ field , adds a
        // prefix to the page number.  The prefix depends on the type of entry.
        // Text in this switch's field-argument shall match the identifier in
        // the SEQ field.
        //
        // \u - Uses the applied paragraph outline level.
        // \w - Preserves tab entries within table entries.
        // \x - Preserves newline characters within table entries.
        // \z - Hides tab leader and page numbers in Web layout view.

        //NOTE: Nested fields had been processed and wrote into m_fld->m_writer
        //by the writeToFile function.  The m_fldStates stack should be empty.
        Q_ASSERT(m_fldStates.empty());

        /*
         * ************************************************
         * process switches
         * ************************************************
         */
        // \h - Makes the table of contents entries hyperlinks.
        QRegExp rx = QRegExp("\\s\\\\h\\s");
        QString hlinkStyleName;
        bool hyperlink = false;

        if (rx.indexIn(*inst) >= 0) {
            hyperlink = true;
        }
        // Hyperlink style info is not provided by the TOC field, reusing the
        // text style of text:index-body content.
        if (m_fld->m_styleName.contains('T')) {
            hlinkStyleName = m_mainStyles->style(m_fld->m_styleName, "text")->parentName();
        } else {
            debugMsDoc << "TOC: Missing text style to format the hyperlink!";
        }

        // \n field-argument - Without field-argument, omits page numbers from
        // the table of contents.  Page numbers are omitted from all levels
        // unless a range of entry levels is specified by text in this switch's
        // field-argument.  A range is specified as for \l.
        rx = QRegExp("\\s\\\\n\\s");
        bool pgnum = true;

        if (rx.indexIn(*inst) >= 0) {
            pgnum = false;
        }

        // \o field-argument - Uses paragraphs formatted with all or the
        // specified range of built-in heading styles.  Headings in a style
        // range are specified by text in field-argument.  If no heading range
        // is specified, all heading levels used in the document are listed.
        rx = QRegExp("\\s\\\\o\\s\"(\\S+)\"");
        int levels = 0;

        if (rx.indexIn(*inst) >= 0) {
            QStringList levels_lst = rx.cap(1).split('-');
            levels = levels_lst.last().toInt();
        }

        // \p field-argument - text in this switch's field-argument specifies a
        // sequence of characters that separate an entry and its page number.
        // The default is a tab with leader dots.
        rx = QRegExp("\\s\\\\p\\s\"(\\s)\"");
        QString separator;

        if (rx.indexIn(*inst) >= 0) {
            separator = rx.cap(1);
        } else {
            // The tab leader character is not provided by the TOC field,
            // reusing from the last paragraph style applied in index-body.
            // If not provided, use the default value.
            if (m_fld->m_tabLeader.isNull()) {
                m_fld->m_tabLeader = QChar('.');
            }
        }

        // \t field-argument - Uses paragraphs formatted with styles other than
        // the built-in heading styles.  Text in this switch's field-argument
        // specifies those styles as a set of comma-separated doublets, with
        // each doublet being a comma-separated set of style name and table of
        // content level.  \t can be combined with \o.
        rx = QRegExp("\\s\\\\t\\s\"(.+)\"");
        QMap<QString, int> customStyles;
        bool useOutlineLevel = true;

        if (rx.indexIn(*inst) >= 0) {
            // Most of the files contain semicolons instead of commas.
            QStringList fragments = rx.cap(1).split(QRegExp(";"));
            if (fragments.size() % 2) {
                fragments = rx.cap(1).split(QRegExp(","));
            }
            if (!(fragments.size() % 2)) {
                bool ok;
                for (int n, i = 0 ; i < fragments.size(); i += 2) {
                    n = fragments[i + 1].toInt(&ok);
                    if (!ok) {
                        continue;
                    }
                    else if (levels < n) {
                        levels = n;
                    }
                    customStyles.insert(Conversion::processStyleName(fragments[i]), n);
                }
                useOutlineLevel = false;
            }
        }
        /*
         * ************************************************
         * post-processing
         * ************************************************
         */
        QStringList styleNames = document()->tocStyleNames();

        //Set levels in case any of the switches in {\t,\o} were not present.
        if (levels == 0) {
            levels = styleNames.size();
        }
        //post-process m_tocStyleNames
        if (styleNames.size() < levels) {
            if (styleNames.isEmpty()) {
                for (int i = 0; i < levels; i++) {
                    styleNames.append("Normal");
                }
            } else {
                rx = QRegExp("\\S+(\\d+)$");
                if (rx.indexIn(styleNames.first()) >= 0) {
                    bool ok = false;
                    uint n = rx.cap(1).toUInt(&ok, 10);
                    if (!ok) {
                        debugMsDoc << "Conversion of QString to uint failed!";
                    } else {
                        for (uint i = 1; i < n; i++) {
                            styleNames.prepend("Normal");
                        }
                    }
                } else {
                    debugMsDoc << "Missing TOC related style with level info.";
                    debugMsDoc << "Not that bad actually.";
                }
                while (styleNames.size() < levels) {
                    styleNames.append("Normal");
                }
            }
        }
        //TODO: re-order m_tocStyleNames based on the outline level

        /*
         * ************************************************
         * table-of-content
         * ************************************************
         */
        //NOTE: TOC content is not encapsulated in a paragraph
        KoXmlWriter* cwriter = currentWriter();
        cwriter->startElement("text:table-of-content");
        //text:name
        cwriter->addAttribute("text:name", QString().setNum(m_tocNumber).prepend("_TOC"));
        //text:protected
        //text:protection-key
        //text:protection-key-digest-algorithm
        //text:style-name
        //<text:table-of-content-source>
        cwriter->startElement("text:table-of-content-source");
        cwriter->addAttribute("text:index-scope", "document");
        if (levels > 0) {
            cwriter->addAttribute("text:outline-level", levels);
        }
        cwriter->addAttribute("text:relative-tab-stop-position", "false");
        cwriter->addAttribute("text:use-index-marks", "false");
        cwriter->addAttribute("text:use-index-source-styles", !useOutlineLevel);
        cwriter->addAttribute("text:use-outline-level", useOutlineLevel);
        //|-- <text:index-source-styles>
        if (!useOutlineLevel) {
            QStringList styles;
            for (int i = 1; i <= levels; i++) {
                styles = customStyles.keys(i);
                if (styles.isEmpty()) {
                    continue;
                }
                cwriter->startElement("text:index-source-styles");
                cwriter->addAttribute("text:outline-level", i);
                for (int j = 0; j < styles.size(); j++) {
                    cwriter->startElement("text:index-source-style");
                    cwriter->addAttribute("text:style-name", styles[j].toUtf8());
                    cwriter->endElement(); //text:index-source-style
                }
                cwriter->endElement(); //text:index-source-styles
            }
        }
        //|-- <text:index-title-template>
        int n = styleNames.indexOf(QRegExp("\\S+Heading$"));
        if (n != -1) {
            cwriter->startElement("text:index-title-template");
            cwriter->addAttribute("text:style-name", styleNames[n]);
            cwriter->endElement(); //text:index-title-template
        }
        //|-- <text:table-of-content-entry-template>
        for (int i = 0; i < levels; i++) {
            cwriter->startElement("text:table-of-content-entry-template");
            cwriter->addAttribute("text:outline-level", i + 1);
            cwriter->addAttribute("text:style-name", styleNames[i].toUtf8());

            if (hyperlink) {
                cwriter->startElement("text:index-entry-link-start");
                if (!hlinkStyleName.isEmpty()) {
                    cwriter->addAttribute("text:style-name", hlinkStyleName);
                }
                cwriter->endElement(); //text:index-entry-link-start
            }
            //Represents the chapter number where an index entry is located.
            //FIXME: we need some logic here!
            cwriter->startElement("text:index-entry-chapter");
            cwriter->endElement(); //text:index-entry-chapter

            cwriter->startElement("text:index-entry-text");
            cwriter->endElement(); //text:index-entry-text
            if (pgnum) {
                if (separator.isEmpty()) {
                    cwriter->startElement("text:index-entry-tab-stop");
                    cwriter->addAttribute("style:leader-char", m_fld->m_tabLeader);
                    //NOTE: "right" is the only option available
                    cwriter->addAttribute("style:type", "right");
                    cwriter->endElement(); //text:index-entry-tab-stop
                } else {
                    cwriter->startElement("text:index-entry-span");
                    cwriter->addTextNode(separator);
                    cwriter->endElement(); //text:index-entry-span
                }
                cwriter->startElement("text:index-entry-page-number");
                cwriter->endElement(); //text:index-entry-page-number
            }
            if (hyperlink) {
                cwriter->startElement("text:index-entry-link-end");
                cwriter->endElement(); //text:index-entry-link-end
            }
            cwriter->endElement(); //text:table-of-content-entry-template
        }

        cwriter->endElement(); //text:table-of-content-source
        //<text:index-body>
        cwriter->startElement("text:index-body");
        cwriter->addCompleteElement(m_fld->m_buffer);
        cwriter->endElement(); //text:index-body
        cwriter->endElement(); //text:table-of-content
        break;
    } // TOC
    default:
        break;
    }

    QString contents = QString::fromUtf8(buf.buffer(), buf.buffer().size());
    if (!contents.isEmpty()) {
        //nested field
        if (!m_fldStates.empty()) {
            m_fld_snippets.prepend(contents);
        }
        else {
            //add writer content to m_paragraph as a runOfText with text style
            m_paragraph->addRunOfText(contents, m_fldChp, QString(""),
                                      m_parser->styleSheet(), true);
        }
    }

    //reset
    delete m_fld;
    m_fld = 0;
    m_fldEnd++;

    //nested field
    if (!m_fldStates.empty()) {
        fld_restoreState();
    }
    else {
        m_fld = new fld_State();
        QList<QString>* list = &m_fld_snippets;
        while (!list->isEmpty()) {
            //add writer content to m_paragraph as a runOfText with text style
            m_paragraph->addRunOfText(list->takeFirst(), m_fldChp, QString(""),
                                      m_parser->styleSheet(), true);
        }
        m_fldChp = 0;
    }
} //end fieldEnd()

/**
 * This handles a basic section of text.
 *
 * Fields which are not supported by inline variables in @ref fieldEnd are also
 * dealt with by emitting the "result" text generated by Word as vanilla text.
 */
void WordsTextHandler::runOfText(const wvWare::UString& text, wvWare::SharedPtr<const wvWare::Word97::CHP> chp)
{
    bool common_flag = false;
    QString newText(Conversion::string(text));
    debugMsDoc << newText;

    if (newText.isEmpty()) {
        return;
    }

    if (m_fld->m_insideField) {
        //processing field instructions
        if (!m_fld->m_afterSeparator) {
            switch (m_fld->m_type) {
            case EQ:
            case REF:
            case HYPERLINK:
            case MACROBUTTON:
            case PAGEREF:
            case SYMBOL:
            case TOC:
            case PAGE:
            case CREATEDATE:
            case SAVEDATE:
            case TIME:
            case DATE:
                m_fld->m_instructions.append(newText);
                break;
            default:
                debugMsDoc << "Ignoring field instructions!";
                break;
            }
        }
        //processing the field result
        else {
            KoXmlWriter* writer = m_fld->m_writer;
            switch (m_fld->m_type) {
            case REF:
            case CREATEDATE:
            case SAVEDATE:
            case DATE:
            case TIME:
            case PAGEREF:
            case HYPERLINK:
                if (newText == "\t") {
                    writer->startElement("text:tab", false);
                    writer->endElement();
                } else {
                    writer->addTextNode(newText);
                }
                break;
            case AUTHOR:
            case AUTOTEXTLIST:
            case EDITTIME:
            case FILENAME:
            case MERGEFIELD:
            case REF_WITHOUT_KEYWORD:
            case SEQ:
            case SHAPE:
            case TOC:
                //NOTE: Ignoring bookmarks in the field result!
                debugMsDoc << "Processing field result as vanilla text string.";
                common_flag = true;
                break;
            default:
                debugMsDoc << "Ignoring the field result.";
                break;
            }
            if (!m_fldChp.data()) {
                m_fldChp = chp;
            }
        }
        if (common_flag) {
            //the root field controls formatting of child fields
            if (m_fldChp.data()) {
                chp = m_fldChp;
            }
        } else {
            return;
        }
    }

    // Font name, TODO: We only use the Ascii font code. We should work out
    // how/when to use the FE and Other font codes.  ftcAscii = (rgftc[0]) font
    // for ASCII text
    QString fontName = getFont(chp->ftcAscii);
    if (!fontName.isEmpty()) {
        m_mainStyles->insertFontFace(KoFontFace(fontName));
    }

    //only show text that is not hidden, TODO use text:display="none"
    if (chp->fVanish != 1) {
        //add text string and formatting style to m_paragraph
        m_paragraph->addRunOfText(newText, chp, fontName, m_parser->styleSheet());
    }

} //end runOfText()

//#define FONT_DEBUG

// Return the name of a font. We have to convert the Microsoft font names to
// something that might just be present under X11.
QString WordsTextHandler::getFont(unsigned ftc) const
{
    debugMsDoc ;
    Q_ASSERT(m_parser);

    if (!m_parser) {
        return QString();
    }
    const wvWare::Word97::FFN& ffn(m_parser->font(ftc));
    QString fontName(Conversion::string(ffn.xszFfn));
    return fontName;

// #ifdef FONT_DEBUG
//     debugMsDoc <<"    MS-FONT:" << font;
// #endif

//     static const unsigned ENTRIES = 6;
//     static const char* const fuzzyLookup[ENTRIES][2] =
//         {
//             // MS contains      X11 font family
//             // substring.       non-Xft name.
//             { "times",          "times" },
//             { "courier",        "courier" },
//             { "andale",         "monotype" },
//             { "monotype.com",   "monotype" },
//             { "georgia",        "times" },
//             { "helvetica",      "helvetica" }
//         };

//     // When Xft is available, Qt will do a good job of looking up our local
//     // equivalent of the MS font. But, we want to work even without Xft.  So,
//     // first, we do a fuzzy match of some common MS font names.
//     unsigned i;

//     for (i = 0; i < ENTRIES; i++)
//     {
//         // The loop will leave unchanged any MS font name not fuzzy-matched.
//         if (font.find(fuzzyLookup[i][0], 0, false) != -1)
//         {
//             font = fuzzyLookup[i][1];
//             break;
//         }
//     }

// #ifdef FONT_DEBUG
//     debugMsDoc <<"    FUZZY-FONT:" << font;
// #endif

//     // Use Qt to look up our canonical equivalent of the font name.
//     QFont xFont( font );
//     QFontInfo info( xFont );

// #ifdef FONT_DEBUG
//     debugMsDoc <<"    QT-FONT:" << info.family();
// #endif
//     return info.family();

}//end getFont()

bool WordsTextHandler::writeListInfo(KoXmlWriter* writer, const wvWare::Word97::PAP& pap,
                                     const wvWare::ListInfo* listInfo)
{
    debugMsDoc;

    int nfc = listInfo->numberFormat();

    //check to see if we're in a heading instead of a list if so, just return
    //false so writeLayout can process the heading
    if (listInfo->lsid() == 1 && nfc == 255) {
        return false;
    }

    //put the currently used writer in the stack
    m_usedListWriters.push(writer);

    quint8 listLevel = pap.ilvl;
    int listId = listInfo->lsid();

    m_currentListID = listId;
    m_currentListLevel = listLevel;

    // update automatic numbering info
    if (listInfo->type() == wvWare::ListInfo::NumberType) {
        if (m_continueListNum.contains(listId)) {
            if ( !(listLevel <= m_continueListNum[listId].first) ) {

                // TODO: Check if any of the lists that inherit numbering
                // from the abstract numbering definition was opened.

                m_continueListNum[listId].second = false;

                QString key;
                int i = m_continueListNum[listId].first;
                while (i > listLevel) {
                    key = QString("%1").arg(listId);
                    key.append(QString(".lvl%1").arg(i));
                    m_numIdXmlIdMap.remove(key);
                    --i;
                }
            }
        }
    } else {
        // update all items with listLevel > m_currentListLevel
        QMap<int, QPair<quint8, bool> >::const_iterator i = m_continueListNum.constBegin();
        while (i != m_continueListNum.constEnd()) {
            if (i.value().first > listLevel) {
                m_continueListNum[i.key()].second = false;
            }
            i++;
        }
    }

    // A new style is required because text-properties of a
    // list-leve-style-* might differ.

    KoGenStyle listStyle(KoGenStyle::ListAutoStyle);
    if (document()->writingHeader()) {
        listStyle.setAutoStyleInStylesDotXml(true);
    }
    defineListStyle(listStyle);

    writer->startElement("text:list");
    writer->addAttribute("text:style-name", m_mainStyles->insert(listStyle));

    if (listInfo->type() == wvWare::ListInfo::NumberType) {
        QString key = QString("%1").arg(listId);
        key.append(QString(".lvl%1").arg(listLevel));

        //automatic numbering
        if (m_continueListNum.contains(listId) && m_continueListNum[listId].second) {
            writer->addAttribute("text:continue-list", m_numIdXmlIdMap[key]);
        }
        QString xmlId = key;
        xmlId.append(QString("_%1").arg(qrand())).prepend("lst");
        writer->addAttribute("xml:id", xmlId);
        m_numIdXmlIdMap[key] = xmlId;
    }

    writer->startElement("text:list-item");
    for (int i = 0; i < listLevel; i++) {
        writer->startElement("text:list");
        writer->startElement("text:list-item");
    }

    // restart numbering if applicable
    if (listInfo->type() == wvWare::ListInfo::NumberType) {
        if (!m_continueListNum.contains(listId) ||
            (m_continueListNum.contains(listId) && !m_continueListNum[listId].second)) {
            writer->addAttribute("text:start-value", listInfo->startAt());
        }
        m_continueListNum[listId] = qMakePair(listLevel, true);
    }

    return true;
} //end writeListInfo()

/**
 * A helper function to add the <style:list-level-properties> child element to
 * the <text:list-level-style-*> element.
 */
void setListLevelProperties(KoXmlWriter& out,
                            const wvWare::Word97::PAP& pap, const wvWare::ListInfo& listInfo,
                            const QString& fontSizePt)
{
    //
    // TEXT POSITION:
    //
    // fo:margin-left - Specifies the left margin for the paragraph, so it
    // controls position of the 2nd line of the paragraph from the left page
    // margin.
    //
    // text:list-tab-stop-position - Specifies the Tab space from the left page
    // margin, so it controls position of the text of the 1st line of the
    // paragraph.
    //
    // BULLET POSITION:
    //
    // fo:text-indent - Specifies indent for the 1st line of the paragraph,
    // relative to the rest of the paragraph, so it controls position of the
    // bullet from the left margin specified by fo:margin-left.
    //
    // fo:text-align - Specifies the horizontal alignment of the list label at
    // the alignment position.  The alignment position is specified by the
    // fo:margin-left and fo:text-indent attributes.
    //

    out.startElement("style:list-level-properties");
    //fo:text-align
    switch (listInfo.alignment()) {
    case 0: //Left justified - OOo seems to treat this value as DEFAULT
        out.addAttribute("fo:text-align", "start");
        break;
    case 1: //Center justified
        out.addAttribute("fo:text-align", "center");
        break;
    case 2: //Right justified
        out.addAttribute("fo:text-align", "end");
        break;
    case 3: //TODO: Not documented in [MS-DOC] - v20101219, any test files?
        out.addAttribute("fo:text-align", "justify");
        break;
    default:
        break;
    }

    //TODO: make use of PICF, only use fontSize if chp->fNoAutoSize == 0
    if (listInfo.type() == wvWare::ListInfo::PictureType) {
        if (!fontSizePt.isEmpty()) {
            out.addAttribute("fo:width", fontSizePt);
            out.addAttribute("fo:height", fontSizePt);
        } else {
            debugMsDoc << "Can NOT set automatic size of the bullet picture, fontSize empty!";
        }
    }

    //text:list-level-position-and-space-mode
    out.addAttribute("text:list-level-position-and-space-mode", "label-alignment");
    //style:list-level-label-alignment
    out.startElement("style:list-level-label-alignment");
    //fo:margin-left
    out.addAttributePt("fo:margin-left", Conversion::twipsToPt(pap.dxaLeft));
    //fo:text-indent
    out.addAttributePt("fo:text-indent", Conversion::twipsToPt(pap.dxaLeft1));
    //text:label-followed-by
    switch (listInfo.followingChar()) {
    case 0: //A tab follows the number text.
        out.addAttribute("text:label-followed-by", "listtab");
        break;
    case 1: //A space follows the number text.
        out.addAttribute("text:label-followed-by", "space");
        break;
    case 2: //Nothing follows the number text.
        out.addAttribute("text:label-followed-by", "nothing");
        break;
    default:
        break;
    }
    out.endElement(); //style:list-level-label-alignment
    out.endElement(); //style:list-level-properties
}

void WordsTextHandler::defineListStyle(KoGenStyle &style)
{
    const wvWare::ListInfo* listInfo = m_currentPPs->listInfo();
    if (!listInfo) {
        return;
    }

    const wvWare::Word97::PAP& pap = m_currentPPs->pap();
    const wvWare::SharedPtr<wvWare::Word97::CHP> chp = listInfo->text().chp;
    wvWare::UString text = listInfo->text().text;
    int nfc = listInfo->numberFormat();

    QBuffer buf;
    buf.open(QIODevice::WriteOnly);
    KoXmlWriter out(&buf);

    //---------------------------------------------
    // list-level-style-*
    //---------------------------------------------
    if (listInfo->type() == wvWare::ListInfo::BulletType) {
        out.startElement("text:list-level-style-bullet");
        if (text.length() == 1) {
            // With bullets, text can only be one character, which tells us
            // what kind of bullet to use
            unsigned int code = text[0].unicode();
//             debugMsDoc << "Bullet code: 0x" << hex << code << "id:" << code%256;

            // NOTE: What does the next comment mean, private use area is in
            // <0xE000, 0xF8FF>, disabled the conversion code.

            // unicode: private use area (0xf000 - 0xf0ff).
//             if ((code & 0xFF00) == 0xF000) {
//                 if (code >= 0x20) {
//                     // microsoft symbol charset shall apply here.
//                     code = Conversion::MS_SYMBOL_ENCODING[code%256];
//             debugMsDoc << "Changed the symbol encoding: new code: 0x" << hex << code <<
//                                       dec << "("<< code << ")";
//                 } else {
//                     code &= 0x00FF;
//                 }
//             }
            out.addAttribute("text:bullet-char", QChar(code));
        } else {
            warnMsDoc << "Bullet with more than one character, not supported";
        }
    }
    else if (listInfo->type() == wvWare::ListInfo::PictureType) {
        out.startElement("text:list-level-style-image");
        out.addAttribute("xlink:href", listInfo->bulletPictureName().prepend(QString("Pictures/")));
        out.addAttribute("xlink:type", "simple");
        out.addAttribute("xlink:show", "embed");
        out.addAttribute("xlink:actuate", "onLoad");
    }
    else {
        out.startElement("text:list-level-style-number");

        //*************************************
        int depth = pap.ilvl; //both are 0 based

        // Now we need to parse the text, to try and convert msword's powerful
        // list template stuff
        QString prefix, suffix;
        bool depthFound = false;
        bool anyLevelFound = false;
        int displayLevels = 1;

        // We parse <0>.<2>.<1>. as "level 2 with suffix='.'" (no prefix).  But
        // "Section <0>.<1>)" has both prefix and suffix.  The common case is
        // <0>.<1>.<2> (display-levels=3) loop through all of text this just
        // sets depthFound & displayLevels & the suffix & prefix
        for (int i = 0 ; i < text.length() ; ++i) {
            short ch = text[i].unicode();
            //debugMsDoc << i <<":" << ch;
            if (ch < 10) {   // List level place holder
                if (ch == pap.ilvl) {
                    if (depthFound) {
                        warnMsDoc << "ilvl " << pap.ilvl << " found twice in listInfo text...";
                    }
                    else {
                        depthFound = true;
                    }
                    // really should never do anything so why is it here??
                    suffix.clear();
                } else {
                    // Can't see how level 1 would have a <0> in it...
                    Q_ASSERT(ch < pap.ilvl);
                    if (ch < pap.ilvl) {
                        // we found a 'parent level', to be displayed
                        ++displayLevels;
                    }
                }
                anyLevelFound = true;
            } else {
                //add it to suffix if we've found the level that we're at
                if (depthFound) {
                    suffix += QChar(ch);
                }
                //or add it to prefix if we haven't
                else if (!anyLevelFound) {
                    prefix += QChar(ch);
                }
            }
        }
        if (displayLevels > 1) {
            // This is a hierarchical list numbering e.g. <0>.<1>.
            //
            // Unless this is about a heading, in that case we've set numbering
            // type to 1 already, so it will indeed look like that.  The
            // question is whether the '.' is the suffix of the parent level
            // already.  Do I still need to keep the m_listSuffixes stuff?
            if (depth > 0 && !prefix.isEmpty() && m_listSuffixes[ depth - 1 ] == prefix) {
                 // it's already the parent's suffix -> remove it
                prefix.clear();
                debugMsDoc << "depth=" << depth << " parent suffix is" << prefix << " -> clearing";
            }
        }
        // Heading styles don't set the ilvl, but must have a depth coming from
        // their heading level (the style's STI)

//         bool isHeading = style->sti() >= 1 && style->sti() <= 9;
//         if ( depth == 0 && isHeading ) {
//             depth = style->sti() - 1;
//         }

//         int numberingType = listInfo->isWord6() && listInfo->prev() ? 1 : 0;
//         if ( isHeading ) numberingType = 1;
        if (depthFound) {
            // Word6 models "1." as nfc=5
            if (nfc == 5 && suffix.isEmpty()) {
                suffix = '.';
            }
            debugMsDoc << " prefix=" << prefix << " suffix=" << suffix;
            out.addAttribute("style:num-format", Conversion::numberFormatCode(nfc));
            if (!prefix.isEmpty()) {
                out.addAttribute("style:num-prefix", prefix);
            }
            if (!suffix.isEmpty()) {
                out.addAttribute("style:num-suffix", suffix);
            }
            if (displayLevels > 1) {
                out.addAttribute("text:display-levels", displayLevels);
            }
            debugMsDoc << "storing suffix" << suffix << " for depth" << depth;
            m_listSuffixes[ depth ] = suffix;
        } else {
            warnMsDoc << "Not supported: counter text without the depth in it:" << Conversion::string(text);
        }
    } //end numbered list

    out.addAttribute("text:level", pap.ilvl + 1);

    //---------------------------------------------
    // text-properties
    //---------------------------------------------
    KoGenStyle textStyle(KoGenStyle::TextStyle, "text");

    if (chp) {
        QString fontName = getFont(chp->ftcAscii);
        if (!fontName.isEmpty()) {
            m_mainStyles->insertFontFace(KoFontFace(fontName));
            textStyle.addProperty("style:font-name", fontName, KoGenStyle::TextType);
        }
        m_paragraph->applyCharacterProperties(chp, &textStyle, 0);
    } else {
        debugMsDoc << "Missing CHPs for the label!";
    }
    if (listInfo->type() != wvWare::ListInfo::PictureType) {
        out.addAttribute("text:style-name", m_mainStyles->insert(textStyle, "T"));
    }

    //---------------------------------------------
    // list-level-properties
    //---------------------------------------------
    setListLevelProperties(out, pap, *listInfo, textStyle.property("fo:font-size"));
    out.endElement(); //text:list-level-style-*

    //---------------------------------------------
    // update list style
    //---------------------------------------------
    QString contents = QString::fromUtf8(buf.buffer(), buf.buffer().size());
    QString name(QString::number(listInfo->lsid()));
    style.addChildElement(name.append("lvl").append(QString::number(pap.ilvl)), contents);
} //end defineListStyle()

void WordsTextHandler::closeList()
{
    debugMsDoc;
    // Set the correct XML writer, get the last used writer from stack
    KoXmlWriter *writer = m_usedListWriters.pop();

    //TODO: should probably test this more, to make sure it does work this way
    //for level 0, we need to close the last item and the list
    //for level 1, we need to close the last item and the list, and the last item and the list
    //for level 2, we need to close the last item and the list, and the last item adn the list, and again
    for (int i = 0; i <= m_currentListLevel; i++) {
        writer->endElement(); //text:list-item
        writer->endElement(); //text:list
    }

    m_currentListID = 0;
    m_currentListLevel = -1;
}

bool WordsTextHandler::listIsOpen()
{
    return m_currentListID != 0;
}

void WordsTextHandler::saveState()
{
    debugMsDoc;
    m_oldStates.push(State(m_currentTable, m_paragraph,
                           m_currentListLevel, m_currentListID,
                           m_drawingWriter, m_insideDrawing));
    m_currentTable = 0;
    m_paragraph = 0;
    m_currentListLevel = -1;
    m_currentListID = 0;

    m_drawingWriter = 0;
    m_insideDrawing = false;
}

void WordsTextHandler::restoreState()
{
    debugMsDoc;
    //if the stack is corrupt, we won't even try to set it correctly
    if (m_oldStates.empty()) {
        warnMsDoc << "Error: save/restore stack is corrupt!";
        return;
    }
    State s(m_oldStates.top());
    m_oldStates.pop();

    //warn if pointers weren't reset properly, but restore state anyway
    if (m_paragraph != 0) {
        warnMsDoc << "Warning: m_paragraph pointer wasn't reset!";
    }
    if (m_currentTable != 0) {
        warnMsDoc << "Warning: m_currentTable pointer wasn't reset!";
    }
    if (m_drawingWriter != 0) {
        warnMsDoc << "Warning: m_drawingWriter pointer wasn't reset!";
    }

    m_paragraph = s.paragraph;
    m_currentTable = s.table;
    m_currentListLevel = s.listDepth;
    m_currentListID = s.listID;

    m_drawingWriter = s.drawingWriter;
    m_insideDrawing = s.insideDrawing;
}

void WordsTextHandler::fld_saveState()
{
    m_fldStates.push(m_fld);

    //reset fields related variables
    m_fld = 0;
}

void WordsTextHandler::fld_restoreState()
{
    //if the stack is corrupt, we won't even try to set it correctly
    if (m_fldStates.empty()) {
        warnMsDoc << "Error: save/restore stack is corrupt!";
        return;
    }

    //warn if pointers weren't reset properly, but restore state anyway
    if (m_fld && m_fld->m_writer != 0) {
        warnMsDoc << "m_fld->m_writer pointer wasn't reset";
    }
    if (m_fld && m_fld->m_buffer != 0) {
        warnMsDoc << "m_fld->m_buffer pointer wasn't reset";
    }

    m_fld = m_fldStates.top();
    m_fldStates.pop();
}

// ************************************************
//  Obsolete
// ************************************************

#ifdef TEXTHANDLER_OBSOLETE

//create an element for the variable
QDomElement WordsTextHandler::insertVariable(int type, wvWare::SharedPtr<const wvWare::Word97::CHP> chp, const QString& format)
{
    Q_UNUSED(chp);

    debugMsDoc ;
    //m_paragraph += '#';

    QDomElement formatElem;
    //writeFormattedText(chp, m_currentStyle ? &m_currentStyle->chp() : 0);

    //m_index += 1;

    QDomElement varElem = m_formats.ownerDocument().createElement("VARIABLE");
    QDomElement typeElem = m_formats.ownerDocument().createElement("TYPE");
    typeElem.setAttribute("type", QString::number(type));
    typeElem.setAttribute("key", format);
    varElem.appendChild(typeElem);
    formatElem.appendChild(varElem);
    return varElem;
}

QDomElement WordsTextHandler::insertAnchor(const QString& fsname)
{
    Q_UNUSED(fsname);

    debugMsDoc ;
    //m_paragraph += '#';

    // Can't call writeFormat, we have no chp.
    //QDomElement format( mainDocument().createElement( "FORMAT" ) );
    //format.setAttribute( "id", 6 );
    //format.setAttribute( "pos", m_index );
    //format.setAttribute( "len", 1 );
    //m_formats.appendChild( format );
    //QDomElement formatElem = format;

    //m_index += 1;

    //QDomElement anchorElem = m_formats.ownerDocument().createElement( "ANCHOR" );
    //anchorElem.setAttribute( "type", "frameset" );
    //anchorElem.setAttribute( "instance", fsname );
    //formatElem.appendChild( anchorElem );
    return QDomElement();
}
#endif // TEXTHANDLER_OBSOLETE
