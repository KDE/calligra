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

#include <klocale.h>

#include <qlistview.h>
#include <qsplitter.h>
#include <qevent.h>
#include <qheader.h>

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
    
    for ( unsigned int i = 0; i < doc->getPageNums(); ++i ) {
        QListViewItem *item = new QListViewItem( slides );
        item->setText( 0, QString( "%1" ).arg( i + 1 ) );
        item->setText( 1, doc->getPageTitle( i, i18n( "Slide %1" ).arg( i +1 ) ) );
	for ( unsigned int j = 0; j < doc->objNums(); ++j ) {
	    if ( doc->getPageOfObj( j, 0, 0 ) == (int)i + 1 ) {
		( new QListViewItem( item ) )->setText( 0, "Object" );
	    }
	}
    }
}

/*================================================================*/
void KPPresStructView::resizeEvent( QResizeEvent *e )
{
    QDialog::resizeEvent( e );
    hsplit->resize( size() );
}
