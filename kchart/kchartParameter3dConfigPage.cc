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
#include <knumvalidator.h>

KChartParameter3dConfigPage::KChartParameter3dConfigPage(KChartParameters* params,QWidget* parent ) :
    QWidget( parent ),_params( params )
{
  //QVBoxLayout* toplevel = new QVBoxLayout( this, 10 );

  QGridLayout* layout = new QGridLayout(this, 2, 2,15,7 );
  //toplevel->addLayout( layout );
  QButtonGroup* gb = new QButtonGroup( i18n("3D Parameters"), this );
  QGridLayout *grid1 = new QGridLayout(gb,7,1,15,7);
  layout->addWidget(gb,0,0);

  QLabel *tmpLabel = new QLabel( i18n( "Angle" ), gb );
  tmpLabel->resize( tmpLabel->sizeHint() );
  tmpLabel->setAlignment(Qt::AlignCenter);
  grid1->addWidget(tmpLabel,0,0);

  angle3d=new QLineEdit(gb);
  angle3d->resize( 100,angle3d->sizeHint().height() );
  //angle3d->setMaximumWidth(100);
  grid1->addWidget(angle3d,1,0);
  KIntValidator *validate=new KIntValidator( angle3d );
  validate->setRange( 0,90 );
  angle3d->setValidator(validate);


  tmpLabel = new QLabel( i18n( "Depth" ), gb );
  tmpLabel->resize( tmpLabel->sizeHint() );
  tmpLabel->setAlignment(Qt::AlignCenter);
  grid1->addWidget(tmpLabel,2,0);

  depth=new QLineEdit(gb);
  depth->resize(100,depth->sizeHint().height());
  //depth->setMaximumWidth(100);
  grid1->addWidget(depth,3,0);
  KFloatValidator *validate2=new KFloatValidator( depth );
  validate2->setRange( 0,10 );
  //depth->setValidator( new KFloatValidator( depth ) );
  depth->setValidator(validate2);

  tmpLabel = new QLabel( i18n( "Bar width" ), gb );
  tmpLabel->resize( tmpLabel->sizeHint() );
  tmpLabel->setAlignment(Qt::AlignCenter);
  grid1->addWidget(tmpLabel,4,0);

  bar_width=new QLineEdit(gb);
  bar_width->resize( 100,bar_width->sizeHint().height() );
  //bar_width->setMaximumWidth(100);
  bar_width->setAlignment(Qt::AlignLeft);
  grid1->addWidget(bar_width,5,0);
  validate=new KIntValidator( bar_width );
  validate->setRange( 0,100 );
  //bar_width->setValidator( new KIntValidator( bar_width ) );
  bar_width->setValidator( validate);

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
QString tmp;
tmp=tmp.setNum(_params->_3d_angle);
angle3d->setText(tmp);

tmp=tmp.setNum(_params->_3d_depth);
depth->setText(tmp);

if(_params->do_bar())
        {
        tmp=tmp.setNum(_params->bar_width);
        bar_width->setText(tmp);
        }
}

void KChartParameter3dConfigPage::apply()
{
QString tmp;
tmp=angle3d->text();
_params->_3d_angle=tmp.toInt();

tmp=depth->text();
_params->_3d_depth=tmp.toDouble();

if(_params->do_bar())
        {
        tmp=bar_width->text();
        _params->bar_width=tmp.toInt();
        }
}
