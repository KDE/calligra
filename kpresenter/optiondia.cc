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
/* Module: Option Dialog                                            */
/******************************************************************/

#include "optiondia.h"
#include "optiondia.moc"

#include <qwidget.h>
#include <qlabel.h>
#include <qchkbox.h>
#include <qstring.h>
#include <qgrpbox.h>
#include <qvalidator.h>

#include <kapp.h>
#include <klocale.h>

#include <stdlib.h>

/******************************************************************/
/* class OptionDia                                                */
/******************************************************************/

/*==================== constructor ===============================*/
OptionDia::OptionDia( QWidget *parent, const char *name )
    :QTabDialog( parent, name, true )
{
    int col1 = 20, dummy, w, h;

    /* Tab: General */
    general = new QWidget( this, i18n( "General" ) );

    lRastX = new QLabel( general, "lRastX" );
    lRastX->setText( i18n( "Horizontal Raster: " ) );
    lRastX->move( col1, 20 );
    lRastX->resize( lRastX->sizeHint() );

    lRastY = new QLabel( general, "lRastY" );
    lRastY->setText( i18n( "Vertikal Raster: " ) );
    lRastY->move( col1, lRastX->y()+lRastX->height()+15 );
    lRastY->resize( lRastY->sizeHint() );

    dummy = max(lRastX->sizeHint().width(),lRastY->sizeHint().width());
    lRastX->resize( dummy, lRastX->height() );
    lRastY->resize( dummy, lRastY->height() );

    eRastX = new QLineEdit( general );
    eRastX->setValidator( new QIntValidator( eRastX ) );
    eRastX->setText( "20" );
    eRastX->move( lRastX->x()+lRastX->width()+10, lRastX->y() );
    eRastX->resize( eRastX->sizeHint().width()/2, eRastX->sizeHint().height() );
    eRastX->setMaxLength( 2 );

    eRastY = new QLineEdit( general );
    eRastY->setValidator( new QIntValidator( eRastY ) );
    eRastY->setText( "20" );
    eRastY->move( lRastY->x()+lRastY->width()+10, lRastY->y() );
    eRastY->resize( eRastY->sizeHint().width()/2, eRastY->sizeHint().height() );
    eRastY->setMaxLength( 2 );

    lRastX->resize( lRastX->width(), eRastX->height() );
    lRastY->resize( lRastY->width(), eRastY->height() );

    general->resize( 20 + eRastX->x() + eRastX->width(), 20 + eRastY->y() + eRastY->height() );

    /* Tab: Objects */
    objects = new QWidget( this, "objects" );

    txtObj = new QGroupBox( i18n( "Objects in Editing-Mode" ), objects, "txtObjs" );
    txtObj->move( col1, col1 );

    lBackCol = new QLabel( txtObj, "lBackCol" );
    lBackCol->setText( i18n( "Backgroud color:" ) );
    lBackCol->move( 10, 20 );
    lBackCol->resize( lBackCol->sizeHint() );

    bBackCol = new KColorButton( white, txtObj, "bBackCol" );
    bBackCol->setGeometry( lBackCol->x() + lBackCol->width() + 10, lBackCol->y(), 100, 25 );

    txtObj->resize( bBackCol->x() + bBackCol->width() + 20, bBackCol->y() + bBackCol->height() + 10 );

    objects->resize( txtObj->width() + 40, txtObj->height() + 40 );

    w = max(objects->width(),general->width());
    h = max(objects->height(),general->height());


    resize( 300, 200 );

    /* build dialog */
    addTab( general, i18n( "General" ) );
    addTab( objects, i18n( "Objects" ) );

    setCancelButton( i18n( "Cancel" ) );
    setApplyButton( i18n( "Apply" ) );
    setOkButton( i18n( "OK" ) );
}

/*===================== destructor ===============================*/
OptionDia::~OptionDia()
{
}
