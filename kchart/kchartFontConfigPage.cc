/*
 * Copyright 2001 by Laurent Montel, released under Artistic License.
 */

#include "kchartFontConfigPage.h"

#include "kchartFontConfigPage.moc"

#include <kapplication.h>
#include <klocale.h>
#include <kcolorbutton.h>
#include <kdebug.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpushbutton.h>

#include <kfontdialog.h>

// For IRIX
namespace std {}

using namespace std;

#include "kchart_params.h"

KChartFontConfigPage::KChartFontConfigPage( KChartParams* params,
                                            QWidget* parent, KoChart::Data *dat) :
    QWidget( parent ),_params( params ), data(dat)
{
  QGridLayout *grid = new QGridLayout(this,5,4,KDialog::marginHint(), KDialog::spacingHint());

  list = new QListBox(this);
  list->resize( list->sizeHint() );
  grid->addMultiCellWidget(list,0,4,0,0);
  fontButton = new QPushButton( this);
  fontButton->setText(i18n("Font..."));

  fontButton->resize( fontButton->sizeHint() );
  grid->addWidget( fontButton,2,1);

    connect( fontButton, SIGNAL(clicked()), this, SLOT(changeLabelFont()));
    connect( list, SIGNAL(doubleClicked ( QListBoxItem * )), this, SLOT(changeLabelFont()));

    initList();
}


void KChartFontConfigPage::initList()
{
  if( _params->chartType() != KDChartParams::Pie &&
      _params->chartType() != KDChartParams::Ring ) {
    list->insertItem(i18n("X-Title"));
    list->insertItem(i18n("Y-Title"));
    list->insertItem(i18n("X-Axis"));
    list->insertItem(i18n("Y-Axis"));
    list->insertItem(i18n("All Axes"));
  }
  list->insertItem(i18n("Label"));
  list->setCurrentItem(0);


  // PENDING(kalle) Assign legend colors
  //   for( QStringList::Iterator it = _params->legend.begin();
//        it != _params->legend.end(); ++it, num++ ) {
//     if( num<12 )
//       listColor->insertItem(*it);
//     else {
//       if( !noEnough ) {
// 	listColor->insertItem(i18n("Not enough color"));
// 	noEnough = true;
//       }
//     }
//   }

  // PENDING(kalle) Assign ExtColor
//   listColor->setCurrentItem(0);
//   for( unsigned int i = 0; i < _params->ExtColor.count(); i++ ) {
//     if( i<12 )
//       extColor.setColor(i,_params->ExtColor.color(i));
//   }
//   index = 0;
//   colorButton->setColor( extColor.color(index));
}



void KChartFontConfigPage::changeLabelFont()
{
  QFont *font = 0;
  QButton::ToggleState *state = 0;
  bool diffAxes = false;
  if(list->currentText()==i18n("X-Title")) {
    font  = &xTitle;
    state = &xTitleIsRelative;
  } else if(list->currentText()==i18n("Y-Title")) {
    font  = &yTitle;
    state = &yTitleIsRelative;
  } else if(list->currentText()==i18n("X-Axis")) {
    font  = &xAxis;
    state = &xAxisIsRelative;
  } else if(list->currentText()==i18n("Y-Axis")) {
    font  = &yAxis;
    state = &yAxisIsRelative;
  } else if(list->currentText()==i18n("All Axes")) {
    diffAxes = true;
  } else if(list->currentText()==i18n("Label")) {
    font  = &label;
    state = &labelIsRelative;
  }
  else
    kdDebug( 35001 ) << "Pb in listBox" << endl;

  if( diffAxes ) {
    QFont newFont;
    int flags = 0;
    QButton::ToggleState newState
                       = (xAxisIsRelative == yAxisIsRelative)
                       ? (xAxisIsRelative ? QButton::On : QButton::Off)
                       : QButton::NoChange;
    if (KFontDialog::getFontDiff( newFont,
                                  flags,
                                  false,
                                  this,
                                  true,
                                  &newState ) != QDialog::Rejected) {
      if( KFontChooser::FamilyList & flags ) {
        xAxis.setFamily( newFont.family() );
        yAxis.setFamily( newFont.family() );
      }
      if( KFontChooser::StyleList & flags ) {
        xAxis.setWeight( newFont.weight() );
        xAxis.setItalic( newFont.italic() );
        xAxis.setUnderline( newFont.underline() );
        xAxis.setStrikeOut( newFont.strikeOut() );
        yAxis.setWeight( newFont.weight() );
        yAxis.setItalic( newFont.italic() );
        yAxis.setUnderline( newFont.underline() );
        yAxis.setStrikeOut( newFont.strikeOut() );
      }
      if( KFontChooser::SizeList & flags ) {
        xAxis.setPointSize( newFont.pointSize() );
        yAxis.setPointSize( newFont.pointSize() );
      }
      // CharSet settings are ignored since we are not Qt 2.x compatible
      // if( KFontChooser::CharsetList & flags ) {
      // }
      if( QButton::NoChange != newState ) {
        xAxisIsRelative = newState;
        yAxisIsRelative = newState;
      }
    }
  }
  else if( font && state ) {
    QFont newFont( *font );
    QButton::ToggleState newState = *state;
    if (KFontDialog::getFont( newFont,
                              false,
                              this,
                              true,
                              &newState ) != QDialog::Rejected) {
      *font = newFont;
      if( QButton::NoChange != newState )
        *state = newState;
    }
  }
}


void KChartFontConfigPage::init()
{
    KDChartAxisParams leftparms = _params->axisParams( KDChartAxisParams::AxisPosLeft );
    KDChartAxisParams rightparms = _params->axisParams( KDChartAxisParams::AxisPosRight );
    KDChartAxisParams bottomparms = _params->axisParams( KDChartAxisParams::AxisPosBottom );
    xAxis = bottomparms.axisLabelsFont();
    xAxisIsRelative = bottomparms.axisLabelsFontUseRelSize() ? QButton::On : QButton::Off;
    if( QButton::On == xAxisIsRelative )
      xAxis.setPointSize( bottomparms.axisLabelsFontRelSize() );
    yAxis = leftparms.axisLabelsFont();
    yAxisIsRelative = leftparms.axisLabelsFontUseRelSize() ? QButton::On : QButton::Off;
    if( QButton::On == yAxisIsRelative )
      yAxis.setPointSize( leftparms.axisLabelsFontRelSize() );
    // PENDING(khz) Add support for the other 6 possible axes

    // PENDING(khz) Add support for the other 16 possible hd/ft areas


//   xTitle = _params->xTitleFont();
//   yTitle = _params->yTitleFont();
//   label = _params->labelFont();

    // PENDING(kalle) Adapt
//   for(int i=0;i<12;i++)
//     extColor.setColor(i,_params->ExtColor.color(i));
//   index = 0;
//   colorButton->setColor(extColor.color(index));
}


void KChartFontConfigPage::apply()
{
    // PENDING(kalle) Adapt
    //   _params->setLabelFont(label);

      // PENDING(kalle) Adapt
    KDChartAxisParams leftparms = _params->axisParams( KDChartAxisParams::AxisPosLeft );
    KDChartAxisParams rightparms = _params->axisParams( KDChartAxisParams::AxisPosRight );
    KDChartAxisParams bottomparms = _params->axisParams( KDChartAxisParams::AxisPosBottom );

    leftparms.setAxisLabelsFont( yAxis, QButton::Off == yAxisIsRelative );
    if( QButton::On == yAxisIsRelative )
      leftparms.setAxisLabelsFontRelSize( yAxis.pointSize() );
    // PENDING(khz) change right axis handling
    // use left axis settings for the right axis as well
    //   (this must be changed, khz 14.12.2001)
    rightparms.setAxisLabelsFont( yAxis, QButton::Off == yAxisIsRelative );
    if( QButton::On == yAxisIsRelative )
      rightparms.setAxisLabelsFontRelSize( yAxis.pointSize() );
    bottomparms.setAxisLabelsFont( xAxis, QButton::Off == xAxisIsRelative );
    if( QButton::On == xAxisIsRelative )
      bottomparms.setAxisLabelsFontRelSize( xAxis.pointSize() );
    // PENDING(khz) Add support for the other 6 possible axes

    _params->setAxisParams( KDChartAxisParams::AxisPosLeft, leftparms );
    _params->setAxisParams( KDChartAxisParams::AxisPosRight, rightparms );
    _params->setAxisParams( KDChartAxisParams::AxisPosBottom, bottomparms );
    // PENDING(khz) change hd2 and ft handling
    // use header settings for header 2 and footer as well
    //   (this must be changed, khz 14.12.2001)
    // PENDING(khz) Add support for the other 16 possible hd/ft areas

//     _params->setXTitleFont(xtitle);
//     _params->setYTitleFont(ytitle);

}
