/* This file is part of the KDE project
   Copyright (C) 2001 Eva Brucherseifer <eva@kde.org>
   based on kspread csv export filter by David Faure

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

#include <htmlexport.h>
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
#include <koDocumentInfo.h>

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


HTMLExport::HTMLExport(KoFilter *parent, const char *name) :
                     KoFilter(parent, name) {
}

// HTML enitities, AFAIK we don't need to escape " to &quot; (dnaber):
const QString strAmp ("&amp;");
const QString strLt  ("&lt;");
const QString strGt  ("&gt;");
const QRegExp regExpAmp ("&");
const QRegExp regExpLt  ("<");
const QRegExp regExpGt  (">");

// The reason why we use the KoDocument* approach and not the QDomDocument
// approach is because we don't want to export formulas but values !
bool HTMLExport::filterExport(const QString &file, KoDocument * document,
                         const QString &from, const QString &to,
                         const QString &config) {

    if(strcmp(document->className(), "KSpreadDoc")!=0)  // it's safer that way :)
    {
        kdWarning(30501) << "document isn't a KSpreadDoc but a " << document->className() << endl;
        return false;
    }
    if(to!="text/html" || from!="application/x-kspread")
    {
        kdWarning(30501) << "Invalid mimetypes " << to << " " << from << endl;
        return false;
    }

    const KSpreadDoc * ksdoc=static_cast<const KSpreadDoc *>(document);

    if( ksdoc->mimeType() != "application/x-kspread" )
    {
        kdWarning(30501) << "Invalid document mimetype " << ksdoc->mimeType() << endl;
        return false;
    }

    QString html_table_tag = "table";
    QString html_table_options = " border=\"1\"";
    QString html_row_tag = "tr";
    QString html_row_options = "";
    QString html_cell_tag = "td";
    QString html_cell_options = "";
    QString html_bold = "b";
    QString html_italic = "i";
    QString html_underline = "u";

    // Ah ah ah - the document is const, but the map and table aren't. Safety:0.
    QString str;
    QString emptyLines;

    QString title;
    KoDocumentInfo *info = document->documentInfo();
    KoDocumentInfoAbout *aboutPage = static_cast<KoDocumentInfoAbout *>(info->page( "about" ));
    if ( aboutPage && !aboutPage->title().isEmpty() )
      title = aboutPage->title();
    else
      title = file;
    
    // header
    str = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" ";
    str += " \"http://www.w3.org/TR/html4/loose.dtd\"> \n";
    str += "<html>\n";
    str += "<head>\n";
    // TODO: possibility of choosing other encodings
    str += "<meta http-equiv=\"Content-Type\" ";
    str += "content=\"text/html; charset=UTF-8\">\n";
    str += "<meta name=\"Generator\" ";
    str += "content=\"KSpread HTML Export Filter Version = 0.2\">\n";
    // I have no idea where to get the document name from :-(  table->tableName()
    str += "<title>" + title + "</title>\n";
    str += "</head>\n";
    str += "<body bgcolor=\"#FFFFFF\">\n";

    // Now get hold of the table to export
    // (Hey, this could be part of the dialog too, choosing which table to export....
    //  It's great to have parametrable filters... IIRC even MSOffice doesn't have that)
    // Ok, for now we'll use the first table - my document has only one table anyway ;-)))
    KSpreadTable * table = ksdoc->map()->firstTable();
    
    while (table != 0)
    {
    
      // Either we get hold of KSpreadTable::m_dctCells and apply the old method below (for sorting)
      // or, cleaner and already sorted, we use KSpreadTable's API (slower probably, though)
      int iMaxColumn = table->maxColumn();
      int iMaxRow = table->maxRow();

      // determine max number of used columns to make rect table
      int iUsedColumn=0;
      int iMaxUsedColumn=0;
      int iMaxUsedRow=0;
      int currentrow, currentcolumn;

      for ( currentrow = 1 ; currentrow < iMaxRow ; ++currentrow)
      {
        KSpreadCell * cell;
        iUsedColumn=0;
        for ( currentcolumn = 1 ; currentcolumn < iMaxColumn ; currentcolumn++ )
        {
            cell = table->cellAt( currentcolumn, currentrow, true );
            QString text;
            if ( !cell->isDefault() && !cell->isEmpty() )
            {
                iUsedColumn = currentcolumn;
            }
        }
        iUsedColumn += cell->extraXCells();
	if (iUsedColumn > iMaxUsedColumn)
	  iMaxUsedColumn = iUsedColumn;
	if ( iUsedColumn > 0 )
	  iMaxUsedRow = currentrow;

      }
      iMaxUsedRow++;
      iMaxUsedColumn++;

      // this is just a bad approximation which fails for documents with less than 50 rows, but
      // we don't need any progress stuff there anyway :) (Werner)
      int value=0;
      int step=iMaxRow > 50 ? iMaxRow/50 : 1;
      int i=1;

      str += "<" + html_table_tag + html_table_options + ">\n";
    
      unsigned int nonempty_cells_prev=0;

      for ( currentrow = 1 ; currentrow < iMaxUsedRow ; ++currentrow, ++i )
      {
        if(i>step) {
            value+=2;
            emit sigProgress(value);
            i=0;
        }

        QString separators;
        QString line;
        unsigned int nonempty_cells=0;
        unsigned int colspan_cells=0;

        for ( currentcolumn = 1 ; currentcolumn < iMaxUsedColumn ; currentcolumn++ )
        {
            KSpreadCell * cell = table->cellAt( currentcolumn, currentrow, true );
            colspan_cells=cell->extraXCells();
            if (cell->needsPrinting())
              nonempty_cells++;
            QString text;
            QColor bgcolor = cell->bgColor(currentcolumn,currentrow);
            // FIXME: some formatting seems to be missing with cell->text(), e.g.
            // "208.00" in KSpread will be "208" in HTML (not always?!)
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
            text = cell->prefix(currentrow, currentcolumn) + " " + text + " " 
                 + cell->postfix(currentrow, currentcolumn);
            line += "  <" + html_cell_tag + html_cell_options;
            if (bgcolor.isValid() && bgcolor.name()!="#ffffff") // change color only for non-white cells
              line += " bgcolor=\"" + bgcolor.name() + "\"";
            if (cell->extraXCells()>0)
            {
              QString tmp;
              int extra_cells=cell->extraXCells();
              line += " colspan=\"" + tmp.setNum(extra_cells+1) + "\"";
              currentcolumn += extra_cells;
            }
            text = text.stripWhiteSpace();
            if( text.at(0) == '!' ) {
              // this is supposed to be markup, just remove the '!':
              text = text.right(text.length()-1);
            } else {
              // Escape HTML characters. No need to be very efficient IMHO,
              // so use a RegExp:
              text.replace (regExpAmp , strAmp)
                  .replace (regExpLt  , strLt)
                  .replace (regExpGt  , strGt);
            }
            line += ">\n";

            if (cell->textFontBold(currentcolumn,currentrow))
            {
              text.insert(0, "<" + html_bold + ">");
              text.append("</" + html_bold + ">");
            }
            if (cell->textFontItalic(currentcolumn,currentrow))
            {
              text.insert(0, "<" + html_italic + ">");
              text.append("</" + html_italic + ">");
            }
            if (cell->textFontUnderline(currentcolumn,currentrow))
            {
              text.insert(0, "<" + html_underline + ">");
              text.append("</" + html_underline + ">");
            }
            QColor textColor = cell->textColor(currentcolumn,currentrow);
            if (textColor.isValid() && textColor.name()!="#000000") // change color only for non-default text
            {
              text.insert(0, "<font color=\"" + textColor.name() + "\">");
              text.append("</font>");
            }
            line += "  " + text;
            line += "\n  </" + html_cell_tag + ">\n";
        }

        if (nonempty_cells == 0 && nonempty_cells_prev == 0) {
          nonempty_cells_prev = nonempty_cells;
	  // skip line if there's more than one empty line
	  continue;
        } else {
          nonempty_cells_prev = nonempty_cells;
          str += emptyLines;
          str += "<" + html_row_tag + html_row_options + ">\n";
          str += line;
          str += "</" + html_row_tag + ">";
          emptyLines = QString::null;
          // Append a CR, but in a temp string -> if no other real line,
          // then those will be dropped
          emptyLines += "\n";
        }
      }
      str += "\n</" + html_table_tag + ">\n<br>\n";

    table = ksdoc->map()->nextTable();
    }

    str += "</body>\n";
    str += "</html>\n";
    str += "\n"; // Last CR
    emit sigProgress(100);

    // Ok, now write to export file
    QFile out(file);
    if(!out.open(IO_WriteOnly)) {
        kdError(30501) << "Unable to open output file!" << endl;
        out.close();
        return false;
    }
    QTextStream streamOut(&out);
    streamOut.setEncoding( QTextStream::UnicodeUTF8 ); // TODO: possibility of choosing other encodings
    streamOut << str << endl;
    out.close();

    return true;
}

/*
bool HTMLExport::filter(const QString &fileIn, const QString &fileOut,
                       const QString& from, const QString& to,
                       const QString &config) {

    if(to!="text/html" || from!="application/x-kspread")
    {
        kdWarning(30501) << "Invalid mimetypes " << to << " " << from << endl;
        return false;
    }

    KoStore in=KoStore(QString(fileIn), KoStore::Read);
    if(!in.open("root", "")) {
        QApplication::restoreOverrideCursor();
        KMessageBox::sorry( 0L, i18n("HTML filter can't open input file %1 - please report.").arg( fileIn ) );
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
#include <htmlexport.moc>
