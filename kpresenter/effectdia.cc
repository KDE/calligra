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

#include "kpresenter_view.h"
#include "effectdia.h"
#include "effectdia.moc"
#include "effectcmd.h"
#include "kpobject.h"

#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlineedit.h>
#include <qvaluelist.h>

#include <kapp.h>
#include <klocale.h>
#include <kbuttonbox.h>

/******************************************************************/
/* class EffectDia                                                */
/******************************************************************/

/*================================================================*/
EffectDia::EffectDia( QWidget* parent, const char* name, const QList<KPObject>& _objs, KPresenterView *_view )
    : QDialog( parent, name, TRUE ), objs( _objs )
{
    view = _view;
    KPObject *obj = objs.at( 0 );

    back = new QVBox( this );
    back->setMargin( 10 );
    back->setSpacing( 5 );

    QGroupBox *grp1 = new QGroupBox( 4, Qt::Horizontal, i18n( "Appear" ), back );

    lNum = new QLabel( i18n( "Number: " ), grp1 );
    lNum->setAlignment( AlignVCenter );

    eNum = new QSpinBox( 0, 100, 1, grp1 );
    eNum->setValue( obj->getPresNum() );

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
    cEffect->setCurrentItem( static_cast<int>( obj->getEffect() ) );

    lEffect2 = new QLabel( i18n( "Effect (object specific): " ), grp1 );
    lEffect2->setAlignment( AlignVCenter );

    cEffect2 = new QComboBox( false, grp1, "cEffect2" );
    cEffect2->insertItem( i18n( "No Effect" ) );

    switch ( obj->getType() ) {
    case OT_TEXT: {
        cEffect2->insertItem( i18n( "Paragraph after paragraph" ) );
    } break;
    default: break;
    }

    if ( obj->getEffect2() == EF2_NONE )
        cEffect2->setCurrentItem( static_cast<int>( obj->getEffect2() ) );
    else {
        switch ( obj->getType() )
        {
        case OT_TEXT:
            cEffect2->setCurrentItem( static_cast<int>( obj->getEffect2() + TxtObjOffset ) );
            break;
        default: break;
        }
    }

    disappear = new QCheckBox( i18n( "Disappear" ), back );
    disappear->setChecked( obj->getDisappear() );

    QGroupBox *grp2 = new QGroupBox( 2, Qt::Horizontal, back );

    lDisappear = new QLabel( i18n( "Number: " ), grp2 );
    lDisappear->setAlignment( AlignVCenter );

    eDisappear = new QSpinBox( 0, 100, 1, grp2 );
    eDisappear->setValue( obj->getDisappearNum() );

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
    cDisappear->setCurrentItem( static_cast<int>( obj->getEffect3() ) );

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

    resize( 630, 260 );
}

/*================================================================*/
void EffectDia::slotEffectDiaOk()
{
    QValueList<EffectCmd::EffectStruct> oldEffects;
    for ( unsigned int i = 0; i < objs.count(); ++i ) {
	KPObject *o = objs.at( i );
	EffectCmd::EffectStruct e;
	e.presNum = o->getPresNum();
	e.disappearNum = o->getDisappearNum();
	e.effect = o->getEffect();
	e.effect2 = o->getEffect2();
	e.effect3 = o->getEffect3();
	e.disappear = o->getDisappear();
	oldEffects << e;
    }

    EffectCmd::EffectStruct eff;
    eff.presNum = eNum->value();
    eff.disappearNum = eDisappear->value();
    eff.effect = ( Effect )cEffect->currentItem();
    eff.effect2 = ( Effect2 )cEffect2->currentItem();
    eff.effect3 = ( Effect3 )cDisappear->currentItem();
    eff.disappear = disappear->isChecked();

    EffectCmd *effectCmd = new EffectCmd( i18n( "Assign Object Effects" ), objs,
					  oldEffects, eff );
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
