/* This file is part of the KDE project
   Copyright (C) 2002 Alexander Dymo <cloudtemple@mksat.net>

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

#include "kudesigner_part.h"
#include "kudesigner_factory.h"
#include "kudesigner_view.h"

#include <qpainter.h>
#include <qprinter.h>
#include <qpaintdevicemetrics.h>

#include "mycanvas.h"

KudesignerPart::KudesignerPart( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode )
    : KoDocument( parentWidget, widgetName, parent, name, singleViewMode )
{
    setInstance( KudesignerFactory::global(), false );
    history = new KCommandHistory(actionCollection());

    connect( history, SIGNAL( documentRestored() ), this, SLOT( slotDocumentRestored() ) );
    connect( history, SIGNAL( commandExecuted() ), this, SLOT( slotCommandExecuted() ) );
}

KudesignerPart::~KudesignerPart()
{
    delete history;
}

bool KudesignerPart::initDoc()
{
    // If nothing is loaded, do initialize here
    bool ok = FALSE;

    int top = 20;
    int bottom = 20;
    int left = 20;
    int right = 20;
    bool orientation = 0;
    int pageSize = 1;

    docCanvas = new MyCanvas(210, 297);
  	int height = 297;
  	int width = 210;
//  	height = pSize.height();
//  	width = pSize.width();
  	if (orientation)
  	{
  	    int temp = height;
  	    height = width;
  	    width = temp;
  	}

  	//zooming canvas according to screen resolution
  	QPrinter* printer;

  	// Set the page size
  	printer = new QPrinter();
  	printer->setFullPage(true);
  	printer->setPageSize((QPrinter::PageSize)pageSize);
  	printer->setOrientation((QPrinter::Orientation)orientation);

  	// Get the page metrics and set appropriate wigth and height
  	QPaintDeviceMetrics pdm(printer);
  	width = pdm.width();
  	height = pdm.height();

  	//this is not needed anymore
  	delete printer;


   	//creating canvas
  	docCanvas = new MyCanvas(width, height);
  	docCanvas->setAdvancePeriod(30);

  	//creating KugarTemplate object
  	CanvasKugarTemplate *templ = new CanvasKugarTemplate(0, 0, width, height, docCanvas);
  	templ->show();
  	templ->props["PageSize"].first = QString("%1").arg(pageSize);
  	templ->props["PageOrientation"].first = QString("%1").arg(orientation);
  	templ->props["TopMargin"].first = QString("%1").arg(top);
  	templ->props["BottomMargin"].first = QString("%1").arg(bottom);
  	templ->props["LeftMargin"].first = QString("%1").arg(left);
  	templ->props["RightMargin"].first = QString("%1").arg(right);

/*    modified=true;
    doc_url.setFileName(i18n("Untitled"));*/


    return TRUE;
}

KoView* KudesignerPart::createViewInstance( QWidget* parent, const char* name )
{
    return new KudesignerView( this, parent, name );
}

bool KudesignerPart::loadXML( QIODevice *, const QDomDocument & )
{
    // TODO load the document from the QDomDocument
    return true;
}

QDomDocument KudesignerPart::saveXML()
{
    // TODO save the document into a QDomDocument
    return QDomDocument();
}


void KudesignerPart::paintContent( QPainter& painter, const QRect& rect, bool /*transparent*/,
                                double /*zoomX*/, double /*zoomY*/ )
{
    // ####### handle transparency

    // Need to draw only the document rectangle described in the parameter rect.

    canvas()->drawArea(rect, &painter, TRUE);

/*    int left = rect.left() / 20;
    int right = rect.right() / 20 + 1;
    int top = rect.top() / 20;
    int bottom = rect.bottom() / 20 + 1;

    for( int x = left; x < right; ++x )
        painter.drawLine( x * 20, top * 20, x * 20, bottom * 20 );
    for( int y = left; y < right; ++y )
        painter.drawLine( left * 20, y * 20, right * 20, y * 20 );*/
}

void KudesignerPart::commandExecuted()
{
    setModified(true);
}

void KudesignerPart::documentRestored()
{
    setModified(false);
}

MyCanvas *KudesignerPart::canvas()
{
    return docCanvas;
}

#include "kudesigner_part.moc"
