
#include "khtmlview_patched.h"
#include "khtmlview_patched.moc"

#include "khtmlwidget_patched.h"

#include <iostream.h>


KHTMLView_Patched::KHTMLView_Patched(QWidget *parent = 0L, const char *name = 0L, int flags = 0,
                    KHTMLView_Patched *parent_view = 0L)
:KHTMLView(parent, name, flags, parent_view, (new KHTMLWidget_Patched(0L, "" )) )
{
}

KHTMLView_Patched::~KHTMLView_Patched()
{
  if (view) delete view;
}

void KHTMLView_Patched::draw(SavedPage *p, QPainter *painter, int width, int height, float scale)
{
  QPixmap pix(width, height);

  ((KHTMLWidget_Patched*)view)->draw(&pix, width, height);

//  if (scale != 1.0)
//     tmpPainter.scale(scale, scale);
     
  painter->drawPixmap(0, 0, pix);

/*
  if (p->frames)
     {
       cerr << "iterating" << endl;
       QListIterator<SavedPage> it(*p->frames);
       for (; it.current(); ++it)
           {
	     KHTMLView *v = findView( it.current()->frameName );
	     if (v)
	        ((KHTMLView_Patched*)v)->draw(it.current(), painter, width, height, scale);
	   }     
     }     
*/     
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
