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
/* Module: Effect Dialog                                          */
/******************************************************************/

#include "kpresenter_view.h"
#include "effectdia.h"
#include "effectdia.moc"
#include "effectcmd.h"
#include "kpobject.h"

#include <qlabel.h>
#include <qpushbt.h>
#include <qbttngrp.h>
#include <qcombo.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlineedit.h>

#include <kapp.h>
#include <klocale.h>
#include <kbuttonbox.h>

#include <stdio.h>
#include <stdlib.h>

/******************************************************************/
/* class EffectDia                                                */
/******************************************************************/

/*================================================================*/
EffectDia::EffectDia( QWidget* parent, const char* name, int _pageNum, int _objNum, KPresenterView *_view )
    : QDialog( parent, name, true )
{
    pageNum = _pageNum;
    objNum = _objNum;
    view = _view;

    back = new QVBox( this );
    back->setMargin( 10 );
    back->setSpacing( 5 );

    QGroupBox *grp1 = new QGroupBox( 4, Qt::Horizontal, i18n( "Appear" ), back );

    lNum = new QLabel( i18n( "Number: " ), grp1 );
    lNum->setAlignment( AlignVCenter );

    eNum = new QSpinBox( 0, 100, 1, grp1 );
    eNum->setValue( view->kPresenterDoc()->objectList()->at(_objNum)->getPresNum() );

    ( void )new QWidget( grp1 );
    ( void )new QWidget( grp1 );

    lEffect = new QLabel( i18n( "Effect (appearing): " ), grp1 );
    lEffect->setAlignment( AlignVCenter );

    cEffect = new QComboBox( false, grp1, "cEffect" );
    cEffect->insertItem( i18n( "No Effect" ) );
    cEffect->insertItem( i18n( "Come from right" ) );
    cEffect->insertItem( i18n( "Come from left" ) );
    cEffect->insertItem( i18n( "Come from top" ) );
    cEffect->insertItem( i18n( "Come from bottom" ) );
    cEffect->insertItem( i18n( "Come from right/top" ) );
    cEffect->insertItem( i18n( "Come from right/bottom" ) );
    cEffect->insertItem( i18n( "Come from left/top" ) );
    cEffect->insertItem( i18n( "Come from left/bottom" ) );
    cEffect->insertItem( i18n( "Wipe from left" ) );
    cEffect->insertItem( i18n( "Wipe from right" ) );
    cEffect->insertItem( i18n( "Wipe from top" ) );
    cEffect->insertItem( i18n( "Wipe from bottom" ) );
    cEffect->setCurrentItem( static_cast<int>( view->kPresenterDoc()->objectList()->at( _objNum )->getEffect() ) );

    lEffect2 = new QLabel( i18n( "Effect (object specific): " ), grp1 );
    lEffect2->setAlignment( AlignVCenter );

    cEffect2 = new QComboBox( false, grp1, "cEffect2" );
    cEffect2->insertItem( i18n( "No Effect" ) );

    switch ( view->kPresenterDoc()->objectList()->at( _objNum )->getType() )
    {
    case OT_TEXT:
    {
        cEffect2->insertItem( i18n( "Paragraph after paragraph" ) );
    } break;
    default: break;
    }

    if ( view->kPresenterDoc()->objectList()->at( _objNum )->getEffect2() == EF2_NONE )
        cEffect2->setCurrentItem( static_cast<int>( view->kPresenterDoc()->objectList()->at( _objNum )->getEffect2() ) );
    else
    {
        switch ( view->kPresenterDoc()->objectList()->at( _objNum )->getType() )
        {
        case OT_TEXT:
            cEffect2->setCurrentItem( static_cast<int>( view->kPresenterDoc()->objectList()->at( _objNum )->getEffect2() + TxtObjOffset ) );
            break;
        default: break;
        }
    }

    disappear = new QCheckBox( i18n( "Disappear" ), back );
    disappear->setChecked( view->kPresenterDoc()->objectList()->at(_objNum)->getDisappear() );

    QGroupBox *grp2 = new QGroupBox( 2, Qt::Horizontal, back );

    lDisappear = new QLabel( i18n( "Number: " ), grp2 );
    lDisappear->setAlignment( AlignVCenter );

    eDisappear = new QSpinBox( 0, 100, 1, grp2 );
    eDisappear->setValue( view->kPresenterDoc()->objectList()->at(_objNum)->getDisappearNum() );

    lDEffect = new QLabel( i18n( "Effect (disappearing): " ), grp2 );
    lDEffect->setAlignment( AlignVCenter );

    cDisappear = new QComboBox( false, grp2, "cDisappear" );
    cDisappear->insertItem( i18n( "No Effect" ) );
    cDisappear->insertItem( i18n( "Disappear to the right" ) );
    cDisappear->insertItem( i18n( "Disappear to the left" ) );
    cDisappear->insertItem( i18n( "Disappear to the top" ) );
    cDisappear->insertItem( i18n( "Disappear to the bottom" ) );
    cDisappear->insertItem( i18n( "Disappear to the right/top" ) );
    cDisappear->insertItem( i18n( "Disappear to the right/bottom" ) );
    cDisappear->insertItem( i18n( "Disappear to the left/top" ) );
    cDisappear->insertItem( i18n( "Disappear to the left/bottom" ) );
    cDisappear->insertItem( i18n( "Wipe to the left" ) );
    cDisappear->insertItem( i18n( "Wipe to the right" ) );
    cDisappear->insertItem( i18n( "Wipe to the top" ) );
    cDisappear->insertItem( i18n( "Wipe to the bottom" ) );
    cDisappear->setCurrentItem( static_cast<int>( view->kPresenterDoc()->objectList()->at( _objNum )->getEffect3() ) );

    ( void )new QWidget( back );

    KButtonBox *bb = new KButtonBox( back );
    bb->addStretch();

    okBut = bb->addButton( i18n( "OK" ) );
    okBut->setAutoRepeat( false );
    okBut->setAutoResize( false );
    okBut->setAutoDefault( true );
    okBut->setDefault( true );

    cancelBut = bb->addButton( i18n( "Cancel" ) );

    bb->layout();

    bb->setMaximumHeight( bb->sizeHint().height() );

    connect( okBut, SIGNAL( clicked() ), this, SLOT( slotEffectDiaOk() ) );
    connect( cancelBut, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( okBut, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( disappear, SIGNAL( clicked() ), this, SLOT( disappearChanged() ) );
    disappearChanged();

    resize( 630, 300 );
}

/*================================================================*/
void EffectDia::slotEffectDiaOk()
{
    EffectCmd *effectCmd = new EffectCmd( i18n( "Assign Object Effects" ), eNum->value(),
                                          ( Effect )cEffect->currentItem(), ( Effect2 )cEffect2->currentItem(),
                                          disappear->isChecked(), ( Effect3 )cDisappear->currentItem(),
                                          eDisappear->value(),
                                          view->kPresenterDoc()->objectList()->at( objNum )->getPresNum(),
                                          view->kPresenterDoc()->objectList()->at( objNum )->getEffect(),
                                          view->kPresenterDoc()->objectList()->at( objNum )->getEffect2(),
                                          view->kPresenterDoc()->objectList()->at( objNum )->getDisappear(),
                                          view->kPresenterDoc()->objectList()->at( objNum )->getEffect3(),
                                          view->kPresenterDoc()->objectList()->at( objNum )->getDisappearNum(),
                                          view->kPresenterDoc()->objectList()->at( objNum ) );
    effectCmd->execute();
    view->kPresenterDoc()->commands()->addCommand( effectCmd );
    emit effectDiaOk();
}

/*================================================================*/
void EffectDia::resizeEvent( QResizeEvent *e )
{
    QDialog::resizeEvent( e );
    back->resize( size() );
}

/*================================================================*/
void EffectDia::disappearChanged()
{
    cDisappear->setEnabled( disappear->isChecked() );
    eDisappear->setEnabled( disappear->isChecked() );
}

/*================================================================*/
void EffectDia::num1Changed( int /*num*/ )
{
}

/*================================================================*/
void EffectDia::num2Changed( int /*num*/ )
{
}
