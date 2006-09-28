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
** Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
**
*/

#include <stdlib.h>		/* for atoi function */
#include <kdebug.h>		/* for kDebug() stream */
#include <q3ptrstack.h>		/* for getFormula() */
#include <qdom.h>
//Added by qt3to4:
#include <QTextStream>
#include "formula.h"
#include <kapplication.h>

#include <kformuladocument.h>
#include <kformulamimesource.h>

/*******************************************/
/* Constructor                             */
/*******************************************/
Formula::Formula()
{
	_left              = 0;
	_right             = 0;
	_top               = 0;
	_bottom            = 0;
	_runaround         = TA_NONE;
	_runaroundGap      = 0;
	_autoCreate        = TC_EXTEND;
	_newFrameBehaviour = TF_RECONNECT;

}

/*******************************************/
/* analyze                                 */
/*******************************************/
void Formula::analyze(const QDomNode balise)
{

	/* MARKUP TYPE : FRAMESET INFO = TEXTE, ENTETE CONNUE */

	/* Parameter analysis */
	Element::analyze(balise);

	kDebug(30522) << "FRAME ANALYSIS (Formula)" << endl;

	/* Child markup analysis */
	for(int index= 0; index < getNbChild(balise); index++)
	{
		if(getChildName(balise, index).compare("FRAME")== 0)
		{
			analyzeParamFrame(balise);
		}
		else if(getChildName(balise, index).compare("FORMULA")== 0)
		{
			getFormula(getChild(getChild(balise, "FORMULA"), "FORMULA"), 0);
			kDebug(30522) << _formula << endl;
		}

	}
	kDebug(30522) << "END OF A FRAME" << endl;
}

/*******************************************/
/* getFormula                              */
/*******************************************/
/* Get back the xml markup tree.           */
/*******************************************/
void Formula::getFormula(QDomNode p, int indent)
{
/*	while( p.)
	{*/
		switch( p.nodeType() )
		{
			case QDomNode::TextNode:
				_formula = _formula + QString(p.toText().data()) + " ";
				break;
		/*	case TT_Space:
				_formula = _formula + p->zText;
				//printf("%*s\"%s\"\n", indent, "", p->zText);
				break;
			case TT_EOL:
				_formula = _formula + "\n";
				//printf("%*s\n", indent, "");
				break;*/
			case QDomNode::ElementNode:
				_formula = _formula + "<" + p.nodeName();
				QDomNamedNodeMap attr = p.attributes();
				for(unsigned int index = 0; index < attr.length(); index++)
				{ // The attributes
					_formula = _formula + " " + attr.item(index).nodeName();
					_formula = _formula + "=\"" + attr.item(index).nodeValue() + "\"";
				}
				if(p.childNodes().length() == 0)
					_formula = _formula + "/>\n";
				else
				{
					_formula = _formula + ">\n";
					QDomNodeList child = p.childNodes();
					for(unsigned int index = 0; index < child.length(); index++)
					{
						getFormula(child.item(index), indent+3); // The child elements
					}
					_formula = _formula + "</" + p.nodeName() + ">\n";
				}
				break;
			/*default:
				kError(30522) << "Can't happen" << endl;
				break;*/
		}
	/*	p = p.nextSibling();
	}*/
}

/*******************************************/
/* analyzeParamFrame                       */
/*******************************************/
void Formula::analyzeParamFrame(const QDomNode balise)
{
	/*<FRAME left="28" top="42" right="566" bottom="798" runaround="1" />*/

	_left = getAttr(balise, "left").toInt();
	_top = getAttr(balise, "top").toInt();
	_right = getAttr(balise, "right").toInt();
	_bottom = getAttr(balise, "bottom").toInt();
	setRunAround(getAttr(balise, "runaround").toInt());
	setAroundGap(getAttr(balise, "runaroundGap").toInt());
	setAutoCreate(getAttr(balise, "autoCreateNewFrame").toInt());
	setNewFrame(getAttr(balise, "newFrameBehaviour").toInt());
	setSheetSide(getAttr(balise, "sheetside").toInt());
}

/*******************************************/
/* generate                                */
/*******************************************/
void Formula::generate(QTextStream &out)
{
	kDebug(30522) << "FORMULA GENERATION" << endl;
	QDomDocument doc;
	doc.setContent(_formula);

   	// a new KFormula::Document for every formula is not the best idea.
	// better to have only one such beast for the whole document.
	KFormula::Document formulaDoc( kapp->sessionConfig() );

	KFormula::Container* formula = new KFormula::Container( &formulaDoc );
	if ( !formula->load( doc ) ) {
		kError(30522) << "Failed." << endl;
	}

 	out << "$" << formula->texString() << "$";
        delete formula;
}

