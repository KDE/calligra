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

/*******************************************/
/* Constructor                             */
/*******************************************/
Header::Header()
{
	_hasColor     = false;
	_hasUnderline = false;
	_hasEnumerate = false;
	_hasGradient  = false;
	setDx(20);
	setDy(20);
	setGridAlign(0);
	setHelpAlign(0);
	//setUnit(TU_PT);
	setFileHeader(this);		/* for xmlParser class. */
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Header::~Header()
{
	kdDebug() << "Header Destructor" << endl;
}

/*******************************************/
/* Analyse                                 */
/*******************************************/
void Header::analyse(const QDomNode balise)
{
	kdDebug() << "BEGIN THE ANALYSE OF AN HEAD" << endl;
	//init(balise);
	//analyseLayoutParam(getChild(balise, "layout"));
	analyseGrid(getChild(balise, "grid"));
	kdDebug() << "END OF ANALYSE OF AN HEAD" << endl;
}

/*******************************************/
/* AnalyseLayout                           */
/*******************************************/
/*void Header::analyseLayoutParam(const QDomNode balise)
{
	* Get parameters *
	setWidth(getAttr(balise, "width").toInt());
	setHeight(getAttr(balise, "height").toInt());
	setOrientation(getAttr(balise, "orientation").toInt());
	setLeftMargin(getAttr(balise, "lmargin").toInt());
	setRightMargin(getAttr(balise, "rmargin").toInt());
	setBottomMargin(getAttr(balise, "bmargin").toInt());
	setTopMargin(getAttr(balise, "tmargin").toInt());
	setFormat(getAttr(balise, "format").toInt());
	//setUnit(getAttr(balise, "unit").toInt();
}*/

/*QString Header::getColor(QString name)
{
	QColor col;

	// Chercher une couleur
	for(col = _colors.first(); col != 0 && col.name() != color.name(); col = _colors.next())
	{ }
	if(col == 0)
	{
		return QString();
	}
	else
	{
		return col.name();
	}
}*/

QString Header::addNewColor(QString color)
{
	QColor* col = 0;
	static int num = 0;

	if(color!= 0)
	{
		/* Chercher une couleur */
		for(col = _colors.first(); col != 0 && col->name() != color; col = _colors.next())
		{
			kdDebug() << col->name() << "!= " << color << endl;
		}
		if (col == 0)
		{
			col = new QColor(color);
			QString* name = new QString("color");
			num = num + 1;
			*name = *name + QString::number(num);
			kdDebug() << "New color : " << *name << endl;
			/* Ajouter la couleur */
			_colors.append(col);
			_colorname.append(name);
			return *name;
		}
		else
			return *_colorname.at(_colors.at());
	}
}

/*******************************************/
/* AnalyseGrid                             */
/*******************************************/
void Header::analyseGrid(const QDomNode balise)
{
	kdDebug() << "BEGIN THE ANALYSE OF A GRID" << endl;
	analyseGridParam(balise);
	//init(getChild(balise, "helplines"));	
	setHelpAlign(getAttr(balise, "align").toInt());
	kdDebug() << "END OF ANALYSE OF A GRID" << endl;
}

/*******************************************/
/* AnalyseGridParam                        */
/*******************************************/
void Header::analyseGridParam(const QDomNode balise)
{
	setDx(getAttr(balise, "dx").toInt());
	setDy(getAttr(balise, "dy").toInt());
	setGridAlign(getAttr(balise, "align").toInt());
}

/*******************************************/
/* Generate                                */
/*******************************************/
void Header::generate(QTextStream &out)
{
	kdDebug() << "GENERATION OF THE FILE HEADER" << endl;
	generatePreambule(out);
	generatePackage(out);
	//if(getFormat() == TF_CUSTOM)
		generatePaper(out);
	kdDebug() << "color :" << endl;
	for(QColor* col = _colors.first(); col != 0; col = _colors.next())
	{
		out << "\\newrgbcolor{" << *_colorname.at(_colors.at()) << "}{";
		out << ((double) col->red() / 255) << " ";
		out << ((double) col->green() / 255) << " ";
		out << ((double) col->blue() / 255) << "}" << endl;
		kdDebug() << col->red() << "," << col->green() << "," << col->blue() << endl;
	}
	out << "%%%%%%%%%%%%%%%%%% END OF PREAMBULE %%%%%%%%%%%%%%%%%%" <<endl;
}

/*******************************************/
/* GeneratePaper                           */
/*******************************************/
void Header::generatePaper(QTextStream &out)
{
	QString unit;

	out << "% Format of paper" << endl;
	kdDebug() << "Generate custom size paper" << endl;
	out << "\\setlength{\\hoffset}{-1.2in}" << endl;
	out << "\\setlength{\\voffset}{-1in}" << endl;
	out << "\\setlength{\\headheight}{0mm}" << endl;
	out << "\\setlength{\\headsep}{0mm}" << endl;
	out << "\\setlength{\\oddsidemargin}{0mm}" << endl;

	/* paper size */
	/*if(getFormat() == TF_CUSTOM)
	{
		out << "\\setlength{\\paperwidth}{"  << getWidth()  << "mm}" << endl;
		out << "\\setlength{\\paperheight}{" << getHeight() << "mm}" << endl;
	}

	out << "\\setlength{\\textwidth}{"  << (getWidth() - getLeftMargin() - getRightMargin())  << "mm}" << endl;
	out << "\\setlength{\\textheight}{" << (getHeight()) << "mm}" << endl;
*/
	/* Margin */
/*	out << "\\setlength{\\topmargin}{" << getTopMargin() << "mm}" << endl;
	out << "\\addtolength{\\leftmargin}{" << getLeftMargin() << "mm}" << endl;
	out << endl;*/
	out << "\\pagestyle{empty}" << endl;
	out << "\\psset{unit=1pt}" << endl << endl;
}

/*******************************************/
/* GeneratePreambule                       */
/*******************************************/
void Header::generatePreambule(QTextStream &out)
{
	out << "%% Generated by Killu. Don't modify this file but the file *.kill." << endl;
	out << "%% Send an email to rjaolin@ifrance.com for bugs, whishes, .... Thanks you." << endl;
	out << "%% Compile this file with : latex filename.tex" << endl;
	out << "%% a dvi file will be generated." << endl;
	out << "\\documentclass[";
	/*switch(getFormat())
	{
		case TF_A3:
			out << "";
			break;
		case TF_A4:
			out << "a4paper, ";
			break;
		case TF_A5:
			out << "a5paper, ";
			break;
		case TF_USLETTER:
			out << "letterpaper, ";
			break;
		case TF_USLEGAL:
			out << "legalpaper, ";
			break;
		case TF_SCREEN:
			out << "";
			break;
		case TF_CUSTOM:
			out << "";
			break;
		case TF_B3:
			out << "";
			break;
		case TF_USEXECUTIVE:
			out << "executivepaper, ";
			break;
	}
	if(getOrientation() == TO_LANDSCAPE)
		out << "landscape, ";*/
	/* The font and the type of the doc. can not be changed, hmm ? */
	out << "11pt]{article}" << endl;
}

/*******************************************/
/* GeneratePackage                         */
/*******************************************/
void Header::generatePackage(QTextStream &out)
{
	out << "% Package(s) to include" << endl;
	out << "\\usepackage{pstricks}" << endl;
	if(_hasGradient)
		out << "\\usepackage{pst-grad}" << endl;
	if(_colors.count() > 0)
		out << "\\usepackage{colortbl}" << endl;
	out << endl;
			
}
