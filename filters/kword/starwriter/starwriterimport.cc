/*
   This file is part of the KDE project
   Copyright (C) 2002 Marco Zanon <info@marcozanon.com>
                  and Ariya Hidayat <ariya@kde.org>

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

#include <starwriterimport.h>

#include <qdatastream.h>
#include <qiodevice.h>
#include <qstring.h>

#include <KoFilterChain.h>
#include <kgenericfactory.h>

#include "pole.h"

typedef KGenericFactory<StarWriterImport, KoFilter> StarWriterImportFactory;
K_EXPORT_COMPONENT_FACTORY(libstarwriterimport, StarWriterImportFactory("kofficefilters"))

// Get unsigned 24-bits integer at given offset
static inline Q_UINT32 readU24(QByteArray array, Q_UINT32 p)
{
   Q_UINT8* ptr = (Q_UINT8*) array.data();
   return (Q_UINT32) (ptr[p] + (ptr[p+1] << 8) + (ptr[p+2] << 16));
}

// Get unsigned 16-bits integer at given offset
static inline Q_UINT16 readU16(QByteArray array, Q_UINT32 p)
{
   Q_UINT8* ptr = (Q_UINT8*) array.data();
   return (Q_UINT16) (ptr[p] + (ptr[p+1] << 8));
}

StarWriterImport::StarWriterImport(KoFilter *, const char *, const QStringList&) : KoFilter()
{
    hasHeader = false;
    hasFooter = false;
    tablesNumber = 1;
}

StarWriterImport::~StarWriterImport()
{
}

KoFilter::ConversionStatus StarWriterImport::convert(const QCString& from, const QCString& to)
{
    // Check for proper conversion
    // When 4.x is supported, use also: || (from != "application/x-starwriter")
    if ((to != "application/x-kword") || (from != "application/vnd.stardivision.writer"))
        return KoFilter::NotImplemented;

    // Read streams
    POLE::Storage storage;
    storage.open(m_chain->inputFile().latin1());

    POLE::Stream* stream;

    stream = storage.stream("StarWriterDocument");
    if (!stream) return KoFilter::WrongFormat;
    StarWriterDocument.resize(stream->size());
    stream->read((unsigned char*)StarWriterDocument.data(), StarWriterDocument.size());
    delete stream;

    stream = storage.stream("SwPageStyleSheets");
    if (!stream) return KoFilter::WrongFormat;
    SwPageStyleSheets.resize(stream->size());
    stream->read((unsigned char*)SwPageStyleSheets.data(), SwPageStyleSheets.size());
    delete stream;

    // Check document version
    if (!checkDocumentVersion()) return KoFilter::WrongFormat;

    // Algorithm for creating the main document
    if (!addBody()) return KoFilter::ParsingError;
    if (!addHeaders()) return KoFilter::ParsingError;
    if (!addFooters()) return KoFilter::ParsingError;
    if (!addStyles()) return KoFilter::ParsingError;
    if (!addPageProperties()) return KoFilter::ParsingError;
    maindoc = bodyStuff + tablesStuff + picturesStuff;   // + lots of other things :)

    if (!addKWordHeader()) return KoFilter::ParsingError;

    // Prepare storage device and return
    KoStoreDevice *out = m_chain->storageFile("maindoc.xml", KoStore::Write);
    if (out) {
        QCString cstring = maindoc.utf8();
        cstring.prepend("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        out->writeBlock((const char*) cstring, cstring.length());
    }

    return KoFilter::OK;
}

bool StarWriterImport::checkDocumentVersion()
{
    if (SwPageStyleSheets.size() < 0x0B) return false;
    if (StarWriterDocument.size() < 0x0B) return false;

    if (SwPageStyleSheets[0x00] != 'S') return false;
    if (SwPageStyleSheets[0x01] != 'W') return false;
    // When 4.x is supported use also: || (SwPageStyleSheets[0x02] != '4')
    if (SwPageStyleSheets[0x02] != '5') return false;
    if (SwPageStyleSheets[0x03] != 'H') return false;
    if (SwPageStyleSheets[0x04] != 'D') return false;
    if (SwPageStyleSheets[0x05] != 'R') return false;

    if (StarWriterDocument[0x00] != 'S') return false;
    if (StarWriterDocument[0x01] != 'W') return false;
    // When 4.x is supported use also: || (StarWriterDocument[0x02] != '4')
    if (StarWriterDocument[0x02] != '5') return false;
    if (StarWriterDocument[0x03] != 'H') return false;
    if (StarWriterDocument[0x04] != 'D') return false;
    if (StarWriterDocument[0x05] != 'R') return false;

    // Password-protection is not supported for the moment
    Q_UINT16 flags = readU16(StarWriterDocument, 0x0A);
    if (flags & 0x0008) return false;

    return true;
}

bool StarWriterImport::addKWordHeader()
{
    // Proper prolog and epilog
    QString prolog;
    prolog = "<!DOCTYPE DOC>\n";
    prolog.append("<DOC mime=\"application/x-kword\" syntaxVersion=\"2\" editor=\"KWord\">\n");
    prolog.append("<PAPER width=\"595\" height=\"841\" format=\"1\" fType=\"0\" orientation=\"0\" hType=\"0\" columns=\"1\">\n");
    prolog.append(" <PAPERBORDERS left=\"36\" right=\"36\" top=\"36\" bottom=\"36\" />\n");
    prolog.append("</PAPER>\n");
    prolog.append("<ATTRIBUTES standardpage=\"1\" hasFooter=\"0\" hasHeader=\"0\" processing=\"0\" />\n");
    prolog.append("<FRAMESETS>\n");

    maindoc.prepend(prolog);

    maindoc.append("</FRAMESETS>\n");
    maindoc.append("</DOC>");

    return true;
}

bool StarWriterImport::addPageProperties()
{
    return true;
}

bool StarWriterImport::addStyles()
{
    return true;
}

// FIXME
// 1. search for the right starting point
// 2. determine the length
// 3. parse everything with parseNodes()
bool StarWriterImport::addHeaders()
{
    return true;
}

bool StarWriterImport::addFooters()
{
    return true;
}

bool StarWriterImport::addBody()
{
    // Find the starting point, by:
    // 1. skipping the header
    Q_UINT32 len = StarWriterDocument[0x07];
    Q_UINT32 p = len;

    // 2. skipping 8 more bytes
    p += 0x08;

    // 3. skipping useless sections
    char c = StarWriterDocument[p];
    while (c != 'N') {
        len = readU24(StarWriterDocument, p+1);
        p += len;
        c = StarWriterDocument[p];
    };   // there is at least one empty paragraph!

    // Select nodes and pass them to parseNodes()
    len = readU24(StarWriterDocument, p+1);
    QByteArray data(len);
    for (Q_UINT32 k=0; k<len; k++)
      data[k] = StarWriterDocument[p+k];
    bool retval = parseNodes(data);

    // add proper tags
    bodyStuff.prepend(" <FRAME right=\"567\" left=\"28\" top=\"42\" bottom=\"799\" />\n");
    bodyStuff.prepend(" <FRAMESET removable=\"0\" frameType=\"1\" frameInfo=\"0\" name=\"Text Frameset 1\" autoCreateNewFrame=\"1\">\n");
    bodyStuff.append(" </FRAMESET>\n");

    return retval;
}

QString StarWriterImport::convertToKWordString(QByteArray s)
{
    QString result;

    for (Q_UINT32 i = 0x00; i < s.size(); i++)
        if (s[i] == '&') result += "&amp;";
        else if (s[i] == '<') result += "&lt;";
        else if (s[i] == '>') result += "&gt;";
        else if (s[i] == '"') result += "&quot;";
        else if (s[i] == 0x27) result += "&apos;";
        else if (s[i] == 0x09) result += "\t";
        // FIXME: more to add here
        //        (manual breaks, soft-hyphens, non-breaking spaces, variables)
        else result += QChar(s[i]);

    return result;
}

bool StarWriterImport::parseNodes(QByteArray n)
{
    QByteArray s;
    Q_UINT32 len, p;

    // Loop
    p = 0x09;   // is this a fixed value? is it the same for headers/footers?

    while (p < n.size()) {
        char c = n[p];
        len = readU24(n, p+1);

        s.resize(len);
        for (Q_UINT32 k = 0x00; k < len; k++)
            s[k] = n[p+k];

        switch (c) {
            case 'T':
                //if ((s[0x0A] == 0x01) && (s[0x0B] == 0x00) && (s[0x0C] == 0xFF)) {
                //    if (!parseGraphics(s)) return false;
                //}
                //else {
                    if (!parseText(s)) return false;
                //}
                break;
            case 'E':
                if (!parseTable(s)) return false;
                break;
            default:
                break;
        };
        p += len;
    };

    return true;
}

bool StarWriterImport::parseText(QByteArray n)
{
    QByteArray s;
    Q_UINT16 len;
    Q_UINT32 p;
    QString text;
    // Q_UINT16 attributeStart, attributeEnd, formatPos, formatLen;
    // QString pAttributes, cAttributes, tempCAttributes;
    // QStringList cAttributesList;

    // Retrieve the paragraph (text-only)
    len = readU16(n, 0x09);
    s.resize(len);
    for (Q_UINT16 k = 0x00; k < len; k++)
        s[k] = n[0x0B+k];

    /*
    // Retrieve paragraph and character attributes
    P = len;
    while (n[p] == 'S') {
        p += 0x04;
        // parse 'A' sub-sections and write to pAttributes
        // FIXME: all this part
        // get section length
        // if (length > ...) {
        //    if (special characters found) {
        //        get start, len, type
        //        write to the list
        //    }
        // }
        // increment p
    }
    while (n[p] == 'A') {
        // parse 'A' sections and fill cAttributesList
        // FIXME: similar as above
    }
    // Parse list
    while ((!cAttributesList.isEmpty()) || (formatPos < len)) {
        formatLen = 65535;
        // FIXME: point to first list item
        while (FIXME: current list item < last list item) {
            // FIXME: get item members
            if ((attributeStart <= formatPos) && (formatPos <= attributeEnd)) {   // this attribute has to be considered
                // FIXME: write attribute to tempCAttributes
                formatLen = min(formatLen, (attributeStart - attributeEnd));
            }
            else if (attributeLen < formatPos) [   // this attribute has to be removed
                // FIXME: remove list item
            }
            // FIXME: point to next list item
        }
        // FIXME: copy tempCAttributes to cAttributes
        formatPos += formatLen;
    }
    */

    // Write everything to the variable
    text = convertToKWordString(s);
    bodyStuff.append("  <PARAGRAPH>\n");
    bodyStuff.append("   <TEXT xml:space=\"preserve\">" + text + "</TEXT>\n");
    // FIXME: add FORMATS for pAttributes and cAttributes
    bodyStuff.append("  </PARAGRAPH>\n");

    return true;
}

bool StarWriterImport::parseTable(QByteArray n)
{
/*
    QByteArray s;
    Q_UINT32 len, len2;
    Q_UINT16 len3;
    Q_UINT32 p, p2;
    QString text;
    QString tableCell, tableText, tableName;
    Q_UINT8 row, column;

    // Set table name
    tableName = QString("Table %1").arg(tablesNumber);
    tablesNumber++;

    // Skip useless sections and retrieve the right point
    p = 0x13;
    while (n[p] != 'L') {
        len = readU24(n, p+1);
        p += len;
    }

    row = 0;

    // Read rows
    while (n[p] == 'L') {
        column = 0;

        // Find the first 't'
        while (n[p] != 't') p++;

        // Read cells
        while (n[p] == 't') {
            // Get cell length
            len2 = readU24(n, p+1);
            p2 = p + len2;

            // Find the 'T' section
            while (n[p] != 'T') p++;

            // Get cell text/value
            len3 = readU16(n, p+0x09);
            s.resize(len3);
            for (Q_UINT16 k = 0x00; k < len3; k++)
                s[k] = n[p+0x0B+k];
            text = convertToKWordString(s);

            // FIXME: check this stuff
            QString frameName = QString("%1 Cell %2,%3").arg(tableName).arg(row).arg(column);
            tableText.append(QString(" <FRAMESET name=\"%1\" frameType=\"1\" frameInfo=\"0\" removable=\"0\" visible=\"1\" grpMgr=\"%2\" row=\"%3\" col=\"%4\" rows=\"1\" cols=\"1\" protectSize=\"0\">\n").arg(frameName).arg(tableName).arg(row).arg(column));
            tableText.append(" <FRAME runaround=\"1\" copy=\"0\" newFrameBehavior=\"1\" runaroundSide=\"biggest\" autoCreateNewFrame=\"0\" bleftpt=\"2.8\" brightpt=\"2.8\" btoppt=\"2.8\" bbottompt=\"2.8\" runaroundGap=\"2.8\" />\n");
            tableText.append("  <PARAGRAPH>\n");
            tableText.append("   <TEXT xml:space=\"preserve\">" + text + "</TEXT>\n");
            tableText.append("  </PARAGRAPH>\n");
            tableText.append(" </FRAMESET>\n");

            // Skip other sections or bytes
            p = p2;

            // Increase column pointers
            column++;
        }

        // Increase row pointer
        row++;
    }

    // Add everything to tablesStuff
    tablesStuff.append(tableText);

    // Add anchor to bodyStuff
    bodyStuff.append("  <PARAGRAPH>\n");
    bodyStuff.append("   <TEXT xml:space=\"preserve\">#</TEXT>\n");
    bodyStuff.append("   <FORMATS>\n");
    bodyStuff.append("    <FORMAT id=\"6\" pos=\"0\" len=\"1\">\n");
    bodyStuff.append(QString("    <ANCHOR type=\"frameset\" instance=\"%1\" />\n").arg(tableName));
    bodyStuff.append("    </FORMAT>\n");
    bodyStuff.append("   </FORMATS>\n");
    bodyStuff.append("  </PARAGRAPH>\n");

*/
    return true;
}

bool StarWriterImport::parseGraphics(QByteArray n)
{
    return true;
}

#include <starwriterimport.moc>
