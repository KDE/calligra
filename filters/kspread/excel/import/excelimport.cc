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

#include <qstring.h>
#include <qdom.h>

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

ExcelImport::ExcelImport ( QObject*, const char*, const QStringList& )
    : KoFilter()
{
}

// convert from RGB values to "#rrggbb"
static QString convertColor( const Sidewinder::Color& color )
{
  QString c;
  c.append( '#' );
  c.append( QString::number( color.red / 16, 16 ) );
  c.append( QString::number( color.red % 10, 16 ) );
  c.append( QString::number( color.green / 16, 16 ) );
  c.append( QString::number( color.green % 10, 16 ) );
  c.append( QString::number( color.blue / 16, 16 ) );
  c.append( QString::number( color.blue % 10, 16 ) );
  return c;
}

QDomElement convertFormat( QDomDocument& doc, const Sidewinder::Format& format )
{
  QDomElement e = doc.createElement( "format" );

  unsigned align = 0;
  switch( format.alignment().alignX() )
  {
    case Sidewinder::Format::Left: align = 1; break;
    case Sidewinder::Format::Center: align = 2; break;
    case Sidewinder::Format::Right: align = 3; break;
     default: align = 0; break;
  };

  e.setAttribute( "align", QString::number( align ) );

  QDomElement fontElement = doc.createElement( "font" );
  const Sidewinder::FormatFont& font = format.font();
  QString fontFamily = string( font.fontFamily()).string();
  double fontSize = font.fontSize();
  fontElement.setAttribute( "family", fontFamily );
  fontElement.setAttribute( "size", QString::number( fontSize ) );
  fontElement.setAttribute( "weight", font.bold() ? "75" : "50" );
  fontElement.setAttribute( "bold", font.bold() ? "yes" : "no" );
  fontElement.setAttribute( "italic", font.italic() ? "yes" : "no" );
  fontElement.setAttribute( "underline", font.underline() ? "yes" : "no" );
  fontElement.setAttribute( "strikeout", font.strikeout() ? "yes" : "no" );
  e.appendChild( fontElement );

  QDomElement penElement = doc.createElement( "pen" );
  penElement.setAttribute( "width", 0 );
  penElement.setAttribute( "style", 1 );
  penElement.setAttribute( "color", convertColor( font.color() ) );
  e.appendChild( penElement );

  return e;
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

  QDomDocument mainDocument( "spreadsheet" );
  mainDocument.appendChild( mainDocument.createProcessingInstruction(
    "xml","version=\"1.0\" encoding=\"UTF-8\"" ) );

  QDomElement spreadsheet;
  spreadsheet = mainDocument.createElement( "spreadsheet" );
  spreadsheet.setAttribute( "editor","MS Excel Import Filter" );
  spreadsheet.setAttribute( "mime","application/x-kspread" );
  mainDocument.appendChild( spreadsheet );

  QDomElement map;
  map = mainDocument.createElement( "map" );
  map.setAttribute( "activeTable", "Table1" );
  spreadsheet.appendChild( map );

  for( unsigned i=0; i < workbook->sheetCount(); i++ )
  {
    Sidewinder::Sheet* sheet = workbook->sheet( i );

    if( !sheet ) break;

    QDomElement table;
    table = mainDocument.createElement( "table" );
    table.setAttribute( "name", string( sheet->name() ).string() );
    map.appendChild( table );

    for( unsigned i = 0; i <= sheet->maxColumn(); i++ )
    {
      Sidewinder::Column* column = sheet->column( i, false );
      if( column )
      {
        QDomElement e;
        e = mainDocument.createElement( "column" );
        e.setAttribute( "column", QString::number( i+1 ) );
        e.setAttribute( "width", QString::number( column->width() ) );
        table.appendChild( e );

        QDomElement fe = convertFormat( mainDocument, column->format() );
        e.appendChild( fe );
      }
    }

    for( unsigned i = 0; i <= sheet->maxRow(); i++ )
    {
      Sidewinder::Row* row = sheet->row( i, false );
      if( row )
      {
        QDomElement e;
        e = mainDocument.createElement( "row" );
        e.setAttribute( "row", QString::number( i+1 ) );
        e.setAttribute( "height", QString::number( POINT_TO_MM ( row->height() ) ) );
        table.appendChild( e );

        QDomElement fe = convertFormat( mainDocument, row->format() );
        e.appendChild( fe );
      }
    }

    for( unsigned row = 0; row <= sheet->maxRow(); row++ )
      for( unsigned col = 0; col <= sheet->maxColumn(); col++ )
      {
        Sidewinder::Cell* cell = sheet->cell( col, row, false );
        if( cell )
        {
          QDomElement ce;
          ce = mainDocument.createElement( "cell" );
          ce.setAttribute( "row", QString::number( row+1 ) );
          ce.setAttribute( "column", QString::number( col+1 ) );
          table.appendChild( ce );

          QDomElement fe;
          fe = convertFormat( mainDocument, cell->format() );
          ce.appendChild( fe );

          Sidewinder::Value value = cell->value();

          QDomElement ve;
          ve = mainDocument.createElement( "text" );
          ce.appendChild( ve );

          if( value.isBoolean() )
          {
            ve.setAttribute( "dataType", "Bool" );
            if( value.asBoolean() )
            {
              ve.setAttribute( "outStr", "True" );
              ve.appendChild( mainDocument.createTextNode( "true" ) );
            }
            else
            {
              ve.setAttribute( "outStr", "False" );
              ve.appendChild( mainDocument.createTextNode( "false" ) );
            }
          }
          else if( value.isFloat() )
          {
            ve.setAttribute( "dataType", "Num" );
            QString str = QString::number( value.asFloat() );
            ve.appendChild( mainDocument.createTextNode( str ) );
          }
          else if( value.isInteger() )
          {
            ve.setAttribute( "dataType", "Num" );
            QString str = QString::number( value.asInteger() );
            ve.appendChild( mainDocument.createTextNode( str ) );
          }
          else if( value.isString() )
          {
            ve.setAttribute( "dataType", "Str" );
            QString str = string( value.asString() ).string();
            ve.appendChild( mainDocument.createTextNode( str ) );
          }

        }
      }

  }

  // prepare storage
  KoStoreDevice* out=m_chain->storageFile( "root", KoStore::Write );

  // store output document
  if( out )
    {
      QCString cstring = mainDocument.toCString();
      out->writeBlock( (const char*) cstring, cstring.length() );
      out->close();
    }

  // store document info
  out = m_chain->storageFile( "documentinfo.xml", KoStore::Write );
  if ( out )
    {
       QCString cstring = documentInfo.utf8();
       cstring.prepend( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );
       out->writeBlock( (const char*) cstring, cstring.length() );
       out->close();
     }

  delete reader;
  delete workbook;

  return KoFilter::OK;
}
