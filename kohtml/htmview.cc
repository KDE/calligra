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

#include <qobjectlist.h>

#include <kurl.h>
#include <list>

#include "htmwidget.h"
#include "kohtml_doc.h"

KMyHTMLView::KMyHTMLView(KoHTMLDoc *doc, QWidget *parent, const char *name,
                         int flags, KMyHTMLView *parent_view)
:KHTMLView(parent, name, flags, parent_view, (new KMyHTMLWidget(0L, "" )) )
{
  m_pDoc = doc;
  m_lstChildren.setAutoDelete( true );
  m_bParsing = false;
  m_bReload = false;
  m_bDone = true;
  m_pParent = parent_view;
  m_strDocument = "";

  getKHTMLWidget()->setFocusPolicy( QWidget::StrongFocus );

  connect( this, SIGNAL(documentDone( KHTMLView * )),
           this, SLOT(documentFinished( KHTMLView * )));
  connect( this, SIGNAL( imageRequest( const char * )),
           this, SLOT( requestImage( const char * )));
  connect( this, SIGNAL( cancelImageRequest( const char * )),
           this, SLOT( cancelImage( const char * )));
	
  if ( m_pParent )
     {
       connect(this, SIGNAL(URLSelected(KHTMLView *, const char *, int, const char *)),
               m_pParent, SLOT(slotURLSelected(KHTMLView *, const char *, int, const char *)));
       connect(this, SIGNAL(onURL(KHTMLView *, const char *)),
               m_pParent, SLOT(slotOnURL(KHTMLView *, const char *)));
     }			

}

KMyHTMLView::~KMyHTMLView()
{
  stop();
}

void KMyHTMLView::draw(QPainter *painter, int width, int height)
{
  m_pPixmap = new QPixmap( width, height );

//  pixmap->fill();
  view->resize( width, height );

  drawWidget( view );

  // this does not seem to work :-((
  cerr << "drawing scrollbars" << endl;
  if (displayHScroll) drawWidget( horz );
  if (displayVScroll) drawWidget( vert );

  painter->drawPixmap( 0, 0, *m_pPixmap );

  delete m_pPixmap;

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

  QPainter::redirect( widget, m_pPixmap );
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

void KMyHTMLView::begin( const char *url, int dx, int dy )
{
  m_strDocument = "";
  m_lstChildren.clear();
  KHTMLView::begin( url, dx, dy );
}

KHTMLView *KMyHTMLView::newView(QWidget *parent, const char *name, int flags)
{
  KMyHTMLView *view = new KMyHTMLView(m_pDoc, parent, name, flags, this);

  m_lstChildren.append( new FrameChild( view ));

  return view;
}

void KMyHTMLView::openURL( const char *url )
{
  openURL( url, false );
}

void KMyHTMLView::openURL( const char *url, bool reload )
{
  if ( m_bParsing )
     {
       end();
       m_bParsing = false;
     }

  cancelAllRequests(); // just to be on the save side

  m_bReload = reload;
  m_bDone = false;
  m_strURL = url;

  /*
   * TODO: Do something like (pseudo code) :
   *  ( ONLY IF m_bReload = TRUE )
   *  imageList = getImageRefsFromWidget()
   *  for (imageList)
   *      imageCache->removeURL( entry.url );
   */

  m_pDoc->requestDocument( this, url, m_bReload );
}

void KMyHTMLView::feedDocumentData( const char *data, bool eof )
{
  if ( m_bDone )
    return; //aiieeee

  if ( !m_bParsing )
     {
       m_bParsing = true;
       m_lstChildren.clear();

       KURL::List lst;
       lst = KURL::split( m_strURL );
       KURL::List::ConstIterator it = lst.fromLast();
       begin( (*it).url(), 0, 0 );
       parse();
     }

  m_strDocument += data;
  write( data );

  if ( eof )
     {
       end();
       m_bParsing = false;
       documentFinished( this );
     }
}

void KMyHTMLView::stop()
{

  if ( m_bParsing )
     {
       m_bParsing = false;
       end();
       m_pDoc->cancelDocument( this, m_strURL );
     }

  m_bDone = true;
  cancelAllRequests(); //we don't want to run into any trouble

  //let's "stop" all children
  QListIterator<FrameChild> it( m_lstChildren );
  for (; it.current(); ++it)
    if ( !it.current()->m_bDone )
      it.current()->m_pView->stop();

  if ( m_pParent )
     m_pParent->childFinished( this );
  else
     //let's notify our "parent document"
     m_pDoc->viewFinished( this );
}

void KMyHTMLView::childFinished( KMyHTMLView *child_view )
{
  QListIterator<FrameChild> it( m_lstChildren );
  for (; it.current(); ++it)
    if ( it.current()->m_pView == child_view )
       it.current()->m_bDone = true;

  if ( m_bDone )
    documentFinished( this );
}

void KMyHTMLView::save( ostream &out )
{
  return;

  SavedPage *p = saveYourself();

  out << otag << "<VIEW "
      << "scrolling=\"" << p->scrolling << "\" "
      << "frameborder=\"" << p->frameborder << "\" "
      << "marginwidth=\"" << p->marginwidth << "\" "
      << "marginheight=\"" << p->marginheight << "\" "
      << "allowresize=\"" << p->allowresize << "\" "
      << "url=\"" << p->url << "\" "
      << "title=\"" << p->title << "\" "
      << "xOffset=\"" << p->xOffset << "\" "
      << "yOffset=\"" << p->yOffset << "\" ";

  if (p->isFrame)
     out << "frameName=\"" << p->frameName << "\" ";

  out << ">" << endl;

/*
  out << "<DATA>"
      << html_data_here (encoded?)
      << "</DATA>" << endl;
*/

  //hm... I might consider moving this to KoHTMLDoc...

  out << etag << "</VIEW>" << endl;

  delete p;
}

bool KMyHTMLView::isChild( KMyHTMLView *view )
{
  QListIterator<FrameChild> it( m_lstChildren );
  for (; it.current(); ++it)
    {
      KMyHTMLView *childView = it.current()->m_pView;
      if ( childView == view || childView->isChild( view ) )
        return true;
    }
  return false;
}

void KMyHTMLView::documentFinished( KHTMLView *view )
{
  if ( view != this )
    return;

  m_bDone = true;

  QListIterator<FrameChild> it( m_lstChildren );
  for (; it.current(); ++it)
    if ( !it.current()->m_bDone )
      return;

  if ( m_pParent )
    m_pParent->childFinished( this );
  else
     //let's notify our "parent document"
     m_pDoc->viewFinished( this );
}

void KMyHTMLView::requestImage( const char *url )
{
  m_pDoc->requestImage( this, url, m_bReload );
}

void KMyHTMLView::cancelImage( const char *url )
{
  m_pDoc->cancelImage( this, url );
}
