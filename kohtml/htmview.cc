/*

	Copyright (C) 1998 Simon Hausmann
                       <tronical@gmx.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/
//
// $Id$
//

#include "htmview.h"
#include "htmview.moc"

#include "htmwidget.h"

#include <iostream.h>

#include <qobjectlist.h>

KMyHTMLView::KMyHTMLView(QWidget *parent = 0L, const char *name = 0L, int flags = 0,
                    KMyHTMLView *parent_view = 0L)
:KHTMLView(parent, name, flags, parent_view, (new KMyHTMLWidget(0L, "" )) )
{
}

KMyHTMLView::~KMyHTMLView()
{
  if (view) delete view;
}

void KMyHTMLView::draw(QPainter *painter, int width, int height)
{
  cerr << "void KMyHTMLView::draw(QPainter *painter, int width, int height)" << endl;

  pixmap = new QPixmap( width, height );

//  pixmap->fill();
  view->resize( width, height );

  cerr << "drawing view" << endl;
  drawWidget( view );
  
  // this does not seem to work :-((
  cerr << "drawing scrollbars" << endl;
  if (displayHScroll) drawWidget( horz );
  if (displayVScroll) drawWidget( vert );
  
  painter->drawPixmap( 0, 0, *pixmap );
  
  delete pixmap;
  
  cerr << "done" << endl;
}

void KMyHTMLView::drawWidget( QWidget *widget )
{
  cerr << "trying to paint " << widget->className() << endl;

  int x1 = widget->pos().x() ;//- pos().x();
  int y1 = widget->pos().y() ;//- pos().y();

  cerr << "coords: " << x1 << " " 
                     << y1 << " : "
		     << widget->width() << " "
		     << widget->height() << endl;

  QPainter::redirect( widget, pixmap );
  QPaintEvent pe( QRect(x1, y1, widget->width(), widget->height()));
  QApplication::sendEvent( widget, &pe );
  QPainter::redirect( widget, 0 );

  const QObjectList *childrenList = widget->children();
  cerr << "children? " << childrenList << endl;
  if (childrenList)
    {
      QObjectListIt it( *childrenList );
      for (; it.current(); ++it )
        {
	cerr << "className : " << it.current()->className() << endl;
        if (it.current()->isWidgetType())
	  {
	    cerr << "preparing for paint: " << it.current()->className() << endl;
	    QWidget *w = (QWidget *)it.current();
	    if (w->parentWidget() == widget /*&& w->isVisible()*/)
	      drawWidget( w );
	  }
	}  
    }  
    
}

KHTMLView *KMyHTMLView::newView(QWidget *parent = 0L, const char *name = 0L, int flags = 0L)
{
  cout << "uh, creating new patched view" << endl;
  
  KMyHTMLView *view = new KMyHTMLView(parent, name, flags, this);
  
    connect( view, SIGNAL( documentStarted( KHTMLView * ) ),
	     this, SLOT( slotDocumentStarted( KHTMLView * ) ) );
    connect( view, SIGNAL( documentDone( KHTMLView * ) ),
	     this, SLOT( slotDocumentDone( KHTMLView * ) ) );
    connect( view, SIGNAL( documentRequest( KHTMLView *, const char * ) ),
             this, SLOT( slotDocumentRequest( KHTMLView *, const char * ) ) );	     
    connect( view, SIGNAL( imageRequest( KHTMLView *, const char * ) ),
	     this, SLOT( slotImageRequest( KHTMLView *, const char * ) ) );
    connect( view, SIGNAL( URLSelected( KHTMLView *, const char*, int, const char* ) ),
	     this, SLOT( slotURLSelected( KHTMLView *, const char *, int, const char* ) ) );    
    connect( view, SIGNAL( onURL( KHTMLView *, const char* ) ),
	     this, SLOT( slotOnURL( KHTMLView *, const char * ) ) );
    connect( view, SIGNAL( popupMenu( KHTMLView *, const char*, const QPoint & ) ),
	     this, SLOT( slotPopupMenu( KHTMLView *, const char *, const QPoint & ) ) );
    connect( view, SIGNAL( cancelImageRequest( KHTMLView *, const char* ) ),
	     this, SLOT( slotCancelImageRequest( KHTMLView *, const char * ) ) );
    connect( view, SIGNAL( formSubmitted( KHTMLView *, const char *, const char*, const char* ) ),
	     this, SLOT( slotFormSubmitted( KHTMLView *, const char *, const char*, const char* ) ) );
  
  return view;
}
