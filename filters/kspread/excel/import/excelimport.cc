/* This file is part of the KDE project
   Copyright (C) 2003 Ariya Hidayat <ariya@kde.org>

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

#include <config.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <excelimport.h>
#include <excelimport.moc>

#include <qfile.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qstring.h>
#include <qregexp.h>

#include <kdebug.h>
#include <koFilterChain.h>
#include <koGlobal.h>
#include <koUnit.h>
#include <kgenericfactory.h>
#include <kmessagebox.h>

#include <sidewinder.h>
#include <iostream>

typedef KGenericFactory<ExcelImport, KoFilter> ExcelImportFactory;
K_EXPORT_COMPONENT_FACTORY( libexcelimport, ExcelImportFactory( "kspreadexcelfilter" ) );


// UString -> QConstString conversion. Use .string() to get the QString.
// Always store the QConstString into a variable first, to avoid a deep copy.
inline QConstString string( const Sidewinder::UString& str ) {
   // Let's hope there's no copying of the QConstString happening...
   return QConstString( reinterpret_cast<const QChar*>( str.data() ), str.length() );
}

// encode text for XML-ness
// FIXME could be faster without QRegExp
QString encodeXML( const QString& s )
{
  QString text = s;
  text.replace( QRegExp("&"), "&amp;" );
  text.replace( QRegExp("<"), "&lt;" );
  text.replace( QRegExp(">"), "&gt;" );
  text.replace( QRegExp("\""), "&quot;" );
  text.replace( QRegExp("'"), "&apos;" );
  return text;
}


ExcelImport::ExcelImport ( QObject*, const char*, const QStringList& )
    : KoFilter()
{
}

KoFilter::ConversionStatus ExcelImport::convert( const QCString& from, const QCString& to )
{
  if (to != "application/x-kspread" || from != "application/msexcel")
    return KoFilter::NotImplemented;

  QString inputFile = m_chain->inputFile();


  Sidewinder::Reader *reader;
  reader = Sidewinder::ReaderFactory::createReader( "application/msexcel" );

  if( !reader )
  {
    KMessageBox::sorry( 0, i18n("Internal problem: format not supported." ) );
    return KoFilter::StupidError;
  }

  Sidewinder::Workbook* workbook;
  workbook = reader->load( inputFile.local8Bit() );
  if( !workbook )
  {
    KMessageBox::sorry( 0, i18n("Could not read from file." ) );
    delete reader;
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

  for( unsigned i=0; i < workbook->sheetCount(); i++ )
  {
    Sidewinder::Sheet* sheet = workbook->sheet( i );

    if( !sheet ) break;

    root += "<table name=\"";
    root += string( sheet->name() ).string();
    root += "\" columnnumber=\"0\" borders=\"0\"";
    root += "  hide=\"0\" hidezero=\"0\" firstletterupper=\"0\" grid=\"1\"";
    root += "  formular=\"0\" lcmode=\"0\" >\n";

    QFont font = KoGlobal::defaultFont();
    for( unsigned row = 0; row <= sheet->maxRow(); row++ )
      for( unsigned col = 0; col <= sheet->maxColumn(); col++ )
      {
        Sidewinder::Cell* cell = sheet->cell( col, row, false );
        if( cell )
        {
          root += "<cell row=\"" + QString::number( row+1 ) + "\"" +
            "column=\"" + QString::number( col+1 ) + "\" >\n";


          const Sidewinder::Format& format = cell->format();

          unsigned align = 0;
          switch( format.alignment().alignX() )
          {
            case Sidewinder::Format::Left: align = 1; break;
            case Sidewinder::Format::Center: align = 2; break;
            case Sidewinder::Format::Right: align = 3; break;
            default: align = 0; break;
          };

          root += "<format align=\"";
          root += QString::number( align );
          root += "\">";
          root += "</format>\n";

          Sidewinder::Value value = cell->value();
          if( value.isEmpty() )
            root += "<text></text>\n";
          else if( value.isBoolean() )
          {
            if( value.asBoolean() )
              root += "<text dataType=\"Bool\" outStr=\"True\" >true</text>\n";
            else;
              root += "<text dataType=\"Bool\" outStr=\"False\" >false</text>\n";
          }
          else if( value.isFloat() )
          {
            root += QString("<text dataType=\"Num\" >%1</text>\n").
              arg( QString::number( value.asFloat() ) );
              std::cout << "row:" << row << " col:" << col;
              std::cout << " is " << value.asFloat() << std::endl;
          }else if( value.isInteger() )
            root += QString("<text dataType=\"Num\" >%1</text>\n").
              arg( QString::number( value.asInteger() ) );
          else if( value.isString() )
            root += QString("<text dataType=\"Str\" >%1</text>\n").
              arg( encodeXML( string( value.asString() ).string() ) );
          else
            root += "<text></text>\n"; // fallback

         root += "</cell>\n";
        }
      }

    root += "</table>\n";

    }

  root += "</map>\n";
  root += "</spreadsheet>";

  // prepare storage
  KoStoreDevice* out=m_chain->storageFile( "root", KoStore::Write );

  // store output document
  if( out )
    {
      QCString cstring = root.utf8();
      cstring.prepend( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );
      out->writeBlock( (const char*) cstring, cstring.length() );
    }

  // store document info
  out = m_chain->storageFile( "documentinfo.xml", KoStore::Write );
  if ( out )
    {
       QCString cstring = documentInfo.utf8();
       cstring.prepend( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );
       out->writeBlock( (const char*) cstring, cstring.length() );
     }

  delete reader;
  delete workbook;

  return KoFilter::OK;
}
