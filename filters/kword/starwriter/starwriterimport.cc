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

typedef KGenericFactory<StarWriter5Import, KoFilter> StarWriter5ImportFactory;
K_EXPORT_COMPONENT_FACTORY(libstarwriter5import, StarWriter5ImportFactory("starwriter5import"));

// get unsigned 24-bit integer at given offset
static inline Q_UINT32 readU24(QByteArray array, unsigned p)
{
   Q_UINT8* ptr = (Q_UINT8*) array.data();
   return (Q_UINT32) (ptr[p] + (ptr[p+1] << 8) + (ptr[p+2] << 16));
}

StarWriter5Import::StarWriter5Import(KoFilter *, const char *, const QStringList&) : KoFilter(), hasHeader(false), hasFooter(false)
{
}

StarWriter5Import::~StarWriter5Import()
{
}

KoFilter::ConversionStatus StarWriter5Import::convert(const QCString& from, const QCString& to)
{
    // Check for proper conversion
    if ((to != "application/x-kword") || (from != "application/vnd.stardivision.writer"))
        return KoFilter::NotImplemented;

    KOLE::Storage storage;
    storage.open(m_chain->inputFile());

    QDataStream* stream;

    stream = storage.stream("StarWriterDocument");
    if (!stream)
        return KoFilter::WrongFormat;

    StarWriterDocument.resize(stream->device()->size());
    stream->readRawBytes(StarWriterDocument.data(), StarWriterDocument.size());

    stream = storage.stream("SwPageStyleSheets");
    if (!stream)
        return KoFilter::WrongFormat;

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
    if (out)
    {
        QCString cstring = maindoc.utf8();
        cstring.prepend("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        out->writeBlock((const char*) cstring, cstring.length());
    }

    return KoFilter::OK;
}

// This should always return true,
// because StarWriter non-5.x files have a different mime-type...
bool StarWriter5Import::checkDocumentVersion()
{
    if(SwPageStyleSheets.size() < 6) return false;
    if(StarWriterDocument.size() < 6) return false;

    if(SwPageStyleSheets[0x00] != 'S') return false;
    if(SwPageStyleSheets[0x01] != 'W') return false;
    if(SwPageStyleSheets[0x02] != '5') return false;
    if(SwPageStyleSheets[0x03] != 'H') return false;
    if(SwPageStyleSheets[0x04] != 'D') return false;
    if(SwPageStyleSheets[0x05] != 'R') return false;

    if(StarWriterDocument[0x00] != 'S') return false;
    if(StarWriterDocument[0x01] != 'W') return false;
    if(StarWriterDocument[0x02] != '5') return false;
    if(StarWriterDocument[0x03] != 'H') return false;
    if(StarWriterDocument[0x04] != 'D') return false;
    if(StarWriterDocument[0x05] != 'R') return false;

    return true;
}

bool StarWriter5Import::addKWordHeader()
{
    // Proper prolog and epilog
    QString  prolog = "<!DOCTYPE DOC>\n";
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

    return true;   // is it really useful?
}

bool StarWriter5Import::addPageProperties()
{
    return true;   // is it really useful?
}

// FIXME: written just for compiling needs...
bool StarWriter5Import::addStyles()
{
    return true;   // is it really useful?
}

// FIXME: written just for compiling needs...
// 1. search for the right starting point
// 2. determine the length
// 3. parse everything with parseNodes()
bool StarWriter5Import::addHeaders()
{
    return true;   // is it really useful?
}

// FIXME: same as before
bool StarWriter5Import::addFooters()
{
    return true;   // is it really useful?
}

bool StarWriter5Import::addBody()
{
    // Find the starting point, by: 
    // 1. skipping the header       
    int len = StarWriterDocument[7];
    int p = len;

    // 2. skipping 8 more bytes if necessary    
    p += 8;

    // 3. skipping useless sections 
    char c = StarWriterDocument[p];
    while (c != 'N') {
        len = readU24( StarWriterDocument, p+1 );
        p += len;
        c = StarWriterDocument[p];
    };   // there is at least one empty paragraph!

    // Select nodes and pass them to parseNodes() 
    len = readU24( StarWriterDocument, p+1 );

    // FIXME: is this the right frame name?
    QByteArray data( len );
    for( unsigned k=0; k<len; k++ )
      data[k] = StarWriterDocument[p+k];
    return parseNodes( data, "Text Frameset");   // is it really useful?
}

QString StarWriter5Import::convertToKWordString( QByteArray s)
{
    QString result;

    for ( unsigned i = 0; i < s.size(); i++)
        if (s[i] == '&') result += "&amp;";
        else if (s[i] == '<') result += "&lt;";
        else if (s[i] == '>') result += "&gt;";
        else if (s[i] == '"') result += "&quot;";
        else if (s[i] == 39) result += "&apos;";
        else if (s[i] == 9) result += "\t";
        // FIXME: more to add here
        //        (manual breaks, soft-hyphens, non-breaking spaces, variables)
        else result += QChar( s[i] );

    return result;
}

bool StarWriter5Import::parseNodes( QByteArray n, QString caption )
{
    // textTable and textGraphics are not necessary
    QByteArray s;

    // Loop
    int p = 0x09;   // is it a fixed value? is it the same for headers/footers?

    while (p < n.size()) {
        char c = n[p];
        int len = readU24(n, p+1);

        s.resize(len);
        for (unsigned k = 0x00; k < len; k++)
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

bool StarWriter5Import::parseText(QByteArray n, QString caption)
{
    QByteArray s;
    int len;
    QString paragraph;

    // Preliminary check
    if (n[0x00] != 'T') return false;

    // Retrieve the paragraph (text-only)
    len = readU24(n, 0x09) & 0xFFFF;
    s.resize(len);
    for (unsigned k = 0x00; k < len; k++)
        s[k] = n[0x0B+k];

    // Write it to the variable
    QString text = convertToKWordString(s);
    maindoc.append("<PARAGRAPH>\n");
    maindoc.append("<TEXT>" + text + "</TEXT>\n");
    maindoc.append("</PARAGRAPH>\n");

    return true;
}

// FIXME: written just for compiling needs...
bool StarWriter5Import::parseTable(QByteArray n)
{
    return (n[0x00] == 'E');
};

// FIXME: written just for compiling needs...
bool StarWriter5Import::parseGraphics(QByteArray n)
{
    return (n[0x00] == 'G');
}

#include <starwriterimport.moc>
