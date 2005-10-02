/* This file is part of the KDE project
   Copyright (C) 1999 Werner Trobin <trobin@kde.org>

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

#include <klocale.h>
#include <koFilterManager.h>
#include <ktempfile.h>
#include <properties.h>
#include <qfile.h>
#include <qcolor.h>
#include <winworddoc.h>
#include <typeinfo>
#include <unistd.h>

#define ROW_SIZE 20

const unsigned WinWordDoc::s_height = 841;
const unsigned WinWordDoc::s_width = 595;
const unsigned WinWordDoc::s_hMargin = 28;
const unsigned WinWordDoc::s_vMargin = 42;

WinWordDoc::WinWordDoc(
    QCString &result,
    const myFile &mainStream,
    const myFile &table0Stream,
    const myFile &table1Stream,
    const myFile &dataStream) :
        Document(
            mainStream,
            table0Stream,
            table1Stream,
            dataStream),
        m_result(result)
{
    m_isConverted = false;
    m_success = true;
    m_body = "";
    m_tables = "";
    m_pixmaps = "";
    m_pictureCount = 0;
    m_embedded = "";
    m_embeddedCount = 0;
    m_extraFrameSets = "";
    m_cellEdges.setAutoDelete(true);
    m_table.setAutoDelete(true);
    m_delay = 0;
    m_shapeID = 0;
}

WinWordDoc::~WinWordDoc()
{
}

char WinWordDoc::borderStyle(unsigned btc) const
{
    // border type code:
    //     0 none
    //     1 single
    //     2 thick
    //     3 double
    //     5 hairline
    //     6 dot
    //     7 dash large gap
    //     8 dot dash
    //     9 dot dot dash
    //     10 triple
    //     11 thin-thick small gap
    //     12 thick-thin small gap
    //     13 thin-thick-thin small gap
    //     14 thin-thick medium gap
    //     15 thick-thin medium gap
    //     16 thin-thick-thin medium gap
    //     17 thin-thick large gap
    //     18 thick-thin large gap
    //     19 thin-thick-thin large gap
    //     20 wave
    //     21 double wave
    //     22 dash small gap
    //     23 dash dot stroked
    //     24 emboss 3D
    //     25 engrave 3D
    //     codes 64 - 230 represent border art types and are used only for page borders.

    if (btc == 7)
        return '1';
    if (btc == 6)
        return '2';
    if (btc == 8)
        return '3';
    if (btc == 9)
        return '4';
    return '0';
}

//
// Add to/lookup a cell edge in the cache of cell edges for a given table.
//

int WinWordDoc::cacheCellEdge(
    unsigned tableNumber,
    unsigned cellEdge)
{
    QMemArray<unsigned> *edges = m_cellEdges[tableNumber - 1];
    unsigned i;
    unsigned *data;
    unsigned index;

    // Do we already know about this edge?

    data = edges->data();
    index = edges->size();
    for (i = 0; i < index; i++)
    {
        if (data[i] == cellEdge)
            return i;
    }

    // Add the edge to the (sorted) array.

    edges->resize(index + 1);
    data = edges->data();
    data[index] = cellEdge;
    for (i = index; i > 0; i--)
    {
        unsigned tmp;

        if (data[i - 1] > data[i])
        {
            tmp = data[i - 1];
            data[i - 1] = data[i];
            data[i] = tmp;
        }
        else
        {
            break;
        }
    }
    return i;
}

//
// Compute the Word notion of cell edge ordinate into a Kword one.
//

unsigned WinWordDoc::computeCellEdge(
    MsWordGenerated::TAP &row,
    unsigned edge)
{
    unsigned rowWidth = row.rgdxaCenter[row.itcMac] - row.rgdxaCenter[0];
    unsigned cellEdge;

    // We want to preserve the proportion of row widths in the original document.
    // For now, we do so on the assumption that the table occupies the full width of
    // the page.

    cellEdge = row.rgdxaCenter[edge] - row.rgdxaCenter[0];
    cellEdge = (unsigned)((double)cellEdge * (s_width - s_hMargin - s_hMargin - 6) / rowWidth);
    return cellEdge + s_hMargin;
}

bool WinWordDoc::convert()
{
    // We do the conversion in two passes, to allow all the tables to be turned into framesets
    // after the main frameset with the text.

    if (!m_isConverted)
    {
        m_body = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE DOC>\n"
            "<DOC editor=\"KWord\" mime=\"application/x-kword\" syntaxVersion=\"1\">\n"
            " <PAPER format=\"1\" width=\"595\" height=\"841\" orientation=\"0\" columns=\"1\" columnspacing=\"2\" hType=\"0\" fType=\"0\" spHeadBody=\"9\" spFootBody=\"9\">\n"
            "  <PAPERBORDERS left=\"");
        m_body.append(QString::number(s_hMargin));
        m_body.append("\" top=\"");
        m_body.append(QString::number(s_vMargin));
        m_body.append("\" right=\"");
        m_body.append(QString::number(s_hMargin));
        m_body.append("\" bottom=\"");
        m_body.append(QString::number(s_vMargin));
        m_body.append("\"/>\n"
            " </PAPER>\n"
            " <ATTRIBUTES processing=\"0\" standardpage=\"1\" hasHeader=\"0\" hasFooter=\"0\" unit=\"mm\"/>\n"
            " <FOOTNOTEMGR>\n"
            "  <START value=\"1\"/>\n"
            "  <FORMAT superscript=\"1\" type=\"1\"/>\n"
            "  <FIRSTPARAG ref=\"(null)\"/>\n"
            " </FOOTNOTEMGR>\n"
            " <FRAMESETS>\n");
        m_body.append(
            "  <FRAMESET frameType=\"1\" frameInfo=\"0\" removeable=\"0\" visible=\"1\">\n"
            "   <FRAME left=\"");
        m_body.append(QString::number(s_hMargin));
        m_body.append("\" top=\"");
        m_body.append(QString::number(s_vMargin));
        m_body.append("\" right=\"");
        m_body.append(QString::number(s_width - s_hMargin));
        m_body.append("\" bottom=\"");
        m_body.append(QString::number(s_height - s_vMargin));
        m_body.append("\" runaround=\"1\" runaGap=\"2\"");
        m_body.append(" autoCreateNewFrame=\"1\" newFrameBehaviour=\"0\"/>\n");
        parse();
        m_body.append(
            "  </FRAMESET>\n");
        m_body.append(m_tables);
        m_body.append(m_extraFrameSets);
        m_body.append(
            " </FRAMESETS>\n");

        // Output styles.

        m_body.append(
            " <STYLES>\n");
        getStyles();
        m_body.append(
            " </STYLES>\n");

        // Do we have any images?

        if (m_pixmaps.length())
        {
            m_body.append(
                "  <PIXMAPS>\n");
            m_body.append(m_pixmaps);
            m_body.append(
                "  </PIXMAPS>\n");
        }

        // Do we have any embedded objects?

        if (m_embedded.length())
        {
            m_body.append(m_embedded);
        }
        m_body.append(
            "</DOC>\n");
//        kdDebug(s_area) << m_body << endl;
        m_result = m_body.utf8();
        m_isConverted = true;
    }
    return m_success;
}

void WinWordDoc::internalCommDelayStream( const char* delay )
{
    delay = m_delay;
}

void WinWordDoc::internalCommShapeID( unsigned int& shapeID )
{
    shapeID = m_shapeID;
}

void WinWordDoc::encode(QString &text)
{
    // When encoding the stored form of text to its run-time form,
    // be sure to do the conversion for "&amp;" to "&" first to avoid
    // accidentally converting user text into one of the other escape
    // sequences.

    text.replace('&', "&amp;");
    text.replace('<', "&lt;");
    text.replace('>', "&gt;"); // Needed to avoid ]]>

    // Strictly, there is no need to encode " or ', but we do so to allow
    // them to co-exist!

    text.replace('"', "&quot;");
    text.replace('\'', "&apos;");

    // Replace ASCII 12 with a page break.
    text.replace(char(12), "</TEXT>\n<LAYOUT>\n<PAGEBREAKING hardFrameBreakAfter=\"true\" /></LAYOUT>\n</PARAGRAPH>\n<PARAGRAPH>\n<TEXT>");

}

QString WinWordDoc::generateBorder(
    const char *borderName,
    const BRC &brc)
{
    QString prefix = QString(" ") +  borderName;
    QString border;

    border.append(prefix);

    // TBD: we seem to have trouble with Word2000 border widths.
    int width = (brc.dptLineWidth < 4) ? brc.dptLineWidth : 4;
    border.append(QString::fromLatin1("Width=\"%1\"").arg(width * 0.125));
    border.append(prefix);
    border.append(QString::fromLatin1("Style=\"%1\"").arg(borderStyle(brc.brcType)));

    QColor colour = colorForNumber(QString::number(brc.ico), -1);
    border.append(generateColour(borderName, colour));
    return border;
}

QString WinWordDoc::generateColour(
    const char *colourName,
    const QColor &qc)
{
    QString prefix = QString(" ") +  colourName;
    QString colour;

    colour.append(prefix);
    colour.append(QString::fromLatin1("Red=\"%1\"").arg(qc.red()));
    colour.append(prefix);
    colour.append(QString::fromLatin1("Green=\"%1\"").arg(qc.green()));
    colour.append(prefix);
    colour.append(QString::fromLatin1("Blue=\"%1\"").arg(qc.blue()));
    return colour;
}

QString WinWordDoc::generateFormat(
    const CHP *chp)
{
    QString format;
    QColor color = colorForNumber(QString::number(chp->ico), -1);

    format.append(QString::fromLatin1("<COLOR red=\"%1\" green=\"%2\" blue=\"%3\"/>\n").arg(color.red()).arg(color.green()).arg(color.blue()));
    format.append("<FONT name=\"");
    // TBD: We only use the Ascii font code. We should work out how/when to use the FE and Other font codes.
    format.append(getFont(chp->ftcAscii));
    format.append("\"/>\n");
    format.append("<SIZE value=\"");
    // TBD: where should we really get the size?
    format.append(QString::number(chp->hps / 2));
    //format.append(QString::number(12));
    format.append("\"/>\n");
    if (chp->fBold)
        format.append("<WEIGHT value=\"75\"/>\n");
    else
        format.append("<WEIGHT value=\"50\"/>\n");
    if (chp->fItalic)
        format.append("<ITALIC value=\"1\"/>\n");
    else
        format.append("<ITALIC value=\"0\"/>\n");
    if (chp->kul != 0)
        format.append("<UNDERLINE value=\"1\"/>\n");
    else
        format.append("<UNDERLINE value=\"0\"/>\n");
    if (chp->iss != 0)
    {
        if (chp->iss == 1)
            format.append("<VERTALIGN value=\"2\"/>\n");
        else
            format.append("<VERTALIGN value=\"1\"/>\n");
    }
    else
    {
        format.append("<VERTALIGN value=\"0\"/>\n");
    }
    return format;
}

// Write out the <FORMATS> for each run in a paragraph.
QString WinWordDoc::generateFormats(
    Attributes &attributes)
{
    QString formats;
    KSharedPtr<Run> run;
    QValueList<KSharedPtr<Run> >::ConstIterator it;

    formats.append("<FORMATS>\n");

    for (it = attributes.runs().begin(); it != attributes.runs().end(); ++it)
    {
        run = *it;
        if (typeid(Format) == typeid(*run))
        {
            const MsWordGenerated::CHP *chp = static_cast<Format *>(run.data())->values->getChp();

            formats.append("<FORMAT id=\"1\" pos=\"");
            formats.append(QString::number(run->start));
            formats.append("\" len=\"");
            formats.append(QString::number(run->end - run->start));
            formats.append("\">\n");
            formats.append(generateFormat(chp));
            formats.append("</FORMAT>\n");
        }
        else
        if (typeid(Image) == typeid(*run))
        {
            Image *image = static_cast<Image *>(run.data());
            QString ourKey;
            QString uid;

            ourKey = "image" + QString::number(image->id) + "." + image->type;
            emit signalSavePic(
                    ourKey,
                    uid,
                    image->type,
                    image->length,
                    image->data);

            // Add an anchor for this frameset.

            m_pictureCount++;
            formats.append("<FORMAT id=\"6\" pos=\"");
            formats.append(QString::number(image->start));
            formats.append("\" len=\"1\">\n");
            formats.append("<ANCHOR type=\"frameset\" instance=\"");
            formats.append(i18n("Picture %1").arg(m_pictureCount));
            formats.append("\"/>\n");
            formats.append("</FORMAT>\n");

            m_extraFrameSets.append("<FRAMESET frameType=\"2\" frameInfo=\"0\" name=\"");
            m_extraFrameSets.append(i18n("Picture %1").arg(m_pictureCount));
            m_extraFrameSets.append("\">\n");
            m_extraFrameSets.append("<FRAME runaround=\"1\" right=\"74.92\" left=\"32.68\" newFrameBehaviour=\"0\" bottom=\"87.4\" runaroundGap=\"2.83465\" top=\"45.16\" />\n");
            m_extraFrameSets.append("<IMAGE>\n");
            m_extraFrameSets.append("<FILENAME value=\"");
            m_extraFrameSets.append(ourKey);
            m_extraFrameSets.append("\"/>\n");
            m_extraFrameSets.append("</IMAGE>\n");
            m_extraFrameSets.append("</FRAMESET>\n");

            // Add an entry to the list of pixmaps too.

            m_pixmaps.append("<KEY key=\"");
            m_pixmaps.append(ourKey);
            m_pixmaps.append("\" name=\"");
            m_pixmaps.append(uid);
            m_pixmaps.append("\"/>\n");
        }
        else
        if (typeid(VectorGraphic) == typeid(*run))
        {
#define KONTOUR_WORKS_AGAIN
#ifdef KONTOUR_WORKS_AGAIN
            VectorGraphic *vectorGraphic = static_cast<VectorGraphic *>(run.data());
            QString ourKey;
            QString uid;
            QString mimeType;
            QString filterArgs;

            // Send the picture to the outside world and get back the UID.

            ourKey = "vectorGraphic" + QString::number(vectorGraphic->id) + "." + vectorGraphic->type;
            if (vectorGraphic->type == "msod")
            {
                m_shapeID = vectorGraphic->id;
                m_delay = vectorGraphic->delay;
                //filterArgs = "shape-id=";
                //filterArgs += QString::number(vectorGraphic->id);
                //filterArgs += ";delay-stream=";
                //filterArgs += QString::number((unsigned long)vectorGraphic->delay);
            }
            emit signalSavePart(
                    ourKey,
                    uid,
                    mimeType,
                    vectorGraphic->type,
                    vectorGraphic->length,
                    vectorGraphic->data);

            m_shapeID = 0;
            m_delay = 0;

            // Add an anchor for this frameset.

            m_pictureCount++;
            formats.append("<FORMAT id=\"6\" pos=\"");
            formats.append(QString::number(vectorGraphic->start));
            formats.append("\" len=\"1\">\n");
            formats.append("<ANCHOR type=\"frameset\" instance=\"");
            formats.append(i18n("Picture %1").arg(m_pictureCount));
            formats.append("\"/>\n");
            formats.append("</FORMAT>\n");

            // Add an entry to the list of embedded objects too. TBD: fix
            // RECT and FRAME settings.

            m_embedded.append(
                "  <EMBEDDED>\n");
            m_embedded.append("<OBJECT url=\"");
            m_embedded.append(uid);
            m_embedded.append("\" mime=\"");
            m_embedded.append(mimeType);
            m_embedded.append("\">\n<RECT x=\"30\" y=\"190\" w=\"120\" h=\"80\"/>\n");
            m_embedded.append("</OBJECT>\n");
            m_embedded.append("<SETTINGS name=\"");
            m_embedded.append(i18n("Picture %1").arg(m_pictureCount));
            m_embedded.append("\">\n");
            m_embedded.append("<FRAME left=\"30\" top=\"190\" right=\"149\" bottom=\"269\" tRed=\"0\" tGreen=\"0\" tBlue=\"0\" bRed=\"0\" bGreen=\"0\" bBlue=\"0\"/>\n");
            m_embedded.append("</SETTINGS>\n");
            m_embedded.append(
                "  </EMBEDDED>\n");
#endif
        }
        else
        if (typeid(Object) == typeid(*run))
        {
            Object *object = static_cast<Object *>(run.data());

            // Send the OLE id to the outside world and get back the UID.

            QString uid;
            QString mimeType;

            emit signalPart(
                QFile::encodeName("_" + QString::number(object->id)),
                uid,
                mimeType);

            // Add an anchor for this frameset.

            m_embeddedCount++;
            formats.append("<FORMAT id=\"6\" pos=\"");
            formats.append(QString::number(object->start));
            formats.append("\" len=\"1\">\n");
            formats.append("<ANCHOR type=\"frameset\" instance=\"");
            formats.append(i18n("Object %1").arg(m_embeddedCount));
            formats.append("\"/>\n");
            formats.append("</FORMAT>\n");

            // Add an entry to the list of embedded objects too. TBD: fix
            // RECT and FRAME settings.

            m_embedded.append(
                "  <EMBEDDED>\n");
            m_embedded.append("<OBJECT url=\"");
            m_embedded.append(uid);
            m_embedded.append("\" mime=\"");
            m_embedded.append(mimeType);
            m_embedded.append("\">\n<RECT x=\"30\" y=\"190\" w=\"120\" h=\"80\"/>\n");
            m_embedded.append("</OBJECT>\n");
            m_embedded.append("<SETTINGS name=\"");
            m_embedded.append(i18n("Object %1").arg(m_embeddedCount));
            m_embedded.append("\">\n");
            m_embedded.append("<FRAME left=\"30\" top=\"190\" right=\"149\" bottom=\"269\" tRed=\"0\" tGreen=\"0\" tBlue=\"0\" bRed=\"0\" bGreen=\"0\" bBlue=\"0\"/>\n");
            m_embedded.append("</SETTINGS>\n");
            m_embedded.append(
                "  </EMBEDDED>\n");
        }
        else
        {
            kdError(s_area) << "WinWordDoc::generateFormats: unexpected typeid" << endl;
        }
    }
    formats.append("</FORMATS>\n");
    return formats;
}

void WinWordDoc::gotDocumentInformation(
    const QString &title,
    const QString &subject,
    const QString &author,
    const QString &lastRevisedBy)
{
    emit signalSaveDocumentInformation(
        i18n("%1, last revised by %2").arg(author).arg(lastRevisedBy),
        QString::null, QString::null, QString::null, QString::null,
        QString::null, QString::null, QString::null, QString::null,
        QString::null, title, subject);
}

void WinWordDoc::gotError(
    const QString &text)
{
    QString xml_friendly = text;

    encode(xml_friendly);
    m_body.append("<PARAGRAPH>\n<TEXT>");
    m_body.append(xml_friendly);
    m_body.append("</TEXT>\n</PARAGRAPH>\n");
    m_success = false;
}

void WinWordDoc::gotParagraph(
    const QString &text,
    Attributes &styles)
{
    QString xml_friendly = text;
    const PAP *pap = styles.baseStyle().getPap();

    encode(xml_friendly);
    m_body.append("<PARAGRAPH>\n<TEXT>");
    m_body.append(xml_friendly);
    m_body.append("</TEXT>\n");
    m_body.append(generateFormats(styles));
    m_body.append(
        " <LAYOUT>\n"
        "  <NAME value=\"");
    m_body.append(m_styles.names[pap->istd]);
    m_body.append("\"/>\n");
    m_body.append(
        "   <FLOW ");
    m_body.append(justification(pap->jc));
    m_body.append("/>\n");
    m_body.append(
        " </LAYOUT>\n");
    m_body.append("</PARAGRAPH>\n");
}

void WinWordDoc::gotHeadingParagraph(
    const QString &text,
    Attributes &styles)
{
    QString xml_friendly = text;
    const PAP *pap = styles.baseStyle().getPap();

    encode(xml_friendly);
    m_body.append("<PARAGRAPH>\n<TEXT>");
    m_body.append(xml_friendly);
    m_body.append("</TEXT>\n");
    m_body.append(generateFormats(styles));
    m_body.append(
        " <LAYOUT>\n"
        "  <NAME value=\"");
    m_body.append(m_styles.names[pap->istd]);
    m_body.append("\"/>\n");
    m_body.append(
        "   <FLOW ");
    m_body.append(justification(pap->jc));
    m_body.append("/>\n");
//    m_body.append("  <COUNTER type=\"");
//    m_body.append(numbering(styles.baseStyle.anld.nfc));
//    m_body.append("\" depth=\"");
//    m_body.append(QString::number(styles.baseStyle.istd - 1));
//    m_body.append("\" bullet=\"176\" start=\"1\" numberingtype=\"1\" lefttext=\"\" righttext=\"\" bulletfont=\"times\"/>\n");
    m_body.append(
        " </LAYOUT>\n");
    m_body.append("</PARAGRAPH>\n");
}

void WinWordDoc::gotListParagraph(
    const QString &text,
    Attributes &styles)
{
    QString paragraph;
    QString xml_friendly = text;
    const PAP *pap = styles.baseStyle().getPap();
    int styleIndex = pap->istd;

    encode(xml_friendly);
    paragraph.append("<PARAGRAPH>\n<TEXT>");
    paragraph.append(xml_friendly);
    paragraph.append("</TEXT>\n");
    paragraph.append(generateFormats(styles));
    paragraph.append(
        " <LAYOUT>\n"
        "  <NAME value=\"");
    paragraph.append(m_styles.names[styleIndex]);
    paragraph.append("\"/>\n");
    paragraph.append(
        "  <FOLLOWING name=\"");
    paragraph.append(m_styles.names[styleIndex]);
    paragraph.append("\"/>\n");
    paragraph.append(
        "   <FLOW ");
    paragraph.append(justification(pap->jc));
    paragraph.append("/>\n");

    const ANLD& anld = pap->anld;

    paragraph.append(
        "  <COUNTER type=\"");
    paragraph.append(numbering(anld.nfc));
    paragraph.append("\" depth=\"");
    paragraph.append(QString::number((int)pap->ilvl));
    paragraph.append("\" bullet=\"183\" start=\"");
    paragraph.append(QString::number((int)anld.iStartAt));
    paragraph.append("\" numberingtype=\"0\" lefttext=\"");
    for (unsigned i = 0; i < anld.cxchTextBefore; i++)
        paragraph.append(anld.rgxch[i]);
    paragraph.append("\" righttext=\"");
//    kdDebug(s_area) << "WinWordDoc::gotListParagraph '" << text << "' nfc=" << anld.nfc << " cxchTextBefore=" << anld.cxchTextBefore << " cxchTextAfter=" << anld.cxchTextAfter << endl;
    for (unsigned i = anld.cxchTextBefore; i < anld.cxchTextAfter; i++)
        paragraph.append(anld.rgxch[i]);
    paragraph.append("\" bulletfont=\"symbol\"/>\n");
    paragraph.append(
        " </LAYOUT>\n");
    paragraph.append("</PARAGRAPH>\n");
//    kdDebug(s_area) << paragraph << endl;
    m_body.append(paragraph);
}

// Create a <STYLE>.
void WinWordDoc::gotStyle(
    const QString &name,
    const Properties &style)
{
#define stiNormal       0
#define stiLev1         1
#define stiLev9         9
#define stiLevFirst     stiLev1
#define stiLevLast      stiLev9

#define stiList         47
#define stiListBullet   48
#define stiListNumber   49
#define stiList2        50
#define stiList5        53
#define stiListBullet2  54
#define stiListBullet5  57
#define stiListNumber2  58
#define stiListNumber5  61
#define stiListCont     68
#define stiListCont5    72

#define isHeading(istd) \
    ((istd >= stiLev1) && (istd <= stiLev9))
#define isListAlpha(istd) \
    ((istd == stiList) || ((istd >= stiList2) && (istd <= stiList5)))
#define isListBullet(istd) \
    ((istd == stiListBullet) || ((istd >= stiListBullet2) && (istd <= stiListBullet5)))
#define isListNumber(istd) \
    ((istd == stiListNumber) || ((istd >= stiListNumber2) && (istd <= stiListNumber5)))
#define isListContination(istd) \
    ((istd >= stiListCont) && (istd <= stiListCont5))

    QString styleDef;
    int styleIndex;

    styleIndex = style.getPap()->istd;
    styleDef.append(
        "  <STYLE>\n");
    styleDef.append(
        "   <NAME value=\"");
    styleDef.append(name);
    styleDef.append("\" />\n");
    if (isHeading(styleIndex) && style.getPap()->ilvl)
    {
        // Headings are followed by normal text.
        styleDef.append(
            "   <FOLLOWING name=\"");
        styleDef.append(m_styles.names[stiNormal]);
        styleDef.append("\"/>\n");
        styleDef.append(
            "   <FLOW ");
        styleDef.append(justification(style.getPap()->jc));
        styleDef.append("/>\n");
        styleDef.append(
            "   <COUNTER numberingtype=\"1\" type=\"1\" bullet=\"45\" lefttext=\"\" bulletfont=\"\" righttext=\".\" start=\"1\" depth=\"");
        styleDef.append(QString::number(style.getPap()->ilvl));
        styleDef.append("\" customdef=\"\"/>\n");
    }
    else
    if ((isListAlpha(styleIndex) || isListBullet(styleIndex) || isListNumber(styleIndex) || isListContination(styleIndex)) && style.getPap()->ilvl)
    {
        // Note: it is quite common to encounter list paragraphs which do not
        // use one of these list styles (!!). Instead, they often seem to be
        // encoded as Normal text with a number!
        unsigned i;

        // List entries are followed by more of the same.
        const ANLD& anld = style.getPap()->anld;
        styleDef.append(
            "   <FOLLOWING name=\"");
        styleDef.append(name);
        styleDef.append("\"/>\n");
        styleDef.append(
            "   <FLOW ");
        styleDef.append(justification(style.getPap()->jc));
        styleDef.append("/>\n");
        styleDef.append(
            "   <COUNTER numberingtype=\"0\" type=\"");
        styleDef.append(numbering(anld.nfc));
        styleDef.append("\" bullet=\"45\" lefttext=\"");
        for (i = 0; i < anld.cxchTextBefore; i++)
            styleDef.append(anld.rgxch[i]);
        styleDef.append("\" bulletfont=\"\" righttext=\"");
        for (i = anld.cxchTextBefore; i < anld.cxchTextAfter; i++)
            styleDef.append(anld.rgxch[i]);
        styleDef.append("\" start=\"1\" depth=\"");
        styleDef.append(QString::number(style.getPap()->ilvl));
        styleDef.append("\" customdef=\"\"/>\n");
    }
    else
    {
        // Unnumbered text is followed by normal text.
        styleDef.append(
            "   <FOLLOWING name=\"");
        styleDef.append(m_styles.names[stiNormal]);
        styleDef.append("\"/>\n");
        styleDef.append(
            "   <FLOW ");
        styleDef.append(justification(style.getPap()->jc));
        styleDef.append("/>\n");
//        styleDef.append(
//            "   <COUNTER numberingtype=\"2\" type=\"0\" bullet=\"45\" lefttext=\"\" bulletfont=\"\" righttext=\"\" start=\"1\" depth=\"0\" customdef=\"\"/>\n");
    }
    styleDef.append(
        "   <FORMAT>\n");
    styleDef.append(generateFormat(style.getChp()));
    styleDef.append(
        "   </FORMAT>\n");
    styleDef.append(
        "  </STYLE>\n");
//    kdDebug(s_area) << styleDef<<endl;
    m_body.append(styleDef);
}

void WinWordDoc::gotTableBegin(
    unsigned tableNumber)
{
    // Add an entry for the column computation data for this table.

    m_cellEdges.resize(tableNumber);
    m_cellEdges.insert(tableNumber - 1, new QMemArray<unsigned>);
    m_body.append("<PARAGRAPH>\n<TEXT>");
    m_body.append(Document::s_anchor);
    m_body.append("</TEXT>\n");
    m_body.append("<FORMATS>\n<FORMAT id=\"6\" pos=\"0\" len=\"1\">\n");
    m_body.append("<ANCHOR type=\"frameset\" instance=\"");
    m_body.append(i18n("Table %1").arg(tableNumber));
    m_body.append("\"/>\n</FORMAT>\n</FORMATS>\n");
    m_body.append(
        " <LAYOUT>\n"
        "  <NAME value=\"");
    m_body.append(m_styles.names[0]);
    m_body.append("\"/>\n");
    m_body.append(
        " </LAYOUT>\n");
    m_body.append("</PARAGRAPH>\n");
}

void WinWordDoc::gotTableEnd(
    unsigned tableNumber)
{
    unsigned y;

    for (y = 0; y < m_table.count(); y++)
    {
        int x;
        MsWordGenerated::TAP row = m_table[y]->m_row;
        QString xml_friendly;

        // Create the XML for each cell in the row.

        for (x = 0; x < row.itcMac; x++)
        {
            QString cell;
            unsigned left;
            unsigned right;
            unsigned cellEdge;

            cell.append("<FRAMESET frameType=\"1\" frameInfo=\"0\" grpMgr=\"");
            cell.append(i18n("Table %1").arg(tableNumber));
            cell.append("\" name=\"Table");
            cell.append(QString::number(tableNumber));
            cell.append('_');
            cell.append(QString::number(y));
            left = cacheCellEdge(tableNumber, computeCellEdge(row, x));
            cell.append(',');
            cell.append(QString::number(left));
            cell.append("\" row=\"");
            cell.append(QString::number(y));
            cell.append("\" col=\"");
            cell.append(QString::number(left));
            cell.append("\" rows=\"");
            cell.append(QString::number(1));
            cell.append("\" cols=\"");

            // In cases where not all columns are present, ensure that the last
            // column spans the remainder of the table.

            if ((int)x < row.itcMac - 1)
            {
                right = cacheCellEdge(tableNumber, computeCellEdge(row, x + 1));
            }
            else
            {
                right = m_cellEdges[tableNumber - 1]->size() - 1;
            }
            cell.append(QString::number(right - left));
            cell.append("\" removeable=\"0\" visible=\"1\">\n"
                " <FRAME left=\"");
            cellEdge = m_cellEdges[tableNumber - 1]->at(left);
            cell.append(QString::number(cellEdge + 1));
            cell.append("\" right=\"");
            cellEdge = m_cellEdges[tableNumber - 1]->at(right);
            cell.append(QString::number(cellEdge - 1));
            cell.append("\" top=\"");
            cell.append(QString::number(ROW_SIZE + y * ROW_SIZE));
            cell.append("\" bottom=\"");
            cell.append(QString::number(2 * ROW_SIZE + y * ROW_SIZE));
            cell.append("\" runaround=\"0\" runaGap=\"2\"");
            cell.append(generateBorder("l", row.rgtc[x].brcLeft));
            cell.append(generateBorder("r", row.rgtc[x].brcRight));
            cell.append(generateBorder("t", row.rgtc[x].brcTop));
            cell.append(generateBorder("b", row.rgtc[x].brcBottom));

            QColor backGround = colorForNumber(QString::number(row.rgshd[x].icoBack), 8, true);
            cell.append(generateColour("bk", backGround));
            cell.append(" bleft=\"0\" bright=\"0\" btop=\"0\" bbottom=\"0\"");
            cell.append(" autoCreateNewFrame=\"0\" newFrameBehaviour=\"1\"/>\n");
            cell.append("<PARAGRAPH>\n<TEXT>");
            xml_friendly = m_table[y]->m_texts[x];
            encode(xml_friendly);
            cell.append(xml_friendly);
            cell.append("</TEXT>\n");
            Attributes *styles = m_table[y]->m_styles[x];
            const PAP *pap = styles->baseStyle().getPap();
            cell.append(generateFormats(*styles));
            cell.append(
                " <LAYOUT>\n"
                "  <NAME value=\"");
            cell.append(m_styles.names[pap->istd]);
            cell.append("\"/>\n");
            cell.append(
                " </LAYOUT>\n");
            cell.append("</PARAGRAPH>\n");
            cell.append("</FRAMESET>\n");
            m_tables.append(cell);
        }
    }
    m_table.resize(0);
}

void WinWordDoc::gotTableRow(
    unsigned tableNumber,
    const QString texts[],
    const QValueList<Attributes *> styles,
    MsWordGenerated::TAP &row)
{
    int i;
    TableRow *newRow = new TableRow(texts, styles, row);

    // Add the left and right edge of each cell to our array.

    for (i = 0; i < row.itcMac; i++)
    {
        cacheCellEdge(tableNumber, computeCellEdge(row, i));
        cacheCellEdge(tableNumber, computeCellEdge(row, i + 1));
    }
    i = m_table.count();

    // Fundamentally, MsWord provides all data on a per-row basis, but we have
    // to save it all till the end of the table so that we can work out the
    // number of cell edges in the table.
    m_table.resize(i + 1);
    m_table.insert(i, newRow);
}

QColor WinWordDoc::colorForNumber(QString number, int defaultcolor, bool defaultWhite)
{
    switch(number.toInt())
    {
	case 0:
	    if(defaultWhite)
		return QColor("white");
	case 1:
	    return QColor("black");
	case 2:
	    return QColor("blue");
	case 3:
	    return QColor("cyan");
	case 4:
	    return QColor("green");
	case 5:
	    return QColor("magenta");
	case 6:
	    return QColor("red");
	case 7:
	    return QColor("yellow");
	case 8:
	    return QColor("white");
	case 9:
	    return QColor("darkBlue");
	case 10:
	    return QColor("darkCyan");
	case 11:
	    return QColor("darkGreen");
	case 12:
	    return QColor("darkMagenta");
	case 13:
	    return QColor("darkRed");
	case 14:
	    return QColor("darkYellow");
	case 15:
	    return QColor("darkGray");
	case 16:
	    return QColor("lightGray");

	default:
	    if(defaultcolor == -1)
		return QColor("black");
	    else
		return colorForNumber(QString::number(defaultcolor), -1);
    }
}

QString WinWordDoc::justification(unsigned jc) const
{
    // Word justification codes are:
    //
    // 0 left justify
    // 1 center
    // 2 right justify
    // 3 left and right justify

    static const char *justificationTypes[4] =
    {
        "left",
        "center",
        "right",
        "justify"
    };

    if (jc > 3)
        jc = 3;
    return QString("align=\"").append(justificationTypes[jc]).append("\" ");
}

const char *WinWordDoc::list(unsigned nfc) const
{
    static const char *listStyle[6] =
    {
        "Enumerated List",
        "Enumerated List",
        "Enumerated List",
        "Alphabetical List",
        "Alphabetical List",
        "Bullet List"
    };

    if (nfc > 5)
        nfc = 5;
    return listStyle[nfc];
}

char WinWordDoc::numbering(unsigned nfc) const
{
    // Word number formats are:
    //
    // 0 Arabic numbering
    // 1 Upper case Roman
    // 2 Lower case Roman
    // 3 Upper case Letter
    // 4 Lower case letter
    // 5 Ordinal

    static char numberingTypes[6] =
    {
        '1', '5', '4', '3', '2', '6'
    };

    if (nfc > 5)
        nfc = 5;
    return numberingTypes[nfc];
}

WinWordDoc::TableRow::TableRow(
    const QString texts[],
    const QValueList<Attributes *> styles,
    MsWordGenerated::TAP &row)
{
    m_texts.clear();
    for (int i = 0; i < row.itcMac; i++)
    {
        m_texts.append(texts[i]);
    }
    m_styles = styles;
    m_row = row;
}

WinWordDoc::TableRow::~TableRow()
{
//    m_styles.setAutoDelete(true);
//    m_texts.setAutoDelete(true);
}

#include <winworddoc.moc>
