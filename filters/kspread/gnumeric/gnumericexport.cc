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

/* Gnumeric export filter by Phillip Ezolt (phillipezolt@hotmail.com) */

#include <gnumericexport.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kgenericfactory.h>
#include <koFilterChain.h>
#include <qapplication.h>
#include <qptrlist.h>
#include <qsortedlist.h>
#include <qfile.h>

#include <kspread_map.h>
#include <kspread_sheet.h>
#include <kspread_doc.h>

#include <koDocumentInfo.h>

#include <zlib.h>

typedef KGenericFactory<GNUMERICExport, KoFilter> GNUMERICExportFactory;
K_EXPORT_COMPONENT_FACTORY( libgnumericexport, GNUMERICExportFactory( "gnumericexport" ) )

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



GNUMERICExport::GNUMERICExport(KoFilter *, const char *, const QStringList&) :
                     KoFilter() {
}




QDomElement GNUMERICExport::GetCellStyle(QDomDocument gnumeric_doc,KSpreadCell * cell, int currentcolumn, int currentrow)
{
  QColorGroup defaultColorGroup = QApplication::palette().active();

		     QDomElement cell_style;
		     QDomElement font_style;

		     cell_style = gnumeric_doc.createElement("gmr:Style");

		     QString font_name = "Helvetica";

		     //		     if (defaultColorGroup.background()!=cell->bgColor(currentcolumn, currentrow))
		     if (cell->hasProperty(KSpreadCell::PBackgroundColor))
		       {
			 int red, green, blue;

			 QColor bgColor =  cell->bgColor(currentcolumn, currentrow);
			 red = bgColor.red()<<8;
			 green = bgColor.green()<<8;
			 blue = bgColor.blue()<<8;

			 cell_style.setAttribute("Shade","1");
			 cell_style.setAttribute("Back",QString::number(red,16)+":"+QString::number(green,16) +":"+QString::number(blue,16) );
		       }


		     if (defaultColorGroup.foreground()!=cell->textColor(currentcolumn, currentrow))
		       {
			 int red, green, blue;

			 QColor textColor =  cell->textColor(currentcolumn, currentrow);
			 red = textColor.red()<<8;
			 green = textColor.green()<<8;
			 blue = textColor.blue()<<8;

			 cell_style.setAttribute("Fore",QString::number(red,16)+":"+QString::number(green,16) +":"+QString::number(blue,16) );
		       }

		     if (cell->align(currentcolumn,currentrow) ==  KSpreadFormat::Left)
		       {
			 cell_style.setAttribute("HAlign","2");
		       }
		     else if (cell->align(currentcolumn,currentrow) ==  KSpreadFormat::Right)
		       {
			 cell_style.setAttribute("HAlign","4");
		       }
		     else if (cell->align(currentcolumn,currentrow) ==  KSpreadFormat::Center)
		       {
			 cell_style.setAttribute("HAlign","8");
		       }

		     if (cell->alignY(currentcolumn,currentrow) ==  KSpreadFormat::Top)
		       {
			 cell_style.setAttribute("VAlign","1");
		       }
		     else if (cell->alignY(currentcolumn,currentrow) ==  KSpreadFormat::Bottom)
		       {
			 cell_style.setAttribute("VAlign","2");
		       }
		     else if (cell->alignY(currentcolumn,currentrow) ==  KSpreadFormat::Middle)
		       {
			 cell_style.setAttribute("VAlign","4");
		       }



		     font_style = gnumeric_doc.createElement("gmr:Font");
		     font_style.appendChild(gnumeric_doc.createTextNode(font_name));
		     cell_style.appendChild(font_style);


		     if (cell->textFontItalic(currentcolumn,currentrow)){
		       font_style.setAttribute("Italic","1");
		     }

		     if (cell->textFontBold(currentcolumn,currentrow) ){
		       font_style.setAttribute("Bold","1");
		     }

		     if (cell->textFontUnderline(currentcolumn,currentrow)){
		       font_style.setAttribute("Underline","1");
		     }

		     if (cell->textFontStrike(currentcolumn,currentrow)){
		       font_style.setAttribute("StrikeThrough","1");
		     }

		     if (cell->textFontSize(currentcolumn,currentrow)){
		       font_style.setAttribute("Unit",QString::number(cell->textFontSize(currentcolumn,currentrow)));
		     }

		     QString stringFormat;

		     /*
			cell->setFaktor( 1.0);
			cell->setPrecision(2);

			cell->precision
			cell->prefix
			cell->postfix


		     */

		     switch( cell->getFormatType(currentcolumn, currentrow))
		       {
		       case KSpreadCell::Number:
			 stringFormat="0.00";
			 cell_style.setAttribute("Format",stringFormat);
			 break;
		       case KSpreadCell::Money:


			 stringFormat="$0.00" +  cell->prefix(currentcolumn,currentrow) + cell->postfix(currentcolumn,currentrow) + QString::number(cell->precision(currentcolumn,currentrow));
			 cell_style.setAttribute("Format",stringFormat);

			 break;
		       case KSpreadCell::Percentage:
			 stringFormat="0.00%";
			 cell_style.setAttribute("Format",stringFormat);
			 break;
		       case KSpreadCell::Scientific:
			 stringFormat="0.00E+00";
			 cell_style.setAttribute("Format",stringFormat);
			 break;
		       case KSpreadCell::ShortDate:
			 stringFormat="ShortDate";
			 break;
		       case KSpreadCell::TextDate:
			 stringFormat="TextDate";
			 break;
		       case KSpreadCell::date_format1:
		       case KSpreadCell::date_format2:
		       case KSpreadCell::date_format3:
		       case KSpreadCell::date_format4:
		       case KSpreadCell::date_format5:
		       case KSpreadCell::date_format6:
		       case KSpreadCell::date_format7:
		       case KSpreadCell::date_format8:
		       case KSpreadCell::date_format9:
		       case KSpreadCell::date_format10:
		       case KSpreadCell::date_format11:
		       case KSpreadCell::date_format12:
		       case KSpreadCell::date_format13:
		       case KSpreadCell::date_format14:
		       case KSpreadCell::date_format15:
		       case KSpreadCell::date_format16:
			 stringFormat="date format";
			 break;
		       case KSpreadCell::Time:
			 stringFormat="Time";
			 break;
		       case KSpreadCell::SecondeTime:
			 stringFormat="SecondeTime";
			 break;
		       case KSpreadCell::fraction_half:
			 stringFormat="fraction_half";
			 break;
		       case KSpreadCell::fraction_quarter:
			 stringFormat="fraction_quarter";
			 break;
		       case KSpreadCell::fraction_eighth:
			 stringFormat="fraction_eighth";
			 break;
		       case KSpreadCell::fraction_sixteenth:
			 stringFormat="fraction_sixteenth";
			 break;
		       case KSpreadCell::fraction_tenth:
			 stringFormat="fraction_tenth";
			 break;
		       case KSpreadCell::fraction_hundredth:
			 stringFormat="fraction_hundredth";
			 break;
		       case KSpreadCell::fraction_one_digit:
			 stringFormat="fraction_one_digit";
			 break;
		       case KSpreadCell::fraction_two_digits:
			 stringFormat="fraction_two_digits";
			 break;
		       case KSpreadCell::fraction_three_digits:
			 stringFormat="fraction_three_digits";
			 break;
		       }


		     /* <gmr:Style Format="0.0000%"/> */
		     /*  <format bgcolor="#ffffff" float="3" format="25" floatcolor="2" faktor="100"> */

		     /* <gmr:Style Format="0.00E+00"/> */
		     /* <format bgcolor="#ffffff" float="3" format="30" floatcolor="2" > */


		     /*  <format bgcolor="#ffffff" float="3" format="10" floatcolor="2" > */


		     /* <format bgcolor="#ffffff" float="1" floatcolor="1" >    */


		     /* Unit="20" */

		     /* textColor() */
		     /* bgColor() */


		     /* <gmr:Font Italic="1">Helvetica</gmr:Font> */

		     return cell_style;
}


// The reason why we use the KoDocument* approach and not the QDomDocument
// approach is because we don't want to export formulas but values !
KoFilter::ConversionStatus GNUMERICExport::convert( const QCString& from, const QCString& to )
{
    kdDebug(30501) << "Exporting GNUmeric" << endl;

    QDomDocument gnumeric_doc=QDomDocument("gmr:Workbook");

    KoDocument* document = m_chain->inputDocument();

    if ( !document )
        return KoFilter::StupidError;

    if(strcmp(document->className(), "KSpreadDoc")!=0)  // it's safer that way :)
    {
        kdWarning(30501) << "document isn't a KSpreadDoc but a " << document->className() << endl;
        return KoFilter::NotImplemented;
    }
    if(to!="application/x-gnumeric" || from!="application/x-kspread")
    {
        kdWarning(30501) << "Invalid mimetypes " << to << " " << from << endl;
        return KoFilter::NotImplemented;
    }

    const KSpreadDoc * const ksdoc=(const KSpreadDoc* const)document;

    if( ksdoc->mimeType() != "application/x-kspread" )
    {
        kdWarning(30501) << "Invalid document mimetype " << ksdoc->mimeType() << endl;
        return KoFilter::NotImplemented;
    }

    /* This could be Made into a function */

  gnumeric_doc.appendChild( gnumeric_doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );

  QDomElement workbook = gnumeric_doc.createElement("gmr:Workbook");
  workbook.setAttribute("xmlns:gmr","http://www.gnome.org/gnumeric/v5");
  gnumeric_doc.appendChild(workbook);

  /* End Made into a function */

  QDomElement sheets,sheet,tmp,cells,selections, cols,rows,styles;

  KoDocumentInfo *DocumentInfo = document->documentInfo();
  KoDocumentInfoAbout *aboutPage = static_cast<KoDocumentInfoAbout *>(DocumentInfo->page( "about" ));

  KoDocumentInfoAuthor *authorPage = static_cast<KoDocumentInfoAuthor*>(DocumentInfo->page( "author" ));

    /* Start document information. */
  QDomElement summary =  gnumeric_doc.createElement("gmr:Summary");
  workbook.appendChild(summary);

  //  QDomNode gmr_item = summary.namedItem("gmr:Item");

  QDomElement gmr_item, gmr_name,gmr_val_string;
  gmr_item = gnumeric_doc.createElement("gmr:Item");
  summary.appendChild(gmr_item);

  /* Title */
  gmr_name = gnumeric_doc.createElement("gmr:name");
  gmr_val_string = gnumeric_doc.createElement("gmr:val-string");

  gmr_name.appendChild(gnumeric_doc.createTextNode("title"));
  gmr_val_string.appendChild(gnumeric_doc.createTextNode(aboutPage->title()));

  //
  //
  //


  gmr_item.appendChild(gmr_name);
  gmr_item.appendChild(gmr_val_string);

  gmr_name = gnumeric_doc.createElement("gmr:name");
  gmr_val_string = gnumeric_doc.createElement("gmr:val-string");

  gmr_name.appendChild(gnumeric_doc.createTextNode("company"));
  gmr_val_string.appendChild(gnumeric_doc.createTextNode(authorPage->company()));

  gmr_item.appendChild(gmr_name);
  gmr_item.appendChild(gmr_val_string);


  gmr_name = gnumeric_doc.createElement("gmr:name");
  gmr_val_string = gnumeric_doc.createElement("gmr:val-string");

  gmr_name.appendChild(gnumeric_doc.createTextNode("author"));
  gmr_val_string.appendChild(gnumeric_doc.createTextNode(authorPage->fullName()
));

  gmr_item.appendChild(gmr_name);
  gmr_item.appendChild(gmr_val_string);


  gmr_name = gnumeric_doc.createElement("gmr:name");
  gmr_val_string = gnumeric_doc.createElement("gmr:val-string");

  gmr_name.appendChild(gnumeric_doc.createTextNode("comments"));
  gmr_val_string.appendChild(gnumeric_doc.createTextNode(aboutPage->abstract()));

  gmr_item.appendChild(gmr_name);
  gmr_item.appendChild(gmr_val_string);

  /* End document information. */

  sheets = gnumeric_doc.createElement("gmr:Sheets");
  workbook.appendChild(sheets);

  QString str;
  KSpreadSheet * table;

      for ( table = ksdoc->map()->firstTable(); table != 0L; table =ksdoc->map()->nextTable() )
       {

	 sheet = gnumeric_doc.createElement("gmr:Sheet");
	 sheets.appendChild(sheet);

	 tmp = gnumeric_doc.createElement("gmr:Name");
	 tmp.appendChild(gnumeric_doc.createTextNode(table->tableName()));

	 sheet.appendChild(tmp);

	 tmp = gnumeric_doc.createElement("gmr:MaxCol");
	 tmp.appendChild(gnumeric_doc.createTextNode(QString::number(table->maxColumn())));
	 sheet.appendChild(tmp);

	 tmp = gnumeric_doc.createElement("gmr:MaxRow");

	 tmp.appendChild(gnumeric_doc.createTextNode(QString::number(table->maxRow())));
	 sheet.appendChild(tmp);

	 tmp = gnumeric_doc.createElement("gmr:Zoom");
	 tmp.appendChild(gnumeric_doc.createTextNode("1.0"));
	 sheet.appendChild(tmp);

	 styles = gnumeric_doc.createElement("gmr:Styles");
	 sheet.appendChild(styles);

	 cells = gnumeric_doc.createElement("gmr:Cells");
	 sheet.appendChild(cells);

         cols = gnumeric_doc.createElement("gmr:Cols");
	 sheet.appendChild(cols);

         rows = gnumeric_doc.createElement("gmr:Rows");
	 sheet.appendChild(rows);

         /*
	 selections = gnumeric_doc.createElement("gmr:Selections");
	 sheet.appendChild(selections);
         */
	 // Ah ah ah - the document is const, but the map and table aren't. Safety: 0.
	 // Either we get hold of KSpreadSheet::m_dctCells and apply the old method below (for sorting)
	 // or, cleaner and already sorted, we use KSpreadSheet's API (slower probably, though)
	 int iMaxColumn = table->maxColumn();
	 int iMaxRow = table->maxRow();

	 // this is just a bad approximation which fails for documents with less than 50 rows, but
	 // we don't need any progress stuff there anyway :) (Werner)
	 int value=0;
	 int step=iMaxRow > 50 ? iMaxRow/50 : 1;
	 int i=1;

	 QString emptyLines;

	 /* Save selection info. */

         /* can't save selection anymore -- part of the view, not table */
         /*
	 QDomElement selection = gnumeric_doc.createElement("gmr:Selection");
	 QRect table_selection(table->selection());

	 selections.appendChild(selection);
         */
	 /*  <gmr:Selection startCol="3" startRow="2" endCol="3" endRow="2"/>*/
/*
	 selection.setAttribute("startCol", QString::number(table_selection.left()-1));
	 selection.setAttribute("startRow", QString::number(table_selection.top()-1));

	 selection.setAttribute("endCol", QString::number(table_selection.right()-1));
	 selection.setAttribute("endRow", QString::number(table_selection.bottom()-1));
*/
	 /* End selection info. */


	 /* Start COLS */
	 ColumnFormat *cl=table->firstCol();
	 while (cl)
	   {
	     QDomElement colinfo = gnumeric_doc.createElement("gmr:ColInfo");
	     cols.appendChild(colinfo);
	     colinfo.setAttribute("No", QString::number(cl->column()-1));
	     colinfo.setAttribute("Hidden", QString::number(cl->isHide()));
	     colinfo.setAttribute("Unit", QString::number((cl->width()*3)/4));

	     cl=cl->next();
	   }

	 /* End COLS */

	 //	 RowFormat *rl=table->m_cells.firstCell;
	 //   <gmr:ColInfo No="1" Unit="96.75" MarginA="2" MarginB="2" HardSize="-1" Hidden="0"/>


	 /* Start ROWS */

	 RowFormat *rl=table->firstRow();
	 while (rl)
	   {
	     QDomElement rowinfo = gnumeric_doc.createElement("gmr:RowInfo");
	     rows.appendChild(rowinfo);
	     rowinfo.setAttribute("No", QString::number(rl->row()-1));
	     rowinfo.setAttribute("Hidden", QString::number(rl->isHide()));
	     rowinfo.setAttribute("Unit", QString::number((rl->height()*3)/4));

	     rl=rl->next();
	   }

	 /* End ROWS */

	 //rl->setHeight
	 //	 colinfo.info();
	 /*
	 <gmr:ColInfo No="1" Unit="96.75" MarginA="2" MarginB="2" HardSize="-1" Hidden="0"/>
        <gmr:ColInfo No="3" Unit="113.25" MarginA="2" MarginB="2" HardSize="-1"
Hidden="0"/>
	 */

	 /* End COLS */

	 for ( int currentrow = 1 ; currentrow <= iMaxRow ; ++currentrow, ++i )
	   {
	     if(i>step) {
	       value+=2;
	       emit sigProgress(value);
	       i=0;
	     }

	     QString line;
	     for ( int currentcolumn = 1 ; currentcolumn <= iMaxColumn ; currentcolumn++ )
	       {

		 QDomElement cell_contents;
		 KSpreadCell * cell = table->cellAt( currentcolumn, currentrow, false );


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

		       /* cell->calc( TRUE ); // Incredible, cells are not calculated if the document was just opened
			  text = cell->valueString(); */

                  text = cell->text();
                  break;
		     }
		   }

		 if ( !text.isEmpty() )
		   {
		     QDomElement gnumeric_cell = gnumeric_doc.createElement("gmr:Cell");
		     QDomElement cell_style;

		     QDomElement style_region = gnumeric_doc.createElement("gmr:StyleRegion");

		     cells.appendChild(gnumeric_cell);

		     gnumeric_cell.setAttribute("Col", QString::number(currentcolumn-1));
		     gnumeric_cell.setAttribute("Row", QString::number(currentrow-1));


		     /* Right now, we create a single region for each cell.. This is inefficient, but the
		      * implementation is quicker.. Probably later we will have to consolidate styles into
		      * style regions.
		      */

		     style_region.setAttribute("startCol", QString::number(currentcolumn-1));
		     style_region.setAttribute("startRow", QString::number(currentrow-1));
		     style_region.setAttribute("endCol", QString::number(currentcolumn-1));
		     style_region.setAttribute("endRow", QString::number(currentrow-1));

		     cell_style = GetCellStyle(gnumeric_doc,cell,currentcolumn,currentrow);

		     style_region.appendChild(cell_style);

		     styles.appendChild(style_region);

		     cell_contents = gnumeric_doc.createElement("gmr:Content");
		     cell_contents.appendChild(gnumeric_doc.createTextNode(text));
		     gnumeric_cell.appendChild(cell_contents);

		   }
		 // Append a delimiter, but in a temp string -> if no other real cell in this line,
		 // then those will be dropped
	       }

	   }
       }

    str = gnumeric_doc.toString ();


    emit sigProgress(100);

    // Ok, now write to export file
    const QCString cstr(str.utf8());

    gzFile gzfile;
    gzfile = gzopen( QFile::encodeName(m_chain->outputFile()), "wb");

    if (gzfile==NULL)
      {
        kdError(30501) << "Unable to open output file!" << endl;
        return KoFilter::FileNotFound;
      }


    gzwrite(gzfile,cstr.data() ,cstr.length());

    gzclose(gzfile);

    return KoFilter::OK;
}


#include <gnumericexport.moc>
