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

#include <stdlib.h>

#include <kdebug.h>

#include "fileheader.h"

/*******************************************/
/* Constructor                             */
/*******************************************/
FileHeader::FileHeader()
{
	_hasHeader    = false;
	_hasFooter    = false;
	_hasColor     = false;
	_hasUnderline = false;
	_hasEnumerate = false;
	_standardPage = 0;
	_processing   = TP_NORMAL;
	setFileHeader(this);		/* for xmlParser class. */
}

/*******************************************/
/* Destructor                              */
/*******************************************/
FileHeader::~FileHeader()
{
	kdDebug() << "FileHeader Destructor" << endl;
}

/*******************************************/
/* AnalysePaperParam                       */
/*******************************************/
void FileHeader::analysePaperParam(const Markup* balise_initiale)
{
	Arg* arg = 0;

	// Get parameters
	for(arg= balise_initiale->pArg; arg; arg= arg->pNext)
	{
		if(strcmp(arg->zName, "FORMAT")== 0)
		{
			setFormat(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "WIDTH")== 0)
		{
			_width = atoi(arg->zValue);
		}
		else if(strcmp(arg->zName, "HEIGHT")== 0)
		{
			_height = atoi(arg->zValue);
		}
		else if(strcmp(arg->zName, "ORIENTATION")== 0)
		{
			setOrientation(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "COLUMNS")== 0)
		{
			if(atoi(arg->zValue) > 2)
				setColumns(TC_MORE);
			else
				setColumns(atoi(arg->zValue) - 1);
		}
		else if(strcmp(arg->zName, "COLUMNSPACING")== 0)
		{
			_columnSpacing = atoi(arg->zValue);
		}
		else if(strcmp(arg->zName, "HTYPE")== 0)
		{
			setHeadType(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "FTYPE")== 0)
		{
			setFootType(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "SPHEADBODY")== 0)
		{
			_headBody = atoi(arg->zValue);
		}
		else if(strcmp(arg->zName, "SPFOOTBODY")== 0)
		{
			_footBody = atoi(arg->zValue);
		}
		else if(strcmp(arg->zName, "ZOOM") == 0)
		{
			/* UNUSEFULL */
		}
	}
}

/*******************************************/
/* AnalysePaper                            */
/*******************************************/
void FileHeader::analysePaper(const Markup * balise_initiale)
{
	Markup* balise = 0;
	Arg*    arg    = 0;

	analysePaperParam(balise_initiale);

	setTokenCurrent(balise_initiale->pContent);
	// Analyse children markups --> PAPERBORDERS
	while((balise = getNextMarkup()) != 0)
	{
		kdDebug() << balise << endl;
		kdDebug() << balise->token.zText << endl;
		if(strcmp(balise->token.zText, "PAPERBORDERS")== 0)
		{
			for(arg= balise->pArg; arg; arg= arg->pNext)
			{
				if(strcmp(arg->zName, "LEFT")== 0)
				{
					_leftBorder = atoi(arg->zValue);
				}
				else if(strcmp(arg->zName, "RIGHT")== 0)
				{
					_rightBorder = atoi(arg->zValue);
				}
				else if(strcmp(arg->zName, "BOTTOM")== 0)
				{
					_bottomBorder = atoi(arg->zValue);
				}
				else if(strcmp(arg->zName, "TOP")== 0)
				{
					_topBorder = atoi(arg->zValue);
				}
			}

		}
	}
	
}

/*******************************************/
/* AnalyseAttributs                        */
/*******************************************/
void FileHeader::analyseAttributs(const Markup *balise)
{
	Arg* arg = 0;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		if(strcmp(arg->zName, "PROCESSING") == 0)
		{
			setProcessing(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "STANDARDPAGE")== 0)
		{
			setStandardPge(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "HASHEADER")== 0)
		{
			_hasHeader = atoi(arg->zValue);
		}
		else if(strcmp(arg->zName, "HASFOOTER")== 0)
		{
			_hasFooter = atoi(arg->zValue);
		}
		else if(strcmp(arg->zName, "UNIT")== 0)
		{
			setUnit(atoi(arg->zValue));
		}
	}
}

/*******************************************/
/* Generate                                */
/*******************************************/
void FileHeader::generate(QTextStream &out)
{
	kdDebug() << "GENERATION OF THE FILE HEADER" << endl;
	generatePreambule(out);
	generatePackage(out);
	if(getFormat() == TF_CUSTOM)
		generatePaper(out);
	out << "%%%%%%%%%%%%%%%%%% END OF PREAMBULE %%%%%%%%%%%%%%%%%%" <<endl;
}

/*******************************************/
/* GeneratePaper                           */
/*******************************************/
void FileHeader::generatePaper(QTextStream &out)
{
	QString unit;

	out << "% Format of paper" << endl;
	kdDebug() << "Generate custom size paper" << endl;
	/* paper size */
	out << "\\setlength{\\paperwidth}{"  << _width  << "pt}" << endl;
	out << "\\setlength{\\paperheight}{" << _height << "pt}" << endl;
	/* FileHeader and footer */
	out << "\\setlength{\\headsep}{" << _headBody << "pt}" << endl;
	out << "\\setlength{\\footskip}{" << _footBody + _bottomBorder << "pt}" << endl;
	/* Margin */
	out << "\\setlength{\\topmargin}{" << _topBorder << "pt}" << endl;
	out << "\\setlength{\\textwidth}{" << _width - _rightBorder - _leftBorder << "pt}" << endl;
	out << endl;
}

/*******************************************/
/* GeneratePreambule                       */
/*******************************************/
void FileHeader::generatePreambule(QTextStream &out)
{
	out << "%% Generated by Kword. Don't modify this file but the file *.kwd." << endl;
	out << "%% Send an email to rjaolin@ifrance.com for bugs, whishes, .... Thanks you." << endl;
	out << "%% Compile this file with : latex filename.tex" << endl;
	out << "%% a dvi file will be generated." << endl;
	out << "%% The file uses the latex style (not the kword style). " << endl;
	out << "\\documentclass[";
	switch(getFormat())
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
		out << "landscape, ";
	/* To change : will use a special latexcommand to able to
	 * obtain more than one column :))
	 */
	switch(getColumns())
	{
		case TC_1:
			//out << "onecolumn, ";
			break;
		case TC_2:
			out << "twocolumn, ";
			break;
		case TC_MORE:
			out << "";
	}
	/* The font and the type of the doc. can not be changed, hmm ? */
	out << "11pt]{article}" << endl;
}

/*******************************************/
/* GeneratePackage                         */
/*******************************************/
void FileHeader::generatePackage(QTextStream &out)
{
	out << "% Package(s) to include" << endl;
	if(hasFooter() || hasHeader())
		out << "\\usepackage{fancyhdr}" << endl;
	if(hasColor())
		out << "\\usepackage{color}" << endl;
	if(hasUnderline())
		out << "\\usepackage{ulem}" << endl;
	if(hasEnumerate())
		out << "\\usepackage{enumerate}" << endl;
	if(hasGraphics())
		out << "\\usepackage{graphics}" << endl;
	out << endl;
			
}
