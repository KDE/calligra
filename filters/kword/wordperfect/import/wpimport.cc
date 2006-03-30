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

#include <config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <kdebug.h>
#include <KoFilterChain.h>
#include <kgenericfactory.h>

#include <wpimport.h>

#include <stdio.h> // debug
//Added by qt3to4:
#include <Q3CString>

typedef KGenericFactory<WPImport, KoFilter> WPImportFactory;
K_EXPORT_COMPONENT_FACTORY( libwpimport, WPImportFactory( "kofficefilters" ) )

#include <libwpd/libwpd.h>
#include <libwpd/WPXStream.h>
#include <libwpd/WPXHLListenerImpl.h>


class WPXMemoryInputStream : public WPXInputStream
{
public:
        WPXMemoryInputStream(uint8_t *data, size_t size);
        virtual ~WPXMemoryInputStream();

        virtual bool isOLEStream() { return false; }
        virtual WPXInputStream * getDocumentOLEStream() { return NULL; }

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
	m_data(data),
	m_size(size),
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

	if ((m_offset+numBytes) < m_size)
		numBytesToRead = numBytes;
	else
		numBytesToRead = m_size - m_offset;
	
	numBytesRead = numBytesToRead; // about as paranoid as we can be..

	if (numBytesToRead == 0)
		return NULL;

	m_tmpBuf = new uint8_t[numBytesToRead];
	for (size_t i=0; i<numBytesToRead; i++)
	{
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
	if (m_offset >= m_size ) 
		return true; 

	return false;
}


class KWordListener : public WPXHLListenerImpl
{
public:
	KWordListener();
	virtual ~KWordListener();

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
    
    QString root;

private:
	unsigned int m_currentListLevel;
};



KWordListener::KWordListener()
{
}

KWordListener::~KWordListener()
{
}

void KWordListener::startDocument()
{
  root = "<!DOCTYPE DOC>\n";
  root.append( "<DOC mime=\"application/x-kword\" syntaxVersion=\"2\" editor=\"KWord\">\n");

  // paper definition
  root.append( "<PAPER width=\"595\" height=\"841\" format=\"1\" fType=\"0\" orientation=\"0\" hType=\"0\" columns=\"1\">\n" );
  root.append( "<PAPERBORDERS right=\"28\" left=\"28\" bottom=\"42\" top=\"42\" />" );
  root.append( "</PAPER>\n" );

  root.append( "<ATTRIBUTES standardpage=\"1\" hasFooter=\"0\" hasHeader=\"0\" processing=\"0\" />\n" );

  root.append( "<FRAMESETS>\n" );
  root.append( "<FRAMESET removable=\"0\" frameType=\"1\" frameInfo=\"0\" autoCreateNewFrame=\"1\">\n" );
  root.append( "<FRAME right=\"567\" left=\"28\" top=\"42\" bottom=\"799\" />\n" );
}

void KWordListener::endDocument()
{
  root.append( "</FRAMESET>\n" );
  root.append( "</FRAMESETS>\n" );

  root.append( "</DOC>\n" );
}

void KWordListener::openParagraph(const WPXPropertyList &propList, const WPXPropertyListVector &tabStops)
{
  root.append( "<PARAGRAPH>\n" );
  root.append( "<TEXT>" );
}

void KWordListener::closeParagraph()
{
  root.append( "</TEXT>\n" );
  root.append( "<LAYOUT>\n");
  root.append( "<NAME value=\"Standard\" />\n");
  root.append( "<FLOW align=\"left\" />\n");
  root.append( "<FORMAT/>\n");
  root.append( "</LAYOUT>\n");
  root.append( "</PARAGRAPH>\n" );
}

void KWordListener::insertTab()
{
}

void KWordListener::insertText(const WPXString &text)
{
  root.append( QString::fromUtf8( text.cstr()) );
}

void KWordListener::openSpan(const WPXPropertyList &propList)
{
}


void KWordListener::closeSpan()
{
}

void KWordListener::insertLineBreak()
{
}

WPImport::WPImport( KoFilter *, const char *, const QStringList& ):  KoFilter()
{
}

KoFilter::ConversionStatus WPImport::convert( const QByteArray& from, const QByteArray& to )
{
  // check for proper conversion
  if( to!= "application/x-kword" || from != "application/wordperfect" )
     return KoFilter::NotImplemented;

  // open input file
  const char* infile = m_chain->inputFile().latin1();
  FILE *f = fopen( infile, "rb" );
  if( !f )
     return KoFilter::StupidError;
  
  fseek( f, 0, SEEK_END );
  long fsize = ftell( f );
  fseek( f, 0, SEEK_SET );
  
  unsigned char* buf = new unsigned char[fsize];
  fread( buf, 1, fsize, f );
  fclose( f );
  
  // instream now owns buf, no need to delete buf later
  WPXMemoryInputStream* instream = new WPXMemoryInputStream( buf, fsize );
     
  // open and parse the file   	
  KWordListener listener;
  WPDResult error = WPDocument::parse( instream, static_cast<WPXHLListenerImpl *>(&listener));
  delete instream;

  if( error != WPD_OK )  
    return KoFilter::StupidError;
     
  QString root = listener.root;

  
  if( root.isEmpty() ) return KoFilter::StupidError;

  // prepare storage
  KoStoreDevice* out = m_chain->storageFile( "root", KoStore::Write );

  if( out )
    {
      Q3CString cstring = root.utf8();
      cstring.prepend( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );
      //qDebug("RESULT:\n%s", (const char*)cstring );
      out->write( (const char*) cstring, cstring.length() );
    }

  return KoFilter::OK;
}

#include "wpimport.moc"
