/* $Id$ */

#include "kchartWizardSelectDataPage.h"

#include <QLabel>
#include <QLineEdit>
#include <kdebug.h>

namespace KChart
{

KChartWizardSelectDataPage::KChartWizardSelectDataPage( QWidget* parent ) :
  QWidget( parent )
{
  rangeED = new QLineEdit( this, "LineEdit_1" );
  rangeED->setGeometry( 10, 90, 380, 30 );
  rangeED->setText( "" );

  QLabel* tmpQLabel;
  tmpQLabel = new QLabel( this );
  tmpQLabel->setObjectName( "Label_1" );
  tmpQLabel->setGeometry( 10, 30, 360, 20 );
  tmpQLabel->setText( "If the selected cells dont match your table," );

  tmpQLabel = new QLabel( this );
  tmpQLabel->setObjectName( "Label_2" );
  tmpQLabel->setGeometry( 10, 50, 360, 20 );
  tmpQLabel->setText( "you must select another rectangular area here." );

  kDebug(35001) << "send needNewData() signal here and update area field in KChartWizard" << endl;

  setMinimumSize( 600, 300 );
}

}  //namespace KChart
