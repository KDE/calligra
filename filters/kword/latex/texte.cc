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
void Texte::analyse(const QDomNode balise)
{
	/* MARKUP TYPE : FRAMESET INFO = TEXTE, ENTETE CONNUE */
	
	/* Parameters Analyse */
	Element::analyse(balise);

	kdDebug() << "FRAME ANALYSE (Texte)" << endl;

	/* Chlidren markups Analyse */
	analyseParamFrame(getChild(balise, "FRAME"));

	for(int index = 0; index < getNbChild(balise); index++)
	{
		if(getChildName(balise, index).compare("PARAGRAPH")== 0)
		{
			// 1. Create a paragraph :
			Para *prg = new Para(this);
			// 2. Add the informations :
			prg->analyse(getChild(balise, index));
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
void Texte::analyseParamFrame(const QDomNode balise)
{
	/*<FRAME left="28" top="42" right="566" bottom="798" runaround="1" />*/
	setLeft(getAttr(balise, "left").toInt());
	setTop(getAttr(balise, "top").toInt());
	setRight(getAttr(balise, "right").toInt());
	setBottom(getAttr(balise, "bottom").toInt());
	setRunAround(getAttr(balise, "runaround").toInt());
	setAroundGap(getAttr(balise, "runaroundGap").toInt());
	setAutoCreate(getAttr(balise, "autoCreateNewFrame").toInt());
	setNewFrame(getAttr(balise, "newFrameBehaviour").toInt());
	setSheetSide(getAttr(balise, "sheetSide").toInt());
	if(getAttr(balise, "lwidth").toInt() > 0)
	{
		setLeftWidth(getAttr(balise, "lwidth").toInt());
		useLeftBorder();
	}
	if(getAttr(balise, "rwidth").toInt() > 0)
	{
		setRightWidth(getAttr(balise, "rwidth").toInt());
		useRightBorder();
	}
	if(getAttr(balise, "twidth").toInt() > 0)
	{
		setTopWidth(getAttr(balise, "twidth").toInt());
		useTopBorder();
	}
	if(getAttr(balise, "bwidth").toInt() > 0)
	{
		setBottomWidth(getAttr(balise, "bwidth").toInt());
		useBottomBorder();
	}
	setLeftRed(getAttr(balise, "lred").toInt());
	setLeftGreen(getAttr(balise, "lgreen").toInt());
	setLeftBlue(getAttr(balise, "lblue").toInt());
	
	setRightRed(getAttr(balise, "rred").toInt());
	setRightGreen(getAttr(balise, "rgreen").toInt());
	setRightBlue(getAttr(balise, "rblue").toInt());
	
	setTopRed(getAttr(balise, "tred").toInt());
	setTopGreen(getAttr(balise, "tgreen").toInt());
	setTopBlue(getAttr(balise, "tblue").toInt());
	
	setBottomRed(getAttr(balise, "bred").toInt());
	setBottomGreen(getAttr(balise, "bgreen").toInt());
	setBottomBlue(getAttr(balise, "bblue").toInt());
	
	setLeftStyle(getAttr(balise, "lstyle").toInt());
	setRightStyle(getAttr(balise, "rstyle").toInt());
	setTopStyle(getAttr(balise, "tstyle").toInt());
	setBottomStyle(getAttr(balise, "bstyle").toInt());
	setBkRed(getAttr(balise, "bkred").toInt());
	setBkGreen(getAttr(balise, "bkgreen").toInt());
	setBkBlue(getAttr(balise, "bkblue").toInt());
/*
	setLeftWidth(getAttr(balise, "bleftpt").toInt());
	setLeftWidth(getAttr(balise, "brightpt").toInt());
	setLeftWidth(getAttr(balise, "bktoppt").toInt());
	setLeftWidth(getAttr(balise, "bkbottompt").toInt());
*/
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
			_lastTypeEnum == TL_NONE && getSection() != SS_FOOTNOTES &&
		getSection() != SS_HEADERS && getSection() != SS_FOOTERS)
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
		/* layout managment (left, center, justify, right) */
		if(!lastPara->isChapter() && _lastEnv != getNextEnv(_parags, _parags.at()) &&
				getSection() != SS_FOOTNOTES && getSection() != SS_HEADERS &&
				getSection() != SS_FOOTERS)
			lastPara->generateEndEnv(out);
		out << endl << endl;
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
	if(next->isList() && getSection() != SS_FOOTNOTES &&
		getSection() != SS_HEADERS && getSection() != SS_FOOTERS)
	{
		if(previous == 0 || !previous->isList() ||
			  (previous->isList() && (
				(previous->getCounterDepth() < next->getCounterDepth()) ||
				(previous->getCounterType() != next->getCounterType() &&
				previous->getCounterDepth() == next->getCounterDepth()))
			  ))
			return true;
	}
	return false;
}

/* next is the paragraph which will be generated in the next loop */
bool Texte::isCloseEnum(Para* previous, Para* next)
{
	if(previous->isList() && getSection() != SS_FOOTNOTES &&
		getSection() != SS_HEADERS && getSection() != SS_FOOTERS)
	{
		if(next == 0 || !next->isEnum() ||
			  (next->isList() && next->getCounterDepth() < previous->getCounterDepth()) ||
			  (next->isList() && next->getCounterType() != previous->getCounterType() &&
			  	next->getCounterDepth() == previous->getCounterDepth()) ||
			  previous->getFrameType() == SS_TABLE)
			return true;
	}
	return false;
}
