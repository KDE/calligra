/* This file is part of the KDE project
   Copyright (C) 2001-2005 Ariya Hidayat <ariya@kde.org>

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

#include <kdebug.h>
#include <KoFilterChain.h>
#include <kpluginfactory.h>

#include <wpimport.h>

#include <QByteArray>

K_PLUGIN_FACTORY(WPImportFactory, registerPlugin<WPImport>();)
K_EXPORT_PLUGIN(WPImportFactory("calligrafilters"))

#include <libwpd/libwpd.h>
#ifndef LIBWPD_VERSION_MINOR
#define LIBWPD_VERSION_MINOR 8
#endif

#if LIBWPD_VERSION_MINOR>8
#include <libwpd-stream/libwpd-stream.h>
#else
#include <libwpd/WPXStream.h>
#include <libwpd/WPXHLListenerImpl.h>
#endif


#if LIBWPD_VERSION_MINOR>8
class WPXMemoryInputStream : public WPXInputStream
{
public:
    WPXMemoryInputStream(unsigned char *data, unsigned long size);
    virtual ~WPXMemoryInputStream();

    virtual bool isOLEStream() {
        return false;
    }
    virtual WPXInputStream * getDocumentOLEStream(const char *name) {
        return NULL;
    }

    const virtual unsigned char *read(unsigned long numBytes, unsigned long &numBytesRead);
    virtual int seek(long offset, WPX_SEEK_TYPE seekType);
    virtual long tell();
    virtual bool atEOS();

private:
    long m_offset;
    size_t m_size;
    unsigned char *m_data;
};

WPXMemoryInputStream::WPXMemoryInputStream(unsigned char *data, unsigned long size) :
	WPXInputStream(),
	m_offset(0),
	m_size(size),
	m_data(data)
{
}

WPXMemoryInputStream::~WPXMemoryInputStream()
{
}

const unsigned char * WPXMemoryInputStream::read(unsigned long numBytes, unsigned long &numBytesRead)
{
	numBytesRead = 0;

	if (numBytes == 0)
		return 0;
	
	int numBytesToRead;

	if ((m_offset+numBytes) < m_size)
		numBytesToRead = numBytes;
	else
		numBytesToRead = m_size - m_offset;
	
	numBytesRead = numBytesToRead; // about as paranoid as we can be..

	if (numBytesToRead == 0)
		return 0;

	long oldOffset = m_offset;
	m_offset += numBytesToRead;
	
	return &m_data[oldOffset];
}

int WPXMemoryInputStream::seek(long offset, WPX_SEEK_TYPE seekType)
{
	if (seekType == WPX_SEEK_CUR)
		m_offset += offset;
	else if (seekType == WPX_SEEK_SET)
		m_offset = offset;

	if (m_offset < 0)
	{
		m_offset = 0;
		return 1;
	}
	if ((long)m_offset > (long)m_size)
	{
		m_offset = m_size;
		return 1;
	}

	return 0;
}

long WPXMemoryInputStream::tell()
{
	return m_offset;
}

bool WPXMemoryInputStream::atEOS()
{
	if ((long)m_offset == (long)m_size) 
		return true; 

	return false;
}
#else
class WPXMemoryInputStream : public WPXInputStream
{
public:
    WPXMemoryInputStream(uint8_t *data, size_t size);
    virtual ~WPXMemoryInputStream();

    virtual bool isOLEStream() {
        return false;
    }
    virtual WPXInputStream * getDocumentOLEStream() {
        return NULL;
    }

    const virtual uint8_t *read(size_t numBytes, size_t &numBytesRead);
    virtual int seek(long offset, WPX_SEEK_TYPE seekType);
    virtual long tell();
    virtual bool atEOS();

private:
    long m_offset;
    size_t m_size;
    uint8_t *m_data;
    uint8_t *m_tmpBuf;
};


WPXMemoryInputStream::WPXMemoryInputStream(uint8_t *data, size_t size) :
        WPXInputStream(false),
        m_offset(0),
        m_size(size),
        m_data(data),
        m_tmpBuf(NULL)
{
}

WPXMemoryInputStream::~WPXMemoryInputStream()
{
    delete [] m_tmpBuf;
    delete [] m_data;
}

const uint8_t * WPXMemoryInputStream::read(size_t numBytes, size_t &numBytesRead)
{
    delete [] m_tmpBuf;
    int numBytesToRead;

    if ((m_offset + numBytes) < m_size)
        numBytesToRead = numBytes;
    else
        numBytesToRead = m_size - m_offset;

    numBytesRead = numBytesToRead; // about as paranoid as we can be..

    if (numBytesToRead == 0)
        return NULL;

    m_tmpBuf = new uint8_t[numBytesToRead];
    for (size_t i = 0; i < numBytesToRead; i++) {
        m_tmpBuf[i] = m_data[m_offset];
        m_offset++;
    }

    return m_tmpBuf;
}

int WPXMemoryInputStream::seek(long offset, WPX_SEEK_TYPE seekType)
{
    if (seekType == WPX_SEEK_CUR)
        m_offset += offset;
    else if (seekType == WPX_SEEK_SET)
        m_offset = offset;

    if (m_offset < 0)
        m_offset = 0;
    else if (m_offset >= m_size)
        m_offset = m_size;

    return 0;
}

long WPXMemoryInputStream::tell()
{
    return m_offset;
}

bool WPXMemoryInputStream::atEOS()
{
    if (m_offset >= m_size)
        return true;

    return false;
}
#endif

#if LIBWPD_VERSION_MINOR>8
class WordsListener : public WPXDocumentInterface
#else
class WordsListener : public WPXHLListenerImpl
#endif
{
public:
    WordsListener();
    virtual ~WordsListener();
#if LIBWPD_VERSION_MINOR>8
	virtual void setDocumentMetaData(const WPXPropertyList &propList) {};
	virtual void startDocument();
	virtual void endDocument();
	virtual void definePageStyle(const WPXPropertyList &propList) {};
	virtual void openPageSpan(const WPXPropertyList &propList) {};
	virtual void closePageSpan() {};
	virtual void openHeader(const WPXPropertyList &propList) {};
	virtual void closeHeader() {};
	virtual void openFooter(const WPXPropertyList &propList) {};
	virtual void closeFooter() {};
	virtual void defineParagraphStyle(const WPXPropertyList &propList, const WPXPropertyListVector &tabStops) {};
	virtual void openParagraph(const WPXPropertyList &propList, const WPXPropertyListVector &tabStops);
	virtual void closeParagraph();
	virtual void defineCharacterStyle(const WPXPropertyList &propList) {};
	virtual void openSpan(const WPXPropertyList &propList);
	virtual void closeSpan();
	virtual void defineSectionStyle(const WPXPropertyList &propList, const WPXPropertyListVector &columns) {};
	virtual void openSection(const WPXPropertyList &propList, const WPXPropertyListVector &columns) {};
	virtual void closeSection() {};
	virtual void insertTab();
	virtual void insertSpace() {};
	virtual void insertText(const WPXString &text);
 	virtual void insertLineBreak();
	virtual void insertField(const WPXString &type, const WPXPropertyList &propList) {};
	virtual void defineOrderedListLevel(const WPXPropertyList &propList) {};
	virtual void defineUnorderedListLevel(const WPXPropertyList &propList) {};	
	virtual void openOrderedListLevel(const WPXPropertyList &propList) {};
	virtual void openUnorderedListLevel(const WPXPropertyList &propList) {};
	virtual void closeOrderedListLevel() {};
	virtual void closeUnorderedListLevel() {};
	virtual void openListElement(const WPXPropertyList &propList, const WPXPropertyListVector &tabStops) {};
	virtual void closeListElement() {};       
	virtual void openFootnote(const WPXPropertyList &propList) {};
	virtual void closeFootnote() {};
	virtual void openEndnote(const WPXPropertyList &propList) {};
	virtual void closeEndnote() {};
	virtual void openComment(const WPXPropertyList &propList) {};
	virtual void closeComment() {};
	virtual void openTextBox(const WPXPropertyList &propList) {};
	virtual void closeTextBox() {};
 	virtual void openTable(const WPXPropertyList &propList, const WPXPropertyListVector &columns) {};
 	virtual void openTableRow(const WPXPropertyList &propList) {};
	virtual void closeTableRow() {};
 	virtual void openTableCell(const WPXPropertyList &propList) {};
	virtual void closeTableCell() {};
	virtual void insertCoveredTableCell(const WPXPropertyList &propList) {};
 	virtual void closeTable() {};
	virtual void openFrame(const WPXPropertyList &propList) {};
	virtual void closeFrame() {};
	virtual void insertBinaryObject(const WPXPropertyList &propList, const WPXBinaryData &data) {};
	virtual void insertEquation(const WPXPropertyList &propList, const WPXString &data) {};
#else

    virtual void setDocumentMetaData(const WPXPropertyList &propList) {}

    virtual void startDocument() ;
    virtual void endDocument() ;

    virtual void openPageSpan(const WPXPropertyList &propList) {}
    virtual void closePageSpan() {}
    virtual void openHeader(const WPXPropertyList &propList) {}
    virtual void closeHeader() {}
    virtual void openFooter(const WPXPropertyList &propList) {}
    virtual void closeFooter() {}

    virtual void openSection(const WPXPropertyList &propList, const WPXPropertyListVector &columns) {}
    virtual void closeSection() {}
    virtual void openParagraph(const WPXPropertyList &propList, const WPXPropertyListVector &tabStops);
    virtual void closeParagraph();
    virtual void openSpan(const WPXPropertyList &propList) ;
    virtual void closeSpan() ;

    virtual void insertTab();
    virtual void insertText(const WPXString &text);
    virtual void insertLineBreak();

    virtual void defineOrderedListLevel(const WPXPropertyList &propList) {}
    virtual void defineUnorderedListLevel(const WPXPropertyList &propList) {}
    virtual void openOrderedListLevel(const WPXPropertyList &propList) {}
    virtual void openUnorderedListLevel(const WPXPropertyList &propList) {}
    virtual void closeOrderedListLevel() {}
    virtual void closeUnorderedListLevel() {}
    virtual void openListElement(const WPXPropertyList &propList, const WPXPropertyListVector &tabStops) {}
    virtual void closeListElement() {}

    virtual void openFootnote(const WPXPropertyList &propList) {}
    virtual void closeFootnote() {}
    virtual void openEndnote(const WPXPropertyList &propList) {}
    virtual void closeEndnote() {}

    virtual void openTable(const WPXPropertyList &propList, const WPXPropertyListVector &columns) {}
    virtual void openTableRow(const WPXPropertyList &propList) {}
    virtual void closeTableRow() {}
    virtual void openTableCell(const WPXPropertyList &propList) {}
    virtual void closeTableCell() {}
    virtual void insertCoveredTableCell(const WPXPropertyList &propList) {}
    virtual void closeTable() {}
#endif

    QString root;

private:
    unsigned int m_currentListLevel;
};



WordsListener::WordsListener()
{
}

WordsListener::~WordsListener()
{
}

void WordsListener::startDocument()
{
    root = "<!DOCTYPE DOC>\n";
    root.append("<DOC mime=\"application/x-words\" syntaxVersion=\"2\" editor=\"Words\">\n");

    // paper definition
    root.append("<PAPER width=\"595\" height=\"841\" format=\"1\" fType=\"0\" orientation=\"0\" hType=\"0\" columns=\"1\">\n");
    root.append("<PAPERBORDERS right=\"28\" left=\"28\" bottom=\"42\" top=\"42\" />");
    root.append("</PAPER>\n");

    root.append("<ATTRIBUTES standardpage=\"1\" hasFooter=\"0\" hasHeader=\"0\" processing=\"0\" />\n");

    root.append("<FRAMESETS>\n");
    root.append("<FRAMESET removable=\"0\" frameType=\"1\" frameInfo=\"0\" autoCreateNewFrame=\"1\">\n");
    root.append("<FRAME right=\"567\" left=\"28\" top=\"42\" bottom=\"799\" />\n");
}

void WordsListener::endDocument()
{
    root.append("</FRAMESET>\n");
    root.append("</FRAMESETS>\n");

    root.append("</DOC>\n");
}

void WordsListener::openParagraph(const WPXPropertyList &propList, const WPXPropertyListVector &tabStops)
{
    root.append("<PARAGRAPH>\n");
    root.append("<TEXT>");
}

void WordsListener::closeParagraph()
{
    root.append("</TEXT>\n");
    root.append("<LAYOUT>\n");
    root.append("<NAME value=\"Standard\" />\n");
    root.append("<FLOW align=\"left\" />\n");
    root.append("<FORMAT/>\n");
    root.append("</LAYOUT>\n");
    root.append("</PARAGRAPH>\n");
}

void WordsListener::insertTab()
{
}

void WordsListener::insertText(const WPXString &text)
{
    root.append(QString::fromUtf8(text.cstr()));
}

void WordsListener::openSpan(const WPXPropertyList &propList)
{
}


void WordsListener::closeSpan()
{
}

void WordsListener::insertLineBreak()
{
}

WPImport::WPImport(QObject* parent, const QVariantList&)
        : KoFilter(parent)
{
}

KoFilter::ConversionStatus WPImport::convert(const QByteArray& from, const QByteArray& to)
{
    // check for proper conversion
    if (to != "application/x-words" || from != "application/wordperfect")
        return KoFilter::NotImplemented;

    // open input file
    const char* infile = m_chain->inputFile().toLatin1();
    FILE *f = fopen(infile, "rb");
    if (!f)
        return KoFilter::StupidError;

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    unsigned char* buf = new unsigned char[fsize];
    fread(buf, 1, fsize, f);
    fclose(f);

    // instream now owns buf, no need to delete buf later
    WPXMemoryInputStream* instream = new WPXMemoryInputStream(buf, fsize);

    // open and parse the file
    WordsListener listener;
#if LIBWPD_VERSION_MINOR>8
    WPDResult error = WPDocument::parse(instream, static_cast<WPXDocumentInterface *>(&listener), NULL);
#else
    WPDResult error = WPDocument::parse(instream, static_cast<WPXHLListenerImpl *>(&listener));
#endif
    delete instream;

    if (error != WPD_OK)
        return KoFilter::StupidError;

    QString root = listener.root;


    if (root.isEmpty()) return KoFilter::StupidError;

    // prepare storage
    KoStoreDevice* out = m_chain->storageFile("root", KoStore::Write);

    if (out) {
        QByteArray cstring = root.toUtf8();
        cstring.prepend("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
        //qDebug("RESULT:\n%s", (const char*)cstring );
        out->write((const char*) cstring, cstring.length());
    }

    return KoFilter::OK;
}

#include "wpimport.moc"
