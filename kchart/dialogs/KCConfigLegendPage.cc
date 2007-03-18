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
#include <QButtonGroup> 
#include <QLabel>
#include <qradiobutton.h>
#include <QLineEdit>
#include <QToolTip> 

#include <klocale.h>
#include <kcolorbutton.h>
#include <kfontdialog.h>
#include <kiconloader.h>

#include "kchart_params.h"
#include "kchart_factory.h"
#include "KCConfigLegendPage.h"


namespace KChart
{


KCConfigLegendPage::KCConfigLegendPage( KChartParams* params,
					QWidget* parent ) :
    QWidget( parent ),_params( params )
{
  //Layout for 4 blocks
  QGridLayout* layout = new QGridLayout( this );
  layout->setMargin( KDialog::marginHint() );
  layout->setSpacing( KDialog::spacingHint() );

  // 1. Block: General settings
  QGroupBox* gb = new QGroupBox( i18n("General") );
  layout->addWidget( gb, 0, 0 );

  QVBoxLayout *vbox = new QVBoxLayout( );
  vbox->setSpacing(KDialog::spacingHint());
  vbox->setMargin(KDialog::marginHint());
  gb->setLayout( vbox );

  QLabel* lab = new QLabel( i18n("Title:") );
  lab->setWhatsThis( i18n("Write here the title of the legend, which is displayed at the top of the legend box."));
  vbox->addWidget( lab );

  title = new QLineEdit( );
  vbox->addWidget( title );
  vbox->addStretch( 1 );

  // 2. Block: Legend position
  gb = new QGroupBox( i18n("Legend Position") );
  gb->setWhatsThis( i18n("<qt>Choose the location of the legend on the chart by clicking a location button.\nUse the central button to <b>hide</b> the legend.</qt>"));
  layout->addWidget( gb, 1, 0 );

  QGridLayout *grid1 = new QGridLayout( );
  grid1->setSpacing( 3 );
  grid1->setMargin( KDialog::marginHint() );
  gb->setLayout( grid1 );

  QButtonGroup *bg = new QButtonGroup( );
  bg->setExclusive( true );

  lTopLeft  = addButton( grid1, bg, i18n("Top-Left"),  "chart_legend_topleft",  0, 0 );
  lTop      = addButton( grid1, bg, i18n("Top"),       "chart_legend_top",      0, 1 );
  lTopRight = addButton( grid1, bg, i18n("Top-Right"), "chart_legend_topright", 0, 2 );

  lLeft     = addButton( grid1, bg, i18n("Left"),      "chart_legend_left",     1, 0 );
  noLegend  = addButton( grid1, bg, i18n("No Legend"), "chart_legend_nolegend", 1, 1 );
  lRight    = addButton( grid1, bg, i18n("Right"),     "chart_legend_right",    1, 2 );

  lBottomLeft  = addButton( grid1, bg, i18n("Bottom-Left"),  "chart_legend_bottomleft",  2, 0 );
  lBottom      = addButton( grid1, bg, i18n("Bottom"),       "chart_legend_bottom",      2, 1 );
  lBottomRight = addButton( grid1, bg, i18n("Bottom-Right"), "chart_legend_bottomright", 2, 2 );

  gb->setAlignment( Qt::AlignLeft );

  // 3. Block: Font
  gb = new QGroupBox( i18n("Font") );
  gb->setWhatsThis( i18n("This Font box can be used to set different fonts for the legend title and the individual entries."));
  layout->addWidget( gb, 0, 1 );

  vbox = new QVBoxLayout( );
  vbox->setSpacing( KDialog::spacingHint() );
  vbox->setMargin( KDialog::marginHint() );
  gb->setLayout( vbox );

  lab = new QLabel( i18n("Legend title font:") );
  vbox->addWidget( lab );

  titleLegendFontButton = new QPushButton( );
  titleLegendFontButton->setText( i18n("Select Font...") );
  titleLegendFontButton->setWhatsThis( i18n("Click here to display the KDE font chooser dialog. You will be able to change the font family, style and size for the legend title."));
  vbox->addWidget( titleLegendFontButton );

  lab = new QLabel( i18n("Legend text font:") );
  vbox->addWidget( lab );

  textLegendFontButton = new QPushButton( );
  textLegendFontButton->setText( i18n("Select Font...") );
  textLegendFontButton->setWhatsThis( i18n("Click here to display the KDE font chooser dialog. You will be able to change the font family, style and size for the legend text."));
  vbox->addWidget( textLegendFontButton );

  vbox->addStretch( 1 );

  connect( titleLegendFontButton, SIGNAL(clicked()), 
	   this, SLOT(changeTitleLegendFont()));
  connect( textLegendFontButton, SIGNAL(clicked()),
	   this, SLOT(changeTextLegendFont()));

  // 4. Block: Text Colors
  gb = new QGroupBox( i18n("Color") );
  gb->setWhatsThis( i18n("This Color box can be used to set different colors for the legend title and text."));
  layout->addWidget( gb, 1, 1 );

  vbox = new QVBoxLayout( );
  vbox->setSpacing( KDialog::spacingHint() );
  vbox->setMargin( KDialog::marginHint() );
  gb->setLayout( vbox );

  lab = new QLabel( i18n("Legend title color:") );
  vbox->addWidget( lab );

  legendTitleColor = new KColorButton( );
  legendTitleColor->setWhatsThis( i18n("Click here to display the KDE Select Color dialog. You will be able to change the color for the legend title."));
  vbox->addWidget( legendTitleColor );

  lab = new QLabel( i18n("Legend text color:") );
  vbox->addWidget( lab );

  legendTextColor = new KColorButton( );
  legendTextColor->setWhatsThis( i18n("Click here to display the KDE Select Color dialog. You will be able to change the color for the legend text."));
  vbox->addWidget( legendTextColor );

  vbox->addStretch( 1 );

  //it's not good but I don't know how
  //to reduce space
  layout->addItem( new QSpacerItem( 300, 0 ), 0, 1 );
}

void KCConfigLegendPage::init()
{
    switch( _params->legendPosition())
    {
    case KDChartParams::NoLegend:
        noLegend->setChecked( true );
        break;
    case KDChartParams::LegendTop:
        lTop->setChecked( true );
        break;
    case KDChartParams::LegendBottom:
        lBottom->setChecked( true );
        break;
    case KDChartParams::LegendLeft:
        lLeft->setChecked( true );
        break;
    case KDChartParams::LegendRight:
        lRight->setChecked( true );
        break;
    case KDChartParams::LegendTopLeft:
        lTopLeft->setChecked( true );
        break;
    case KDChartParams::LegendTopRight:
        lTopRight->setChecked( true );
        break;
    case KDChartParams::LegendBottomRight:
        lBottomRight->setChecked( true );
        break;
    case  KDChartParams::LegendBottomLeft:
        lBottomLeft->setChecked( true );
        break;
    default:
        lRight->setChecked( true );
        break;
    }
    title->setText(_params->legendTitleText());
    legendTitleColor->setColor(_params->legendTitleTextColor());
    legendTextColor->setColor(_params->legendTextColor());

    titleLegend = _params->legendTitleFont();
    titleLegendIsRelative = _params->legendTitleFontUseRelSize()
                          ? Qt::Checked
                          : Qt::Unchecked;
    if( Qt::Checked == titleLegendIsRelative )
        titleLegend.setPointSize( _params->legendTitleFontRelSize() );

    textLegend=_params->legendFont();
    textLegendIsRelative = _params->legendFontUseRelSize()
                         ? Qt::Checked
                         : Qt::Unchecked;
    if( Qt::Checked == textLegendIsRelative )
        textLegend.setPointSize(_params->legendFontRelSize());
}

void KCConfigLegendPage::changeTitleLegendFont()
{
		Qt::CheckState state = titleLegendIsRelative;
    if (    KFontDialog::getFont( titleLegend,false,this, true,&state ) != QDialog::Rejected
         && Qt::PartiallyChecked != state )
        titleLegendIsRelative = state;
}

void KCConfigLegendPage::changeTextLegendFont()
{
   Qt::CheckState state = textLegendIsRelative;
    if (    KFontDialog::getFont( textLegend,false,this, true,&state ) != QDialog::Rejected
         && Qt::PartiallyChecked != state )
        textLegendIsRelative = state;
}

void KCConfigLegendPage::apply()
{
    if( noLegend->isChecked() )
        _params->setLegendPosition( KDChartParams::NoLegend );
    else if( lTop->isChecked() )
        _params->setLegendPosition( KDChartParams::LegendTop );
    else if( lBottom->isChecked() )
        _params->setLegendPosition( KDChartParams::LegendBottom );
    else if( lLeft->isChecked() )
        _params->setLegendPosition( KDChartParams::LegendLeft );
    else if( lRight->isChecked() )
        _params->setLegendPosition( KDChartParams::LegendRight );
    else if( lTopLeft->isChecked() )
        _params->setLegendPosition( KDChartParams::LegendTopLeft );
    else if( lTopRight->isChecked() )
        _params->setLegendPosition( KDChartParams::LegendTopRight );
    else if( lBottomRight->isChecked() )
        _params->setLegendPosition( KDChartParams::LegendBottomRight );
    else if( lBottomLeft->isChecked() )
        _params->setLegendPosition( KDChartParams::LegendBottomLeft );
    else
        _params->setLegendPosition( KDChartParams::LegendRight );

    _params->setLegendTitleText(title->text());
    _params->setLegendTitleTextColor(legendTitleColor->color());
    _params->setLegendTextColor(legendTextColor->color());

    _params->setLegendTitleFont(titleLegend, Qt::Unchecked == titleLegendIsRelative);
    if( Qt::Checked == titleLegendIsRelative )
        _params->setLegendTitleFontRelSize(titleLegend.pointSize());
    _params->setLegendFont(textLegend, Qt::Unchecked == textLegendIsRelative);
    if( Qt::Checked == textLegendIsRelative )
        _params->setLegendFontRelSize(textLegend.pointSize());
}

QPushButton* KCConfigLegendPage::addButton( QGridLayout* layout,
					    QButtonGroup *bg,
					    const QString &toolTipText,
					    const QString &icon,
					    int posY,
					    int posX )
{
  QPushButton* button = new QPushButton( );
  button->setCheckable( true );
  bg->addButton( button );
  button->setIcon( BarIcon( icon,
			    K3Icon::SizeMedium,
			    K3Icon::DefaultState ) );
  button->setToolTip( toolTipText );
  layout->addWidget( button, posY, posX );

  return button;
}

}  //KChart namespace

#include "KCConfigLegendPage.moc"
