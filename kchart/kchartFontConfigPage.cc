/*
 * $Id$
 *
 * Copyright 2000 by Laurent Montel, released under Artistic License.
 */

#include "kchartFontConfigPage.h"

#include "kchartFontConfigPage.moc"

#include <kapp.h>
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

#include "kdchart/KDChartParams.h"

KChartFontConfigPage::KChartFontConfigPage( KDChartParams* params,
                                            QWidget* parent ) :
  QWidget( parent ),_params( params )
{
  QGridLayout *grid = new QGridLayout(this,5,4,15,7);

  list = new QListBox(this);
  list->resize( list->sizeHint() );
  grid->addMultiCellWidget(list,0,4,0,0);
  fontButton = new QPushButton( this);
  fontButton->setText(i18n("Font"));

  fontButton->resize( fontButton->sizeHint() );
  grid->addWidget( fontButton,2,1);

  listColor = new QListBox(this);
  listColor->resize( listColor->sizeHint() );
  grid->addMultiCellWidget(listColor,0,4,2,2);
  colorButton = new KColorButton( this);

  colorButton->resize( colorButton->sizeHint() );
  grid->addWidget( colorButton,2,3);
  grid->addColSpacing(0,list->width());
  grid->addColSpacing(2,listColor->width());
  //grid->addColSpacing(3,list->width());

  initList();
  connect( fontButton, SIGNAL(clicked()), this, SLOT(changeLabelFont()));
  connect( listColor, SIGNAL(highlighted(int )), this, SLOT(changeIndex(int)));
}

void KChartFontConfigPage::initList()
{
  list->insertItem(i18n("Title"));
  if( _params->chartType() != KDChartParams::Pie &&
      _params->chartType() != KDChartParams::Ring ) {
    list->insertItem(i18n("X-Title"));
    list->insertItem(i18n("Y-Title"));
    list->insertItem(i18n("X-Axis"));
    list->insertItem(i18n("Y-Axis"));
  }
  list->insertItem(i18n("Label"));
  list->setCurrentItem(0);
  int num=0;
  bool noEnough=false;
  //num <12 because there are 12 colors

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


void KChartFontConfigPage::changeIndex(int newindex)
{
  if(index>11)
    colorButton->setEnabled(false);
  else {
    if(!colorButton->isEnabled())
      colorButton->setEnabled(true);
    extColor.setColor(index,colorButton->color());
    colorButton->setColor(extColor.color(newindex));
    index=newindex;
  }
}

void KChartFontConfigPage::changeLabelFont()
{
  if(list->currentText()==i18n("Title")) {
	if (KFontDialog::getFont( title,false,this ) == QDialog::Rejected )
	  return;
  } else if(list->currentText()==i18n("X-Title")) {
	if (KFontDialog::getFont( xtitle,false,this ) == QDialog::Rejected )
	  return;
  } else if(list->currentText()==i18n("Y-Title")) {
	if (KFontDialog::getFont( ytitle,false,this ) == QDialog::Rejected )
	  return;
  } else if(list->currentText()==i18n("X-Axis")) {
	if (KFontDialog::getFont( xaxis,false,this ) == QDialog::Rejected )
	  return;
  } else if(list->currentText()==i18n("Y-Axis")) {
	if (KFontDialog::getFont( yaxis,false,this ) == QDialog::Rejected )
	  return;
  } else if(list->currentText()==i18n("Label")) {
	if (KFontDialog::getFont( label,false,this ) == QDialog::Rejected )
	  return;
  } else {
	kdDebug( 35001 ) << "Pb in listBox" << endl;
  }
}


void KChartFontConfigPage::init()
{
    KDChartAxisParams leftparms = _params->axisParams( KDChartAxisParams::AxisPosLeft );
    KDChartAxisParams rightparms = _params->axisParams( KDChartAxisParams::AxisPosRight );
    KDChartAxisParams bottomparms = _params->axisParams( KDChartAxisParams::AxisPosBottom );
    xaxis = bottomparms.axisLabelsFont();
    yaxis = leftparms.axisLabelsFont();
    
    
    // PENDING(kalle) Adapt
    //   title = _params->titleFont();
//   xtitle = _params->xTitleFont();
//   ytitle = _params->yTitleFont();
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
    bottomparms.setAxisLabelsFont( xaxis, true );
    leftparms.setAxisLabelsFont( yaxis, true );
    _params->setAxisParams( KDChartAxisParams::AxisPosLeft, leftparms );
    _params->setAxisParams( KDChartAxisParams::AxisPosRight, rightparms );
    _params->setAxisParams( KDChartAxisParams::AxisPosBottom, bottomparms );
    
//     _params->setXTitleFont(xtitle);
//     _params->setYTitleFont(ytitle);

  // PENDING(kalle) Adapt
  //   _params->setTitleFont(title);

  extColor.setColor(index,colorButton->color());
  // PENDING(kalle) Adapt
  //   for(unsigned int i=0;i<extColor.count();i++)
//     _params->ExtColor.setColor(i,extColor.color(i));
}
