/* This file is part of the KDE project
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <starwriterimport.h>

#include <qdatastream.h>
#include <qiodevice.h>
#include <qstring.h>

#include <koFilterChain.h>
#include <kgenericfactory.h>

#include <kolestorage.h>

typedef KGenericFactory<StarWriterImport, KoFilter> StarWriterImportFactory;
K_EXPORT_COMPONENT_FACTORY(libstarwriterimport, StarWriterImportFactory("starwriterimport"));

// get unsigned 24-bit integer at given offset
static inline Q_UINT32 readU24(QByteArray array, Q_UINT32 p)
{
   Q_UINT8* ptr = (Q_UINT8*) array.data();
   return (Q_UINT32) (ptr[p] + (ptr[p+1] << 8) + (ptr[p+2] << 16));
}

StarWriterImport::StarWriterImport(KoFilter *, const char *, const QStringList&) : KoFilter(), hasHeader(false), hasFooter(false)
{
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

    // Read the streams
    KOLE::Storage storage;
    storage.open(m_chain->inputFile());

    QDataStream* stream;

    stream = storage.stream("StarWriterDocument");
    if (!stream) return KoFilter::WrongFormat;
    StarWriterDocument.resize(stream->device()->size());
    stream->readRawBytes(StarWriterDocument.data(), StarWriterDocument.size());

    stream = storage.stream("SwPageStyleSheets");
    if (!stream) return KoFilter::WrongFormat;
    SwPageStyleSheets.resize(stream->device()->size());
    stream->readRawBytes(SwPageStyleSheets.data(), SwPageStyleSheets.size());

    // Check the document version
    if (!checkDocumentVersion()) return KoFilter::NotImplemented;

    // Algorithm for creating the main document
    if (!addBody()) return KoFilter::ParsingError;
    if (!addHeaders()) return KoFilter::ParsingError;
    if (!addFooters()) return KoFilter::ParsingError;
    if (!addStyles()) return KoFilter::ParsingError;
    if (!addPageProperties()) return KoFilter::ParsingError;
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
    Q_UINT16 flags = StarWriterDocument[0x0A] + 0x0100*StarWriterDocument[0x0D];
    if (flags & 0x0008) return false;

    return true;
}

bool StarWriterImport::addKWordHeader()
{
    // Proper prolog and epilog
    QString prolog = "<!DOCTYPE DOC>\n";
    prolog.append("<DOC mime=\"application/x-kword\" syntaxVersion=\"2\" editor=\"KWord\">\n");
    prolog.append("<PAPER width=\"595\" height=\"841\" format=\"1\" fType=\"0\" orientation=\"0\" hType=\"0\" columns=\"1\">\n");
    prolog.append(" <PAPERBORDERS left=\"36\" right=\"36\" top=\"36\" bottom=\"36\" />\n");
    prolog.append("</PAPER>\n");
    prolog.append("<ATTRIBUTES standardpage=\"1\" unit=\"mm\" hasFooter=\"0\" hasHeader=\"0\" processing=\"0\" />\n");
    prolog.append("<FRAMESETS>\n");
    prolog.append("<FRAMESET removable=\"0\" frameType=\"1\" frameInfo=\"0\" autoCreateNewFrame=\"1\">\n");
    prolog.append("<FRAME right=\"567\" left=\"28\" top=\"42\" bottom=\"799\" />\n");

    QString epilog = "</FRAMESET>\n";
    epilog.append("</FRAMESETS>\n");
    epilog.append("</DOC>");

    maindoc.prepend(prolog);
    maindoc.append(epilog);

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
    // FIXME: is this the right frame name?
    QByteArray data(len);
    for (Q_UINT32 k=0; k<len; k++)
      data[k] = StarWriterDocument[p+k];
    return parseNodes(data, "Text Frameset");
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

bool StarWriterImport::parseNodes(QByteArray n, QString caption)
{
    // textTable and textGraphics are not necessary
    QByteArray s;

    // Loop
    Q_UINT32 p = 0x09;   // is it a fixed value? is it the same for headers/footers?

    while (p < n.size()) {
        char c = n[p];
        Q_UINT32 len = readU24(n, p+1);

        s.resize(len);
        for (Q_UINT32 k = 0x00; k < len; k++)
            s[k] = n[p+k];

        switch (c) {
            case 'T':
                if (!parseText(s, caption)) return false;
                break;
            case 'E':
                if (!parseTable(s)) return false;
                break;
            case 'G':
                if (!parseGraphics(s)) return false;
                break;
            default:
                break;
        };
        p += len;
    };

    return true;
}

bool StarWriterImport::parseText(QByteArray n, QString caption)
{
    QByteArray s;
    Q_UINT32 len;
    QString paragraph;

    // Preliminary check
    if (n[0x00] != 'T') return false;

    // Retrieve the paragraph (text-only)
    len = readU24(n, 0x09) & 0xFFFF;
    s.resize(len);
    for (Q_UINT32 k = 0x00; k < len; k++)
        s[k] = n[0x0B+k];

    // Write it to the variable
    QString text = convertToKWordString(s);
    maindoc.append("<PARAGRAPH>\n");
    maindoc.append("<TEXT>" + text + "</TEXT>\n");
    maindoc.append("</PARAGRAPH>\n");

    return true;
}

bool StarWriterImport::parseTable(QByteArray n)
{
    return (n[0x00] == 'E');
};

bool StarWriterImport::parseGraphics(QByteArray n)
{
    return (n[0x00] == 'G');
}

#include <starwriterimport.moc>
