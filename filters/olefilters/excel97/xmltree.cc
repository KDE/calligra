/* This file is part of the KDE project

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

#include <kdebug.h>
#include <qdatastream.h>
#include <qstringlist.h>
#include "xmltree.moc"

const char *palette[65] = {
  "#000000", "#ffffff", "#ffffff", "#ffffff", "#ffffff", "#ffffff",
  "#ffffff", "#ffffff", "#000000", "#ffffff", "#ff0000", "#00ff00",
  "#0000ff", "#ffff00", "#ff00ff", "#00ffff", "#800000", "#008000",
  "#000080", "#808000", "#800080", "#008080", "#c0c0c0", "#808080",
  "#9999ff", "#993366", "#ffffcc", "#ccffff", "#660066", "#ff8080",
  "#0066cc", "#ccccff", "#000080", "#ff00ff", "#ffff00", "#00ffff",
  "#800080", "#800000", "#008080", "#0000ff", "#00ccff", "#ccffff",
  "#ccffcc", "#ffff99", "#99ccff", "#ff99cc", "#cc99ff", "#ffcc99",
  "#3366ff", "#33cccc", "#99cc00", "#ffcc00", "#ff9900", "#ff6600",
  "#666699", "#969696", "#003366", "#339966", "#003300", "#333300",
  "#993300", "#993366", "#333399", "#333333", "#ffffff"
};

const int borderStyles[] = {1, 1, 2, 3, 1, 0, 1, 0, 4, 0, 5, 0, 0, 0};
const int ndays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const int ldays[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

XMLTree::XMLTree() : QObject(),table(0L), fontCount(0), footerCount(0),
                    headerCount(0), xfCount(0)
{
    biff = 0;
    date1904 = 0;
    m_streamDepth = 0;
    mergelist.setAutoDelete(true);
    
    root = new QDomDocument("spreadsheet");

    root->appendChild(root->createProcessingInstruction
                      ("xml", "version=\"1.0\" encoding =\"UTF-8\""));

    doc = root->createElement("spreadsheet");
    doc.setAttribute("editor", "KSpread");
    doc.setAttribute("mime", "application/x-kspread");
    root->appendChild(doc);

    paper = root->createElement("paper");
    paper.setAttribute("format", "A4");
    paper.setAttribute("orientation", "Portrait");
    doc.appendChild(paper);

    borders = root->createElement("borders");
    borders.setAttribute("left", 20);
    borders.setAttribute("top", 20);
    borders.setAttribute("right", 20);
    borders.setAttribute("bottom", 20);
    paper.appendChild(borders);

    map = root->createElement("map");
    doc.appendChild(map);

    // ### David: I added m_locale to xmltree.h to make this compile
    //     But setting the language on that locale isn't done at all in here...
    m_locale.setLanguage("C"); // ##### FIXME
}

XMLTree::~XMLTree()
{
    delete root;
    root=0L;
}

const QDomDocument* const XMLTree::part()
{
    return root;
}

void XMLTree::getFont(Q_UINT16, QDomElement &f, Q_UINT16 fontid)
{
    QDomElement font = root->createElement("font");

    font.setAttribute("family", fonts[fontid]->rgch);
    font.setAttribute("size", fonts[fontid]->dyHeight / 20);
    font.setAttribute("weight", fonts[fontid]->bls / 8);

    if ((fonts[fontid]->bls / 8) != 50)
        font.setAttribute("bold", "yes");

    if ((fonts[fontid]->grbit & 0x02) == 2)
        font.setAttribute("italic", "yes");

    if ((fonts[fontid]->grbit & 0x08) == 8)
	font.setAttribute("strikeout", "yes" );

    if (fonts[fontid]->uls != 0)
        font.setAttribute("underline", "yes");

    f.appendChild(font);
}

void XMLTree::getPen(Q_UINT16 xf, QDomElement &f, Q_UINT16 fontid)
{
    int penWidth, penStyle;
    QDomElement border, pen;

    pen = root->createElement("pen");
    pen.setAttribute("width", 0);
    pen.setAttribute("style", 1);
    pen.setAttribute("color", palette[(fonts[fontid]->icv) & 0x7f]);
    f.appendChild(pen);

    if ((xfs[xf]->borderStyle & 0x0f) != 0) {
    border = root->createElement("left-border");
    pen = root->createElement("pen");
    penStyle = xfs[xf]->borderStyle & 0x0f;
    if (penStyle == 1)
        penWidth = 2;
    else if (penStyle == 5)
        penWidth = 4;
    else
        penWidth = 1;
    pen.setAttribute("width", penWidth);
    if(borderStyles[penStyle-1]==0)
      kdDebug(s_area)<<"Border style not supported\n";
    pen.setAttribute("style", borderStyles[penStyle-1]);
    pen.setAttribute("color", ((xfs[xf]->sideBColor ) & 0x7f) == 64 ?
		     "#000000" : palette[(xfs[xf]->sideBColor  ) & 0x7f]);
    /*palette[xfs[xf]->sideBColor & 0x7f]);*/
    border.appendChild(pen);
    f.appendChild(border);
  }

  if (((xfs[xf]->borderStyle >> 4) & 0x0f) != 0) {
    border = root->createElement("right-border");
    pen = root->createElement("pen");
    penStyle = (xfs[xf]->borderStyle >> 4) & 0x0f;
    if (penStyle == 1)
        penWidth = 2;
    else if (penStyle == 5)
        penWidth = 4;
    else
        penWidth = 1;
    pen.setAttribute("width", penWidth);
    pen.setAttribute("style", borderStyles[penStyle-1]);
    if(borderStyles[penStyle-1]==0)
      kdDebug(s_area)<<"Border style not supported\n";
    pen.setAttribute("color", ((xfs[xf]->sideBColor >>7 ) & 0x7f) == 64 ?
		     "#000000" : palette[(xfs[xf]->sideBColor >>7 ) & 0x7f]);


/*palette[(xfs[xf]->sideBColor >> 7) & 0x7f]);*/
    border.appendChild(pen);
    f.appendChild(border);
  }

  if (((xfs[xf]->borderStyle >> 8) & 0x0f) != 0) {
    border = root->createElement("top-border");
    pen = root->createElement("pen");
    penStyle = (xfs[xf]->borderStyle >> 8) & 0x0f;
    if (penStyle == 1)
        penWidth = 2;
    else if (penStyle == 5)
        penWidth = 4;
    else
        penWidth = 1;
    pen.setAttribute("width", penWidth);
    if(borderStyles[penStyle-1]==0)
      kdDebug(s_area)<<"Border style not supported\n";
    pen.setAttribute("style", borderStyles[penStyle-1]);
    pen.setAttribute("color", ((xfs[xf]->topBColor ) & 0x7f) == 64 ?
		     "#000000" : palette[(xfs[xf]->topBColor ) & 0x7f]);
		     /*palette[xfs[xf]->topBColor & 0x7f]);*/
    border.appendChild(pen);
    f.appendChild(border);
  }

  if (((xfs[xf]->borderStyle >> 12) & 0x0f) != 0) {
    border = root->createElement("bottom-border");
    pen = root->createElement("pen");
    penStyle = (xfs[xf]->borderStyle >> 12) & 0x0f;
    if (penStyle == 1)
        penWidth = 2;
    else if (penStyle == 5)
        penWidth = 4;
    else
        penWidth = 1;
    pen.setAttribute("width", penWidth);
    if(borderStyles[penStyle-1]==0)
      kdDebug(s_area)<<"Border style not supported\n";
    pen.setAttribute("style", borderStyles[penStyle-1]);
    pen.setAttribute("color",  ((xfs[xf]->topBColor >> 7) & 0x7f) == 64 ?
		     "#000000" : palette[(xfs[xf]->topBColor >> 7) & 0x7f]);

/*palette[(xfs[xf]->topBColor >> 7) & 0x7f]);*/
    border.appendChild(pen);
    f.appendChild(border);
  }

  if ((xfs[xf]->sideBColor >> 14) & 0x03)
  {
      penStyle = (xfs[xf]->topBColor >> 21) & 0x0f;
      switch ((xfs[xf]->sideBColor >> 14) & 0x03)
      {
          case 1:
              border = root->createElement("fall-diagonal");
              break;
          case 2:
              border = root->createElement("up-diagonal");
              break;
          case 3:
              border = root->createElement("fall-diagonal");
              pen = root->createElement("pen");
              if (penStyle == 1)
                  penWidth = 2;
              else if (penStyle == 2)
                  penWidth = 3;
              else if (penStyle == 5)
                  penWidth = 4;
              else
                  penWidth = 1;
              pen.setAttribute("width", penWidth);
              pen.setAttribute("style", borderStyles[penStyle-1]);
	      if(borderStyles[penStyle-1]==0)
		kdDebug(s_area)<<"Border style not supported\n";
              pen.setAttribute("color", ((xfs[xf]->topBColor >> 14) & 0x7f) == 64 ?
                               "#000000" : palette[(xfs[xf]->topBColor >> 14) & 0x7f]);
              border.appendChild(pen);
              f.appendChild(border);
              border = root->createElement("up-diagonal");
              break;
      }

      pen = root->createElement("pen");
      if (penStyle == 1)
          penWidth = 2;
      else if (penStyle == 2)
          penWidth = 3;
      else if (penStyle == 5)
          penWidth = 4;
      else
          penWidth = 1;
      pen.setAttribute("width", penWidth);
      if(borderStyles[penStyle-1]==0)
	kdDebug(s_area)<<"Border style not supported\n";
      pen.setAttribute("style", borderStyles[penStyle-1]);
      // the following is necessary to handle Excels "Automatic" color option
      // somehow this is only needed for diagonal borders
      pen.setAttribute("color", ((xfs[xf]->topBColor >> 14) & 0x7f) == 64 ?
                       "#000000" : palette[(xfs[xf]->topBColor >> 14) & 0x7f]);
      border.appendChild(pen);
      f.appendChild(border);
  }
}

void XMLTree::getDate(int date, int& year, int& month, int& day)
{
    year = 0;

    while (date > (((year % 4) == 0) ? 366 : 365))
        date -= ((year++%4) == 0) ? 366 : 365;

    if (date1904 == 1)
        year += 4;

    if ((year % 4) == 0)
    {
        for (month = 0; month < 12; ++month)
        {
            if (date <= ldays[month])
                break;
            date -= ldays[month];
        }
    }
    else
    {
        for (month = 0; month < 12; ++month)
        {
            if (date <= ndays[month])
                break;
            date -= ndays[month];
        }
    }

    ++month;
    day = (date == 0) ? 1 : date;
    year += 1900;
}

void XMLTree::getTime( double time, int &hour,int  &min, int &second)
{
  double tmp;
  tmp=time*24;
  hour=(int)tmp;
  tmp=(tmp-hour)*60;
  min=(int)tmp;
  tmp=(tmp-min)*60;
  second=(int)tmp;
}

const QDomElement XMLTree::getFormat(Q_UINT16 xf)
{
    QString s;
    QDomElement format = root->createElement("format");

    if(!xfs[xf])
	return format;
    Q_UINT16 fontid = xfs[xf]->ifnt;

    if (fontid > 3)
        --fontid;

    format.setAttribute("bgcolor", palette[(xfs[xf]->cellColor & 0x7f) > 64 ?
                                          64 : xfs[xf]->cellColor & 0x7f]);

    format.setAttribute("align", (xfs[xf]->align & 0x07) == 0 ? 4 : xfs[xf]->align & 0x07);
    format.setAttribute("alignY", ((xfs[xf]->align >> 4) & 0x07) == 3 ? 2 : ((xfs[xf]->align >> 4) & 0x07) + 1);

    int angle = xfs[xf]->align >> 8;
    if(angle!=255 && angle !=0)
      format.setAttribute("angle", angle < 91 ? angle * (-1) : angle - 90);
    else if(angle==255)
      format.setAttribute("verticaltext", "yes");
    int indent = xfs[xf]->indent & 0x0f;
    if( indent !=0)
      format.setAttribute("indent", (indent*10));
    if ((xfs[xf]->align >> 3) & 0x01 == 1)
        format.setAttribute("multirow", "yes");
    switch (xfs[xf]->ifmt)
    {
        case 0x00:  // We need this to avoid 'default'
            break;
        case 0x01:  // Number 0
            format.setAttribute("precision", "0");
            break;
        case 0x02:  // Number       0.00
            format.setAttribute("precision", "2");
            break;
        case 0x03:  // Number w/comma       0,000
            format.setAttribute("precision", "0");
            break;
        case 0x04:  // Number w/comma       0,000.00
            format.setAttribute("precision", "2");
            break;
        case 0x05:  // Number currency
        case 0x06:
        case 0x07:
        case 0x08:
            format.setAttribute("precision", "2");
            format.setAttribute("format", "10");
            break;
        case 0x09:  // Percent 0%
            format.setAttribute("precision", "0");
            format.setAttribute("format", "25");
            format.setAttribute("faktor", "100");
            break;
        case 0x0A:  // Percent 0.00%
            format.setAttribute("precision", "2");
            format.setAttribute("format", "25");
            format.setAttribute("faktor", "100");
            break;
        case 0x0B:  // Scientific 0.00+E00
            format.setAttribute("precision", "2");
            format.setAttribute("format", "30");
            break;
        case 0x0C:  // Fraction 1 number  e.g. 1/2, 1/3
            format.setAttribute("precision", "-1");
            format.setAttribute("format", "76");
            break;
        case 0x0D:  // Fraction 2 numbers  e.g. 1/50, 25/33
            format.setAttribute("precision", "-1");
            format.setAttribute("format", "77");
            break;
        case 0x0E:	// Date
            format.setAttribute("format", "35");
            break;
        case 0x0F:	// Date
            format.setAttribute("format", "200");
            break;
        case 0x10:	// Date
            format.setAttribute("format", "35");
            break;
        case 0x11:	// Date
            format.setAttribute("format", "206");
            break;
        case 0x12:  // Time: h:mm AM/PM
	    format.setAttribute("format", "52");
            break;
        case 0x13:  // Time: h:mm:ss AM/PM
	  format.setAttribute("format", "53");
            break;
        case 0x14:  // Time: h:mm this format doesn't exist
	  //in kspread => time system h:mm
	    format.setAttribute("format", "50");
            break;
        case 0x15:  // Time: h:mm:ss
	    format.setAttribute("format", "51");
            break;
        case 0x2a:  // Number currency
        case 0x2c:
            format.setAttribute("precision", "2");
            format.setAttribute("format", "10");
            format.setAttribute("faktor", "1");
            break;
        case 0x2D:  // Time: mm:ss
            format.setAttribute("format", "51");
            break;
        case 0x2E:  // Time: [h]:mm:ss
            format.setAttribute("format", "51");
            break;
        case 0x2F:  // Time: mm:ss.0
            format.setAttribute("format", "51");
            break;
        case 0xA4:	// fraction 3 digits
	    format.setAttribute("format", "78");
            break;
        case 0xA5:	// number fraction 1/2
            format.setAttribute("format", "70");
            break;
        case 0xA6:	// number fraction 1/4
            format.setAttribute("format", "71");
            break;
        case 0xA7:	// number fraction 1/8
            format.setAttribute("format", "72");
            break;
        case 0xA8:	// number fraction 1/16
            format.setAttribute("format", "73");
            break;
        case 0xA9:	// number fraction 1/10
            format.setAttribute("format", "74");
            break;
        case 0xAA:	// number fraction 1/100
            format.setAttribute("format", "75");
            break;
        case 0xAB:	// number only neg signed red
	  format.setAttribute("format", "0");
	  format.setAttribute("faktor", "1");
	  format.setAttribute("precision", "2");
	  format.setAttribute("float","3");
	  format.setAttribute("floatcolor","1");
	  break;
    case 0xAC: //fix me format. (neg black)
	  format.setAttribute("format", "0");
	  format.setAttribute("faktor", "1");
	  format.setAttribute("precision", "2");
	  format.setAttribute("float","3");
	  format.setAttribute("floatcolor","2");
	  break;
    case 0xAD: //fix me format. (neg red)
	  format.setAttribute("format", "0");
	  format.setAttribute("faktor", "1");
	  format.setAttribute("precision", "2");
	  format.setAttribute("float","3");
	  format.setAttribute("floatcolor","1");
	  break;
    case 0xAE: //date d/m
	  format.setAttribute("format", "211");
	  break;
    case 0xB0: //date 2/2/00
      format.setAttribute("format", "204");
      break;
    case 0xB1: //date 2-feb
	  format.setAttribute("format", "202");
	  break;
    case 0xB2: //date 2-feb-00
      format.setAttribute("format", "200");
	  break;
    case 0xB3: //date february-00
      format.setAttribute("format", "207");
	  break;
    case 0xB4: //date 2 february 2000
	  format.setAttribute("format", "216");
	  break;
    case 0xB5:  //date 2/2/00 12:00 AM :doesn't exist in kspread
      //=>2/2/00
      //this is a date-time format => problem when it's a time
          format.setAttribute("format", "204");
	  break;
    case 0xB6: //date 2/2/00 0:00 : doesn't exist in kspread
      //=>2/2/00
      //this a a date-time format => problem when
      //it's a time
          format.setAttribute("format", "204");
	  break;
    case 0xB7: //date F doesn't exist in kspread
      //=>F-00
          format.setAttribute("format", "209");
	  break;
    case 0xB8: //date F-00
      format.setAttribute("format", "209");
      break;
    case 0xB9: //time format ?
      format.setAttribute("format", "50");
	  break;
    default:
      if(!formats[xfs[xf]->ifmt])
      {
        kdError() << "Formatting IFMT " << xfs[xf]->ifmt << " NOT found!" << endl;
        break;
      }
      s = QString::fromLatin1(formats[xfs[xf]->ifmt]->rgch,
			      formats[xfs[xf]->ifmt]->cch);
    }

    getFont(xf, format, fontid);
    getPen(xf, format, fontid);

    return format;
}

const QString XMLTree::getFormula(Q_UINT16 row, Q_UINT16 column, QDataStream& rgce)
{
    double number;
    Q_UINT8 byte, ptg;
    Q_UINT16 integer;
    Q_INT16 refRow, refColumn, refRowLast, refColumnLast;
    QString str;
    QStringList parsedFormula;
    QStringList::Iterator stringPtr;

    parsedFormula.append("=");

    while (!rgce.atEnd())
    {
        rgce >> ptg;
        switch (ptg)
        {
	    case 0x01:  // ptgExpr
		kdDebug(s_area) << "WARNING: ptgExpr formula not supported, yet" << endl;
                return ""; // Return empty formula-string on error
		break;
            case 0x03:  // ptgAdd
                stringPtr = parsedFormula.fromLast();
                do {
                    --stringPtr;
                    --stringPtr;
                } while (!(*stringPtr).isEmpty());
                *stringPtr = "+";
                break;
            case 0x04:  // ptgSub
                stringPtr = parsedFormula.fromLast();
                do {
                    --stringPtr;
                    --stringPtr;
                } while (!(*stringPtr).isEmpty());
                *stringPtr = "-";
                break;
            case 0x05:  // ptgMul
                stringPtr = parsedFormula.fromLast();
                do {
                    --stringPtr;
                    --stringPtr;
                } while (!(*stringPtr).isEmpty());
                *stringPtr = "*";
                break;
            case 0x06:  // ptgDiv
                stringPtr = parsedFormula.fromLast();
                do {
                    --stringPtr;
                    --stringPtr;
                } while (!(*stringPtr).isEmpty());
                *stringPtr = "/";
                break;
            case 0x07:  // ptgPower
                stringPtr = parsedFormula.fromLast();
                do {
                    --stringPtr;
                    --stringPtr;
                } while (!(*stringPtr).isEmpty());
                *stringPtr = "^"; // Hmmm, not supported by kspread.
                break;
            case 0x08:  // ptgConcat
                stringPtr = parsedFormula.fromLast();
                do {
                    --stringPtr;
                    --stringPtr;
                } while (!(*stringPtr).isEmpty());
                *stringPtr = "&";
                break;
            case 0x09:  // ptgLT
                stringPtr = parsedFormula.fromLast();
                do {
                    --stringPtr;
                    --stringPtr;
                } while (!(*stringPtr).isEmpty());
                *stringPtr = "<";
                break;
            case 0x0a:  // ptgLE
                stringPtr = parsedFormula.fromLast();
                do {
                    --stringPtr;
                    --stringPtr;
                } while (!(*stringPtr).isEmpty());
                *stringPtr = "<=";
                break;
            case 0x0b:  // ptgEQ
                stringPtr = parsedFormula.fromLast();
                do {
                    --stringPtr;
                    --stringPtr;
                } while (!(*stringPtr).isEmpty());
                *stringPtr = "==";
                break;
            case 0x0c:  // ptgGE
                stringPtr = parsedFormula.fromLast();
                do {
                    --stringPtr;
                    --stringPtr;
                } while (!(*stringPtr).isEmpty());
                *stringPtr = ">=";
                break;
            case 0x0d:  // ptgGT
                stringPtr = parsedFormula.fromLast();
                do {
                    --stringPtr;
                    --stringPtr;
                } while (!(*stringPtr).isEmpty());
                *stringPtr = ">";
                break;
            case 0x0e:  // ptgNE
                stringPtr = parsedFormula.fromLast();
                do {
                    --stringPtr;
                    --stringPtr;
                } while (!(*stringPtr).isEmpty());
                *stringPtr = "!=";
                break;
	    case 0x12:  // ptgUPlus
		kdDebug(s_area) << "WARNING: ptgUPlus formula not supported, yet" << endl;
                return ""; // Return empty formula-string on error
		break;
	    case 0x13:  // ptgUMinus
		kdDebug(s_area) << "WARNING: ptgUMinus formula not supported, yet" << endl;
                return ""; // Return empty formula-string on error
		break;
	    case 0x14:  // ptgPercent
		kdDebug(s_area) << "WARNING: ptgPercent formula not supported, yet" << endl;
                return ""; // Return empty formula-string on error
		break;	    
            case 0x15:  // ptgParen
                stringPtr = parsedFormula.fromLast();
                --stringPtr;
                *stringPtr += ")";
                ++stringPtr;
                do {
                    --stringPtr;
                    --stringPtr;
                } while (!(*stringPtr).isEmpty() && (*stringPtr) != "=");
                ++stringPtr;
                (*stringPtr).prepend("(");
                break;
	    case 0x16:  // ptgMissArg
		kdDebug(s_area) << "WARNING: ptgMissArg formula not supported, yet" << endl;
                return ""; // Return empty formula-string on error
		break;
	    case 0x17:  // ptgStr
		kdDebug(s_area) << "WARNING: ptgStr formula not supported, yet" << endl;
                return ""; // Return empty formula-string on error
		break;
	    case 0x18:  // ptgExtended
		kdDebug(s_area) << "WARNING: ptgExtended formula not supported, yet" << endl;
                return ""; // Return empty formula-string on error
		break;
            case 0x19:  // ptgAttr
                rgce >> byte >> integer;
                if (byte & 0x10)
                {
                    stringPtr = parsedFormula.fromLast();
                    --stringPtr;
                    *stringPtr += ")";
                    (*stringPtr).prepend("sum(");
                }
                break;
	    case 0x1c:  // ptgErr
		kdDebug(s_area) << "WARNING: ptgErr formula not supported, yet" << endl;
                return ""; // Return empty formula-string on error
		break;
            case 0x1d:  // ptgBool
                rgce >> byte;
                if (byte == 1)
                    parsedFormula.append("True");
                else
                    parsedFormula.append("False");
                parsedFormula.append("");
                break;
            case 0x1e:  // ptgInt
                rgce >> integer;
                parsedFormula.append(m_locale.formatNumber((double) integer));
                parsedFormula.append("");
                break;
            case 0x1f:  // ptgNum
                rgce >> number;
                parsedFormula.append(m_locale.formatNumber(number));
                parsedFormula.append("");
                break;
            case 0x21:  // ptgFunc
            case 0x41:
                rgce >> integer;
                stringPtr = parsedFormula.fromLast();
                --stringPtr;
                *stringPtr += ")";

                switch (integer) {
                    case 15:  // sin
                        (*stringPtr).prepend("sin(");
                        break;
                    case 16:  // cos
                        (*stringPtr).prepend("cos(");
                        break;
                    case 17:  // tan
                        (*stringPtr).prepend("tan(");
                        break;
                    case 18:  // atan
                        (*stringPtr).prepend("atan(");
                        break;
                    case 19:  // pi
                        parsedFormula.append("PI()");
                        parsedFormula.append("");
                        break;
                    case 20:  // sqrt
                        (*stringPtr).prepend("sqrt(");
                        break;
                    case 21:  // exp
                        (*stringPtr).prepend("exp(");
                        break;
                    case 22:  // ln
                        (*stringPtr).prepend("ln(");
                        break;
                    case 23:  // log
                        (*stringPtr).prepend("log(");
                        break;
                    case 24:  // fabs
                        (*stringPtr).prepend("fabs(");
                        break;
                    case 25:  // floor
                        (*stringPtr).prepend("floor(");
                        break;
                    case 26:  // sign
                        (*stringPtr).prepend("sign(");
                        break;
                    case 39:  // mod
                        --stringPtr;
                        *stringPtr = ";";
                        --stringPtr;
                        (*stringPtr).prepend("MOD(");
                        break;
                    case 97:  // atan2
                        --stringPtr;
                        *stringPtr = ";";
                        --stringPtr;
                        (*stringPtr).prepend("atan2(");
                        break;
                    case 98:  // asin
                        (*stringPtr).prepend("asin(");
                        break;
                    case 99:  // acos
                        (*stringPtr).prepend("acos(");
                        break;
                    case 184:  // fact
                        (*stringPtr).prepend("fact(");
                        break;
                    case 212:  // ceil
                        *stringPtr = ""; // no exact match, so we have to workaround
                        --stringPtr;
                        *stringPtr = "";
                        --stringPtr;
                        *stringPtr += ")";
                        (*stringPtr).prepend("ceil(");
                        break;
		    case 221:  // today
        		parsedFormula.append("currentDate()");
			parsedFormula.append("");
			break;
                    case 229:  // sinh
                        (*stringPtr).prepend("sinh(");
                        break;
                    case 230:  // cosh
                        (*stringPtr).prepend("cosh(");
                        break;
                    case 231:  // tanh
                        (*stringPtr).prepend("tanh(");
                        break;
                    case 232:  // asinh
                        (*stringPtr).prepend("asinh(");
                        break;
                    case 233:  // acosh
                        (*stringPtr).prepend("acosh(");
                        break;
                    case 337:  // pow
                        --stringPtr;
                        *stringPtr = ";";
                        --stringPtr;
                        (*stringPtr).prepend("pow(");
                        break;
                    case 342:  // radian
                        (*stringPtr).prepend("radian(");
                        break;
                    case 343:  // degree
                        (*stringPtr).prepend("degree(");
                        break;
                    default:
                        kdDebug(s_area) << "Formula contains unhandled function " << integer << endl;
                        break;
                }
                break;
            case 0x22:  // ptgFuncVar
            case 0x42:
                rgce >> byte >> integer;
                stringPtr = parsedFormula.fromLast();
                --stringPtr;
                *stringPtr += ")";
                ++stringPtr;
                for (; byte > 1; --byte) {
                    --stringPtr;
                    --stringPtr;
                    *stringPtr = ";";
                }
                --stringPtr;

                switch (integer) {
                    case 4:  // sum
                        (*stringPtr).prepend("sum(");
                        break;
                    case 5:  // average
                        (*stringPtr).prepend("average(");
                        break;
                    case 6:  // min
                        (*stringPtr).prepend("min(");
                        break;
                    case 7:  // max
                        (*stringPtr).prepend("max(");
                        break;
                    case 183:  // multiply
                        (*stringPtr).prepend("multiply(");
                        break;
                    case 197:  // ent
                        (*stringPtr).prepend("ENT(");
                        *stringPtr += ")"; // no exact match, so we have to workaround
                        ++stringPtr;
                        *stringPtr = "";
                        ++stringPtr;
                        *stringPtr = "";
                        break;
		    default:
                        kdDebug(s_area) << "Formula contains unhandled function " << integer << endl;
                        break;
                }
                break;
            case 0x24:  // ptgRef
            case 0x44:
                if (biff == BIFF_8) {
                    rgce >> refRow >> refColumn;
                    if (refColumn & 0x8000)
                        refRow += -1*row;
                    if (refColumn & 0x4000) {
                        refColumn &= 0x3fff;
                        refColumn += -1*column;
                    }
                } else {
                    rgce >> refRow >> byte;
                    refColumn = byte;
                    if (refRow & 0x8000)
                        refRow += -1*row;
                    if (refRow & 0x4000) {
                        refRow &= 0x3fff;
                        refColumn += -1*column;
                    }
                }
                str = "#";
                str += QString::number((int) refColumn);
                str += "#";
                str += QString::number((int) refRow);
                str += "#";
                parsedFormula.append(str);
                parsedFormula.append("");
                break;
            case 0x25:  // ptgArea
            case 0x45:
                if (biff == BIFF_8) {
                    rgce >> refRow >> refRowLast >> refColumn >> refColumnLast;
                    if (refColumn & 0x8000)
                        refRow += -1*row;
                    if (refColumn & 0x4000) {
                        refColumn &= 0x3fff;
                        refColumn += -1*column;
                    }
                    if (refColumnLast & 0x8000)
                        refRowLast += -1*row;
                    if (refColumnLast & 0x4000) {
                        refColumnLast &= 0x3fff;
                        refColumnLast += -1*column;
                    }
                } else {
                    rgce >> refRow >> refRowLast >> byte;
                    refColumn = byte;
                    rgce >> byte;
                    refColumnLast = byte;
                    if (refRow & 0x8000)
                        refRow += -1*row;
                    if (refRow & 0x4000) {
                        refColumn &= 0x3fff;
                        refColumn += -1*column;
                    }
                    if (refRowLast & 0x8000)
                        refRowLast += -1*row;
                    if (refRowLast & 0x4000) {
                        refColumnLast &= 0x3fff;
                        refColumnLast += -1*column;
                    }
                }
                str = "#";
                str += QString::number((int) refColumn);
                str += "#";
                str += QString::number((int) refRow);
                str += "#:#";
                str += QString::number((int) refColumnLast);
                str += "#";
                str += QString::number((int) refRowLast);
                str += "#";
                parsedFormula.append(str);
                parsedFormula.append("");
                break;
            default:
                kdDebug(s_area) << "Formula contains unhandled ptg " << ptg << endl;
                return ""; // Return empty formula-string on error
                break;
        }
    }
    kdDebug(s_area) << "XMLTree::formula: " << parsedFormula.join("") << endl;
    return parsedFormula.join("");
}

bool XMLTree::invokeHandler(Q_UINT16 opcode, Q_UINT32 bytes, QDataStream &operands)
{
    typedef bool (XMLTree::*method)(Q_UINT32 size, QDataStream &operands);

    typedef struct
    {
        const char *name;
        Q_UINT16 opcode;
        method handler;
    } opcodeEntry;

    static const opcodeEntry funcTab[] =
    {
        { "1904",           0x0022, &XMLTree::_1904 },
        { "ADDIN",          0x0087, 0 /* &XMLTree::_addin */ },
	{ "ADDMENU",        0x00c1, 0 /* &XMLTree::_addmenu */ },
	{ "ARRAY",          0x0021, &XMLTree::_array },
	{ "AUTOFILTERINFO", 0x009d, 0 /* &XMLTree::_autofilterinfo */ },
	{ "AUTOFILTER",     0x009e, 0 /* &XMLTree::_autofilter */ },
        { "BACKUP",         0x0040, &XMLTree::_backup },
        { "BLANK",          0x0001, &XMLTree::_blank },
        { "BOF",            0x0009, &XMLTree::_bof },
        { "BOOKBOOL",       0x00da, &XMLTree::_bookbool },
        { "BOOLERR",        0x0005, &XMLTree::_boolerr },
        { "BOTTOMMARGIN",   0x0029, &XMLTree::_bottommargin },
        { "BOUNDSHEET",     0x0085, &XMLTree::_boundsheet },
	{ "CALCCOUNT",	    0x000c, 0 /* &XMLTree::_calccount */ },
	{ "CALCMODE",	    0x000d, 0 /* &XMLTree::_calcmode */ },
        { "CF",             0x01b1, &XMLTree::_cf },
        { "CONDFMT",        0x01b0, &XMLTree::_condfmt },
        { "CODENAME",       0x00ba, 0 /* &XMLTree::_codename */ },
        { "CODEPAGE",       0x0042, &XMLTree::_codepage },
        { "COLINFO",        0x007d, &XMLTree::_colinfo },
	{ "CONTINUE",       0x003c, 0 /* &XMLTree::_continue */ },
        { "COORDLIST",      0x00a9, 0 /* &XMLTree::_coordlist */ },
	{ "COUNTRY",        0x008c, &XMLTree::_country },
        { "CRN",            0x005a, &XMLTree::_crn },
	{ "DBCELL",         0x00d7, &XMLTree::_dbcell },
        { "DCON",           0x0050, 0 /* &XMLTree::_dcon */ },
        { "DCONREF",        0x0051, 0 /* &XMLTree::_dconref */ },
        { "DCONNAME",       0x0052, 0 /* &XMLTree::_dconname */ },
	{ "DELMENU",        0x00c3, 0 /* &XMLTree::_delmenu */ },
	{ "DEFAULTROWHEIGHT", 0x0025, &XMLTree::_defaultrowheight },
        { "DEFCOLWIDTH",    0x0055, &XMLTree::_defcolwidth },
        { "DELTA",          0x0010, 0 /* &XMLTree::_delta */ },
        { "DOCROUTE",       0x00b8, 0 /* &XMLTree::_docroute */ },
	{ "DSF",            0x0061, 0 /* &XMLTree::_dsf */ },
	{ "COLWIDTH",       0x0024, 0 /* &XMLTree::_colwidth */ },
	{ "COLUMNDEFAULT",  0x0020, 0 /* &XMLTree::_coldefault */ },
        { "DIMENSIONS",     0x0000, &XMLTree::_dimensions },
        { "EDG",            0x0088, 0 /* &XMLTree::_edg */ },
	{ "EOF",            0x000a, &XMLTree::_eof },
        { "EXTERNCOUNT",    0x0016, &XMLTree::_externcount },
        { "EXTERNNAME",     0x0023, &XMLTree::_externname },
        { "EXTERNSHEET",    0x0017, &XMLTree::_externsheet },
        { "EXTSST",         0x00ff, &XMLTree::_extsst },
        { "FILEPASS",       0x002f, &XMLTree::_filepass },
        { "FILESHARING",    0x005b, &XMLTree::_filesharing },
        { "FILESHARING2",   0x00a5, &XMLTree::_filesharing2 },
	{ "FILTERMODE",     0x009b, 0 /* &XMLTree::_filtermode */ },
        { "FNGROUPCOUNT",   0x009c, 0 /* &XMLTree::_fngroupcount */ },
        { "FNGROUPNAME",    0x009a, 0 /* &XMLTree::_fngroupname */ },
	{ "FONT",           0x0031, &XMLTree::_font },
        { "FONT2",          0x0032, 0 /* &XMLTree::_font2 */ },
        { "FOOTER",         0x0015, &XMLTree::_footer },
        { "FORMAT",         0x001e, &XMLTree::_format },
        { "FORMATCOUNT",    0x001f, 0 /* &XMLTree::_formatcount */ },
        { "FORMULA",        0x0006, &XMLTree::_formula },
        { "GCW",            0x00ab, &XMLTree::_gcw },
        { "GRIDSET",        0x0082, 0 /* &XMLTree::_gridset */ },
	{ "GUTS",           0x0080, &XMLTree::_guts },
        { "HCENTER",        0x0083, &XMLTree::_hcenter },
        { "HEADER",         0x0014, &XMLTree::_header },
	{ "HIDEOBJ",        0x008d, 0 /* &XMLTree::_hideobj */ },
	{ "HLINK",          0x01b8, &XMLTree::_hlink },
        { "HORIZONTALPAGEBREAKS", 0x001b, &XMLTree::_horizontalpagebreaks },
        { "IMDATA",         0x007f, &XMLTree::_imdata },
	{ "INDEX",	    0x000b, 0 /* &XMLTree::_index */ },
        { "INTEGER",        0x0002, 0 /* &XMLTree::_integer */ },
	{ "INTERFACEHDR",   0x00e1, 0 /* &XMLTree::_interfacehdr */ },
	{ "INTERFACEEND",   0x00e2, 0 /* &XMLTree::_interfaceend */ },
	{ "ITERATION",      0x0011, 0 /* &XMLTree::_iteration */ },
        { "LABELSST",       0x00fd, &XMLTree::_labelsst },
        { "LABEL",          0x0004, &XMLTree::_label },
        { "LEFTMARGIN",     0x0026, &XMLTree::_leftmargin },
        { "LHRECORD",       0x0094, 0 /* &XMLTree::_lhrecord */ },
	{ "LHNGRAPH",       0x0095, 0 /* &XMLTree::_lhngraph */ },
	{ "LPR",            0x0098, 0 /* &XMLTree::_lpr */ },
	{ "MERGECELL",      0x00e5, &XMLTree::_mergecell },
	{ "MMS",            0x00c1, 0 /* &XMLTree::_mms */ },
	{ "MSOD",           0x00ec, 0 /* &XMLTree::_msod */ },
	{ "MSODGROUP",      0x00eb, 0 /* &XMLTree::_msodgroup */ },
	{ "MSODSEL",        0x00ed, 0 /* &XMLTree::_msodsel */ },
	{ "MULBLANK",       0x00be, &XMLTree::_mulblank },
        { "MULRK",          0x00bd, &XMLTree::_mulrk },
        { "NAME",           0x0018, &XMLTree::_name },
        { "NOTE",           0x001c, &XMLTree::_note },
        { "NUMBER",         0x0003, &XMLTree::_number },
        { "OBJ",            0x005d, 0 /* &XMLTree::_obj */ },
	{ "OBJPROJ",        0x00d3, 0 /* &XMLTree::_objproj */ },
        { "OBJPROTECT",     0x0063, 0 /* &XMLTree::_objprotect */ },
	{ "OLESIZE",        0x00de, 0 /* &XMLTree::_olesize */ },
	{ "PALETTEINFO",    0x0092, 0 /* &XMLTree::_paletteinfo */ },
	{ "PANE",           0x0041, &XMLTree::_pane },
        { "PARAMQRY",       0x00dc, &XMLTree::_paramqry },
        { "PASSWORD",       0x0013, &XMLTree::_password },
	{ "PLS",            0x004d, 0 /* &XMLTree::_pls */ },
	{ "PRECISION",	    0x000e, 0 /* &XMLTree::_precision */ },
        { "PRINTGRIDLINES", 0x002b, 0 /* &XMLTree::_printgridlines */ },
        { "PRINTROWHEADERS", 0x002a, 0 /* &XMLTree::_printrowheaders */ },
        { "PRINTSIZE",      0x0033, 0 /* &XMLTree::_printsize */ },
	{ "PROTECT",        0x0012, &XMLTree::_protect },
        { "PUB",            0x0089, 0 /* &XMLTree::_pub */ },
	{ "QSI",            0x00ad, &XMLTree::_qsi },
        { "RECIPNAME",      0x00b9, &XMLTree::_recipname },
	{ "REFMODE",	    0x000f, 0 /* &XMLTree::_refomde */ },
        { "REFRESHALL",     0x00b7, 0 /* &XMLTree::_refreshall */ },
	{ "RIGHTMARGIN",    0x0027, &XMLTree::_rightmargin },
        { "RK",             0x007e, &XMLTree::_rk },
        { "ROW",            0x0008, &XMLTree::_row },
        { "RSTRING",        0x00d6, 0 /* &XMLTree::_rstring */ },
	{ "SAVERECALC",     0x005f, 0 /* &XMLTree::_saverecalc */ },
	{ "SCENMAN",        0x00ae, 0 /* &XMLTree::_scenman */ },
	{ "SCENARIO",       0x00af, 0 /* &XMLTree::_scenario */ },
	{ "SCENPROTECT",    0x00dd, 0 /* &XMLTree::_scenprotect */ },
	{ "SCL",            0x00a0, &XMLTree::_scl },
        { "SELECTION",      0x001d, 0 /* &XMLTree::_selection */ },
        { "SETUP",          0x00a1, &XMLTree::_setup },
        { "SHRFMLA",        0x00bc, &XMLTree::_shrfmla },
        { "SORT",           0x0090, &XMLTree::_sort },
        { "SOUND",          0x0096, 0 /* &XMLTree::_sound */ },
	{ "SUB",            0x0091, 0 /* &XMLTree::_sub */ },
	{ "SST",            0x00fc, &XMLTree::_sst },
        { "STANDARDWIDTH",  0x0099, &XMLTree::_standardwidth },
        { "STRING",         0x0007, &XMLTree::_string },
        { "STYLE",          0x0093, 0 /* &XMLTree::_style */ },
	{ "SXDB",           0x00c6, 0 /* &XMLTree::_sxdb */ },
	{ "SXDI",           0x00c5, 0 /* &XMLTree::_sxdi */ },
	{ "SXEX",           0x00f1, 0 /* &XMLTree::_sxex */ },
	{ "SXFILT",         0x00f2, 0 /* &XMLTree::_sxfilt */ },
	{ "SXFMLA",         0x00f9, 0 /* &XMLTree::_sxfmla */ },
	{ "SXFORMAT",       0x00fb, 0 /* &XMLTree::_sxformat */ },
	{ "SXIDSTM",        0x00d5, 0 /* &XMLTree::_sxidstm */ },
	{ "SXLI",           0x00b5, 0 /* &XMLTree::_sxli */ },
	{ "SXIVD",          0x00b4, 0 /* &XMLTree::_sxivd */ },
	{ "SXNAME",         0x00f6, 0 /* &XMLTree::_sxname */ },
	{ "SXPAIR",         0x00f8, 0 /* &XMLTree::_sxpair */ },
	{ "SXPI",           0x00b6, 0 /* &XMLTree::_sxpi */ },
	{ "SXRULE",         0x00f0, 0 /* &XMLTree::_sxrule */ },
	{ "SXSELECT",       0x00f7, 0 /* &XMLTree::_sxselect */ },
	{ "SXSTRING",       0x00cd, 0 /* &XMLTree::_sxstring */ },
	{ "SXTBL",          0x00d0, 0 /* &XMLTree::_sxtbl */ },
	{ "SXTBRGIITM",     0x00d1, 0 /* &XMLTree::_sxtbrgiitm */ },
	{ "SXTBPG",         0x00d2, 0 /* &XMLTree::_sxtbpg */ },
        { "SXVD",           0x00b1, 0 /* &XMLTree::_sxvd */ },
        { "SXVI",           0x00b2, 0 /* &XMLTree::_sxvi */ },
        { "SXVIEW",         0x00b0, 0 /* &XMLTree::_sxview */ },
        { "SXVX",           0x00e3, 0 /* &XMLTree::_sxvx */ },
	{ "TABID",          0x003d, &XMLTree::_tabid },
        { "TABIDCONF",      0x00ea, &XMLTree::_tabidconf },
        { "TABLE",          0x0036, &XMLTree::_table },
	{ "TABLE2",         0x0037, 0 /* &XMLTree::_table2 */ },
        { "TEMPLATE",       0x0060, 0 /* &XMLTree::_template */ },
	{ "TOPMARGIN",      0x0028, &XMLTree::_topmargin },
	{ "UDDESC",         0x00df, 0 /* &XMLTree::_uddesc */ },
        { "UNCALCED",       0x005e, 0 /* &XMLTree::_uncalced */ },
	{ "VCENTER",        0x0084, &XMLTree::_vcenter },
        { "VERTICALPAGEBREAKS", 0x001a, &XMLTree::_verticalpagebreaks },
        { "WINDOW1",        0x003d, &XMLTree::_window1 },
        { "WINDOW2",        0x003e, &XMLTree::_window2 },
	{ "WINDOWPROT",     0x0019, 0 /* &XMLTree::_windowprot */ },
        { "WRITEACCESS",    0x005c, &XMLTree::_writeaccess },
        { "WRITEPROT",      0x0086, &XMLTree::_writeprot },
        { "WSBOOL",         0x0081, &XMLTree::_wsbool },
        { "XCT",            0x0059, 0 /* &XMLTree::_xct */ },
	{ "XF",             0x00e0, &XMLTree::_xf },
	{ "XF-OLD",         0x0043, 0 /* &XMLTree::_xfold */ },
    };
    unsigned int i;
    method result;

    // Scan lookup table for operation.

    for (i = 0; funcTab[i].name; i++)
    {
        if (funcTab[i].opcode == (opcode & 0x00FF))
        {
            break;
        }
    }

    // Invoke handler.

    result = funcTab[i].handler;
    if (!result)
    {
        if (funcTab[i].name)
            kdWarning(s_area) << "invokeHandler: unsupported opcode: " <<
                funcTab[i].name <<
                " operands: " << bytes << endl;
        else
            kdWarning(s_area) << "invokeHandler: unsupported opcode: 0x" <<
                QString::number((opcode & 0x00FF), 16) <<
                " operands: " << bytes << endl;

        // Skip data we cannot use.

        //skip(bytes, operands);
    }
    else
    {
        kdDebug(s_area) << "invokeHandler: opcode: " << funcTab[i].name <<
            " operands: " << bytes << endl;
        return (this->*result)(bytes, operands);
    }
    return true;
}

bool XMLTree::_1904(Q_UINT32, QDataStream &body)
{
    body >> date1904;

    return true;
}

bool XMLTree::_array(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_backup(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_blank(Q_UINT32, QDataStream &body)
{
    Q_UINT16 row, column, xf;

    body >> row >> column >> xf;
    QDomElement e = root->createElement("cell");
    e.appendChild(getFormat(xf));
    e.setAttribute("row", (int) ++row);
    e.setAttribute("column", (int) ++column);
    table->appendChild(e);
    return true;
}

bool XMLTree::_bof(Q_UINT32, QDataStream &body)
{
    Q_UINT16 dt;
    Q_UINT16 rupBuild;
    Q_UINT16 rupYear;
    Q_UINT32 bfh;
    Q_UINT32 sfo;

    body >> biff;

    switch (biff)
    {
    case BIFF_5_7:
        body >> dt >> rupBuild >> rupYear;
        break;
    case BIFF_8:
        body >> dt >> rupBuild >> rupYear >> bfh >> sfo;
        break;
    default:
        return false;
    }

    m_streamDepth++;
    if (m_streamDepth == 1)
    {
        if (table != 0L)
        {
            delete table;
            table = 0L;
        }
        switch (dt)
        {
        case 0x5:
            kdDebug(s_area) << "BOF: Workbook globals" << endl;
            break;
        case 0x10:
            table = tables.dequeue();
            kdDebug(s_area) << "BOF: Worksheet: " << table->attribute("name") << endl;
            break;
        case 0x20:
            table = tables.dequeue();
            kdDebug(s_area) << "BOF: Chart: " << table->attribute("name") << endl;
            break;
        default:
            kdWarning(s_area) << "BOF: Unsupported substream: 0x" << QString::number(dt, 16) << endl;
    //        table = tables.dequeue();
    //        delete table;
            table = 0L;
            break;
        }
    }
    return true;
}

bool XMLTree::_bookbool(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_boolerr(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_bottommargin(Q_UINT32, QDataStream &body)
{
    double value;
    body >> value;//value in INCH
    borders.setAttribute("bottom", (value*2.54));

    return true;
}

bool XMLTree::_boundsheet(Q_UINT32, QDataStream &body)
{
    Q_UINT32 lbPlyPos;
    Q_UINT16 grbit;
    Q_UINT16 cch;
    Q_UINT8 tmp8;
    QDomElement *e;

    switch (biff)
    {
    case BIFF_5_7:
        body >> lbPlyPos >> grbit >> tmp8;
        cch = tmp8;
        break;
    case BIFF_8:
        body >> lbPlyPos >> grbit >> cch;
        break;
    default:
        return false;
    }

    if ((grbit & 0x0f) == 0)
    {
        char *name = new char[cch];
        body.readRawBytes(name, cch);
        QString s = QString::fromLatin1(name, cch);
        delete [] name;

        kdDebug(s_area) << "Worksheet: " << s << " at: " << lbPlyPos << endl;
        e = new QDomElement(root->createElement("table"));
        e->setAttribute("name", s);

        // Hide the table if it is either hidden, or very hidden.
        if (((grbit >> 8) & 0x03) != 0)
        {
            e->setAttribute("hide",true);
        }
        map.appendChild(*e);
        tables.enqueue(e);
    }

    if ((grbit & 0x0f) == 1)
    {
        char *name = new char[cch];
        body.readRawBytes(name, cch);
        QString s = QString::fromLatin1(name, cch);
        delete [] name;

	// Macrosheet
	kdDebug(s_area) << "Macrosheet: " << s << " at: " << lbPlyPos << "! UNIMPLEMENTED" << endl;
    }

    if ((grbit & 0x0f) == 2)
    {
        char *name = new char[cch];
        body.readRawBytes(name, cch);
        QString s = QString::fromLatin1(name, cch);
        delete [] name;

	// Chart
	kdDebug(s_area) << "Chart: " << s << " at: " << lbPlyPos << "! UNIMPLEMENTED" << endl;
    }

    return true;
}

bool XMLTree::_cf(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_condfmt(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_codepage(Q_UINT32, QDataStream &body)
{
    Q_UINT16 cv;

    body >> cv;
    return true;
}

bool XMLTree::_colinfo(Q_UINT32, QDataStream &body)
{
    Q_UINT16 first, last, width, xf, options;

    body >> first >> last >> width >>xf>>options;

    bool hidden = (options & 0x0001) ? true : false;

    for (Q_UINT32 i = first; i <= last; ++i)
    {
        QDomElement col = root->createElement("column");
        col.setAttribute("column", (int) i+1);
        col.setAttribute("width", (int) width / 120);
	if(hidden)
	  col.setAttribute("hide", hidden);
	col.appendChild(getFormat(xf));
        table->appendChild(col);
    }

    return true;
}

bool XMLTree::_country(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_crn(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_dbcell(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_defaultrowheight(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_defcolwidth(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_dimensions(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_eof(Q_UINT32, QDataStream &)
{
    MergeInfo *merge;

    QDomElement map = root->documentElement().namedItem("map").toElement();
    QDomNode n = map.firstChild();
    while (!n.isNull() && !mergelist.isEmpty())
    {
	QDomElement e = n.toElement();
	if (!e.isNull() && e.tagName() == "table")
	{
	    QDomNode n2 = e.firstChild();  
	    while (!n2.isNull() && !mergelist.isEmpty())
	    {
		QDomElement e2 = n2.toElement();
		if (!e2.isNull() && e2.tagName() == "cell")
		{
		    QDomNode n3 = e2.firstChild();
		    while (!n3.isNull() && !mergelist.isEmpty())
		    {
			QDomElement e3 = n3.toElement();
			if (!e3.isNull() && e3.tagName() == "format")
			{
			    int row = e2.attribute("row").toInt();
			    int col = e2.attribute("column").toInt();
			    for (merge = mergelist.first(); merge != 0; merge = mergelist.next())
			    {
			        if (row == merge->row() && col == merge->col())
			        {
			    	    e3.setAttribute("rowspan", QString::number(merge->rowspan()));
		    		    e3.setAttribute("colspan", QString::number(merge->colspan()));
				
				    mergelist.remove(mergelist.current());
				    break;
				}
    			    }
			}
			n3 = n3.nextSibling();
		    }
		}
		n2 = n2.nextSibling();
	    }
	}
	n = n.nextSibling();
    }

    m_streamDepth--;
    return true;
}

bool XMLTree::_externcount(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_externname(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_externsheet(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_extsst(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_filepass(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_filesharing(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_filesharing2(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_font(Q_UINT32, QDataStream &body)
{
    QChar *c;
    Q_UINT8 lsb, msb;

    font_rec *f = new font_rec;
    body >> f->dyHeight >> f->grbit >> f->icv >> f->bls >> f->sss;
    body >> f->uls >> f->bFamily >> f->bCharSet >> f->reserved >> f->cch;

    if (biff == BIFF_5_7)
    {
        for (int i = 0; i < f->cch; i++)
        {
            body >> lsb;
            c = new QChar(lsb, 0);
            f->rgch += *c;
        }
    }
    else // BIFF 8
    {
        body >> lsb;
        for (int i = 0; i < f->cch; i++)
        {
            body >> lsb >> msb;
            c = new QChar(lsb, msb);
            f->rgch += *c;
        }
    }
    fonts.insert(fontCount++, f);

    return true;
}

bool XMLTree::_footer(Q_UINT32, QDataStream &body)
{
if (footerCount++ == 0)
  {
        Q_UINT8 cch;

        body >> cch;
        if (!cch) return true;
        char *name = new char[cch];
        body.readRawBytes(name, cch);

        QString s = QString::fromLatin1(name, cch);

        QDomElement e = root->createElement("foot");
        QDomElement text = root->createElement("center");
        text.appendChild(root->createTextNode(s));
        e.appendChild(text);
        paper.appendChild(e);
        delete []name;
	}
    return true;
}

bool XMLTree::_format(Q_UINT32, QDataStream &body)
{
  Q_UINT16 id;
  format_rec *f = new format_rec;
  body >> id >> f->cch;
  f->rgch = new char[f->cch];
  body.readRawBytes(f->rgch, f->cch);
  formats.insert(id, f);

  return true;
}

bool XMLTree::_formula(Q_UINT32 size, QDataStream &body)
{
  if(size <= 22)
  {
    kdWarning(s_area) << "Formula size broken!" << endl;
    return true;
  }
   
  char *store = new char[size];
  Q_UINT16 row, column, xf, skip;
  QByteArray a;

  body >> row >> column >> xf;
  body >> skip >> skip >> skip >> skip >> skip >> skip >> skip >> skip;

  body.readRawBytes(store, size-22);
  a.setRawData(store, size-22);
  QDataStream fbody(a, IO_ReadOnly);
  fbody.setByteOrder(QDataStream::LittleEndian);

  QDomElement e = root->createElement("cell");
  e.appendChild(getFormat(xf));
  e.setAttribute("row", (int) row+1);
  e.setAttribute("column", (int) column+1);

  QDomElement text = root->createElement("text");
  text.appendChild(root->createTextNode(getFormula(row, column, fbody)));
  e.appendChild(text);
  table->appendChild(e);
  
  a.resetRawData(store, size-22);
  delete []store;

  return true;
}

bool XMLTree::_gcw(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_guts(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_hcenter(Q_UINT32, QDataStream &body)
{
    Q_UINT16 flag;
    body >> flag;

    bool hcenter = (flag & 0x1) ? true : false;
    
    if(hcenter)
    {
	// FIXME: Add functionality to kspread
	kdDebug(s_area) << "Printing Information: Center horizontally when printing!" << endl;
    }
    else
    {
	// FIXME: Add functionality to kspread
	kdDebug(s_area) << "Printing Information: Don't center horizontally when printing!" << endl;
    }
    
    return true;
}

bool XMLTree::_header(Q_UINT32, QDataStream &body)
{
    if (headerCount++ == 0)
    {
        Q_UINT8 cch;

        body >> cch;
        if (!cch) return true;
        char *name = new char[cch];
        body.readRawBytes(name, cch);

        QString s = QString::fromLatin1(name, cch);

        QDomElement e = root->createElement("head");
        QDomElement text = root->createElement("center");
        text.appendChild(root->createTextNode(s));
        e.appendChild(text);
        paper.appendChild(e);
        delete []name;
    }
    return true;
}

bool XMLTree::_hlink(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_horizontalpagebreaks(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_imdata(Q_UINT32, QDataStream &)
{
  /* Could be used to find out if this document was writting
   * on Windows or on Macintosh, same for the bitmap format
   */
  return true;
}

bool XMLTree::_label(Q_UINT32, QDataStream &body)
{
  Q_UINT16 row, column, xf, length;

  body >> row >> column >> xf >> length;
  QDomElement e = root->createElement("cell");
  e.appendChild(getFormat(xf));

  char *name = new char[length];
  body.readRawBytes(name, length);
  QString s = QString::fromLatin1(name, length);
  e.setAttribute("row", (int) ++row);
  e.setAttribute("column", (int) ++column);
  QDomElement text = root->createElement("text");
  text.appendChild(root->createTextNode(s));
  e.appendChild(text);
  table->appendChild(e);

  delete []name;
  return true;
}

bool XMLTree::_labelsst(Q_UINT32, QDataStream &body)
{
  Q_UINT16 row, column, xf;
  Q_UINT32 isst;

  body >> row >> column >> xf >> isst;
  QDomElement e = root->createElement("cell");
  e.appendChild(getFormat(xf));

  e.setAttribute("row", (int) ++row);
  e.setAttribute("column", (int) ++column);
  QDomElement text = root->createElement("text");
  text.appendChild(root->createTextNode(*(sst[isst])));
  e.appendChild(text);
  table->appendChild(e);
  return true;
}

bool XMLTree::_leftmargin(Q_UINT32, QDataStream &body)
{
  double value;
  body >> value;
  borders.setAttribute("left", (value*2.54));

  return true;
}

bool XMLTree::_mergecell(Q_UINT32, QDataStream &body)
{
  Q_UINT16 count, firstrow, lastrow, firstcol, lastcol;
  body >> count;
    
  for (int i=0; i < count; ++i) {
    body >> firstrow >> lastrow >> firstcol >> lastcol; 
    mergelist.append(new MergeInfo(firstrow, lastrow, firstcol, lastcol));
  }

  return true;
}

bool XMLTree::_mulblank(Q_UINT32 size, QDataStream &body)
{
  Q_UINT16 row, xf, count, first;

  body >> row >> first;
  count = (size-6)/2;
  for (int i=0; i < count; ++i) {
    body >> xf;
    QDomElement e = root->createElement("cell");
    e.appendChild(getFormat(xf));
    e.setAttribute("row", row+1);
    e.setAttribute("column", first+i+1);
    table->appendChild(e);
  }
  return true;
}

bool XMLTree::_mulrk(Q_UINT32 size, QDataStream &body)
{

  double value = 0;

  QString s;

  Q_UINT16 first, last, row, xf;
  Q_UINT32 number, t[2];

  body >> row >> first;
  last = (size-6)/6;
  for (int i=0; i < last; ++i) {
    body >> xf >> number;

    switch (number & 0x03) {
    case 0:
      t[0] = 0;
      t[1] = number & 0xfffffffc;
      value = *((double*) t);
      break;
    case 1:
      t[0] = 0;
      t[1] = number & 0xfffffffc;
      value = *((double*) t) / 100;
      break;
    case 2:
      value = (double) (number >> 2);
      break;
    case 3:
      value = (double) (number >> 2) / 100;
      break;
    }

    switch (xfs[xf]->ifmt) {
    case 0:
     s = QString::number((int) value);  
     break;
    case 14: // Dates
    case 15:
    case 16:
    case 17:
    case 174:
    case 176:
    case 177:
    case 178:
    case 179:
    case 180:
    case 181:
    case 182:
    case 183:
    case 184:
      int year, month, day;
      getDate((int) value, year, month, day);
      s.sprintf("%d/%d/%d", year, month, day);
      break;
    case 164:
      s = "'0" + QString::number((int) value);
      break;
    default: // Number
      s = m_locale.formatNumber(value);
      break;
    }

    QDomElement e = root->createElement("cell");
    e.appendChild(getFormat(xf));
    e.setAttribute("row", row+1);
    e.setAttribute("column", first+i+1);

    QDomElement text = root->createElement("text");
    text.appendChild(root->createTextNode(s));
    e.appendChild(text);
    table->appendChild(e);
  }

  return true;
}

bool XMLTree::_name(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_note(Q_UINT32, QDataStream &body)
{
  kdDebug(s_area) <<"Note\n";
  Q_UINT16 row,col;

  body >> row >>col;
  kdDebug(s_area) <<"col :"<<++col <<"row :"<<++row<<endl;

  return true;
}

bool XMLTree::_number(Q_UINT32, QDataStream &body)
{
  double value;
  QString s;
  Q_UINT16 row, column, xf;
  body >> row >> column >> xf >> value;
  QDomElement e = root->createElement("cell");
  e.appendChild(getFormat(xf));
  e.setAttribute("row", (int) ++row);
  e.setAttribute("column", (int) ++column);

  switch (xfs[xf]->ifmt) {
    case 0x12:
    case 0x13:
    case 0x14: // Time
    case 0x15:
    case 0x2D:
    case 0x2E:
    case 0x2F:
    case 0xB5:
    case 0xB6:
    case 0xB9:
      int hour, min, second;
      getTime( value, hour, min, second);
      s.sprintf("%d:%d:%d", hour, min, second);
      break;
    default: // Number
      s = m_locale.formatNumber(value);
      break;
    }


  QDomElement text = root->createElement("text");
  text.appendChild(root->createTextNode(s));
  e.appendChild(text);
  table->appendChild(e);

  return true;
}

bool XMLTree::_pane(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_paramqry(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_password(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_protect(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_qsi(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_recipname(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_rightmargin(Q_UINT32, QDataStream &body)
{
  double value;
  body >> value;
  borders.setAttribute("right", (value*2.54));

  return true;
}

bool XMLTree::_rk(Q_UINT32, QDataStream &body)
{
  double value = 0;

  QString s;

  Q_UINT32 number, t[2];
  Q_UINT16 row, column, xf;

  body >> row >> column >> xf >> number;

  switch (number & 0x03) {
  case 0:
    t[0] = 0;
    t[1] = number & 0xfffffffc;
    value = *((double*) t);
    break;
  case 1:
    t[0] = 0;
    t[1] = number & 0xfffffffc;
    value = *((double*) t) / 100;
    break;
  case 2:
    value = (double) (number >> 2);
    break;
  case 3:
    value = (double) (number >> 2) / 100;
    break;
  }

  switch (xfs[xf]->ifmt) {
  case 14: // Dates
  case 15:
  case 16:
  case 17:
  case 174:
  case 176:
  case 177:
  case 178:
  case 179:
  case 180:
  case 181:
  case 182:
  case 183:
  case 184:
    int year, month, day;
    getDate((int) value, year, month, day);
    s.sprintf("%d/%d/%d", year, month, day);
    break;
  default: // Number
    s = m_locale.formatNumber(value);
    break;
  }

  QDomElement e = root->createElement("cell");
  e.appendChild(getFormat(xf));
  e.setAttribute("row", (int) ++row);
  e.setAttribute("column", (int) ++column);

  QDomElement text = root->createElement("text");
  text.appendChild(root->createTextNode(s));

  e.appendChild(text);
  table->appendChild(e);

  return true;
}

bool XMLTree::_row(Q_UINT32, QDataStream &body)
{
  Q_UINT16 rowNr, skip, height,flags,flags2,xf;

  body >> rowNr >> skip >> skip >> height >>flags>>flags>>flags>>flags2;

  xf = flags2 & 0xffff;
  if (!xfs[xf])
  {
    kdError(s_area) << "Missing format definition: " << xf << " in row: " << rowNr << endl;
    xf = 0;
  }
  QDomElement row = root->createElement("row");
  row.setAttribute("row", (int) rowNr + 1);
  row.setAttribute("height", (int) height / 40);
  if (flags & 0x30)
    row.setAttribute("hide",true);
  if (flags & 0x80)
    row.appendChild(getFormat(xf));
  table->appendChild(row);

  return true;
}

bool XMLTree::_scl(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_setup(Q_UINT32, QDataStream &body)
{
    Q_UINT16 papersize, scale, resolution, verresolution, copies, flags;
    
    body >> papersize >> scale >> resolution >> verresolution >> copies >> flags;
    
    if((flags & 0x4) != 0x4)
    {
	if((flags & 0x40) != 0x40)
	{
	    if((flags & 0x2) == 0x2)
	    {
		paper.setAttribute("orientation", "portrait");
	    	kdDebug(s_area) << "Printing Information: Orientation: Vertical!" << endl;
	    }
	    else
	    {
		paper.setAttribute("orientation", "landscape");
	    	kdDebug(s_area) << "Printing Information: Orientation: Horizontal!" << endl;
	    }
	}
    }
    
    if((flags & 0x1) == 0x1)
    {
	// FIXME: Add functionality to kspread
	kdDebug(s_area) << "Printing Information: Print Order: Right then Down" << endl;
    }
    else
    {
	// FIXME: Add functionality to kspread
	kdDebug(s_area) << "Printing Information: Print Order: Down then Right" << endl;
    }

    if((flags & 0x8) == 0x8)
    {
	// FIXME: Add functionality to kspread
	kdDebug(s_area) << "Printing Information: Black and White only!" << endl;
    }
    else
    {
	// FIXME: Add functionality to kspread
	kdDebug(s_area) << "Printing Information: Color Printing!" << endl;
    }

    if((flags & 0x10) == 0x10)
    {
	// FIXME: Add functionality to kspread
	kdDebug(s_area) << "Printing Information: Print as Draft!" << endl;
    }

    if((flags & 0x20) == 0x20)
    {
	// FIXME: Add functionality to kspread
	kdDebug(s_area) << "Printing Information: Printing Comments allowed!" << endl;
    }
    
    return true;
}

bool XMLTree::_shrfmla(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_sort(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_sst(Q_UINT32, QDataStream &body)
{
    char *buffer_8bit;
    QString *s;
    Q_UINT8 grbit;
    Q_UINT16 cch, fRichSt, uniShort;
    Q_UINT32 count, extsst, richSt;

    body >> count >> extsst;
    for (int i = 0; i < (int) count; ++i) {
        body >> cch >> grbit;
        if (!(grbit & 0x01)) // compressed strings with 1 byte per character
        {
            fRichSt = 0;
            if (grbit & 0x08)
                body >> fRichSt;
            buffer_8bit = new char[cch+1];
            body.readRawBytes(buffer_8bit, cch);
            buffer_8bit[cch] = '\0';
            s = new QString(buffer_8bit);
            sst.insert(i, s);
            delete[] buffer_8bit;
	    for (; fRichSt; --fRichSt)
		body >> richSt;
        } else if (grbit & 0x01) // not compressed; strings are unicode
        {
            // this has to be tested
            fRichSt = 0;
            if (grbit & 0x08)
		body >> fRichSt;
            s = new QString;
            for (int j = 0; j < cch; ++j)
            {
                body >> uniShort;
                *s += QChar(uniShort);
            }
            sst.insert(i, s);
	    for (; fRichSt; --fRichSt)
		body >> richSt;
        }
    }

    return true;
}

bool XMLTree::_standardwidth(Q_UINT32, QDataStream &body)
{
  Q_UINT16 width;
  body >> width;
  //kdDebug(s_area)<<"Standard width :"<<width<<endl;
  kdDebug(s_area)<<"Standard width not implemented in kspread\n";
  return true;
}

bool XMLTree::_string(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_tabid(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_tabidconf(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_table(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_topmargin(Q_UINT32, QDataStream &body)
{
  double value;
  body >> value;
  borders.setAttribute("top", (value*2.54));

  return true;
}

bool XMLTree::_vcenter(Q_UINT32, QDataStream &body)
{
    Q_UINT16 flag;
    body >> flag;

    bool vcenter = (flag & 0x1) ? true : false;
    
    if(vcenter)
    {
	// FIXME: Add functionality to kspread
	kdDebug(s_area) << "Printing Information: Center vertically when printing!" << endl;
    }
    else
    {
	// FIXME: Add functionality to kspread
	kdDebug(s_area) << "Printing Information: Don't center vertically when printing!" << endl;
    }
    
    return true;
}

bool XMLTree::_verticalpagebreaks(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_window1(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_window2(Q_UINT32, QDataStream &body)
{
  Q_UINT16 nOpt;
  body>> nOpt;
  if(table!=0)
    {
      if( nOpt & 0x0001 )
	{
	  table->setAttribute("formular",1);
	  // Display Formulas
	  //kdDebug(s_area)<<"Show formular\n";
	}
      else
	{
	  table->setAttribute("formular",0);
	  //kdDebug(s_area)<<"Hide formular\n";
	}

      if( nOpt & 0x0002 )
	{
	  // Display Gridlines
	  //kdDebug(s_area)<<"Show grid\n";
	  table->setAttribute("grid",1);
	}
      else
	{
	  // Display No Gridlines
	  //kdDebug(s_area)<<"Hide grid\n";
	  table->setAttribute("grid",0);
	}

      if( nOpt & 0x0004 )
	kdDebug(s_area)<<"Show col/row hearder\n";
      else
	kdDebug(s_area)<<"Hide col/row hearder. Not store in table\n";

      if( nOpt & 0x0010 )
	{
	  table->setAttribute("hidezero",0);
	  //kdDebug(s_area)<<"Show zero value\n";
	}
      else
	{
	table->setAttribute("hidezero",1);
	//kdDebug(s_area)<<"Hide zero value\n";
	}
    }
  return true;
}

bool XMLTree::_writeaccess(Q_UINT32, QDataStream &body)
{
  Q_UINT8 length;

  body >> length;

  if (biff == BIFF_8) {
    Q_UINT16 skip;
    body >> skip;
  }

  char *name = new char[length];
  body.readRawBytes(name, length);
  emit gotAuthor(QString::fromLatin1(name, length));

  delete []name;

  return true;
}

bool XMLTree::_writeprot(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_wsbool(Q_UINT32, QDataStream &)
{
  return true;
}

bool XMLTree::_xf(Q_UINT32, QDataStream &body)
{
  xf_rec *x = new xf_rec;

  body >> x->ifnt >> x->ifmt >> x->attr >> x->align >> x->indent;
  body >> x->borderStyle >> x->sideBColor >> x->topBColor >> x->cellColor;

  xfs.insert(xfCount++, x);

  return true;
}

