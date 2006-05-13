/*
   This file is part of the KDE project
   Copuright (C) 2001 Michael Johnson <mikej@xnet.com>
   Copyright (C) 2001, 2002, 2003, 2004 Nicolas GOUTTE <goutte@kde.org>
   Copyright (C) 2002 Ariya Hidayat <ariya@kde.org>

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

#include <QString>
#include <QTextCodec>
#include <QFile>
#include <QFileInfo>
#include <qfontinfo.h>
#include <QFontDatabase>
#include <q3picture.h>
#include <QImage>
#include <QRegExp>
#include <QColor>
#include <QDateTime>
//Added by qt3to4:
#include <QTextStream>
#include <Q3ValueList>

#include <klocale.h>
#include <kglobal.h>
#include <kdebug.h>

#include <KWEFUtil.h>
#include <KWEFBaseWorker.h>

#include "ExportFilter.h"

// 1 twip = 1/20 pt = 1/1400 inch
// 1 inch = 25.4 mm

// some conversion macros
#define TWIP_TO_MM(x) (x)*25.4/1440.0
#define MM_TO_TWIP(x) (x)*1440.0/25.4
#define PT_TO_TWIP(x) (x)*20
#define TWIP_TO_PT(x) (x)/20

// map KWord field name to RTF field name
// e.g authorName -> AUTHOR
static QString mapFieldName( const QString& kwordField )
{
  QString rtfField;

  if( kwordField == "fileName" ) rtfField = "FILENAME";
  else if( kwordField == "authorName" ) rtfField = "AUTHOR";
  else if( kwordField == "docTitle" ) rtfField = "TITLE";

  return rtfField;
}

RTFWorker::RTFWorker():
    m_ioDevice(NULL), m_streamOut(NULL), m_eol("\r\n"), m_inTable(false),
    m_paperOrientation(false), m_paperWidth(20), m_paperHeight(20),
    m_paperMarginTop(72), m_paperMarginLeft(72),
    m_paperMarginBottom(72), m_paperMarginRight(72), m_startPageNumber(1)
{
}

static QString WritePositiveKeyword(const QString& keyword, const int value)
{
    QString str;
    str += keyword;

    if (value>0) // The value of the keyword cannot be negative
        str += QString::number( value );
    else
        str += '0';

    return str;
}

QString RTFWorker::writeRow(const QString& textCellHeader, const QString& rowText, const FrameData& frame)
{
    QString row;

    row += "\\trowd\\trgaph60\\trql";  // start new row
    row += WritePositiveKeyword("\\trrh", qRound(PT_TO_TWIP(frame.minHeight)));
    row += WritePositiveKeyword("\\trleft", qRound(PT_TO_TWIP(frame.left) - m_paperMarginLeft));
    //row += "\\trautofit0"; // ### VERIFY
    row += textCellHeader;
    row += " "; // End of keyword
    row += rowText;

    return row;
}

QString RTFWorker::writeBorder(const char whichBorder, const int borderWidth, const QColor& color)
{
    QString str;
    if (borderWidth > 0)
    {
        str += "\\clbrdr"; // Define border
        str += whichBorder; // t=top, l=left, b=bottom, r=right
        str += "\\brdrs\\brdrw"; // Single border; thickness
        str += QString::number(borderWidth);
        if (color.isValid())
        {
            str += lookupColor("\\brdrcf",color);
        }
    }
    return str;
}

QString RTFWorker::makeTable(const FrameAnchor& anchor)
{
    QString textBody; // Text to be returned
    textBody += m_prefix;
    m_prefix = QString::null;
    QString rowText;

    int rowCurrent = 0;
    bool firstCellInRow = true;
    FrameData firstFrameData;
    int debugCellCurrent = 0; //DEBUG
    int debugRowCurrent = 0; //DEBUG
    QString textCellHeader; // <celldef>

    const bool oldInTable = m_inTable;
    m_inTable = true;

    Q3ValueList<TableCell>::ConstIterator itCell;
    for (itCell=anchor.table.cellList.begin();
        itCell!=anchor.table.cellList.end(); itCell++)
    {
        // ### TODO: rowspan, colspan
        if (rowCurrent!=(*itCell).row)
        {
            rowCurrent = (*itCell).row;
            textBody += writeRow( textCellHeader, rowText, firstFrameData);
            textBody += "\\row";
            textBody += m_eol;
            rowText = QString::null;
            textCellHeader = QString::null;
            firstCellInRow=true;
            debugRowCurrent ++; // DEBUG
            debugCellCurrent = 0; //DEBUG
        }

        const FrameData& frame = (*itCell).frame;

        if (firstCellInRow) // Not yet set, so set the position of the left border.
        {
            firstFrameData=frame;
            firstCellInRow=false;
        }

        kDebug(30515) << "Cell: " << debugRowCurrent << "," << debugCellCurrent
            << " left: " << frame.left << " right: " << frame.right << " top: " << frame.top << " bottom " << frame.bottom << endl;
        textCellHeader += writeBorder('t',qRound(PT_TO_TWIP(frame.tWidth)),frame.tColor);
        textCellHeader += writeBorder('l',qRound(PT_TO_TWIP(frame.lWidth)),frame.lColor);
        textCellHeader += writeBorder('b',qRound(PT_TO_TWIP(frame.bWidth)),frame.bColor);
        textCellHeader += writeBorder('r',qRound(PT_TO_TWIP(frame.rWidth)),frame.rColor);
        textCellHeader += WritePositiveKeyword("\\cellx",qRound(PT_TO_TWIP(frame.right) - m_paperMarginRight)); //right border of cell

        QString endOfParagraph;
        Q3ValueList<ParaData> *paraList = (*itCell).paraList;
        Q3ValueList<ParaData>::ConstIterator it;
		Q3ValueList<ParaData>::ConstIterator end(paraList->end());
        for (it=paraList->begin();it!=end;++it)
        {
            rowText += endOfParagraph;
            rowText += ProcessParagraphData( (*it).text,(*it).layout,(*it).formattingList);
            rowText += m_eol;
            endOfParagraph = "\\par"; // The problem is that the last paragraph ends with \cell not with \par
        }
        rowText += "\\cell";
        debugCellCurrent ++; // DEBUG
    }

    textBody += writeRow( textCellHeader, rowText, firstFrameData);
    textBody += "\\row\\pard";  // delimit last row
    textBody += m_eol;
    m_inTable = oldInTable;

    return textBody;
}

QString RTFWorker::makeImage(const FrameAnchor& anchor)
{
    QString textBody; // Text to be returned

    QString strImageName(anchor.picture.koStoreName);
    QString strExt;
    QByteArray image;

    kDebug(30515) << "RTFWorker::makeImage" << endl << anchor.picture.koStoreName << endl;

    const int pos=strImageName.findRev('.');
    if(pos!=-1) strExt = strImageName.mid(pos+1).lower();

    QString strTag;
    if (strExt=="png")
        strTag="\\pngblip";
#if 0
    else if (strExt=="bmp")
        strTag="\\dibitmap";
#endif
    else if ( (strExt=="jpeg") || (strExt=="jpg") )
        strTag="\\jpegblip";
    else if (strExt=="wmf")
        strTag="\\wmetafile8"; // 8 == anisotropic
    else
    {
        // either without extension or format is unknown
        // let's try to convert it to PNG format
        kDebug(30515) << "Converting image " << anchor.picture.koStoreName << endl;

        strTag="\\pngblip";
        if( !loadAndConvertToImage(anchor.picture.koStoreName,strExt,"PNG",image) )
        {
            kWarning(30515) << "Unable to convert " << anchor.picture.koStoreName << endl;
            return QString::null;
        }
    }
    // ### TODO: SVG, QPicture

    // load the image, this isn't necessary for converted image
    if( !image.size() )
        if (!loadSubFile(anchor.picture.koStoreName,image))
        {
            kWarning(30515) << "Unable to load picture " << anchor.picture.koStoreName << endl;
            return QString::null;
        }


    // find displayed width and height (in twips)
    const long width  = (long)(PT_TO_TWIP(anchor.frame.right  - anchor.frame.left));
    const long height = (long)(PT_TO_TWIP(anchor.frame.bottom - anchor.frame.top));

    // find original image width and height (in twips)
    long origWidth  = width;
    long origHeight = height;
    if( strExt == "wmf" )
    {
        // special treatment for WMF with metaheader
        // d7cdc69a is metaheader magic id
        quint8* data = (quint8*) image.data();
        if( ( data[0] == 0xd7 ) && ( data[1] == 0xcd ) &&
            ( data[2] == 0xc6 ) && ( data[3] == 0x9a ) &&
            ( image.size() > 22 ) )
        {
            // grab bounding box, find original size
            unsigned left = data[6]+(data[7]<<8);
            unsigned top = data[8]+(data[9]<<8);
            unsigned right = data[10]+(data[11]<<8);
            unsigned bottom = data[12]+(data[13]<<8);
            origWidth = (long) (MM_TO_TWIP(right-left)/100);
            origHeight = (long) (MM_TO_TWIP(bottom-top)/100);
#warning "kde4 : port it"
#if 0			
            // throw away WMF metaheader (22 bytes)
            for( uint i=0; i<image.size()-22; i++)
                image.at(i)=image.at(i+22)); // As we use uint, we need at()  ( [] uses int only .)
            image.resize( image.size()-22 );
#endif			
        }
    }
    else
    {
        // It must be an image
        QImage img( image );
        if( img.isNull() )
        {
            kWarning(30515) << "Unable to load picture as image " << anchor.picture.koStoreName << endl;
            return QString::null;
        }
        // check resolution, assume 2835 dpm (72 dpi) if not available
        int resx = img.dotsPerMeterX();
        int resy = img.dotsPerMeterY();
        if( resx <= 0 ) resx = 2835;
        if( resy <= 0 ) resy = 2835;

        origWidth =  long(img.width() * 2834.65 * 20 / resx);
        origHeight = long(img.height() * 2834.65 * 20 / resy);
    }

    // Now that we are sure to have a valid image, we can write the RTF tags
    textBody += "{\\pict";
    textBody += strTag;

    // calculate scaling factor (in percentage)
    int scaleX = width * 100 / origWidth;
    int scaleY = height * 100 / origHeight;

    // size in 1/100 mm
    int picw = (int)(100 * TWIP_TO_MM(origWidth));
    int pich = (int)(100 * TWIP_TO_MM(origHeight));

    textBody += "\\picscalex";
    textBody += QString::number(scaleX, 10);
    textBody += "\\picscaley";
    textBody += QString::number(scaleY, 10);
    textBody += "\\picw";
    textBody += QString::number(picw,10);
    textBody += "\\pich";
    textBody += QString::number(pich,10);
    textBody += "\\picwgoal";
    textBody += QString::number(origWidth, 10);
    textBody += "\\pichgoal";
    textBody += QString::number(origHeight, 10);

    textBody+=" ";
    const char hex[] = "0123456789abcdef";
    for (uint i=0; i<image.size(); i++)
    {
        if (!(i%40))
            textBody += m_eol;
        const char ch=image.at(i);
        textBody += hex[(ch>>4)&0x0f]; // Done this way to avoid signed/unsigned problems
        textBody += hex[(ch&0x0f)];
    }


    textBody+="}";

    return textBody;
}

QString RTFWorker::formatTextParagraph(const QString& strText,
    const FormatData& formatOrigin, const FormatData& format)
{
    QString str;

    if (!format.text.missing)
    {
        // Opening elements
        str+=openSpan(formatOrigin,format);
    }

    QString strEscaped = escapeRtfText(strText);

    // Replace line feeds by forced line breaks
    int pos;
    QString strBr("\\line ");
    while ((pos=strEscaped.find(QChar(10)))>-1)
    {
        strEscaped.replace(pos,1,strBr);
    }

    str+=strEscaped;

    if (!format.text.missing)
    {
        // Closing elements
        str+=closeSpan(formatOrigin,format);
    }

    return str;
}

QString RTFWorker::ProcessParagraphData ( const QString &paraText,
    const LayoutData& layout, const ValueListFormatData &paraFormatDataList)
{
    QString str;
    QString content;
    QString markup;
    // open paragraph
    markup += "\\pard";
    markup += "\\plain";
    if (m_inTable)
        markup += "\\intbl";

//lists
if (layout.counter.style)
        {
        markup += "{\\pntext\\pard\\plain";
        if( layout.formatData.text.fontSize >= 0)
        {
                markup += "\\fs";
                markup += QString::number((2 * layout.formatData.text.fontSize));
                markup += lookupFont("\\f",layout.formatData.text.fontName);
        }
        markup += " ";
        markup += layout.counter.text;
        markup += "\\tab}{\\*\\pn";
        if (layout.counter.style > 5)
        {
        markup += "\\pnlvlblt ";
        markup += "{\\pntxtb ";
        if (!layout.counter.lefttext.isEmpty() && layout.counter.lefttext != "{" && layout.counter.lefttext != "}")
        {
                markup += layout.counter.lefttext;
        }
        switch (layout.counter.style)
        {
        case 6:
        {
                //custom bullets (one char)
                //TODO: use correct character/sign for bullet

            markup += layout.counter.customCharacter;
            break;
        }
        case 7:
        {
                //custom bullets (complex)
            markup += layout.counter.text;
            break;
        }
        case 8:
        {
                //circle bullets
                //TODO: use correct character/sign for bullet
            markup += "\\bullet";
            break;
        }
        case 9:
        {
                //square bullets
                //TODO: use correct character/sign for bullet
            markup += "\\bullet";
            break;
        }
        case 10:
        {
                //disc bullets
                //TODO: make work in OO
            markup += "\\bullet";
            break;
        }
        case 11:
        {
                //disc bullets
                //TODO: use correct character/sign for bullet
            markup += layout.counter.text;
            break;
        }
        default:
            markup += "\\bullet";
        }
        markup += "}";
        }
        else
        {
            if (layout.counter.numbering!=0)
		{
        	    markup += "\\pnlvl";
        	    markup += QString::number(layout.counter.depth + 1);
        	    markup += "\\pnprev1";
		}
        else if (layout.counter.style==1)
        {
        markup += "\\pnlvlbody";
        }
        else
        {
        markup += "\\pnlvl";
        markup += QString::number(11 - layout.counter.style);
        }

        switch (layout.counter.style)
        {
        case 1:
        {
        markup += "\\pndec";
        break;
        }
        case 2:
        {
        markup += "\\pnlcltr";
        break;
        }
        case 3:
        {
        markup += "\\pnucltr";
        break;
        }
        case 4:
        {
        markup += "\\pnlcrm";
        break;
        }
        case 5:
        {
        markup += "\\pnucrm";
        break;
        }
        default:
        markup += "\\pndec";
        }
        markup += "{\\pntxtb ";
        markup += layout.counter.lefttext;
        markup += " }";
    }
        markup += "{\\pntxta ";
        markup += layout.counter.righttext;
        markup += " }";

        // ### FIXME: that is too late! And why at every list paragraph? (See bug #88241)
        if (layout.counter.start!=0)
        {
        markup += "\\pnstart";
        markup += QString::number(layout.counter.start);
        }
        else
        {
        markup += "\\pnstart1";
        }
        markup += "\\pnindent0\\pnhang";

        if( layout.formatData.text.fontSize > 0 )
        {
        markup += "\\pnfs";
        markup += QString::number((2 * layout.formatData.text.fontSize));
        }

        if( !layout.formatData.text.fontName.isEmpty() )
        {
            markup += lookupFont("\\pnf", layout.formatData.text.fontName);
        }

        markup += "}";
}


    LayoutData styleLayout;
    markup += lookupStyle(layout.styleName, styleLayout);
    markup += layoutToRtf(styleLayout,layout,true);

    if ( 1==layout.formatData.text.verticalAlignment )
    {
        markup += "\\sub"; //Subscript
    }
    else if ( 2==layout.formatData.text.verticalAlignment )
    {
        markup += "\\super"; //Superscript
    }


    if (layout.pageBreakBefore)
        content += "\\page ";





    if (!paraText.isEmpty())
    {

        ValueListFormatData::ConstIterator  paraFormatDataIt;

        QString partialText;

        FormatData formatRef = layout.formatData;

        for ( paraFormatDataIt = paraFormatDataList.begin ();
              paraFormatDataIt != paraFormatDataList.end ();
              paraFormatDataIt++ )
        {
            if (1==(*paraFormatDataIt).id)
            {
                //Retrieve text
                partialText=paraText.mid ( (*paraFormatDataIt).pos, (*paraFormatDataIt).len );
                content +=formatTextParagraph(partialText, formatRef, *paraFormatDataIt);
	    }
            else if (4==(*paraFormatDataIt).id)
            {
                // ### TODO: put date/time fields into own method.
                if ( (0==(*paraFormatDataIt).variable.m_type) // date
                    || (2==(*paraFormatDataIt).variable.m_type) ) // time
                {
                    // ### TODO: fixed/variable date
                    content += "{\\field{\\*\\fldinst ";
                    if (0==(*paraFormatDataIt).variable.m_type)
                        content += "DATE ";
                    else
                        content += "TIME ";
                    QString key((*paraFormatDataIt).variable.m_key.mid(4));
                    kDebug(30515) << "Time format: " << key << endl;
                    if (key.startsWith("locale"))
                    {
                        if (key == "locale" )
                        {
                            if (0==(*paraFormatDataIt).variable.m_type)
                                key = KGlobal::locale()->dateFormat();
                            else
                                key = KGlobal::locale()->timeFormat();
                        }
                        else if ( key == "localeshort" )
                            key = KGlobal::locale()->dateFormatShort();
                        else if ( key == "localedatetime" )
                        {
                            key = KGlobal::locale()->dateFormat();
                            key += ' ';
                            key += KGlobal::locale()->timeFormat();
                        }
                        else if ( key == "localedatetimeshort" )
                        {
                            key = KGlobal::locale()->dateFormat();
                            key += ' ';
                            key += KGlobal::locale()->timeFormat();
                        }

                        kDebug(30515) << "Locale date in KLocale format:  " << key << endl;

                        // KLocale's key differ from KWord

                        // Date
                        key.replace( "%Y", "yyyy" );    // Year 4 digits
                        key.replace( "%y", "yy" );      // Year 2 digits
                        key.replace( "%n", "M" );       // Month 1 digit
                        key.replace( "%m", "MM" );      // Month 2 digits
                        key.replace( "%e", "d" );       // Day 1 digit
                        key.replace( "%d", "dd" );      // Day 2 digits
                        key.replace( "%b", "MMM" );     // Month 3 letters
                        key.replace( "%B", "MMMM" );    // Month all letters
                        key.replace( "%a", "ddd" );     // Day 3 letters
                        key.replace( "%A", "dddd" );    // Day all letters
                        // 12h
                        key.replace( "%p", "am/pm" );   // AM/PM (KLocale knows it only lower case)
                        key.replace( "%I", "hh" );      // 12 hour 2 digits
                        key.replace( "%l", "h" );       // 12 hour 1 digits
                        // 24h
                        key.replace( "%H", "HH" );      // 24 hour 2 digits
                        key.replace( "%k", "H" );       // 24 hour 1 digit
                        // Other times
                        key.replace( "%M", "mm" );      // minute 2 digits (KLocale knows it with 2 digits)
                        key.replace( "%S", "ss" );      // second 2 digits (KLocale knows it with 2 digits)

                        kDebug(30515) << "Locale date in RTF format:  " << key << endl;
                    }
                    else if (!key.isEmpty())
                    {
                        const QRegExp regexp("AP",false); // Not case-sensitive
                        if (regexp.search(key)!=-1)
                        {
                            // 12h
                            key.replace("ap","am/pm");
                            key.replace("AP","AM/PM");
                        }
                        else
                        {
                            //24h
                            key.replace('h','H'); // MS Word uses H for 24hour times
                        }
                        // MS Word do not know possesive months
                        key.replace("PPP","MMM");
                        key.replace("PPPP","MMMM");
                        key.replace("zzz","000"); // replace microseconds by 000
                        kDebug(30515) << "New format:  " << key << endl;
                        content += "\\@ \"";
                        content += key;
                        content += "\" ";
                    }
                    content += "\\* MERGEFORMAT }{\\fldrslt ";
                    content += escapeRtfText((*paraFormatDataIt).variable.m_text);
                    content += "}}";
                }
                else if (4==(*paraFormatDataIt).variable.m_type)
                {
                    QString strFieldType;
                    if ((*paraFormatDataIt).variable.isPageNumber())
                    {
                        content += "{\\field{\\*\\fldinst PAGE \\* MERGEFORMAT }{\\fldrslt ";
                        content += escapeRtfText((*paraFormatDataIt).variable.m_text);
                        content += "}}";
                    }
                    else if ((*paraFormatDataIt).variable.isPageCount())
                    {
                        content += "{\\field{\\*\\fldinst NUMPAGES \\* MERGEFORMAT }{\\fldrslt ";
                        content += escapeRtfText((*paraFormatDataIt).variable.m_text);
                        content += "}}";
                    }
                    else
                    {
                        // Unknown subtype, therefore write out the result
                        content += escapeRtfText((*paraFormatDataIt).variable.m_text);
                    }
                }
                else if (8==(*paraFormatDataIt).variable.m_type)
                {
                    // Field
                    QString name = escapeRtfText((*paraFormatDataIt).variable.getFieldName());
                    QString value = escapeRtfText((*paraFormatDataIt).variable.getFieldValue());
                    QString rtfField = mapFieldName(name);

                    if( rtfField.isEmpty() )
                        // Couldn't map field name, just write out the value
                        content += escapeRtfText((*paraFormatDataIt).variable.m_text);
                    else
                    {
                        content += "{\\field";
                        content += "{\\*\\fldinst ";
                        content +=  rtfField;
                        content += "  \\* MERGEFORMAT }";
                        content += "{\\fldrslt {";
                        content += value;
                        content += "}}}";
                    }
                }
                else if (9==(*paraFormatDataIt).variable.m_type)
                {
                    // A link
                    content += "{\\field";
                    content += "{\\*\\fldinst HYPERLINK ";
                    content +=  escapeRtfText((*paraFormatDataIt).variable.getHrefName());
                    content += "}";
                    content += "{\\fldrslt ";
                    content += "{\\ul";   // underline, ### TODO: use style Hyperlink
                    content += lookupColor("\\cf",QColor(0,0,255));// blue
                    content += escapeRtfText((*paraFormatDataIt).variable.getLinkName());
                    content += "}}}";
                }
                else if (11==(*paraFormatDataIt).variable.m_type)
                {
                    // Footnote
                    QString value = (*paraFormatDataIt).variable.getFootnoteValue();
                    bool automatic = (*paraFormatDataIt).variable.getFootnoteAuto();
                    Q3ValueList<ParaData> *paraList = (*paraFormatDataIt).variable.getFootnotePara();
                    if( paraList )
                    {
                        QString fstr;
                        Q3ValueList<ParaData>::ConstIterator it;
						Q3ValueList<ParaData>::ConstIterator end(paraList->end());
                        for (it=paraList->begin();it!=end;++it)
                            fstr += ProcessParagraphData( (*it).text, (*it).layout,(*it).formattingList);
                        content += "{\\super ";
                        content += automatic ? "\\chftn " : value;
                        content += "{\\footnote ";
                        content += "{\\super ";
                        content += automatic ? "\\chftn " : value;
                        content += fstr;
                        content += " }";
                        content += " }";
                        content += " }";
                    }
                }
                else
                {
                    // Generic variable
                    content += escapeRtfText((*paraFormatDataIt).variable.m_text);
                }
            }
            else if (6==(*paraFormatDataIt).id)
            {
                kDebug(30515) << "Found an anchor of type: " << (*paraFormatDataIt).frameAnchor.type << endl;
                // We have an image, a clipart or a table

		if (6==(*paraFormatDataIt).frameAnchor.type)
                {


		    if (!content.isEmpty())
		    {
			str += m_prefix;
			str += markup;
			str += " {";
			str += content;
			str += "}";
			str += m_eol;
			content = QString::null;
			if (!m_inTable)
			{
			    m_prefix = "\\par";
			}
		    }
		    str += makeTable((*paraFormatDataIt).frameAnchor);
		}
                else if ((2==(*paraFormatDataIt).frameAnchor.type) || (5==(*paraFormatDataIt).frameAnchor.type))
                {
                    content += makeImage((*paraFormatDataIt).frameAnchor);

                }
	    }
        }
    }

    if (layout.pageBreakAfter)
        content += "\\page";

    if (!content.isEmpty())
    {
        str += m_prefix;
	str += markup;
	str += " {";
	str += content;
	str += "}";
	str += m_eol;
	if (m_inTable==false)
	{
	   m_prefix = "\\par";
        }
    }
    if (str.isEmpty())
    {
     str ="\\par\\pard\\plain";
    if (m_inTable==false)
    {
       m_prefix = "\\par";
    }
    }
    return str;
}

bool RTFWorker::doFullParagraph(const QString& paraText,
    const LayoutData& layout, const ValueListFormatData& paraFormatDataList)
{
    kDebug(30515) << "Entering RTFWorker::doFullParagraph" << endl << paraText << endl;
    QString par = ProcessParagraphData( paraText, layout, paraFormatDataList);
    m_textBody += par;
    kDebug(30515) << "Quiting RTFWorker::doFullParagraph" << endl;
    return true;
}

bool RTFWorker::doHeader(const HeaderData& header)
{
    QString str;
    QString content;
    if( header.page == HeaderData::PAGE_ODD )
        str = "\\facingp{\\headerr";
    else if( header.page == HeaderData::PAGE_EVEN )
        str = "\\facingp{\\headerl";
    else if( header.page == HeaderData::PAGE_FIRST )
        str = "\\facingp{\\headerl";
    else if( header.page == HeaderData::PAGE_ALL )
        str = "{\\header";
    else
        return false;

    str += " {";

    Q3ValueList<ParaData>::ConstIterator it;
	Q3ValueList<ParaData>::ConstIterator end(header.para.end());
    for (it=header.para.begin();it!=end;++it)
        content += ProcessParagraphData( (*it).text,(*it).layout,(*it).formattingList);

    if (content!="\\par\\pard\\plain")
    {
    str += content;
    str += "}";

    str += "}";

    m_textBody += str;
    }
    m_prefix=QString::null;
    return true;
}

bool RTFWorker::doFooter(const FooterData& footer)
{
    QString str;
    QString content;
    if( footer.page == FooterData::PAGE_ODD )
        str = "\\facingp{\\footerr";
    else if( footer.page == FooterData::PAGE_EVEN )
        str = "\\facingp{\\footerl";
    else if( footer.page == FooterData::PAGE_FIRST )
        str = "\\facingp{\\headerl";
    else if( footer.page == FooterData::PAGE_ALL )
        str = "{\\footer";
    else
        return false;

    str += " {";

    Q3ValueList<ParaData>::ConstIterator it;
	Q3ValueList<ParaData>::ConstIterator end(footer.para.end());
    for (it=footer.para.begin();it!=end;++it)
        content += ProcessParagraphData( (*it).text,(*it).layout,(*it).formattingList);

    if (content!="\\par\\pard\\plain")
    {
    str += content;
    str += "}";

    str += "}";

    m_textBody += str;
    }
    m_prefix=QString::null;
    return true;
}

bool RTFWorker::doOpenFile(const QString& filenameOut, const QString& /*to*/)
{
    m_ioDevice=new QFile(filenameOut);

    if (!m_ioDevice)
    {
        kError(30515) << "No output file! Aborting!" << endl;
        return false;
    }

    if ( !m_ioDevice->open (QIODevice::WriteOnly) )
    {
        kError(30515) << "Unable to open output file!" << endl;
        return false;
    }

    m_streamOut=new QTextStream(m_ioDevice);

    // ### TODO: should "CP 1252" be used directly? (But RTFWorker::escapeRtfText is beased on ISO-8859-1 only.)
    m_streamOut->setEncoding(QTextStream::Latin1); // We are declaring the RTF document as CP 1252, so use ISO-8859-1

    m_fileName=filenameOut;

    return true;
}

bool RTFWorker::doCloseFile(void)
{
    kDebug(30515) << __FILE__ << ":" << __LINE__ << endl;
    delete m_streamOut;
    m_streamOut=NULL;
    if (m_ioDevice)
        m_ioDevice->close();
    return true;
}

bool RTFWorker::doOpenDocument(void)
{
    // Make the file header

    // Note: we use \\ansicpg1252 because 1200 is not supposed to be supported
    // Note2: we assume using \\ansicpg1252 in RTFWorker::escapeRtfText and in RTFWorker::doOpenFile
    *m_streamOut << "{\\rtf1\\ansi\\ansicpg1252\\uc1\\deff0" << m_eol;

    // Default color table
    m_colorList
        << QColor(0,0,0)     << QColor(0,0,255)     << QColor(0,255,255)
        << QColor(0,255,0)   << QColor(255,0,255)   << QColor(255,0,0)
        << QColor(255,255,0) << QColor(255,255,255) << QColor(0,0,128)
        << QColor(0,128,128) << QColor(0,128,0)     << QColor(128,0,128)
        << QColor(128,0,0)   << QColor(128,128,0)   << QColor(128,128,128);

    return true;
}

void RTFWorker::writeFontData(void)
{
    kDebug(30515) << "Fonts:" << m_fontList << endl;
    *m_streamOut << "{\\fonttbl";
    uint count;
    QFontDatabase fontDatabase;
    QStringList::ConstIterator it;
    for (count=0, it=m_fontList.begin();
        it!=m_fontList.end();
        count++, it++)
    {
        const QString strLower( (*it).lower() );
        *m_streamOut << "{\\f" << count;
        if ( (strLower.find("symbol")>-1) || (strLower.find("dingbat")>-1) )
            *m_streamOut << "\\ftech";
        else if ( (strLower.find("script")>-1) )
            *m_streamOut << "\\fscript";

#if 1
        else
        {
            // We do not know the font type that we have
            *m_streamOut << "\\fnil";
        }
#else
        else
        // QFontInfo:styleHint() does not guess anything, it just returns what is in the QFont. Nothing put in, nothing gets out.
        {
            QFontInfo info(*it);
            switch (info.styleHint())
            {
            case QFont::SansSerif:
            default:
                {
                    *m_streamOut << "\\fswiss";
                    break;
                }
            case QFont::Serif:
                {
                    *m_streamOut << "\\froman";
                    break;
                }
            case QFont::Courier:
                {
                    *m_streamOut << "\\fmodern";
                    break;
                }
            case QFont::OldEnglish:
                {
                    *m_streamOut << "\\fdecor";
                    break;
                }
            }
        }
#endif
        // ### TODO: \fcharset would be mandatory but Qt3 does not give us the font charset. :-(
        *m_streamOut << "\\fprq" << ( fontDatabase.isFixedPitch( *it ) ? 1 : 2 ) << " "; // font definition
        *m_streamOut << escapeRtfText( *it );
        *m_streamOut <<  ";}" << m_eol; // end font table entry
    }
    *m_streamOut << "}"; // end of font table
}

void RTFWorker::writeColorData(void)
{
    *m_streamOut << "{\\colortbl;";
    uint count;
    Q3ValueList<QColor>::ConstIterator it;
    for (count=0, it=m_colorList.begin();
        it!=m_colorList.end();
        count++, it++)
    {
        *m_streamOut << "\\red" << (*it).red();
        *m_streamOut << "\\green" << (*it).green();
        *m_streamOut << "\\blue" << (*it).blue();
        *m_streamOut <<  ";"; // end of entry
    }
    *m_streamOut << "}"; // end of color table
}

void RTFWorker::writeStyleData(void)
{
    *m_streamOut << "{\\stylesheet" << m_eol;

    uint count;
    Q3ValueList<LayoutData>::ConstIterator it;
    for (count=0, it=m_styleList.begin();
        it!=m_styleList.end();
        count++, it++)
    {
        *m_streamOut << "{";
        if (count>0) // \s0 is not written out
            *m_streamOut << "\\s" << count;

        *m_streamOut << layoutToRtf((*it),(*it),true);

        // \snext must be the last keyword before the style name
        // Find the number of the following style
        uint counter=0;  // counts position in style table starting at 0
        Q3ValueList < LayoutData > ::ConstIterator it2;
        for( it2 =  m_styleList.begin(); it2 != m_styleList.end(); counter++, ++it2 )
        {
            if ( (*it2).styleName == (*it).styleFollowing )
            {
                *m_streamOut << "\\snext" << counter;
                break;
            }
        }

        *m_streamOut << " " << (*it).styleName << ";";
        *m_streamOut << "}";
        *m_streamOut << m_eol;
    }

    *m_streamOut << "}";
}

bool RTFWorker::doCloseDocument(void)
{

    writeFontData();
    writeColorData();
    writeStyleData();

    if (!m_textDocInfo.isEmpty())
    {
        *m_streamOut << "{\\info ";  // string of document information markup
        *m_streamOut << m_textDocInfo;   // add document author, title, operator
        *m_streamOut << "}";
    }
    *m_streamOut << "\\paperw" << int(m_paperWidth);
    *m_streamOut << "\\paperh" << int(m_paperHeight);
    if (1==m_paperOrientation)
        *m_streamOut << "\\landscape";
    *m_streamOut << "\\margl" << int(m_paperMarginLeft);
    *m_streamOut << "\\margr" << int(m_paperMarginRight);
    *m_streamOut << "\\margt" << int(m_paperMarginTop);
    *m_streamOut << "\\margb" << int(m_paperMarginBottom);
    *m_streamOut << m_textPage;  // add page size, margins, etc.
    *m_streamOut << "\\widowctrl\\ftnbj\\aenddoc\\formshade \\fet0\\sectd\n";
    if (m_startPageNumber >= 1)
        *m_streamOut << "\\pgnstart" << m_startPageNumber << endl;
    //*m_streamOut << "\\linex0\\endnhere\\plain";
    *m_streamOut << "\\pard\\plain";
    *m_streamOut << m_textBody;

    *m_streamOut << "}" << m_eol;
    return true;
}

bool RTFWorker::doFullDocumentInfo(const KWEFDocumentInfo& docInfo)
{

    if ( !docInfo.title.isEmpty() )
    {
        m_textDocInfo += "{\\title ";
        m_textDocInfo += escapeRtfText( docInfo.title );
        m_textDocInfo += "}";
    }

    if ( !docInfo.fullName.isEmpty() )
    {
        m_textDocInfo += "{\\author ";
        m_textDocInfo += escapeRtfText( docInfo.fullName );
        m_textDocInfo += "}";
    }

    if ( !docInfo.keywords.isEmpty() )
    {
        m_textDocInfo += "{\\keywords ";
        m_textDocInfo += escapeRtfText( docInfo.keywords );
        m_textDocInfo += "}";
    }
    if ( !docInfo.subject.isEmpty() )
    {
        m_textDocInfo += "{\\subject ";
        m_textDocInfo += escapeRtfText( docInfo.subject );
        m_textDocInfo += "}";
    }

    if ( !docInfo.company.isEmpty() )
    {
        m_textDocInfo += "{\\company ";
        m_textDocInfo += escapeRtfText( docInfo.company );
        m_textDocInfo += "}";
    }

    // Now add who we are in a \comment
    QString revision("$Revision$");
    m_textDocInfo += "{\\comment ";
    m_textDocInfo += "Generated by KWord's RTF Export Filter";
    m_textDocInfo += revision.mid(10).remove('$'); // has a leading and a trailing space.
    m_textDocInfo += "}";

    if ( !docInfo.abstract.isEmpty() )
    {
        m_textDocInfo += "{\\doccomm ";
        m_textDocInfo += escapeRtfText( docInfo.abstract );
        m_textDocInfo += "}";
    }

    return true;
}

bool RTFWorker::doOpenTextFrameSet(void)
{
    return true;
}

bool RTFWorker::doCloseTextFrameSet(void)
{
    return true;
}

QString RTFWorker::openSpan(const FormatData& formatOrigin, const FormatData& format)
{
    QString result;

    result += "{";
    result += textFormatToRtf(formatOrigin.text,format.text,false);

    if ( 1==format.text.verticalAlignment )
    {
        result += "\\sub"; //Subscript
    }
    else if ( 2==format.text.verticalAlignment )
    {
        result += "\\super"; //Superscript
    }

    result += " ";
    return result;
}

QString RTFWorker::closeSpan(const FormatData& , const FormatData& )
{
    QString result;
    result += "}";
    return result;
}

// The following function encodes the kword unicode characters into
// RTF seven bit ASCII. This affects any 8 bit characters.
// They are encoded either with \' or with \u
QString RTFWorker::escapeRtfText ( const QString& text ) const
{
    // initialize strings
    QString escapedText;
    const uint length = text.length();
    for ( uint i = 0; i < length; i++ )
    {
        QChar QCh ( text.at( i ) );  // get out one unicode char from the string
        const ushort ch = QCh.unicode();  // take unicode value of the char

        if ( QCh == '\\' )  escapedText += "\\\\"; // back-slash
        else if ( QCh == '{' )   escapedText += "\\{";
        else if ( QCh == '}' )   escapedText += "\\}";
        else if ( ch >= 32 && ch <= 127) // ASCII character
            escapedText += QCh;
        else if ( ch == 0x0009 ) escapedText += "\\tab "; // tabulator
        else if ( ch == 0x00a0 ) escapedText += "\\~"; // Non-breaking space
        else if ( ch == 0x00ad ) escapedText += "\\-"; // Soft hyphen
        else if ( ch == 0x00b7 ) escapedText += "\\|";
        else if ( ch == 0x2011 ) escapedText += "\\_"; // Non-breaking hyphen
        else if ( ch == 0x2002 ) escapedText += "\\enspace ";
        else if ( ch == 0x2003 ) escapedText += "\\emspace ";
        else if ( ch == 0x2004 ) escapedText += "\\qmspace ";
        else if ( ch == 0x200c ) escapedText += "\\zwnj ";
        else if ( ch == 0x200d ) escapedText += "\\zwj ";
        else if ( ch == 0x200e ) escapedText += "\\ltrmark ";
        else if ( ch == 0x200f ) escapedText += "\\rtlmark ";
        else if ( ch == 0x2013 ) escapedText += "\\endash ";
        else if ( ch == 0x2014 ) escapedText += "\\emdash ";
        else if ( ch == 0x2018 ) escapedText += "\\lquote ";
        else if ( ch == 0x2019 ) escapedText += "\\rquote ";
        else if ( ch == 0x201c ) escapedText += "\\ldblquote ";
        else if ( ch == 0x201d ) escapedText += "\\rdblquote ";
        else if ( ch == 0x2022 ) escapedText += "\\bullet ";
        else if ( ch >= 160 && ch < 256) // check for characters common between ISO-8859-1 and CP1252
        {   // NOTE: 128 to 159 in CP1252 are somewhere else in UTF-8 and do not exist in ISO-8859-1 (### TODO?)
            escapedText += "\\\'";   // escape upper page character to 7 bit
            escapedText += QString::number ( ch, 16 );
        }
        else if ( ch >= 256) // check for a higher code non-ASCII character
        {
            // encode this as decimal unicode with a replacement character.
            escapedText += "\\u";
            escapedText += QString::number ( ch, 10 );
            // We decompose the character. If it works, the first character is whitout any accent.
            // (Of course this only works with Latin letters.)
            // WARNING: QChar::decomposition is not re-entrant in Qt 3.x
            QChar replacement ( QCh.decomposition().at(0) );
            kDebug(30515) << "Proposed replacement character: " << QString(replacement) << endl;

            if (replacement.isNull() || replacement<=' ' || replacement>=char(127)
                || replacement=='{' || replacement=='}' || replacement=='\\')
                replacement='?'; // Not a normal ASCII character, so default to show a ? sign

            escapedText += replacement; // The \uc1 dummy character.

        }
        else
            escapedText += QCh ;

    }

    return escapedText;

}

bool RTFWorker::doFullPaperFormat(const int /*format*/,
    const double width, const double height, const int orientation)
{
    m_paperWidth=width*20;
    m_paperHeight=height*20;
    m_paperOrientation=orientation;
    return true;
}

bool RTFWorker::doFullPaperBorders (const double top, const double left,
    const double bottom, const double right)
{
    m_paperMarginTop=top*20;
    m_paperMarginLeft=left*20;
    m_paperMarginBottom=bottom*20;
    m_paperMarginRight=right*20;
    return true;
}

bool RTFWorker::doFullDefineStyle(LayoutData& layout)
{
    //Register the new style in the style list
    m_styleList << layout;

    // Now we must register a few things (with help of the lookup methods.)
    lookupFont("\\f", layout.formatData.text.fontName);
    lookupColor(QString::null, layout.formatData.text.fgColor);
    lookupColor(QString::null, layout.formatData.text.bgColor);

    return true;
}

static QString writeDate(const QString keyword, const QDateTime& now)
{
    QString str;
    if (now.isValid())
    {
        kDebug(30515) << "Date " << keyword << " " << now.toString() << endl;
        str += '{';
        str += keyword;
        const QDate nowDate(now.date());
        str += "\\yr";
        str += QString::number(nowDate.year());
        str += "\\mo";
        str += QString::number(nowDate.month());
        str += "\\dy";
        str += QString::number(nowDate.day());
        const QTime nowTime(now.time());
        str += "\\hr";
        str += QString::number(nowTime.hour());
        str += "\\min";
        str += QString::number(nowTime.minute());
        str += "\\sec";
        str += QString::number(nowTime.second());
        str += '}';
    }
    else
        kWarning(30515) << "Date " << keyword << " is not valid! Skipping!" << endl;

    return str;
}

bool RTFWorker::doVariableSettings(const VariableSettingsData& vs)
{
    m_textDocInfo += writeDate("\\creatim",vs.creationTime);
    m_textDocInfo += writeDate("\\revtim", vs.modificationTime);
    m_textDocInfo += writeDate("\\printim",vs.printTime);
    m_startPageNumber = vs.startingPageNumber;

    return true;
}

QString RTFWorker::textFormatToRtf(const TextFormatting& formatOrigin,
    const TextFormatting& formatData, const bool force)
{
    // TODO: rename variable formatData
    QString strElement; // TODO: rename this variable

    // Font name
    const QString fontName(formatData.fontName);
    if (!fontName.isEmpty()
        && (force || (formatOrigin.fontName!=formatData.fontName)))
    {
        strElement+=lookupFont("\\f", fontName);
    }

    if (force || (formatOrigin.fontSize!=formatData.fontSize))
    {
        const int size=formatData.fontSize;
        if (size>0)
        {
            strElement+="\\fs";
            strElement+=QString::number(2*size,10);
        }
    }

    if (force || (formatOrigin.italic!=formatData.italic))
    {
        // Font style
        if ( formatData.italic )
        {
            strElement+="\\i";
        }
        else
        {
            strElement+="\\i0";
        }
    }

    if (force || ((formatOrigin.weight>=75)!=(formatData.weight>=75)))
    {
        if ( formatData.weight >= 75 )
        {
            strElement+="\\b";
        }
        else
        {
            strElement+="\\b0";
        }
    }

    if (force || (formatOrigin.fgColor!=formatData.fgColor))
    {
        if ( formatData.fgColor.isValid() )
        {
            strElement+=lookupColor("\\cf", formatData.fgColor);
        }
    }

    if (force || (formatOrigin.bgColor!=formatData.bgColor))
    {
        if ( formatData.bgColor.isValid() )
        {
            strElement+=lookupColor("\\cb", formatData.bgColor);
            strElement+=lookupColor("\\highlight", formatData.bgColor); // MS Word wants this
            // ### FIXME: \highlight is not allowed in style definitions (RTF 1.6)
        }
    }

    if ( force
        || ( formatOrigin.underline != formatData.underline )
        || ( formatOrigin.underlineValue != formatData.underlineValue )
        || ( formatOrigin.underlineStyle != formatData.underlineStyle )
        || ( formatOrigin.underlineWord != formatData.underlineWord ) )
    {
        if ( formatData.underline )
        {
            QString underlineValue = formatData.underlineValue;
            QString underlineStyle = formatData.underlineStyle;
            bool underlineWord = formatData.underlineWord;
            QString ul ( "\\ul" );  // fall-back: simple underline

            if( underlineStyle.isEmpty() ) underlineStyle = "solid";
            if( underlineValue == "1" ) underlineValue = "single";

            if( underlineValue == "double" )
                ul = "\\uldb";
            else if( underlineValue == "single-bold" )
                ul = "\\ulth";
            else if( underlineValue == "wave" )
                ul = "\\ulwave";
            else if( underlineValue == "single" )
            {
                if( underlineStyle == "dash" )
                    ul = "\\uldash";
                else if( underlineStyle == "dot" )
                    ul = "\\uld";
                else if( underlineStyle == "dashdot" )
                    ul = "\\uldashd";
                else if( underlineStyle == "dashdotdot" )
                    ul = "\\uldashdd";
                else if( underlineWord )
                    ul = "\\ulw";
            };

            strElement+= ul;
            if (formatData.underlineColor.isValid())
            {
                strElement+=lookupColor("\\ulc",formatData.underlineColor);
            }
        }
        else
        {
            strElement+="\\ul0";
        }
    }

    if ( force
        || ( formatOrigin.strikeout != formatData.strikeout )
        || ( formatOrigin.strikeoutType != formatData.strikeoutType ) )
    {
        if ( formatData.strikeout )
        {
            if( formatData.strikeoutType == "double" )
                strElement+="\\striked1"; // 1 needed! (The exception that confirms the rule!)
            else
                strElement+="\\strike";
        }
        else
        {
            strElement+="\\strike0"; // ### TODO: \striked0 too?
        }
    }

    return strElement;
}

QString RTFWorker::layoutToRtf(const LayoutData& layoutOrigin,
    const LayoutData& layout, const bool force)
{
    QString strLayout;


    if (force || (layoutOrigin.alignment!=layout.alignment))
    {
        if (layout.alignment=="left")
            strLayout += "\\ql";
        else if (layout.alignment== "right")
            strLayout += "\\qr";
        else if (layout.alignment=="center")
            strLayout += "\\qc";
        else if (layout.alignment=="justify")
            strLayout += "\\qj";
        else if ( layout.alignment=="auto")
        {
            // ### TODO: what for BIDI?
            //strLayout += "\\ql";
        }
        else
        {
            kWarning(30515) << "Unknown alignment: " << layout.alignment << endl;
        }
    }

    if ((layout.indentLeft>=0.0)
        && (force || (layoutOrigin.indentLeft!=layout.indentLeft)))
    {
       strLayout += "\\li";
       strLayout += QString::number(int(layout.indentLeft)*20, 10);
    }

    if ((layout.indentRight>=0.0)
        && (force || (layoutOrigin.indentRight!=layout.indentRight)))
    {
       strLayout += "\\ri";
       strLayout += QString::number(int(layout.indentRight)*20, 10);
    }

    if (force || (layoutOrigin.indentFirst!=layout.indentFirst))
    {
       strLayout += "\\fi";
       strLayout += QString::number(int(layout.indentFirst)*20, 10);
    }

    if ((layout.marginBottom>=0.0)
        && (force || (layoutOrigin.marginBottom!=layout.marginBottom)))
    {
       strLayout += "\\sa";
       strLayout += QString::number(int(layout.marginBottom)*20 ,10);
    }

    if ((layout.marginTop>=0.0)
        && (force || (layoutOrigin.marginTop!=layout.marginTop)))
    {
       strLayout += "\\sb";
       strLayout += QString::number(int(layout.marginTop)*20, 10);
    }

    if (force || (layoutOrigin.keepLinesTogether!=layout.keepLinesTogether))
    {
       if(layout.keepLinesTogether) strLayout += "\\keep";
    }

    // Note: there seems to be too many problems of using a page break in a layout
    // - KWord's RTF import filter makes the page break immediately (also in styles)
    // - AbiWord's RTF import does not like \*\pgbrk
    // ### TODO: decide if we really remove this code
#if 0
    if (force || (layoutOrigin.pageBreakBefore!=layout.pageBreakBefore))
    {
       if(layout.pageBreakBefore) strLayout += "\\pagebb";
    }

    // Note: RTF doesn't specify "page break after"
    // \*\pgbrk0 is used after OpenOffice.org Writer
    if (force || (layoutOrigin.pageBreakAfter!=layout.pageBreakAfter))
    {
       if(layout.pageBreakAfter) strLayout += "\\*\\pgbrk0";
    }
#endif

    if (force
        || ( layoutOrigin.lineSpacingType != layout.lineSpacingType )
        || ( layoutOrigin.lineSpacing != layout.lineSpacing ) )
    {
        if ( layout.lineSpacingType==LayoutData::LS_SINGLE  )
           ;// do nothing, single linespace is default in RTF

        else if ( layout.lineSpacingType==LayoutData::LS_ONEANDHALF  )
           strLayout += "\\sl360\\slmult1"; // one-and-half linespace

        else if ( layout.lineSpacingType==LayoutData::LS_DOUBLE  )
           strLayout += "\\sl480\\slmult1"; // double linespace

        else if ( layout.lineSpacingType==LayoutData::LS_ATLEAST  )
           strLayout += QString("\\sl%1\\slmult0").arg(int(layout.lineSpacing)*20);

        else if ( layout.lineSpacingType==LayoutData::LS_MULTIPLE  )
           strLayout += QString("\\sl%1\\slmult1").arg( int(layout.lineSpacing)*240 );

        else if ( layout.lineSpacingType==LayoutData::LS_CUSTOM )
           // "Custom" in KWord is like "Exactly" in MS Word
           strLayout += QString("\\sl-%1\\slmult0").arg(int(layout.lineSpacing)*20);

        else
            kWarning(30515) << "Unsupported lineSpacingType: " << layout.lineSpacingType << " (Ignoring!)" << endl;
    }

    if (!layout.tabulatorList.isEmpty()
        && (force || (layoutOrigin.tabulatorList!=layout.tabulatorList) ))
    {
        TabulatorList::ConstIterator it;
        for (it=layout.tabulatorList.begin();it!=layout.tabulatorList.end();++it)
        {
            switch ((*it).m_type)
            {
                case 0: default:  break; // left tab is default
                case 1:  strLayout += "\\tqc"; break;
                case 2:  strLayout += "\\tqr"; break;
                case 3:  strLayout += "\\tqdec"; break;
            }

            switch ((*it).m_filling)
            {
                case TabulatorData::TF_NONE: default: break; // without leader/filling
                case TabulatorData::TF_DOT:  strLayout += "\\tldot"; break;
                case TabulatorData::TF_LINE:  strLayout += "\\tlul"; break;

                // these belows are all treated as RTF's \tqul
                case TabulatorData::TF_DASH:
                case TabulatorData::TF_DASHDOT:
                case TabulatorData::TF_DASHDOTDOT:
                    strLayout += "\\tlul"; break;
            }

            // must be the last
            strLayout += "\\tx";
            strLayout += QString::number(int((*it).m_ptpos)*20, 10);

        }
    }

    // shadow support
    // note shadow in KWord is more full-feature/sophisticated than RTF
    // here we just treat KWord's shadow as simple \shad mark-up
    if( layout.shadowDistance > 0 )
    {
       strLayout += "\\shad";
    }

    // TODO: borders

    // This must remain last, as it adds a terminating space.
    strLayout+=textFormatToRtf(layoutOrigin.formatData.text,
        layout.formatData.text,force);

    return strLayout;
}


QString RTFWorker::lookupFont(const QString& markup, const QString& fontName)
{
    if (fontName.isEmpty())
        return QString::null;

    // First we have to remove Qt-typical foundry names, as some RTF readers are confused by them.
    QString cookedFontName(fontName);
    QRegExp regexp("\\s*\\[\\S*\\]"); // Some white space, opening square bracket, some non-white-space, ending square bracket
    cookedFontName.remove(regexp);
    // But we cannot have an empty name font
    if (cookedFontName.isEmpty())
        cookedFontName=fontName;

    kDebug(30515) << "RTFWorker::lookupFont " << fontName << " cooked: " << cookedFontName << endl;

    uint counter=0;  // counts position in font table (starts at 0)
    QString strFont(markup); // markup for font selection
    QStringList::ConstIterator it;

    // search font table for this font
    for( it = m_fontList.begin(); it != m_fontList.end(); counter++, ++it )
    {
        if((*it) == cookedFontName)  // check for match
        {
            strFont += QString::number(counter);
            kDebug(30515) << strFont << endl;
            return strFont;
        }
    }  // end for()

    kDebug(30515) << "New font: " << cookedFontName << " count: " << counter << endl;
    m_fontList << cookedFontName;

    strFont += QString::number(counter);
    return strFont;
}

QString RTFWorker::lookupColor(const QString& markup, const QColor& color)
{
    if (!color.isValid())
        return QString::null;

    uint counter=1;  // counts position in color table starting at 1
    QString strColor(markup);  // Holds RTF markup for the color

    Q3ValueList < QColor > ::ConstIterator it;

    // search color table for this color
    for( it =  m_colorList.begin(); it != m_colorList.end(); counter++, ++it )
    {
        if ( (*it) == color )
        {
            strColor += QString::number(counter);
            return strColor;
        }
    }

    kDebug(30515) << "New color: " << color.name() << " count: " << counter << endl;
    m_colorList << color;

    strColor += QString::number(counter);
    return strColor;
}

QString RTFWorker::lookupStyle(const QString& styleName, LayoutData& returnLayout)
{
    if (styleName.isEmpty())
        return QString::null;

    uint counter=0;  // counts position in style table starting at 0
    QString strMarkup("\\s");  // Holds RTF markup for the style

    Q3ValueList < LayoutData > ::ConstIterator it;
	Q3ValueList < LayoutData > ::ConstIterator end(m_styleList.end());

    // search color table for this color
    for( it =  m_styleList.begin(); it != end; counter++, ++it )
    {
        if ( (*it).styleName == styleName )
        {
            strMarkup += QString::number(counter);
            returnLayout=(*it);
            return strMarkup;
        }
    }

    kDebug(30515) << "New style: " << styleName << " count: " << counter << endl;
    LayoutData layout;
    m_styleList << layout;
    returnLayout=layout;

    strMarkup += QString::number(counter);
    return strMarkup;
}
