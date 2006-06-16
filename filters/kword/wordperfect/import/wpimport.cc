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
   Boston, MA 02110-1301, USA.
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

typedef KGenericFactory<WPImport, KoFilter> WPImportFactory;
K_EXPORT_COMPONENT_FACTORY( libwpimport, WPImportFactory( "kofficefilters" ) )

#include <libwpd/libwpd.h>
#include <libwpd/WPXStream.h>
#include "DocumentHandler.hxx"
#include "WordPerfectCollector.hxx"

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

class KWordHandler : public DocumentHandler
{
public:
        KWordHandler();
	virtual ~KWordHandler() {};
	void startDocument();
        void endDocument();
        void startElement(const char *psName, const WPXPropertyList &xPropList);
        void endElement(const char *psName);
        void characters(const WPXString &sCharacters);
	WPXString documentstring;
private:
	bool isTagOpened;
	WPXString openedTagName;
};

KWordHandler::KWordHandler() :
	isTagOpened(false)
{
}

void KWordHandler::startDocument()
{
  documentstring.clear();
}

void KWordHandler::startElement(const char *psName, const WPXPropertyList &xPropList)
{
	if (isTagOpened)
	{
		documentstring.append( ">" );
		isTagOpened = false;
	}
	WPXString tempString;
        tempString.sprintf("<%s", psName);
	documentstring.append( tempString );
        WPXPropertyList::Iter i(xPropList);
        for (i.rewind(); i.next(); )
        {
                // filter out libwpd elements
                if (strlen(i.key()) > 6 && strncmp(i.key(), "libwpd", 6) != 0)
		{
			tempString.sprintf(" %s=\"%s\"", i.key(), i()->getStr().cstr());
                        documentstring.append( tempString );
        	}
	}
	isTagOpened = true;
	openedTagName.sprintf("%s", psName);
}

void KWordHandler::endElement(const char *psName)
{
	if ((isTagOpened) && (openedTagName == psName))
		documentstring.append( " />" );
	else
	{
		WPXString tempString;
		tempString.sprintf("</%s>", psName);
	        documentstring.append( tempString );
	}
	isTagOpened = false;
}

void KWordHandler::characters(const WPXString &sCharacters)
{
	if (isTagOpened)
	{
		documentstring.append( ">" );
		isTagOpened = false;
	}
        documentstring.append( WPXString(sCharacters, true) );
}


void KWordHandler::endDocument()
{
	if (isTagOpened)
	{
		documentstring.append( ">" );
		isTagOpened = false;
	}
}
	


WPImport::WPImport( KoFilter *, const char *, const QStringList& ):  KoFilter()
{
}

KoFilter::ConversionStatus WPImport::convert( const QCString& from, const QCString& to )
{
  // check for proper conversion
  if(to!= "application/vnd.sun.xml.writer" || from != "application/wordperfect" )
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
  WPXMemoryInputStream instream = WPXMemoryInputStream( buf, fsize );

  WPDConfidence confidence = WPDocument::isFileFormatSupported(&instream, false);
  if( confidence == WPD_CONFIDENCE_NONE )
  {
    fprintf(stderr, "ERROR: We have no confidence that you are giving us a valid WordPerfect document.\n");
    return KoFilter::StupidError;
  }
  instream.seek(0, WPX_SEEK_SET);

  // open and parse the file   	
  KWordHandler handler;
        
  WordPerfectCollector collector(&instream, &handler);
  
  if ( !collector.filter() ) return KoFilter::StupidError;
  
  // prepare storage
  KoStoreDevice* manifest = m_chain->storageFile( "META-INF/manifest.xml", KoStore::Write );
  if ( manifest )
    {
      QCString manifeststring = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<!DOCTYPE manifest:manifest PUBLIC \"-//OpenOffice.org//DTD Manifest 1.0//EN\" \"Manifest.dtd\">\n\
<manifest:manifest xmlns:manifest=\"http://openoffice.org/2001/manifest\">\n\
<manifest:file-entry manifest:media-type=\"application/vnd.sun.xml.writer\" manifest:full-path=\"/\"/>\n\
<manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"content.xml\"/>\n\
<manifest:file-entry manifest:media-type=\"text/xml\" manifest:full-path=\"styles.xml\"/>\n\
</manifest:manifest>\n";
      manifest->writeBlock( (const char*) manifeststring, manifeststring.length() );
    }
    
  KoStoreDevice* styles = m_chain->storageFile( "styles.xml", KoStore::Write );
  if ( styles )
    {
      QCString stylesstring = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<!DOCTYPE office:document-styles PUBLIC \"-//OpenOffice.org//DTD OfficeDocument 1.0//EN\" \"office.dtd\">\
<office:document-styles xmlns:office=\"http://openoffice.org/2000/office\" xmlns:style=\"http://openoffice.org/2000/style\"\
 xmlns:text=\"http://openoffice.org/2000/text\" xmlns:table=\"http://openoffice.org/2000/table\"\
 xmlns:draw=\"http://openoffice.org/2000/drawing\" xmlns:fo=\"http://www.w3.org/1999/XSL/Format\"\
 xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns:number=\"http://openoffice.org/2000/datastyle\"\
 xmlns:svg=\"http://www.w3.org/2000/svg\" xmlns:chart=\"http://openoffice.org/2000/chart\" xmlns:dr3d=\"http://openoffice.org/2000/dr3d\"\
 xmlns:math=\"http://www.w3.org/1998/Math/MathML\" xmlns:form=\"http://openoffice.org/2000/form\"\
 xmlns:script=\"http://openoffice.org/2000/script\" office:version=\"1.0\">\
<office:styles>\
<style:default-style style:family=\"paragraph\">\
<style:properties style:use-window-font-color=\"true\" style:text-autospace=\"ideograph-alpha\"\
 style:punctuation-wrap=\"hanging\" style:line-break=\"strict\" style:writing-mode=\"page\"/>\
</style:default-style>\
<style:default-style style:family=\"table\"/>\
<style:default-style style:family=\"table-row\"/>\
<style:default-style style:family=\"table-column\"/>\
<style:style style:name=\"Standard\" style:family=\"paragraph\" style:class=\"text\"/>\
<style:style style:name=\"Text body\" style:family=\"paragraph\" style:parent-style-name=\"Standard\" style:class=\"text\"/>\
<style:style style:name=\"List\" style:family=\"paragraph\" style:parent-style-name=\"Text body\" style:class=\"list\"/>\
<style:style style:name=\"Header\" style:family=\"paragraph\" style:parent-style-name=\"Standard\" style:class=\"extra\"/>\
<style:style style:name=\"Footer\" style:family=\"paragraph\" style:parent-style-name=\"Standard\" style:class=\"extra\"/>\
<style:style style:name=\"Caption\" style:family=\"paragraph\" style:parent-style-name=\"Standard\" style:class=\"extra\"/>\
<style:style style:name=\"Footnote\" style:family=\"paragraph\" style:parent-style-name=\"Standard\" style:class=\"extra\"/>\
<style:style style:name=\"Endnote\" style:family=\"paragraph\" style:parent-style-name=\"Standard\" style:class=\"extra\"/>\
<style:style style:name=\"Index\" style:family=\"paragraph\" style:parent-style-name=\"Standard\" style:class=\"index\"/>\
<style:style style:name=\"Footnote Symbol\" style:family=\"text\">\
<style:properties style:text-position=\"super 58%\"/>\
</style:style>\
<style:style style:name=\"Endnote Symbol\" style:family=\"text\">\
<style:properties style:text-position=\"super 58%\"/>\
</style:style>\
<style:style style:name=\"Footnote anchor\" style:family=\"text\">\
<style:properties style:text-position=\"super 58%\"/>\
</style:style>\
<style:style style:name=\"Endnote anchor\" style:family=\"text\">\
<style:properties style:text-position=\"super 58%\"/>\
</style:style>\
<text:footnotes-configuration text:citation-style-name=\"Footnote Symbol\" text:citation-body-style-name=\"Footnote anchor\"\
 style:num-format=\"1\" text:start-value=\"0\" text:footnotes-position=\"page\" text:start-numbering-at=\"document\"/>\
<text:endnotes-configuration text:citation-style-name=\"Endnote Symbol\" text:citation-body-style-name=\"Endnote anchor\"\
 text:master-page-name=\"Endnote\" style:num-format=\"i\" text:start-value=\"0\"/>\
<text:linenumbering-configuration text:number-lines=\"false\" text:offset=\"0.1965inch\" style:num-format=\"1\"\
 text:number-position=\"left\" text:increment=\"5\"/>\
</office:styles>\
<office:automatic-styles>\
<style:page-master style:name=\"PM0\">\
<style:properties fo:margin-bottom=\"1.0000inch\" fo:margin-left=\"1.0000inch\" fo:margin-right=\"1.0000inch\" fo:margin-top=\"1.0000inch\"\
 fo:page-height=\"11.0000inch\" fo:page-width=\"8.5000inch\" style:print-orientation=\"portrait\">\
<style:footnote-sep style:adjustment=\"left\" style:color=\"#000000\" style:distance-after-sep=\"0.0398inch\"\
 style:distance-before-sep=\"0.0398inch\" style:rel-width=\"25%\" style:width=\"0.0071inch\"/>\
</style:properties>\
</style:page-master>\
<style:page-master style:name=\"PM1\">\
<style:properties fo:margin-bottom=\"1.0000inch\" fo:margin-left=\"1.0000inch\" fo:margin-right=\"1.0000inch\" fo:margin-top=\"1.0000inch\"\
 fo:page-height=\"11.0000inch\" fo:page-width=\"8.5000inch\" style:print-orientation=\"portrait\">\
<style:footnote-sep style:adjustment=\"left\" style:color=\"#000000\" style:rel-width=\"25%\"/>\
</style:properties>\
</style:page-master>\
</office:automatic-styles>\
<office:master-styles>\
<style:master-page style:name=\"Standard\" style:page-master-name=\"PM0\"/>\
<style:master-page style:name=\"Endnote\" style:page-master-name=\"PM1\"/>\
</office:master-styles>\
</office:document-styles>";
      styles->writeBlock( (const char*) stylesstring, stylesstring.length() );
    }
  
  KoStoreDevice* out = m_chain->storageFile( "content.xml", KoStore::Write );

  if( out )
      out->writeBlock( (const char*) handler.documentstring.cstr(), strlen(handler.documentstring.cstr()) );

  return KoFilter::OK;
}

#include "wpimport.moc"
