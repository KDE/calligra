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
        //kDebugError( 31501, "Unable to open input file!");
        KMessageBox::sorry( 0L, i18n("CSV filter can't open input file - please report.") );
        in.close();
        return 0;
    }

    QTextStream inputStream(&in);
    XMLTree tree(document);

    QChar csv_delimiter;

    // is there a config info or do we have to use a dialog box?
    if(config!=QString::null) {
        kDebugInfo(30003, "CSVFilter::CSVFilter(): config found");
        kDebugInfo(30003, config);
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

    //for debuggging only
#if 0
    kdDebug(31501) << "XXYYYYYYZZ" << endl;
    QString tmp=tree.part();
    kdDebug(31501) << "Size: " << tmp.length() << endl
    		   << "String: " << tmp << endl;
    kdDebug(31501) << "XXYYYYYYZZ" << endl;

#if 0
    KoTarStore out=KoTarStore("/tmp/debug_csvfilter.tgz", KoStore::Write);
    if(!out.open("root", "")) {
        kDebugError( 31501, "Unable to open output file!");
        in.close();
        out.close();
        return false;
    }
    out.write((const char*)tmp, tmp.length());
    kDebugInfo( 31501, "%s", tmp.data());
    out.close();
#endif
#endif

    in.close();
    return bSuccess;
}
