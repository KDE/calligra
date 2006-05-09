/* This file is part of the KDE project
   Copyright (C) 2002 Ariya Hidayat <ariyahidayat@yahoo.de>

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

#include <dbaseimport.h>
#include <dbaseimport.moc>
#include <dbase.h>

#include <qfile.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qstring.h>
//Added by qt3to4:
#include <Q3CString>

#include <kdebug.h>
#include <KoFilterChain.h>
#include <KoGlobal.h>
#include <KoUnit.h>
#include <kgenericfactory.h>
#include <kmessagebox.h>

typedef KGenericFactory<DBaseImport> DBaseImportFactory;
K_EXPORT_COMPONENT_FACTORY( libdbaseimport, DBaseImportFactory( "kofficefilters" ) )


DBaseImport::DBaseImport ( QObject* parent, const QStringList& )
    : KoFilter( parent )
{
}

KoFilter::ConversionStatus DBaseImport::convert( const QByteArray& from, const QByteArray& to )
{
  if (to != "application/x-kspread" || from != "application/x-dbase")
    return KoFilter::NotImplemented;

  QString inputFile = m_chain->inputFile();

  DBase dbase;
  bool result = dbase.load( inputFile );

  if( dbase.version() !=3 )
  {
    KMessageBox::sorry( 0, i18n("File format is not supported.") );
    return KoFilter::NotImplemented;
  }

  if( !result )
  {
    KMessageBox::sorry( 0, i18n("Could not read from file." ) );
    return KoFilter::StupidError;
  }

  QString root, documentInfo;

  root = "<!DOCTYPE spreadsheet >\n";
  root += "<spreadsheet mime=\"application/x-kspread\" editor=\"KSpread\" >\n";
  root += "<paper format=\"A4\" orientation=\"Portrait\" >\n";
  root += "<borders right=\"20\" left=\"20\" bottom=\"20\" top=\"20\" />\n";
  root += "<head/>\n";
  root += "<foot/>\n";
  root += "</paper>\n";
  root += "<map activeTable=\"Table1\" >\n";

  root += "<locale positivePrefixCurrencySymbol=\"True\"";
  root += "  negativeMonetarySignPosition=\"0\"";
  root += "  negativePrefixCurrencySymbol=\"True\" fracDigits=\"2\"";
  root += "  thousandsSeparator=\",\" dateFormat=\"%A %d %B %Y\"";
  root += "  timeFormat=\"%H:%M:%S\" monetaryDecimalSymbol=\".\"";
  root += "  weekStartsMonday=\"True\" currencySymbol=\"$\"";
  root += "  negativeSign=\"-\" positiveSign=\"\"";
  root += "  positiveMonetarySignPosition=\"1\" decimalSymbol=\".\"";
  root += "  monetaryThousandsSeparator=\",\" dateFormatShort=\"%Y-%m-%d\" />\n";

  root += "<table name=\"Table1\" columnnumber=\"0\" borders=\"0\"";
  root += "  hide=\"0\" hidezero=\"0\" firstletterupper=\"0\" grid=\"1\"";
  root += "  formular=\"0\" lcmode=\"0\" >\n";

  // KOffice default font
  QFont font = KoGlobal::defaultFont();

  // define columns
  QFontMetrics fm( font );
  for( unsigned i=0; i<dbase.fields.count(); i++ )
  {
    int mw = qMax( (int)dbase.fields.at(i)->length, dbase.fields.at(i)->name.length());
    double w = POINT_TO_MM( fm.maxWidth() * mw );
    root += "<column column=\"" + QString::number(i+1) + "\"";
    root += " width=\"" + QString::number( w ) + "\"><format/></column>\n";
  }

  // define rows
  double h = POINT_TO_MM( 5 + fm.height() + fm.leading() );
  for( unsigned j=0; j<dbase.recordCount(); j++ )
  {
    root += "<row row=\"" + QString::number(j+1) + "\"";
    root += " height=\"" + QString::number( h ) + "\" ><format/></row>\n";
  }

  // field names come as first row
  for( unsigned i=0; i<dbase.fields.count(); i++ )
  {
    root += "<cell row=\"1\" column=\"" + QString::number(i+1) + "\" >\n";
    root += "<format><pen width=\"0\" style=\"1\" color=\"#000000\" />";
    root += "<font family=\"" + font.family() + "\"" +
      " size=\"" + QString::number(font.pointSizeFloat()) + "\"" +
      " weight=\"50\" />";
    root += "</format>\n";
    root += "<text>" + dbase.fields.at(i)->name + "</text></cell>\n";
  }

  // process all records
  unsigned row = 1;
  for( unsigned j=0; j<dbase.recordCount(); j++ )
  {
    QStringList rec = dbase.readRecord( j );
    if( rec.count() )
    {
      row++;
      for( unsigned i=0; i<rec.count(); i++ )
      {
        root += "<cell row=\"" + QString::number(row) + "\"" +
          "column=\"" + QString::number(i+1) + "\" >\n";
        root += "<format><pen width=\"0\" style=\"1\" color=\"#000000\" />";
        root += "<font family=\"" + font.family() + "\"" +
          " size=\"" + QString::number(font.pointSizeFloat()) + "\"" +
          " weight=\"50\" />";
        root += "</format>\n";
        root += "<text>" + rec[i] + "</text></cell>\n";
      }
    }
  }

  dbase.close();

  root += "</table>\n";
  root += "</map>\n";
  root += "</spreadsheet>";

  // prepare storage
  KoStoreDevice* out=m_chain->storageFile( "root", KoStore::Write );

  // store output document
  if( out )
    {
      Q3CString cstring = root.utf8();
      cstring.prepend( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );
      out->write( (const char*) cstring, cstring.length() );
    }

  // store document info
  out = m_chain->storageFile( "documentinfo.xml", KoStore::Write );
  if ( out )
    {
       Q3CString cstring = documentInfo.utf8();
       cstring.prepend( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );

       out->write( (const char*) cstring, cstring.length() );
     }

  return KoFilter::OK;
}
