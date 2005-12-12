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

#include "kchartParameterPolarConfigPage.h"

#include "kchartParameterPolarConfigPage.moc"

#include <kapplication.h>
#include <klocale.h>
#include <kdebug.h>
#include <kdialog.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qvbuttongroup.h>
#include <qwhatsthis.h>

#include "kchart_params.h"

namespace KChart
{

KChartParameterPolarConfigPage::KChartParameterPolarConfigPage( KChartParams* params,
                                                            QWidget* parent ) :
    QWidget( parent ),_params( params )
{
    QVBoxLayout *grid1 = new QVBoxLayout(this);
    QVButtonGroup* gb = new QVButtonGroup( i18n( "Parameter" ), this );
    grid1->addWidget(gb);

    polarMarker=new QCheckBox(i18n("Polar marker"), gb);
    QWhatsThis::add(polarMarker, i18n("If this is checked, the polar markers are shown; otherwise they are not."));
    showCircularLabel = new QCheckBox(i18n("Show circular label"), gb);
    QWhatsThis::add(showCircularLabel, i18n("Toggle the circular label display."));

    QLabel *label = new QLabel( i18n( "Zero degree position:" ), gb );
    angle = new QSpinBox(-359, 359, 1, gb);
    QWhatsThis::add(angle, i18n("Set the position for the X axis (horizontal) from -359 to 359. Default is 0."));

    label=new QLabel(i18n("Line width:"), gb);
    lineWidth=new QSpinBox(gb);
    QWhatsThis::add(lineWidth, i18n("Set the width for the chart lines. 0 is default and is the thinnest."));

    grid1->activate();
}


void KChartParameterPolarConfigPage::init()
{
    polarMarker->setChecked(_params->polarMarker());
    angle->setValue( _params->polarZeroDegreePos() );
    showCircularLabel->setChecked(_params->polarRotateCircularLabels());
    lineWidth->setValue(_params->polarLineWidth());
}


void KChartParameterPolarConfigPage::apply()
{
    _params->setPolarZeroDegreePos(angle->value());
    _params->setPolarMarker(polarMarker->isChecked());
    _params->setPolarRotateCircularLabels(showCircularLabel->isChecked());
    _params->setPolarLineWidth(lineWidth->value());
}

}  //KChart namespace
