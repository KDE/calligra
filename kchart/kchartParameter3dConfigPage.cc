/*
 * Copyright 2001 by Laurent Montel, released under Artistic License.
 */

#include "kchartParameter3dConfigPage.h"
#include "kchartParameter3dConfigPage.moc"

#include <kapplication.h>
#include <klocale.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>

#include "kchart_params.h"

KChartParameter3dConfigPage::KChartParameter3dConfigPage( KChartParams* params,
                                                          QWidget* parent ) :
    QWidget( parent ),_params( params )
{
  QGridLayout* layout = new QGridLayout(this, 2, 2,15,7 );

  QButtonGroup* gb = new QButtonGroup( i18n("3D Parameters"), this );
  QGridLayout *grid1 = new QGridLayout(gb,3,2,15,7);
  layout->addWidget(gb,0,0);

  bar3d=new QCheckBox(i18n("3D Bar"),gb);
  grid1->addWidget(bar3d,0,0);

  connect(bar3d, SIGNAL(toggled ( bool )),this,SLOT(slotChange3DParameter(bool)));

  drawShadowColor=new QCheckBox(i18n("Draw Shadow color"),gb);
  grid1->addWidget(drawShadowColor,1,0);

  QLabel *tmpLabel = new QLabel( i18n( "Angle" ), gb );
  tmpLabel->resize( tmpLabel->sizeHint() );
  tmpLabel->setAlignment(Qt::AlignCenter);
  grid1->addWidget(tmpLabel,2,0);

  angle3d=new KIntNumInput(0, gb, 10);
  grid1->addWidget(angle3d,2,1);
  angle3d->setRange(0, 90, 1);



  tmpLabel = new QLabel( i18n( "Depth" ), gb );
  tmpLabel->resize( tmpLabel->sizeHint() );
  tmpLabel->setAlignment(Qt::AlignCenter);
  grid1->addWidget(tmpLabel,3,0);

  depth=new KDoubleNumInput(0, gb);
  depth->resize(100,depth->sizeHint().height());
  grid1->addWidget(depth,3,1);
  depth->setRange(0,40, 0.1);

  gb->setAlignment(Qt::AlignLeft);
  grid1->addColSpacing(0,depth->width());
  grid1->addColSpacing(0,angle3d->width());
  grid1->setColStretch(0,1);
  grid1->activate();
  //it's not good but I don't know how
  //to reduce space
  layout->addColSpacing(1,300);
}

void KChartParameter3dConfigPage::slotChange3DParameter(bool b)
{
    angle3d->setEnabled(b);
    depth->setEnabled(b);
    drawShadowColor->setEnabled(b);
}

void KChartParameter3dConfigPage::init()
{
    bool state=_params->threeDBars();
    bar3d->setChecked(state);

    drawShadowColor->setChecked(_params->threeDShadowColors());
    angle3d->setValue( _params->threeDBarAngle() );
    depth->setValue( _params->threeDBarDepth() );
    slotChange3DParameter(state);
}

void KChartParameter3dConfigPage::apply()
{
    _params->setThreeDBars(bar3d->isChecked());
    _params->setThreeDBarAngle( angle3d->value() );
    _params->setThreeDBarDepth( depth->value() );
    _params->setThreeDShadowColors( drawShadowColor->isChecked());
}
