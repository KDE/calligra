/* This file is part of the KDE project
   Copyright (C) 1999 David Faure <faure@kde.org>

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
#include <kspread_table.h>
#include <kspread_cell.h>

#include <csvdialog.h>

// hehe >:->

/*
 To generate a test CSV file:

 perl -e '$i=0;while($i<30000) { print rand().",".rand()."\n"; $i++ }' > file.csv
*/

typedef KGenericFactory<CSVFilter, KoFilter> CSVImportFactory;
K_EXPORT_COMPONENT_FACTORY( libcsvimport, CSVImportFactory( "csvfilter" ) );

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

    QByteArray inputFile(in.size());
    in.readBlock(inputFile.data(), in.size());
    in.close();

    CSVDialog *dialog = new CSVDialog(0L, inputFile, csv_delimiter);
    if (!dialog->exec())
        return KoFilter::UserCancelled;

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

    for (int row = 0; row < numRows; ++row)
        for (int col = 0; col < numCols; ++col)
        {
            value += step;
            emit sigProgress(value);
            table->setText(row + 1, col + 1, dialog->getText(row, col), false);
            cell = table->cellAt( col + 1, row + 1, false );

            switch (dialog->getHeader(col))
            {
            case CSVDialog::TEXT:
                break;
            case CSVDialog::NUMBER:
                cell->setFormatType(KSpreadCell::Number);
                cell->setPrecision(2);
                break;
            case CSVDialog::DATE:
                cell->setFormatType(KSpreadCell::ShortDate);
                break;
            case CSVDialog::CURRENCY:
                cell->setFormatType(KSpreadCell::Money);
                break;
            }
        }

    emit sigProgress(100);
    QApplication::restoreOverrideCursor();
    delete dialog;

    return KoFilter::OK;
}

#include <csvimport.moc>
