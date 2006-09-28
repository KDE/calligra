/*
** A program to convert the XML rendered by KWord into LATEX.
**
** Copyright (C) 2000, 2001, 2002 Robert JACOLIN
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
** Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
**
*/

#include <stdlib.h>		/* for atoi function */
#include <kdebug.h>		/* for kDebug() stream */
#include "textFrame.h"
//Added by qt3to4:
#include <QTextStream>
#include <Q3PtrList>

/*******************************************/
/* Constructor                             */
/*******************************************/
TextFrame::TextFrame()
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
/*Para* TextFrame::searchFootnote(const QString name)
{
	for(Para* current = _footnotes.first(); current!= 0; current = _footnotes.next())
	{
		QString* string = current->getName();
		kDebug(30522) << *string << endl;
		if(*string == name)
			return current;
		//iter.next();
	}
	return 0;
}*/

/*******************************************/
/* analyze                                 */
/*******************************************/
void TextFrame::analyze(const QDomNode node)
{
	/* Markup type: Frameset info = text, heading known */

	/* Parameter analysis */
	Element::analyze(node);

	kDebug(30522) << "FRAME ANALYSIS (TextFrame)" << endl;

	/* Child markup analysis */
	analyzeParamFrame(getChild(node, "FRAME"));

	for(int index = 0; index < getNbChild(node); index++)
	{
		if(getChildName(node, index).compare("PARAGRAPH")== 0)
		{
			// 1. Create a paragraph :
			Para *prg = new Para(this);
			// 2. Add the information :
			prg->analyze(getChild(node, index));
			if(prg->getInfo() == EP_FOOTNOTE)
			{
				// 3. add this parag. in the footnote list
				//if(_footnotes == 0)
				//	_footnotes = new ListPara;
				//_footnotes.append(prg);
			}
			else
			{
				// 3. add this parag. in the text list
				_parags.append(prg);
			}
			kDebug(30522) << "PARA ADDED" << endl;
		}

	}
	kDebug(30522) << "END OF A FRAME ANALYSIS" << endl;
}

/*******************************************/
/* analyzeParamFrame                       */
/*******************************************/
void TextFrame::analyzeParamFrame(const QDomNode node)
{
	/*<FRAME left="28" top="42" right="566" bottom="798" runaround="1" />*/
	setLeft(getAttr(node, "left").toDouble());
	setTop(getAttr(node, "top").toDouble());
	setRight(getAttr(node, "right").toDouble());
	setBottom(getAttr(node, "bottom").toDouble());
	setRunAround(getAttr(node, "runaround").toInt());
	setAroundGap(getAttr(node, "runaroundGap").toInt());
	setAutoCreate(getAttr(node, "autoCreateNewFrame").toInt());
	setNewFrame(getAttr(node, "newFrameBehaviour").toInt());
	setSheetSide(getAttr(node, "sheetSide").toInt());
	if(getAttr(node, "lWidth").toInt() > 0)
	{
		setLeftWidth(getAttr(node, "lWidth").toInt());
		useLeftBorder();
	}
	if(getAttr(node, "rWidth").toInt() > 0)
	{
		setRightWidth(getAttr(node, "rWidth").toInt());
		useRightBorder();
	}
	if(getAttr(node, "tWidth").toInt() > 0)
	{
		setTopWidth(getAttr(node, "tWidth").toInt());
		useTopBorder();
	}
	if(getAttr(node, "bWidth").toInt() > 0)
	{
		setBottomWidth(getAttr(node, "bWidth").toInt());
		useBottomBorder();
	}
	setLeftRed(getAttr(node, "lRed").toInt());
	setLeftGreen(getAttr(node, "lGreen").toInt());
	setLeftBlue(getAttr(node, "lBlue").toInt());

	setRightRed(getAttr(node, "rRed").toInt());
	setRightGreen(getAttr(node, "rGreen").toInt());
	setRightBlue(getAttr(node, "rBlue").toInt());

	setTopRed(getAttr(node, "tRed").toInt());
	setTopGreen(getAttr(node, "tGreen").toInt());
	setTopBlue(getAttr(node, "tBlue").toInt());

	setBottomRed(getAttr(node, "bRed").toInt());
	setBottomGreen(getAttr(node, "bGreen").toInt());
	setBottomBlue(getAttr(node, "bBlue").toInt());

	setLeftStyle(getAttr(node, "lStyle").toInt());
	setRightStyle(getAttr(node, "rStyle").toInt());
	setTopStyle(getAttr(node, "tStyle").toInt());
	setBottomStyle(getAttr(node, "bStyle").toInt());
	setBkRed(getAttr(node, "bkred").toInt());
	setBkGreen(getAttr(node, "bkgreen").toInt());
	setBkBlue(getAttr(node, "bkblue").toInt());
/*
	setLeftWidth(getAttr(node, "bleftpt").toDouble());
	setLeftWidth(getAttr(node, "brightpt").toDouble());
	setLeftWidth(getAttr(node, "bktoppt").toDouble());
	setLeftWidth(getAttr(node, "bkbottompt").toDouble());
*/
}

/*******************************************/
/* generate                                */
/*******************************************/
void TextFrame::generate(QTextStream &out)
{
	Para * lastPara = 0;

	kDebug(30522) << "TEXT GENERATION" << endl;
	kDebug(30522) << "NB PARA " << _parags.count() << endl;

	if(getSection() == SS_TABLE || getSection() == SS_HEADERS ||
	   getSection() == SS_FOOTERS)
	{
		Config::instance()->writeIndent(out);
		out << "\\begin{minipage}{";
		out << (getRight() - getLeft()) << "pt}" << endl;
	}
	_lastEnv = ENV_NONE;
	_lastTypeEnum = TL_NONE;

	Para* currentPara = _parags.first();
	while( currentPara != 0)
	{
		//indent();
		if((!currentPara->isChapter() && _lastTypeEnum == TL_NONE &&
			_lastEnv != getNextEnv(_parags, _parags.at()) &&
			currentPara->notEmpty()) ||
			_lastEnv != getNextEnv(_parags, _parags.at()) )
		{
			currentPara->generateBeginEnv(out);
			_lastEnv = currentPara->getEnv();
		}

		/* List management */
		if(isBeginEnum(lastPara, currentPara))
		{
			currentPara->openList(out);
			_lastTypeEnum = currentPara->getCounterType();
		}
		/* paragraph generation */
		currentPara->generate(out);

		lastPara = currentPara;
		currentPara = _parags.next();

		/* list management */
		if(isCloseEnum(lastPara, currentPara))
		{
			lastPara->closeList(out, currentPara);
			_lastTypeEnum = TL_NONE;
		}
		/* layout management (left, center, justify, right) */
		if((!lastPara->isChapter() && _lastEnv != getNextEnv(_parags, _parags.at()) &&
			lastPara->notEmpty()) ||
			_lastEnv != getNextEnv(_parags, _parags.at()))
		{
			lastPara->generateEndEnv(out);
			out << endl;
		}
		if(getSection() != SS_HEADERS && getSection() != SS_FOOTERS)
			out << endl;
	}

	if(getSection() == SS_TABLE || getSection() == SS_HEADERS ||
	   getSection() == SS_FOOTERS)
	{
		Config::instance()->desindent();
		Config::instance()->writeIndent(out);
		out << "\\end{minipage}" << endl;
	}
}

EEnv TextFrame::getNextEnv(Q3PtrList<Para> liste, const int pos)
{
	if ( pos < 0 )
		return ENV_NONE;
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
bool TextFrame::isBeginEnum(Para* previous, Para* next)
{
	/* If it's a list : */
	/* - go in a new list */
	/* - change depth (a list in a list) */
	/* - or two lists nearby (but with the same depth) */
	kDebug(30522) << "---------------------------------" << endl;
	kDebug(30522) << getSection() << " = " << SS_HEADERS << endl;
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
bool TextFrame::isCloseEnum(Para* previous, Para* next)
{
	if(previous->isList() && getSection() != SS_FOOTNOTES &&
		getSection() != SS_HEADERS && getSection() != SS_FOOTERS)
	{
		if(next == 0 || !next->isList() ||
			  (next->isList() && next->getCounterDepth() < previous->getCounterDepth()) ||
			  (next->isList() && next->getCounterType() != previous->getCounterType() &&
			  	next->getCounterDepth() == previous->getCounterDepth()) ||
			  previous->getFrameType() == SS_TABLE)
			return true;
	}
	return false;
}
