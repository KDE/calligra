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
#include <qnamespace.h>
#include <qdatastream.h>
#include <qstringlist.h>
#include <qdatetime.h>
#include "helper.h"
#include "definitions.h"

Helper::Helper(QDomDocument *root, QPtrList<QDomElement> *tables)
: m_locale("koffice")
{
	// ### David: I added m_locale to xmltree.h to make this compile
	//     But setting the language on that locale isn't done at all in here...
	m_locale.setLanguage(QString("C")); // ##### FIXME

	m_root = root;
	m_tables = tables;

	m_todoFormula.setAutoDelete(true);
	m_formulaList.setAutoDelete(true);

	m_date1904 = 0;
}

Helper::~Helper()
{
}

void Helper::done()
{
	FormulaTodo *todo;
	for(todo = m_todoFormula.first(); todo != 0; todo = m_todoFormula.next())
	{
		SharedFormula *formula;
		for(formula = m_formulaList.first(); formula != 0; formula = m_formulaList.next())
		{
			if(formula->checkrow(todo->row()) && formula->checkcol(todo->col()))
			{
				QString temp = getFormula(todo->row(), todo->col(), *formula->stream(), todo->biff(), true);
				formula->stream()->device()->reset();

				bool finished = false;
				QDomElement map = m_root->documentElement().namedItem("map").toElement();
				QDomNode n = map.firstChild();
				while(!n.isNull() && !finished)
				{
					QDomElement e = n.toElement();
					if(!e.isNull() && e.tagName() == "table")
					{
						QDomNode n2 = e.firstChild();
						while(!n2.isNull() && !finished)
						{
							QDomElement e2 = n2.toElement();
							if(!e2.isNull() && e2.tagName() == "cell")
							{
								if(e2.attribute("row").toInt() == todo->row() + 1 &&
								   e2.attribute("column").toInt() == todo->col() + 1)
								{
									QDomNode n3 = e2.firstChild();
									while(!n3.isNull() && !finished)
									{
										QDomElement e3 = n3.toElement();
										if(!e3.isNull() && e3.tagName() == "text")
										{
											QDomText text = e3.firstChild().toText();
											if(!text.isNull())
												text.setData(temp);

											finished = true;
										}

										n3 = n3.nextSibling();
									}
								}
							}

							n2 = n2.nextSibling();
						}
					}

					n = n.nextSibling();
				}
			}
		}
	}
}

void Helper::addSharedFormula(SharedFormula *formula)
{
	m_formulaList.append(formula);
}

void Helper::addDict(Dictionary dict, int index, void *obj)
{
	if(dict == D_XF)
		m_xfrec.insert(index, static_cast<xfrec *>(obj));
	else if(dict == D_SST)
		m_sstrec.insert(index, static_cast<QString *>(obj));
	else if(dict == D_FONT)
		m_fontrec.insert(index, static_cast<fontrec *>(obj));
	else if(dict == D_FORMAT)
		m_formatrec.insert(index, static_cast<formatrec *>(obj));
}

void *Helper::queryDict(Dictionary dict, int index)
{
	if(dict == D_XF)
		return m_xfrec[index];
	if(dict == D_SST)
		return m_sstrec[index];
	else if(dict == D_FONT)
		return m_fontrec[index];
	else if(dict == D_FORMAT)
		return m_formatrec[index];

	return 0;
}

void Helper::getFont(Q_UINT16, QDomElement &f, Q_UINT16 fontid)
{
	QDomElement font = m_root->createElement("font");

	fontrec *fwork = m_fontrec[fontid];

	font.setAttribute("family", fwork->rgch);
	font.setAttribute("size", fwork->dyHeight / 20);
	font.setAttribute("weight", fwork->bls / 8);

	if((fwork->bls / 8) != 50)
		font.setAttribute("bold", "yes");

	if((fwork->grbit & 0x02) == 2)
		font.setAttribute("italic", "yes");

	if((fwork->grbit & 0x08) == 8)
		font.setAttribute("strikeout", "yes" );

	if(fwork->uls != 0)
		font.setAttribute("underline", "yes");

	f.appendChild(font);
}

PenFormat Helper::borderStyleToQtStyle(int penStyle)
{
	PenFormat pen;
	int penWidth = 1, qtPenStyle = 0;

	switch(penStyle)
	{
		case STYLE_BORDER_NONE:
			qtPenStyle = Qt::NoPen;
			break;
		case STYLE_BORDER_THIN:
			penWidth = 2;
			qtPenStyle = Qt::SolidLine;
			break;
		case STYLE_BORDER_MEDIUM:
			penWidth = 3;
			qtPenStyle = Qt::SolidLine;
			break;
		case STYLE_BORDER_DASHED:
			qtPenStyle = Qt::DashLine;
			break;
		case STYLE_BORDER_DOTTED:
			qtPenStyle = Qt::DotLine;
			break;
		case STYLE_BORDER_THICK:
			penWidth = 4;
			qtPenStyle = Qt::SolidLine;
			break;
		case STYLE_BORDER_DOUBLE:
			// FIXME: How to do that? (Niko)
			qtPenStyle = Qt::SolidLine;
			break;
		case STYLE_BORDER_HAIR:
			penWidth = 1;
			qtPenStyle = Qt::SolidLine;
			break;
		case STYLE_BORDER_MEDIUM_DASH:
			penWidth = 3;
			qtPenStyle = Qt::DashLine;
			break;
		case STYLE_BORDER_DASH_DOT:
			qtPenStyle = Qt::DashDotLine;
			break;
		case STYLE_BORDER_MEDIUM_DASH_DOT:
			penWidth = 3;
			qtPenStyle = Qt::DashDotLine;
			break;
		case STYLE_BORDER_DASH_DOT_DOT:
			qtPenStyle = Qt::DashDotDotLine;
			break;
		case STYLE_BORDER_MEDIUM_DASH_DOT_DOT:
			penWidth = 3;
			qtPenStyle = Qt::DashDotDotLine;
			break;
		case STYLE_BORDER_SLANTED_DASH_DOT:
			// FIXME: How to do that? (Niko)
			qtPenStyle = Qt::DashDotLine;
			break;
	}

	pen.setWidth(penWidth);
	pen.setStyle(qtPenStyle);

	return pen;
}

void Helper::getPen(Q_UINT16 xf, QDomElement &f, Q_UINT16 fontid)
{
	int penStyle;
	QDomElement border, pen;

	fontrec *fwork = m_fontrec[fontid];
	xfrec *xwork = m_xfrec[xf];

	pen = m_root->createElement("pen");
	pen.setAttribute("width", 0);
	pen.setAttribute("style", 1);
	pen.setAttribute("color", palette[(fwork->icv) & 0x3f]);
	f.appendChild(pen);

	if((xwork->borderStyle & 0x0f) != 0)
	{
		border = m_root->createElement("left-border");
		pen = m_root->createElement("pen");
		penStyle = xwork->borderStyle & 0x0f;

		PenFormat qtpen = borderStyleToQtStyle(penStyle);

		pen.setAttribute("width", qtpen.width());
		pen.setAttribute("style", qtpen.style());

		if((xwork->sideBColor) & 0x7f > 65)
		{
			kdDebug() << "Weird workaround needed for this problem! WEIRD!" << endl;
			pen.setAttribute("color", "#000000");
		}
		else
			pen.setAttribute("color", ((xwork->sideBColor) & 0x7f) == 64 ? "#000000" : palette[(xwork->sideBColor) & 0x7f]);

		/*palette[m_xfrec[xf]->sideBColor & 0x7f]);*/
		border.appendChild(pen);
		f.appendChild(border);
	}

	if(((xwork->borderStyle >> 4) & 0x0f) != 0)
	{
		border = m_root->createElement("right-border");
		pen = m_root->createElement("pen");
		penStyle = (xwork->borderStyle >> 4) & 0x0f;

		PenFormat qtpen = borderStyleToQtStyle(penStyle);

		pen.setAttribute("width", qtpen.width());
		pen.setAttribute("style", qtpen.style());
		pen.setAttribute("color", ((xwork->sideBColor >> 7) & 0x7f) == 64 ? "#000000" : palette[(xwork->sideBColor >> 7) & 0x7f]);

		/*palette[(m_xfrec[xf]->sideBColor >> 7) & 0x7f]);*/
		border.appendChild(pen);
		f.appendChild(border);
	}

	if(((xwork->borderStyle >> 8) & 0x0f) != 0)
	{
		border = m_root->createElement("top-border");
		pen = m_root->createElement("pen");
		penStyle = (xwork->borderStyle >> 8) & 0x0f;

		PenFormat qtpen = borderStyleToQtStyle(penStyle);

		pen.setAttribute("width", qtpen.width());
		pen.setAttribute("style", qtpen.style());
		pen.setAttribute("color", ((xwork->topBColor) & 0x7f) == 64 ? "#000000" : palette[(xwork->topBColor) & 0x7f]);

		/*palette[m_xfrec[xf]->topBColor & 0x7f]);*/
		border.appendChild(pen);
		f.appendChild(border);
	}

	if(((xwork->borderStyle >> 12) & 0x0f) != 0)
	{
		border = m_root->createElement("bottom-border");
		pen = m_root->createElement("pen");
		penStyle = (xwork->borderStyle >> 12) & 0x0f;

		PenFormat qtpen = borderStyleToQtStyle(penStyle);

		pen.setAttribute("width", qtpen.width());
		pen.setAttribute("style", qtpen.style());
		pen.setAttribute("color", ((xwork->topBColor >> 7) & 0x7f) == 64 ? "#000000" : palette[(xwork->topBColor >> 7) & 0x7f]);

		/*palette[(m_xfrec[xf]->topBColor >> 7) & 0x7f]);*/
		border.appendChild(pen);
		f.appendChild(border);
	}

	if((xwork->sideBColor >> 14) & 0x03)
	{
		penStyle = (xwork->topBColor >> 21) & 0x0f;
		switch((xwork->sideBColor >> 14) & 0x03)
		{
			case 1:
				border = m_root->createElement("fall-diagonal");
				break;
			case 2:
				border = m_root->createElement("up-diagonal");
				break;
			case 3:
				border = m_root->createElement("fall-diagonal");
				pen = m_root->createElement("pen");

				PenFormat qtpen = borderStyleToQtStyle(penStyle);

				pen.setAttribute("width", qtpen.width());
				pen.setAttribute("style", qtpen.style());
				pen.setAttribute("color", ((xwork->topBColor >> 14) & 0x7f) == 64 ? "#000000" : palette[(xwork->topBColor >> 14) & 0x7f]);
				border.appendChild(pen);
				f.appendChild(border);
				border = m_root->createElement("up-diagonal");
				break;
		}

		pen = m_root->createElement("pen");

		PenFormat qtpen = borderStyleToQtStyle(penStyle);

		pen.setAttribute("width", qtpen.width());
		pen.setAttribute("style", qtpen.style());

		// the following is necessary to handle Excels "Automatic" color option
		// somehow this is only needed for diagonal borders
		pen.setAttribute("color", ((xwork->topBColor >> 14) & 0x7f) == 64 ?	"#000000" : palette[(xwork->topBColor >> 14) & 0x7f]);
		border.appendChild(pen);
		f.appendChild(border);
	}
}

QDate Helper::getDate(double _numdays)
{
	long numdays = (long) _numdays;

	QDate date;

        /* Excel's strange 1900/1904 modes:
	 *  1900: day#1 starts at 01.jan.1900,
	 *  1904: day#1 starts at 02.jan.1904 */
	if (!m_date1904)
		date = QDate(1899, 12, 31);
	else
		date = QDate(1903, 12, 31);

	date = date.addDays(numdays);

	if (date.year() >= 1904)
		date = date.addDays(-1);

	return date;
}

void Helper::getTime(double time, int &hour,int  &min, int &second)
{
	double tmp = time * 24;

	hour = (int) tmp;
	tmp = (tmp - hour) * 60;

	min = (int) tmp;
	tmp = (tmp - min) * 60;

	second = (int) tmp;
}




#define EXC_RK_INTFLAG 0x02
#define EXC_RK_100FLAG 0x01

double Helper::GetDoubleFromRK( Q_UINT32 nRKValue )
{
    double fVal;

    //kdDebug() << "RK raw: " << nRKValue << " => " << QString::number( nRKValue, 2 ) << endl;
    if( nRKValue & EXC_RK_INTFLAG )
    {
        fVal = (double) (*((Q_UINT32*) &nRKValue) >> 2); // integer
        kdDebug() << "RK integer: " << fVal << endl;
    }
    else
    {
        // 64-Bit IEEE-Float
#ifdef __BIGENDIAN
        ((Q_UINT32*) &fVal)[1] = 0;                         // lower 32 bits = 0
        ((Q_UINT32*) &fVal)[0] = nRKValue & 0xFFFFFFFC; // bit 0, 1 = 0
#else
        ((Q_UINT32*) &fVal)[0] = 0;                         // lower 32 bits = 0
        ((Q_UINT32*) &fVal)[1] = nRKValue & 0xFFFFFFFC; // bit 0, 1 = 0
#endif
        kdDebug() << "RK float: " << fVal << endl;
    }

    if( nRKValue & EXC_RK_100FLAG )
        fVal *= 0.01;

    kdDebug() << "RK end result: " << fVal << endl;

    return fVal;
}

QString Helper::formatValue( double value, Q_UINT16 xf )
{
	QString s;

	// kdWarning(30511) << __FUNCTION__ << " value " << value << " format " << xf << endl;

	switch(xf)
	{
		case 14: // Dates
		case 15:
		case 16:
		case 17:

		case 26: // assume Date-format (normally defined by file)
		case 28:

			s = locale().formatDate(getDate(value),true);
			break;
		default: // Number
			s = locale().formatNumber(value);
			break;
	}
	// kdWarning(30511) << __FUNCTION__ << " RESULT: " << s << endl;

	return s;

}

const QDomElement Helper::getFormat(Q_UINT16 xf)
{
	QString s;
	QDomElement format = m_root->createElement("format");

	xfrec *xwork = m_xfrec[xf];

	if(!xwork)
		return format;

	Q_UINT16 fontid = xwork->ifnt;

	if(fontid > 3)
		--fontid;

	format.setAttribute("bgcolor", palette[(xwork->cellColor & 0x7f) > 64 ? 64 : xwork->cellColor & 0x7f]);
	format.setAttribute("align", (xwork->align & 0x07) == 0 ? 4 : xwork->align & 0x07);
	format.setAttribute("alignY", ((xwork->align >> 4) & 0x07) == 3 ? 2 : ((xwork->align >> 4) & 0x07) + 1);

	int angle = xwork->align >> 8;
	if(angle != 255 && angle != 0)
		format.setAttribute("angle", angle < 91 ? angle * (-1) : angle - 90);
	else if(angle == 255)
		format.setAttribute("verticaltext", "yes");

	int indent = xwork->indent & 0x0f;
	if(indent != 0)
		format.setAttribute("indent", (indent * 10));
	if((xwork->align >> 3) & 0x01 == 1)
		format.setAttribute("multirow", "yes");

	switch(xwork->ifmt) {
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
		case 0x31:  // Date: dd.mm.yy
			format.setAttribute("format", "204");
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
			if(!m_formatrec[xwork->ifmt])
			{
				kdWarning() << "Formatting IFMT 0x" << QString::number(xwork->ifmt, 16) << " NOT implemented!" << endl;
				break;
			}

			s = QString::fromLatin1(m_formatrec[xwork->ifmt]->rgch, m_formatrec[xwork->ifmt]->cch);
	}

	getFont(xf, format, fontid);
	getPen(xf, format, fontid);

	return format;
}

void getReference(Q_UINT16 row, Q_UINT16 column, Q_INT16 &refRow, Q_INT16 &refColumn, Q_UINT16 biff, bool shared, QString &rowSign, QString &colSign)
{
	if(biff == BIFF_8)
	{
		bool rowRelative = (refColumn & 0x8000);
		bool colRelative = (refColumn & 0x4000);
		refColumn &= 0x3fff;

		rowSign = rowRelative ? "#" : "$";
		colSign = colRelative ? "#" : "$";

		if( rowRelative && !shared)
			refRow -= row;

		if( colRelative )
                {
			// column is int8 in shared, i.e 128 to 255 becomes  -128 to -1
			if( shared )
                            refColumn = (refColumn<128) ? refColumn : refColumn-256;
                        else
			refColumn -= column;
                }
	}
	else
	{
		// TODO: Test this part!

		rowSign = (refRow & 0x8000) ? "#" : "$";
		colSign = (refRow & 0x4000) ? "#" : "$";

		refRow &= 0x3fff;

		if(refRow & 0x8000)
		{
			if(shared && (refRow & 0x2000))
				refRow |= 0xc000;
			else
				refRow -= row;
		}

		if(refRow & 0x4000)
		{
			if(!shared)
				refColumn -= column;
			else
				refColumn = (Q_INT8) refColumn;
		}
	}

	// Correct positions in absolute mode
	// TODO: Also in BIFF != 8 mode ??
	if(colSign == "$")
		refColumn++;
	if(rowSign == "$")
		refRow++;
}

struct sExcelFunction
{
	const char *name;
	Q_UINT16 index;
	Q_UINT16 params;
};

static const sExcelFunction ExcelFunctions[] =
{
	{ "COUNT",            0,    0 },
	{ "IF",               1,    0 },
	{ "ISNV",             2,    1 },
	{ "ISERROR",          3,    1 },
	{ "SUM",              4,    0 },
	{ "AVERAGE",          5,    0 },
	{ "MIN",              6,    0 },
	{ "MAX",              7,    0 },
	{ "ROW",              8,    0 },
	{ "COLUMN",           9,    0 },
	{ "NOVALUE",          10,   0 },
	{ "NBW",              11,   0 },
	{ "STDEV",            12,   0 },
	{ "DOLLAR",           13,   0 },
	{ "FIXED",            14,   0 },
	{ "SIN",              15,   1 },
	{ "COS",              16,   1 },
	{ "TAN",              17,   1 },
	{ "ATAN",             18,   1 },  /* EXCEL: ARCTAN */
	{ "PI",               19,   0 },
	{ "SQRT",             20,   1 },
	{ "EXP",              21,   1 },
	{ "LN",               22,   1 },
	{ "LOG10",            23,   1 },
	{ "ABS",              24,   1 },
	{ "INT",              25,   1 },
	{ "SIGN",             26,   1 },  /* EXCEL: PLUSMINUS */
	{ "ROUND",            27,   2 },
	{ "LOOKUP",           28,   0 },
	{ "INDEX",            29,   0 },
	{ "REPT",             30,   2 },
	{ "MID",              31,   3 },
	{ "LEN",              32,   1 },
	{ "VALUE",            33,   1 },
	{ "TRUE",             34,   0 },
	{ "FALSE",            35,   0 },
	{ "AND",              36,   0 },
	{ "OR",               37,   0 },
	{ "NOT",              38,   1 },
	{ "MOD",              39,   2 },
	{ "DCOUNT",           40,   3 },
	{ "DSUM",             41,   3 },
	{ "DAVERAGE",         42,   3 },
	{ "DMIN",             43,   3 },
	{ "DMAX",             44,   3 },
	{ "DSTDEV",           45,   3 },
	{ "VAR",              46,   0 },
	{ "DVAR",             47,   3 },
	{ "TEXT",             48,   2 },
	{ "RGP",              49,   0 },
	{ "TREND",            50,   0 },
	{ "RKP",              51,   0 },
	{ "GROWTH",           52,   0 },
	{ "BW",               56,   0 },
	{ "ZW",               57,   0 },
	{ "ZZR",              58,   0 },
	{ "RMZ",              59,   0 },
	{ "ZINS",             60,   0 },
	{ "MIRR",             61,   3 },
	{ "IRR",              62,   0 },
	{ "RANDOM",           63,   0 },
	{ "MATCH",            64,   0 },
	{ "DATE",             65,   3 },
	{ "TIME",             66,   3 },
	{ "DAY",              67,   1 },
	{ "MONTH",            68,   1 },
	{ "YEAR",             69,   1 },
	{ "DAYOFWEEK",        70,   0 },
	{ "HOUR",             71,   1 },
	{ "MIN",              72,   1 },
	{ "SEC",              73,   1 },
	{ "NOW",              74,   0 },
	{ "AREAS",            75,   1 },
	{ "ROWS",             76,   1 },
	{ "COLUMNS",          77,   1 },
	{ "OFFSET",           78,   0 },
	{ "SEARCH",           82,   0 },
	{ "TRANSPOSE",        83,   1 },
	{ "TYPE",             86,   1 },
	{ "ATAN2",            97,   2 },  /* EXCEL: ARCTAN2 */
	{ "ASIN",             98,   1 },  /* EXCEL: ARCSIN */
	{ "ACOS",             99,   1 },  /* EXCEL: ARCCOS */
	{ "CHOSE",            100,  0 },
	{ "HLOOKUP",          101,  0 },
	{ "VLOOKUP",          102,  0 },
	{ "ISREF",            105,  1 },
	{ "LOG",              109,  0 },
	{ "CHAR",             111,  1 },
	{ "LOWER",            112,  1 },
	{ "UPPER",            113,  1 },
	{ "PROPER",           114,  1 },
	{ "LEFT",             115,  0 },
	{ "RIGHT",            116,  0 },
	{ "EXACT",            117,  2 },
	{ "TRIM",             118,  1 },
	{ "REPLACE",          119,  4 },
	{ "SUBSTITUTE",       120,  0 },
	{ "CODE",             121,  1 },
	{ "FIND",             124,  0 },
	{ "CELL",             125,  0 },
	{ "ISERR",            126,  1 },
	{ "ISTEXT",           127,  1 },
	{ "ISNUMBER",         128,  1 },
	{ "ISBLANK",          129,  1 },
	{ "T",                130,  1 },
	{ "N",                131,  1 },
	{ "DATEVALUE",        140,  1 },
	{ "TIMEVALUE",        141,  1 },
	{ "SLN",              142,  3 },  /* EXCEL: LIA */
	{ "SYD",              143,  4 },  /* EXCEL: DIA */
	{ "GDA",              144,  0 },
	{ "INDIRECT",         148,  0 },
	{ "CLEAN",            162,  1 },
	{ "MATDET",           163,  1 },
	{ "MATINV",           164,  1 },
	{ "MATMULT",          165,  2 },
	{ "ZINSZ",            167,  0 },
	{ "KAPZ",             168,  0 },
	{ "COUNT2",           169,  0 },
	{ "PRODUCT",          183,  0 },
	{ "FACT",             184,  1 },
	{ "DPRODUCT",         189,  3 },
	{ "ISNONTEXT",        190,  1 },
	{ "STDEVP",           193,  0 },
	{ "VARP",             194,  0 },
	{ "DSTDEVP",          195,  3 },
	{ "DVARP",            196,  3 },
	{ "TRUNC",            197,  0 },
	{ "ISLOGICAL",        198,  1 },
	{ "DCOUNTA",          199,  3 },
	{ "ROUNDUP",          212,  2 },
	{ "ROUNDDOWN",        213,  2 },
	{ "RANK",             216,  0 },
	{ "ADDRESS",          219,  0 },
	{ "GETDIFFDATE360",   220,  0 },
	{ "CURRENTDATE",      221,  0 },  /* EXCEL: TODAY */
	{ "VBD",              222,  0 },
	{ "MEDIAN",           227,  0 },
	{ "SUMPRODUCT",       228,  0 },
	{ "SINH",             229,  1 },  /* EXCEL: SINHYP */
	{ "COSH",             230,  1 },  /* EXCEL: COSHYP */
	{ "TANH",             231,  1 },  /* EXCEL: TANHYP */
	{ "ASINH",            232,  1 },  /* EXCEL: ARCSINHYP */
	{ "ACOSH",            233,  1 },  /* EXCEL: ARCCOSHYP */
	{ "ATANH",            234,  1 },  /* EXCEL: ARCTANHYP */
	{ "DGET",             235,  3 },
	{ "INFO",             244,  1 },
	{ "DB",               247,  0 },  /* EXCEL: GDA2 */
	{ "FREQUENCY",        252,  2 },
	{ "ERRORTYPE",        261,  1 },
	{ "AVEDEV",           269,  0 },
	{ "BETADIST",         270,  0 },
	{ "GAMMALN",          271,  1 },
	{ "BETAINV",          272,  0 },
	{ "BINOMDIST",        273,  4 },
	{ "CHIDIST",          274,  2 },
	{ "CHIINV",           275,  2 },
	{ "COMBIN",           276,  2 },
	{ "CONFIDENCE",       277,  3 },
	{ "KRITBINOM",        278,  3 },
	{ "EVEN",             279,  1 },
	{ "EXPDIST",          280,  3 },
	{ "FDIST",            281,  3 },
	{ "FINV",             282,  3 },
	{ "FISHER",           283,  1 },
	{ "FISHERINV",        284,  1 },
	{ "FLOOR",            285,  2 },
	{ "GAMMADIST",        286,  4 },
	{ "GAMMAINV",         287,  3 },
	{ "CEIL",             288,  2 },
	{ "HYPGEOMDIST",      289,  4 },
	{ "LOGNORMDIST",      290,  3 },
	{ "LOGINV",           291,  3 },
	{ "NEGBINOMVERT",     292,  3 },
	{ "NORMDIST",         293,  4 },
	{ "STDNORMDIST",      294,  1 },
	{ "NORMINV",          295,  3 },
	{ "SNORMINV",         296,  1 },
	{ "STANDARD",         297,  3 },
	{ "ODD",              298,  1 },
	{ "VARIATIONEN",      299,  2 },
	{ "POISSONDIST",      300,  3 },
	{ "TDIST",            301,  3 },
	{ "WEIBULL",          302,  4 },
	{ "SUMXMY2",          303,  2 },
	{ "SUMX2MY2",         304,  2 },
	{ "SUMX2DY2",         305,  2 },
	{ "CHITEST",          306,  2 },
	{ "CORREL",           307,  2 },
	{ "COVAR",            308,  2 },
	{ "FORECAST",         309,  3 },
	{ "FTEST",            310,  2 },
	{ "INTERCEPT",        311,  2 },
	{ "PEARSON",          312,  2 },
	{ "RSQ",              313,  2 },
	{ "STEYX",            314,  2 },
	{ "SLOPE",            315,  2 },
	{ "TTEST",            316,  4 },
	{ "PROB",             317,  0 },
	{ "DEVSQ",            318,  0 },
	{ "GEOMEAN",          319,  0 },
	{ "HARMEAN",          320,  0 },
	{ "SUMSQ",            321,  0 },
	{ "KURT",             322,  0 },
	{ "SCHIEFE",          323,  0 },
	{ "ZTEST",            324,  0 },
	{ "LARGE",            325,  2 },
	{ "SMALL",            326,  2 },
	{ "QUARTILE",         327,  2 },
	{ "PERCENTILE",       328,  2 },
	{ "PERCENTRANK",      329,  0 },
	{ "MODALVALUE",       330,  0 },
	{ "TRIMMEAN",         331,  2 },
	{ "TINV",             332,  2 },
	{ "CONCATENATE",      336,  0 },  /* EXCEL: CONCAT */
	{ "POWER",            337,  2 },
	{ "RADIANS",          342,  1 },
	{ "DEGREES",          343,  1 },
	{ "SUBTOTAL",         344,  0 },
	{ "SUMIF",            345,  0 },
	{ "COUNTIF",          346,  2 },
	{ "COUNTBLANK",       347,  1 },
	{ "ROMAN",            354,  0 },
	{ "ISPMT",            350,  4 },
	{ "GETPIVOTDATA",     358,  2 },
	{ "AVERAGEA",         361,  0 },
	{ "MAXA",             362,  0 },
	{ "MINA",             363,  0 },
	{ "STDEVPA",          364,  0 },
	{ "VARPA",            365,  0 },
	{ "STDEVA",           366,  0 },
	{ "VARA",             367,  0 }
};

static const sExcelFunction *ExcelFunction(Q_UINT16 nIndex)
{
	// Scan lookup table for operation.
	for(unsigned int i = 0; ExcelFunctions[i].name; i++)
	{
		if(ExcelFunctions[i].index == nIndex)
			return &ExcelFunctions[i];
	}
	return 0;
}

static QString Excel_ErrorString(Q_UINT8 no)
{
	switch(no)
	{
		case 0x00:
			return "#NULL!";
		case 0x07:
			return "#DIV/0!";
		case 0x0F:
			return "#VALUE!";
		case 0x17:
			return "#REF!";
		case 0x1D:
			return "#NAME?";
		case 0x24:
			return "#NUM!";
		case 0x2A:
			return "#N/A!";
	}

	return "#UNKNOWN!";
}

const QString &concatValues(QStringList *parsedFormula, int count, QString joinVal, QString prefix = QString::null, QString postfix = QString::null)
{
	QString sum;
	while(count)
	{
		count--;
		sum.prepend(parsedFormula->last());

		if(count)
			sum.prepend(joinVal);

		parsedFormula->pop_back();
	}

	if(!prefix.isNull())
		sum.prepend(prefix);

	if(!postfix.isNull())
		sum.append(postfix);

	parsedFormula->append(sum);
	return parsedFormula->last();
}

#define NA(x) QString("NA_%1  Formula: %2").arg(x).arg(parsedFormula.join("  "))

const QString Helper::getFormula(Q_UINT16 row, Q_UINT16 column, QDataStream &rgce, Q_UINT16 biff, bool shared)
{
	double number;
	Q_UINT8 byte, ptg, count;
	Q_UINT16 integer;
	Q_INT16 refRow, refColumn, refRowLast, refColumnLast;
	QString rowSign1, rowSign2, colSign1, colSign2;
	QString str, newop;
	QStringList parsedFormula;
	bool found;
	const sExcelFunction *excelFunc;

	parsedFormula.append("=");

	// kdWarning(30511) << "Entering " << __FUNCTION__ << endl;

	while (!rgce.atEnd())
	{
		rgce >> ptg;

		// Correct ptg parsing!
		ptg = ((ptg & 0x40) ? (ptg | 0x20) : ptg) & 0x3F;

		// kdWarning(30511) << "Parsing ptg " << QString::number(ptg, 16)  << endl;

		switch (ptg)
		{
			case 0x01:  // ptgExpr
				Q_UINT16 tlrow, tlcol;
				rgce >> tlrow >> tlcol;

				found = false;

				SharedFormula *formula;
				for(formula = m_formulaList.first(); formula != 0; formula = m_formulaList.next())
				{
					if(formula->checkrow(tlrow) && formula->checkcol(tlcol))
					{
						found = true;
						QString temp = getFormula(row, column, *formula->stream(), biff, true);
						formula->stream()->device()->reset();

						parsedFormula.append(temp.mid(1));
					}
				}

				if(!found)
					m_todoFormula.append(new FormulaTodo(tlcol, tlrow, biff));

				break;
			case 0x03:  // ptgAdd
				concatValues(&parsedFormula, 2, "+");
				break;
			case 0x04:  // ptgSub
				concatValues(&parsedFormula, 2, "-");
				break;
			case 0x05:  // ptgMul
				concatValues(&parsedFormula, 2, "*");
				break;
			case 0x06:  // ptgDiv
				concatValues(&parsedFormula, 2, "/");
				break;
			case 0x07:  // ptgPower
				concatValues(&parsedFormula, 2, "^"); // Hmmm, not supported by kspread.
				break;
			case 0x08:  // ptgConcat
				concatValues(&parsedFormula, 2, "+"); // Was & first, testing...
				break;
			case 0x09:  // ptgLT
				concatValues(&parsedFormula, 2, "<");
				break;
			case 0x0a:  // ptgLE
				concatValues(&parsedFormula, 2, "<=");
				break;
			case 0x0b:  // ptgEQ
				concatValues(&parsedFormula, 2, "==");
				break;
			case 0x0c:  // ptgGE
				concatValues(&parsedFormula, 2, ">=");
				break;
			case 0x0d:  // ptgGT
				concatValues(&parsedFormula, 2, ">");
				break;
			case 0x0e:  // ptgNE
				concatValues(&parsedFormula, 2, "!=");
				break;
			case 0x10:  // ptgUnion
				concatValues(&parsedFormula, 2, ";");
				break;
			case 0x11:  // ptgRange
				concatValues(&parsedFormula, 2, ":");
				break;
			case 0x12:  // ptgUPlus
				parsedFormula.last().prepend("+");
				break;
			case 0x13:  // ptgUMinus
				parsedFormula.last().prepend("-");
				break;
			case 0x14:  // ptgPercent
				parsedFormula.last().append("%");
				break;
			case 0x15:  // ptgParen
				parsedFormula.last() = QString("(%1)").arg(parsedFormula.last());
				break;
			case 0x16:  // ptgMissArg
				// parsedFormula.last().prepend("");
				break;
			case 0x17:  // ptgStr
				char *buffer8bit;
				Q_UINT16 cch;
				rgce >> cch;

				buffer8bit = new char[cch + 1];

				rgce.readRawBytes(buffer8bit, cch);
				buffer8bit[cch] = '\0';

				parsedFormula.append(QString("\"%1\"").arg(buffer8bit));

				delete []buffer8bit;
				break;
			case 0x18:  // ptgExtended
				rgce >> byte;
				parsedFormula.append(QString("Func_18(%1)").arg(byte));
				break;
			case 0x19:  // ptgAttr
				rgce >> byte >> integer;
				if(byte & 0x04)
				{
					integer++;
					integer *= 2;
					while(integer--)
						rgce >> byte;
				}
				else if(byte & 0x10)
					concatValues(&parsedFormula, 1, "", "sum(" ,")");
				break;
			case 0x1c:  // ptgErr
				rgce >> byte;
				parsedFormula.append(Excel_ErrorString(byte));
				break;
			case 0x1d:  // ptgBool
				rgce >> byte;
				parsedFormula.append(byte ? "true" : "false");
				break;
			case 0x1e:  // ptgInt
				rgce >> integer;
				parsedFormula.append(m_locale.formatNumber((long int)integer));
				break;
			case 0x1f:  // ptgNum
				rgce >> number;
				if(((int) number) == number)
					parsedFormula.append(QString().setNum((int) number));
				else
					parsedFormula.append(m_locale.formatNumber(number, 5));
				break;
			case 0x20:  // ptgArray
				parsedFormula.append("ConstArray");
				break;
			case 0x22:  // ptgFuncVar
				rgce >> byte;
				// fall through....
			case 0x21:  // ptgFunc
				Q_UINT16 index;
				rgce >> index;
				index &= 0x7FFF;

				if( ( index == 255 ) && ( ptg == 0x22 ) )
				{
					// external function with variable number of arg
					count = byte & 0x7f;
					if( parsedFormula.count() >= count )
					{
						newop = parsedFormula[ parsedFormula.count() - count];
						concatValues(&parsedFormula, count-1, ";", newop + "(", ")");
						// now remove function name still in the list
						newop = parsedFormula.last();
						parsedFormula.pop_back();
						parsedFormula.pop_back();
						parsedFormula.append( newop );
					}
				}

				else
				{

					excelFunc = ExcelFunction(index);

					if(excelFunc)
					{
						newop = QString(excelFunc->name) + "(";
						count = excelFunc->params;
					}
					else
					{
						newop = QString("ExcelFunc_%1(").arg(index);
						kdWarning(30511) << "Formula contains unhandled function "  << index << endl;
						count = 1;
					}

				if(ptg == 0x22) // variable count of arguments
					count = byte & 0x7F;
				concatValues(&parsedFormula, count, ";", newop, ")");
				}

				break;

			case 0x23:  // ptg????
				rgce >> integer;
				str = QString("NAME(%1)").arg(integer);
				rgce >> integer; // ignore
				parsedFormula.append(str);
				break;
			case 0x24:  // ptgRef
			case 0x2a:  // ptgRef (deleted)
			case 0x2c:
				rgce >> refRow;

				if(biff == BIFF_8)
					rgce >> refColumn;
				else
				{
					rgce >> byte;
					refColumn = byte;
				}

				getReference(row, column, refRow, refColumn, biff, shared, rowSign1, colSign1);

				str = QString("%1%2%3%4#").arg(colSign1).arg(refColumn).arg(rowSign1).arg(refRow);
				parsedFormula.append(str);
				break;
			case 0x25:  // ptgArea
			case 0x2b:
				rgce >> refRow >> refRowLast;

				if(biff == BIFF_8)
					rgce >> refColumn >> refColumnLast;
				else
				{
					rgce >> byte;
					refColumn = byte;
					rgce >> byte;
					refColumnLast = byte;
				}

				getReference(row, column, refRow, refColumn, biff, shared, rowSign1, colSign1);
				getReference(row, column, refRowLast, refColumnLast, biff, shared, rowSign2, colSign2);

				str = QString("%1%2%3%4#:%5%6%7%8#").arg(colSign1).arg(refColumn).arg(rowSign1).arg(refRow)
						.arg(colSign2).arg(refColumnLast).arg(rowSign2).arg(refRowLast);
				parsedFormula.append(str);
				break;
			case 0x26:  // ptgMemArea
				rgce >> integer >> integer >> integer;
				parsedFormula.append("MemArea");
				break;
			case 0x27:  // ptgMemErr
			case 0x28:  // ptgMemNoMem
				rgce >> integer >> integer >> integer;
				break;
			case 0x29:  // ptgNoMem
			case 0x2e:  // ptgAreaN
			case 0x2f:  // ptgNoMemN
				rgce >> integer;
				break;
			case 0x39:  // ptgExtName
				Q_UINT16 extno, intno;
				rgce >> extno >> intno;
				rgce >> integer;

				if( intno <= m_names.count() )
					str = m_names[intno-1];
				else
					str =  QString("extname(EXT(%1),%2)").arg(extno).arg(intno);
				parsedFormula.append(str);
				break;
			case 0x3a:  // ptgRef3d
			case 0x3c:  // ptgRef3dErr
				if(biff == BIFF_8)
				{
					Q_UINT16 sheetNumber;
					rgce >> sheetNumber >> refRow >> refColumn;

					getReference(row, column, refRow, refColumn, biff, shared, rowSign1, colSign1);

					QDomElement *sheet = m_tables->at(sheetNumber);

					if (sheet)
						str = sheet->attribute("name");
					else
						str = "Unknown_Sheet";

					str = QString("%1!%2%3%4%5#").arg(str).arg(colSign1).arg(refColumn).arg(rowSign1).arg(refRow);
					parsedFormula.append(str);
				}
				else
				{
					kdDebug() << "WARNING: External sheet references not done for Excel 95!" << endl;
					return NA("Excel95_sheet");
				}
				break;
			case 0x3b:  // AreaRef3d
			case 0x3d:  // AreaRef3dErr
				if(biff == BIFF_8)
				{
					Q_UINT16 sheetNumber;
					rgce >> sheetNumber;
				        rgce >> refRow >> refRowLast;
					rgce >> refColumn >> refRowLast;

					QDomElement *sheet = m_tables->at(sheetNumber);
					if (sheet)
						str = sheet->attribute("name");
					else
						str = "Unknown_Sheet";

					getReference(row, column, refRow, refColumn, biff, shared, rowSign1, colSign1);
					getReference(row, column, refRowLast, refColumnLast, biff, shared, rowSign2, colSign2);

					str = QString("%1!%2%3%4%5#:%6%7%8%9#").arg(str).arg(colSign1).arg(refColumn)
							.arg(rowSign1).arg(refRow).arg(colSign2).arg(refColumnLast)
							.arg(rowSign2).arg(refRowLast);
					parsedFormula.append(str);
				}
				else
				{
					kdDebug() << "WARNING: External sheet references not done for Excel 95!" << endl;
					return NA("Excel95_sheet");
				}
				break;
			default:
				kdWarning(30511) << "Formula contains unhandled ptg 0x" << QString::number(ptg, 16) << endl;
				return NA(QString("unhandled_ptg_0x%1)").arg(QString::number(ptg,16)));
				break;
		}
	}

	// kdWarning(30511) << "Leaving " << __FUNCTION__ << endl;

	return parsedFormula.join("");
}

void Helper::addName( const QString& s )
{
	m_names.append( s );
}

