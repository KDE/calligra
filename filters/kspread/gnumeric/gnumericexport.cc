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
/* 2004 - Some updates by Tim Beaulen (tbscope@gmail.com) */


/*
 * Update notes - 30 september 2004
 *
 * I have updated the exporter to be as close to the gnumeric V10 standard as possible.
 * Some features of gnumeric are not available from kspread and vice versa though.
 *
 * For the gnumeric format, I based it on the xml-io code from the gnumeric cvs tree.
 * And in some parts on the output of gnumeric itself.
 *
 * Tim Beaulen
 */

#include <gnumericexport.h>
#include <kdebug.h>
#include <kfilterdev.h>
#include <kmessagebox.h>
#include <kgenericfactory.h>
#include <koFilterChain.h>
#include <qapplication.h>
#include <qptrlist.h>
#include <qsortedlist.h>
#include <qfile.h>
#include <qtextstream.h>

#include <kspread_map.h>
#include <kspread_sheet.h>
#include <kspread_doc.h>
#include <kspread_view.h>
#include <kspread_canvas.h>

#include <koDocumentInfo.h>


typedef KGenericFactory<GNUMERICExport, KoFilter> GNUMERICExportFactory;
K_EXPORT_COMPONENT_FACTORY( libgnumericexport, GNUMERICExportFactory( "kofficefilters" ) )

class Cell 
{
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
KoFilter() 
{
}


/*
 * This function will check if a cell has any type of border.
 */
bool GNUMERICExport::hasBorder(KSpreadCell *cell, int currentcolumn, int currentrow)
{          
    if ( ( (cell->leftBorderWidth(currentcolumn, currentrow) != 0) &&
           (cell->leftBorderStyle(currentcolumn, currentrow) != Qt::NoPen ) ) ||
         ( (cell->rightBorderWidth(currentcolumn, currentrow) != 0) &&
           (cell->rightBorderStyle(currentcolumn, currentrow) != Qt::NoPen ) ) ||
         ( (cell->topBorderWidth(currentcolumn, currentrow) != 0) &&
           (cell->topBorderStyle(currentcolumn, currentrow) != Qt::NoPen ) ) ||           
         ( (cell->bottomBorderWidth(currentcolumn, currentrow) != 0) &&
           (cell->bottomBorderStyle(currentcolumn, currentrow) != Qt::NoPen ) ) ||
         ( (cell->fallDiagonalWidth(currentcolumn, currentrow) != 0) &&
           (cell->fallDiagonalStyle(currentcolumn, currentrow) != Qt::NoPen ) ) ||
         ( (cell->goUpDiagonalWidth(currentcolumn, currentrow) != 0) &&
           (cell->goUpDiagonalStyle(currentcolumn, currentrow) != Qt::NoPen ) ) )
        return true;
    else
        return false;
}

QDomElement GNUMERICExport::GetCellStyle(QDomDocument gnumeric_doc,KSpreadCell * cell, int currentcolumn, int currentrow)
{
    QColorGroup defaultColorGroup = QApplication::palette().active();

	QDomElement cell_style;
	QDomElement font_style;
    QDomElement border_style;    

	cell_style = gnumeric_doc.createElement("gmr:Style");

	QString font_name = "Helvetica";

    int red, green, blue;

    QColor bgColor =  cell->bgColor(currentcolumn, currentrow);
	red = bgColor.red()<<8;
	green = bgColor.green()<<8;
	blue = bgColor.blue()<<8;

    switch (cell->backGroundBrushStyle(currentcolumn, currentrow))
    {
        case Qt::NoBrush:
            cell_style.setAttribute("Shade","0");
            break; 
        case Qt::SolidPattern:
            // 100%
            cell_style.setAttribute("Shade","1");
            break;
        case Qt::Dense1Pattern:
            // 87.5%
            cell_style.setAttribute("Shade","25");
            break;
        case Qt::Dense2Pattern:
            // 75%
            cell_style.setAttribute("Shade","2");
            break;
        case Qt::Dense3Pattern:
            // 62.5%
            // Not supported by GNumeric
            // Fall back to 50%
            cell_style.setAttribute("Shade","3");
            break;
        case Qt::Dense4Pattern:
            // 50%
            cell_style.setAttribute("Shade","3");
            break;
        case Qt::Dense5Pattern:
            // 25%
            cell_style.setAttribute("Shade","4");
            break;
        case Qt::Dense6Pattern:
            // 12.5%
            cell_style.setAttribute("Shade","5");
            break;
        case Qt::Dense7Pattern:
            // 6.25%
            cell_style.setAttribute("Shade","6");
            break;
        case Qt::HorPattern:
            cell_style.setAttribute("Shade","13");
            break;
        case Qt::VerPattern:
            cell_style.setAttribute("Shade","14");
            break;
        case Qt::CrossPattern:
            cell_style.setAttribute("Shade","17");
            break;
        case Qt::BDiagPattern:
            cell_style.setAttribute("Shade","16");
            break;
        case Qt::FDiagPattern:
            cell_style.setAttribute("Shade","15");
            break;
        case Qt::DiagCrossPattern:
            cell_style.setAttribute("Shade","18");
            break;
        case Qt::CustomPattern:
            // Not supported by Gnumeric
            cell_style.setAttribute("Shade","0");
            break;
    }

	cell_style.setAttribute("Back",QString::number(red,16)+":"+QString::number(green,16) +":"+QString::number(blue,16) );


	QColor textColor =  cell->textColor(currentcolumn, currentrow);
	red = textColor.red()<<8;
	green = textColor.green()<<8;
	blue = textColor.blue()<<8;

	cell_style.setAttribute("Fore",QString::number(red,16)+":"+QString::number(green,16) +":"+QString::number(blue,16) );

    if (cell->align(currentcolumn,currentrow) ==  KSpreadFormat::Undefined)
    {
        cell_style.setAttribute("HAlign","1");
    }
	else if (cell->align(currentcolumn,currentrow) ==  KSpreadFormat::Left)
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

    if (cell->multiRow(currentcolumn,currentrow))
        cell_style.setAttribute("WrapText","1");
    else
        cell_style.setAttribute("WrapText","0");
    
    // ShrinkToFit not supported by KSpread (?)
    cell_style.setAttribute("ShrinkToFit","0");
    
    // I'm not sure about the rotation values.
    // I never got it to work in GNumeric.
    cell_style.setAttribute("Rotation", QString::number(cell->getAngle(currentcolumn,currentrow)));
    
    // The indentation in GNumeric is an integer value. In KSpread, it's a double.
    // Save the double anyway, makes it even better when importing the document back in KSpread.
    cell_style.setAttribute("Indent", QString::number(cell->getIndent(currentcolumn,currentrow)));
    
    cell_style.setAttribute("Locked", !cell->notProtected(currentcolumn,currentrow));
    
    // A KSpread cell can have two options to hide: only formula hidden, or everything hidden.
    // I only consider a cell with everything hidden as hidden.
    // Gnumeric hides everything or nothing.
    cell_style.setAttribute("Hidden", cell->isHideAll(currentcolumn,currentrow));

    QColor patColor =  cell->backGroundBrushColor(currentcolumn, currentrow);
    red = patColor.red()<<8;
    green = patColor.green()<<8;
    blue = patColor.blue()<<8;
            
    cell_style.setAttribute("PatternColor", QString::number(red,16)+":"+QString::number(green,16) +":"+QString::number(blue,16));
    
    if (cell->content() == KSpreadCell::RichText)
    {
        QDomElement link_style;
        
        link_style = gnumeric_doc.createElement("gmr:HyperLink");
        
        QString path;
        QDomDocument domLink;
        QDomElement domRoot;
        //QDomNode domNode;
                
        domLink.setContent(cell->text().section("!",1,1));
                            
        domRoot = domLink.documentElement();
        
        //domNode = domLink.firstChild();
                            
        /*while (!domNode.isNull()) 
        {
            text = domNode.toElement().text();
            domNode = domNode.nextSibling();
        }*/
        // TODO |----> get <b> and <i> settings too
        
        path = domRoot.attribute("href");
        
        if (path.section(":",0,0).lower() == "http")
            link_style.setAttribute("type","GnmHLinkURL");
        else if (path.section(":",0,0).lower() == "mailto") 
            link_style.setAttribute("type","GnmHLinkEMail");
        else if (path.section(":",0,0).lower() == "file") 
            link_style.setAttribute("type","GnmHLinkExternal");
        else if (path.left(5).lower() == "sheet")
            link_style.setAttribute("type","GnmHLinkCurWB");
        else
            link_style.setAttribute("type","");
            
        link_style.setAttribute("target",path);
        
        // KSpread doesn't support hyperlink tips.
        link_style.setAttribute("tip","");
        
        cell_style.appendChild(link_style);
    }
    
	font_style = gnumeric_doc.createElement("gmr:Font");
	font_style.appendChild(gnumeric_doc.createTextNode(cell->textFontFamily(currentcolumn, currentrow)));
	cell_style.appendChild(font_style);

	if (cell->textFontItalic(currentcolumn,currentrow))
    {
        font_style.setAttribute("Italic","1");
	}
    if (cell->textFontBold(currentcolumn,currentrow))
    {
        font_style.setAttribute("Bold","1");
	}
    if (cell->textFontUnderline(currentcolumn,currentrow))
    {
	    font_style.setAttribute("Underline","1");
	}
    if (cell->textFontStrike(currentcolumn,currentrow))
    {
        font_style.setAttribute("StrikeThrough","1");
	}
    if (cell->textFontSize(currentcolumn,currentrow))
    {
	    font_style.setAttribute("Unit",QString::number(cell->textFontSize(currentcolumn,currentrow)));
	}
    
	QString stringFormat;

    KSpreadFormat::Currency c;
    KSpreadCurrency currency;
    
	switch( cell->getFormatType(currentcolumn, currentrow))
	{
        case Number_format:
            stringFormat="0.00";			
			break;
        case Text_format:
            stringFormat="general";
            break;
		case Money_format:
            
            if (!cell->currencyInfo(c))
            {
                stringFormat = "0.00";
                break;
            }            
            
            if (currency.getCurrencyCode(c.type) == "")
                stringFormat = "0.00";
            else if (currency.getCurrencyCode(c.type) == "$")
                stringFormat = "$0.00";
            else if (currency.getCurrencyCode(c.type) == QString::fromUtf8("€"))
                stringFormat = "[$€-2]0.00";
            else if (currency.getCurrencyCode(c.type) == QString::fromUtf8("£"))
                stringFormat = "£0.00";
            else if (currency.getCurrencyCode(c.type) == QString::fromUtf8("¥"))
                stringFormat = "¥0.00";
            else
                stringFormat="[$" + currency.getCurrencyCode(c.type) + "]0.00";

            break;
		case Percentage_format:
		    stringFormat="0.00%";
			break;
		case Scientific_format:
		    stringFormat="0.00E+00";
			break;
		case ShortDate_format:
			stringFormat=cell->locale()->dateFormatShort();
			break;
		case TextDate_format:
			stringFormat=cell->locale()->dateFormat();
			break;
		case date_format1:
            stringFormat="dd-mmm-yy";
            break;
		case date_format2:
            stringFormat="dd-mmm-yyyy";
            break;
		case date_format3:
            stringFormat="dd-mmm";
            break;
		case date_format4:
            stringFormat="dd-mm";
            break;
		case date_format5:
            stringFormat="dd/mm/yy";
            break;
		case date_format6:
            stringFormat="dd/mm/yyyy";
            break;
		case date_format7:
            stringFormat="mmm-yy";
            break;
		case date_format8:
            stringFormat="mmmm-yy";
            break;
		case date_format9:
            stringFormat="mmmm-yyyy";
            break;
		case date_format10:
            stringFormat="m-yy";
            break;
		case date_format11:
            stringFormat="dd/mmm";
            break;
		case date_format12:
            stringFormat="dd/mm";
            break;
		case date_format13:
            stringFormat="dd/mmm/yyyy";
            break;
		case date_format14:
            stringFormat="yyyy/mmm/dd";
            break;
		case date_format15:
            stringFormat="yyyy-mmm-dd";
            break;
		case date_format16:
            stringFormat="yyyy-mm-dd";
            break;
        case date_format17:
            stringFormat="d mmmm yyyy";
            break;
        case date_format18:
            stringFormat="mm/dd/yyyy";
            break;
        case date_format19:
            stringFormat="mm/dd/yy";
            break;
        case date_format20:
            stringFormat="mmm/dd/yy";
            break;
        case date_format21:
            stringFormat="mmm/dd/yyyy";
            break;
        case date_format22:
            stringFormat="mmm-yyyy";
            break;
        case date_format23:
            stringFormat="yyyy";
            break;
        case date_format24:
            stringFormat="yy";
            break;
        case date_format25:
            stringFormat="yyyy/mm/dd";
            break;
        case date_format26:
			stringFormat="yyyy/mmm/dd";
            break;
		case Time_format:
		case SecondeTime_format:
			stringFormat=cell->locale()->timeFormat();
			break;
        case Time_format1:
            stringFormat = "h:mm AM/PM";
            break;
        case Time_format2:
            stringFormat = "h:mm:ss AM/PM";
            break;
        case Time_format3:
            stringFormat = "h \"h\" mm \"min\" ss \"s\"";
            break;
        case Time_format4:
            stringFormat = "h:mm";
            break;   
        case Time_format5:
            stringFormat = "h:mm:ss";
            break;
        case Time_format6:
            stringFormat = "mm:ss";
            break; 
        case Time_format7:
            stringFormat = "[h]:mm:ss";
            break;   
        case Time_format8:
            stringFormat = "[h]:mm";
            break;
		case fraction_half:
			stringFormat="# ?/2";
			break;
		case fraction_quarter:
			stringFormat="# ?/4";
			break;
		case fraction_eighth:
			stringFormat="# ?/8";
			break;
		case fraction_sixteenth:
			stringFormat="# ?/16";
			break;
		case fraction_tenth:
			stringFormat="# ?/10";
			break;
		case fraction_hundredth:
			stringFormat="# ?/100";
			break;
		case fraction_one_digit:
			stringFormat="# ?/?";
			break;
		case fraction_two_digits:
			stringFormat="# ??/??";
			break;
		case fraction_three_digits:
			stringFormat="# ???/???";
			break;
        case Custom_format:
            stringFormat = cell->getFormatString(currentcolumn,currentrow);
            break;
    }
    cell_style.setAttribute("Format",stringFormat);
             
    if(hasBorder(cell, currentcolumn, currentrow))
    {
        border_style = gnumeric_doc.createElement("gmr:StyleBorder");
             
        if ( (cell->leftBorderWidth(currentcolumn, currentrow) != 0) &&
             (cell->leftBorderStyle(currentcolumn, currentrow) != Qt::NoPen ) ) 
        {
            QDomElement border = gnumeric_doc.createElement("gmr:Left");
            border.setAttribute("Style","1");
                
            int red, green, blue;

            QColor color =  cell->leftBorderColor(currentcolumn, currentrow);
            red = color.red()<<8;
            green = color.green()<<8;
            blue = color.blue()<<8;
                    
            border.setAttribute("Color", QString::number(red,16)+":"+QString::number(green,16) +":"+QString::number(blue,16));
            border_style.appendChild(border);                
        }
        else
        {
            QDomElement border = gnumeric_doc.createElement("gmr:Left");
            border.setAttribute("Style","0");
            border_style.appendChild(border);
        }  
             
        if ( (cell->rightBorderWidth(currentcolumn, currentrow) != 0) &&
             (cell->rightBorderStyle(currentcolumn, currentrow) != Qt::NoPen ) )
        {
            QDomElement border = gnumeric_doc.createElement("gmr:Right");
            border.setAttribute("Style","1");
                
            int red, green, blue;

            QColor color =  cell->rightBorderColor(currentcolumn, currentrow);
            red = color.red()<<8;
            green = color.green()<<8;
            blue = color.blue()<<8;
                    
            border.setAttribute("Color", QString::number(red,16)+":"+QString::number(green,16) +":"+QString::number(blue,16));
            border_style.appendChild(border);                
        }
        else
        {
            QDomElement border = gnumeric_doc.createElement("gmr:Right");
            border.setAttribute("Style","0");
            border_style.appendChild(border);
        }  
                        
        if ( (cell->topBorderWidth(currentcolumn, currentrow) != 0) &&
             (cell->topBorderStyle(currentcolumn, currentrow) != Qt::NoPen ) )
        {
            QDomElement border = gnumeric_doc.createElement("gmr:Top");
            border.setAttribute("Style","1");
                
            int red, green, blue;

            QColor color =  cell->topBorderColor(currentcolumn, currentrow);
            red = color.red()<<8;
            green = color.green()<<8;
            blue = color.blue()<<8;
                    
            border.setAttribute("Color", QString::number(red,16)+":"+QString::number(green,16) +":"+QString::number(blue,16));
            border_style.appendChild(border);                
        } 
        else
        {
            QDomElement border = gnumeric_doc.createElement("gmr:Top");
            border.setAttribute("Style","0");
            border_style.appendChild(border);
        }
                
        if ( (cell->bottomBorderWidth(currentcolumn, currentrow) != 0) &&
             (cell->bottomBorderStyle(currentcolumn, currentrow) != Qt::NoPen ) )
        {
            QDomElement border = gnumeric_doc.createElement("gmr:Bottom");
            border.setAttribute("Style","1");
                
            int red, green, blue;

            QColor color =  cell->bottomBorderColor(currentcolumn, currentrow);
            red = color.red()<<8;
            green = color.green()<<8;
            blue = color.blue()<<8;
                    
            border.setAttribute("Color", QString::number(red,16)+":"+QString::number(green,16) +":"+QString::number(blue,16));
            border_style.appendChild(border);                
        } 
        else
        {
            QDomElement border = gnumeric_doc.createElement("gmr:Bottom");
            border.setAttribute("Style","0");
            border_style.appendChild(border);
        }
                
        if ( (cell->fallDiagonalWidth(currentcolumn, currentrow) != 0) &&
             (cell->fallDiagonalStyle(currentcolumn, currentrow) != Qt::NoPen ) )
        {
            QDomElement border = gnumeric_doc.createElement("gmr:Diagonal");
            border.setAttribute("Style","1");
                
            int red, green, blue;

            QColor color =  cell->fallDiagonalColor(currentcolumn, currentrow);
            red = color.red()<<8;
            green = color.green()<<8;
            blue = color.blue()<<8;
                    
            border.setAttribute("Color", QString::number(red,16)+":"+QString::number(green,16) +":"+QString::number(blue,16));
            border_style.appendChild(border);                
        } 
        else
        {
            QDomElement border = gnumeric_doc.createElement("gmr:Diagonal");
            border.setAttribute("Style","0");
            border_style.appendChild(border);
        }
                
        if ( (cell->goUpDiagonalWidth(currentcolumn, currentrow) != 0) &&
             (cell->goUpDiagonalStyle(currentcolumn, currentrow) != Qt::NoPen ) )
        {
            QDomElement border = gnumeric_doc.createElement("gmr:Rev-Diagonal");
            border.setAttribute("Style","1");
                
            int red, green, blue;

            QColor color =  cell->goUpDiagonalColor(currentcolumn, currentrow);
            red = color.red()<<8;
            green = color.green()<<8;
            blue = color.blue()<<8;
                    
            border.setAttribute("Color", QString::number(red,16)+":"+QString::number(green,16) +":"+QString::number(blue,16));
            border_style.appendChild(border);                
        } 
        else
        {
            QDomElement border = gnumeric_doc.createElement("gmr:Rev-Diagonal");
            border.setAttribute("Style","0"); 
            border_style.appendChild(border); 
        }
                
        cell_style.appendChild(border_style);
    }

	return cell_style;
}


void GNUMERICExport::addAttributeItem(QDomDocument gnumeric_doc, QDomElement attributes, const QString& type, const QString& name, bool value)
{
    QDomElement gmr_attribute, gmr_type, gmr_name, gmr_value;
  
    gmr_attribute = gnumeric_doc.createElement("gmr:Attribute");
    attributes.appendChild(gmr_attribute);
  
    gmr_type = gnumeric_doc.createElement("gmr:type");
    gmr_type.appendChild(gnumeric_doc.createTextNode(type));
    gmr_attribute.appendChild(gmr_type);
  
    gmr_name = gnumeric_doc.createElement("gmr:name");
    gmr_name.appendChild(gnumeric_doc.createTextNode(name));
    gmr_attribute.appendChild(gmr_name);
  
    QString txtValue;
    if (value)
        txtValue = "true";
    else
        txtValue = "false";
        
    gmr_value = gnumeric_doc.createElement("gmr:value");
    gmr_value.appendChild(gnumeric_doc.createTextNode(txtValue));
    gmr_attribute.appendChild(gmr_value);
}

void GNUMERICExport::addSummaryItem(QDomDocument gnumeric_doc, QDomElement summary, const QString& name, const QString& value)
{
    QDomElement gmr_item, gmr_name, gmr_val_string;
    
    gmr_item = gnumeric_doc.createElement("gmr:Item");
    summary.appendChild(gmr_item);

    gmr_name = gnumeric_doc.createElement("gmr:name");
    gmr_name.appendChild(gnumeric_doc.createTextNode(name));
    gmr_item.appendChild(gmr_name);
      
    gmr_val_string = gnumeric_doc.createElement("gmr:val-string");
    gmr_val_string.appendChild(gnumeric_doc.createTextNode(value));
    gmr_item.appendChild(gmr_val_string);
}

// The reason why we use the KoDocument* approach and not the QDomDocument
// approach is because we don't want to export formulas but values !
KoFilter::ConversionStatus GNUMERICExport::convert( const QCString& from, const QCString& to )
{
    kdDebug(30501) << "Exporting GNUmeric" << endl;

    QDomDocument gnumeric_doc=QDomDocument();

    KSpreadSheet* table;
    KoDocument* document = m_chain->inputDocument();

    if (!document)
        return KoFilter::StupidError;

    if (strcmp(document->className(), "KSpreadDoc") != 0)  // it's safer that way :)
    {
        kdWarning(30501) << "document isn't a KSpreadDoc but a " << document->className() << endl;
        return KoFilter::NotImplemented;
    }
    if (to != "application/x-gnumeric" || from != "application/x-kspread")
    {
        kdWarning(30501) << "Invalid mimetypes " << to << " " << from << endl;
        return KoFilter::NotImplemented;
    }

    const KSpreadDoc* const ksdoc = (const KSpreadDoc* const)document;

    if (ksdoc->mimeType() != "application/x-kspread")
    {
        kdWarning(30501) << "Invalid document mimetype " << ksdoc->mimeType() << endl;
        return KoFilter::NotImplemented;
    }

    /* This could be Made into a function */

    gnumeric_doc.appendChild( gnumeric_doc.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement workbook = gnumeric_doc.createElement("gmr:Workbook");
    workbook.setAttribute("xmlns:gmr","http://www.gnumeric.org/v10.dtd");
    workbook.setAttribute("xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance");
    workbook.setAttribute("xmlns:schemaLocation", "http://www.gnumeric.org/v8.xsd");
    gnumeric_doc.appendChild(workbook);

    /* End Made into a function */

    QDomElement sheets,sheet,tmp,cells,selections, cols,rows,styles,merged;

    KoDocumentInfo *DocumentInfo = document->documentInfo();
    KoDocumentInfoAbout *aboutPage = static_cast<KoDocumentInfoAbout *>(DocumentInfo->page( "about" ));

    KoDocumentInfoAuthor *authorPage = static_cast<KoDocumentInfoAuthor*>(DocumentInfo->page( "author" ));

    /*
     * Attributes
    */
    QDomElement attributes = gnumeric_doc.createElement("gmr:Attributes");
    workbook.appendChild(attributes);
  
    addAttributeItem(gnumeric_doc, attributes, "4", "WorkbookView::show_horizontal_scrollbar", ksdoc->showHorizontalScrollBar());
    addAttributeItem(gnumeric_doc, attributes, "4", "WorkbookView::show_vertical_scrollbar", ksdoc->showVerticalScrollBar());
    addAttributeItem(gnumeric_doc, attributes, "4", "WorkbookView::show_notebook_tabs", ksdoc->showTabBar());
    if (ksdoc->completionMode() == KGlobalSettings::CompletionAuto)
        addAttributeItem(gnumeric_doc, attributes, "4", "WorkbookView::do_auto_completion", "true");
    else
        addAttributeItem(gnumeric_doc, attributes, "4", "WorkbookView::do_auto_completion", "false");
    addAttributeItem(gnumeric_doc, attributes, "4", "WorkbookView::is_protected", ksdoc->map()->isProtected());
  
    /*
     * Doccument summary
    */
    QDomElement summary =  gnumeric_doc.createElement("gmr:Summary");
    workbook.appendChild(summary);

    addSummaryItem(gnumeric_doc, summary, "title", aboutPage->title());
    addSummaryItem(gnumeric_doc, summary, "company", authorPage->company());
    addSummaryItem(gnumeric_doc, summary, "author", authorPage->fullName());
    addSummaryItem(gnumeric_doc, summary, "comments", aboutPage->abstract());
    addSummaryItem(gnumeric_doc, summary, "application", "KSpread");

    /*
     * Sheet name index (necessary for the gnumeric xml_sax importer)
    */       
    QDomElement sheetNameIndex = gnumeric_doc.createElement("gmr:SheetNameIndex");
    workbook.appendChild(sheetNameIndex);
  
    for (table = ksdoc->map()->firstTable(); table != 0L; table =ksdoc->map()->nextTable())
    {
        QDomElement sheetName = gnumeric_doc.createElement("gmr:SheetName");
        sheetName.appendChild(gnumeric_doc.createTextNode(table->tableName()));
        sheetNameIndex.appendChild(sheetName);
    }
  
    /*
     * Sheets 
    */ 
    sheets = gnumeric_doc.createElement("gmr:Sheets");
    workbook.appendChild(sheets);

    QString str;
    
    KSpreadView * view = static_cast<KSpreadView*>( ksdoc->views().getFirst());
    KSpreadCanvas * canvas;
    
    if (view)
        canvas = view->canvasWidget();
    
    for (table = ksdoc->map()->firstTable(); table != 0L; table =ksdoc->map()->nextTable())
    {
        sheet = gnumeric_doc.createElement("gmr:Sheet");
	    sheets.appendChild(sheet);
     
        sheet.setAttribute("DisplayFormulas", table->getShowFormula());
        sheet.setAttribute("HideZero", table->getHideZero());
        sheet.setAttribute("HideGrid", !table->getShowGrid());
        sheet.setAttribute("HideColHeader", !ksdoc->showColumnHeader());
        sheet.setAttribute("HideRowHeader", !ksdoc->showRowHeader());     
        /* Not available in KSpread ?
         * sheet.setAttribute("DisplayOutlines", "true");
         * sheet.setAttribute("OutlineSymbolsBelow", "true");
         * sheet.setAttribute("OutlineSymbolsRight", "true");
         * sheet.setAttribute("TabColor", "");
         * sheet.setAttribute("TabTextColor", "");
        */

	    tmp = gnumeric_doc.createElement("gmr:Name");
	    tmp.appendChild(gnumeric_doc.createTextNode(table->tableName()));

	    sheet.appendChild(tmp);

	    tmp = gnumeric_doc.createElement("gmr:MaxCol");
	    tmp.appendChild(gnumeric_doc.createTextNode(QString::number(table->maxColumn())));
	    sheet.appendChild(tmp);

	    tmp = gnumeric_doc.createElement("gmr:MaxRow");

	    tmp.appendChild(gnumeric_doc.createTextNode(QString::number(table->maxRow())));
	    sheet.appendChild(tmp);

	    // Zoom value doesn't appear to be correct
        // KSpread 200% gives zoom() = 2.5, this in GNumeric = 250%
        tmp = gnumeric_doc.createElement("gmr:Zoom");
        if (view)
           tmp.appendChild(gnumeric_doc.createTextNode(QString::number(canvas->zoom())));   
        else
	       tmp.appendChild(gnumeric_doc.createTextNode("1.0"));
	    sheet.appendChild(tmp);

	    styles = gnumeric_doc.createElement("gmr:Styles");
	    sheet.appendChild(styles);

	    cells = gnumeric_doc.createElement("gmr:Cells");
	    sheet.appendChild(cells);
     
        merged = gnumeric_doc.createElement("gmr:MergedRegions");
        bool mergedCells = false; // if there are no merged cells in this sheet, don't write an
                                  // empty mergedRegions to the file.
                                  // So, depending on the value of mergedCells, 
                                  // the merged dom element is added or not.

        cols = gnumeric_doc.createElement("gmr:Cols");
	    sheet.appendChild(cols);

        rows = gnumeric_doc.createElement("gmr:Rows");
	    sheet.appendChild(rows);

        /*
	       selections = gnumeric_doc.createElement("gmr:Selections");
	       sheet.appendChild(selections);
        */
	    // Ah ah ah - the document is const, but the map and table aren't. Safety: 0.
	    // Either we get hold of KSpreadSheet::m_dctCells and apply the old method below 
        // (for sorting) or, cleaner and already sorted, we use KSpreadSheet's API 
        // (slower probably, though)
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
	        colinfo.setAttribute("Unit", QString::number(cl->width()));

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
	        rowinfo.setAttribute("Unit", QString::number(rl->height()));

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

        for (int currentrow = 1; currentrow <= iMaxRow; ++currentrow, ++i)
	    {
            if(i>step) 
            {
                value+=2;
	            emit sigProgress(value);
	            i=0;
	        }

	        QString line;
	        for (int currentcolumn = 1; currentcolumn <= iMaxColumn; currentcolumn++)
	        {
                QDomElement cell_contents;
		        KSpreadCell * cell = table->cellAt( currentcolumn, currentrow, false );

                QString text;
                QDomDocument domLink;
                QDomElement domRoot;
                QDomNode domNode;
		        
                if (!cell->isDefault() && !cell->isEmpty())
		        {
                    switch (cell->content()) 
                    {
                        case KSpreadCell::Text:
                            text = cell->text();
                            break;
                        case KSpreadCell::RichText:
                            // hyperlinks
                            // Extract the cell text
                            domLink.setContent(cell->text().section("!",1,1));
                            
                            domNode = domLink.firstChild();
                            
                            while (!domNode.isNull()) 
                            {
                                text = domNode.toElement().text();
                                domNode = domNode.nextSibling();
                            }

                            kdDebug() << "---> link, text = " << text << endl;
                            break;
		                case KSpreadCell::VisualFormula:
		                    text = cell->text(); // untested
		                    break;
		                case KSpreadCell::Formula:
		                    /* cell->calc( TRUE ); // Incredible, cells are not calculated if the document was just opened text = cell->valueString(); */
                            text = cell->text();
                            break;
                    }
                }
         
                bool tst = false;
                
                if (!text.isEmpty() )
                    tst = true;
                else
                {
                    if ( cell->backGroundBrushStyle(currentcolumn, currentrow) != Qt::NoBrush )
                        tst = true;
                    else
                    {
                        if ( hasBorder(cell, currentcolumn, currentrow) )
                            tst = true;
                        else
                            tst = false;
                    }
                }
                    
                    
		        //if (!text.isEmpty() || hasBorder(cell, currentcolumn, currentrow) || cell->backGroundBrushStyle(currentcolumn, currentrow) != Qt::NoBrush)
                if (tst)
		        {
           
                    // Check if the cell is merged
                    // Only cells with content are interesting?
                    // Otherwise it can take a while to parse a large sheet
        
                    if (cell->isForceExtraCells())
                    {
                        // The cell is forced to occupy other cells
                        QDomElement merge = gnumeric_doc.createElement("gmr:Merge");
                
                        // Set up the range
                        QString fromCol, toCol, fromRow, toRow;
                        fromCol = cell->columnName(currentcolumn);
                        fromRow = QString::number(currentrow);
                        toCol = cell->columnName(currentcolumn + cell->mergedXCells());
                        toRow = QString::number(currentrow + cell->mergedYCells());
                
                        merge.appendChild(gnumeric_doc.createTextNode(fromCol + fromRow + ":" + toCol + toRow));
                        mergedCells = true;
                        merged.appendChild(merge);
                    }
                    // ---
        
		            QDomElement gnumeric_cell = gnumeric_doc.createElement("gmr:Cell");
		            QDomElement cell_style;

		            QDomElement style_region = gnumeric_doc.createElement("gmr:StyleRegion");

		            cells.appendChild(gnumeric_cell);

		            gnumeric_cell.setAttribute("Col", QString::number(currentcolumn-1));
		            gnumeric_cell.setAttribute("Row", QString::number(currentrow-1));
             
		            /* Right now, we create a single region for each cell.. This is inefficient,
                     * but the implementation is quicker.. Probably later we will have to
                     * consolidate styles into style regions.
		             */

		            style_region.setAttribute("startCol", QString::number(currentcolumn-1));
		            style_region.setAttribute("startRow", QString::number(currentrow-1));
		            style_region.setAttribute("endCol", QString::number(currentcolumn-1));
		            style_region.setAttribute("endRow", QString::number(currentrow-1));
    
	                cell_style = GetCellStyle(gnumeric_doc,cell,currentcolumn,currentrow);

		            style_region.appendChild(cell_style);

		            styles.appendChild(style_region);

		            //cell_contents = gnumeric_doc.createElement("gmr:Content");
		            gnumeric_cell.appendChild(gnumeric_doc.createTextNode(text));
		            //gnumeric_cell.appendChild(cell_contents);
                }
                      
		        // Append a delimiter, but in a temp string -> if no other real cell in this line,
		        // then those will be dropped
            }
        }
       
        if (mergedCells)
        sheet.appendChild(merged);
    }

    str = gnumeric_doc.toString ();

    emit sigProgress(100);

    // Ok, now write to export file
    
    QIODevice* out = KFilterDev::deviceForFile(m_chain->outputFile(),"application/x-gzip");
    
    if (!out)
    {
        kdError(30501) << "No output file! Aborting!" << endl;
        return KoFilter::FileNotFound;
    }

    if (!out->open(IO_WriteOnly))
    {
        kdError(30501) << "Unable to open output file! Aborting!" << endl;
        delete out;
        return KoFilter::FileNotFound;
    }
    
    QTextStream streamOut(out);
        
    streamOut << str;
    
    out->close();
    delete out;    

    return KoFilter::OK;
}


#include <gnumericexport.moc>
