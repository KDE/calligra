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

#include "kchartLegendConfigPage.h"
#include "kchartLegendConfigPage.moc"

#include <kapplication.h>
#include <klocale.h>
#include <kcolorbutton.h>
#include <kfontdialog.h>
#include <kiconloader.h>

#include <qlabel.h>
#include <qbuttongroup.h>
#include <qvbuttongroup.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qtooltip.h> 
#include <qwhatsthis.h>

#include "kchart_params.h"
#include "kchart_factory.h"

namespace KChart
{


KChartLegendConfigPage::KChartLegendConfigPage( KChartParams* params,
                                                QWidget* parent ) :
    QWidget( parent ),_params( params )
{
  //Layout for 4 blocks
  QGridLayout* layout = new QGridLayout( this, 3, 2, KDialog::marginHint(), KDialog::spacingHint() );

  //1. Block: General settings
  QButtonGroup* gb = new QButtonGroup( 0, Qt::Vertical, i18n("General"), this );
  gb->layout()->setSpacing(KDialog::spacingHint());
  gb->layout()->setMargin(KDialog::marginHint());
  layout->addWidget( gb, 0, 0 );

  QGridLayout *grid2 = new QGridLayout( gb->layout(), 4, 2 );

  QLabel* lab = new QLabel( i18n("Title:"), gb );
  QWhatsThis::add(lab, i18n("Write here the title of the legend, which is displayed at the top of the legend box."));
  grid2->addWidget( lab, 0, 0 );

  title = new QLineEdit( gb );
  grid2->addWidget( title, 0, 1 );

  // 2. Block: Legend position
  gb = new QButtonGroup( 0, Qt::Vertical, i18n("Legend Position"), this );
  QWhatsThis::add(gb, i18n("<qt>Choose the location of the legend on the chart by "
      "clicking a location button.\nUse the central button to <b>hide</b> the legend.</qt>"));
  gb->layout()->setSpacing(KDialog::spacingHint());
  gb->layout()->setMargin(KDialog::marginHint());
  gb->setExclusive( true );

  QGridLayout *grid1 = new QGridLayout( gb->layout(), 5, 5 );

  lTopLeft          = addButton( grid1, gb, i18n("Top-Left"),      "chart_legend_topleft",     0, 0 );
  lTopLeftTop       = addButton( grid1, gb, i18n("Top-Left-Top"),  "chart_legend_toplefttop",  0, 1 );
  lTop              = addButton( grid1, gb, i18n("Top"),           "chart_legend_top",         0, 2 );
  lTopRightTop      = addButton( grid1, gb, i18n("Top-Right-Top"), "chart_legend_toprighttop", 0, 3 );
  lTopRight         = addButton( grid1, gb, i18n("Top-Right"),     "chart_legend_topright",    0, 4 );

  lTopLeftLeft      = addButton( grid1, gb, i18n("Top-Left-Left"),   "chart_legend_topleftleft",   1, 0 );
  lTopRightRight    = addButton( grid1, gb, i18n("Top-Right-Right"), "chart_legend_toprightright", 1, 4 );

  lLeft             = addButton( grid1, gb, i18n("Left"),      "chart_legend_left",     2, 0 );
  noLegend          = addButton( grid1, gb, i18n("No Legend"), "chart_legend_nolegend", 2, 2 );
  lRight            = addButton( grid1, gb, i18n("Right"),     "chart_legend_right",    2, 4 );

  lBottomLeftLeft   = addButton( grid1, gb, i18n("Bottom-Left-Left"),    "chart_legend_bottomleftleft",   3, 0 );
  lBottomRightRight = addButton( grid1, gb, i18n("Bottom-Right-Right"),  "chart_legend_bottomrightright", 3, 4 );

  lBottomLeft       = addButton( grid1, gb, i18n("Bottom-Left"),         "chart_legend_bottomleft",       4, 0 );
  lBottomLeftBottom = addButton( grid1, gb, i18n("Bottom-Left-Bottom"),  "chart_legend_bottomleftbottom", 4, 1 );
  lBottom           = addButton( grid1, gb, i18n("Bottom"),              "chart_legend_bottom",           4, 2 );
  lBottomRightBottom= addButton( grid1, gb, i18n("Bottom-Right-Bottom"), "chart_legend_bottomrightbottom",4, 3 );
  lBottomRight      = addButton( grid1, gb, i18n("Bottom-Right"),        "chart_legend_bottomright",      4, 4 );

  gb->setAlignment( Qt::AlignLeft );
  layout->addMultiCellWidget( gb, 1,2, 0,0 );

  // 3. Block: Font
  gb = new QButtonGroup( 0, Qt::Vertical, i18n("Font"), this );
  QWhatsThis::add(gb, i18n("This Font box can be used to set different fonts for the legend title and the individual entries."));
  gb->layout()->setSpacing(KDialog::spacingHint());
  gb->layout()->setMargin(KDialog::marginHint());
  layout->addWidget( gb, 0, 1 );

  QGridLayout *grid4 = new QGridLayout( gb->layout(), 4, 2 );
  titleLegendFontButton = new QPushButton( gb );

  lab = new QLabel( i18n("Legend title font:"), gb );
  grid4->addWidget( lab, 0 ,0 );

  titleLegendFontButton->setText( i18n("Select Font...") );
  QWhatsThis::add(titleLegendFontButton, i18n("Click here to display the KDE font chooser dialog. You will be able to change the font family, style and size for the legend title."));
  grid4->addWidget( titleLegendFontButton, 1, 0 );

  lab = new QLabel( i18n("Legend text font:"), gb );
  grid4->addWidget( lab, 2, 0 );
  textLegendFontButton = new QPushButton( gb );
  textLegendFontButton->setText( i18n("Select Font...") );
  QWhatsThis::add(textLegendFontButton, i18n("Click here to display the KDE font chooser dialog. You will be able to change the font family, style and size for the legend text."));
  grid4->addWidget( textLegendFontButton, 3, 0 );

  connect( titleLegendFontButton, SIGNAL(clicked()), 
	   this, SLOT(changeTitleLegendFont()));
  connect( textLegendFontButton, SIGNAL(clicked()),
	   this, SLOT(changeTextLegendFont()));

  // 4. Block: Orientation
  orientationGroup = new QVButtonGroup( i18n("Orientation"), this );
  QWhatsThis::add(orientationGroup, i18n("Select, if the legend's items should be dxrawn next to each other, or below each other."));
  orientationGroup->setRadioButtonExclusive(true);
  orientationGroup->layout()->setSpacing(KDialog::spacingHint());
  orientationGroup->layout()->setMargin(KDialog::marginHint());
  layout->addWidget( orientationGroup, 1, 1 );

  QRadioButton* orientationVertical = new QRadioButton( i18n("Vertically"), orientationGroup );
  QRadioButton* orientationHorizontal = new QRadioButton( i18n("Horizontally"), orientationGroup );

  // 5. Block: Text Colors
  gb = new QButtonGroup( 0, Qt::Vertical, i18n("Color"), this );
  QWhatsThis::add(gb, i18n("This Color box can be used to set different colors for the legend title and text."));
  gb->layout()->setSpacing(KDialog::spacingHint());
  gb->layout()->setMargin(KDialog::marginHint());
  layout->addWidget( gb, 2, 1 );

  QGridLayout *grid3 = new QGridLayout( gb->layout(), 4, 2 );

  lab = new QLabel( i18n("Legend title color:"), gb );
  grid3->addWidget( lab, 0, 0 );

  legendTitleColor = new KColorButton( gb );
  QWhatsThis::add(legendTitleColor, i18n("Click here to display the KDE Select Color dialog. You will be able to change the color for the legend title."));
  grid3->addWidget( legendTitleColor, 1, 0 );


  lab = new QLabel( i18n("Legend text color:"), gb );
  grid3->addWidget( lab, 2, 0 );

  legendTextColor = new KColorButton( gb );
  QWhatsThis::add(legendTextColor, i18n("Click here to display the KDE Select Color dialog. You will be able to change the color for the legend text."));

  grid3->addWidget( legendTextColor, 3, 0 );

  //it's not good but I don't know how
  //to reduce space
  layout->addColSpacing( 1, 300 );
}

void KChartLegendConfigPage::init()
{
    switch( _params->legendPosition())
    {
    case KDChartParams::NoLegend:
        noLegend->setOn( true );
        break;
    case KDChartParams::LegendTop:
        lTop->setOn( true );
        break;
    case KDChartParams::LegendBottom:
        lBottom->setOn( true );
        break;
    case KDChartParams::LegendLeft:
        lLeft->setOn( true );
        break;
    case KDChartParams::LegendRight:
        lRight->setOn( true );
        break;
    case KDChartParams::LegendTopLeft:
        lTopLeft->setOn( true );
        break;
    case KDChartParams::LegendTopLeftTop:
        lTopLeftTop->setOn( true );
        break;
    case KDChartParams::LegendTopLeftLeft:
        lTopLeftLeft->setOn( true );
        break;
    case KDChartParams::LegendTopRight:
        lTopRight->setOn( true );
        break;
    case KDChartParams::LegendTopRightTop:
        lTopRightTop->setOn( true );
        break;
    case KDChartParams::LegendTopRightRight:
        lTopRightRight->setOn( true );
        break;
    case KDChartParams::LegendBottomRight:
        lBottomRight->setOn( true );
        break;
    case KDChartParams::LegendBottomRightBottom:
        lBottomRightBottom->setOn( true );
        break;
    case KDChartParams::LegendBottomRightRight:
        lBottomRightRight->setOn( true );
        break;
    case  KDChartParams::LegendBottomLeft:
        lBottomLeft->setOn( true );
        break;
    case  KDChartParams::LegendBottomLeftBottom:
        lBottomLeftBottom->setOn( true );
        break;
    case  KDChartParams::LegendBottomLeftLeft:
        lBottomLeftLeft->setOn( true );
        break;
    default:
        lRight->setOn( true );
        break;
    }
    title->setText(_params->legendTitleText());
    legendTitleColor->setColor(_params->legendTitleTextColor());
    legendTextColor->setColor(_params->legendTextColor());

    if( _params->legendOrientation() == Qt::Vertical )
        orientationGroup->setButton(0);
    else
        orientationGroup->setButton(1);

    titleLegend = _params->legendTitleFont();
    titleLegendIsRelative = _params->legendTitleFontUseRelSize()
                          ? QButton::On
                          : QButton::Off;
    if( QButton::On == titleLegendIsRelative )
        titleLegend.setPointSize( _params->legendTitleFontRelSize() );

    textLegend=_params->legendFont();
    textLegendIsRelative = _params->legendFontUseRelSize()
                         ? QButton::On
                         : QButton::Off;
    if( QButton::On == textLegendIsRelative )
        textLegend.setPointSize(_params->legendFontRelSize());
}

void KChartLegendConfigPage::changeTitleLegendFont()
{
    QButton::ToggleState state = titleLegendIsRelative;
    if (    KFontDialog::getFont( titleLegend,false,this, true,&state ) != QDialog::Rejected
         && QButton::NoChange != state )
        titleLegendIsRelative = state;
}

void KChartLegendConfigPage::changeTextLegendFont()
{
    QButton::ToggleState state = textLegendIsRelative;
    if (    KFontDialog::getFont( textLegend,false,this, true,&state ) != QDialog::Rejected
         && QButton::NoChange != state )
        textLegendIsRelative = state;
}

void KChartLegendConfigPage::apply()
{
    if( noLegend->isOn() )
        _params->setLegendPosition( KDChartParams::NoLegend );
    else if( lTop->isOn() )
        _params->setLegendPosition( KDChartParams::LegendTop );
    else if( lBottom->isOn() )
        _params->setLegendPosition( KDChartParams::LegendBottom );
    else if( lLeft->isOn() )
        _params->setLegendPosition( KDChartParams::LegendLeft );
    else if( lRight->isOn() )
        _params->setLegendPosition( KDChartParams::LegendRight );
    else if( lTopLeft->isOn() )
        _params->setLegendPosition( KDChartParams::LegendTopLeft );
    else if( lTopLeftTop->isOn() )
        _params->setLegendPosition( KDChartParams::LegendTopLeftTop );
    else if( lTopLeftLeft->isOn() )
        _params->setLegendPosition( KDChartParams::LegendTopLeftLeft );
    else if( lTopRight->isOn() )
        _params->setLegendPosition( KDChartParams::LegendTopRight );
    else if( lTopRightTop->isOn() )
        _params->setLegendPosition( KDChartParams::LegendTopRightTop );
    else if( lTopRightRight->isOn() )
        _params->setLegendPosition( KDChartParams::LegendTopRightRight );
    else if( lBottomRight->isOn() )
        _params->setLegendPosition( KDChartParams::LegendBottomRight );
    else if( lBottomRightBottom->isOn() )
        _params->setLegendPosition( KDChartParams::LegendBottomRightBottom );
    else if( lBottomRightRight->isOn() )
        _params->setLegendPosition( KDChartParams::LegendBottomRightRight );
    else if( lBottomLeft->isOn() )
        _params->setLegendPosition( KDChartParams::LegendBottomLeft );
    else if( lBottomLeftBottom->isOn() )
        _params->setLegendPosition( KDChartParams::LegendBottomLeftBottom );
    else if( lBottomLeftLeft->isOn() )
        _params->setLegendPosition( KDChartParams::LegendBottomLeftLeft );
    else
        _params->setLegendPosition( KDChartParams::LegendRight );

    if( orientationGroup->selectedId() == 0 )
        _params->setLegendOrientation( Qt::Vertical );
    else
        _params->setLegendOrientation( Qt::Horizontal );

    _params->setLegendTitleText(title->text());
    _params->setLegendTitleTextColor(legendTitleColor->color());
    _params->setLegendTextColor(legendTextColor->color());

    _params->setLegendTitleFont(titleLegend, QButton::Off == titleLegendIsRelative);
    if( QButton::On == titleLegendIsRelative )
        _params->setLegendTitleFontRelSize(titleLegend.pointSize());
    _params->setLegendFont(textLegend, QButton::Off == textLegendIsRelative);
    if( QButton::On == textLegendIsRelative )
        _params->setLegendFontRelSize(textLegend.pointSize());
}

QPushButton* KChartLegendConfigPage::addButton( QGridLayout* layout,
                                                QButtonGroup* gb,
                                                const QString &toolTipText,
                                                const QString &icon,
                                                int posY,
                                                int posX )
{
  QPushButton* button = new QPushButton( gb );
  button->setToggleButton( true );
  button->setPixmap( BarIcon( icon,
                              KIcon::SizeMedium,
                              KIcon::DefaultState,
                              KChartFactory::global() ) );
  QToolTip::add( button, toolTipText );
  layout->addWidget( button, posY, posX );
  return button;
}

}  //KChart namespace
