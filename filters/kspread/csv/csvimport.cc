/* This file is part of the KDE project
   Copyright (C) 1999 David Faure <faure@kde.org>
   Copyright (C) 2004 Nicolas GOUTTE <goutte@kde.org>

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

#include <csvimport.h>

#include <qfile.h>

#include <kapplication.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kgenericfactory.h>

#include <koFilterChain.h>
#include <kspread_doc.h>
#include <kspread_global.h>
#include <kspread_sheet.h>
#include <kspread_style.h>
#include <kspread_style_manager.h>
#include <kspread_cell.h>

#include <csvdialog.h>

// hehe >:->

/*
 To generate a test CSV file:

 perl -e '$i=0;while($i<30000) { print rand().",".rand()."\n"; $i++ }' > file.csv
*/

typedef KGenericFactory<CSVFilter, KoFilter> CSVImportFactory;
K_EXPORT_COMPONENT_FACTORY( libcsvimport, CSVImportFactory( "kofficefilters" ) )

CSVFilter::CSVFilter(KoFilter *, const char*, const QStringList&) :
                     KoFilter() {
}

KoFilter::ConversionStatus CSVFilter::convert( const QCString& from, const QCString& to )
{
    QString file( m_chain->inputFile() );
    KoDocument* document = m_chain->outputDocument();

    if ( !document )
        return KoFilter::StupidError;

    kdDebug(30501) << "here we go... " << document->className() << endl;

    if(strcmp(document->className(), "KSpreadDoc")!=0)  // it's safer that way :)
    {
        kdWarning(30501) << "document isn't a KSpreadDoc but a " << document->className() << endl;
        return KoFilter::NotImplemented;
    }
    if(from!="text/x-csv" && from!="text/plain" || to!="application/x-kspread")
    {
        kdWarning(30501) << "Invalid mimetypes " << from << " " << to << endl;
        return KoFilter::NotImplemented;
    }

    kdDebug(30501) << "...still here..." << endl;

    // No need for a dynamic cast here, since we use Qt's moc magic
    KSpreadDoc *ksdoc=(KSpreadDoc*)document;

    if(ksdoc->mimeType()!="application/x-kspread")
    {
        kdWarning(30501) << "Invalid document mimetype " << ksdoc->mimeType() << endl;
        return KoFilter::NotImplemented;
    }

    QFile in(file);
    if(!in.open(IO_ReadOnly)) {
        KMessageBox::sorry( 0L, i18n("CSV filter can't open input file - please report.") );
        in.close();
        return KoFilter::FileNotFound;
    }

    QString csv_delimiter = QString::null;
    // ###### FIXME: disabled for now
    //if (config != QString::null)
    //    csv_delimiter = config[0];

    QByteArray inputFile( in.readAll() );
    in.close();

    CSVDialog *dialog = new CSVDialog(0L, inputFile, csv_delimiter);
    if (!dialog->exec())
        return KoFilter::UserCancelled;
    inputFile.resize( 0 ); // Release memory (input file content)

    ElapsedTime t( "Filling data into document" );

    KSpreadCell *cell;
    KSpreadSheet *table=ksdoc->createTable();
    ksdoc->addTable(table);

    int numRows = dialog->getRows();
    int numCols = dialog->getCols();

    if (numRows == 0)
      ++numRows;

    int step = 100 / numRows * numCols;
    int value = 0;

    emit sigProgress(value);
    QApplication::setOverrideCursor(Qt::waitCursor);

    int i;
    double init = table->nonDefaultColumnFormat( 1 )->dblWidth();
    QMemArray<double> widths( numCols );
    for ( i = 0; i < numCols; ++i )
      widths[i] = init;

    KSpreadCell * c = table->nonDefaultCell( 1, 1 );
    QFontMetrics fm( c->textFont( 1, 1 ) );
    double width = fm.width('x');

    KSpreadStyle * s = ksdoc->styleManager()->defaultStyle();

    for ( int row = 0; row < numRows; ++row )
    {
        for (int col = 0; col < numCols; ++col)
        {
            value += step;
            emit sigProgress(value);
            QString text( dialog->getText( row, col ) );

            double len = (double) text.length() * width;
            if ( len > widths[col] )
              widths[col] = len;
            double d;
            bool ok = false;

            switch (dialog->getHeader(col))
            {
             case CSVDialog::TEXT:
              cell = table->nonDefaultCell( col + 1, row + 1, false, s );
              cell->setCellText( text, false, true );
              break;
             case CSVDialog::NUMBER:
              d = ksdoc->locale()->readNumber( text, &ok );
              // If not, try with the '.' as decimal separator
              if ( !ok )
                d = text.toDouble( &ok );
              if ( !ok )
              {
                cell = table->nonDefaultCell( col + 1, row + 1, false, s );
                cell->setCellText( text, false, true );
                cell->setFormatType( KSpreadCell::Number );
              }
              else
              {
                cell = table->nonDefaultCell( col + 1, row + 1, false, s );
                cell->setNumber( d );
              }
              cell->setPrecision( 2 );
              break;
             case CSVDialog::DATE:
              cell = table->nonDefaultCell( col + 1, row + 1, false, s );
              cell->setDate( text );
              cell->setFormatType( KSpreadCell::ShortDate );
              break;
             case CSVDialog::CURRENCY:
              cell = table->nonDefaultCell( col + 1, row + 1, false, s );
              cell->setCellText( text, false, false );
              cell->setFormatType( KSpreadCell::Money );
              cell->setPrecision( 2 );
              break;
            }
        }
    }

    emit sigProgress( 98 );

    ElapsedTime t2( "Resizing columns" );
    for ( i = 0; i < numCols; ++i )
    {
      ColumnFormat * c  = table->nonDefaultColumnFormat( i + 1 );
      c->setDblWidth( widths[i] );
    }

    emit sigProgress( 100 );
    QApplication::restoreOverrideCursor();
    delete dialog;

    return KoFilter::OK;
}

#include <csvimport.moc>
