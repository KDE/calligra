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

/* static data */
QStack<EType> Para::_historicList;

/*******************************************/
/* Constructor                             */
/*******************************************/
Para::Para(Texte* texte)
{
	_element   = texte;
	_lines     = 0;
	_next      = 0;
	_previous  = 0;
	_name      = 0;
	_info      = EP_NONE;	/* the parag is not a footnote */
	_hardbrk   = EP_FLOW;	/* and it's not a new page */
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
/* IsColored                               */
/*******************************************/
/* Return TRUE if there is at least one    */
/* text zone which use color.              */
/*******************************************/
/* DEPRECATED.                             */
/*******************************************/
/*bool Para::isColored() const
{
	bool color;
	FormatIter iter;

	color = false;
	iter.setList(_lines);
	while(!iter.isTerminate() && !color)
	{
		if(iter.getCourant()->getFormatType()== EF_TEXTZONE)
			color = ((TextZone*) iter.getCourant())->isColor();
		iter.next();
	}
	return color;
}*/

/*******************************************/
/* isUlined                                */
/*******************************************/
/* Return TRUE if there is at least one    */
/* text zone which use uline.              */
/*******************************************/
/* DEPRECATED.                             */
/*******************************************/
/*bool Para::isUlined() const
{
	bool uline;
	FormatIter iter;

	uline = false;
	iter.setList(_lines);
	while(!iter.isTerminate() && !uline)
	{
		if(iter.getCourant()->getFormatType()== EF_TEXTZONE)
			uline = ((TextZone*) iter.getCourant())->isUnderlined();
		iter.next();
	}
	return uline;
}*/

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
EFormat Para::getTypeFormat(const Markup* balise) const
{
	Arg *arg = 0;

	//<FORMAT id="1" ...>
	for(arg= balise->pArg; arg!= 0; arg= arg->pNext)
	{
		kdDebug() << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "ID")== 0)
		{
			return (EFormat) atoi(arg->zValue);
		}
	}
	return EF_ERROR;
}

/*******************************************/
/* getNbCharPara                           */
/*******************************************/
/* To know the size of a paragraph.        */
/*******************************************/
int Para::getNbCharPara() const
{
	int nb = 0;
	FormatIter iter;

	if(_lines != 0)
	{
		kdDebug() << "  NB ZONE : " << _lines->getSize() << endl;
		iter.setList(_lines);
		while(!iter.isTerminate())
		{
			Format* zone = iter.getCourant();
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
			iter.next();
		}
	}
	return nb;
}

/*******************************************/
/* Analyse                                 */
/*******************************************/
void Para::analyse(const Markup * balise_initiale)
{
	Token* savedToken = 0;
	Markup* balise    = 0;

	/* MARKUP TYPE :  PARAGRAPH */

	/* Analyse of the parameters */
	kdDebug() << "ANALYSE A PARAGRAPH" << endl;
	
	/* Analyse of the children markups */
	savedToken = enterTokenChild(balise_initiale);
	while((balise = getNextMarkup()) != 0)
	{
		if(strcmp(balise->token.zText, "TEXT")== 0)
		{
			for(Token *p = balise->pContent; p!= 0; p = p->pNext)
			{
				if(p->zText!= 0)
				{
					_texte += p->zText;
				}
			}
		}
		else if(strcmp(balise->token.zText, "NAME")== 0)
		{
			analyseName(balise);
		}
		else if(strcmp(balise->token.zText, "INFO")== 0)
		{
			analyseInfo(balise);
		}
		else if(strcmp(balise->token.zText, "HARDBRK")== 0)
		{
			analyseBrk(balise);
		}
		else if(strcmp(balise->token.zText, "FORMATS")== 0)
		{
			// Garder pour la nouvelle dtd
			// IMPORTANT ==> police + style
			kdDebug() << "FORMATS" << endl;
			analyseFormats(balise);
			
		}
		else if(strcmp(balise->token.zText, "LAYOUT")== 0)
		{
			kdDebug() << "LAYOUT" << endl;
			analyseLayoutPara(balise);
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
void Para::analyseName(const Markup* balise)
{
	Arg *arg = 0;

	//<NAME name="Footnote/Endnote_1">
	for(arg= balise->pArg; arg!= 0; arg= arg->pNext)
	{
		kdDebug() << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "NAME")== 0)
		{
			_name = new QString(arg->zValue);
		}
	}
}

/*******************************************/
/* AnalyseInfo                             */
/*******************************************/
/* Type of the parag. : if info is 1, it's */
/* a footnote/endnote (so it have a name). */
/*******************************************/
void Para::analyseInfo(const Markup* balise)
{
	Arg *arg = 0;

	//<INFO info="1">
	for(arg= balise->pArg; arg!= 0; arg= arg->pNext)
	{
		kdDebug() << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "INFO")== 0)
		{
			_info = (EP_INFO) atoi(arg->zValue);
		}
	}
}

/*******************************************/
/* AnalyseBrk                              */
/*******************************************/
/* There is a new page before this         */
/* paragraph.                              */
/*******************************************/
void Para::analyseBrk(const Markup* balise)
{
	Arg *arg = 0;

	//<NAME name="Footnote/Endnote_1">
	for(arg= balise->pArg; arg!= 0; arg= arg->pNext)
	{
		kdDebug() << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "FRAME")== 0)
		{
			_hardbrk = (EP_HARDBRK) atoi(arg->zValue);
		}
	}
}

/*******************************************/
/* AnalyseLayoutPara                       */
/*******************************************/
/* Analyse the layout of a para.           */
/* For each format, keep the type (picture,*/
/* text, variable, footnote) and put the   */
/* zone in a list.                         */
/*******************************************/
void Para::analyseLayoutPara(const Markup *balise_initiale)
{
	Token* savedToken = 0;
	Markup* balise    = 0;
	Format* zone      = 0;

	savedToken = enterTokenChild(balise_initiale);
	analyseLayout(balise_initiale);
	while((balise = getNextMarkup()) != NULL)
	{
		kdDebug() << balise << endl;
		kdDebug() << balise->token.zText << endl;
		if(strcmp(balise->token.zText, "FORMAT")== 0)
		{
			//analyseFormat(balise);
			/* No more format : verify if all the text zone has been formated */
			if(_currentPos != _texte.length())
			{
				zone = new TextZone(_texte, this);
				((TextZone*) zone)->setPos(_currentPos);
				((TextZone*) zone)->setLength(_currentPos - _texte.length());
				zone->analyse(0);
				if(_lines == 0)
					_lines = new ListeFormat;
				/* add the text */
				_lines->addLast(zone);
				_currentPos = _currentPos + ((TextZone*) zone)->getLength();
			
			}
		}
		else
			kdDebug() << " FORMAT FIELD UNKNOWN" << endl;
	}
	setTokenCurrent(savedToken);

}

/*******************************************/
/* AnalyseFormats                          */
/*******************************************/
/* Analyse several formats.                */
/*  keep the type (picture, text, variable,*/
/* footnote) and put the zone in a list.   */
/*******************************************/
void Para::analyseFormats(const Markup *balise_initiale)
{
	Token* savedToken = 0;
	Markup* balise    = 0;

	savedToken = enterTokenChild(balise_initiale);
	//analyseLayout(balise_initiale);
	while((balise = getNextMarkup()) != NULL)
	{
		kdDebug() << balise << endl;
		kdDebug() << balise->token.zText << endl;
		if(strcmp(balise->token.zText, "FORMAT")== 0)
		{
			kdDebug() << "A FORMAT !!!" << endl;
			analyseFormat(balise);
		}
		else
			kdDebug() << " FORMAT UNUSEFULL HERE" << endl;
	}
	setTokenCurrent(savedToken);

}

/*******************************************/
/* AnalyseFormat                           */
/*******************************************/
/* Analyse one format.                     */
/*  keep the type (picture, text, variable,*/
/* footnote) and put the zone in a list.   */
/*******************************************/
void Para::analyseFormat(const Markup *balise)
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
					if(((TextZone*) zone)->getPos() != _currentPos)
					{
						if(_lines == 0)
							_lines = new ListeFormat;
						/* Create first a default format */
						zoneFirst = new TextZone(_texte, this);
						((TextZone*) zoneFirst)->setPos(_currentPos);
						((TextZone*) zoneFirst)->setLength(((TextZone*) zone)->getPos() - _currentPos);
						zoneFirst->analyse(0);
						/* Add the text without format */
						_lines->addLast(zoneFirst);
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
		//		zone = new Anchor(this);
		//		zone->analyse(baslie);
			break;
		default: /* Unknown */
				kdDebug() << "Format not yet supported" << endl;
		}
	if(zone != 0)
	{
		if(_lines == 0)
			_lines = new ListeFormat;
		/* add the text */
		_lines->addLast(zone);
		_currentPos = _currentPos + ((TextZone*) zone)->getLength();
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
		if(_hardbrk == EP_NEXT)
			out << "\\newpage" << endl;
		generateDebut(out);
	}
	/*setLastName();
	setLastCounter();*/
	/* If a parag. have text :))) */
	if(_lines != 0)
	{
		FormatIter iter;
		kdDebug() << "  NB ZONE : " << _lines->getSize() << endl;
		iter.setList(_lines);
		while(!iter.isTerminate())
		{
			iter.getCourant()->generate(out);
			iter.next();
		}
		/* To separate the text zones */
		out << endl;
	}

	if(getInfo() != EP_FOOTNOTE && getFrameType() != SS_HEADERS &&
	   getFrameType() != SS_FOOTERS)
	{
		/* id than above : a parag. have a special format. (end) 
		 * only it's not a header, nor a footer nor a footnote/endnote
		 */
		generateFin(out);
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
		int sizeCell = 5;
		/* first number depends with the cell size (next number}
		 * and with the number of characters in the para. 
		 * It can be 20 char. / 5 cm  = 4 char / cm so */
		/* nbLines = nb_char_para / (4 * cell size) + 1 */
		sizeCell = (_element->getRight() - _element->getLeft()) / 27;
		kdDebug() << "SIZE OF CELL : " << sizeCell << endl;
		// TODO : arrondir au superieur 
		_nbLines = ((_element->getBottom() - _element->getTop()) / 27) + 1;
		kdDebug() << "NB OF LINES : " << _nbLines << endl;
		/* 2 at least, 1 for the line, 1 for the space line */
		if(_nbLines < 2)
			_nbLines = 2;
		out << "\\multirow{" << _nbLines << "}{"<< sizeCell << "cm}{" << endl;
	}
	/* if it's a chapter */
	if(isChapter())
	{
		/* switch the type, the depth do*/
		generateTitle(out);
	}
	else if(_lines != 0)
	{
		/* If the paragraph had text */
		if(_previous == 0 || _previous->getEnv() != getEnv() || getInfo() == EP_FOOTNOTE)
		{
			/* It's a parag. */
			switch(getEnv())
			{
				case ENV_LEFT: out << "\\begin{flushleft}" << endl;
					break;
				case ENV_RIGHT: out << "\\begin{flushright}" << endl;
					break;
				case ENV_CENTER: out << "\\begin{center}" << endl;
					break;
				case ENV_NONE: if(getFrameType() == SS_TABLE)
							out << "\\begin{flushleft}";
						out << endl;
					break;
			}
		}
		if(isEnum())
		{
			/* If it's a list : */
			/* - go in a new list */
			/* - change depth (a list in a list) */
			/* - or two lists nearby */
			if(_previous == 0 || !_previous->isList() ||
			  (_previous->isList() && (
				(_previous->getCounterDepth() < getCounterDepth()) ||
				(_previous->getCounterType() != getCounterType()))
			  ))
			{
				openList(getCounterType(), out);
			}
			out << "\\item ";
		}
	}
}

/*******************************************/
/* openList                                */
/*******************************************/
/* Generate the markup to begin a list and */
/* push the type in the historic stack.    */
/*******************************************/
void Para::openList(EType type, QTextStream &out)
{
	EType *type_temp = 0;

	switch(type)
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
		case TL_CLNUMBER: /* I, II, ... */
			out << "\\begin{enumerate}[I]" << endl;
			break;
		case TL_CUSTOM_SIMPLE: /* - */
			out << "\\begin{enumerate}[" << getCounterBullet() << "]" << endl;
			break;
		case TL_CUSTOM_COMPLEX: /* - */
			out << "\\begin{enumerate}[" << getCounterBullet() << "]" << endl;
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
	type_temp = new EType(type);
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
		out << "}" << endl;
	else if(isList())
	{
		/* It's a list : */
		/* - end of a list */
		/* - change the deph (end of a list in a list) */
		/* - end of a first list with two lists nearby */
		/* - always in a cell of a table */
		if(_next == 0 || !_next->isEnum() ||
		  (_next->isList() && _next->getCounterDepth() < getCounterDepth()) ||
		  (_next->isList() && _next->getCounterType() != getCounterType() && _next->getCounterDepth() == getCounterDepth()) ||
		  getFrameType() == SS_TABLE)
		{
			closeList(getCounterType(), out);

			if(((getCounterDepth() - 1) >= 0) && ((_next!= 0 && !_next->isEnum()) || _next == 0))
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
	}
	if(((_previous && !_previous->isChapter()) || !_previous) && _lines != 0)
	{
		if((_next == 0 || _next->getEnv() != getEnv()) && !isChapter())
		{
			switch(getEnv())
			{
				case ENV_LEFT: out << "\\end{flushleft}" << endl;
					break;
				case ENV_RIGHT: out << "\\end{flushright}" << endl;
					break;
				case ENV_CENTER: out << "\\end{center}" << endl;
					break;
				case ENV_NONE: if(getFrameType() == SS_TABLE)
							out << "\\end{flushleft}" << endl;
					break;
			}
		}
		/* Be carefull : for table the CR are dangerous */
		/*if((_next == 0 || _next->getEnv() == getEnv()) &&
		   getFrameType() != SS_TABLE)
			out << endl;*/
	}
	/* Be careful we are in a table ! 
	 * You can't use directly environment, ...
	 */
	if(getFrameType() == SS_TABLE)
	{
		out << "} \\\\" << endl;
		/* Create all the lines used by this paragraph */
		for(int i = 0; i< _nbLines; i++)
			out << "\\\\ ";
		out << endl;
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
		default:
			out << "% section too deep" << endl;
	}
}
