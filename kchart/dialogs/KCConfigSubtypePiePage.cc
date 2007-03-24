/* This file is part of the KDE project
   Copyright (C) 2001,2002,2003,2004 Laurent Montel <montel@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <QLayout>
#include <QGroupBox>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <qradiobutton.h>
#include <QSpinBox>

#include <klocale.h>
#include <kdebug.h>
#include <kdialog.h>

#include "kchart_params.h"
#include "KCConfigSubtypePiePage.h"


namespace KChart
{

KCConfigSubtypePiePage::KCConfigSubtypePiePage( KChartParams* params,
						QWidget* parent ) :
    QWidget( parent ),_params( params )
{
    QVBoxLayout  *toplevel = new QVBoxLayout( this );
    toplevel->setMargin( 10 );

    QGroupBox* gb = new QGroupBox( i18n( "Parameter" ) );
    toplevel->addWidget(gb);

    QVBoxLayout *layout = new QVBoxLayout( gb );

    pie3d = new QCheckBox(i18n("Pie 3D"), gb);
    pie3d->setWhatsThis( i18n("Check this option if you want a 3D effect for your pie."));
    layout->addWidget( pie3d );

    drawShadowColor=new QCheckBox(i18n("Draw shadow color"), gb);
    drawShadowColor->setWhatsThis( i18n("Check this option if you want a shadow color on a 3D pie."));
    layout->addWidget( drawShadowColor );

    QLabel *label = new QLabel( i18n( "Explode factor (%):" ), gb );
    layout->addWidget( label );

    explode = new QSpinBox( gb );
    explode->setMinimum( 0 );
    explode->setMaximum( 100 );
    explode->setWhatsThis( i18n("This will place gaps between the segments of your pie. Default is 0 which means the pie is a whole."));
    layout->addWidget( explode );

    label = new QLabel( i18n( "Start angle:" ), gb );
    layout->addWidget( label );

    angle = new QSpinBox( gb );
    angle->setMinimum( 0 );
    angle->setMaximum( 90 );
    angle->setWhatsThis( i18n("This will set the orientation of your pie. Default is 0."));
    layout->addWidget( angle );

    label = new QLabel( i18n( "3D-depth:" ), gb );
    layout->addWidget( label );

    depth = new QSpinBox( gb );
    depth->setMinimum( 0 );
    depth->setMaximum( 40 );
    depth->setWhatsThis( i18n("Set the depth from 0 to 40 of the 3D effect, if you have checked Pie 3D. Default is 20."));
    layout->addWidget( depth );

    layout->addStretch( 1 );

    connect(pie3d,SIGNAL(toggled ( bool )),this, SLOT(active3DPie(bool)));
}

void KCConfigSubtypePiePage::active3DPie(bool b)
{
    drawShadowColor->setEnabled(b);
    depth->setEnabled(b);
}

void KCConfigSubtypePiePage::init()
{
    bool state=_params->threeDPies();
    pie3d->setChecked(state);
    depth->setEnabled(state);
    active3DPie(state);

    explode->setValue((int)(_params->explodeFactor() * 100));
    depth->setValue( _params->threeDPieHeight() );
    drawShadowColor->setChecked(_params->threeDShadowColors());
    angle->setValue( _params->pieStart() );

}


void KCConfigSubtypePiePage::apply()
{
     _params->setThreeDPies( pie3d->isChecked() );
     if( _params->threeDPies() )	{
        _params->setThreeDPieHeight( depth->value() );
    }
    _params->setThreeDShadowColors( drawShadowColor->isChecked());
    _params->setExplodeFactor(((double)(explode->value()))/100.0);
    _params->setPieStart( angle->value() );
}

}  //KChart namespace

#include "KCConfigSubtypePiePage.moc"

