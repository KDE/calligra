/*
 * Copyright 2001 by Laurent Montel, released under Artistic License.
 */

#include "kchartLegendConfigPage.h"
#include "kchartLegendConfigPage.moc"

#include <kapplication.h>
#include <klocale.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include "kdchart/KDChartParams.h"

KChartLegendConfigPage::KChartLegendConfigPage( KDChartParams* params,
                                                QWidget* parent ) :
    QWidget( parent ),_params( params )
{
  QGridLayout* layout = new QGridLayout(this, 2, 2,15,7 );

  QButtonGroup* gb = new QButtonGroup( i18n("Legend Position:"), this );
  QGridLayout *grid1 = new QGridLayout(gb,3,3,15,7);
  layout->addWidget(gb,0,0);


  noLegend=new QRadioButton(i18n("No Legend"),gb);
  grid1->addWidget(noLegend,0,0);

  lTop=new QRadioButton(i18n("Top"),gb);
  grid1->addWidget(lTop,1,0);

  lBottom=new QRadioButton(i18n("Bottom"),gb);
  grid1->addWidget(lBottom,2,0);

  lLeft=new QRadioButton(i18n("Left"),gb);
  grid1->addWidget(lLeft,0,1);

  lRight=new QRadioButton(i18n("Right"),gb);
  grid1->addWidget(lRight,1,1);

  lTopLeft=new QRadioButton(i18n("Top-Left"),gb);
  grid1->addWidget(lTopLeft,2,1);

  lTopRight=new QRadioButton(i18n("Top-Right"),gb);
  grid1->addWidget(lTopRight,0,2);

  lBottomRight=new QRadioButton(i18n("Bottom-Right"),gb);
  grid1->addWidget(lBottomRight,1,2);

  lBottomLeft=new QRadioButton(i18n("Bottom-Left"),gb);
  grid1->addWidget(lBottomLeft,2,2);

  gb->setAlignment(Qt::AlignLeft);
  //it's not good but I don't know how
  //to reduce space
  layout->addColSpacing(1,300);
}

void KChartLegendConfigPage::init()
{
    switch( _params->legendPosition())
    {
    case KDChartParams::NoLegend:
        noLegend->setChecked(true);
        break;
    case KDChartParams::LegendTop:
        lTop->setChecked(true);
        break;
    case KDChartParams::LegendBottom:
        lBottom->setChecked(true);
        break;
    case KDChartParams::LegendLeft:
        lLeft->setChecked(true);
        break;
    case KDChartParams::LegendRight:
        lRight->setChecked(true);
        break;
    case KDChartParams::LegendTopLeft:
        lTopLeft->setChecked(true);
        break;
    case KDChartParams::LegendTopRight:
        lTopRight->setChecked(true);
        break;
    case KDChartParams::LegendBottomRight:
        lBottomRight->setChecked(true);
        break;
    case  KDChartParams::LegendBottomLeft:
        lBottomLeft->setChecked(true);
        break;
    default:
        lRight->setChecked(true);
        break;
    }
}

void KChartLegendConfigPage::apply()
{
    if(noLegend->isChecked())
        _params->setLegendPosition(KDChartParams::NoLegend);
    else if(lTop->isChecked())
        _params->setLegendPosition(KDChartParams::LegendTop);
    else if(lBottom->isChecked())
        _params->setLegendPosition(KDChartParams::LegendBottom);
    else if(lLeft->isChecked())
        _params->setLegendPosition(KDChartParams::LegendLeft);
    else if(lRight->isChecked())
        _params->setLegendPosition(KDChartParams::LegendRight);
    else if(lTopLeft->isChecked())
        _params->setLegendPosition(KDChartParams::LegendTopLeft);
    else if(lTopRight->isChecked())
        _params->setLegendPosition(KDChartParams::LegendTopRight);
    else if(lBottomRight->isChecked())
        _params->setLegendPosition(KDChartParams::LegendBottomRight);
    else if(lBottomLeft->isChecked())
        _params->setLegendPosition(KDChartParams::LegendBottomLeft);
    else
        _params->setLegendPosition(KDChartParams::LegendRight);

}
