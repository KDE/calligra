/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Insert Dialog                                          */
/******************************************************************/

#include "kword_doc.h"
#include "frame.h"
#include "kword_page.h"
#include "insdia.h"
#include "insdia.moc"

#include <klocale.h>

#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qstring.h>
#include <qevent.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qpainter.h>

#include <stdlib.h>

/******************************************************************/
/* Class: KWInsertDia                                             */
/******************************************************************/

/*================================================================*/
KWInsertDia::KWInsertDia( QWidget *parent, const char *name, KWGroupManager *_grpMgr, KWordDocument *_doc, InsertType _type, KWPage *_page )
    : QTabDialog( parent, name, true )
{
    type = _type;
    grpMgr = _grpMgr;
    doc = _doc;
    page = _page;

    setupTab1();

    setCancelButton( i18n( "Cancel" ) );
    setOkButton( i18n( "OK" ) );

    resize( 300, 250 );
}

/*================================================================*/
void KWInsertDia::setupTab1()
{
    tab1 = new QWidget( this );

    grid1 = new QGridLayout( tab1, 3, 1, 15, 7 );

    QButtonGroup *grp = new QButtonGroup( type == ROW ? i18n( "Insert new Row" ) : i18n( "Insert New Column" ), tab1 );
    grp->setExclusive( true );

    grid2 = new QGridLayout( grp, 3, 1, 7, 7 );

    rBefore = new QRadioButton( i18n( "Before" ), grp );
    rBefore->resize( rBefore->sizeHint() );
    grp->insert( rBefore );
    grid2->addWidget( rBefore, 1, 0 );

    rAfter = new QRadioButton( i18n( "After" ), grp );
    rAfter->resize( rAfter->sizeHint() );
    grp->insert( rAfter );
    grid2->addWidget( rAfter, 2, 0 );
    rAfter->setChecked( true );

    grid2->addRowSpacing( 0, 7 );
    grid2->addRowSpacing( 1, rBefore->height() );
    grid2->addRowSpacing( 2, rAfter->height() );
    grid2->setRowStretch( 0, 0 );
    grid2->setRowStretch( 1, 0 );
    grid2->setRowStretch( 1, 0 );

    grid2->addColSpacing( 0, rBefore->width() );
    grid2->addColSpacing( 0, rAfter->width() );
    grid2->setColStretch( 0, 1 );

    grid2->activate();

    grid1->addWidget( grp, 0, 0 );

    rc = new QLabel( type == ROW ? i18n( "Row:" ) : i18n( "Column:" ), tab1 );
    rc->resize( rc->sizeHint() );
    rc->setAlignment( AlignLeft | AlignBottom );
    grid1->addWidget( rc, 1, 0 );

    value = new QSpinBox( 1, type == ROW ? grpMgr->getRows() : grpMgr->getCols(), 1, tab1 );
    value->resize( value->sizeHint() );
    value->setValue( type == ROW ? grpMgr->getRows() : grpMgr->getCols() );
    grid1->addWidget( value, 2, 0 );

    grid1->addRowSpacing( 0, grp->height() );
    grid1->addRowSpacing( 1, rc->height() );
    grid1->addRowSpacing( 2, value->height() );
    grid1->setRowStretch( 0, 0 );
    grid1->setRowStretch( 1, 1 );
    grid1->setRowStretch( 2, 0 );

    grid1->addColSpacing( 0, grp->width() );
    grid1->addColSpacing( 0, rc->width() );
    grid1->addColSpacing( 0, value->width() );
    grid1->setColStretch( 0, 1 );

    grid1->activate();

    addTab( tab1, type == ROW ? i18n( "Insert Row" ) : i18n( "Insert Column" ) );

    connect( this, SIGNAL( applyButtonPressed() ), this, SLOT( doInsert() ) );

    resize(minimumSize());
}

/*================================================================*/
void KWInsertDia::doInsert()
{
    if ( type == ROW )
        grpMgr->insertRow( value->value() - ( rBefore->isChecked() ? 1 : 0 ) );
    else
        grpMgr->insertCol( value->value() - ( rBefore->isChecked() ? 1 : 0 ) );

    doc->recalcFrames();
    doc->updateAllFrames();
    doc->updateAllViews( 0L );
    page->recalcCursor();
}
