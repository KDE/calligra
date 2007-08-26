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

#include <kdialog.h>
#include <klocale.h>

#include "kchart_params.h"
#include "KCConfigSubtypeBarPage.h"


namespace KChart
{

KCConfigSubtypeBarPage::KCConfigSubtypeBarPage( KChartParams* params,
						QWidget* parent ) :
    QWidget( parent ),m_params( params )
{
  // The outer layout that will only contain the groupbox.
  QVBoxLayout* layout0 = new QVBoxLayout( this );
  layout0->setMargin( KDialog::marginHint() );
  layout0->setSpacing( KDialog::spacingHint() );

  QGroupBox* gb = new QGroupBox( i18n("3D Parameters") );
  layout0->addWidget( gb );

  // The inner layout that will layout the actual contents of the widgets.
  QGridLayout* layout = new QGridLayout( );
  layout->setSpacing(KDialog::spacingHint());
  layout->setMargin(KDialog::marginHint());

  // The main on/off checkbox.
  bar3d = new QCheckBox( i18n("3D bar") );
  bar3d->setWhatsThis( i18n("If checked, this will enable 3D mode for viewing the bars. You can then add a shadow and set the angle and depth for 3D."));
  layout->addWidget( bar3d, 0, 0, 1, 2);

  connect(bar3d, SIGNAL(toggled ( bool )),
	  this,  SLOT(slotChange3DParameter(bool)));

  // Checkbox for shadows
  drawShadowColor = new QCheckBox( i18n("Draw dark shadow") );
  drawShadowColor->setWhatsThis( i18n("If checked, this will add a dark shadow on the 3D bars."));
  layout->addWidget( drawShadowColor, 1, 0, 1, 2);

  QLabel *tmpLabel = new QLabel( i18n( "Angle:" ) );
  tmpLabel->resize( tmpLabel->sizeHint() );  // FIXME: Why?
  layout->addWidget( tmpLabel, 2, 0 );

  angle3d = new KIntNumInput( 0, gb, 10 );
  angle3d->setRange(0, 90, 1);
  angle3d->setWhatsThis( i18n("You can set here the angle for the 3D effect from 0 to 90. 90 will give you flat bars without any 3D effect.\nDefault is 45."));
  layout->addWidget( angle3d, 2, 1 );

  tmpLabel = new QLabel( i18n( "Depth:" ) );
  tmpLabel->resize( tmpLabel->sizeHint() );
  layout->addWidget( tmpLabel, 3, 0 );

  depth = new KDoubleNumInput( gb );
  depth->setRange(0, 2.0, 0.1);
  depth->setWhatsThis( i18n("You can set here the depth of the 3D effect from 0 to 2. 0 will give you no depth at all.\nDefault is 1."));
  depth->resize( 100, depth->sizeHint().height() );
  layout->addWidget( depth, 3, 1 );
  
  gb->setAlignment(Qt::AlignLeft);
  //layout->addItem( new QSpacerItem(depth->width(), 0 ), 0, 0 );
  //layout->addItem( new QSpacerItem(angle3d->width(), 0 ), 0, 0 );
  layout->setColumnStretch(2,1);
  layout->setRowStretch(4,1);

  gb->setLayout( layout );

  //it's not good but I don't know how
  //to reduce space
  //layout->addItem( new QSpacerItem(300, 0 ), 0, 1 );
}


void KCConfigSubtypeBarPage::slotChange3DParameter(bool b)
{
    angle3d->setEnabled(b);
    depth->setEnabled(b);
    drawShadowColor->setEnabled(b);
}


void KCConfigSubtypeBarPage::init()
{
    bool state=m_params->threeDBars();
    bar3d->setChecked(state);

    drawShadowColor->setChecked(m_params->threeDShadowColors());
    angle3d->setValue( m_params->threeDBarAngle() );
    depth->setValue( m_params->threeDBarDepth() );
    slotChange3DParameter(state);
}


void KCConfigSubtypeBarPage::apply()
{
    m_params->setThreeDBars(bar3d->isChecked());
    m_params->setThreeDBarAngle( angle3d->value() );
    m_params->setThreeDBarDepth( depth->value() );
    m_params->setThreeDShadowColors( drawShadowColor->isChecked());
}


}  //KChart namespace

#include "KCConfigSubtypeBarPage.moc"
