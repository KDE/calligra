/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include <presstructview.h>

#include <kpbackground.h>
#include <kpobject.h>
#include "kptextobject.h"
#include <kpresenter_view.h>
#include "kprcanvas.h"

#include <klocale.h>
#include <kiconloader.h>

#include <qsplitter.h>
#include <qheader.h>
#include <qwmatrix.h>
#include <qvbox.h>
#include <qcheckbox.h>
#include <kdebug.h>
#include <qlayout.h>
#include <qimage.h>

/******************************************************************
 *
 * Class: KPSlidePreview
 *
 ******************************************************************/

/*================================================================*/
KPSlidePreview::KPSlidePreview( QWidget *parent, KPresenterDoc *_doc, KPresenterView *_view )
    : QLabel( parent ), doc( _doc ), view( _view )
{
    setFrameStyle( StyledPanel | Sunken );
}

/*================================================================*/
void KPSlidePreview::setPage( QListViewItem *item )
{
    if ( !item )
        return;
    int i = ( (KPPresStructObjectItem*)item )->getPageNum();
    QRect rect=doc->pageList().at(i)->getZoomPageRect();
    QPixmap pix( rect.size() );
    pix.fill( Qt::white );
    view->getCanvas()->drawPageInPix( pix, i );

    int w = rect.width();
    int h = rect.height();
    if ( w > h ) {
        w = 297;
        h = 210;
    }
    else if ( w < h ) {
        w = 210;
        h = 297;
    }
    else if ( w == h ) {
        w = 297;
        h = 297;
    }

    const QImage img( pix.convertToImage().smoothScale( w, h, QImage::ScaleFree ) );
    pix.convertFromImage( img );
    setPixmap( pix );
}

/******************************************************************
 *
 * Class: KPPresStructObjectItem
 *
 ******************************************************************/

/*================================================================*/
KPPresStructObjectItem::KPPresStructObjectItem( KListView *parent )
    : KListViewItem( parent ), page( 0 ), object( 0 )
{
}

/*================================================================*/
KPPresStructObjectItem::KPPresStructObjectItem( KListViewItem *parent )
    : KListViewItem( parent ), page( 0 ), object( 0 )
{
}

/*================================================================*/
void KPPresStructObjectItem::setPage( KPBackGround *p, int pgnum )
{
    page = p;
    pageNum = pgnum;
    if ( page && !parent() )
        setPixmap( 0, KPBarIcon( "newslide" ) );
}

/*================================================================*/
void KPPresStructObjectItem::setObject( KPObject *o, int num, bool sticky,const QString &_objName )
{
  object = o;
  if ( object && parent() ) {
    QString type = _objName.isEmpty() ? (object->getTypeString() + " (%1)").arg( num + 1 ):_objName;
    if( sticky )
        type+=i18n(" (Sticky object)");
    setText(0, type);
    switch ( object->getType() ) {
    case OT_PICTURE:
      setPixmap( 0, KPBarIcon( "frame_image" ) );
      break;
    case OT_LINE:
      setPixmap( 0, KPBarIcon( "mini_line" ) );
      break;
    case OT_RECT:
      setPixmap( 0, KPBarIcon( "mini_rect" ) );
      break;
    case OT_ELLIPSE:
      setPixmap( 0, KPBarIcon( "mini_circle" ) );
      break;
    case OT_TEXT:
      setPixmap( 0, KPBarIcon( "frame_text" ) );
      break;
    case OT_AUTOFORM:
      setPixmap( 0, KPBarIcon( "mini_autoform" ) );
      break;
    case OT_CLIPART:
      setPixmap( 0, KPBarIcon( "mini_clipart" ) );
      break;
    case OT_UNDEFINED:
      setText( 0, i18n( "Undefined (%1)" ).arg( num + 1 ) );
      break;
    case OT_PIE:
      setPixmap( 0, KPBarIcon( "mini_pie" ) );
      break;
    case OT_PART:
      setPixmap( 0, KPBarIcon( "frame_query" ) );
      break;
    case OT_FREEHAND:
      setPixmap( 0, KPBarIcon( "freehand" ) );
      break;
    case OT_POLYLINE:
      setPixmap( 0, KPBarIcon( "polyline" ) );
      break;
    case OT_QUADRICBEZIERCURVE:
      setPixmap( 0, KPBarIcon( "quadricbeziercurve" ) );
      break;
    case OT_CUBICBEZIERCURVE:
      setPixmap( 0, KPBarIcon( "cubicbeziercurve" ) );
      break;
    case OT_POLYGON:
      setPixmap( 0, KPBarIcon( "mini_polygon" ) );
      break;
    case OT_CLOSED_LINE: {
        QString _typeString = object->getTypeString();

        if ( _typeString == i18n( "Closed Freehand" ) )
            setPixmap( 0, KPBarIcon( "closed_freehand" ) );
        else if ( _typeString == i18n( "Closed Polyline" ) )
            setPixmap( 0, KPBarIcon( "closed_polyline" ) );
        else if ( _typeString == i18n( "Closed Quadric Bezier Curve" ) )
            setPixmap( 0, KPBarIcon( "closed_quadricbeziercurve" ) );
        else if ( _typeString == i18n( "Closed Cubic Bezier Curve" ) )
            setPixmap( 0, KPBarIcon( "closed_cubicbeziercurve" ) );
    } break;
    case OT_GROUP:
      setPixmap( 0, KPBarIcon( "group" ) );
      break;
    }
  }
}

/*================================================================*/
KPBackGround *KPPresStructObjectItem::getPage() const
{
    return page;
}

/*================================================================*/
KPObject *KPPresStructObjectItem::getObject() const
{
    return object;
}

/*================================================================*/
int KPPresStructObjectItem::getPageNum() const
{
    return pageNum;
}

/******************************************************************
 *
 * Class: KPPresStructView
 *
 ******************************************************************/

/*================================================================*/
KPPresStructView::KPPresStructView( QWidget *parent, const char *name,
                                    KPresenterDoc *_doc, KPresenterView *_view )
    : KDialogBase( parent, name, false, "", KDialogBase::Close ), doc( _doc ), view( _view )
{
    QWidget *page = new QWidget( this );
    setMainWidget(page);
    QVBoxLayout *topLayout = new QVBoxLayout( page, 0, spacingHint() );
    hsplit = new QSplitter( page );
    topLayout->addWidget(hsplit);
    setupSlideList();
    setupPagePreview();
    slides->setSelected( slides->firstChild(), true );

    setMinimumSize( 600, 400 );
    connect(this,SIGNAL(closeClicked()),this,SLOT(slotCloseDialog()));
}


/*================================================================*/
void KPPresStructView::setupSlideList()
{
    slides = new KListView( hsplit );
    slides->addColumn( i18n( "Slide No." ) );
    slides->addColumn( i18n( "Slide Title" ) );
    slides->header()->setMovingEnabled( false );
    slides->setAllColumnsShowFocus( true );
    slides->setRootIsDecorated( true );
    slides->setSorting( -1 );

    for ( int i = doc->getPageNums() - 1; i >= 0; --i ) {
        KPPresStructObjectItem *item = new KPPresStructObjectItem( slides );
        item->setPage( doc->pageList().at( i )->background(), i );
        item->setText( 0, QString( "%1" ).arg( i + 1 ) );
        item->setText( 1, doc->pageList().at( i )->pageTitle( i18n( "Slide %1" ).arg( i + 1 ) ) );
        KPrPage *page=doc->pageList().at( i );
        QPtrList<KPObject> list(page->objectList());
        for ( int j = page->objNums() - 1; j >= 0; --j ) {
            KPPresStructObjectItem *item_ = new KPPresStructObjectItem( item );
            item_->setPage( page->background(), i );
            item_->setObject( list.at( j ), j );
            item_->setNum(j);
        }
        //add sticky obj
        list=doc->stickyPage()->objectList();
        int offset=page->objNums() - 1;
        QPtrListIterator<KPObject> it( doc->stickyPage()->objectList() );
        for ( ; it.current() ; ++it )
        {
            QString name;
            if(doc->isHeaderFooter(it.current()))
            {
                if(it.current()==doc->header()&&!doc->hasHeader())
                    continue;
                if( it.current()==doc->header()&&doc->hasHeader())
                    name=i18n("Header");
                if(it.current()==doc->footer()&&!doc->hasFooter())
                    continue;
                if( it.current()==doc->footer()&&doc->hasFooter())
                    name=i18n("Footer");
            }
            KPPresStructObjectItem *item_ = new KPPresStructObjectItem( item );
            item_->setPage( page->background(), i );
            item_->setObject( it.current(), offset, name.isEmpty(), name );
            item_->setNum(offset);
            offset++;
        }
    }
}

/*================================================================*/
void KPPresStructView::setupPagePreview()
{
    QVBox *box = new QVBox( hsplit );
    box->setMargin( 5 );
    box->setSpacing( 5 );

    showPreview = new QCheckBox( i18n( "&Show Preview" ), box );
    showPreview->setChecked( true );
    // #### for now
    // lukas: what was it meant to do?
    showPreview->hide( );

    slidePreview = new KPSlidePreview( box, doc, view );
    connect( slides, SIGNAL( selectionChanged( QListViewItem * ) ),
             slidePreview, SLOT( setPage( QListViewItem * ) ) );
    connect( slides, SIGNAL( selectionChanged( QListViewItem * ) ),
             this, SLOT( makeStuffVisible( QListViewItem * ) ) );
}

/*================================================================*/
void KPPresStructView::resizeEvent( QResizeEvent * )
{
    hsplit->resize( size() );
}

/*================================================================*/
void KPPresStructView::makeStuffVisible( QListViewItem *item )
{
    if ( !item )
        return;

    if ( !item->parent() )
        view->skipToPage( item->text( 0 ).toInt() - 1 );
    else {
        if (item->parent())
            view->skipToPage( item->parent()->text( 0 ).toInt() - 1 );
        else
            view->skipToPage( item->text( 0 ).toInt() - 1 );
        KPObject *kpobject = dynamic_cast<KPPresStructObjectItem *>(item)->getObject();
        if (kpobject) {
          QRect rect( doc->zoomHandler()->zoomRect(kpobject->getBoundingRect(doc->zoomHandler()  ) ));
          kpobject->setSelected( true );
          doc->repaint( kpobject );
          rect.setLeft( rect.left() - 20 );
          rect.setTop( rect.top() - 20 );
          rect.setRight( rect.right() + 20 );
          rect.setBottom( rect.bottom() + 20 );
          view->makeRectVisible( rect );
        }
    }
}

#include <presstructview.moc>
