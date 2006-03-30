/* This file is part of the KDE project
   Copyright (C) 2002,2006 Ariya Hidayat <ariya@kde.org>

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

#include <hancomwordimport.h>
#include <hancomwordimport.moc>

#include <qbuffer.h>
#include <q3cstring.h>
#include <qfile.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtextstream.h>

#include <kdebug.h>
#include <KoFilterChain.h>
#include <KoGlobal.h>
#include <KoUnit.h>
#include <kgenericfactory.h>

#include <KoXmlWriter.h>

#include <iostream>
#include "pole.h"

typedef KGenericFactory<HancomWordImport, KoFilter> HancomWordImportFactory;
K_EXPORT_COMPONENT_FACTORY( libhancomwordimport, HancomWordImportFactory( "kofficefilters" ) )

class HancomWordImport::Private
{
public:
  QString inputFile;
  QString outputFile;
  
  QStringList paragraphs;
  
  QByteArray createStyles();
  QByteArray createContent();
  QByteArray createManifest();
};

HancomWordImport::HancomWordImport ( QObject*, const char*, const QStringList& )
    : KoFilter()
{
  d = new Private;
}

HancomWordImport::~HancomWordImport()
{
  delete d;
}

namespace
{

static inline unsigned long readU16( const void* p )
{
  const unsigned char* ptr = (const unsigned char*) p;
  return ptr[0]+(ptr[1]<<8);
}

}

KoFilter::ConversionStatus HancomWordImport::convert( const QByteArray& from, const QByteArray& to )
{
  if ( from != "application/x-hancomword" )
    return KoFilter::NotImplemented; 

  if ( to != "application/vnd.oasis.opendocument.text" )
    return KoFilter::NotImplemented;

  d->inputFile = m_chain->inputFile();
  d->outputFile = m_chain->outputFile();
  d->paragraphs.clear();

  POLE::Storage storage( d->inputFile.latin1() );
  if( !storage.open() )
    return KoFilter::WrongFormat;
  
  POLE::Stream* stream;
  stream = new POLE::Stream( &storage, "/PrvText" );
  if( stream->fail() || (stream->size() == 0) )
  {
    delete stream;
    return KoFilter::WrongFormat;
  }
  
  int len = stream->size() / 2;
  QString plaindoc;
  plaindoc.reserve( len );
  
  unsigned char* buf = new unsigned char [stream->size()];
  stream->read( buf, stream->size());
  for(int i = 0; i < len; i++ )
    plaindoc.append( QChar((int)readU16(buf + i*2) ) );
  delete[] buf;
  delete stream;

  // split into paragraphs
  d->paragraphs = QStringList::split( "\n", plaindoc, true );
    
  // create output store
  KoStore* storeout;
  storeout = KoStore::createStore( d->outputFile, KoStore::Write, 
    "application/vnd.oasis.opendocument.text", KoStore::Zip );

  if ( !storeout )
  {
    kWarning() << "Couldn't open the requested file." << endl;
    return KoFilter::FileNotFound;
  }

  if ( !storeout->open( "styles.xml" ) )
  {
    kWarning() << "Couldn't open the file 'styles.xml'." << endl;
    return KoFilter::CreationError;
  }
  storeout->write( d->createStyles() );
  storeout->close();

  if ( !storeout->open( "content.xml" ) )
  {
    kWarning() << "Couldn't open the file 'content.xml'." << endl;
    return KoFilter::CreationError;
  }
  storeout->write( d->createContent() );
  storeout->close();

  // store document manifest
  storeout->enterDirectory( "META-INF" );
  if ( !storeout->open( "manifest.xml" ) )
  {
     kWarning() << "Couldn't open the file 'META-INF/manifest.xml'." << endl;
     return KoFilter::CreationError;
  }
  storeout->write( d->createManifest() );
  storeout->close();

  // we are done!
  d->inputFile = QString::null;
  d->outputFile = QString::null;
  delete storeout;

  return KoFilter::OK;
}

QByteArray HancomWordImport::Private::createContent()
{
  KoXmlWriter* contentWriter;
  QByteArray contentData;
  QBuffer contentBuffer( &contentData );

  contentBuffer.open( QIODevice::WriteOnly );
  contentWriter = new KoXmlWriter( &contentBuffer );

  contentWriter->startDocument( "office:document-content" );
  contentWriter->startElement( "office:document-content" );
  
  contentWriter->addAttribute( "xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0" );
  contentWriter->addAttribute( "xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0" );
  contentWriter->addAttribute( "xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0" );
  contentWriter->addAttribute( "xmlns:table", "urn:oasis:names:tc:opendocument:xmlns:table:1.0" );
  contentWriter->addAttribute( "xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0" );
  contentWriter->addAttribute( "xmlns:fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0" );
  contentWriter->addAttribute( "xmlns:svg","urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0" );
  contentWriter->addAttribute( "office:version","1.0" );

  contentWriter->startElement( "office:automatic-styles" );
  contentWriter->endElement(); // office:automatic-style

  // office:body
  contentWriter->startElement( "office:body" );
  
  contentWriter->startElement( "office:text" );
  
  contentWriter->startElement( "text:sequence-decls" );
  contentWriter->endElement();  // text:sequence-decls

  for( uint i = 0; i < paragraphs.count(); i++ )
  {
    QString text = paragraphs[i];
    text.replace( '\r', ' ' );
    contentWriter->startElement( "text:p" );
    contentWriter->addTextNode( text );
    contentWriter->endElement();  // text:p
  }
  
  contentWriter->endElement();  //office:text
  contentWriter->endElement();  // office:body
  
  contentWriter->endElement();  // office:document-content
  contentWriter->endDocument();
  
  delete contentWriter;

  return contentData;
}

QByteArray HancomWordImport::Private::createStyles()
{
  KoXmlWriter* stylesWriter;
  QByteArray stylesData;
  QBuffer stylesBuffer( &stylesData );

  stylesBuffer.open( QIODevice::WriteOnly );
  stylesWriter = new KoXmlWriter( &stylesBuffer );

  stylesWriter->startDocument( "office:document-styles" );
  stylesWriter->startElement( "office:document-styles" );
  stylesWriter->addAttribute( "xmlns:office", "urn:oasis:names:tc:opendocument:xmlns:office:1.0" );
  stylesWriter->addAttribute( "xmlns:style", "urn:oasis:names:tc:opendocument:xmlns:style:1.0" );
  stylesWriter->addAttribute( "xmlns:text", "urn:oasis:names:tc:opendocument:xmlns:text:1.0" );
  stylesWriter->addAttribute( "xmlns:table", "urn:oasis:names:tc:opendocument:xmlns:table:1.0" );
  stylesWriter->addAttribute( "xmlns:draw", "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0" );
  stylesWriter->addAttribute( "xmlns:fo", "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0" );
  stylesWriter->addAttribute( "xmlns:svg","urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0" );
  stylesWriter->addAttribute( "office:version","1.0" );
  stylesWriter->startElement( "office:styles" );
  
  // dummy default paragraph style
  stylesWriter->startElement( "style:default-style" );
  stylesWriter->addAttribute( "style:family", "paragraph" );
  
  stylesWriter->startElement( "style:paragraph-properties" );
  stylesWriter->addAttribute( "fo:hyphenation-ladder-count", "no-limit" );
  stylesWriter->addAttribute( "style:text-autospace", "ideograph-alpha" );
  stylesWriter->addAttribute( "style:punctuation-wrap", "hanging" );
  stylesWriter->addAttribute( "style:line-break", "strict" );
  stylesWriter->addAttribute( "tyle:tab-stop-distance", "0.5in" );
  stylesWriter->addAttribute( "style:writing-mode", "page" );
  stylesWriter->endElement(); // style:paragraph-properties

  stylesWriter->startElement( "style:text-properties" );
  stylesWriter->addAttribute( "style:use-window-font-color", "true" );
  stylesWriter->addAttribute( "style:font-name", "Sans Serif" );
  stylesWriter->addAttribute( "fo:font-size", "12pt" );
  stylesWriter->addAttribute( "fo:hyphenate", "false" );
  stylesWriter->endElement(); // style:text-properties
  
  stylesWriter->endElement(); // style:default-style
  stylesWriter->endElement(); // office:styles
  
  // office:automatic-styles
  stylesWriter->startElement( "office:automatic-styles" );
  stylesWriter->endElement(); // office:automatic-styles

  stylesWriter->endElement();  // office:document-styles
  stylesWriter->endDocument();
  
  delete stylesWriter;

  return stylesData;
}

QByteArray HancomWordImport::Private::createManifest()
{
  KoXmlWriter* manifestWriter;
  QByteArray manifestData;
  QBuffer manifestBuffer( &manifestData );

  manifestBuffer.open( QIODevice::WriteOnly );
  manifestWriter = new KoXmlWriter( &manifestBuffer );
  
  manifestWriter->startDocument( "manifest:manifest" );
  manifestWriter->startElement( "manifest:manifest" );
  manifestWriter->addAttribute( "xmlns:manifest", "urn:oasis:names:tc:openoffice:xmlns:manifest:1.0" );
  manifestWriter->addManifestEntry( "/", "application/vnd.oasis.opendocument.text" );
  manifestWriter->addManifestEntry( "styles.xml", "text/xml" );
  manifestWriter->addManifestEntry( "content.xml", "text/xml" );
  manifestWriter->endElement();
  manifestWriter->endDocument();
  delete manifestWriter;

  return manifestData;
}
