/* $Id$ */

#include "kchartWizardSetupDataPage.h"
#include "kchart_view.h"
#include "kchartWizard.h"

#include <qradiobutton.h>
#include <QCheckBox>
#include <q3buttongroup.h>
#include <QLabel>
//Added by qt3to4:
#include <Q3Frame>

namespace KChart
{

KChartWizardSetupDataPage::KChartWizardSetupDataPage( QWidget* parent,
                                                                                                          KChartPart* chart) :
  QWidget( parent ),
  _chart( chart ),
  _parent( (KChartWizard*)parent )
{
  Q3ButtonGroup* tmpQGroupBox;
  tmpQGroupBox = new Q3ButtonGroup( this, "GroupBox_1" );
  tmpQGroupBox->setGeometry( 260, 10, 130, 80 );
  //tmpQGroupBox->setFrameStyle( 49 );
  tmpQGroupBox->setAlignment( 1 );

  datarow = new QRadioButton( this, "RadioButton_1" );
  datarow->setGeometry( 270, 40, 100, 20 );
  datarow->setText( i18n("Rows") );
  connect( datarow, SIGNAL( clicked() ),
                   this, SLOT( dataInRowsClicked() ) );
  tmpQGroupBox->insert( datarow );

  datacol = new QRadioButton( this, "RadioButton_2" );
  datacol->setGeometry( 270, 60, 100, 20 );
  datacol->setText( i18n("Columns") );
  datacol->setChecked( true );
  connect( datarow, SIGNAL( clicked() ),
                   this, SLOT( dataInColsClicked() ) );
  tmpQGroupBox->insert( datacol );

  QLabel* tmpQLabel;
  tmpQLabel = new QLabel( this, "Label_3" );
  tmpQLabel->setGeometry( 270, 20, 100, 20 );
  tmpQLabel->setText( i18n("Data is in:") );

  Q3ButtonGroup* descrBG = new Q3ButtonGroup( this );
  descrBG->hide();
  coldescript = new QCheckBox( this, "CheckBox_3" );
  coldescript->setGeometry( 260, 110, 20, 30 );
  coldescript->setChecked( true );
  connect( coldescript, SIGNAL( toggled( bool ) ),
                   this, SLOT( firstColumnIsDescriptionToggled( bool ) ) );
  coldescript->setText( "" );
  descrBG->insert( coldescript );

  tmpQLabel = new QLabel( this, "Label_4" );
  tmpQLabel->setGeometry( 280, 110, 100, 30 );
  tmpQLabel->setText( i18n("Use first row as description") );
  tmpQLabel->setAlignment( 1313 );

  rowdescript = new QCheckBox( this, "CheckBox_4" );
  rowdescript->setGeometry( 260, 150, 20, 30 );
  connect( rowdescript, SIGNAL( toggled( bool ) ),
                   this, SLOT( firstRowIsDescriptionToggled( bool ) ) );
  rowdescript->setText( "" );
  descrBG->insert( rowdescript );

  tmpQLabel = new QLabel( this, "Label_5" );
  tmpQLabel->setGeometry( 280, 150, 100, 30 );
  tmpQLabel->setText( i18n("Use first column as description") );
  tmpQLabel->setAlignment( 1313 );

  Q3Frame* tmpQFrame;
  tmpQFrame = new Q3Frame( this, "Frame_1" );
  tmpQFrame->setGeometry( 10, 10, 240, 220 );
  tmpQFrame->setFrameStyle( Q3Frame::Panel | Q3Frame::Sunken );
  tmpQFrame->setLineWidth( 2 );

  /*
  preview = new kchartWidget( _chart, tmpQFrame );
  preview->show();
  _chart->addAutoUpdate( preview );
  preview->resize( tmpQFrame->contentsRect().width(),
                                   tmpQFrame->contentsRect().height() );
  */
  //parent->resize( 400, 350 );
}


KChartWizardSetupDataPage::~KChartWizardSetupDataPage()
{
    /*
  _chart->removeAutoUpdate( preview );
    */
}


void KChartWizardSetupDataPage::dataInRowsClicked()
{
  _parent->emitNeedNewData( (const char*)_parent->dataArea().local8Bit(), KChartWizard::Row,
                                                                 coldescript->isChecked(),
                                                                 rowdescript->isChecked() );
}

void KChartWizardSetupDataPage::dataInColsClicked()
{
  _parent->emitNeedNewData( (const char*)_parent->dataArea().local8Bit(), KChartWizard::Col,
                                                        coldescript->isChecked(),
                                                        rowdescript->isChecked() );
}

void KChartWizardSetupDataPage::firstColumnIsDescriptionToggled( bool )
{
  _parent->emitNeedNewData( (const char*)_parent->dataArea().local8Bit(),
                                                        ( datarow->isChecked() ? KChartWizard::Row :
                                                          KChartWizard::Col ),
                                                        coldescript->isChecked(),
                                                        rowdescript->isChecked() );
}

void KChartWizardSetupDataPage::firstRowIsDescriptionToggled( bool )
{
  _parent->emitNeedNewData( (const char*)_parent->dataArea().local8Bit(),
			    ( datarow->isChecked() ? KChartWizard::Row 
			                           : KChartWizard::Col ),
			    coldescript->isChecked(),
			    rowdescript->isChecked() );
}

}  //namespace KChart

#include "kchartWizardSetupDataPage.moc"
