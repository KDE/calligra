/* This file is part of the KDE project
   Copyright (C) 2000 David Faure <faure@kde.org>

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

#include <csvexport.h>

#include <qfile.h>

#include <kdebug.h>
#include <kmessagebox.h>
#include <kgenericfactory.h>
#include <koFilterChain.h>

#include <kspread_map.h>
#include <kspread_sheet.h>
#include <kspread_doc.h>
#include <kspread_view.h>

#include <csvexportdialog.h>

typedef KGenericFactory<CSVExport, KoFilter> CSVExportFactory;
K_EXPORT_COMPONENT_FACTORY( libcsvexport, CSVExportFactory( "kofficefilters" ) )

class Cell 
{
 public:
  int row, col;
  QString text;

  bool operator < ( const Cell & c ) const
  {
    return row < c.row || ( row == c.row && col < c.col );
  }
  bool operator == ( const Cell & c ) const
  {
    return row == c.row && col == c.col;
  }
};


CSVExport::CSVExport( KoFilter *, const char *, const QStringList & ) 
  : KoFilter() 
{
}


void CSVExport::exportCell( KSpreadSheet const * const sheet, int col, int row, QString & separators, 
                            QString & line, QChar const & csvDelimiter, QChar const & textQuote )
{
  KSpreadCell const * const cell = sheet->cellAt( col, row );

  QString text;
  if ( !cell->isDefault() && !cell->isEmpty() )
  {
    switch( cell->content() )
    {
     case KSpreadCell::Text:
      text = cell->strOutText();
      break;
     case KSpreadCell::RichText:
     case KSpreadCell::VisualFormula:
      text = cell->text(); // untested
      break;
     case KSpreadCell::Formula:
      //      cell->setCalcDirtyFlag();
      //      cell->calc(); 
      //      text = cell->value().asString();
      text = cell->strOutText();
      break;
    }
  }
  if ( !text.isEmpty() )
  {
    line += separators;
    if ( text.find( csvDelimiter ) != -1 )
    {
      text = textQuote + text + textQuote;
    }
    line += text;
    separators = QString::null;
  }
  // Append a delimiter, but in a temp string -> if no other real cell in this line,
  // then those will be dropped
  separators += csvDelimiter;
}

// The reason why we use the KoDocument* approach and not the QDomDocument
// approach is because we don't want to export formulas but values !
KoFilter::ConversionStatus CSVExport::convert( const QCString & from, const QCString & to )
{
  KoDocument* document = m_chain->inputDocument();

  if ( !document )
    return KoFilter::StupidError;

  if ( strcmp( document->className(), "KSpreadDoc" ) != 0 )  // it's safer that way :)
  {
    kdWarning(30501) << "document isn't a KSpreadDoc but a " << document->className() << endl;
    return KoFilter::NotImplemented;
  }
  if ( ( to != "text/x-csv" && to != "text/plain" ) || from != "application/x-kspread" )
  {
    kdWarning(30501) << "Invalid mimetypes " << to << " " << from << endl;
    return KoFilter::NotImplemented;
  }

  KSpreadDoc const * const ksdoc = static_cast<const KSpreadDoc *>(document);

  if ( ksdoc->mimeType() != "application/x-kspread" )
  {
    kdWarning(30501) << "Invalid document mimetype " << ksdoc->mimeType() << endl;
    return KoFilter::NotImplemented;
  }

  CSVExportDialog expDialog( 0 );
  expDialog.fillTable( ksdoc->map() );

  if ( !expDialog.exec() )
    return KoFilter::UserCancelled;

  QChar csvDelimiter;

  csvDelimiter = expDialog.getDelimiter();

  // Now get hold of the table to export
  // (Hey, this could be part of the dialog too, choosing which table to export....
  //  It's great to have parametrable filters... IIRC even MSOffice doesn't have that)
  // Ok, for now we'll use the first table - my document has only one table anyway ;-)))

  bool first = true;
  QString str;
  QChar textQuote = expDialog.getTextQuote();

  if ( expDialog.exportSelectionOnly() )
  {
    kdDebug(30501) << "Export as selection mode" << endl;
    KSpreadView const * const view = static_cast<KSpreadView*>(ksdoc->views().getFirst());

    if ( !view ) // no view if embedded document
      return KoFilter::StupidError;

    KSpreadSheet const * const sheet = view->activeTable();

    QRect selection = view->selection();
    int right       = selection.right();
    int bottom      = selection.bottom();

    QString emptyLines;
    for ( int row = selection.top(); row <= bottom; ++row )
    {
      QString separators;
      QString line;

      for ( int col = selection.left(); col <= right; ++col )
      {
        exportCell( sheet, col, row, separators, line, csvDelimiter, textQuote );
      }

      if ( !line.isEmpty() )
      {
        str += emptyLines;
        str += line;
        emptyLines = QString::null;
      }
      // Append a CR, but in a temp string -> if no other real line,
      // then those will be dropped
      emptyLines += "\n";
    }
  }
  else
  {
    kdDebug(30501) << "Export as full mode" << endl;
    QPtrListIterator<KSpreadSheet> it( ksdoc->map()->tableList() );
    for( ; it.current(); ++it )
    {
      KSpreadSheet const * const sheet = it.current();

      if ( !expDialog.exportTable( sheet->tableName() ) )
      {
        continue;
      }

      if ( !first || expDialog.printAlwaysTableDelimiter() )
      {
        if ( !first)
          str += "\n";

        QString name( expDialog.getTableDelimiter() );
        QString tname( i18n("<TABLENAME>") );
        int pos = name.find( tname );
        if ( pos != -1 )
        {
          name.replace( pos, tname.length(), sheet->tableName() );
        }
        str += name;
        str += "\n\n";
      }

      first = false;

      // Either we get hold of KSpreadSheet::m_dctCells and apply the old method below (for sorting)
      // or, cleaner and already sorted, we use KSpreadSheet's API (slower probably, though)
      int iMaxColumn = sheet->maxColumn();
      int iMaxRow    = sheet->maxRow();
      kdDebug(30501) << "Max row x column: " << iMaxRow << " x " << iMaxColumn << endl;
      
      // this is just a bad approximation which fails for documents with less than 50 rows, but
      // we don't need any progress stuff there anyway :) (Werner)
      int value = 0;
      int step = iMaxRow > 50 ? iMaxRow/50 : 1;
      int i = 1;

      QString emptyLines;
      for ( int currentrow = 1 ; currentrow < iMaxRow ; ++currentrow, ++i )
      {
        if ( i > step )
        {
          value += 2;
          emit sigProgress(value);
          i = 0;
        }
          
        QString separators;
        QString line;
        for ( int currentcolumn = 1 ; currentcolumn < iMaxColumn ; currentcolumn++ )
        {
          exportCell( sheet, currentcolumn, currentrow, separators, line, csvDelimiter, textQuote );
        }
        if ( !line.isEmpty() )
        {
          str += emptyLines;
          str += line;
          emptyLines = QString::null;
        }
        // Append a CR, but in a temp string -> if no other real line,
        // then those will be dropped
        emptyLines += "\n";
      }
    }
  }
  str += "\n"; // Last CR
  emit sigProgress(100);

  // Ok, now write to export file
  QCString cstr(str.local8Bit()); // I assume people will prefer local8Bit over utf8... Another param ?

  QFile out(m_chain->outputFile());
  if ( !out.open( IO_WriteOnly ) ) 
  {
    kdError(30501) << "Unable to open output file!" << endl;
    out.close();
    return KoFilter::StupidError;
  }
  out.writeBlock(cstr.data(), cstr.length());

  out.close();
  return KoFilter::OK;
}

#include <csvexport.moc>
