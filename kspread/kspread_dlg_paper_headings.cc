#include "kspread_dlg_paper_headings.h"

KSpreadPaperHeadings::KSpreadPaperHeadings( QWidget* parent, const char* name )
	: QWidget( parent, name ), KSpreadPaperHeadingsData( this )
{
}

KSpreadPaperHeadings::~KSpreadPaperHeadings()
{
}

KSpreadPaperHeadingsData::KSpreadPaperHeadingsData( QWidget* parent )
{
	m_headLeft = new QLineEdit( parent, "LineEdit_5" );
	m_headLeft->setGeometry( 10, 70, 130, 30 );
	m_headLeft->setText( "" );

	QLabel* tmpQLabel;
	tmpQLabel = new QLabel( parent, "Label_5" );
	tmpQLabel->setGeometry( 10, 40, 100, 30 );
	tmpQLabel->setText( "Left:" );

	tmpQLabel = new QLabel( parent, "Label_6" );
	tmpQLabel->setGeometry( 10, 10, 100, 30 );
	tmpQLabel->setText( "Head Line" );

	tmpQLabel = new QLabel( parent, "Label_8" );
	tmpQLabel->setGeometry( 140, 40, 100, 30 );
	tmpQLabel->setText( "Mid:" );

	tmpQLabel = new QLabel( parent, "Label_9" );
	tmpQLabel->setGeometry( 260, 40, 100, 30 );
	tmpQLabel->setText( "Right" );

	m_headMid = new QLineEdit( parent, "LineEdit_6" );
	m_headMid->setGeometry( 140, 70, 120, 30 );
	m_headMid->setText( "" );

	m_headRight = new QLineEdit( parent, "LineEdit_7" );
	m_headRight->setGeometry( 260, 70, 120, 30 );
	m_headRight->setText( "" );

	tmpQLabel = new QLabel( parent, "Label_11" );
	tmpQLabel->setGeometry( 10, 120, 100, 30 );
	tmpQLabel->setText( "Foot Line" );

	tmpQLabel = new QLabel( parent, "Label_12" );
	tmpQLabel->setGeometry( 10, 150, 100, 30 );
	tmpQLabel->setText( "Left:" );

	footLeft = new QLineEdit( parent, "LineEdit_8" );
	footLeft->setGeometry( 10, 180, 130, 30 );
	footLeft->setText( "" );

	footMid = new QLineEdit( parent, "LineEdit_9" );
	footMid->setGeometry( 140, 180, 120, 30 );
	footMid->setText( "" );

	footRight = new QLineEdit( parent, "LineEdit_10" );
	footRight->setGeometry( 260, 180, 120, 30 );
	footRight->setText( "" );

	tmpQLabel = new QLabel( parent, "Label_13" );
	tmpQLabel->setGeometry( 140, 150, 100, 30 );
	tmpQLabel->setText( "Mid:" );

	tmpQLabel = new QLabel( parent, "Label_14" );
	tmpQLabel->setGeometry( 260, 150, 100, 30 );
	tmpQLabel->setText( "Right:" );

	tmpQLabel = new QLabel( parent, "Label_15" );
	tmpQLabel->setGeometry( 10, 260, 380, 30 );
	tmpQLabel->setText( "<page>, <name>, <file>, <time>, <date>, <author>, <email>" );

	tmpQLabel = new QLabel( parent, "Label_16" );
	tmpQLabel->setGeometry( 10, 230, 100, 30 );
	tmpQLabel->setText( "Makros:" );

	parent->resize( 400, 300 );
}

#include "kspread_dlg_paper_headings.moc"
