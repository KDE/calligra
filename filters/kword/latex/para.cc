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

#include <stdlib.h>		/* for atoi function    */
#include <kdebug.h>		/* for kdDebug() stream */
#include "para.h"
#include "texte.h"		/* father class.        */
#include "format.h"		/* children classes.    */
#include "picturezone.h"
#include "textzone.h"
#include "footnote.h"
#include "anchor.h"

/* static data */
QStack<EType> Para::_historicList;

/*******************************************/
/* Constructor                             */
/*******************************************/
Para::Para(Texte* texte)
{
	_element   = texte;
	_lines     = 0;
	_name      = 0;
	_info      = EP_NONE;	/* the parag is not a footnote */
	//_hardbrk   = EP_FLOW;	/* and it's not a new page */
	_currentPos= 0;		/* At the beginning of the paragraph */
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Para::~Para()
{
	kdDebug() << "Destruction of a parag." << endl;
	if(_lines != 0)
		delete _lines;
}

/*******************************************/
/* GetFrameType                            */
/*******************************************/
/* To know if it's the text or it's a      */
/* header or a footer.                     */
/*******************************************/
SSect Para::getFrameType() const
{
	return _element->getSection();
}

/*******************************************/
/* getTypeFormat                           */
/*******************************************/
/* To know if the zone is a textzone, a    */
/* footnote, a picture, a variable.        */
/*******************************************/
EFormat Para::getTypeFormat(const QDomNode balise) const
{
	//<FORMAT id="1" ...>

	return (EFormat) getAttr(balise, "id").toInt();
}

/*******************************************/
/* getNbCharPara                           */
/*******************************************/
/* To know the size of a paragraph.        */
/*******************************************/
int Para::getNbCharPara() const
{
	int nb = 0;
	Format* zone = 0;

	if(_lines != 0)
	{
		kdDebug() << "  NB ZONE : " << _lines->count() << endl;

		for(zone = _lines->first(); zone != 0; zone = _lines->next())
		{
			switch(zone->getId())
			{
				case EF_TEXTZONE:
						nb = nb + ((TextZone*) zone)->getSize();
					break;
				case EF_PICTURE:
					break;
				case EF_TABULATOR:
					break;
				case EF_VARIABLE:
					break;
				case EF_FOOTNOTE:
					break;
				case EF_ANCHOR:
					break;
			}
		}
	}
	return nb;
}

/*******************************************/
/* Analyse                                 */
/*******************************************/
void Para::analyse(const QDomNode balise)
{
	/* MARKUP TYPE :  PARAGRAPH */

	/* Analyse of the parameters */
	kdDebug() << "ANALYSE A PARAGRAPH" << endl;
	
	/* Analyse of the children markups */
	for(int index= 0; index < getNbChild(balise); index++)
	{
		if(getChildName(balise, index).compare("TEXT")== 0)
		{
			_texte =  getChild(getChild(getChild(balise, index), "#text"), 0).nodeValue();
			kdDebug() << "TEXT : " << _texte << endl;
			
		}
		else if(getChildName(balise, index).compare("NAME")== 0)
		{
			analyseName(getChild(balise, index));
		}
		else if(getChildName(balise, index).compare("INFO")== 0)
		{
			analyseInfo(getChild(balise, index));
		}
		/*else if(getChildName(balise, index).compare("HARDBRK")== 0)
		{
			analyseBrk(getChild(balise, index));
		}*/
		else if(getChildName(balise, index).compare("FORMATS")== 0)
		{
			// Garder pour la nouvelle dtd
			// IMPORTANT ==> police + style
			kdDebug() << "FORMATS" << endl;
			analyseFormats(getChild(balise, index));
			
		}
		else if(getChildName(balise, index).compare("LAYOUT")== 0)
		{
			kdDebug() << "LAYOUT" << endl;
			analyseLayoutPara(getChild(balise, index));
		}
	}
	kdDebug() << "END OF PARAGRAPH" << endl;
}

/*******************************************/
/* AnalyseName                             */
/*******************************************/
/* If a footnote have a name : it's a      */
/* footnote/endnote.                       */
/*******************************************/
void Para::analyseName(const QDomNode balise)
{
	//<NAME name="Footnote/Endnote_1">
	
	_name = new QString(getAttr(balise, "NAME"));
}

/*******************************************/
/* AnalyseInfo                             */
/*******************************************/
/* Type of the parag. : if info is 1, it's */
/* a footnote/endnote (so it have a name). */
/*******************************************/
void Para::analyseInfo(const QDomNode balise)
{
	//<INFO info="1">
	
	_info = (EP_INFO) getAttr(balise, "INFO").toInt();
}

/*******************************************/
/* AnalyseBrk                              */
/*******************************************/
/* There is a new page before this         */
/* paragraph.                              */
/*******************************************/
/*void Para::analyseBrk(const QDomNode balise)
{
	//<NAME name="Footnote/Endnote_1">
	
	_hardbrk = (EP_HARDBRK) getAttr(balise, "FRAME").toInt();
}*/

/*******************************************/
/* AnalyseLayoutPara                       */
/*******************************************/
/* Analyse the layout of a para.           */
/* For each format, keep the type (picture,*/
/* text, variable, footnote) and put the   */
/* zone in a list.                         */
/*******************************************/
void Para::analyseLayoutPara(const QDomNode balise)
{
	Format* zone = 0;

	analyseLayout(balise);
	for(int index= 0; index < getNbChild(balise); index++)
	{
		if(getChildName(balise, index).compare("FORMAT")== 0)
		{
			//analyseFormat(balise);
			/* No more format : verify if all the text zone has been formated */
			if(_currentPos != _texte.length())
			{
				zone = new TextZone(_texte, this);
				((TextZone*) zone)->setPos(_currentPos);
				((TextZone*) zone)->setLength(_currentPos - _texte.length());
				((TextZone*) zone)->analyse();
				if(_lines == 0)
					_lines = new QList<Format>;
				/* add the text */
				_lines->append(zone);
				_currentPos = _currentPos + ((TextZone*) zone)->getLength();
			
			}
		}
		/*else
			kdDebug() << " FORMAT FIELD UNKNOWN" << endl;*/
	}
}

/*******************************************/
/* AnalyseFormats                          */
/*******************************************/
/* Analyse several formats.                */
/*  keep the type (picture, text, variable,*/
/* footnote) and put the zone in a list.   */
/*******************************************/
void Para::analyseFormats(const QDomNode balise)
{
	for(int index= 0; index < getNbChild(balise, "FORMAT"); index++)
	{
		if(getChildName(balise, index).compare("FORMAT")== 0)
		{
			kdDebug() << "A FORMAT !!!" << endl;
			analyseFormat(getChild(balise, index));
		}
		else
			kdDebug() << " FORMAT UNUSEFULL HERE" << endl;
	}
}

/*******************************************/
/* AnalyseFormat                           */
/*******************************************/
/* Analyse one format.                     */
/*  keep the type (picture, text, variable,*/
/* footnote) and put the zone in a list.   */
/*******************************************/
void Para::analyseFormat(const QDomNode balise)
{
	Format *zone      = 0;
	Format *zoneFirst = 0;

	kdDebug() << "ANALYSE FORMAT BODY" << endl;
	switch(getTypeFormat(balise))
	{
		case EF_ERROR: kdDebug() << "Id format error" << endl;
			break;
		case EF_TEXTZONE: /* It's a text line (1) */
				if(_currentPos != _texte.length())
				{
					zone = new TextZone(_texte, this);
					zone->analyse(balise);
					if(zone->getPos() != _currentPos)
					{
						if(_lines == 0)
							_lines = new QList<Format>;
							/* Create first a default format */
						zoneFirst = new TextZone(_texte, this);
						zoneFirst->setPos(_currentPos);
						zoneFirst->setLength(zone->getPos() - _currentPos);
						((TextZone*) zoneFirst)->analyse();

						/* Add the text without format */
						_lines->append(zoneFirst);
						_currentPos = _currentPos + ((TextZone*) zoneFirst)->getLength();
					}
				}
			break;
		case EF_PICTURE: /* It's a picture (2) */
				zone = new PictureZone(this);
				zone->analyse(balise);
			break;
		case EF_VARIABLE: /* It's a variable (4) */
		//		zone = new VariableZone(this);
		//		zone->analyse(balise);
			break;
		case EF_FOOTNOTE: /* It's a footnote (5) */
				zone = new Footnote(this);
				zone->analyse(balise);
			break;
		case EF_ANCHOR: /* It's an anchor (6) */
				zone = new Anchor(this);
				zone->analyse(balise);
			break;
		default: /* Unknown */
				kdDebug() << "Format not yet supported" << endl;
	}

	if(zone->getPos() != _currentPos)
	{
		if(_lines == 0)
			_lines = new QList<Format>;
			/* Create first a default format */
		zoneFirst = new TextZone(_texte, this);
		zoneFirst->setPos(_currentPos);
		zoneFirst->setLength(zone->getPos() - _currentPos);
		((TextZone*) zoneFirst)->analyse();
		kdDebug() << "pos courante : " << _currentPos << endl;
		/* Add the text without format */
		_lines->append(zoneFirst);
		_currentPos = _currentPos + zoneFirst->getLength();
	}
	
	if(zone != 0)
	{
		if(_lines == 0)
			_lines = new QList<Format>;

		/* add the text */
		_lines->append(zone);
		_currentPos = _currentPos + zone->getLength();
	}
}

/*******************************************/
/* Generate                                */
/*******************************************/
/* Generate each text zone with the parag. */
/* markup.                                 */
/*******************************************/
void Para::generate(QTextStream &out)
{

	kdDebug() << "  GENERATION PARA" << endl;
	
	if(getInfo() != EP_FOOTNOTE && getFrameType() != SS_HEADERS &&
	   getFrameType() != SS_FOOTERS)
	{	
		/* We generate center, itemize tag and new page only for
		 * parag not for footnote
		 * If a parag. have a special format (begining)
		 */
		if(isHardBreak())
			out << "\\newpage" << endl;
		generateDebut(out);
	}

	/* If a parag. have text :))) */
	if(_lines != 0)
	{
		Format* zone = 0;
		kdDebug() << "  NB ZONE : " << _lines->count() << endl;

		for(zone = _lines->first(); zone != 0; zone = _lines->next())
		{
			zone->generate(out);
		}
		/* To separate the text zones. */
	}

	if(getInfo() != EP_FOOTNOTE && getFrameType() != SS_HEADERS &&
	   getFrameType() != SS_FOOTERS)
	{
		/* id than above : a parag. have a special format. (end) 
		 * only it's not a header, nor a footer nor a footnote/endnote
		 */
		generateFin(out);
		if(isHardBreakAfter())
			out << "\\newpage" << endl;
	}
	kdDebug() << "PARA GENERATED" << endl;
}

/*******************************************/
/* GenerateDebut                           */
/*******************************************/
/* Generate the begining paragraph markup. */
/*******************************************/
void Para::generateDebut(QTextStream &out)
{
	/* Be careful we are in a table ! 
	 * You can't use directly environment, ...
	 */
	if(getFrameType() == SS_TABLE)
	{
		//int sizeCell = 5;
		/* first number depends with the cell size (next number}
		 * and with the number of characters in the para. 
		 * It can be 20 char. / 5 cm  = 4 char / cm so */
		/* nbLines = nb_char_para / (4 * cell size) + 1 */
		//sizeCell = (_element->getRight() - _element->getLeft()) / 27;
		//kdDebug() << "SIZE OF CELL : " << sizeCell << endl;
		// TODO : arrondir au superieur avec tgmath.h ??
		//_nbLines = ((_element->getBottom() - _element->getTop()) / 27) + 1;
		//kdDebug() << "NB OF LINES : " << _nbLines << endl;
		/* 2 at least, 1 for the line, 1 for the space line */
		/*if(_nbLines < 2)
			_nbLines = 2;
		out << "\\multirow{" << _nbLines << "}{"<< sizeCell << "cm}{" << endl;*/
	}
	/* if it's a chapter */
	if(isChapter())
	{
		/* switch the type, the depth do */
		generateTitle(out);
	}
	else if(isEnum())
	{
		out << "\\item ";
	}

}

void Para::generateBeginEnv(QTextStream &out)
{
	kdDebug() << "Begin new Env : " << getEnv() << endl;
	switch(getEnv())
	{
		case ENV_LEFT: out << "\\begin{flushleft}" << endl;
			break;
		case ENV_RIGHT: out << "\\begin{flushright}" << endl;
			break;
		case ENV_CENTER: out << "\\begin{center}" << endl;
			break;
		case ENV_JUSTIFY: out << endl;
			break;
	}
}

/*******************************************/
/* openList                                */
/*******************************************/
/* Generate the markup to begin a list and */
/* push the type in the historic stack.    */
/*******************************************/
void Para::openList(QTextStream &out)
{
	EType *type_temp = 0;

	switch(getCounterType())
	{
		case TL_NONE:
			break;
		case TL_ARABIC:
			out << "\\begin{enumerate}" << endl;
			break;
		case TL_LLETTER:	/* a, b, ... */
			out << "\\begin{enumerate}[a]" << endl;
			break;
		case TL_CLETTER:	/* A, B, ... */
			out << "\\begin{enumerate}[A]" << endl;
			break;
		case TL_LLNUMBER:	/* i, ii, ... */
			out << "\\begin{enumerate}[i]" << endl;
			break;
		case TL_CLNUMBER: 	/* I, II, ... */
			out << "\\begin{enumerate}[I]" << endl;
			break;
		case TL_CUSTOM_SIMPLE: /* - */
			out << "\\begin{enumerate}[" << convertSpecialChar(getCounterBullet()) << "]" << endl;
			break;
		case TL_CUSTOM_COMPLEX: /* - */
			out << "\\begin{enumerate}[" << convertSpecialChar(getCounterBullet()) << "]" << endl;
			break;
		case TL_CIRCLE_BULLET:
			out << "\\begin{itemize}" << endl;
			break;
		case TL_SQUARE_BULLET:
			out << "\\begin{itemize}" << endl;
			break;
		case TL_DISC_BULLET:
			out << "\\begin{itemize}" << endl;
			break;
		default:
			out << "\\begin{itemize}[SPECIAL]" << endl;
	}

	/* Keep the list type */
	type_temp = new EType(getCounterType());
	kdDebug() << " type list to open : " << *type_temp << endl;
	_historicList.push(type_temp);
}

/*******************************************/
/* GenerateFin                             */
/*******************************************/
/* Generate the closing paragraph markup.  */
/*******************************************/
void Para::generateFin(QTextStream &out)
{
	/* Close a title of chapter */
	if(isChapter())
		out << "}";
}

/*******************************************/
/* GenerateEndEnv                          */
/*******************************************/
/* Generate the closing environment markup.*/
/*******************************************/
void Para::generateEndEnv(QTextStream &out)
{
	kdDebug() << "end of an environment : " << getEnv() << endl;
	switch(getEnv())
	{
		case ENV_LEFT: out << endl << "\\end{flushleft}";
			break;
		case ENV_RIGHT: out << endl << "\\end{flushright}";
			break;
		case ENV_CENTER: out << endl << "\\end{center}";
			break;
		case ENV_JUSTIFY:
			break;
	}
}

/*******************************************/
/* closeList                               */
/*******************************************/
/* Generate the closing list markup for a  */
/* list type (letter, custom, ...) and     */
/* remove the last list saved.             */
/*******************************************/
void Para::closeList(QTextStream &out, Para* next)
{
	closeList(getCounterType(), out);

	if(((getCounterDepth() - 1) >= 0) && ((next!= 0 && !next->isEnum()) || next == 0))
	{
		/* We must close all the lists since
		 * after this paragraph it's a normal paragraph.
		 */
		kdDebug() << "lists to close" << endl;
		while(!_historicList.isEmpty())
		{
			EType *type_temp = 0;
			type_temp = _historicList.pop();
			if(type_temp != 0)
				closeList(*type_temp, out);
		}
	}
}

/*******************************************/
/* closeList                               */
/*******************************************/
/* Generate the closing list markup for a  */
/* list type (letter, custom, ...) and     */
/* remove the last list saved.             */
/*******************************************/
void Para::closeList(EType type, QTextStream &out)
{
	//out << endl;
	kdDebug() << " type list to close : " << type << endl;

	/* Because of a new markup, we need a new line. */
	out << endl;

	/* but the next parag is not a same list */
	switch(type)
	{
		case TL_NONE: //out << endl;
			break;
		case TL_ARABIC:
		case TL_LLETTER:  /* a, b, ... */
		case TL_CLETTER:  /* A, B, ... P. 250*/
		case TL_LLNUMBER: /* i, ii, ... */
		case TL_CLNUMBER: /* I, II, ... */
		case TL_CUSTOM_SIMPLE: /* - */
		case TL_CUSTOM_COMPLEX: /* - */
			       out << "\\end{enumerate}" << endl;
			break;
		case TL_CIRCLE_BULLET:
				out << "\\end{itemize}" << endl;
			break;
		case TL_SQUARE_BULLET:
		case TL_DISC_BULLET:
				out << "\\end{itemize}" << endl;
			break;
		default:
				out << "no suported" << endl;
	}

	/* Pop the list which has been closed */
	_historicList.remove();
	kdDebug() << "removed" << endl;
}

/*******************************************/
/* GenerateTitle                           */
/*******************************************/
void Para::generateTitle(QTextStream &out)
{
	switch(getCounterDepth())
	{
		case 0:
			out << "\\section{";
			break;
		case 1:
			out << "\\subsection{";
			break;
		case 2:
			out << "\\subsubsection{";
			break;
		case 3:
			out << "\\paragraph{";
			break;
		case 4:
			out << "\\subparagraph{";
			break;
		case 5:
			out << "% section too deep" << endl;
			out << "\\textbf{";
	}
}

