/*
 * Copyright 2001 by Laurent Montel, released under Artistic License.
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
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qtooltip.h> 

#include "kchart_params.h"
#include "kchart_factory.h"


KChartLegendConfigPage::KChartLegendConfigPage( KChartParams* params,
                                                QWidget* parent ) :
    QWidget( parent ),_params( params )
{
  //Layout for 4 blocks
  QGridLayout* layout = new QGridLayout( this, 2, 2, 15, 7 );

  //1. Block: Legend position
  QButtonGroup* gb = new QButtonGroup( 0, Qt::Vertical, i18n("Legend Position"), this );
  gb->layout()->setSpacing(KDialog::spacingHint());
  gb->layout()->setMargin(KDialog::marginHint());
  gb->setExclusive( true );

  QGridLayout *grid1 = new QGridLayout( gb->layout(), 3, 3 );

  lTopLeft  = addButton( grid1, gb, i18n("Top-left"),  "chart_legend_topleft",  0, 0 );
  lTop      = addButton( grid1, gb, i18n("Top"),       "chart_legend_top",      0, 1 );
  lTopRight = addButton( grid1, gb, i18n("Top-right"), "chart_legend_topright", 0, 2 );

  lLeft    = addButton( grid1, gb, i18n("Left"),      "chart_legend_left",     1, 0 );
  noLegend = addButton( grid1, gb, i18n("No legend"), "chart_legend_nolegend", 1, 1 );
  lRight   = addButton( grid1, gb, i18n("Right"),     "chart_legend_right",    1, 2 );

  lBottomLeft  = addButton( grid1, gb, i18n("Bottom-left"),  "chart_legend_bottomleft",  2, 0 );
  lBottom      = addButton( grid1, gb, i18n("Bottom"),       "chart_legend_bottom",      2, 1 );
  lBottomRight = addButton( grid1, gb, i18n("Bottom-right"), "chart_legend_bottomright", 2, 2 );

  gb->setAlignment( Qt::AlignLeft );
  layout->addWidget( gb, 0, 0 );


  //2. Block: Title text
  gb = new QButtonGroup( 0, Qt::Vertical, i18n("Title"), this );
  gb->layout()->setSpacing(KDialog::spacingHint());
  gb->layout()->setMargin(KDialog::marginHint());
  layout->addWidget( gb, 1, 0 );

  QGridLayout *grid2 = new QGridLayout( gb->layout(), 4, 2 );

  QLabel* lab = new QLabel( i18n("Legend title:"), gb );
  grid2->addWidget( lab, 0, 0 );

  title = new QLineEdit( gb );
  grid2->addWidget( title, 1, 0 );

  //3. Block: Text Colors
  gb = new QButtonGroup( 0, Qt::Vertical, i18n("Color"), this );
  gb->layout()->setSpacing(KDialog::spacingHint());
  gb->layout()->setMargin(KDialog::marginHint());
  layout->addWidget( gb, 0, 1 );

  QGridLayout *grid3 = new QGridLayout( gb->layout(), 4, 2 );

  lab = new QLabel( i18n("Legend title color:"), gb );
  grid3->addWidget( lab, 0, 0 );

  legendTitleColor = new KColorButton( gb );
  grid3->addWidget( legendTitleColor, 1, 0 );


  lab = new QLabel( i18n("Legend text color:"), gb );
  grid3->addWidget( lab, 2, 0 );

  legendTextColor = new KColorButton( gb );

  grid3->addWidget( legendTextColor, 3, 0 );

  //4. Block: Font
  gb = new QButtonGroup( 0, Qt::Vertical, i18n("Font"), this );
  gb->layout()->setSpacing(KDialog::spacingHint());
  gb->layout()->setMargin(KDialog::marginHint());
  layout->addWidget( gb, 1, 1 );

  QGridLayout *grid4 = new QGridLayout( gb->layout(), 4, 2 );
  titleLegendFontButton = new QPushButton( gb );

  lab = new QLabel( i18n("Legend title font:"), gb );
  grid4->addWidget( lab, 0 ,0 );

  titleLegendFontButton->setText( i18n("Legend...") );
  grid4->addWidget( titleLegendFontButton, 1, 0 );

  lab = new QLabel( i18n("Legend text font:"), gb );
  grid4->addWidget( lab, 2, 0 );
  textLegendFontButton = new QPushButton( gb );
  textLegendFontButton->setText( i18n("Text Legend...") );
  grid4->addWidget( textLegendFontButton, 3, 0 );

  connect( titleLegendFontButton, SIGNAL(clicked()), this, SLOT(changeTitleLegendFont()));
  connect( textLegendFontButton, SIGNAL(clicked()), this, SLOT(changeTextLegendFont()));

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
    case KDChartParams::LegendTopRight:
        lTopRight->setOn( true );
        break;
    case KDChartParams::LegendBottomRight:
        lBottomRight->setOn( true );
        break;
    case  KDChartParams::LegendBottomLeft:
        lBottomLeft->setOn( true );
        break;
    default:
        lRight->setOn( true );
        break;
    }
    title->setText(_params->legendTitleText());
    legendTitleColor->setColor(_params->legendTitleTextColor());
    legendTextColor->setColor(_params->legendTextColor());

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
    else if( lTopRight->isOn() )
        _params->setLegendPosition( KDChartParams::LegendTopRight );
    else if( lBottomRight->isOn() )
        _params->setLegendPosition( KDChartParams::LegendBottomRight );
    else if( lBottomLeft->isOn() )
        _params->setLegendPosition( KDChartParams::LegendBottomLeft );
    else
        _params->setLegendPosition( KDChartParams::LegendRight );

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
