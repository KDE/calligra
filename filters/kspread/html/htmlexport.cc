/* This file is part of the KDE project
   Copyright (C) 2001 Eva Brucherseifer <eva@kde.org>
   Copyright (C) 2005 Bram Schoenmakers <bramschoenmakers@kde.nl>
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <htmlexport.h>
#include <exportdialog.h>

#include <qfile.h>
#include <qtextcodec.h>
//Added by qt3to4:
#include <QTextStream>
#include <Q3CString>

#include <kdebug.h>
#include <kgenericfactory.h>
#include <KoFilterChain.h>
#include <KoDocumentInfo.h>
#include <kofficeversion.h>

#include <kspread_map.h>
#include <kspread_sheet.h>
#include <kspread_doc.h>
#include <kspread_util.h>

using namespace KSpread;

typedef KGenericFactory<HTMLExport> HTMLExportFactory;
K_EXPORT_COMPONENT_FACTORY( libkspreadhtmlexport, HTMLExportFactory( "kofficefilters" ) )

const QString html_table_tag = "table";
const QString html_table_options = QString(" border=\"%1\" cellspacing=\"%2\"");
const QString html_row_tag = "tr";
const QString html_row_options = "";
const QString html_cell_tag = "td";
const QString html_cell_options = "";
const QString html_bold = "b";
const QString html_italic = "i";
const QString html_underline = "u";
const QString html_right= "right";
const QString html_left= "left";
const QString html_center= "center";
const QString html_top="top";
const QString html_bottom="bottom";
const QString html_middle="middle";
const QString html_h1="h1";

HTMLExport::HTMLExport(QObject* parent, const QStringList&) :
    KoFilter(parent), m_dialog( new ExportDialog() )
{
}

HTMLExport::~HTMLExport()
{
  delete m_dialog;
}

// HTML enitities, AFAIK we don't need to escape " to &quot; (dnaber):
const QString strAmp ("&amp;");
const QString nbsp ("&nbsp;");
const QString strLt  ("&lt;");
const QString strGt  ("&gt;");

// The reason why we use the KoDocument* approach and not the QDomDocument
// approach is because we don't want to export formulas but values !
KoFilter::ConversionStatus HTMLExport::convert( const QByteArray& from, const QByteArray& to )
{
    if(to!="text/html" || from!="application/x-kspread")
    {
      kWarning(30501) << "Invalid mimetypes " << to << " " << from << endl;
      return KoFilter::NotImplemented;
    }

    KoDocument* document = m_chain->inputDocument();

    if ( !document )
      return KoFilter::StupidError;

    if( !::qobject_cast<const KSpread::Doc *>( document ) )  // it's safer that way :)
    {
      kWarning(30501) << "document isn't a KSpread::Doc but a " << document->metaObject()->className() << endl;
      return KoFilter::NotImplemented;
    }

    const Doc * ksdoc=static_cast<const Doc *>(document);

    if( ksdoc->mimeType() != "application/x-kspread" )
    {
      kWarning(30501) << "Invalid document mimetype " << ksdoc->mimeType() << endl;
      return KoFilter::NotImplemented;
    }

    QString filenameBase = m_chain->outputFile();
    filenameBase = filenameBase.left( filenameBase.lastIndexOf( '.' ) );

    QStringList sheets;
    foreach ( Sheet* sheet, ksdoc->map()->sheetList() )
    {
      int rows = 0;
      int columns = 0;
      detectFilledCells( sheet, rows, columns );
      m_rowmap[ sheet->sheetName() ] = rows;
      m_columnmap[ sheet->sheetName() ] = columns;

      if( rows > 0 && columns > 0 )
      {
        sheets.append( sheet->sheetName() );
      }
    }
    m_dialog->setSheets( sheets );

    if( m_dialog->exec() == QDialog::Rejected )
      return KoFilter::UserCancelled;

    Sheet* sheet = 0;
    sheets = m_dialog->sheets();
    QString str;
    for( int i = 0; i < sheets.count() ; ++i )
    {
      sheet = ksdoc->map()->findSheet( sheets[i] );

      QString file = fileName( filenameBase, sheet->sheetName(), sheets.count() > 1 );

      if( m_dialog->separateFiles() || sheets[i] == sheets.first() )
      {
        str = QString::null;
        openPage( sheet, document, str );
        writeTOC( sheets, filenameBase, str );
      }

      convertSheet( sheet, str, m_rowmap[ sheet->sheetName() ], m_columnmap[ sheet->sheetName() ] );

      if( m_dialog->separateFiles() || sheets[i] == sheets.last() )
      {
        closePage( str );
        QFile out(file);
        if(!out.open(QIODevice::WriteOnly)) {
          kError(30501) << "Unable to open output file!" << endl;
          out.close();
          return KoFilter::FileNotFound;
        }
        QTextStream streamOut(&out);
        streamOut.setCodec( m_dialog->encoding() );
        streamOut << str << endl;
        out.close();
      }

      if( !m_dialog->separateFiles() )
      {
        createSheetSeparator( str );
      }

    }

    emit sigProgress(100);
    return KoFilter::OK;
}

void HTMLExport::openPage( Sheet *sheet, KoDocument *document, QString &str )
{
  QString title;
  KoDocumentInfo *info = document->documentInfo();
  if ( info && !info->aboutInfo( "title" ).isEmpty() )
    title = info->aboutInfo( "title" ) + " - ";
  title += sheet->sheetName();

      // header
  str = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" ";
  str += " \"http://www.w3.org/TR/html4/loose.dtd\"> \n";
  str += "<html>\n";
  str += "<head>\n";
  str += "<meta http-equiv=\"Content-Type\" ";
  str += QString("content=\"text/html; charset=%1\">\n").arg( QString(m_dialog->encoding()->name() ) );
  str += "<meta name=\"Generator\" ";
  str += "content=\"KSpread HTML Export Filter Version = ";
  str += KOFFICE_VERSION_STRING;
  str += "\">\n";

    // Insert stylesheet
  if( !m_dialog->customStyleURL().isEmpty() )
  {
    str += "<link ref=\"stylesheet\" type=\"text/css\" href=\"";
    str += m_dialog->customStyleURL();
    str += "\" title=\"Style\" >\n";
  }

  str += "<title>" + title + "</title>\n";
  str += "</head>\n";
  str += QString("<body bgcolor=\"#FFFFFF\" dir=\"%1\">\n").arg(
      (sheet->layoutDirection() == Sheet::RightToLeft) ? "rtl" : "ltr" );

  str += "<a name=\"__top\">\n";
}

void HTMLExport::closePage( QString &str )
{
  str += "<p align=\"" + html_center + "\"><a href=\"#__top\">" + i18n("Top") + "</a></p>\n";
  str += "</body>\n";
  str += "</html>\n\n";
}

void HTMLExport::convertSheet( Sheet *sheet, QString &str, int iMaxUsedRow, int iMaxUsedColumn )
{
    QString emptyLines;

    // Either we get hold of KSpreadTable::m_dctCells and apply the old method below (for sorting)
    // or, cleaner and already sorted, we use KSpreadTable's API (slower probably, though)
    int iMaxRow = sheet->maxRow();

    if( !m_dialog->separateFiles() )
        str += "<a name=\"" + sheet->sheetName().toLower().trimmed() + "\">\n";

    str += ("<h1>" + sheet->sheetName() + "</h1><br>\n");

    // this is just a bad approximation which fails for documents with less than 50 rows, but
    // we don't need any progress stuff there anyway :) (Werner)
    int value=0;
    int step=iMaxRow > 50 ? iMaxRow/50 : 1;
    int i=1;

    str += "<" + html_table_tag + html_table_options.arg( m_dialog->useBorders() ? "1" : "0" ).arg( m_dialog->pixelsBetweenCells() ) +
        QString("dir=\"%1\">\n").arg( (sheet->layoutDirection() == Sheet::RightToLeft ) ? "rtl" : "ltr" );

    unsigned int nonempty_cells_prev=0;

    for ( int currentrow = 1 ; currentrow <= iMaxUsedRow ; ++currentrow, ++i )
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

        for ( int currentcolumn = 1 ; currentcolumn <= iMaxUsedColumn ; currentcolumn++ )
        {
            Cell * cell = sheet->cellAt( currentcolumn, currentrow, false );
            colspan_cells=cell->extraXCells();
            if (cell->needsPrinting())
                nonempty_cells++;
            QString text;
            QColor bgcolor = cell->bgColor(currentcolumn,currentrow);
            // FIXME: some formatting seems to be missing with cell->text(), e.g.
            // "208.00" in KSpread will be "208" in HTML (not always?!)
            bool link = false;

            if ( !cell->link().isEmpty() )
            {
                if ( localReferenceAnchor(cell->link()) )
                {
                    text = cell->text();
                }
                else
                {
                    text = " <A href=\"" + cell->link() + "\">" + cell->text() + "</A>";
                    link = true;
                }
            }
            else
                text=cell->strOutText();
#if 0
            switch( cell->content() ) {
            case Cell::Text:
                text = cell->text();
                break;
            case Cell::RichText:
            case Cell::VisualFormula:
                text = cell->text(); // untested
                break;
            case Cell::Formula:
                cell->calc( TRUE ); // Incredible, cells are not calculated if the document was just opened
                text = cell->valueString();
                break;
            }
            text = cell->prefix(currentrow, currentcolumn) + " " + text + " "
                   + cell->postfix(currentrow, currentcolumn);
#endif
            line += "  <" + html_cell_tag + html_cell_options;
	    if (text.isRightToLeft() != (sheet->layoutDirection() == Sheet::RightToLeft))
                line += QString(" dir=\"%1\" ").arg(text.isRightToLeft()?"rtl":"ltr");
            if (bgcolor.isValid() && bgcolor.name()!="#ffffff") // change color only for non-white cells
                line += " bgcolor=\"" + bgcolor.name() + "\"";

            switch((Style::HAlign)cell->defineAlignX())
            {
            case Style::Left:
                line+=" align=\"" + html_left +"\"";
                break;
            case Style::Right:
                line+=" align=\"" + html_right +"\"";
                break;
            case Style::Center:
                line+=" align=\"" + html_center +"\"";
                break;
            case Style::HAlignUndefined:
                break;
            }
            switch((Style::VAlign)cell-> format()->alignY(currentrow, currentcolumn))
            {
            case Style::Top:
                line+=" valign=\"" + html_top +"\"";
                break;
            case Style::Middle:
                line+=" valign=\"" + html_middle +"\"";
                break;
            case Style::Bottom:
                line+=" valign=\"" + html_bottom +"\"";
                break;
            case Style::VAlignUndefined:
                break;
            }
            line+=" width=\""+QString::number(cell->width())+"\"";
            line+=" height=\""+QString::number(cell->height())+"\"";

            if (cell->extraXCells()>0)
            {
                QString tmp;
                int extra_cells=cell->extraXCells();
                line += " colspan=\"" + tmp.setNum(extra_cells+1) + "\"";
                currentcolumn += extra_cells;
            }
            text = text.trimmed();
            if( text.at(0) == '!' ) {
                // this is supposed to be markup, just remove the '!':
                text = text.right(text.length()-1);
            } else if ( !link ) {
                // Escape HTML characters.
                text.replace ('&' , strAmp)
                    .replace ('<' , strLt)
                    .replace ('>' , strGt)
                    .replace (' ' , nbsp);
            }
            line += ">\n";

            if (cell->format()->textFontBold(currentcolumn,currentrow))
            {
                text.insert(0, "<" + html_bold + ">");
                text.append("</" + html_bold + ">");
            }
            if (cell->format()->textFontItalic(currentcolumn,currentrow))
            {
                text.insert(0, "<" + html_italic + ">");
                text.append("</" + html_italic + ">");
            }
            if (cell->format()->textFontUnderline(currentcolumn,currentrow))
            {
                text.insert(0, "<" + html_underline + ">");
                text.append("</" + html_underline + ">");
            }
            QColor textColor = cell->format()->textColor(currentcolumn,currentrow);
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
}

void HTMLExport::createSheetSeparator( QString &str )
{
  str += ("<p align=\"" + html_center + "\"><a href=\"#__top\">" + i18n("Top") + "</a></p>\n" );
  str += "<hr width=\"80%\">\n";
}

void HTMLExport::writeTOC( const QStringList &sheets, const QString &base, QString &str )
{
  // don't create TOC for 1 sheet
  if( sheets.count() == 1 )
    return;

  str += "<p align=\"" + html_center + "\">\n";

  for( int i = 0 ; i < sheets.count() ; ++i )
  {
    str += "<a href=\"";

    if( m_dialog->separateFiles() )
    {
      str += fileName( base, sheets[i], sheets.count() > 1  );
    }
    else
    {
      str += "#" + sheets[i].toLower().trimmed();
    }

    str += "\">" + sheets[i] + "</a>\n";
    if( i != sheets.count() -1 )
      str += " - ";
  }

  str += "</p><hr width=\"80%\">\n";
}

QString HTMLExport::fileName( const QString &base, const QString &sheetName, bool multipleFiles )
{
     QString fileName = base;
     if( m_dialog->separateFiles() && multipleFiles )
     {
         fileName += "-" + sheetName;
     }
     fileName += ".html";

     return fileName;
}

void HTMLExport::detectFilledCells( Sheet *sheet, int &rows, int &columns )
{
  int iMaxColumn = sheet->maxColumn();
  int iMaxRow = sheet->maxRow();
  rows = 0;
  columns = 0;

  for ( int currentrow = 1 ; currentrow <= iMaxRow ; ++currentrow)
  {
    Cell * cell = 0L;
    int iUsedColumn=0;
    for ( int currentcolumn = 1 ; currentcolumn <= iMaxColumn ; currentcolumn++ )
    {
      cell = sheet->cellAt( currentcolumn, currentrow, false );
      QString text;
      if ( !cell->isDefault() && !cell->isEmpty() )
      {
        iUsedColumn = currentcolumn;
      }
    }
    if (cell)
      iUsedColumn += cell->extraXCells();
    if (iUsedColumn > columns)
      columns = iUsedColumn;
    if ( iUsedColumn > 0 )
      rows = currentrow;
  }
}

#include <htmlexport.moc>
