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

/* GNUmeric import filter by Phillip Ezolt 6-2-2001*/
/* phillipezolt@hotmail.com */

#include <gnumericimport.h>
#include <qmessagebox.h>
#include <kmessagebox.h>
#include <qdict.h> 

// hehe >:->
#include <kspread_doc.h>
#include <kspread_table.h>
#include <kspread_cell.h>
#include <koDocumentInfo.h>

#include <zlib.h>

GNUMERICFilter::GNUMERICFilter(KoFilter *parent, const char*name) :
                     KoFilter(parent, name) {
}

/* This converts GNUmeric's color string "0:0:0" to a QColor. */
void  convert_string_to_qcolor(QString color_string, QColor *color)
{
  int red, green, blue,first_col_pos,second_col_pos;
		      
  bool number_ok;

  first_col_pos = color_string.find(":",0);
  second_col_pos = color_string.find(":",first_col_pos+1);

  /* Fore="0:0:FF00" */
  /* If GNUmeric kicks out some invalid colors, we could crash. */
  /* GNUmeric gives us two bytes of color data per element. */
  /* We only care about the top byte. */

  red = color_string.mid( 0, first_col_pos).toInt(&number_ok,16)>>8;
  green = color_string.mid(first_col_pos+1, (second_col_pos-first_col_pos)-1).toInt(&number_ok,16)>>8;
  blue = color_string.mid(second_col_pos+1, (color_string.length()-first_col_pos)-1).toInt(&number_ok,16)>>8;
  color->setRgb(red,green,blue);
}
/* This sets the documentation information from the information stored in
   the GNUmeric file. Particularly in the "gmr:Summary" subcategory. 
*/
void set_document_info(KoDocument *document,QDomElement *docElem)
{
	/* Summary Handling START */
	QDomNode summary =  docElem->namedItem("gmr:Summary");

	QDomNode gmr_item = summary.namedItem("gmr:Item");

	while( !gmr_item.isNull() ) {
	  QDomNode gmr_name = gmr_item.namedItem("gmr:name");
	  QDomNode gmr_value = gmr_item.namedItem("gmr:val-string");

	  KoDocumentInfo *DocumentInfo = document->documentInfo();
	  KoDocumentInfoAbout *aboutPage = static_cast<KoDocumentInfoAbout *>(DocumentInfo->page( "about" ));

	  KoDocumentInfoAuthor *authorPage = static_cast<KoDocumentInfoAuthor*>(DocumentInfo->page( "author" ));


	  if (gmr_name.toElement().text()=="title")
	    {
	      aboutPage->setTitle(gmr_value.toElement().text());
	    }
	  else if (gmr_name.toElement().text()=="keywords")
	    {
	      1; /* Not supported by KSpread */
	    }
	  else if (gmr_name.toElement().text()=="comments")
	    {
	      aboutPage->setAbstract(gmr_value.toElement().text());
	    }
	  else if (gmr_name.toElement().text()=="category")
	    {
	      1;  /* Not supported by KSpread */
	    }
	  else if (gmr_name.toElement().text()=="manager")
	    {
	      1;  /* Not supported by KSpread */
	    }
	  else if (gmr_name.toElement().text()=="application")
	    {
	      1;  /* Not supported by KSpread */
	    }
	  else if (gmr_name.toElement().text()=="author")
	    {
	      authorPage->setFullName(gmr_value.toElement().text());
	    }
	  else if (gmr_name.toElement().text()=="company")
	    {
	      authorPage->setCompany(gmr_value.toElement().text());
	    }
	  gmr_item = gmr_item.nextSibling();
	}

	/* Summany Handling STOP */ 
}


void setColInfo(QDomNode *sheet, KSpreadTable *table){
	QDomNode columns =  sheet->namedItem("gmr:Cols");
	QDomNode columninfo = columns.namedItem("gmr:ColInfo");
	while( !columninfo.isNull() ) {
	  QDomElement e = columninfo.toElement(); // try to convert the node to an element.
	  int column_number;

	  column_number = e.attribute("No").toInt()+1;
	  ColumnLayout *cl = new ColumnLayout(table, column_number);
	  if (e.hasAttribute("Hidden"))
	    {
	      if (e.attribute("Hidden")=="1")
		{
		  cl->setHide(TRUE);
		  1; /* This is not supported by kspread yet.. */
		}
	    }
	  if (e.hasAttribute("Unit"))
	    {

	      //  xmm = (x_points) * (1 inch / 72 points) * (25.4 mm/ 1 inch) 
	      cl->setWidth((e.attribute("Unit").toDouble()*4)/3);
	      //cl->setWidth(e.attribute("Unit").toInt());
	    }
	  table->insertColumnLayout(cl);
	  columninfo = columninfo.nextSibling();
	}
}

void setRowInfo(QDomNode *sheet, KSpreadTable *table){
	QDomNode rows =  sheet->namedItem("gmr:Rows");
	QDomNode rowinfo = rows.namedItem("gmr:RowInfo");
	while( !rowinfo.isNull() ) {
	  QDomElement e = rowinfo.toElement(); // try to convert the node to an element.
	  int row_number;
	  row_number = e.attribute("No").toInt()+1;
	  RowLayout *rl = new RowLayout(table, row_number);

	  if (e.hasAttribute("Hidden"))
	    {
	      if (e.attribute("Hidden")=="1")
		{
		  rl->setHide(TRUE);
		  1; /* This is not supported by kspread yet.. */
		}
	    }
	  if (e.hasAttribute("Unit"))
	    {
	      //  xmm = (x_points) * (1 inch / 72 points) * (25.4 mm/ 1 inch) 
	      // 
	      // This formula is magic.. I don't understand it. 
	      rl->setHeight((e.attribute("Unit").toDouble()*4)/3);
	      //	      rl->setHeight(e.attribute("Unit").toInt());
	    }
	  table->insertRowLayout(rl);
	  rowinfo = rowinfo.nextSibling();
	}
}

void setSelectionInfo(QDomNode *sheet, KSpreadTable *table){
	QDomNode selections =  sheet->namedItem("gmr:Selections");
	QDomNode selection = selections.namedItem("gmr:Selection");

	/* Kspread does not support mutiple selections.. */
	/* This code will set the selection to the last one GNUmeric's multiple
	   selections. */
	while( !selection.isNull() ) {
	  QDomElement e = selection.toElement(); // try to convert the node to an element.
	  QRect kspread_selection;

	  kspread_selection.setLeft((e.attribute("startCol").toInt()+1));
	  kspread_selection.setTop((e.attribute("startRow").toInt()+1));
	  kspread_selection.setRight((e.attribute("endCol").toInt()+1));
	  kspread_selection.setBottom((e.attribute("endRow").toInt()+1));
	  table->setSelection(kspread_selection);

	  selection = selection.nextSibling();
	}
}


void setObjectInfo(QDomNode *sheet, KSpreadTable *table){

	QDomNode gmr_objects =  sheet->namedItem("gmr:Objects");
	QDomNode gmr_cellcomment = gmr_objects.namedItem("gmr:CellComment");
	while( !gmr_cellcomment.isNull() ) {
	  QDomElement e = gmr_cellcomment.toElement(); // try to convert the node to an element.
	  if (e.hasAttribute("Text"))
	    {
	      if (e.attribute("Text")=="1")
		{
		  1; 
		}
	    }

	  gmr_cellcomment  = gmr_cellcomment.nextSibling();
	}

}

void setStyleInfo(QDomNode *sheet, KSpreadTable *table){
  int row,column;
  QDomNode styles =  sheet->namedItem("gmr:Styles");
	if (!styles.isNull()) 
	  {
	    // Get a style region within that sheet.
	    QDomNode style_region =  styles.namedItem("gmr:StyleRegion");
	    
	    while (!style_region.isNull()){

	      QDomElement e = style_region.toElement(); // try to convert the node to an element.	  

	      QDomNode style = style_region.namedItem("gmr:Style");
	      QDomNode font = style.namedItem("gmr:Font");
	      QDomNode gmr_styleborder = style.namedItem("gmr:StyleBorder");
	      
	      for(column = (e.attribute("startCol").toInt()+1); column <=(e.attribute("endCol").toInt()+1); column++)
		{
		  for(row = (e.attribute("startRow").toInt()+1); row<= (e.attribute("endRow").toInt()+1); row++)
		    {
		      KSpreadCell * kspread_cell = table->cellAt(column, row, true );
		      
		      QDomElement style_element = style.toElement(); // try to convert the node to an element.	  


		      if (style_element.hasAttribute("Fore"))
			{
			  QString color_string=style_element.attribute("Fore");
			  QColor color;
			  convert_string_to_qcolor(color_string,&color);
			  kspread_cell->setTextColor(color);
			}
		      
		      if (style_element.hasAttribute("Back"))
			{
			  QString color_string=style_element.attribute("Back");
			  QColor color;
			  convert_string_to_qcolor(color_string,&color);
			  kspread_cell->setBgColor(color);
			}

		      if (style_element.hasAttribute("PatternColor"))
			{
			  QString color_string=style_element.attribute("Fore");
			  QColor color;
			  convert_string_to_qcolor(color_string,&color);
			  kspread_cell->setBackGroundBrushColor(color);
			}
		      else
			{
			  /* If the pattern color doesn't exists set it to the
			     background color. */
			  kspread_cell->setBackGroundBrushColor(kspread_cell->bgColor(column, row));
			}


		      if (style_element.hasAttribute("Shade"))
			{
			  QString shade=style_element.attribute("Shade");
			  if (shade=="0")
			    {
			      1;
			    }
			  else if (shade=="1")
			    {
			      /* 1 Solid */
			      kspread_cell->setBackGroundBrushStyle(Qt::SolidPattern);
			      /* What should this be? */

			    }
			  else if (shade=="2")
			    {
			      /* 2 75% */
			      kspread_cell->setBackGroundBrushStyle(Qt::Dense2Pattern);
			    }
			  else if (shade=="3")
			    {
			      /* 3 50% */
			      kspread_cell->setBackGroundBrushStyle(Qt::Dense4Pattern);
			    }
			  else if (shade=="4")
			    {
			      kspread_cell->setBackGroundBrushStyle(Qt::Dense5Pattern);
			      /* This should be 25%... All qt has is 37% */

			      /* 4 25% */
			    }
			  else if (shade=="5")
			    {
			      kspread_cell->setBackGroundBrushStyle(Qt::Dense6Pattern);
			      /* 5 12.5% */
			    }
			  else if (shade=="6")
			    {
			      kspread_cell->setBackGroundBrushStyle(Qt::Dense7Pattern);
			      /* 6 6.25% */
			      1;
			    }
			  else if (shade=="7")
			    {
			      kspread_cell->setBackGroundBrushStyle(Qt::HorPattern);
			      /* 7 Horizontal Stripe */
			      1;
			    }
			  else if (shade=="8")
			    {
			      kspread_cell->setBackGroundBrushStyle(Qt::VerPattern);
			      /* 8 Vertical Stripe */
			    }
			  else if (shade=="9")
			    {
			      kspread_cell->setBackGroundBrushStyle(Qt::BDiagPattern);
			      /* 9 Reverse Diagonal Stripe */
			      1;
			    }
			  else if (shade=="10")
			    {
			      /* 10 Diagonal Stripe */
			      kspread_cell->setBackGroundBrushStyle(Qt::FDiagPattern);
			    }

			  else if (shade=="11")
			    {
			      /* 11 Diagonal Crosshatch */
			      kspread_cell->setBackGroundBrushStyle(Qt::DiagCrossPattern);
			    }

		      /* Pattern's taken from: gnumeric's pattern.c */
		      /* 12 Thick Diagonal Crosshatch */
		      /* 13 Thin Horizontal Stripe */
		      /* 14 Thin Vertical Stripe */
		      /* 15 Thin Reverse Diagonal Stripe */
		      /* 16 Thin Diagonal Stripe */
		      /* 17 Thin Crosshatch */
		      /* 18 Thin Diagonal Crosshatch */
		      /* 19 Applix small circle */
		      /* 20 Applix semicircle */
		      /* 21 Applix small thatch */
		      /* 22 Applix round thatch */
		      /* 23 Applix Brick */
		      /* 24 100% */

			  else if (shade=="25")
			    {
			      /* 25 87.5% */
			      kspread_cell->setBackGroundBrushStyle(Qt::Dense2Pattern);
			    }
			}
		      
		      
		      if (style_element.hasAttribute("HAlign"))
			{
			QString halign_string=style_element.attribute("HAlign");

			if (halign_string=="1")
			  {
			    /* General: No equivalent in Kspread. */
			    1;
			  }
			else if (halign_string=="2")
			  {
			    kspread_cell->setAlign(KSpreadCell::Left);
			  }
			else if (halign_string=="4")
			  {
			    kspread_cell->setAlign(KSpreadCell::Right);
			  }
			else if (halign_string=="8")
			  {
			    kspread_cell->setAlign(KSpreadCell::Center);			    
			  }
			else if (halign_string=="16")
			  {
			    /* Fill: No equivalent in Kspread. */
			    1;
			  }
			else if (halign_string=="32")
			  {
			    /* Justify: No equivalent in Kspread */
			    1;
			  }

			}

		      if (style_element.hasAttribute("VAlign"))
			{
			QString valign_string=style_element.attribute("VAlign");
			
			if (valign_string=="1")
			  {
			    /* General: No equivalent in Kspread. */
			    kspread_cell->setAlignY(KSpreadCell::Top);
			  }
			else if (valign_string=="2")
			  {
			    kspread_cell->setAlignY(KSpreadCell::Bottom);
			  }
			else if (valign_string=="4")
			  {
			    kspread_cell->setAlignY(KSpreadCell::Middle);
			  }
			else if (valign_string=="8")
			  {
			    /* Justify: No equivalent in Kspread */
			    1;
			  }

			}
		      
		      if (!gmr_styleborder.isNull())
			{

			  QDomElement style_element = style.toElement(); // try to convert the node to an element.	  

			  QDomNode gmr_diagonal = gmr_styleborder.namedItem("gmr:Diagonal");
			  QDomNode gmr_rev_diagonal = gmr_styleborder.namedItem("gmr:Rev-Diagonal");
			  QDomNode gmr_top = gmr_styleborder.namedItem("gmr:Top");
			  QDomNode gmr_bottom = gmr_styleborder.namedItem("gmr:Bottom");
			  QDomNode gmr_left = gmr_styleborder.namedItem("gmr:Left");
			  QDomNode gmr_right = gmr_styleborder.namedItem("gmr:Right");
			  

			  // NoPen - no line at all. For example,
			  // QPainter::drawRect() fills but does not
			  // draw any explicit boundary
			  // line. SolidLine - a simple line. DashLine
			  // - dashes, separated by a few
			  // pixels. DotLine - dots, separated by a
			  // few pixels. DashDotLine - alternately
			  // dots and dashes. DashDotDotLine - one da

			  /*
			  if (!gmr_left.isNull())
			    {
			      qDebug("gmr_left is NOT NULL...");

			      //			      kspread_cell->setLeftBorderStyle(Qt::DashLine);
			      kspread_cell->setLeftBorderStyle(DotLine);
			      kspread_cell->setLeftBorderColor( Qt::red );
			      kspread_cell->setLeftBorderWidth( 1 );	    
			      //			      setLeftBorderWidth( o->leftBorderWidth( _column, _row ) );
			      //			      setLeftBorderStyle( o->leftBorderStyle( _column, _row ) );
			      //			      setLeftBorderColor( o->leftBorderColor( _column, _row ) );
			    }
			  */

			  /* gmr:Diagonal */
			  /* gmr:Rev-Diagonal */
			  /* gmr:Top */
			  /* gmr:Bottom */
			  /* gmr:Left */
			  /* gmr:Right */

			  
			  QDomElement gmr_styleborder_element = gmr_styleborder.toElement();
			}

		      if (!font.isNull())
			{
			  QDomElement font_element = font.toElement();
			  if (!font_element.isNull())
			    {
			      if (font_element.attribute("Italic")=="1")
				{ kspread_cell->setTextFontItalic(TRUE); }

			      if (font_element.attribute("Bold")=="1")
				{ kspread_cell->setTextFontBold(TRUE); }

			      if (font_element.attribute("Underline")=="1")
				{ kspread_cell->setTextFontUnderline(TRUE); }

			      if (font_element.attribute("StrikeThrough")=="1")
				{ kspread_cell->setTextFontStrike(TRUE); }
			      
			      if (font_element.hasAttribute("Unit"))
				{ kspread_cell->setTextFontSize(font_element.attribute("Unit").toInt()); }
			      
			    }
			}
		    }
		}
	      style_region = style_region.nextSibling();
	    }
	    
	  }
}

/* NOTE: As of now everything is in a single huge function.  This is
	 very ugly.  It should all be broken up into smaller
	 functions, probably one for each GNUMeric section.  It kind
	 of grew out of control.  It could probably be cleaned up in
	 an hour or so. --PGE
  */

bool GNUMERICFilter::filterImport(const QString &file, 
					KoDocument *document,
					const QString &from, 
					const QString &to,
					const QString &config) {
    bool bSuccess=true;

    qDebug("Entering GNUmeric Import filter.\n");

    kdDebug(30501) << "here we go... " << document->className() << endl;

    if(strcmp(document->className(), "KSpreadDoc")!=0)  // it's safer that way :)
    {
        kdWarning(30501) << "document isn't a KSpreadDoc but a " << document->className() << endl;
        return false;
    }
    if(from!="application/x-gnumeric" || to!="application/x-kspread")
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


    const char* m;
    QString UncompressedContents="";

    QDomDocument doc( "gnumeric" );

#define GZIP_SIZE 512

    char temp_data[GZIP_SIZE]; 


    gzFile gzfile;
    gzfile = gzopen( QFile::encodeName(file), "rb");
    /* Check for a failure to open... */
    
    if (gzfile==NULL)
      {
        kdWarning(30501) << "Couldn't open the requested file." << endl;
        return false;
      }

    int size;

    QCString TotalString="";

    while((size=gzread(gzfile,&temp_data, GZIP_SIZE))!=0)
      {
	QCString TempString(temp_data, size+1); 
	TotalString=TotalString+TempString;
      }
    gzclose(gzfile);
    doc.setContent(TotalString);


    int row,column;
    int value=0;

    QDomElement docElem = doc.documentElement();
    QDomNode sheets = docElem.namedItem("gmr:Sheets"); 
    QDomNode sheet =  sheets.namedItem("gmr:Sheet");

    /* This sets the Document information. */
    set_document_info(document,&docElem);

    KSpreadTable *table;
    
    // This is a mapping of exprID to expressions.

    QDict<char> exprID_dict( 17, FALSE );

    while (!sheet.isNull()) 
      {
	table=ksdoc->createTable();

	ksdoc->addTable(table);

	/* This is probably not safe... */

	table->setTableName(sheet.namedItem("gmr:Name").toElement().text(),false,false);

	setObjectInfo(&sheet, table);
	setColInfo(&sheet, table);
	setRowInfo(&sheet, table);
	setSelectionInfo(&sheet,table);

	/* CELL handling START */
	QDomNode cells =  sheet.namedItem("gmr:Cells");
	QDomNode cell = cells.namedItem("gmr:Cell");
	
	while( !cell.isNull() ) {
	  value+=2;
	  emit sigProgress(value);

	  QDomElement e = cell.toElement(); // try to convert the node to an element.
	  if( !e.isNull() ) { // the node was really an element.
	    QDomNode content_node = cell.namedItem("gmr:Content");
	    if (!content_node.isNull()){
	      QDomElement Content = content_node.toElement();
	      if( !Content.isNull() ) { // the node was really an element.
		column= e.attribute("Col").toInt()+1;
		row =   e.attribute("Row").toInt()+1;
		QString cell_content;
		
		cell_content = Content.text();
		
		table->setText(row, column, Content.text(), false); 	    
		
		if (e.hasAttribute("ExprID"))
		  {
		    //		    QString encoded_string(table->cellAt(column,row, true)->encodeFormular(row,column).utf8());
		    QString encoded_string(table->cellAt(column,row, true)->encodeFormular().latin1());


		    char *tmp_string = (char *)malloc(strlen(encoded_string.latin1()));
		    strcpy(tmp_string,encoded_string.latin1());

		    qDebug("%s",encoded_string.latin1());

		    exprID_dict.insert(e.attribute("ExprID"), tmp_string);

		    qDebug("%s",exprID_dict[e.attribute("ExprID")]);
		    qDebug("%s",exprID_dict[QString("1")]);
		    qDebug("%s",e.attribute("ExprID").latin1());

		  }
	      }
	    }
	    else
	      {
		if (e.hasAttribute("ExprID"))
		  {
		    column= e.attribute("Col").toInt()+1;
		    row =   e.attribute("Row").toInt()+1;
		    char *expr;
		    expr = exprID_dict[e.attribute("ExprID")];
		    //		    expr = exprID_dict[QString("1")];

		    qDebug("FOO:%d %d",column, row);
		    qDebug("%s",table->cellAt(column,row, true)->decodeFormular(expr,column,row).latin1());
		    qDebug("%s",expr);

		    table->setText(row, column, table->cellAt(column,row, true)->decodeFormular(expr,column,row), false); 	    
		  }
	      }
	  }
	  cell = cell.nextSibling();
	}
	


	/* There is a memory leak here... 
	 * The strings in the exprID_dict have been allocated, but they have not been freed.
	 */

	/* exprID_dict.statistics(); */

	/* CELL handling STOP */

	/* STYLE handling START */

	setStyleInfo(&sheet,table);

	/* STYLE handling STOP */

	sheet = sheet.nextSibling();
      }

    emit sigProgress(100);
    return bSuccess;
}

#include <gnumericimport.moc>
