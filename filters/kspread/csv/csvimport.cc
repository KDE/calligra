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

const bool CSVFilter::I_filter(const QString &file, KoDocument *document,
                               const QString &from, const QString &to,
                               const QString &config) {
    bool bSuccess=true;

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

    QTextStream inputStream(&in);
    QChar csv_delimiter;

    // is there a config info or do we have to use a dialog box?
    if(config!=QString::null) {
        kdDebug(30501) << "CSVFilter::CSVFilter(): config found... " << config << endl;
        csv_delimiter=QChar(config[0]);
    }
    else {
        QString firstLine = inputStream.readLine();
        firstLine.truncate(100);
        switch (QMessageBox::information( 0L, i18n( "Information needed" ),
                                  i18n( "What is the separator used in this file ? First line is \n%1" ).arg(firstLine),
                                  i18n( "Comma" ), i18n( "Semicolon" ), i18n( "Tabulator" ) )) {
            case 2:
                csv_delimiter = '\t';
                break;
            case 1:
                csv_delimiter = ';';
                break;
            default:
                csv_delimiter = ','; // "Comma" chosen or Escape typed
        }
        // Now rewind to the beginning of the file
        in.at(0);
    }

    KSpreadTable *table=ksdoc->createTable();
    ksdoc->addTable(table);
    int row=1, column=1;

    QChar x;
    enum { S_START, S_QUOTED_FIELD, S_MAYBE_END_OF_QUOTED_FIELD, S_NORMAL_FIELD } state = S_START;
    QString field = "";
    int step=in.size()/50;
    int value=0;
    int i=0;
    emit sigProgress(value);

    while ( !inputStream.eof() && bSuccess==true )
    {
        ++i;
        if(i>step) {
            kdDebug() << "emitted" << endl;
            i=0;
            value+=2;
            emit sigProgress(value);
        }
        inputStream >> x; // read one char

        if (x == '\r') inputStream >> x; // eat '\r', to handle DOS/LOSEDOWS files correctly
        switch (state)
        {
            case S_START :
                if (x == '"') state = S_QUOTED_FIELD;
                else if (x == csv_delimiter)
                    ++column;
                else if (x == '\n')  {
                    ++row;
                    column=1;
                }
                else
                {
                    field += x;
                    state = S_NORMAL_FIELD;
                }
                break;
            case S_QUOTED_FIELD :
                if (x == '"') state = S_MAYBE_END_OF_QUOTED_FIELD;
                else field += x;
                break;
            case S_MAYBE_END_OF_QUOTED_FIELD :
                if (x == '"')
                {
                    field += x;
                    state = S_QUOTED_FIELD;
                } else if (x == csv_delimiter || x == '\n')
                {
                    table->setText(row, column, field, false);
                    field = "";
                    if (x == '\n') {
                        ++row;
                        column=1;
                    }
                    else
                        ++column;
                    state = S_START;
                } else
                { // This field wasn't quoted. It was "blah"something
                    state = S_NORMAL_FIELD;
                    field.prepend('"');
                    field += '"';
                    field += x;
                }
                break;
            case S_NORMAL_FIELD :
                if (x == csv_delimiter || x == '\n')
                {
                    table->setText(row, column, field, false);
                    field = "";
                    if (x == '\n') {
                        ++row;
                        column=1;
                    }
                    else
                        ++column;
                    state = S_START;
                }
                else
                    field += x;
        }
    }
    emit sigProgress(100);
    in.close();
    return bSuccess;
}

#include <csvimport.moc>
