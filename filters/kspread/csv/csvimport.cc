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

#include "csvdialog.h"

#include <csvimport.h>
#include <qmessagebox.h>
#include <kmessagebox.h>

// hehe >:->
#include <kspread_doc.h>
#include <kspread_table.h>
#include <kspread_cell.h>
/*
 To generate a test CSV file:

 perl -e '$i=0;while($i<30000) { print rand().",".rand()."\n"; $i++ }' > file.csv
*/

CSVFilter::CSVFilter(KoFilter *parent, const char*name) :
                     KoFilter(parent, name) {
}

bool CSVFilter::filterImport(const QString &file, KoDocument *document,
                         const QString &from, const QString &to,
                         const QString &config)
{
    kdDebug(30501) << "here we go... " << document->className() << endl;

    if(strcmp(document->className(), "KSpreadDoc")!=0)  // it's safer that way :)
    {
        kdWarning(30501) << "document isn't a KSpreadDoc but a " << document->className() << endl;
        return false;
    }
    if(from!="text/x-csv" && from!="text/plain" || to!="application/x-kspread")
    {
        kdWarning(30501) << "Invalid mimetypes " << from << " " << to << endl;
        return false;
    }

    kdDebug(30501) << "...still here..." << endl;

    // No need for a dynamic cast here, since we use Qt's moc magic
    KSpreadDoc *ksdoc=(KSpreadDoc*)document;

    if(ksdoc->mimeType()!="application/x-kspread")
    {
        kdWarning(30501) << "Invalid document mimetype " << ksdoc->mimeType() << endl;
        return false;
    }

    QFile in(file);
    if(!in.open(IO_ReadOnly)) {
        KMessageBox::sorry( 0L, i18n("CSV filter can't open input file - please report.") );
        in.close();
        return false;
    }

    QString csv_delimiter = QString::null;
    if (config != QString::null)
        csv_delimiter = config[0];

    QByteArray inputFile(in.size());
    in.readBlock(inputFile.data(), in.size());
    in.close();

    CSVDialog *dialog = new CSVDialog(0L, inputFile, csv_delimiter);
    if (!dialog->exec())
        return false;

    KSpreadCell *cell;
    KSpreadTable *table=ksdoc->createTable();
    ksdoc->addTable(table);

    int numRows = dialog->getRows();
    int numCols = dialog->getCols();
    int step = 100 / numRows * numCols;
    int value = 0;

    emit sigProgress(value);

    for (int row = 0; row < numRows; ++row)
        for (int col = 0; col < numCols; ++col)
        {
            value += step;
            emit sigProgress(value);
            table->setText(row + 1, col + 1, dialog->getText(row, col), false);
            cell = table->cellAt(col + 1, row + 1);

            switch (dialog->getHeader(col))
            {
            case CSVDialog::TEXT:
                break;
            case CSVDialog::NUMBER:
                cell->setFormatNumber(KSpreadCell::Number);
                cell->setPrecision(2);
                break;
            case CSVDialog::DATE:
                cell->setFormatNumber(KSpreadCell::ShortDate);
                break;
            case CSVDialog::CURRENCY:
                cell->setFormatNumber(KSpreadCell::Money);
                break;
            }
        }

    emit sigProgress(100);
    delete dialog;
    return true;
}

#include <csvimport.moc>
