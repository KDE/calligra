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

#include <stdlib.h>		/* for atoi function */
#include <kdebug.h>		/* for kdDebug() stream */
#include "texte.h"

/*******************************************/
/* Constructor                             */
/*******************************************/
Texte::Texte()
{
	_left              = 0;
	_right             = 0;
	_top               = 0;
	_bottom            = 0;
	_runaround         = TA_NONE;
	_runaroundGap      = 0;
	_autoCreate        = TC_EXTEND;
	_newFrameBehaviour = TF_RECONNECT;
	_sheetSide         = TS_ANYSIDE;
	//_footnotes         = 0;

	setType(ST_TEXT);
}

/*******************************************/
/* searchFootnote                          */
/*******************************************/
Para* Texte::searchFootnote(const QString name)
{
	/*ParaIter iter;

	if(_footnotes != 0)
	{
		iter.setList(*_footnotes);
		while(!iter.isTerminate())*/
		for(Para* current = _footnotes.first(); current!= 0; current = _footnotes.next())
		{
			QString* string = current->getName();
			kdDebug() << *string << endl;
			if(*string == name)
				return current;
			//iter.next();
		}
	//}
	return 0;
}

/*******************************************/
/* analyse                                 */
/*******************************************/
void Texte::analyse(const Markup * balise_initiale)
{
	Token* savedToken = 0;
	Markup* balise    = 0;

	/* MARKUP TYPE : FRAMESET INFO = TEXTE, ENTETE CONNUE */
	
	/* Parameters Analyse */
	Element::analyse(balise_initiale);

	kdDebug() << "FRAME ANALYSE (Texte)" << endl;

	/* Chlidren markups Analyse */
	savedToken = enterTokenChild(balise_initiale);
	while((balise = getNextMarkup()) != 0)
	{
		if(strcmp(balise->token.zText, "FRAME")== 0)
		{
			analyseParamFrame(balise);
		}
		else if(strcmp(balise->token.zText, "PARAGRAPH")== 0)
		{
			// 1. Create a paragraph :
			Para *prg = new Para(this);
			// 2. Add the informations :
			prg->analyse(balise);
			if(prg->getInfo() == EP_FOOTNOTE)
			{
				// 3. add this parag. in the footnote list
				//if(_footnotes == 0)
				//	_footnotes = new ListPara;
				_footnotes.append(prg);
			}
			else
			{
				// 3. add this parag. in the text list
				_parags.append(prg);
			}
			kdDebug() << "PARA ADDED" << endl;
		}
		
	}
	kdDebug() << "END OF A FRAME ANALYSE" << endl;
}

/*******************************************/
/* analyseParamFrame                       */
/*******************************************/
void Texte::analyseParamFrame(const Markup *balise)
{
	/*<FRAME left="28" top="42" right="566" bottom="798" runaround="1" />*/
	Arg *arg = 0;

	for(arg= balise->pArg; arg; arg= arg->pNext)
	{
		kdDebug() << "PARAM " << arg->zName << endl;
		if(strcmp(arg->zName, "LEFT")== 0)
		{
			setLeft(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "TOP")== 0)
		{
			setTop(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "RIGHT")== 0)
		{
			setRight(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "BOTTOM")== 0)
		{
			setBottom(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "RUNAROUND")== 0)
		{
			setRunAround(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "RUNAROUNDGAP")== 0)
		{
			setAroundGap(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "AUTOCREATENEWFRAME")== 0)
		{
			setAutoCreate(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "NEWFRAMEBEHAVIOUR")== 0)
		{
			setNewFrame(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "SHEETSIDE")== 0)
		{
			setSheetSide(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "LWIDTH")== 0)
		{
			setLeftWidth(atoi(arg->zValue));
			useLeftBorder();
		}
		else if(strcmp(arg->zName, "RWIDTH")== 0)
		{
			setRightWidth(atoi(arg->zValue));
			useRightBorder();
		}
		else if(strcmp(arg->zName, "TWIDTH")== 0)
		{
			setTopWidth(atoi(arg->zValue));
			useTopBorder();
		}
		else if(strcmp(arg->zName, "BWIDTH")== 0)
		{
			setBottomWidth(atoi(arg->zValue));
			useBottomBorder();
		}
		else if(strcmp(arg->zName, "LRED")== 0)
		{
			setLeftRed(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "LGREEN")== 0)
		{
			setLeftGreen(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "LBLUE")== 0)
		{
			setLeftBlue(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "RRED")== 0)
		{
			setRightRed(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "RGREEN")== 0)
		{
			setRightGreen(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "RBLUE")== 0)
		{
			setRightBlue(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "TRED")== 0)
		{
			setTopRed(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "TGREEN")== 0)
		{
			setTopGreen(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "TBLUE")== 0)
		{
			setTopBlue(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "BRED")== 0)
		{
			setBottomRed(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "BGREEN")== 0)
		{
			setBottomGreen(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "BBLUE")== 0)
		{
			setBottomBlue(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "LSTYLE")== 0)
		{
			setLeftStyle(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "RSTYLE")== 0)
		{
			setRightStyle(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "TSTYLE")== 0)
		{
			setTopStyle(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "BSTYLE")== 0)
		{
			setBottomStyle(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "BKRED")== 0)
		{
			setBkRed(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "BKGREEN")== 0)
		{
			setBkGreen(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "BKBLUE")== 0)
		{
			setBkBlue(atoi(arg->zValue));
		}
		/*else if(strcmp(arg->zName, "BLEFTPT")== 0)
		{
			setLeftWidth(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "BRIGHTPT")== 0)
		{
			setLeftWidth(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "BTOPPT")== 0)
		{
			setLeftWidth(atoi(arg->zValue));
		}
		else if(strcmp(arg->zName, "BBOTTOMPT")== 0)
		{
			setLeftWidth(atoi(arg->zValue));
		}*/
	}
}

/*******************************************/
/* generate                                */
/*******************************************/
void Texte::generate(QTextStream &out)
{
	//ParaIter iter;
	Para * lastPara = 0;

	kdDebug() << "TEXT GENERATION" << endl;
	kdDebug() << "NB PARA " << _parags.count() << endl;
	//iter.setList(_parags);

	if(getSection() == SS_TABLE)
	{
		/* If the element has a border display it here */
		if(hasTopBorder())
		{
			out << "\\cline{" << getCol() << "-" << getCol() << "}" << endl;
		}
		out << "\\begin{minipage}{";
		out << (getRight() - getLeft()) << "pt}" << endl;
	}
	_lastEnv = ENV_NONE;
	_lastTypeEnum = TL_NONE;
	//while(!iter.isTerminate())
	Para* currentPara = _parags.first();
	while( currentPara != 0)
	{
		//Para* currentPara = iter.getCourant();
		/* layout managment */
		if(!currentPara->isChapter() && _lastEnv != getNextEnv(_parags, _parags.at()) &&
			_lastTypeEnum == TL_NONE)
		{
			currentPara->generateBeginEnv(out);
			_lastEnv = currentPara->getEnv();
		}
		/* List managment */
		if(isBeginEnum(lastPara, currentPara))
		{
			currentPara->openList(out);
			_lastTypeEnum = currentPara->getCounterType();
		}
		/* paragraph generation */
		currentPara->generate(out);

		lastPara = currentPara;
		currentPara = _parags.next();

		/* list managment */
		if(isCloseEnum(lastPara, currentPara))
		{
			lastPara->closeList(out, currentPara);
			_lastTypeEnum = TL_NONE;
		}
		/* layout managment */
		if(!lastPara->isChapter() && _lastEnv != getNextEnv(_parags, _parags.at()))
			lastPara->generateEndEnv(out);
		out << endl;
	}

	if(getSection() == SS_TABLE)
	{
		out << "\\end{minipage}" << endl;

		/* If the element has a border display it here */
		/* doesn't work */
		if(hasBottomBorder())
		{
			out << "\\cline{" << getCol() << "-" << getCol() << "}" << endl;
		}
	}
}

EEnv Texte::getNextEnv(QList<Para> liste, const int pos)
{
	Para* index = 0;

	for(index = liste.at(pos); index != 0 && index->isChapter(); index = liste.next())
	{}

	if(index != 0)
	{
		return index->getEnv();
	}
	else
		return ENV_NONE;
}

/* next is the paragraph which will be generated, just after */

bool Texte::isBeginEnum(Para* previous, Para* next)
{
	/* If it's a list : */
	/* - go in a new list */
	/* - change depth (a list in a list) */
	/* - or two lists nearby (but with the same depth) */
	if(previous == 0 || !previous->isList() ||
		  (previous->isList() && (
			(previous->getCounterDepth() < next->getCounterDepth()) ||
			(previous->getCounterType() != next->getCounterType() &&
			previous->getCounterDepth() == next->getCounterDepth()))
		  ))
		return true;
	
	return false;
}

/* next is the paragraph which will be generated in the next loop */
bool Texte::isCloseEnum(Para* previous, Para* next)
{
	if(next == 0 || !next->isEnum() ||
		  (next->isList() && next->getCounterDepth() < previous->getCounterDepth()) ||
		  (next->isList() && next->getCounterType() != previous->getCounterType() &&
		  	next->getCounterDepth() == previous->getCounterDepth()) ||
		  previous->getFrameType() == SS_TABLE)
		return true;

	return false;
}
