
#include "khtmlview_patched.h"
#include "khtmlview_patched.moc"

#include "khtmlwidget_patched.h"

#include <iostream.h>


KHTMLView_Patched::KHTMLView_Patched(QWidget *parent = 0L, const char *name = 0L, int flags = 0,
                    KHTMLView_Patched *parent_view = 0L)
:KHTMLView(parent, name, flags, parent_view)
{
  cout << "KHTMLView_Patched::KHTMLView_Patched " << endl;

  delete view;    
    
  view = new KHTMLWidget_Patched( this, "" );
  CHECK_PTR( view );
  view->setView( this );
  setFocusProxy( view );
    
  connect( view, SIGNAL( scrollVert( int ) ), SLOT( slotInternScrollVert( int ) ) );
  connect( view, SIGNAL( scrollHorz( int ) ), SLOT( slotInternScrollHorz( int ) ) );

  connect( vert, SIGNAL(valueChanged(int)), view, SLOT(slotScrollVert(int)) );
  connect( horz, SIGNAL(valueChanged(int)), view, SLOT(slotScrollHorz(int)) );

  connect( view, SIGNAL( documentChanged() ), SLOT( slotDocumentChanged() ) );
  connect( view, SIGNAL( setTitle( const char* ) ),
           this, SLOT( slotSetTitle( const char * ) ) );
  connect( view, SIGNAL( URLSelected( const char*, int, const char* ) ),
           this, SLOT( slotURLSelected( const char *, int, const char* ) ) );    
  connect( view, SIGNAL( onURL( const char* ) ),
           this, SLOT( slotOnURL( const char * ) ) );
  connect( view, SIGNAL( textSelected( bool ) ),
           this, SLOT( slotTextSelected( bool ) ) );
  connect( view, SIGNAL( popupMenu( const char*, const QPoint & ) ),
           this, SLOT( slotPopupMenu( const char *, const QPoint & ) ) );
  connect( view, SIGNAL( fileRequest( const char* ) ),
           this, SLOT( slotImageRequest( const char * ) ) );
  connect( view, SIGNAL( cancelFileRequest( const char* ) ),
           this, SLOT( slotCancelImageRequest( const char * ) ) );
  connect( view, SIGNAL( formSubmitted( const char *, const char*, const char* , const char *) ),
           this, SLOT( slotFormSubmitted( const char *, const char*, const char*, const char * ) ) );
  connect( view, SIGNAL( documentStarted() ),
           this, SLOT( slotDocumentStarted() ) );
  connect( view, SIGNAL( documentDone() ),
           this, SLOT( slotDocumentDone() ) );
  connect( view, SIGNAL( goUp() ), this, SLOT( slotGoUp() ) );
  connect( view, SIGNAL( goLeft() ), this, SLOT( slotGoLeft() ) );
  connect( view, SIGNAL( goRight() ), this, SLOT( slotGoRight() ) );

  view->setURLCursor( upArrowCursor );
  view->raise();
}

KHTMLView_Patched::~KHTMLView_Patched()
{
}

void KHTMLView_Patched::draw(SavedPage *p, QPaintDevice *dev, int width, int height, float scale)
{
//  cerr << "drawing right??????????????" << endl;
  
//  QPainter::redirect(this, dev);
//  QPaintEvent pe(QRect(0, 0, width, height));
//  QApplication::sendEvent(this, &pe);
//  QPainter::redirect(this, 0);

//  cerr << "anyway, we're done..." << endl;
  
//  ((KHTMLWidget_Patched *)view)->draw(dev, width, height);

  QPixmap pix(width - p->xOffset, height - p->yOffset);

  cerr << "drawing to pixmap" << endl;
     
  ((KHTMLWidget_Patched*)view)->draw(&pix, width - p->xOffset, height - p->yOffset);

  cerr << "painting to " << p->xOffset << " " << p->yOffset << " from pixmap with size " << pix.width() << " " << pix.height() << endl;
  cerr << "required width is " << width << " and the height is " << height << endl;  
  
  QPainter tmpPainter;
  tmpPainter.begin(dev);
  
  if (scale != 1.0)
     tmpPainter.scale(scale, scale);
     
  tmpPainter.drawPixmap(p->xOffset, p->yOffset, pix);
  tmpPainter.end();  
//  bitBlt(dev, p->xOffset, p->yOffset, &pix, 0, 0, pix.width(), pix.height(), CopyROP, true);

  if (p->frames)
     {
       cerr << "iterating" << endl;
       QListIterator<SavedPage> it(*p->frames);
       for (; it.current(); ++it)
           draw(it.current(), dev, width, height, scale);
     }     
}

KHTMLView *KHTMLView_Patched::newView(QWidget *parent = 0L, const char *name = 0L, int flags = 0L)
{
  cout << "uh, creating new patched view" << endl;
  
  KHTMLView_Patched *view = new KHTMLView_Patched(parent, name, flags, this);
  
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

void KHTMLView_Patched::setMouseLock(bool flag)
{
  QListIterator<KHTMLView> it(*viewList);
  
  for (; it.current(); ++it)
      ((KHTMLWidget_Patched*)it.current()->getKHTMLWidget())->setMouseLock(flag);
}
