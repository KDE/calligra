/* This file is part of the KDE project
   Copyright (C) 2007 Ariya Hidayat <ariya@kde.org>

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

#include <WPGImport.h>
#include <WPGImport.moc>

#include <QString>
#include <QDate>

#include <kdebug.h>
#include <KoFilterChain.h>
#include <KoGlobal.h>
#include <KoUnit.h>
#include <kgenericfactory.h>

#include <KoXmlWriter.h>
#include <KoOasisStore.h>

#include <libwpg.h>
#include <WPGStreamImplementation.h>

#include <iostream>

typedef KGenericFactory<WPGImport> WPGImportFactory;
K_EXPORT_COMPONENT_FACTORY( libwpgimport, WPGImportFactory( "kofficefilters" ) )

WPGImport::WPGImport ( QObject* parent, const QStringList& )
    : KoFilter( parent )
{
}

WPGImport::~WPGImport()
{
}

KoFilter::ConversionStatus WPGImport::convert( const QByteArray& from, const QByteArray& to )
{
  if ( from != "application/x-wpg" )
    return KoFilter::NotImplemented;

  if ( to != "image/svg+xml" )
    return KoFilter::NotImplemented;


  WPXInputStream* input = new libwpg::WPGFileStream( m_chain->inputFile().toLocal8Bit() );
  if (input->isOLEStream())
  {
    WPXInputStream* olestream = input->getDocumentOLEStream();
    if (olestream)
    {
      delete input;
      input = olestream;
    }
  }

  if (!libwpg::WPGraphics::isSupported(input))
  {
    std::cerr << "ERROR: Unsupported file format (unsupported version) or file is encrypted!" << std::endl;
    return KoFilter::NotImplemented;
  }

  libwpg::WPGString output;
  if (!libwpg::WPGraphics::generateSVG(input, output))
  {
    std::cerr << "ERROR: SVG Generation failed!" << std::endl;
    return KoFilter::NotImplemented;
  }


  QFile file(  m_chain->outputFile().toLocal8Bit() );
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    std::cerr << "ERROR: SVG Generation failed!" << std::endl;
    return KoFilter::NotImplemented;
  }

  QTextStream out(&file);
  out << output.cstr();


#if 0
  d->inputFile = m_chain->inputFile();
  d->outputFile = m_chain->outputFile();

  // open inputFile
  d->workbook = new Swinder::Workbook;
  if( !d->workbook->load( d->inputFile.toLocal8Bit() ) )
  {
    delete d->workbook;
    d->workbook = 0;
    return KoFilter::StupidError;
  }

  if( d->workbook->isPasswordProtected() )
  {
    delete d->workbook;
    d->workbook = 0;
    return KoFilter::PasswordProtected;
  }
#endif


  return KoFilter::OK;
}
