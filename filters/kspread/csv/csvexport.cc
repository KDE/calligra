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
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <qdom.h>
#include <qapp.h>
#include <qlist.h>
#include <qsortedlist.h>

class Cell {
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


CSVExport::CSVExport(KoFilter *parent, QString name) :
                     KoFilter(parent, name) {
}

const bool CSVExport::filter(const QCString &fileIn, const QCString &fileOut,
                               const QCString& from, const QCString& to,
                               const QString &config) {

    if(to!="text/x-csv" || from!="application/x-kspread")
    {
        kdWarning(30501) << "Invalid mimetypes " << to << " " << from << endl;
        return false;
    }

    KoTarStore in=KoTarStore(QString(fileIn), KoStore::Read);
    if(!in.open("root", "")) {
        QApplication::restoreOverrideCursor();
        KMessageBox::sorry( 0L, i18n("CSV filter can't open input file %1 - please report.").arg( fileIn ) );
        in.close();
        return false;
    }
    // To be made configurable (filter dialog or dialog box), perhaps
    // My opinion : CSV means comma separated values, and if we use
    //  '.' as decimal separator then everything is fine, and the apps
    // should do the conversion after loading.
    // But those who wrote French and German versions of Excel didn't think so...
    //QChar decimal_point = '.';
    QChar csv_delimiter;
    if(config.isEmpty())
	csv_delimiter = ',';
    else
	csv_delimiter = config[0];

    // read the whole file
    QByteArray array=in.read(in.size());
    QString text = QString::fromUtf8( array.data(), array.size() );
    QDomDocument doc;
    doc.setContent( text );
    QDomElement elem = doc.documentElement();
    if( elem.attribute( "mime" ) != "application/x-kspread" )
    {
        kdWarning(30501) << "Invalid document mimetype " << elem.attribute( "mime" ) << endl;
        return false;
    }

    QDomElement map = elem.namedItem("map").toElement();
    QDomElement table = map.namedItem("table").toElement();

    QString str;
    QDomNodeList cells = table.childNodes();
    // Each cell contains a row and a column number.
    // Damn, and they're not necessarily sorted...
    // -> we need to store everything and sort it out
    QSortedList< Cell > list;
    list.setAutoDelete( true );
    for ( uint i = 0 ; i < cells.length() ; i++ )
    {
        QDomNode cellNode = cells.item( i );
        QDomElement cell = cellNode.toElement();
        Cell * newCell = new Cell;
        newCell->row = cell.attribute("row").toInt();
        newCell->col = cell.attribute("column").toInt();
        newCell->text = cell.text();
        list.inSort( newCell );
    }

    int currentrow = 1;
    int currentcolumn = 1;
    for ( QListIterator<Cell> it ( list ) ; it.current(); ++it )
    {
        Cell * cell = it.current();

        for (  ; currentrow < cell->row ; currentrow++ )
        {
            str += "\n";
            currentcolumn = 1;
        }

        for ( ; currentcolumn < cell->col ; currentcolumn++ )
          str += csv_delimiter;

        str += cell->text;
    }

    QCString cstr(str.utf8());

    QFile out(fileOut);
    if(!out.open(IO_WriteOnly)) {
        kDebugError( 31502, "Unable to open output file!");
        in.close();
        out.close();
        return false;
    }
    out.writeBlock(cstr.data(), cstr.length());

    in.close();
    out.close();
    return true;
}

#include <csvexport.moc>

