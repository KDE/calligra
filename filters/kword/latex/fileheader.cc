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
		else if(strcmp(arg->zName, "PTWIDTH")== 0)
		{
			_width[TU_PT] = atoi(arg->zValue);
		}
		else if(strcmp(arg->zName, "PTHEIGHT")== 0)
		{
			_height[TU_PT] = atoi(arg->zValue);
		}
		else if(strcmp(arg->zName, "MMWIDTH")== 0)
		{
			_width[TU_MM] = atoi(arg->zValue);
			_width[TU_CM] = _width[TU_MM] / 10;
		}
		else if(strcmp(arg->zName, "MMHEIGHT")== 0)
		{
			_height[TU_MM] = atoi(arg->zValue);
			_height[TU_CM] = _height[TU_MM] / 10;
		}
		else if(strcmp(arg->zName, "INCHWIDTH")== 0)
		{
			_width[TU_INCH] = atoi(arg->zValue);
		}
		else if(strcmp(arg->zName, "INCHHEIGHT")== 0)
		{
			_height[TU_INCH] = atoi(arg->zValue);
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
		else if(strcmp(arg->zName, "PTCOLUMNSPC")== 0)
		{
			_columnSpc[TU_PT] = atoi(arg->zValue);
		}
		else if(strcmp(arg->zName, "MMCOLUMNSPC")== 0)
		{
			_columnSpc[TU_MM] = atoi(arg->zValue);
			_columnSpc[TU_CM] = _columnSpc[TU_MM] / 10; 
		}
		else if(strcmp(arg->zName, "INCHCOLUMNSPC")== 0)
		{
			_columnSpc[TU_INCH] = atoi(arg->zValue);
		}
		else if(strcmp(arg->zName, "HTYPE")== 0)
		{
			setHeadType(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "FTYPE")== 0)
		{
			setFootType(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "PTHEADBODY")== 0)
		{
			_headBody[TU_PT] = atoi(arg->zValue);
		}
		else if(strcmp(arg->zName, "MMHEADBODY")== 0)
		{
			_headBody[TU_MM] = atoi(arg->zValue);
			_headBody[TU_CM] = _headBody[TU_MM] / 10;
		}
		else if(strcmp(arg->zName, "INCHHEADBODY")== 0)
		{
			_headBody[TU_INCH] = atoi(arg->zValue);
		}
		else if(strcmp(arg->zName, "PTFOOTBODY")== 0)
		{
			_footBody[TU_PT] = atoi(arg->zValue);
		}
		else if(strcmp(arg->zName, "MMFOOTBODY")== 0)
		{
			_footBody[TU_MM] = atoi(arg->zValue);
			_footBody[TU_CM] = _footBody[TU_MM] / 10; 
		}
		else if(strcmp(arg->zName, "INCHFOOTBODY")== 0)
		{
			_footBody[TU_INCH] = atoi(arg->zValue);
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
				if(strcmp(arg->zName, "MMLEFT")== 0)
				{
					_leftBorder[TU_MM] = atoi(arg->zValue);
					_leftBorder[TU_CM] = _leftBorder[TU_MM] / 10; 
				}
				else if(strcmp(arg->zName, "MMRIGHT")== 0)
				{
					_rightBorder[TU_MM] = atoi(arg->zValue);
					_rightBorder[TU_CM] = _rightBorder[TU_MM] / 10; 
				}
				else if(strcmp(arg->zName, "MMBOTTOM")== 0)
				{
					_bottomBorder[TU_MM] = atoi(arg->zValue);
					_bottomBorder[TU_CM] = _bottomBorder[TU_MM] / 10;
				}
				else if(strcmp(arg->zName, "MMTOP")== 0)
				{
					_topBorder[TU_MM] = atoi(arg->zValue);
					_topBorder[TU_CM] = _topBorder[TU_MM] / 10;
				}
				else if(strcmp(arg->zName, "PTRIGHT")== 0)
				{
					_rightBorder[TU_PT] = atoi(arg->zValue);
				}
				else if(strcmp(arg->zName, "PTLEFT")== 0)
				{
					_leftBorder[TU_PT] = atoi(arg->zValue);
				}
				else if(strcmp(arg->zName, "PTBOTTOM")== 0)
				{
					_bottomBorder[TU_PT] = atoi(arg->zValue);
				}
				else if(strcmp(arg->zName, "PTTOP")== 0)
				{
					_topBorder[TU_PT] = atoi(arg->zValue);
				}
				else if(strcmp(arg->zName, "INCHRIGHT")== 0)
				{
					_rightBorder[TU_INCH] = atoi(arg->zValue);
				}
				else if(strcmp(arg->zName, "INCHLEFT")== 0)
				{
					_leftBorder[TU_INCH] = atoi(arg->zValue);
				}
				else if(strcmp(arg->zName, "INCHBOTTOM")== 0)
				{
					_bottomBorder[TU_INCH] = atoi(arg->zValue);
				}
				else if(strcmp(arg->zName, "INCHTOP")== 0)
				{
					_topBorder[TU_INCH] = atoi(arg->zValue);
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
		if(strcmp(arg->zName, "UNIT")== 0)
		{
			setUnit(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "HASHEADER")== 0)
		{
			_hasHeader = atoi(arg->zValue);
		}
		else if(strcmp(arg->zName, "HASFOOTER")== 0)
		{
			_hasFooter = atoi(arg->zValue);
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
	switch(getUnit())
	{
		case TU_MM: unit = "mm";
			break;
		case TU_CM: unit = "cm";
			break;
		case TU_PT: unit = "pt";
			break;
		case TU_INCH: unit = "inch";
			break;
	}
	kdDebug() << "Generate custom size paper" << endl;
	/* paper size */
	out << "\\setlength{\\paperwidth}{"  << _width[getUnit()]  << unit << "}" << endl;
	out << "\\setlength{\\paperheight}{" << _height[getUnit()] << unit << "}" << endl;
	/* FileHeader and footer */
	out << "\\setlength{\\headsep}{" << _headBody[getUnit()] << unit << "}" << endl;
	out << "\\setlength{\\footskip}{" << _footBody[getUnit()] + _bottomBorder[getUnit()] << unit << "}" << endl;
	/* Margin */
	out << "\\setlength{\\topmargin}{" << _topBorder[getUnit()] << unit << "}" << endl;
	out << "\\setlength{\\textwidth}{" << _width[getUnit()] - _rightBorder[getUnit()] - _leftBorder[getUnit()] << unit << "}" << endl;
	out << endl;
}

/*******************************************/
/* GeneratePreambule                       */
/*******************************************/
void FileHeader::generatePreambule(QTextStream &out)
{
	out << "%% Generated by Kword. Don't modify this file but the file *.kwd." << endl;
	out << "%% Send an email to rjaolin@ifrance.com for bugs, whishes, .... Thanks." << endl;
	out << "%% To see it, compile this file with : latex filename.tex" << endl;
	out << "%% a dvi file will be generated." << endl;
	out << "%% The file use the latex style (not the kword style). " << endl;
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
	out << endl;
			
}
