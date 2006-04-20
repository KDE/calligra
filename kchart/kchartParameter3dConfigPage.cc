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

#include "kchartParameter3dConfigPage.h"
#include "kchartParameter3dConfigPage.moc"

#include <kapplication.h>
#include <kdialog.h>
#include <klocale.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <q3buttongroup.h>

//Added by qt3to4:
#include <Q3GridLayout>

#include "kchart_params.h"

namespace KChart
{

KChartParameter3dConfigPage::KChartParameter3dConfigPage( KChartParams* params,
                                                          QWidget* parent ) :
    QWidget( parent ),m_params( params )
{
  Q3GridLayout* layout = new Q3GridLayout(this, 2, 2,KDialog::marginHint(),KDialog::spacingHint());


  Q3ButtonGroup* gb = new Q3ButtonGroup( 0, Qt::Vertical, 
				       i18n("3D Parameters"), this );
  gb->layout()->setSpacing(KDialog::spacingHint());
  gb->layout()->setMargin(KDialog::marginHint());

  // The grid layout inside the buttongroup.
  Q3GridLayout *grid1 = new Q3GridLayout(gb->layout(), 5, 3);
  layout->addWidget(gb,0,0);

  // The main on/off checkbox.
  bar3d=new QCheckBox(i18n("3D bar"),gb);
  bar3d->setWhatsThis( i18n("If checked, this will enable 3D mode for viewing the bars. You can then add a shadow and set the angle and depth for 3D."));
  grid1->addWidget(bar3d,0,0);

  connect(bar3d, SIGNAL(toggled ( bool )),
	  this,  SLOT(slotChange3DParameter(bool)));

  // Checkbox for shadows
  drawShadowColor=new QCheckBox(i18n("Draw dark shadow"),gb);
  drawShadowColor->setWhatsThis( i18n("If checked, this will add a dark shadow on the 3D bars."));
  grid1->addWidget(drawShadowColor,1,0);

  QLabel *tmpLabel = new QLabel( i18n( "Angle:" ), gb );
  tmpLabel->resize( tmpLabel->sizeHint() );
  grid1->addWidget(tmpLabel,2,0);

  angle3d=new KIntNumInput(0, gb, 10);
  angle3d->setWhatsThis( i18n("You can set here the angle for the 3D effect from 0 to 90. 90 will give you flat bars without any 3D effect.\nDefault is 45."));
  grid1->addWidget(angle3d,2,1);
  angle3d->setRange(0, 90, 1);

  tmpLabel = new QLabel( i18n( "Depth:" ), gb );
  tmpLabel->resize( tmpLabel->sizeHint() );
  grid1->addWidget(tmpLabel,3,0);

  depth=new KDoubleNumInput(gb);
  depth->setWhatsThis( i18n("You can set here the depth of the 3D effect from 0 to 2. 0 will give you no depth at all.\nDefault is 1."));
  depth->resize(100,depth->sizeHint().height());
  grid1->addWidget(depth,3,1);
  depth->setRange(0, 2.0, 0.1);
  
  gb->setAlignment(Qt::AlignLeft);
  grid1->addItem( new QSpacerItem(depth->width(), 0 ), 0, 0 );
  grid1->addItem( new QSpacerItem(angle3d->width(), 0 ), 0, 0 );
  grid1->setColStretch(2,1);
  grid1->setRowStretch(4,1);

  //it's not good but I don't know how
  //to reduce space
  //layout->addItem( new QSpacerItem(300, 0 ), 0, 1 );
}


void KChartParameter3dConfigPage::slotChange3DParameter(bool b)
{
    angle3d->setEnabled(b);
    depth->setEnabled(b);
    drawShadowColor->setEnabled(b);
}


void KChartParameter3dConfigPage::init()
{
    bool state=m_params->threeDBars();
    bar3d->setChecked(state);

    drawShadowColor->setChecked(m_params->threeDShadowColors());
    angle3d->setValue( m_params->threeDBarAngle() );
    depth->setValue( m_params->threeDBarDepth() );
    slotChange3DParameter(state);
}


void KChartParameter3dConfigPage::apply()
{
    m_params->setThreeDBars(bar3d->isChecked());
    m_params->setThreeDBarAngle( angle3d->value() );
    m_params->setThreeDBarDepth( depth->value() );
    m_params->setThreeDShadowColors( drawShadowColor->isChecked());
}


}  //KChart namespace
