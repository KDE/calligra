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
#include <qregexp.h>

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

    float faktW = static_cast<float>( width() ) / static_cast<float>( doc->getPageRect( 0, 0, 0 ).width() );
    float faktH = static_cast<float>( height() ) / static_cast<float>( doc->getPageRect( 0, 0, 0 ).height() );
    float fakt = QMIN( faktW, faktH ) - 0.1;

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
        setPixmap( 0, KPBarIcon( "dot" ) );
}

/*================================================================*/
void KPPresStructObjectItem::setObject( KPObject *o, int num )
{
    object = o;
    if ( object && parent() ) {
        switch ( object->getType() ) {
        case OT_PICTURE:
            setText( 0, i18n( "Picture (%1)" ).arg( num + 1 ) );
            setPixmap( 0, KPBarIcon( "frame_image" ) );
            break;
        case OT_LINE:
            setText( 0, i18n( "Line (%1)" ).arg( num + 1 ) );
            setPixmap( 0, KPBarIcon( "mini_line" ) );
            break;
        case OT_RECT:
            setText( 0, i18n( "Rectangle (%1)" ).arg( num + 1 ) );
            setPixmap( 0, KPBarIcon( "mini_rect" ) );
            break;
        case OT_ELLIPSE:
            setText( 0, i18n( "Circle/Ellipse (%1)" ).arg( num + 1 ) );
            setPixmap( 0, KPBarIcon( "mini_circle" ) );
            break;
        case OT_TEXT:
            setText( 0, i18n( "Text (%1)" ).arg( num + 1 ) );
            setPixmap( 0, KPBarIcon( "frame_text" ) );
            break;
        case OT_AUTOFORM:
            setText( 0, i18n( "Autoform (%1)" ).arg( num + 1 ) );
            setPixmap( 0, KPBarIcon( "mini_autoform" ) );
            break;
        case OT_CLIPART:
            setText( 0, i18n( "Clipart (%1)" ).arg( num + 1 ) );
            setPixmap( 0, KPBarIcon( "mini_clipart" ) );
            break;
        case OT_UNDEFINED:
            setText( 0, i18n( "Undefined (%1)" ).arg( num + 1 ) );
            break;
        case OT_PIE:
            setText( 0, i18n( "Pie/Arc/Chord (%1)" ).arg( num + 1 ) );
            setPixmap( 0, KPBarIcon( "mini_pie" ) );
            break;
        case OT_PART:
            setText( 0, i18n( "Embedded Part (%1)" ).arg( num + 1 ) );
            setPixmap( 0, KPBarIcon( "frame_query" ) );
            break;
        case OT_GROUP:
            setText( 0, i18n( "Group Object (%1)" ).arg( num + 1 ) );
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
    showPreview->setEnabled( false );

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
        QString s = item->text( 0 );
        QRegExp r( QString::fromLatin1( "([a-zA-Z0-9\\.\\*\\?\\ \\+\\;]*)$" ) );
        int len;
        int index = r.match( s, 0, &len );
        if ( index >= 0 )
            s = s.mid( index + 1, len - 2 );
        else
            return;
        view->getPage()->deSelectAllObj();
        view->skipToPage( item->text( 0 ).toInt() - 1 );
        int obj = s.toInt() - 1;
        KPObject *kpobject = doc->objectList()->at( obj );
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

#include <presstructview.moc>
