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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <koFilterManager.h>
#include <ktempfile.h>
#include <properties.h>
#include <qfile.h>
#include <qregexp.h>
#include <winworddoc.h>
#include <typeinfo>
#include <unistd.h>

#define ROW_SIZE 20

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
    m_embedded = "";
    m_cellEdges.setAutoDelete(true);
    m_table.setAutoDelete(true);
}

WinWordDoc::~WinWordDoc()
{
}

//
// Add to/lookup a cell edge in the cache of cell edges for a given table.
//

int WinWordDoc::cacheCellEdge(
    unsigned tableNumber,
    unsigned cellEdge)
{
    QArray<unsigned> *edges = m_cellEdges[tableNumber - 1];
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
    cellEdge = (unsigned)((double)cellEdge * (s_width - s_hMargin - s_hMargin) / rowWidth);
    return cellEdge + s_hMargin;
}

const bool WinWordDoc::convert()
{
    // We do the conversion in two passes, to allow all the tables to be turned into framesets
    // after the main frameset with the text.

    if (!m_isConverted)
    {
        m_body = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE DOC >\n"
            "<DOC author=\"Reginald Stadlbauer and Torben Weis\" email=\"reggie@kde.org and weis@kde.org\" editor=\"KWord\" mime=\"application/x-kword\">\n"
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
        m_body.append("\" runaround=\"1\" runaGap=\"2\" lWidth=\"1\" lRed=\"255\" lGreen=\"255\" lBlue=\"255\" lStyle=\"0\"  rWidth=\"1\" rRed=\"255\" rGreen=\"255\" rBlue=\"255\" rStyle=\"0\"  tWidth=\"1\" tRed=\"255\" tGreen=\"255\" tBlue=\"255\" tStyle=\"0\"  bWidth=\"1\" bRed=\"255\" bGreen=\"255\" bBlue=\"255\" bStyle=\"0\" bkRed=\"255\" bkGreen=\"255\" bkBlue=\"255\" bleft=\"0\" bright=\"0\" btop=\"0\" bbottom=\"0\"");
        m_body.append(" autoCreateNewFrame=\"1\" newFrameBehaviour=\"0\"/>\n");
        parse();
        m_body.append(
            "  </FRAMESET>\n");
        m_body.append(m_tables);
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
        m_result = m_body.utf8();
        m_isConverted = true;
    }
    return m_success;
}

void WinWordDoc::encode(QString &text)
{
  // When encoding the stored form of text to its run-time form,
  // be sure to do the conversion for "&amp;" to "&" first to avoid
  // accidentally converting user text into one of the other escape
  // sequences.

  text.replace(QRegExp("&"), "&amp;");
  text.replace(QRegExp("<"), "&lt;");

  // Strictly, there is no need to encode >, but we do so to for safety.

  text.replace(QRegExp(">"), "&gt;");

  // Strictly, there is no need to encode " or ', but we do so to allow
  // them to co-exist!

  text.replace(QRegExp("\""), "&quot;");
  text.replace(QRegExp("'"), "&apos;");
}

QString WinWordDoc::generateFormat(
    const CHP *chp)
{
    QString formatDef;

    formatDef.append("<COLOR red=\"0\" green=\"0\" blue=\"0\"/>\n");
    formatDef.append("<FONT name=\"times\"/>\n");
    formatDef.append("<SIZE value=\"");
    // TBD: where should we really get the size?
    formatDef.append(QString::number(chp->hps / 2));
    //formatDef.append(QString::number(12));
    formatDef.append("\"/>\n");
    if (chp->fBold)
        formatDef.append("<WEIGHT value=\"75\"/>\n");
    else
        formatDef.append("<WEIGHT value=\"50\"/>\n");
    if (chp->fItalic)
        formatDef.append("<ITALIC value=\"1\"/>\n");
    else
        formatDef.append("<ITALIC value=\"0\"/>\n");
    if (chp->kul != 0)
        formatDef.append("<UNDERLINE value=\"1\"/>\n");
    else
        formatDef.append("<UNDERLINE value=\"0\"/>\n");
    if (chp->iss != 0)
    {
        if (chp->iss == 1)
            formatDef.append("<VERTALIGN value=\"2\"/>\n");
        else
            formatDef.append("<VERTALIGN value=\"1\"/>\n");
    }
    else
    {
        formatDef.append("<VERTALIGN value=\"0\"/>\n");
    }
    return formatDef;
}

void WinWordDoc::generateFormats(
    Attributes &attributes)
{
    QString formats = "";
    Run *run;

    formats.append("<FORMATS>\n");
    run = attributes.runs.first();
    while (run)
    {
        if (typeid(Format) == typeid(*run))
        {
            const MsWordGenerated::CHP *chp = static_cast<Format *>(run)->values->getChp();

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
            Image *image = static_cast<Image *>(run);
            QString ourKey;
            QString uid;

            ourKey = "image" + QString::number(image->id) + "." + image->type;
            emit signalSavePic(
                    ourKey,
                    uid,
                    image->type,
                    image->length,
                    image->data);

            formats.append("<FORMAT id=\"2\" pos=\"");
            formats.append(QString::number(image->start));
            formats.append("\">\n");
            formats.append("<FILENAME value=\"");
            formats.append(ourKey);
            formats.append("\"/>\n");
            formats.append("</FORMAT>\n");

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
            VectorGraphic *vectorGraphic = static_cast<VectorGraphic *>(run);
            QString ourKey;
            QString uid;
            QString mimeType;
            QString filterArgs;

            // Send the picture to the outside world and get back the UID.

            ourKey = "vectorGraphic" + QString::number(vectorGraphic->id) + "." + vectorGraphic->type;
            if (vectorGraphic->type == "msod")
            {
                filterArgs = "shape-id=";
                filterArgs += QString::number(vectorGraphic->id);
                filterArgs += ";delay-stream=";
                filterArgs += QString::number((unsigned long)vectorGraphic->delay);
            }
            emit signalSavePart(
                    ourKey,
                    uid,
                    mimeType,
                    vectorGraphic->type,
                    filterArgs,
                    vectorGraphic->length,
                    vectorGraphic->data);

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
            m_embedded.append("<SETTINGS>\n");
            m_embedded.append("<FRAME left=\"30\" top=\"190\" right=\"149\" bottom=\"269\" tRed=\"0\" tGreen=\"0\" tBlue=\"0\" bRed=\"0\" bGreen=\"0\" bBlue=\"0\"/>\n");
            m_embedded.append("</SETTINGS>\n");
            m_embedded.append(
                "  </EMBEDDED>\n");
        }
        else
        if (typeid(Object) == typeid(*run))
        {
            Object *object = static_cast<Object *>(run);

            // Send the OLE id to the outside world and get back the UID.

            QString uid;
            QString mimeType;

            emit signalPart(
                QFile::encodeName("_" + QString::number(object->id)),
                uid,
                mimeType);

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
            m_embedded.append("<SETTINGS>\n");
            m_embedded.append("<FRAME left=\"30\" top=\"190\" right=\"149\" bottom=\"269\" tRed=\"0\" tGreen=\"0\" tBlue=\"0\" bRed=\"0\" bGreen=\"0\" bBlue=\"0\"/>\n");
            m_embedded.append("</SETTINGS>\n");
            m_embedded.append(
                "  </EMBEDDED>\n");
        }
        run = attributes.runs.next();
    }
    formats.append("</FORMATS>\n");
    m_body.append(formats);
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

    encode(xml_friendly);
    m_body.append("<PARAGRAPH>\n<TEXT>");
    m_body.append(xml_friendly);
    m_body.append("</TEXT>\n");
    generateFormats(styles);
    m_body.append("</PARAGRAPH>\n");
}

void WinWordDoc::gotHeadingParagraph(
    const QString &text,
    Attributes &styles)
{
    QString xml_friendly = text;

    encode(xml_friendly);
    m_body.append("<PARAGRAPH>\n<TEXT>");
    m_body.append(xml_friendly);
    m_body.append("</TEXT>\n"
        " <LAYOUT>\n"
        "  <NAME value=\"Head ");
    m_body.append(QString::number((int)styles.baseStyle.istd));
    m_body.append("\"/>\n  <COUNTER type=\"");
    m_body.append(numbering(styles.baseStyle.anld.nfc));
    m_body.append("\" depth=\"");
    m_body.append(QString::number(styles.baseStyle.istd - 1));
    m_body.append("\" bullet=\"176\" start=\"1\" numberingtype=\"1\" lefttext=\"\" righttext=\"\" bulletfont=\"times\"/>\n"
        " </LAYOUT>\n");
    generateFormats(styles);
    m_body.append("</PARAGRAPH>\n");
}

void WinWordDoc::gotListParagraph(
    const QString &text,
    Attributes &styles)
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

    QString xml_friendly = text;

    encode(xml_friendly);
    m_body.append("<PARAGRAPH>\n<TEXT>");
    m_body.append(xml_friendly);
    m_body.append("</TEXT>\n"
        " <LAYOUT>\n"
        "  <NAME value=\"");
    m_body.append(listStyle[styles.baseStyle.anld.nfc]);
    m_body.append("\"/>\n"
        "  <FOLLOWING name=\"");
    m_body.append(listStyle[styles.baseStyle.anld.nfc]);
    m_body.append("\"/>\n"
        "  <COUNTER type=\"");
    m_body.append(numbering(styles.baseStyle.anld.nfc));
    m_body.append("\" depth=\"");
    m_body.append(QString::number((int)styles.baseStyle.ilvl));
    m_body.append("\" bullet=\"183\" start=\"");
    m_body.append(QString::number((int)styles.baseStyle.anld.iStartAt));
    m_body.append("\" numberingtype=\"0\" lefttext=\"\" righttext=\"\" bulletfont=\"symbol\"/>\n"
        " </LAYOUT>\n");
    generateFormats(styles);
    m_body.append("</PARAGRAPH>\n");
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

    styleDef.append(
        "  <STYLE>\n");
    styleDef.append(
        "   <NAME value=\"");
    styleDef.append(name);
    styleDef.append("\" />\n");
    styleDef.append(
        "   <FLOW align=\"left\" />\n");
    styleIndex = style.getPap()->istd;
    if (isHeading(styleIndex))
    {
        // Headings are followed by normal text.
        styleDef.append(
            "   <FOLLOWING name=\"");
        styleDef.append(m_styles.names[stiNormal]);
        styleDef.append("\"/>\n");
        styleDef.append(
            "   <COUNTER numberingtype=\"0\" type=\"1\" bullet=\"45\" lefttext=\"\" bulletfont=\"\" righttext=\".\" start=\"1\" depth=\"");
        styleDef.append(QString::number(styleIndex - stiLev1));
        styleDef.append("\" customdef=\"\"/>\n");
    }
    else
    if (isListAlpha(styleIndex) || isListBullet(styleIndex) || isListNumber(styleIndex) || isListContination(styleIndex))
    {
        unsigned i;
        ANLD anld;

        // List entries are followed by more of the same.
        anld = style.getPap()->anld;
        styleDef.append(
            "   <FOLLOWING name=\"");
        styleDef.append(name);
        styleDef.append("\"/>\n");
        styleDef.append(
            "   <COUNTER numberingtype=\"1\" type=\"");
        styleDef.append(numbering(anld.nfc));
        styleDef.append("\" bullet=\"45\" lefttext=\"");
        for (i = 0; i < anld.cxchTextBefore; i++)
            styleDef.append(anld.rgxch[i]);
        styleDef.append("\" bulletfont=\"\" righttext=\"");
        for (i = anld.cxchTextBefore; i < anld.cxchTextAfter; i++)
            styleDef.append(anld.rgxch[i]);
        styleDef.append("\" start=\"1\" depth=\"");
        styleDef.append(QString::number(style.getPap()->ilvl - 1));
        styleDef.append("\" customdef=\"\"/>\n");
    }
    else
    {
        // Unnumbered text is followed by normal text.
        styleDef.append(
            "   <FOLLOWING name=\"");
        styleDef.append(m_styles.names[stiNormal]);
        styleDef.append("\"/>\n");
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
//    kdDebug() << styleDef<<endl;
    m_body.append(styleDef);
}

void WinWordDoc::gotTableBegin(
    unsigned tableNumber)
{
    // Add an entry for the column computation data for this table.

    m_cellEdges.resize(tableNumber);
    m_cellEdges.insert(tableNumber - 1, new QArray<unsigned>);
    m_body.append("<PARAGRAPH>\n<TEXT>");

    // This '0' will be replaced with the anchor character.

    m_body.append('0');
    m_body.append("</TEXT>\n<FORMATS>\n<FORMAT id=\"6\" pos=\"0\">\n");
    m_body.append("<ANCHOR type=\"grpMgr\" instance=\"grpmgr_");
    m_body.append(QString::number(tableNumber));
    m_body.append("\"/>\n</FORMAT>\n</FORMATS>\n</PARAGRAPH>\n");
}

void WinWordDoc::gotTableEnd(
    unsigned tableNumber)
{
    unsigned y;

    for (y = 0; y < m_table.count(); y++)
    {
        unsigned x;
        MsWordGenerated::TAP row = m_table[y]->m_row;
        QString xml_friendly;

        // Create the XML for each cell in the row.

        for (x = 0; x < row.itcMac; x++)
        {
            QString cell;
            unsigned left;
            unsigned right;
            unsigned cellEdge;

            cell.append("<FRAMESET frameType=\"1\" frameInfo=\"0\" grpMgr=\"grpmgr_");
            cell.append(QString::number(tableNumber));
            cell.append("\" row=\"");
            cell.append(QString::number(y));
            cell.append("\" col=\"");
            left = cacheCellEdge(tableNumber, computeCellEdge(row, x));
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
            cell.append(QString::number(cellEdge));
            cell.append("\" right=\"");
            cellEdge = m_cellEdges[tableNumber - 1]->at(right);
            cell.append(QString::number(cellEdge));
            cell.append("\" top=\"");
            cell.append(QString::number(ROW_SIZE + y * ROW_SIZE));
            cell.append("\" bottom=\"");
            cell.append(QString::number(2 * ROW_SIZE + y * ROW_SIZE));
            cell.append(
                "\" runaround=\"1\" runaGap=\"2\" "
                "lWidth=\"1\" lStyle=\"0\" " +
                colour(row.rgtc[x].brcLeft.ico, "lRed", "lGreen", "lBlue") +
                "rWidth=\"1\" rStyle=\"0\" " +
                colour(row.rgtc[x].brcRight.ico, "rRed", "rGreen", "rBlue") +
                "tWidth=\"1\" tStyle=\"0\" " +
                colour(row.rgtc[x].brcTop.ico, "tRed", "tGreen", "tBlue") +
                "bWidth=\"1\" bStyle=\"0\" " +
                colour(row.rgtc[x].brcBottom.ico, "bRed", "bGreen", "bBlue") +
                colour(row.rgshd[x].icoBack, "bkRed", "bkGreen", "bkBlue", 8) +
                "bleft=\"0\" bright=\"0\" btop=\"0\" bbottom=\"0\"");
            cell.append(" autoCreateNewFrame=\"0\" newFrameBehaviour=\"1\"/>\n");
            cell.append("<PARAGRAPH>\n<TEXT>");
            xml_friendly = m_table[y]->m_texts[x];
            encode(xml_friendly);
            cell.append(xml_friendly);
            cell.append("</TEXT>\n </PARAGRAPH>\n");
            cell.append("</FRAMESET>\n");
            m_tables.append(cell);
        }
    }
    m_table.resize(0);
}

void WinWordDoc::gotTableRow(
    unsigned tableNumber,
    const QVector<QString> &texts,
    QVector<Attributes> &styles,
    MsWordGenerated::TAP &row)
{
    unsigned i;
    TableRow *newRow = new TableRow(texts, styles, row);

    // Add the left and right edge of each cell to our array.

    for (i = 0; i < row.itcMac; i++)
    {
        cacheCellEdge(tableNumber, computeCellEdge(row, i));
        cacheCellEdge(tableNumber, computeCellEdge(row, i + 1));
    }
    i = m_table.count();
    m_table.resize(i + 1);
    m_table.insert(i, newRow);
}

QString WinWordDoc::colour(
    unsigned colour,
    const char *red,
    const char *green,
    const char *blue,
    unsigned defaultColour) const
{
    // Word colours are:
    //     0 Auto
    //     1 Black
    //     2 Blue
    //     3 Cyan
    //     4 Green
    //     5 Magenta
    //     6 Red
    //     7 Yellow
    //     8 White
    //     9 DkBlue
    //     10 DkCyan
    //     11 DkGreen
    //     12 DkMagenta
    //     13 DkRed
    //     14 DkYellow
    //     15 DkGray
    //     16 LtGray
    //
    // I've translated these by approximately matching the Word name
    // to an X name as indicated. I'm sure there is a more rational way
    // to do this!

    static unsigned colourTypes[17] =
    {
        0x000000, // 0 returns the default colour.
        0x000000, // 1 black
        0x0000ff, // 2 blue
        0x00ffff, // 3 cyan
        0x00ff00, // 4 green
        0xff00ff, // 5 magenta
        0xff0000, // 6 red
        0xffff00, // 7 yellow
        0xffffff, // 8 white
        0x483d8b, // 9 DarkSlateBlue
        0x008b8b, // 10 cyan4
        0x006400, // 11 DarkGreen
        0x8b008b, // 12 magenta4
        0x8b0000, // 13 red4
        0x8b8b00, // 14 yellow4
        0x3d3d3d, // 15 grey24
        0xd3d3d3  // 16 LightGrey
    };

    QString result;

    if (colour > 16)
        colour = defaultColour;
    if (colour == 0)
        colour = defaultColour;
    result += red;
    result += "=\"";
    result += QString::number((colourTypes[colour] >> 16) & 0xff);
    result += "\" ";
    result += green;
    result += "=\"";
    result += QString::number((colourTypes[colour] >> 8) & 0xff);
    result += "\" ";
    result += blue;
    result += "=\"";
    result += QString::number((colourTypes[colour]) & 0xff);
    result += "\" ";
    return result;
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

    static unsigned numberingTypes[6] =
    {
        '1', '5', '4', '3', '2', '6'
    };

    return numberingTypes[nfc];
}

WinWordDoc::TableRow::TableRow(
    const QVector<QString> &texts,
    QVector<Attributes> &styles,
    MsWordGenerated::TAP &row)
{
    m_texts.clear();
    m_styles.clear();
    for (int i = 0; i < row.itcMac; i++)
    {
        m_texts.append(*texts[i]);
        m_styles.append(*styles[i]);
    }
    m_row = row;
}

WinWordDoc::TableRow::~TableRow()
{
//    m_styles.setAutoDelete(true);
//    m_texts.setAutoDelete(true);
}

#include <winworddoc.moc>
