/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <iostream>
#include <qstring.h>
#include "kformulamathmlread.h"

KFORMULA_NAMESPACE_BEGIN
using namespace std;


MathMl2KFormula::MathMl2KFormula(QDomDocument * mmldoc)
{
    done = false;
    origdoc = mmldoc;
}

QDomDocument MathMl2KFormula::getKFormulaDom()
{
    return formuladoc;
}



void MathMl2KFormula::startConversion()
{
    //TODO:let it be async
    done = false;
    formuladoc = QDomDocument("KFORMULA");
    QDomElement formula = formuladoc.createElement("FORMULA");
    processElement(origdoc,&formuladoc,&formula);
    formuladoc.appendChild(formula);
    cerr << formuladoc.toCString() << endl;
    done = true;
}

bool MathMl2KFormula::processElement(QDomNode * node,QDomDocument * doc,
				     QDomNode * docnode)
{

    QDomElement *element;
    int type = 1;		//1=unknown,2=Token,3=general layout,4=script and limit,5=tables,5=ee

    if (node->isElement()) {
	QDomElement e = node->toElement();
	element = &e;

	QString tag = element->tagName().lower();
	QString text = element->text();
	if (tag == "mi") {
	    text = text.stripWhiteSpace();
	    type = 2;
	    for (unsigned int i = 0; i < text.length(); i++) {
		QDomElement textelement = doc->createElement("TEXT");
		textelement.setAttribute("CHAR",QString(text.at(i)));
		docnode->appendChild(textelement);
	    }

	}
	else if (tag == "mo") {
	    text = text.stripWhiteSpace();

	    type = 2;
	    for (unsigned int i = 0; i < text.length(); i++) {
		QDomElement textelement = doc->createElement("TEXT");	//OPERATOR
		textelement.setAttribute("CHAR",QString(text.at(i)));
		docnode->appendChild(textelement);
	    }
	}
	else if (tag == "mn") {
	    text = text.stripWhiteSpace();
	    type = 2;
	    for (unsigned int i = 0; i < text.length(); i++) {
		QDomElement textelement = doc->createElement("TEXT");	//NUMBER
		textelement.setAttribute("CHAR",QString(text.at(i)));
		docnode->appendChild(textelement);
	    }
	}
	else if (tag == "mtext") {
	    type = 2;
	    for (unsigned int i = 0; i < text.length(); i++) {
		QDomElement textelement = doc->createElement("TEXT");
		textelement.setAttribute("CHAR",QString(text.at(i)));
		docnode->appendChild(textelement);
	    }
	}
	else if (tag == "ms") {
	    type = 2;
	    for (unsigned int i = 0; i < text.length(); i++) {
		QDomElement textelement = doc->createElement("TEXT");
		textelement.setAttribute("CHAR",QString(text.at(i)));
		docnode->appendChild(textelement);
	    }
	}
	else if (tag == "mrow") {
	    type = 3;
	    QDomNode n = element->firstChild();
	    while (!n.isNull()) {
		if (n.isElement()) {
		    QDomElement e = n.toElement();
		    processElement(&e,doc,docnode);	//We do not allow sequence inside sequence
		}
		n = n.nextSibling();
	    }
	}
	else if (tag == "mfrac") {
	    type = 3;
	    QDomNode n = element->firstChild();
	    QDomElement fraction = doc->createElement("FRACTION");

	    int i = 0;
	    while (!n.isNull() && i < 2) {
		if (n.isElement()) {
		    i++;
		    if (i == 1) {	//first is numerator
			QDomElement numerator =
			    doc->createElement("NUMERATOR");
			QDomElement sequence = doc->createElement("SEQUENCE");
			numerator.appendChild(sequence);
			QDomElement e = n.toElement();
			processElement(&e,doc,&sequence);
			fraction.appendChild(numerator);

		    }
		    else {
			QDomElement denominator =
			    doc->createElement("DENOMINATOR");
			QDomElement sequence = doc->createElement("SEQUENCE");
			denominator.appendChild(sequence);
			QDomElement e = n.toElement();
			processElement(&e,doc,&sequence);
			fraction.appendChild(denominator);

		    }
		}
		n = n.nextSibling();
	    }
	    docnode->appendChild(fraction);
	}
	else if (tag == "mroot") {
	    type = 3;
	    QDomNode n = element->firstChild();
	    int i = 0;
	    QDomElement root = doc->createElement("ROOT");

	    while (!n.isNull() && i < 2) {
		if (n.isElement()) {
		    i++;
		    if (i == 1) {	//first is content
			QDomElement content = doc->createElement("CONTENT");
			QDomElement sequence = doc->createElement("SEQUENCE");
			content.appendChild(sequence);
			QDomElement e = n.toElement();
			processElement(&e,doc,&sequence);

			root.appendChild(content);
		    }
		    else {
			QDomElement index = doc->createElement("INDEX");
			QDomElement sequence = doc->createElement("SEQUENCE");
			index.appendChild(sequence);
			QDomElement e = n.toElement();
			processElement(&e,doc,&sequence);
			root.appendChild(index);

		    }
		}
		n = n.nextSibling();
	    }
	    docnode->appendChild(root);
	}
	else if (tag == "msqrt") {
	    type = 3;
	    QDomNode n = element->firstChild();
	    QDomElement root = doc->createElement("ROOT");

	    int i = 0;
	    while (!n.isNull() && i < 1) {
		if (n.isElement()) {
		    i++;
		    QDomElement content = doc->createElement("CONTENT");
		    QDomElement sequence = doc->createElement("SEQUENCE");
		    content.appendChild(sequence);
		    QDomElement e = n.toElement();
		    processElement(&e,doc,&sequence);
		    root.appendChild(content);

		}
		n = n.nextSibling();

	    }

	    docnode->appendChild(root);
	}


	else if (tag == "mtable") {
	    type = 5;
	    QString subtag;
	    int rows=0,cols=0;
	    QDomNode n = element->firstChild();


	    while (!n.isNull()) {
		if (n.isElement()) {
		    QDomElement e = n.toElement();
            	    subtag = e.tagName().lower();
		    if (subtag == "mtr")
			{
			     rows++;

/* Determins the number of columns */

    			    QDomNode cellnode = e.firstChild();
    			    int cc=0;

			    while (!cellnode.isNull()) {
				if (cellnode.isElement())
				    cc++;

				cellnode = cellnode.nextSibling();
			    }

			    if(cc>cols) cols=cc;

			}
		}
		n = n.nextSibling();
	    }


/*Again createing elements, I need to know the number of rows and cols to leave emty spaces*/


	    n = element->firstChild();
	    QDomElement matrix = doc->createElement("MATRIX");
	    matrix.setAttribute("COLUMNS",cols);
	    matrix.setAttribute("ROWS",rows);


	    while (!n.isNull()) {
		if (n.isElement()) {
		    QDomElement e = n.toElement();
            	    subtag = e.tagName().lower();
		    if (subtag == "mtr") {

    			QDomNode cellnode = e.firstChild();
			int cc=0;
			while (!cellnode.isNull()) {
			    if (cellnode.isElement()) {
			        cc++;
				QDomElement cell = doc->createElement("SEQUENCE");
				QDomElement cellelement = cellnode.toElement();
		  		processElement(&cellelement,doc,&cell);
				matrix.appendChild(cell);

			    }
			    cellnode = cellnode.nextSibling();
			}


			/* Add empty elements */
			for(;cc<cols;cc++) {
			    QDomElement cell = doc->createElement("SEQUENCE");
			    matrix.appendChild(cell);
			}



		    }
		}
		n = n.nextSibling();
	    }












	    docnode->appendChild(matrix);
	}



	else if (tag == "msup" || tag == "msub") {
	    type = 3;
	    QDomNode n = element->firstChild();
	    int i = 0;
	    QDomElement root = doc->createElement("INDEX");

	    while (!n.isNull() && i < 2) {
		if (n.isElement()) {
		    i++;
		    if (i == 1) {	//first is content
			QDomElement content = doc->createElement("CONTENT");
			QDomElement sequence = doc->createElement("SEQUENCE");
			content.appendChild(sequence);
			QDomElement e = n.toElement();
			processElement(&e,doc,&sequence);

			root.appendChild(content);
		    }
		    else {

			QDomElement index;
			if (tag == "msup")
			    index = doc->createElement("UPPERRIGHT");
			else
			    index = doc->createElement("LOWERRIGHT");

			QDomElement sequence = doc->createElement("SEQUENCE");
			index.appendChild(sequence);
			QDomElement e = n.toElement();
			processElement(&e,doc,&sequence);
			root.appendChild(index);

		    }
		}
		n = n.nextSibling();
	    }
	    docnode->appendChild(root);
	}

	else if (tag == "munder" || tag == "mover") {
	    type = 3;
	    QDomNode n = element->firstChild();
	    int i = 0;
	    QDomElement root = doc->createElement("INDEX");

	    while (!n.isNull() && i < 2) {
		if (n.isElement()) {
		    i++;
		    if (i == 1) {	//first is content
			QDomElement content = doc->createElement("CONTENT");
			QDomElement sequence = doc->createElement("SEQUENCE");
			content.appendChild(sequence);
			QDomElement e = n.toElement();
			processElement(&e,doc,&sequence);

			root.appendChild(content);
		    }
		    else {

			QDomElement index;
			if (tag == "mover")
			    index = doc->createElement("UPPERMIDDLE");
			else
			    index = doc->createElement("LOWERMIDDLE");

			QDomElement sequence = doc->createElement("SEQUENCE");
			index.appendChild(sequence);
			QDomElement e = n.toElement();
			processElement(&e,doc,&sequence);
			root.appendChild(index);

		    }
		}
		n = n.nextSibling();
	    }
	    docnode->appendChild(root);
	}
	else if (tag == "msubsup") {
	    type = 3;
	    QDomNode n = element->firstChild();
	    int i = 0;
	    QDomElement root = doc->createElement("INDEX");

	    while (!n.isNull() && i < 2) {
		if (n.isElement()) {
		    i++;
		    if (i == 1) {	//first is content
			QDomElement content = doc->createElement("CONTENT");
			QDomElement sequence = doc->createElement("SEQUENCE");
			content.appendChild(sequence);
			QDomElement e = n.toElement();
			processElement(&e,doc,&sequence);

			root.appendChild(content);
		    }
		    else if (i == 2) {

			QDomElement index;
			index = doc->createElement("LOWERRIGHT");

			QDomElement sequence = doc->createElement("SEQUENCE");
			index.appendChild(sequence);
			QDomElement e = n.toElement();
			processElement(&e,doc,&sequence);
			root.appendChild(index);
		    }
		    else {
			QDomElement index;
			index = doc->createElement("UPPERRIGHT");

			QDomElement sequence = doc->createElement("SEQUENCE");
			index.appendChild(sequence);
			QDomElement e = n.toElement();
			processElement(&e,doc,&sequence);
			root.appendChild(index);
		    }

		}
		n = n.nextSibling();
	    }
	    docnode->appendChild(root);
	}

    }

    if (type == 1) {		//Unknown
	QDomNode n = node->firstChild();
	while (!n.isNull()) {
	    processElement(&n,doc,docnode);
	    n = n.nextSibling();

	}

    }
    return true;
}

KFORMULA_NAMESPACE_END

using namespace KFormula;
#include "kformulamathmlread.moc"
