#include "koPrintDia.h"

#include <qmsgbox.h>
#include <stdlib.h>
#include <dirent.h>   
#include <unistd.h>

#include <kconfig.h>
#include <kapp.h>
#include <klocale.h>
#include <kfiledialog.h>

#include <assert.h>
#include <stdlib.h>

KoPrintDia::KoPrintDia( QWidget* parent, const char* name )
	: QDialog( parent, name, TRUE ), KoPrintDiaData( this )
{
    QString dir = kapp->kde_configdir().copy();
    dir += "/printer";
    
    printer->insertItem( i18n( "File" ) );
    printer->insertItem( i18n( "UNIX Printer" ) );
    
    // Read the directory listing
    DIR *dp;
    struct dirent *ep;
    dp = opendir( dir.data() );
    if ( dp == NULL )
    {
	QMessageBox::message( i18n( "KOffice Error" ), i18n( "Could not open\n" ) + dir );
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
    connect( pageSetup, SIGNAL( clicked() ), this, SLOT( slotPaperLayout() ) );
    connect( printer, SIGNAL( activated( const char* ) ), this, SLOT( slotPrinter( const char* ) ) );
    connect( browse, SIGNAL( clicked() ), this, SLOT( slotBrowse() ) );
    
    printer->setCurrentItem( 0 );
    slotPrinter( printer->currentText() );

    setCaption( i18n( "Print Dialog") );
}

void KoPrintDia::slotPrinter( const char *_printer )
{
  assert( _printer != 0L );
  
    if ( strcmp( _printer, i18n( "File" ) ) == 0 )
    {
	fileName->setEnabled( TRUE );
	printCmd->setEnabled( FALSE );
	printerName->setEnabled( FALSE );
	browse->setEnabled( true );
    }
    else if ( strcmp( _printer, i18n( "UNIX Printer" ) ) == 0 )
    {
	fileName->setEnabled( FALSE );
	printCmd->setEnabled( TRUE );
	printCmd->setText( "lpr" );
	printerName->setEnabled( TRUE );
	printerName->setText( "lp" );
	browse->setEnabled( false );
    }
    else
    {
	fileName->setEnabled( FALSE );
	printCmd->setEnabled( FALSE );
	printerName->setEnabled( FALSE );
	browse->setEnabled( false );
    }
}

void KoPrintDia::slotPaperLayout()
{
  emit sig_pageLayout();
}

void KoPrintDia::configurePrinter( QPrinter &_printer )
{
    _printer.setNumCopies( atoi( copies->text() ) );
    // _printer.setOrientation( m_pView->doc()->orientation() );
    
    /* switch( m_pView->doc()->paperFormat() )
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
    } */
    
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
      /*QString pap;
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
	_printer.setPrintProgram( exec.data() ); */
    }
}

KoPrintDia::~KoPrintDia()
{
}

KoPrintDiaData::KoPrintDiaData( QWidget* parent )
{
	QLabel* tmpQLabel;
	tmpQLabel = new QLabel( parent, "Label_1" );
	tmpQLabel->setGeometry( 10, 10, 100, 30 );
	tmpQLabel->setText( i18n( "Print To:" ) );
	tmpQLabel->setAlignment( 290 );

	printer = new QComboBox( FALSE, parent, "ComboBox_1" );
	printer->setGeometry( 120, 10, 260, 30 );
	printer->setSizeLimit( 10 );

	tmpQLabel = new QLabel( parent, "Label_2" );
	tmpQLabel->setGeometry( 10, 50, 100, 30 );
	tmpQLabel->setText( i18n( "File Name:" ) );
	tmpQLabel->setAlignment( 290 );

	fileName = new QLineEdit( parent, "LineEdit_1" );
	fileName->setGeometry( 120, 50, 260, 30 );
	fileName->setText( "" );

	browse = new QPushButton( i18n( "Browse..." ), parent );
	browse->setGeometry( 400, 50, 80, 30 );

	tmpQLabel = new QLabel( parent, "Label_3" );
	tmpQLabel->setGeometry( 10, 90, 100, 30 );
	tmpQLabel->setText( i18n( "Print Command:" ) );
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
	cancel->setText( i18n( "Cancel" ) );

	ok = new QPushButton( parent, "PushButton_2" );
	ok->setGeometry( 180, 250, 100, 30 );
	ok->setText( i18n( OK ) );

	tmpQLabel = new QLabel( parent, "Label_4" );
	tmpQLabel->setGeometry( 10, 190, 100, 30 );
	tmpQLabel->setText( i18n( "Copies:" ) );
	tmpQLabel->setAlignment( 290 );

	copies = new QLineEdit( parent, "LineEdit_3" );
	copies->setGeometry( 120, 190, 50, 30 );
	copies->setText( "" );

	tmpQFrame = new QFrame( parent, "Frame_2" );
	tmpQFrame->setGeometry( 10, 230, 380, 10 );
	tmpQFrame->setFrameStyle( 52 );

	pageSetup = new QPushButton( parent, "PushButton_3" );
	pageSetup->setGeometry( 10, 250, 100, 30 );
	pageSetup->setText( i18n( "Page Setup..." ) );

	tmpQLabel = new QLabel( parent, "Label_5" );
	tmpQLabel->setGeometry( 10, 130, 100, 30 );
	tmpQLabel->setText( i18n( "Printer Name:" ) );
	tmpQLabel->setAlignment( 290 );

	printerName = new QLineEdit( parent, "LineEdit_4" );
	printerName->setGeometry( 120, 130, 260, 30 );
	printerName->setText( "" );

	parent->resize( 500, 290 );
}

void KoPrintDia::slotBrowse()
{
  QString file = KFileDialog::getSaveFileName( getenv( "HOME" ) );
  if ( !file.isEmpty() )
    fileName->setText( file );
}

bool KoPrintDia::print( QPrinter &_printer, QObject *dest = 0L, const char *slot = 0L )
{
  KoPrintDia dlg;

  if ( dest != 0L && slot != 0L )
    connect( &dlg, SIGNAL( sig_pageLayout() ), dest, slot );

  if ( dlg.exec() )
  {
    dlg.configurePrinter( _printer );
    return true;
  }
  
  return false;
}

#include "koPrintDia.moc"
