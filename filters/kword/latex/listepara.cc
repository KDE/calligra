
/* BUGS : latex don't support alpha list with one command !!! the
 * command generated doesn't exist :))))
 */

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

#include <kdebug.h>		/* for kdDebug() stream */
#include "listepara.h"

ListPara::ListPara()
{
	kdDebug() << "Create liste para empty" << endl;
	_start  = 0;
	_end    = 0;
	_size   = 0;
}

ListPara::~ListPara()
{
	kdDebug() << "Destruction of a list of parag" << endl;
	vider();
	kdDebug() << "ok" << endl;
}

void ListPara::initialiser(Para *elt)
{
	kdDebug() << "initialise a list of parag at " << elt << endl;
	_end = _start = elt;
}

void ListPara::add(Para *elt)
{
	if(_start == 0)
	{
		initialiser(elt);
		_size = 1;
	}
	else
	{
		kdDebug() << "add a parag." << endl;
		_end->setNext(elt);
		elt->setPrevious(_end);
		_end  = elt;
		_size = _size + 1;
	}
}

void ListPara::rem()
{
	Para *first_saved = 0;

	first_saved = _start;
	_start      = _start->getNext();
	delete first_saved;
	_size  = _size - 1;
}

void ListPara::vider()
{
	while(_start != 0)
	{
		rem();
	}
}

/***********************************************************/
Para::Para()
{
	_liste    = 0;
	_next     = 0;
	_previous = 0;
}

Para::~Para()
{
	kdDebug() << "Destruction of a parag." << endl;
	if(_liste != 0)
		delete _liste;
}

void Para::analyse(const Markup * balise_initiale)
{
	Token* savedToken = 0;
	Markup* balise    = 0;

	// MARKUP TYPE :  PARAGRAPH

	// Analyse of the parameters
	kdDebug() << "ANALYSE A PARAGRAPH" << endl;
	
	// Analyse of the children markups
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
			kdDebug() << "TEXTE : " << _texte.latin1() << endl;
		}
		else if(strcmp(balise->token.zText, "FORMATS")== 0)
		{
			// IMPORTANT ==> police + style
			kdDebug() << "FORMATS" << endl;
			analyseFormats(balise);
			
		}
		else if(strcmp(balise->token.zText, "LAYOUT")== 0)
		{
			kdDebug() << "LAYOUT" << endl;
			analyseLayout(balise);
			// Hey, it's a title : not use
			// the format
			if(isChapter())
			{
				TextZoneIter iter;
				iter.setList(_liste);
				while(!iter.isTerminate())
				{
					kdDebug() << "NOT USE FORMAT" << endl;
					iter.getCourant()->notUseFormat();
					iter.next();
				}
			}
		}
	}
	kdDebug() << "END OF PARAGRAPH" << endl;
}

void Para::analyseFormats(const Markup *balise_initiale)
{
	Token* savedToken = 0;
	Markup* balise    = 0;

	savedToken = enterTokenChild(balise_initiale);
	while((balise = getNextMarkup()) != NULL)
	{
		TextZone *texte = new TextZone(_texte);
		if(strcmp(balise->token.zText, "FORMAT")== 0)
		{
			texte->analyse(balise);
			// If it's the first element
			if(_liste == 0)
				_liste = new ListeTextZone;
			// add the text
			_liste->addLast(texte);
		}
		else
			kdDebug() << " FORMAT FIELD UNKNOWN" << endl;
	}
	setTokenCurrent(savedToken);
}

void Para::generate(QTextStream &out)
{

	kdDebug() << "  GENERATION PARA" << endl;
	/* If a parag. have text :))) */
	if(_liste != 0)
	{
		/* If a parag. have a special format (begining) */
		generateDebut(out);
		setLastName();
		setLastCounter();

		TextZoneIter iter;
		kdDebug() << "  NB ZONE : " << _liste->getSize() << endl;
		iter.setList(_liste);
		while(!iter.isTerminate())
		{
			iter.getCourant()->generate(out);
			iter.next();
		}
		/* id than above : a parag. have a special format. (end) */
		generateFin(out);
	}
	kdDebug() << "PARA GENERATED" << endl;
}

void Para::generateDebut(QTextStream &out)
{
	/* if it's a chapter */
	if(isChapter())
	{
		/* switch the type, the depth do*/
		generateTitle(out);
	}
	else if(isList())
	{
		/* if it's a list */
		if(_previous == 0 || !_previous->isList() ||
			(_previous->isList() && _previous->getCounterDepth() < getCounterDepth()))
		{
			switch(getCounterType())
			{
				case STANDARD:
					break;
				case ARABIC:
					   out << "\\begin{enumerate}" << endl;
					break;
				/*case ALPHA:
					    out << "\\item ";
					break;*/
				case BULLET:
					     out << "\\begin{itemize}" << endl;
			}
		}
		out << "\\item ";
	}
	else
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
			case ENV_NONE: /* Nothing to do */
				break;
		}
	}
}

void Para::generateFin(QTextStream &out)
{
	/* Close a title of chapter */
	if(isChapter())
		out << "}" << endl;
	else if(isList())
	{
		/* It's a list */
		out << endl;
		if(_next == 0 || !_next->isList() ||
			(_next->isList() && _next->getCounterDepth() > getCounterDepth()))
		{
			/* but the next parag is not a same list */
			switch(getCounterType())
			{
				case STANDARD: out << endl;
					break;
				case ARABIC: 
					       out << "\\end{enumerate}" << endl;
					break;
				/*case ALPHA:
					    out << "\\end{alphabetic}" << endl;
					break;*/
				case BULLET:
					     out << "\\end{itemize}" << endl;
			}
		}
	}
	else
	{
		/* It's a parag. */
		/* Close an environment */
		out << endl;
		switch(getEnv())
		{
			case ENV_LEFT: out << "\\end{flushleft}" << endl;
				break;
			case ENV_RIGHT: out << "\\end{flushright}" << endl;
				break;
			case ENV_CENTER: out << "\\end{center}" << endl;
				break;
			case ENV_NONE: /* Nothing to do */
				break;
		}
	}
}

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
