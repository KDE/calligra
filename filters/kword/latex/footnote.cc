
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

#include <stdlib.h>		/* for atoi function */
#include <kdebug.h>		/* for kdDebug() stream */
#include "footnote.h"
#include "texte.h"		/* for generate function (catch text footnote) */

Footnote::Footnote(Para* para): Format(para)
{
}

Footnote::~Footnote()
{
	kdDebug() << "Destruction of a footnote." << endl;
}

/* Modifiers */
void Footnote::setSpace (QString new_space)
{
	_space = new_space;
}

void Footnote::setBefore(QString new_before)
{
	_before = new_before;

}

void Footnote::setAfter(QString new_after)
{
	_after = new_after;
}

void Footnote::setRef(QString new_ref)
{
	_ref = new_ref;
}

void Footnote::analyse(const QDomNode balise)
{
	/* MARKUPS FORMAT id="1" pos="0" len="17">...</FORMAT> */
	
	/* Parameters Analyse */
	kdDebug() << "ANALYSE A FOOTNOTE" << endl;
	
	/* Children Markups Analyse */
	for(int index= 0; index < getNbChild(balise); index++)
	{
		if(getChildName(balise, index).compare("INTERNAL")== 0)
		{
			kdDebug() << "INTERNAL : " << endl;
			analyseInternal(balise);
		}
		else if(getChildName(balise, index).compare("RANGE")== 0)
		{
			kdDebug() << "RANGE : " << endl;
			analyseRange(balise);
		}
		else if(getChildName(balise, index).compare("TEXT")== 0)
		{
			kdDebug() << "TEXT : " << endl;
			analyseText(balise);
		}
		else if(getChildName(balise, index).compare("DESCRIPT")== 0)
		{
			kdDebug() << "DESCRIPT : " << endl;
			analyseDescript(balise);
		}
		else if(getChildName(balise, index).compare("FORMAT")== 0)
		{
			kdDebug() << "SUBFORMAT : " << endl;
			Format::analyse(balise);
		}
	}
	kdDebug() << "END OF FOOTNOTE" << endl;
}

void Footnote::analyseInternal(const QDomNode balise)
{
	QDomNode fils;
	/* MARKUPS <INTERNAL> <PART from="1" to="-1" space="-"/> */

	/* Children Markups Analyse */
	fils = getChild(balise, "PART");
	for(int index= 0; index < getNbChild(balise); index++)
	{
		if(getChildName(balise, index).compare("PART")== 0)
		{
			kdDebug() << "PART : " << endl;
			setFrom(getAttr(balise, "FROM").toInt());
			setTo(getAttr(balise, "TO").toInt());
			setSpace(getAttr(balise, "SPACE"));

		}
	}
}

void Footnote::analyseRange(const QDomNode balise)
{
	kdDebug() << "PARAM" << endl;
	setStart(getAttr(balise, "START").toInt());
	setEnd(getAttr(balise, "END").toInt());
}

void Footnote::analyseText(const QDomNode balise)
{
	kdDebug() << "PARAM" << endl;
	setBefore(getAttr(balise, "BEFORE"));
	setAfter(getAttr(balise, "AFTER"));
}

void Footnote::analyseDescript(const QDomNode balise)
{
	kdDebug() << "PARAM" << endl;
	setRef(getAttr(balise, "REF"));
}

void Footnote::generate(QTextStream &out)
{
	Para *footnote = 0;

	kdDebug() << "  GENERATION FOOTNOTE" << endl;
	// Go to keep the footnote parag.
	// then write it with this format.
	// like this : \,\footnote{the parag. }
	out << "\\,\\footnote{";
	kdDebug() << "footnote : " << _ref << endl;
	if((footnote = getPara()->getTexte()->searchFootnote(_ref)) != 0)
		footnote->generate(out);
	out << "}";
	kdDebug() << "FOOTNOTE GENERATED" << endl;
}


