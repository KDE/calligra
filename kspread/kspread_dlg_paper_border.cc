#include "kspread_dlg_paper_border.h"

KSpreadPaperBorder::KSpreadPaperBorder( QWidget* parent, const char* name )
	: QWidget( parent, name ), KSpreadPaperBorderData( this )
{
}

KSpreadPaperBorder::~KSpreadPaperBorder()
{
}

KSpreadPaperBorderData::KSpreadPaperBorderData( QWidget* parent )
{
	leftBorder = new QLineEdit( parent, "LineEdit_1" );
	leftBorder->setGeometry( 20, 40, 80, 30 );
	leftBorder->setText( "" );

	QLabel* tmpQLabel;
	tmpQLabel = new QLabel( parent, "Label_1" );
	tmpQLabel->setGeometry( 20, 10, 100, 30 );
	tmpQLabel->setText( "Left Border:" );

	tmpQLabel = new QLabel( parent, "Label_2" );
	tmpQLabel->setGeometry( 130, 10, 100, 30 );
	tmpQLabel->setText( "Right Border:" );

	rightBorder = new QLineEdit( parent, "LineEdit_2" );
	rightBorder->setGeometry( 130, 40, 80, 30 );
	rightBorder->setText( "" );

	tmpQLabel = new QLabel( parent, "Label_3" );
	tmpQLabel->setGeometry( 20, 80, 90, 30 );
	tmpQLabel->setText( "Top Border:" );

	tmpQLabel = new QLabel( parent, "Label_4" );
	tmpQLabel->setGeometry( 130, 80, 100, 30 );
	tmpQLabel->setText( "Bottom Border:" );

	topBorder = new QLineEdit( parent, "LineEdit_3" );
	topBorder->setGeometry( 20, 110, 80, 30 );
	topBorder->setText( "" );

	bottomBorder = new QLineEdit( parent, "LineEdit_4" );
	bottomBorder->setGeometry( 130, 110, 80, 30 );
	bottomBorder->setText( "" );

	a3 = new QRadioButton( parent, "RadioButton_1" );
	a3->setGeometry( 30, 170, 90, 30 );
	a3->setText( "A3" );

	a4 = new QRadioButton( parent, "RadioButton_2" );
	a4->setGeometry( 30, 200, 90, 30 );
	a4->setText( "A4" );

	a5 = new QRadioButton( parent, "RadioButton_3" );
	a5->setGeometry( 30, 230, 90, 30 );
	a5->setText( "A5" );

	letter = new QRadioButton( parent, "RadioButton_4" );
	letter->setGeometry( 120, 170, 70, 30 );
	letter->setText( "Letter" );

	legal = new QRadioButton( parent, "RadioButton_5" );
	legal->setGeometry( 120, 200, 70, 30 );
	legal->setText( "Legal" );

	executive = new QRadioButton( parent, "RadioButton_6" );
	executive->setGeometry( 120, 230, 80, 30 );
	executive->setText( "Executive" );

	QButtonGroup* tmpQButtonGroup;
	tmpQButtonGroup = new QButtonGroup( parent, "ButtonGroup_1" );
	tmpQButtonGroup->setGeometry( 20, 160, 190, 110 );
	tmpQButtonGroup->setFrameStyle( 49 );
	tmpQButtonGroup->setAlignment( 1 );
	tmpQButtonGroup->insert( a3 );
	tmpQButtonGroup->insert( a4 );
	tmpQButtonGroup->insert( a5 );
	tmpQButtonGroup->insert( letter );
	tmpQButtonGroup->insert( legal );
	tmpQButtonGroup->insert( executive );
	tmpQButtonGroup->lower();

	parent->resize( 400, 300 );
}

#include "kspread_dlg_paper_border.moc"
