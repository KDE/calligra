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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kchartParameter3dConfigPage.h"
#include "kchartParameter3dConfigPage.moc"

#include <kapplication.h>
#include <kdialog.h>
#include <klocale.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>

#include "kchart_params.h"

namespace KChart
{

KChartParameter3dConfigPage::KChartParameter3dConfigPage( KChartParams* params,
                                                          QWidget* parent ) :
    QWidget( parent ),m_params( params )
{
  QGridLayout* layout = new QGridLayout(this, 2, 2,KDialog::marginHint(),KDialog::spacingHint());


  QButtonGroup* gb = new QButtonGroup( 0, Qt::Vertical, 
				       i18n("3D Parameters"), this );
  gb->layout()->setSpacing(KDialog::spacingHint());
  gb->layout()->setMargin(KDialog::marginHint());

  // The grid layout inside the buttongroup.
  QGridLayout *grid1 = new QGridLayout(gb->layout(), 5, 3);
  layout->addWidget(gb,0,0);

  // The main on/off checkbox.
  bar3d=new QCheckBox(i18n("3D bar"),gb);
  grid1->addWidget(bar3d,0,0);

  connect(bar3d, SIGNAL(toggled ( bool )),
	  this,  SLOT(slotChange3DParameter(bool)));

  // Checkbox for shadows
  drawShadowColor=new QCheckBox(i18n("Draw dark shadow"),gb);
  grid1->addWidget(drawShadowColor,1,0);

  QLabel *tmpLabel = new QLabel( i18n( "Angle:" ), gb );
  tmpLabel->resize( tmpLabel->sizeHint() );
  grid1->addWidget(tmpLabel,2,0);

  angle3d=new KIntNumInput(0, gb, 10);
  grid1->addWidget(angle3d,2,1);
  angle3d->setRange(0, 90, 1);

  tmpLabel = new QLabel( i18n( "Depth:" ), gb );
  tmpLabel->resize( tmpLabel->sizeHint() );
  grid1->addWidget(tmpLabel,3,0);

  depth=new KDoubleNumInput(0, gb);
  depth->resize(100,depth->sizeHint().height());
  grid1->addWidget(depth,3,1);
  depth->setRange(0, 2.0, 0.1);
  
  gb->setAlignment(Qt::AlignLeft);
  grid1->addColSpacing(0,depth->width());
  grid1->addColSpacing(0,angle3d->width());
  grid1->setColStretch(2,1);
  grid1->setRowStretch(4,1);

  //it's not good but I don't know how
  //to reduce space
  //layout->addColSpacing(1,300);
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
