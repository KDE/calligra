/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

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

#include <qpopupmenu.h>
#include <qbuffer.h>
#include <qcolor.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpixmap.h>

#include <kcommand.h>

#include "contextstyle.h"
#include "formulacursor.h"
#include "formulaelement.h"
#include "kformulamimesource.h"


using namespace std;


KFormulaMimeSource::KFormulaMimeSource(QDomDocument formula)
        : document(formula)
{
    // The query for text/plain comes very often. So make sure
    // it's fast.

    rootElement = new FormulaElement(this);
    FormulaCursor cursor(rootElement);

    QList<BasicElement> list;
    list.setAutoDelete(true);
    if (cursor.buildElementsFromDom(document, list)) {
        cursor.insert(list);
        latexString = rootElement->toLatex().utf8();
        if (latexString.size() > 0) {
            latexString.truncate(latexString.size()-1);
        }
    }
}

KFormulaMimeSource::~KFormulaMimeSource()
{
    delete rootElement;
}


const char* KFormulaMimeSource::format( int n ) const
{
    switch (n) {
        case 0:
            return "application/x-kformula";
        case 1:
            return "image/ppm";
        case 2:
            return "text/plain";
        case 3:
            return "text/x-tex";
    }
    return NULL;
}

bool KFormulaMimeSource::provides( const char * format) const
{
//This is not completed
    if(QString(format)=="application/x-kformula")
        return true;
    else if(QString(format)=="image/ppm")
        return true;
    else if(QString(format)=="text/plain")
        return true;
    else if(QString(format)=="text/x-tex")
        return true;
    else
        return false;
}

QByteArray KFormulaMimeSource::encodedData ( const char *format ) const
{
    QString fmt=format;  //case sensitive?

    if ((fmt=="text/plain") || (fmt=="text/x-tex"))
        return latexString;

    if (fmt=="application/x-kformula") {
	QByteArray d=document.toCString();
  	d.truncate(d.size()-1);
	return d;
    }

    if (fmt=="image/ppm") {

	//cerr << "asking image" << endl;
        ContextStyle context;
        context.setResolution(5, 5);

        rootElement->calcSizes(context);
        QRect rect(rootElement->getX(), rootElement->getY(),
                   rootElement->getWidth(), rootElement->getHeight());

    	QPixmap pm(rect.width(),rect.height());
	pm.fill();
	QPainter paint(&pm);
        rootElement->draw(paint, rect, context);
	paint.end();

	QByteArray d;
	QBuffer buff(d);
	buff.open(IO_WriteOnly);
	QImageIO io(&buff,"PPM");
	QImage ima=pm.convertToImage();
	ima.detach();
	io.setImage(ima);
	if(!io.write())
	    return QByteArray();

	buff.close();
    	return d;
    }

    return QByteArray();
}

void KFormulaMimeSource::elementRemoval(BasicElement*)
{
}

void KFormulaMimeSource::changed()
{
}

const SymbolTable& KFormulaMimeSource::getSymbolTable() const
{
    return table;
}
