/*
 * $Id$
 *
 * Copyright 2000 by Laurent Montel, released under Artistic License.
 */

#include "kchartBarSubTypeChartPage.h"
#include "kchartBarSubTypeChartPage.moc"

#include <kapp.h>
#include <klocale.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qbuttongroup.h>


KChartSubTypeChartPage::KChartSubTypeChartPage(KChartParameters* params,QWidget* parent ) :
    QWidget( parent ),_params( params )
{
  //QVBoxLayout* toplevel = new QVBoxLayout( this, 10 );

  QGridLayout* layout = new QGridLayout(this, 2, 2,15,7 );
  //toplevel->addLayout( layout );
  QButtonGroup* gb = new QButtonGroup( i18n("Sub Type Chart"), this );
  QGridLayout *grid1 = new QGridLayout(gb,7,1,15,7);
  layout->addWidget(gb,0,0);

  depth=new QRadioButton( i18n("Depth"), gb ); ;
  grid1->addWidget(depth,0,0);
  sum=new QRadioButton( i18n("Sum"), gb );
  grid1->addWidget(sum,1,0);
  beside=new QRadioButton( i18n("Beside"), gb );
  grid1->addWidget(beside,2,0);
  layer=new QRadioButton( i18n("Layer"), gb );
  grid1->addWidget(layer,3,0);
  percent=new QRadioButton( i18n("Percent (only bar2D and bar3D)"), gb );
  grid1->addWidget(percent,4,0);
  if(!_params->do_bar())
        {
        percent->setEnabled(false);
        }
  gb->setAlignment(Qt::AlignLeft);
  grid1->addColSpacing(0,depth->width());
  grid1->addColSpacing(0,sum->width());
  grid1->addColSpacing(0,beside->width());
  grid1->addColSpacing(0,percent->width());
  grid1->setColStretch(0,1);
  grid1->activate();
  //it's not good but I don't know how
  //to reduce space
  layout->addColSpacing(1,300);
}

void KChartSubTypeChartPage::init()
{
switch((int)_params->stack_type)
        {
        case (int)KCHARTSTACKTYPE_DEPTH:
                {
                 depth->setChecked(true);
                 break;
                }
         case (int)KCHARTSTACKTYPE_SUM:
                {
                 sum->setChecked(true);
                 break;
                }
         case (int)KCHARTSTACKTYPE_BESIDE:
                {
                 beside->setChecked(true);
                 break;
                }
          case (int)KCHARTSTACKTYPE_LAYER:
                {
                 layer->setChecked(true);
                 break;
                 }
          case (int)KCHARTSTACKTYPE_PERCENT:
                {
                 percent->setChecked(true);
                 break;
                }
          default:
                {
                 cout <<"Error in stack_type\n";
                 break;
                }
        }
}

void KChartSubTypeChartPage::apply()
{
if(depth->isChecked())
        {
        _params->stack_type = KCHARTSTACKTYPE_DEPTH;
        }
else if(sum->isChecked())
        {
        _params->stack_type = KCHARTSTACKTYPE_SUM;
        }
else if(beside->isChecked())
        {
        _params->stack_type = KCHARTSTACKTYPE_BESIDE;
        }
else if(layer->isChecked())
        {
        _params->stack_type = KCHARTSTACKTYPE_LAYER;
        }
else if(percent->isChecked())
        {
        _params->stack_type = KCHARTSTACKTYPE_PERCENT;
        }

else
        {
        cout <<"Error in groupbutton\n";
        }
}

