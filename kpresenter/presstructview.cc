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
#include <kpresenter_view.h>
#include <page.h>

#include <klocale.h>
#include <kiconloader.h>

#include <qsplitter.h>
#include <qheader.h>
#include <qwmatrix.h>
#include <qvbox.h>
#include <qcheckbox.h>

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

    QPixmap pix( doc->getPageRect( 0, 0, 0 ).size() );
    pix.fill( Qt::white );
    int i = ( (KPPresStructObjectItem*)item )->getPageNum();
    view->getPage()->drawPageInPix2( pix, i * doc->getPageRect( 0, 0, 0 ).height(), i );

    double faktW = static_cast<double>( width() ) / static_cast<double>( doc->getPageRect( 0, 0, 0 ).width() );
    double faktH = static_cast<double>( height() ) / static_cast<double>( doc->getPageRect( 0, 0, 0 ).height() );
    double fakt = QMIN( faktW, faktH ) - 0.05;

    QWMatrix m;
    m.scale( fakt, fakt );
    pix = pix.xForm( m );

    setPixmap( pix );
}

/******************************************************************
 *
 * Class: KPPresStructObjectItem
 *
 ******************************************************************/

/*================================================================*/
KPPresStructObjectItem::KPPresStructObjectItem( QListView *parent )
    : QListViewItem( parent ), page( 0 ), object( 0 )
{
}

/*================================================================*/
KPPresStructObjectItem::KPPresStructObjectItem( QListViewItem *parent )
    : QListViewItem( parent ), page( 0 ), object( 0 )
{
}

/*================================================================*/
void KPPresStructObjectItem::setPage( KPBackGround *p, int pgnum )
{
    page = p;
    pageNum = pgnum;
    if ( page && !parent() )
        setPixmap( 0, KPBarIcon( "newPoint" ) );
}

/*================================================================*/
void KPPresStructObjectItem::setObject( KPObject *o, int num )
{
  object = o;
  if ( object && parent() ) {
    QString type = object->getTypeString() + " (%1)";
    setText(0, type.arg( num + 1 ));
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
    case OT_GROUP:
      setPixmap( 0, KPBarIcon( "group" ) );
      break;
    }
  }
}

/*================================================================*/
KPBackGround *KPPresStructObjectItem::getPage()
{
    return page;
}

/*================================================================*/
KPObject *KPPresStructObjectItem::getObject()
{
    return object;
}

/*================================================================*/
int KPPresStructObjectItem::getPageNum()
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
    : QDialog( parent, name, false ), doc( _doc ), view( _view )
{
    hsplit = new QSplitter( this );
    setupSlideList();
    setupPagePreview();

    setMinimumSize( 600, 400 );
}

/*================================================================*/
void KPPresStructView::setupSlideList()
{
    slides = new QListView( hsplit );
    slides->addColumn( i18n( "Slide No." ) );
    slides->addColumn( i18n( "Slide Title" ) );
    slides->header()->setMovingEnabled( false );
    slides->setAllColumnsShowFocus( true );
    slides->setRootIsDecorated( true );
    slides->setSorting( -1 );

    for ( int i = doc->getPageNums() - 1; i >= 0; --i ) {
        KPPresStructObjectItem *item = new KPPresStructObjectItem( slides );
        item->setPage( doc->backgroundList()->at( i ), i );
        item->setText( 0, QString( "%1" ).arg( i + 1 ) );
        item->setText( 1, doc->getPageTitle( i, i18n( "Slide %1" ).arg( i + 1 ) ) );
        for ( int j = doc->objNums() - 1; j >= 0; --j ) {
            if ( doc->getPageOfObj( j, 0, 0 ) == (int)i + 1 ) {
                KPPresStructObjectItem *item_ = new KPPresStructObjectItem( item );
                item_->setPage( doc->backgroundList()->at( i ), i );
                item_->setObject( doc->objectList()->at( j ), j );
                item_->setNum(j);
            }
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
    //QDialog::resizeEvent( e );
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
        view->getPage()->deSelectAllObj();
        if (item->parent())
            view->skipToPage( item->parent()->text( 0 ).toInt() - 1 );
        else
            view->skipToPage( item->text( 0 ).toInt() - 1 );
        int obj = dynamic_cast<KPPresStructObjectItem *>(item)->getNum();
        KPObject *kpobject = doc->objectList()->at( obj );
        if (kpobject) {
          QRect rect( kpobject->getBoundingRect( 0, 0 ) );
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
