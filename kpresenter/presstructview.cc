/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Presentation Structure Viewer (header)                 */
/******************************************************************/

#include "presstructview.h"
#include "presstructview.moc"

#include "kpbackground.h"
#include "kpobject.h"
#include "kpresenter_doc.h"
#include "kpresenter_view.h"
#include "page.h"

#include <klocale.h>
#include <kiconloader.h>

#include <qsplitter.h>
#include <qevent.h>
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
    
    QPixmap pix( QSize( doc->getPageSize( 0, 0, 0 ).width(), doc->getPageSize( 0, 0, 0 ).height() ) );
    pix.fill( Qt::white );
    int i = ( (KPPresStructObjectItem*)item )->getPageNum();
    view->getPage()->drawPageInPix2( pix, i * doc->getPageSize( 0, 0, 0 ).height(), i );

    float faktW = static_cast<float>( width() ) / static_cast<float>( doc->getPageSize( 0, 0, 0 ).width() );
    float faktH = static_cast<float>( height() ) / static_cast<float>( doc->getPageSize( 0, 0, 0 ).height() );
    float fakt = QMIN( faktW, faktH );
    
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
	setPixmap( 0, BarIcon( "dot" ) );
}

/*================================================================*/
void KPPresStructObjectItem::setObject( KPObject *o )
{
    object = o;
    if ( object && parent() ) {
	switch ( object->getType() ) {
	case OT_PICTURE:
	    setText( 0, i18n( "Picture" ) );
	    setPixmap( 0, BarIcon( "mini_picture" ) );
	    break;
	case OT_LINE:
	    setText( 0, i18n( "Line" ) );
	    setPixmap( 0, BarIcon( "mini_line" ) );
	    break;
	case OT_RECT:
	    setText( 0, i18n( "Rectangle" ) );
	    setPixmap( 0, BarIcon( "mini_rect" ) );
	    break;
	case OT_ELLIPSE:
	    setText( 0, i18n( "Circle/Ellipse" ) );
	    setPixmap( 0, BarIcon( "mini_circle" ) );
	    break;
	case OT_TEXT:
	    setText( 0, i18n( "Text" ) );
	    setPixmap( 0, BarIcon( "mini_text" ) );
	    break;
	case OT_AUTOFORM:
	    setText( 0, i18n( "Autoform" ) );
	    setPixmap( 0, BarIcon( "mini_autoform" ) );
	    break;
	case OT_CLIPART:
	    setText( 0, i18n( "Clipart" ) );
	    setPixmap( 0, BarIcon( "mini_clipart" ) );
	    break;
	case OT_UNDEFINED:
	    setText( 0, i18n( "Undefined" ) );
	    break;
	case OT_PIE:
	    setText( 0, i18n( "Pie/Arc/Chord" ) );
	    setPixmap( 0, BarIcon( "mini_pie" ) );
	    break;
	case OT_PART:
	    setText( 0, i18n( "Embedded Part" ) );
	    setPixmap( 0, BarIcon( "mini_part" ) );
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
    : QDialog( parent, name, FALSE ), doc( _doc ), view( _view )
{
    hsplit = new QSplitter( this );
    setupSlideList();
    setupPagePreview();
    
    resize( 600, 400 );
}

/*================================================================*/
void KPPresStructView::setupSlideList()
{
    slides = new QListView( hsplit );
    slides->addColumn( i18n( "Slide Nr." ) );
    slides->addColumn( i18n( "Slide Title" ) );
    slides->header()->setMovingEnabled( FALSE );
    slides->setAllColumnsShowFocus( TRUE );
    slides->setRootIsDecorated( TRUE );
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
		item_->setObject( doc->objectList()->at( j ) );
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
    showPreview->setChecked( TRUE );
    // #### for now
    showPreview->setEnabled( FALSE );
    
    slidePreview = new KPSlidePreview( box, doc, view );
    connect( slides, SIGNAL( selectionChanged( QListViewItem * ) ),
	     slidePreview, SLOT( setPage( QListViewItem * ) ) );
}

/*================================================================*/
void KPPresStructView::resizeEvent( QResizeEvent *e )
{
    QDialog::resizeEvent( e );
    hsplit->resize( size() );
}
