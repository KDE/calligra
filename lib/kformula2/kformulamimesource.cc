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

#include <qbuffer.h>
#include <qcolor.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpixmap.h>

#include <kcommand.h>

#include "formulacursor.h"
#include "kformulacontainer.h"
#include "kformuladocument.h"
#include "kformulamimesource.h"


KFormulaMimeSource::KFormulaMimeSource(QDomDocument formula)
{
    document=formula;
}

const char* KFormulaMimeSource::format ( int n ) const
{
    switch (n)
    {
     case 0:
      return "application/x-kformula";
    }
    return NULL;
}
bool KFormulaMimeSource::provides ( const char * format) const
{
//This is not completed
    if(QString(format)=="application/x-kformula")
     return true;
    else
    if(QString(format)=="image/ppm")
     return true;
    else
     return false; 
     
}

QByteArray KFormulaMimeSource::encodedData ( const char *format ) const
{
    if(QString(format)=="application/x-kformula")
	return document.toCString();
    else 
    if(QString(format)=="image/ppm") {
        KFormulaDocument document;
        KFormulaContainer tmpContainer(&document);
	FormulaCursor *c=tmpContainer.createCursor();
        tmpContainer.setActiveCursor(c);
        tmpContainer.paste();
        delete c;
        c = 0;
	QRect rect=tmpContainer.boundingRect();
    	QPixmap pm(rect.width(),rect.height());
	pm.fill();
	QPainter paint;
	paint.begin(&pm);
	tmpContainer.draw(paint, rect);
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
    else 
	return QByteArray();
}

