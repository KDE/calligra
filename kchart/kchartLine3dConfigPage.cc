/* This file is part of the KDE project
   Copyright (C) 2000,2001,2002,2003,2004 Laurent Montel <montel@kde.org>

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

#include "kchartLine3dConfigPage.h"
#include "kchartLine3dConfigPage.moc"

#include <kapplication.h>
#include <klocale.h>
#include <kdialog.h>
#include <QLayout>
#include <QLabel>
#include <QCheckBox>
#include <q3buttongroup.h>

//Added by qt3to4:
#include <Q3GridLayout>

#include "kchart_params.h"

namespace KChart
{

KChartLine3dConfigPage::KChartLine3dConfigPage( KChartParams* params,
                                                          QWidget* parent ) :
    QWidget( parent ),_params( params )
{
  Q3GridLayout *grid1 = new Q3GridLayout(this,8,3,KDialog::marginHint(), KDialog::spacingHint());

#if 0
  // The 3D line on/off button.
  line3d=new QCheckBox(i18n("3D lines"),this);
  grid1->addWidget(line3d,1,0);

  connect(line3d, SIGNAL(toggled ( bool )),
	  this,   SLOT(slotChange3DParameter(bool)));
#endif

  // The line width label and input.
  QLabel *tmpLabel = new QLabel( i18n( "Line width:" ), this );
  grid1->addWidget(tmpLabel,2,0);
  lineWidth=new KIntNumInput(0, this, 10);
  lineWidth->setWhatsThis( i18n("You can set here the line width for your chart. Default is 1."));
  grid1->addWidget(lineWidth,2,1);

  lineMarkers = new QCheckBox( i18n( "Line markers" ), this );
  lineMarkers->setWhatsThis( i18n("Check this option if you want to add dots on your chart as markers."));
  grid1->addWidget(lineMarkers, 3, 0);

#if 0
  // The "Draw shadow color" checkbox
  drawShadowColor=new QCheckBox(i18n("Draw shadow color"),this);
  grid1->addWidget(drawShadowColor,3,0);

  tmpLabel = new QLabel( i18n( "Rotation around the X-axis in degrees:" ), 
			 this );
  tmpLabel->resize( tmpLabel->sizeHint() );
  grid1->addWidget(tmpLabel,4,0);

  angle3dX=new KIntNumInput(0, this, 10);
  grid1->addWidget(angle3dX,4,1);
  angle3dX->setRange(0, 90, 1);

  tmpLabel = new QLabel( i18n( "Rotation around the Y-axis in degrees:" ), this );
  tmpLabel->resize( tmpLabel->sizeHint() );
  grid1->addWidget(tmpLabel,5,0);

  angle3dY=new KIntNumInput(0, this, 10);
  grid1->addWidget(angle3dY,5,1);
  angle3dY->setRange(0, 90, 1);


  tmpLabel = new QLabel( i18n( "Depth:" ), this );
  tmpLabel->resize( tmpLabel->sizeHint() );
  grid1->addWidget(tmpLabel,6,0);

  depth=new KDoubleNumInput(0, this);
  depth->resize(100,depth->sizeHint().height());
  grid1->addWidget(depth,6,1);
  depth->setRange(0,40, 0.1);
#endif
#if 0
  grid1->addItem( new QSpacerItem(depth->width(), 0 ), 0, 0 );
  grid1->addItem( new QSpacerItem(angle3dX->width(), 0 ), 0, 0 );
#endif
  grid1->setColumnStretch(2,1);
  grid1->setRowStretch(7,1);
  grid1->activate();
  //it's not good but I don't know how
  //to reduce space
  //layout->addItem( new QSpacerItem(300, 0 ), 0, 1 );
}

void KChartLine3dConfigPage::slotChange3DParameter(bool b)
{
#if 0
    angle3dX->setEnabled(b);
    angle3dY->setEnabled(b);
    depth->setEnabled(b);
    drawShadowColor->setEnabled(b);
#endif
    lineWidth->setEnabled(!b);
}


void KChartLine3dConfigPage::init()
{
#if 0
    bool state=_params->threeDLines();
    line3d->setChecked(state);

    angle3dX->setValue( _params->threeDLineXRotation() );
    angle3dY->setValue( _params->threeDLineYRotation() );
    depth->setValue( _params->threeDLineDepth() );
    drawShadowColor->setChecked(_params->threeDShadowColors());
#endif
    lineWidth->setValue(_params->lineWidth());
    lineMarkers->setChecked(_params->lineMarker());
#if 0
    slotChange3DParameter(state);
#endif
}

void KChartLine3dConfigPage::apply()
{
#if 0
    _params->setThreeDLines(line3d->isChecked());
    _params->setThreeDLineXRotation( angle3dX->value() );
    _params->setThreeDLineYRotation( angle3dY->value() );
    _params->setThreeDLineDepth( static_cast<int>( depth->value() ) );
    _params->setThreeDShadowColors( drawShadowColor->isChecked());
#endif
    _params->setLineWidth( lineWidth->value() );
    _params->setLineMarker(lineMarkers->isChecked());
}


}  //KChart namespace
