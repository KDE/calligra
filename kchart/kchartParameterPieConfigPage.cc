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

#include "kchartParameterPieConfigPage.h"

#include "kchartParameterPieConfigPage.moc"

#include <kapplication.h>
#include <klocale.h>
#include <kdebug.h>
#include <kdialog.h>

#include <qlayout.h>
#include <qlabel.h>
#include <q3buttongroup.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qspinbox.h>

//Added by qt3to4:
#include <Q3VBoxLayout>

#include "kchart_params.h"

namespace KChart
{

KChartParameterPieConfigPage::KChartParameterPieConfigPage( KChartParams* params,
                                                            QWidget* parent ) :
    QWidget( parent ),_params( params )
{
    Q3VBoxLayout  *toplevel = new Q3VBoxLayout( this, 10 );
    Q3VBoxLayout  *grid1 = new Q3VBoxLayout(this);
    toplevel->addLayout( grid1 );

    Q3VButtonGroup* gb = new Q3VButtonGroup( i18n( "Parameter" ), this );
    grid1->addWidget(gb);

    pie3d = new QCheckBox(i18n("Pie 3D"), gb);
    pie3d->setWhatsThis( i18n("Uncheck this option if you do not want a 3D effect for your pie."));
    drawShadowColor=new QCheckBox(i18n("Draw shadow color"), gb);
    drawShadowColor->setWhatsThis( i18n("Uncheck this option if you do not want a shadow color on a 3D pie."));

    QLabel *label = new QLabel( i18n( "Explode factor (%):" ), gb );
    explode = new QSpinBox(0, 100, 1, gb);
    explode->setWhatsThis( i18n("This will place gaps between the segments of your pie. Default is 0 which means the pie is a whole."));

    label = new QLabel( i18n( "Start angle:" ), gb );
    angle = new QSpinBox(0, 90, 1, gb);
    angle->setWhatsThis( i18n("This will set the orientation of your pie. Default is 0."));

    label = new QLabel( i18n( "3D-depth:" ), gb );
    depth = new QSpinBox(0, 40, 1, gb);
    depth->setWhatsThis( i18n("Set the depth from 0 to 40 of the 3D effect, if you have checked Pie 3D. Default is 20."));

    grid1->activate();

    connect(pie3d,SIGNAL(toggled ( bool )),this, SLOT(active3DPie(bool)));
}

void KChartParameterPieConfigPage::active3DPie(bool b)
{
    drawShadowColor->setEnabled(b);
    depth->setEnabled(b);
}

void KChartParameterPieConfigPage::init()
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


void KChartParameterPieConfigPage::apply()
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
