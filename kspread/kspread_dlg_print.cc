#include "kspread_view.h"
#include "kspread_dlg_print.h"
#include "kspread_doc.h"

#include <qmsgbox.h>
#include <stdlib.h>
#include <dirent.h>   
#include <unistd.h>

#include <kconfig.h>
#include <kapp.h>

KSpreadPrintDlg::KSpreadPrintDlg( KSpreadView *_view, QWidget* parent, const char* name )
	: QDialog( parent, name, TRUE ), KSpreadPrintDlgData( this )
{
    m_pView = _view;
    
    QString dir = kapp->kde_configdir().copy();
    dir += "/printer";
    // TODO
    // QString dir = "/opt/kde/share/kprinter";
    
    // Read the directory listing
    DIR *dp;
    struct dirent *ep;
    dp = opendir( dir.data() );
    if ( dp == NULL )
    {
	QMessageBox::message( "KLP Error", "Could not open\n" + dir );
    }
    else
    {
	while ( ( ep = readdir( dp ) ) )
	{
	    QString tmp = ep->d_name;
	    if ( tmp.length() > 7 && tmp.right(7) == ".kdelnk" )
	    {
		tmp = tmp.left( tmp.length() - 7 ).data();
		printer->insertItem( tmp.data() );
	    }
	}
	(void) closedir( dp );
    }
    
    copies->setText( "1" );
    
    connect( ok, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( cancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    // Does not work, since this is a modal dialog box
    // connect( pageSetup, SIGNAL( clicked() ), this, SLOT( slotPageSetup() ) );
    connect( printer, SIGNAL( activated( const char* ) ), this, SLOT( slotPrinter( const char* ) ) );

    printer->setCurrentItem( 0 );
    slotPrinter( printer->currentText() );
}

void KSpreadPrintDlg::slotPrinter( const char *_printer )
{
    if ( strcmp( _printer, "File" ) == 0 )
    {
	fileName->setEnabled( TRUE );
	printCmd->setEnabled( FALSE );
	printerName->setEnabled( FALSE );
    }
    else if ( strcmp( _printer, "UNIX Printer" ) == 0 )
    {
	fileName->setEnabled( FALSE );
	printCmd->setEnabled( TRUE );
	printerName->setEnabled( TRUE );
    }
    else
    {
	fileName->setEnabled( FALSE );
	printCmd->setEnabled( FALSE );
	printerName->setEnabled( FALSE );
    }
}

void KSpreadPrintDlg::slotPageSetup()
{
  m_pView->paperLayoutDlg();
}

void KSpreadPrintDlg::configurePrinter( QPrinter &_printer )
{
    _printer.setNumCopies( atoi( copies->text() ) );
    _printer.setOrientation( m_pView->doc()->orientation() );
    
    switch( m_pView->doc()->paperFormat() )
    {
    case KSpreadDoc::A4:
	_printer.setPageSize( QPrinter::A4 );
	break;
    case KSpreadDoc::LETTER:
	_printer.setPageSize( QPrinter::Letter );
	break;
    case KSpreadDoc::EXECUTIVE:
	_printer.setPageSize( QPrinter::Executive );
	break;
    default:
	printf("ERROR: Page format not supported by Qt\n");
    }
    
    if ( strcmp( printer->currentText(), "File" ) == 0 )
    {
	_printer.setOutputToFile( TRUE );
	_printer.setOutputFileName( fileName->text() );
    }
    else if ( strcmp( printer->currentText(), "UNIX Printer" ) == 0 )
    {
	_printer.setOutputToFile( FALSE );
	_printer.setPrinterName( printerName->text() );
	_printer.setPrintProgram( printCmd->text() );
    }
    else
    {
	QString pap;
	switch( m_pView->doc()->paperFormat() )
	{
	case KSpreadDoc::A4:
	    pap = "a4";
	    break;
	case KSpreadDoc::LETTER:
	    pap = "letter";
	    break;
	case KSpreadDoc::EXECUTIVE:
	    pap = "executive";
	    break;
	case KSpreadDoc::A5:
	    pap = "a5";
	    break;
	default:
	    printf("ERROR: Paper Size not supported by kxcl\n");
	}

	QString tmp2;
	tmp2.sprintf( "%s/printer/%s%s", getenv( "KDEDIR" ), printer->currentText(), ".kdelnk" );
	QFile f( tmp2.data() );
	if ( !f.open( IO_ReadOnly ) )
	{
	    QMessageBox::message("KStylus Error","Could not read\n" + tmp2 );
	    return;
	}
	
	QTextStream pstream( &f );
	KConfig config( tmp2 );
	config.setGroup( "KDE Desktop Entry" );
	QString tmp = config.readEntry( "Exec" );
	f.close();

	QString exec;
	exec.sprintf( "%s/bin/", getenv( "KDEDIR" ) );
	int j = tmp.find( " " );
	if ( j == -1 )
	    exec += tmp.data();
	else
	    exec += tmp.left( j );
	
	_printer.setOutputToFile( FALSE );
	QString tmp3;
	tmp3.sprintf( "%s/printer/%s%s#%s",getenv( "KDEDIR" ), printer->currentText(), ".kdelnk",  pap.data() );
	_printer.setPrinterName( tmp3.data() );
	_printer.setPrintProgram( exec.data() );
    }
}

KSpreadPrintDlg::~KSpreadPrintDlg()
{
}

KSpreadPrintDlgData::KSpreadPrintDlgData( QWidget* parent )
{
	QLabel* tmpQLabel;
	tmpQLabel = new QLabel( parent, "Label_1" );
	tmpQLabel->setGeometry( 10, 10, 100, 30 );
	tmpQLabel->setText( "Print To:" );
	tmpQLabel->setAlignment( 290 );

	printer = new QComboBox( FALSE, parent, "ComboBox_1" );
	printer->setGeometry( 120, 10, 260, 30 );
	printer->setSizeLimit( 10 );
	printer->insertItem( "File" );
	printer->insertItem( "UNIX Printer" );

	tmpQLabel = new QLabel( parent, "Label_2" );
	tmpQLabel->setGeometry( 10, 50, 100, 30 );
	tmpQLabel->setText( "File Name:" );
	tmpQLabel->setAlignment( 290 );

	fileName = new QLineEdit( parent, "LineEdit_1" );
	fileName->setGeometry( 120, 50, 260, 30 );
	fileName->setText( "" );

	tmpQLabel = new QLabel( parent, "Label_3" );
	tmpQLabel->setGeometry( 10, 90, 100, 30 );
	tmpQLabel->setText( "Print Command:" );
	tmpQLabel->setAlignment( 290 );

	printCmd = new QLineEdit( parent, "LineEdit_2" );
	printCmd->setGeometry( 120, 90, 260, 30 );
	printCmd->setText( "" );

	QFrame* tmpQFrame;
	tmpQFrame = new QFrame( parent, "Frame_1" );
	tmpQFrame->setGeometry( 10, 170, 380, 10 );
	tmpQFrame->setFrameStyle( 52 );

	cancel = new QPushButton( parent, "PushButton_1" );
	cancel->setGeometry( 290, 250, 100, 30 );
	cancel->setText( "Cancel" );

	ok = new QPushButton( parent, "PushButton_2" );
	ok->setGeometry( 180, 250, 100, 30 );
	ok->setText( "OK" );

	tmpQLabel = new QLabel( parent, "Label_4" );
	tmpQLabel->setGeometry( 10, 190, 100, 30 );
	tmpQLabel->setText( "Copies:" );
	tmpQLabel->setAlignment( 290 );

	copies = new QLineEdit( parent, "LineEdit_3" );
	copies->setGeometry( 120, 190, 50, 30 );
	copies->setText( "" );

	tmpQFrame = new QFrame( parent, "Frame_2" );
	tmpQFrame->setGeometry( 10, 230, 380, 10 );
	tmpQFrame->setFrameStyle( 52 );

	/* pageSetup = new QPushButton( parent, "PushButton_3" );
	pageSetup->setGeometry( 10, 250, 100, 30 );
	pageSetup->setText( "Page Setup..." ); */

	tmpQLabel = new QLabel( parent, "Label_5" );
	tmpQLabel->setGeometry( 10, 130, 100, 30 );
	tmpQLabel->setText( "Printer Name:" );
	tmpQLabel->setAlignment( 290 );

	printerName = new QLineEdit( parent, "LineEdit_4" );
	printerName->setGeometry( 120, 130, 260, 30 );
	printerName->setText( "" );

	parent->resize( 400, 290 );
}

#include "kspread_dlg_print.moc"
