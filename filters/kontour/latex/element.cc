/*
** A program to convert the XML rendered by KWord into LATEX.
**
** Copyright (C) 2000 Robert JACOLIN
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** To receive a copy of the GNU Library General Public License, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
**
*/

#include <kdebug.h>
#include "header.h"
#include "element.h"

/*******************************************/
/* Constructor                             */
/*******************************************/
Element::Element()
{
	setFillStyle(FS_SOLID);
	setLineWidth(1);
	setStrokeColor("#000000");
	setStrokeStyle(1);
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Element::~Element()
{
	kdDebug() << "Element Destructor" << endl;
}

/*******************************************/
/* Analyse                                 */
/*******************************************/
void Element::analyse(const QDomNode balise)
{
	//init(balise);
	//setMattirx(getAttr(balise, "matrix"));
	//setStrokeColor(getAttr(balise, "strockecolor").toInt());
	//setStrokeStyle(getAttr(balise, "strockestyle").toInt());
	//setLineWidth(getAttr(balise, "linewidth").toInt());
	//setRightMargin(getAttr(balise, "fillcolor").toInt());
	//setFillStyle(getAttr(balise, "fillstyle").toInt());
	//setFillPatern(getAttr(balise, "fillpatern").toInt());
	//setGradStyle(getAttr(balise, "gradstyle").toInt());
	//setGraColor1(getAttr(balise, "gradcolor1").toInt());
	//setGradColor2(getAttr(balise, "gradcolor2").toInt());
	//setId(getAttr(balise, "id").toInt());
	//setRef(getAttr(balise, "ref").toInt());

	//kdDebug() << "END OF ANALYSE OF AN ELEMENT" << endl;
}

void Element::analyseGObject(const QDomNode balise)
{
	kdDebug() << "Analyse gobject" << endl;
	/* Bordure */
	setLineWidth(getAttr(balise, "linewidth").toDouble());

	setStrokeColor(getAttr(balise, "strokecolor"));
	_strokecolorname = _fileHeader->addNewColor(getStrokeColor());
	setStrokeStyle(getAttr(balise, "strokestyle").toInt());

	/* Remplissage */
	setFillColor(getAttr(balise, "fillcolor"));
	_fillcolorname = _fileHeader->addNewColor(getFillColor());
	setFillStyle(getAttr(balise, "fillstyle").toInt());
	setFillPattern(getAttr(balise, "fillpattern").toInt());
	setGradientColor1(getAttr(balise, "gradcolor1"));
	_gradientcolorname1 = _fileHeader->addNewColor(getGradientColor1());
	setGradientColor2(getAttr(balise, "gradcolor2"));
	_gradientcolorname2 = _fileHeader->addNewColor(getGradientColor2());
	if(getFillStyle() == FS_GRADIENT)
		_fileHeader->useGradient();
	analyseMatrix(getChild(balise, "matrix"));
}

void Element::analyseMatrix(const QDomNode balise)
{
	double dx, dy, m11, m12, m21, m22;
	kdDebug() << "Analyse a matrix" << endl;
	dx  = getAttr(balise, "dx").toDouble();
	dy  = getAttr(balise, "dy").toDouble();
	m11 = getAttr(balise, "m11").toDouble();
	m12 = getAttr(balise, "m12").toDouble();
	m21 = getAttr(balise, "m21").toDouble();
	m22 = getAttr(balise, "m22").toDouble();
	kdDebug() << "dx = " << dx << " dy = " << dy << endl;
	kdDebug() << "m11 = " << m11 << " m12 = " << m12 << endl;
	_matrix.setMatrix(m11, m12, m21, m22, dx, dy);
}

/*******************************************/
/* getBaseContentAttr                      */
/*******************************************/
QString Element::getBaseContentAttr() const
{
	QString out;
	if (getLineWidth() > 1)
		out = QString("linewidth=") + QString::number(getLineWidth());
	switch(getStrokeStyle())
	{
		case 1: /* defaut */
			break;
		case 2: concat(out, "linestyle=dashed, dash=12pt 6pt");
			break;
		case 3: concat(out, "linestyle=dashed, dash=3pt 3pt");
			break;
		case 4:
			break;
		case 5:
			break;
	}

	if(getStrokeStyle() >= FS_SOLID && getStrokeColor() != "#000000")
	{
		switch(getStrokeStyle())
		{
			case FS_SOLID: concat(out, QString("linestyle=solid"));
				break;
		}
		concat(out, QString("linecolor="+_strokecolorname));
	}

	if(getFillStyle() >= FS_SOLID)
	{
		switch(getFillStyle())
		{
			case FS_SOLID: concat(out, QString("fillstyle=solid"));
					concat(out, QString("fillcolor=" + _fillcolorname));
				break;
			case FS_PATTERN : generateFillPattern(out);
				break;
			case FS_UNKNOWN:
				break;
			case FS_GRADIENT:
					concat(out, QString("fillstyle=gradient"));
					concat(out, QString("gradbegin=" + _gradientcolorname2));
					concat(out, QString("gradend=" + _gradientcolorname1));
				break;
		}
	}
	return out;
}

void Element::generateFillPattern(QString& out) const
{
	switch(getFillPattern())
	{
		case 1:
			concat(out, QString("fillstyle=solid"));
			concat(out, QString("fillcolor=" + _fillcolorname));
			break;
		case 2:
			concat(out, QString("fillstyle=solid"));
			concat(out, QString("fillcolor=" + _fillcolorname));
			break;
		case 3:
			concat(out, QString("fillstyle=solid"));
			concat(out, QString("fillcolor=" + _fillcolorname));
			break;
		case 4:
			concat(out, QString("fillstyle=crosshatch*"));
			concat(out, QString("fillcolor=" + _fillcolorname));
			break;
		case 5:
			concat(out, QString("fillstyle=crosshatch, hatchsep=1pt"));
			concat(out, QString("hatchcolor=" + _fillcolorname));
			break;
		case 6:
			concat(out, QString("fillstyle=crosshatch, hatchsep=2pt"));
			concat(out, QString("hatchcolor=" + _fillcolorname));
			break;
		case 7:
			concat(out, QString("fillstyle=crosshatch, styleline=dotted"));
			concat(out, QString("hatchcolor=" + _fillcolorname));
			break;
		case 8:
			concat(out, QString("fillstyle=hlines, fillstyle=vlines,hatchstyle=dotted"));
			concat(out, QString("hatchcolor=" + _fillcolorname));
			break;
		case 9:
			concat(out, QString("fillstyle=hlines, hatchangle=0"));
			concat(out, QString("hatchcolor=" + _fillcolorname));
			break;
		case 10:
			concat(out, QString("fillstyle=vlines, hatchangle=0"));
			concat(out, QString("hatchcolor=" + _fillcolorname));
			break;
		case 11:
			concat(out, QString("fillstyle=vlines, hatchangle=0,fillstyle=hlines"));
			concat(out, QString("hatchcolor=" + _fillcolorname));
			break;
		case 12:
			concat(out, QString("fillstyle=vlines,hatchangle=-45"));
			concat(out, QString("hatchcolor=" + _fillcolorname));
			break;
		case 13:
			concat(out, QString("fillstyle=vlines,hatchangle=45"));
			concat(out, QString("hatchcolor=" + _fillcolorname));
			break;
		case 14:
			concat(out, QString("fillstyle=crosshatch"));
			concat(out, QString("hatchcolor=" + _fillcolorname));
			break;

	}
}


void Element::concat(QString& param, QString newParam) const
{
	if(!newParam.isEmpty())
	{
		if(!param.isEmpty())
			param.append(", ");
		param += newParam;
	}
}

void Element::concat(QString& param, float newParam)
{
	if(!param.isEmpty())
		param.append(", ");
	param += QString::number(newParam);
}


void Element::generateList(QTextStream & out, const QString first, const QString body, const QString end)
{
	if(!body.isEmpty())
	{
		out << first << body << end;
	}
}


