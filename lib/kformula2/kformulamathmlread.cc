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

MathMl2KFormula::MathMl2KFormula(QDomDocument *mmldoc)
{
    done=false;
    origdoc=mmldoc;
}
	
QDomDocument MathMl2KFormula::getKFormulaDom()
{
return formuladoc;
}
    
bool MathMl2KFormula::isDone()
{
    return done;
}
	

void MathMl2KFormula::startConversion()
{
    //TODO:let it be async
    done=false;
    formuladoc=QDomDocument("KFORMULA");
    QDomElement formula=formuladoc.createElement("FORMULA");
    processElement(origdoc,&formuladoc,&formula);
    formuladoc.appendChild(formula);
    cerr << formuladoc.toCString() << endl;
    done=true;
}

bool MathMl2KFormula::processElement(QDomNode *node,QDomDocument *doc,QDomNode *docnode)
{
    cerr << " called " <<endl;
    
    QDomElement *element;
    int type=1;  //1=unknown,2=Token,3=general layout,4=script and limit,5=tables,5=ee
    
    if(node->isElement()) {
	QDomElement e = node->toElement();
	element = &e;
	
        QString tag=element->tagName().lower();
        QString text=element->text();
	cerr << "process " << tag.utf8() <<endl;
	if(tag=="mi") {
	    text=text.stripWhiteSpace();	
            type=2;
        for(unsigned int i=0;i<text.length();i++)
                {
                QDomElement textelement=doc->createElement("TEXT");
        	textelement.setAttribute("CHAR",QString(text.at(i)));
        	cerr << "text element created" << endl;
		docnode->appendChild(textelement);
            }

        }
        else if(tag=="mo") {
	    text=text.stripWhiteSpace();	

            type=2;
    	    for(unsigned int i=0;i<text.length();i++)
                {
            	    QDomElement textelement=doc->createElement("TEXT"); //OPERATOR
        	    textelement.setAttribute("CHAR",QString(text.at(i)));
        	    docnode->appendChild(textelement);
        	}
	    }
        else if(tag=="mn") {
	    text=text.stripWhiteSpace();	
            type=2;
            for(unsigned int i=0;i<text.length();i++){
    	    	QDomElement textelement=doc->createElement("TEXT"); //NUMBER
                textelement.setAttribute("CHAR",QString(text.at(i)));
                docnode->appendChild(textelement);
    	    }
	}
        else if(tag=="mtext") {
            type=2;
            for(unsigned int i=0;i<text.length();i++) {
                QDomElement textelement=doc->createElement("TEXT");
                textelement.setAttribute("CHAR",QString(text.at(i)));
                docnode->appendChild(textelement);
            }
	}    
        else if(tag=="ms") {
	    type=2;
            for(unsigned int i=0;i<text.length();i++) {
                QDomElement textelement=doc->createElement("TEXT");
	        textelement.setAttribute("CHAR",QString(text.at(i)));
        	docnode->appendChild(textelement);
            }
        }
	else if(tag=="mrow") {
            type=3;
            QDomNode n=element->firstChild();
            while (!n.isNull()) {
               if (n.isElement()) {
                   QDomElement e = n.toElement();
                   processElement(&e,doc,docnode); //We do not allow sequence inside sequence
               }
               n = n.nextSibling();
            }
        }
	else if(tag=="mfrac") {
            type=3;
        QDomNode n=element->firstChild();
        QDomElement fraction=doc->createElement("FRACTION");

        int i=0;
        while (!n.isNull() && i<2) {
                if (n.isElement()) {
                i++;
            if(i==1) {  //first is numerator
		cerr << "Numerator" << endl;
                QDomElement numerator=doc->createElement("NUMERATOR");
                QDomElement sequence=doc->createElement("SEQUENCE");
                numerator.appendChild(sequence);
                QDomElement e = n.toElement();
                processElement(&e,doc,&sequence);
                fraction.appendChild(numerator);
        	cerr << "Numerator done" << endl;
                
	    } else
            {
		cerr << "denominator" << endl;
                QDomElement denominator=doc->createElement("DENOMINATOR");
                QDomElement sequence=doc->createElement("SEQUENCE");
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
        else if(tag=="mroot") {
            type=3;
        QDomNode n=element->firstChild();
        int i=0;
        QDomElement root=doc->createElement("ROOT");

        while (!n.isNull() && i<2) {
                if (n.isElement()) {
                i++;
            if(i==1) {  //first is content
                QDomElement content=doc->createElement("CONTENT");
                QDomElement sequence=doc->createElement("SEQUENCE");
                content.appendChild(sequence);
                QDomElement e = n.toElement();
                processElement(&e,doc,&sequence);
                
		root.appendChild(content);
            } else
            {
                QDomElement index=doc->createElement("INDEX");
                QDomElement sequence=doc->createElement("SEQUENCE");
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
        else if(tag=="msqrt") {
            type=3;
        QDomNode n=element->firstChild();
        QDomElement root=doc->createElement("ROOT");

        int i=0;
        while (!n.isNull() && i<1) {
                if (n.isElement()) {
                i++;
		cerr << "content" << endl;
                QDomElement content=doc->createElement("CONTENT");
                QDomElement sequence=doc->createElement("SEQUENCE");
                content.appendChild(sequence);
                QDomElement e = n.toElement();
                processElement(&e,doc,&sequence);
                root.appendChild(content);
        	cerr << "content done" << endl;
                
	       }
                n = n.nextSibling();
        	cerr << "next1" << endl;
        
	    }

          docnode->appendChild(root);    
	}
    }
    
    if(type==1) {  //Unknown
	QDomNode n=node->firstChild();
	while (!n.isNull()) {
	    processElement(&n,doc,docnode);
	    cerr << "next " << endl;
    	    n = n.nextSibling();
        
        }

    }
 
}
