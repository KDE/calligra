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
	_footnotes         = 0;

	setType(ST_TEXT);
	setSection(SS_BODY);
}

/*******************************************/
/* hasColor                                */
/*******************************************/
/* Return TRUE if there is at least one    */
/* parag. which use color.                 */
/*******************************************/
/*bool Texte::hasColor() const
{
	bool color;
	ParaIter iter;

	color = false;
	iter.setList(_parags);
	kdDebug() << "TEST COLOR USE" << endl;
	while(!iter.isTerminate() && !color)
	{
		color = iter.getCourant()->isColored();
		kdDebug() << "coul" << color << endl;
		iter.next();
	}
	return color;
}*/

/*******************************************/
/* hasUline                                */
/*******************************************/
/* Return TRUE if there is at least one    */
/* parag. which use underline.             */
/*******************************************/
/*bool Texte::hasUline() const
{
	bool uline;
	ParaIter iter;

	uline = false;
	iter.setList(_parags);
	kdDebug() << "TEST ULINE USE" << endl;
	while(!iter.isTerminate() && !uline)
	{
		uline = iter.getCourant()->isUlined();
		iter.next();
	}
	return uline;
}*/

/*******************************************/
/* searchFootnote                          */
/*******************************************/
Para* Texte::searchFootnote(const QString name)
{
	ParaIter iter;

	if(_footnotes != 0)
	{
		iter.setList(*_footnotes);
		while(!iter.isTerminate())
		{
			QString* string = iter.getCourant()->getName();
			kdDebug() << *string << endl;
			if(*string == name)
				return iter.getCourant();
			iter.next();
		}
	}
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
				if(_footnotes == 0)
					_footnotes = new ListPara;
				_footnotes->add(prg);
			}
			else
			{
				// 3. add this parag. in the text list
				_parags.add(prg);
			}
			kdDebug() << "PARA ADDED" << endl;
		}
		
	}
	kdDebug() << "END OF A FRAME" << endl;
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
			_left = atoi(arg->zValue);
		}
		else if(strcmp(arg->zName, "TOP")== 0)
		{
			_top = atoi(arg->zValue);
		}
		else if(strcmp(arg->zName, "RIGHT")== 0)
		{
			_right = atoi(arg->zValue);
		}
		else if(strcmp(arg->zName, "BOTTOM")== 0)
		{
			_bottom = atoi(arg->zValue);
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
	}
}

/*******************************************/
/* generate                                */
/*******************************************/
void Texte::generate(QTextStream &out)
{
	ParaIter iter;
	kdDebug() << "TEXT GENERATION" << endl;
	kdDebug() << "NB PARA " << _parags.getSize() << endl;
	iter.setList(_parags);
	while(!iter.isTerminate())
	{
		//iter.getCourant()->setFrameType(getSection());
		iter.getCourant()->generate(out);
		iter.next();
		kdDebug() << iter.getCourant() << endl;
	}
}

