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

#include <csvfilter.h>
#include <csvfilter.moc>
#include <qmessagebox.h>
#include <kmessagebox.h>

// hehe >:->
#include <kspread_doc.h>
#include <kspread_table.h>
#include <kspread_cell.h>

CSVFilter::CSVFilter(KoFilter *parent, QString name) :
                     KoFilter(parent, name) {
}

const bool CSVFilter::I_filter(const QCString &file, const QCString &from,
			       QDomDocument &document, const QCString &to,
			       const QString &config) {

    bool bSuccess=true;

    if(to!="application/x-kspread" || from!="text/x-csv")
        return 0;

    QFile in(file);
    if(!in.open(IO_ReadOnly)) {
        //kdError(30501) << "Unable to open input file!" << endl;
        KMessageBox::sorry( 0L, i18n("CSV filter can't open input file - please report.") );
        in.close();
        return 0;
    }

    QTextStream inputStream(&in);
    XMLTree tree(document);

    QChar csv_delimiter;

    // is there a config info or do we have to use a dialog box?
    if(config!=QString::null) {
        kdDebug(30501) << "CSVFilter::CSVFilter(): config found" << endl;
        kdDebug(30501) << config << endl;
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

    QChar x;
    enum { S_START, S_QUOTED_FIELD, S_MAYBE_END_OF_QUOTED_FIELD, S_NORMAL_FIELD } state = S_START;
    QString field = "";
    while ( !inputStream.eof() && bSuccess==true )
    {
        inputStream >> x; // read one char
        if (x == '\r') inputStream >> x; // eat '\r', to handle DOS/LOSEDOWS files correctly
        switch (state)
        {
            case S_START :
                if (x == '"') state = S_QUOTED_FIELD;
                else if (x == csv_delimiter || x == '\n') tree.emptycell();
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
                    tree.cell( field );
                    field = "";
                    if (x == '\n') tree.newline();
                    state = S_START;
                } else
                { // should never happen
                    field += "*** Error : unexpected character : ";
                    field += x;
                    state = S_START;
                    bSuccess=false;
                }
                break;
            case S_NORMAL_FIELD :
                if (x == csv_delimiter || x == '\n')
                {
                    tree.cell( field );
                    field = "";
                    if (x == '\n') tree.newline();
                    state = S_START;
                }
                else field += x;
        }
    }

    //for debugging only
#if 0
    kdDebug(30501) << "XXYYYYYYZZ" << endl;
    QString tmp=tree.part();
    kdDebug(30501) << "Size: " << tmp.length() << endl
    		   << "String: " << tmp << endl;
    kdDebug(30501) << "XXYYYYYYZZ" << endl;

#if 0
    KoTarStore out=KoTarStore("/tmp/debug_csvfilter.tgz", KoStore::Write);
    if(!out.open("root", "")) {
        kdError(30501) << "Unable to open output file!" << endl;
        in.close();
        out.close();
        return false;
    }
    out.write((const char*)tmp, tmp.length());
    kdDebug(30501) << tmp.data() << endl;
    out.close();
#endif
#endif

    in.close();
    return bSuccess;
}

const bool CSVFilter::I_filter(const QCString &file, KoDocument *document,
			       const QCString &from, const QCString &to,
			       const QString &config) {
    bool bSuccess=true;

    kdDebug(30501) << "here we go... " << document->className() << endl;

    if(strcmp(document->className(), "KSpreadDoc")!=0)  // it's safer that way :)
    {
        kdWarning(30501) << "document isn't a KSpreadDoc but a " << document->className() << endl;
    	return false;
    }
    if(from!="text/x-csv" || to!="application/x-kspread")
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
    while ( !inputStream.eof() && bSuccess==true )
    {
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
                    table->setText(row, column, field);
                    field = "";
                    if (x == '\n') {
			++row;
			column=1;
		    }
		    else
			++column;
                    state = S_START;
                } else
                { // should never happen
		    kdError(30501) << "Error: unexpected character!" << endl;
                    field += "*** Error : unexpected character : ";
                    field += x;
                    state = S_START;
                    bSuccess=false;
                }
                break;
            case S_NORMAL_FIELD :
                if (x == csv_delimiter || x == '\n')
                {
                    table->setText(row, column, field);
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
    in.close();
    return bSuccess;
}
