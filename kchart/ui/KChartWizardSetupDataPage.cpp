/* $Id: $ */

#include "KChartWizardSetupDataPage.h"

#include <qradiobt.h>
#include <qchkbox.h>
#include <qbttngrp.h>
#include <qlabel.h>

KChartWizardSetupDataPage::KChartWizardSetupDataPage( QWidget* parent ) :
  QWidget( parent )
{
	QButtonGroup* tmpQGroupBox;
	tmpQGroupBox = new QButtonGroup( this, "GroupBox_1" );
	tmpQGroupBox->setGeometry( 260, 10, 130, 80 );
	tmpQGroupBox->setFrameStyle( 49 );
	tmpQGroupBox->setAlignment( 1 );

 	datarow = new QRadioButton( this, "RadioButton_1" );
 	datarow->setGeometry( 270, 40, 100, 20 );
 	datarow->setText( "Rows" );
 	tmpQGroupBox->insert( datarow );
	
 	datacol = new QRadioButton( this, "RadioButton_2" );
 	datacol->setGeometry( 270, 60, 100, 20 );
 	datacol->setText( "Columns" );
 	tmpQGroupBox->insert( datacol );

 	QLabel* tmpQLabel;
 	tmpQLabel = new QLabel( this, "Label_3" );
 	tmpQLabel->setGeometry( 270, 20, 100, 20 );
 	tmpQLabel->setText( "Data is in:" );

 	coldescript = new QCheckBox( this, "RadioButton_3" );
 	coldescript->setGeometry( 260, 110, 20, 30 );
 	coldescript->setText( "" );

 	tmpQLabel = new QLabel( this, "Label_4" );
 	tmpQLabel->setGeometry( 280, 110, 100, 30 );
 	tmpQLabel->setText( "Use first row as description" );
 	tmpQLabel->setAlignment( 1313 );

 	rowdescript = new QCheckBox( this, "RadioButton_4" );
 	rowdescript->setGeometry( 260, 150, 20, 30 );
 	rowdescript->setText( "" );

 	tmpQLabel = new QLabel( this, "Label_5" );
 	tmpQLabel->setGeometry( 280, 150, 100, 30 );
 	tmpQLabel->setText( "Use first column as description" );
 	tmpQLabel->setAlignment( 1313 );

 	QFrame* tmpQFrame;
 	tmpQFrame = new QFrame( this, "Frame_1" );
 	tmpQFrame->setGeometry( 10, 10, 240, 220 );
 	{
 		QColorGroup normal( QColor( QRgb(0) ), QColor( QRgb(16777215) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
 		QColorGroup disabled( QColor( QRgb(8421504) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(8421504) ), QColor( QRgb(12632256) ) );
 		QColorGroup active( QColor( QRgb(0) ), QColor( QRgb(12632256) ), QColor( QRgb(16777215) ), QColor( QRgb(6316128) ), QColor( QRgb(10789024) ), QColor( QRgb(0) ), QColor( QRgb(16777215) ) );
 		QPalette palette( normal, disabled, active );
 		tmpQFrame->setPalette( palette );
 	}
 	tmpQFrame->setFrameStyle( 50 );
 	tmpQFrame->setLineWidth( 2 );

 	grid = new QCheckBox( this, "RadioButton_5" );
 	grid->setGeometry( 260, 200, 130, 30 );
 	grid->setText( "Display Grid" );

	parent->resize( 400, 350 );
}


