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
#include <qstring.h>
#include <qfile.h>
#include <qapplication.h>
#include <qlist.h>
#include <qsortedlist.h>

#include <kspread_map.h>
#include <kspread_table.h>
#include <kspread_doc.h>

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


CSVExport::CSVExport(KoFilter *parent, const char *name) :
                     KoFilter(parent, name) {
}


// The reason why we use the KoDocument* approach and not the QDomDocument
// approach is because we don't want to export formulas but values !
bool CSVExport::filterExport(const QString &file, KoDocument * document,
                         const QString &from, const QString &to,
                         const QString &config) {

    if(strcmp(document->className(), "KSpreadDoc")!=0)  // it's safer that way :)
    {
        kdWarning(30501) << "document isn't a KSpreadDoc but a " << document->className() << endl;
        return false;
    }
    if(to!="text/x-csv" || from!="application/x-kspread")
    {
        kdWarning(30501) << "Invalid mimetypes " << to << " " << from << endl;
        return false;
    }

    const KSpreadDoc * const ksdoc=(const KSpreadDoc* const)document;

    if( ksdoc->mimeType() != "application/x-kspread" )
    {
        kdWarning(30501) << "Invalid document mimetype " << ksdoc->mimeType() << endl;
        return false;
    }

    QChar csv_delimiter;
    if(config.isEmpty())
        csv_delimiter = ',';
    else
        csv_delimiter = config[0];

    // Now get hold of the table to export
    // (Hey, this could be part of the dialog too, choosing which table to export....
    //  It's great to have parametrable filters... IIRC even MSOffice doesn't have that)
    // Ok, for now we'll use the first table - my document has only one table anyway ;-)))
    KSpreadTable * table = ksdoc->map()->firstTable();

    // Ah ah ah - the document is const, but the map and table aren't. Safety: 0.

    QString str;

    // Either we get hold of KSpreadTable::m_dctCells and apply the old method below (for sorting)
    // or, cleaner and already sorted, we use KSpreadTable's API (slower probably, though)
    int iMaxColumn = table->maxColumn();
    int iMaxRow = table->maxRow();

    // this is just a bad approximation which fails for documents with less than 50 rows, but
    // we don't need any progress stuff there anyway :) (Werner)
    int value=0;
    int step=iMaxRow > 50 ? iMaxRow/50 : 1;
    int i=1;

    QString emptyLines;
    for ( int currentrow = 1 ; currentrow < iMaxRow ; ++currentrow, ++i )
    {
        if(i>step) {
            value+=2;
            emit sigProgress(value);
            i=0;
        }

        QString separators;
        QString line;
        for ( int currentcolumn = 1 ; currentcolumn < iMaxColumn ; currentcolumn++ )
        {
            KSpreadCell * cell = table->cellAt( currentcolumn, currentrow, true );
            QString text;
            if ( !cell->isDefault() && !cell->isEmpty() )
            {
              switch( cell->content() ) {
                case KSpreadCell::Text:
                  text = cell->text();
                  break;
                case KSpreadCell::RichText:
                case KSpreadCell::VisualFormula:
                  text = cell->text(); // untested
                  break;
                case KSpreadCell::Formula:
                  cell->calc( TRUE ); // Incredible, cells are not calculated if the document was just opened
                  text = cell->valueString();
                  break;
              }
            }
            if ( !text.isEmpty() )
            {
                line += separators;
		if(text.find(csv_delimiter)!=-1)
		  text="\""+text+"\"";
                line += text;
                separators = QString::null;
            }
            // Append a delimiter, but in a temp string -> if no other real cell in this line,
            // then those will be dropped
            separators += csv_delimiter;
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
    str += "\n"; // Last CR
    emit sigProgress(100);

    // Ok, now write to export file
    QCString cstr(str.local8Bit()); // I assume people will prefer local8Bit over utf8... Another param ?

    QFile out(file);
    if(!out.open(IO_WriteOnly)) {
        kdError(30501) << "Unable to open output file!" << endl;
        out.close();
        return false;
    }
    out.writeBlock(cstr.data(), cstr.length());

    out.close();
    return true;
}

/*
bool CSVExport::filter(const QString &fileIn, const QString &fileOut,
                       const QString& from, const QString& to,
                       const QString &config) {

    if(to!="text/x-csv" || from!="application/x-kspread")
    {
        kdWarning(30501) << "Invalid mimetypes " << to << " " << from << endl;
        return false;
    }

    KoStore in=KoStore(QString(fileIn), KoStore::Read);
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
        kdError(30501) << "Unable to open output file!" << endl;
        in.close();
        out.close();
        return false;
    }
    out.writeBlock(cstr.data(), cstr.length());

    in.close();
    out.close();
    return true;
}
*/
#include <csvexport.moc>
