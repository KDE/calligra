/*
 * $Id$
 *
 * Copyright 2000 by Laurent Montel, released under Artistic License.
 */

#include "kchartParameter3dConfigPage.h"
#include "kchartParameter3dConfigPage.moc"

#include <kapp.h>
#include <klocale.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qbuttongroup.h>


KChartParameter3dConfigPage::KChartParameter3dConfigPage(KChartParameters* params,QWidget* parent ) :
    QWidget( parent ),_params( params )
{
  //QVBoxLayout* toplevel = new QVBoxLayout( this, 10 );

  QGridLayout* layout = new QGridLayout(this, 2, 2,15,7 );
  //toplevel->addLayout( layout );
  QButtonGroup* gb = new QButtonGroup( i18n("3D Parameters"), this );
  QGridLayout *grid1 = new QGridLayout(gb,3,2,15,7);
  layout->addWidget(gb,0,0);

  QLabel *tmpLabel = new QLabel( i18n( "Angle" ), gb );
  tmpLabel->resize( tmpLabel->sizeHint() );
  tmpLabel->setAlignment(Qt::AlignCenter);
  grid1->addWidget(tmpLabel,0,0);

  angle3d=new KIntNumInput(0, gb, 10);
  grid1->addWidget(angle3d,0,1);
  angle3d->setRange(0, 90, 1);



  tmpLabel = new QLabel( i18n( "Depth" ), gb );
  tmpLabel->resize( tmpLabel->sizeHint() );
  tmpLabel->setAlignment(Qt::AlignCenter);
  grid1->addWidget(tmpLabel,1,0);

  depth=new KDoubleNumInput(0, gb);
  depth->resize(100,depth->sizeHint().height());
  grid1->addWidget(depth,1,1);
  depth->setRange(0, 10, 0.1);


  tmpLabel = new QLabel( i18n( "Bar width" ), gb );
  tmpLabel->resize( tmpLabel->sizeHint() );
  tmpLabel->setAlignment(Qt::AlignCenter);
  grid1->addWidget(tmpLabel,2,0);

  bar_width=new KIntNumInput(0, gb, 10);
  bar_width->resize( 100,bar_width->sizeHint().height() );
  grid1->addWidget(bar_width,2,1);
  bar_width->setRange( 0,100,1 );

  if(_params->do_bar())
        bar_width->setEnabled(true);
  else
        bar_width->setEnabled(false);

  gb->setAlignment(Qt::AlignLeft);
  grid1->addColSpacing(0,bar_width->width());
  grid1->addColSpacing(0,depth->width());
  grid1->addColSpacing(0,angle3d->width());
  grid1->setColStretch(0,1);
  grid1->activate();
  //it's not good but I don't know how
  //to reduce space
  layout->addColSpacing(1,300);
}

void KChartParameter3dConfigPage::init()
{

angle3d->setValue(_params->_3d_angle);



depth->setValue( _params->_3d_depth);

if(_params->do_bar())
        bar_width->setValue( _params->bar_width);
}

void KChartParameter3dConfigPage::apply()
{
_params->_3d_angle=angle3d->value();

_params->_3d_depth=depth->value();

if(_params->do_bar())
        _params->bar_width=bar_width->value();
}
