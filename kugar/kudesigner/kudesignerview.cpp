/***************************************************************************
                          kudesignerview.cpp  -  description
                             -------------------
    begin                : Thu Jun  6 11:31:39 EEST 2002
    copyright            : (C) 2002 by Alexander Dymo
    email                : cloudtemple@mksat.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// include files for Qt
#include <qprinter.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>

// include files for KDE
#include <kapplication.h>
#include <kprinter.h>

// application specific includes
#include "kudesignerview.h"
#include "kudesignerdoc.h"
#include "kudesigner.h"

KuDesignerView::KuDesignerView(QWidget *parent, const char *name) : ReportCanvas(0, parent, name)
{
  setBackgroundMode(PaletteBase);
}

KuDesignerView::~KuDesignerView()
{
}

KuDesignerDoc *KuDesignerView::getDocument() const
{
  KuDesignerApp *theApp=(KuDesignerApp *) parentWidget();

  return theApp->getDocument();
}

void KuDesignerView::print(KPrinter *pPrinter)
{
  QPainter p;
  p.begin(pPrinter);
	
  // TODO: add your printing code here


	QFontMetrics fm = p.fontMetrics();
	QPaintDeviceMetrics metrics(pPrinter);
	
	QString s;
	int yPos = 0;
	int pageNo = 1;
	int Margin = 20;

    QString str = getDocument()->canvas()->templ->getXml();
    QTextStream t(&str, IO_ReadOnly);

	while (!t.eof())
	{
	    if ( Margin + yPos > metrics.height() - Margin ) {
/*		QString msg( "Printing (page " );
		msg += QString::number( ++pageNo );
		msg += ")...";
		statusBar()->message( msg );*/
    		pPrinter->newPage();             // no more room on this page
	    	yPos = 0;                       // back to top of page
	    }
	
	    s = t.readLine();
	    p.drawText(Margin, Margin + yPos, metrics.width(), fm.lineSpacing(),
		       AlignLeft | AlignVCenter | WordBreak | ExpandTabs, s);
	    yPos += fm.lineSpacing();
	}


  p.end();
}
#include "kudesignerview.moc"
