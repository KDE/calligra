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

void Helper::getDate(int date, int &year, int &month, int &day, Q_UINT16 date1904)
{
	year = 0;

	while(date > (((year % 4) == 0) ? 366 : 365))
		date -= ((year++ % 4) == 0) ? 366 : 365;

	if(date1904 == 1)
		year += 4;

	if((year % 4) == 0)
	{
		for(month = 0; month < 12; ++month)
		{
			if(date <= ldays[month])
				break;

			date -= ldays[month];
		}
	}
	else
	{
		for(month = 0; month < 12; ++month)
		{
			if(date <= ndays[month])
				break;

			date -= ndays[month];
		}
	}

	++month;

	day = (date == 0) ? 1 : date;
	year += 1900;
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

	switch(xwork->ifmt)
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
			if(!m_formatrec[xwork->ifmt])
			{
				kdError() << "Formatting IFMT 0x" << QString::number(xwork->ifmt, 16) << " NOT implemented!" << endl;
				break;
			}
			
			s = QString::fromLatin1(m_formatrec[xwork->ifmt]->rgch, m_formatrec[xwork->ifmt]->cch);
	}

	getFont(xf, format, fontid);
	getPen(xf, format, fontid);

	return format;
}

void getReference(Q_UINT16 row, Q_UINT16 column, Q_INT16 &refRow, Q_INT16 &refColumn, Q_UINT16 biff, bool shared)
{
	if(biff == BIFF_8)
	{
		if((refColumn & 0x8000) && !shared)
			refRow -= row;

		if(refColumn & 0x4000)
		{
			if(!shared)
			{
				refColumn &= 0x3fff;
				refColumn -= column;
			}
			else
				refColumn = (Q_INT8) refColumn;
		}
	}
	else
	{
		// TODO: Test this part!
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
}

const QString Helper::getFormula(Q_UINT16 row, Q_UINT16 column, QDataStream &rgce, Q_UINT16 biff, bool shared)
{
	double number;
	Q_UINT8 byte, ptg;
	Q_UINT16 integer;
	Q_INT16 refRow, refColumn, refRowLast, refColumnLast;
	QString str;
	QStringList parsedFormula;
	QStringList::Iterator stringPtr;
	bool found;

	parsedFormula.append("=");

	while (!rgce.atEnd())
	{
		rgce >> ptg;
		
		// Correct ptg parsing!
		ptg = ((ptg & 0x40) ? (ptg | 0x20) : ptg) & 0x3F;
		
		switch (ptg)
		{
			case 0x01:  // ptgExpr
				Q_UINT16 tlrow, tlcol;
				rgce >> tlrow >> tlcol;

				kdDebug(30511) << "WARNING: ptgExpr requested in Row " << row << " Col " << column << " !" << endl;
				kdDebug(30511) << "HASH Table: top-Left-Row " << tlrow << ", top-Left-Col " << tlcol << endl;

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

				parsedFormula.append("");
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
				stringPtr = parsedFormula.fromLast();
				--stringPtr;
				--stringPtr;
				(*stringPtr).append("+");
				break;
			case 0x13:  // ptgUMinus
				stringPtr = parsedFormula.fromLast();
				--stringPtr;
				--stringPtr;
				(*stringPtr).append("-");
				break;
			case 0x14:  // ptgPercent
				kdDebug(30511) << "WARNING: ptgPercent formula not supported, yet" << endl;
				return "N/A"; // Return _error_ formula-string
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
				kdDebug(30511) << "WARNING: ptgMissArg formula not supported, yet" << endl;
				return "N/A"; // Return _error_ formula-string
				break;
			case 0x17:  // ptgStr
				char *buffer8bit;
				QString *s;
				
				Q_UINT16 cch;				
				rgce >> cch;
				
				buffer8bit = new char[cch + 1];
				rgce.readRawBytes(buffer8bit, cch);

				buffer8bit[cch] = '\0';
				s = new QString(buffer8bit);

				delete []buffer8bit;

				parsedFormula.append("\"" + *s + "\"");
				parsedFormula.append("");
				break;
			case 0x18:  // ptgExtended
				kdDebug(30511) << "WARNING: ptgExtended formula not supported, yet" << endl;
				return "N/A"; // Return _error_ formula-string
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
				kdDebug(30511) << "WARNING: ptgErr formula not supported, yet" << endl;
				return "N/A"; // Return _error_ formula-string
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
					case 142:  // SLN
						--stringPtr;
						*stringPtr = ";";
						--stringPtr;
						--stringPtr;
						*stringPtr = ";";
						--stringPtr;
						(*stringPtr).prepend("SLN(");
						break;
					case 143:  // SYD
						--stringPtr;
						*stringPtr = ";";
						--stringPtr;
						--stringPtr;
						*stringPtr = ";";
						--stringPtr;
						--stringPtr;
						*stringPtr = ";";
						--stringPtr;
						(*stringPtr).prepend("SYD(");
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
						(*stringPtr).prepend("not_handled_yet(");
						kdDebug(30511) << "Formula contains unhandled function " << integer << endl;
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
					case 1: // if
						--stringPtr;
						--stringPtr;
						(*stringPtr).prepend("IF(");
						break;
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
					case 247:  // DB
						(*stringPtr).prepend("DB(");
						break;
					default:
						(*stringPtr).prepend("not_handled_yet(");
						kdDebug(30511) << "Formula contains unhandled function " << integer << endl;
						break;
				}
				break;
			case 0x24:  // ptgRef
			case 0x2c:
				rgce >> refRow;
				
				if(biff == BIFF_8)
					rgce >> refColumn;
				else
				{
					rgce >> byte;
					refColumn = byte;
				}
				
				getReference(row, column, refRow, refColumn, biff, shared);

				str = "#";
				str += QString::number((int) refColumn);
				str += "#";
				str += QString::number((int) refRow);
				str += "#";
				parsedFormula.append(str);
				parsedFormula.append("");
				break;
			case 0x25:  // ptgArea
			case 0x2d:
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

				getReference(row, column, refRow, refColumn, biff, shared);
				getReference(row, column, refRowLast, refColumnLast, biff, shared);

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
			case 0x3a: // ptgRef3d
				if(biff == BIFF_8)
				{
					Q_UINT16 sheetNumber;
					rgce >> sheetNumber >> refRow >> refColumn;
	
					getReference(row, column, refRow, refColumn, biff, shared);
					
					QDomElement *sheet = m_tables->at(sheetNumber);

					if(!sheet)
						return "N/A";

					str = sheet->attribute("name") + "!#";
					str += QString::number((int) refColumn);
					str += "#";
					str += QString::number((int) refRow);
					str += "#";
					parsedFormula.append(str);
					parsedFormula.append("");
				}
				else
				{
					kdDebug() << "WARNING: External sheet references not done for Excel 95!" << endl;
					return "N/A";
				}				
				break;			
			default:
				kdDebug(30511) << "Formula contains unhandled ptg " << ptg << endl;
				return "N/A"; // Return _error_ formula-string
				break;
		}
	}
	kdDebug(30511) << "Helper::getFormula: " << parsedFormula.join("") << endl;
	return parsedFormula.join("");
}
