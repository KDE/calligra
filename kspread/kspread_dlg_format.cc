#include "kspread_dlg_format.h"
#include "kspread_view.h"
#include "kspread_table.h"
#include "kspread_factory.h"
#include "kspread_undo.h"
#include "kspread_doc.h"

#include <kinstance.h>
#include <kstddirs.h>
#include <ksimpleconfig.h>
#include <kbuttonbox.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qstringlist.h>
#include <qdom.h>
#include <qfile.h>


KSpreadFormatDlg::KSpreadFormatDlg( KSpreadView* view, const char* name )
    : QDialog( view, name, TRUE )
{
    for( int i = 0; i < 16; ++i )
	m_cells[ i ] = 0;
    
    m_view = view;
    setCaption( i18n("Table Style") );
    QVBoxLayout* vbox = new QVBoxLayout( this, 6, 6 );
    m_combo = new QComboBox( this );
    m_label = new QLabel( this );

    vbox->addWidget( m_combo );
    vbox->addWidget( m_label );

    KButtonBox *bb = new KButtonBox( this );
    bb->addStretch();
    QPushButton* ok = bb->addButton( i18n("OK") );
    ok->setDefault( TRUE );
    QPushButton* close = bb->addButton( i18n( "Close" ) );
    bb->layout();

    vbox->addWidget( bb );

    QStringList lst = KSpreadFactory::global()->dirs()->findAllResources( "table-styles", "*.ksts", TRUE );

    QStringList::Iterator it = lst.begin();
    for( ; it != lst.end(); ++it )
    {
	KSimpleConfig cfg( *it, TRUE );
	cfg.setGroup( "Table-Style" );

	Entry e;
	e.config = *it;
	e.xml = cfg.readEntry( "XML" );
	e.image = cfg.readEntry( "Image" );
	e.name = cfg.readEntry( "Name" );
	
	m_entries.append( e );
	
	m_combo->insertItem( e.name );
    }

    slotActivated( 0 );

    connect( ok, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
    connect( close, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( m_combo, SIGNAL( activated( int ) ), this, SLOT( slotActivated( int ) ) );
}

KSpreadFormatDlg::~KSpreadFormatDlg()
{
    for( int i = 0; i < 16; ++i )
	delete m_cells[ i ];
}

void KSpreadFormatDlg::slotActivated( int index )
{
    QString img = KSpreadFactory::global()->dirs()->findResource( "table-styles", m_entries[ index ].image );
    if ( img.isEmpty() )
    {
	QString str( i18n( "Could not find image %1" ) );
	str = str.arg( m_entries[ index ].image );
	KMessageBox::error( this, str );
	return;
    }

    QPixmap pix( img );
    if ( pix.isNull() )
    {
	QString str( i18n( "Could not load image %1" ) );
	str = str.arg( img );
	KMessageBox::error( this,str );
	return;
    }

    m_label->setPixmap( pix );
}

void KSpreadFormatDlg::slotOk()
{
    QString xml = KSpreadFactory::global()->dirs()->findResource( "table-styles", m_entries[ m_combo->currentItem() ].xml );
    if ( xml.isEmpty() )
    {
	QString str( i18n( "Could not find table-style XML file '%1'" ) );
	str = str.arg( m_entries[ m_combo->currentItem() ].xml );
	KMessageBox::error( this, str );
	return;
    }

    QFile file( xml );
    file.open( IO_ReadOnly );
    QDomDocument doc;
    doc.setContent( &file );
    file.close();

    if ( !parseXML( doc ) )
    {
	QString str( i18n( "Parsing error in table-style XML file %1" ) );
	str = str.arg( m_entries[ m_combo->currentItem() ].xml );
	KMessageBox::error( this, str );
	return;
    }

    QRect r = m_view->activeTable()->selectionRect();

    if ( !m_view->doc()->undoBuffer()->isLocked() )
    {
        QString title=i18n("Change layout");
        KSpreadUndoCellLayout *undo = new KSpreadUndoCellLayout( m_view->doc(), m_view->activeTable(), r ,title);
        m_view->doc()->undoBuffer()->appendUndo( undo );
    }
    //
    // Set colors, borders etc.
    //

    // Top left corner
    KSpreadCell* cell = m_view->activeTable()->nonDefaultCell( r.left(), r.top() );
    cell->copy( *m_cells[0] );

    // Top column
    int x, y;
    for( x = r.left() + 1; x <= r.right(); ++x )
    {
	int pos = 1 + ( ( x - r.left() - 1 ) % 2 );
	KSpreadCell* cell = m_view->activeTable()->nonDefaultCell( x, r.top() );
        if(!cell->isObscuringForced())
        {
        cell->copy( *m_cells[ pos ] );

	KSpreadLayout* c;
	if ( x == r.right() )
	    c = m_cells[2];
	else
	    c = m_cells[1];

	if ( c )
	    cell->setTopBorderPen( c->topBorderPen( 0, 0 ) );

	if ( x == r.left() + 1 )
	    c = m_cells[1];
	else
	    c = m_cells[2];

	if ( c )
	    cell->setLeftBorderPen( c->leftBorderPen( 0, 0 ) );
        }
    }

    cell = m_view->activeTable()->nonDefaultCell( r.right(), r.top() );
    if ( m_cells[3] )
	cell->setRightBorderPen( m_cells[3]->leftBorderPen( 0, 0 ) );

    // Left row
    for( y = r.top() + 1; y <= r.bottom(); ++y )
    {
	int pos = 4 + ( ( y - r.top() - 1 ) % 2 ) * 4;
	KSpreadCell* cell = m_view->activeTable()->nonDefaultCell( r.left(), y );
        if(!cell->isObscuringForced())
        {
        cell->copy( *m_cells[ pos ] );

	KSpreadLayout* c;
	if ( y == r.bottom() )
	    c = m_cells[8];
	else
	    c = m_cells[4];

	if ( c )
	    cell->setLeftBorderPen( c->leftBorderPen( 0, 0 ) );

	if ( y == r.top() + 1 )
	    c = m_cells[4];
	else
	    c = m_cells[8];

	if ( c )
	    cell->setTopBorderPen( c->topBorderPen( 0, 0 ) );
        }
    }

    // Body
    for( x = r.left() + 1; x <= r.right(); ++x )
	for( y = r.top() + 1; y <= r.bottom(); ++y )
        {
	    int pos = 5 + ( ( y - r.top() - 1 ) % 2 ) * 4 + ( ( x - r.left() - 1 ) % 2 );
	    KSpreadCell* cell = m_view->activeTable()->nonDefaultCell( x, y );
            if(!cell->isObscuringForced())
            {
            cell->copy( *m_cells[ pos ] );

	    KSpreadLayout* c;
	    if ( x == r.left() + 1 )
		c = m_cells[ 5 + ( ( y - r.top() - 1 ) % 2 ) * 4 ];
	    else
		c = m_cells[ 6 + ( ( y - r.top() - 1 ) % 2 ) * 4 ];

	    if ( c )
		cell->setLeftBorderPen( c->leftBorderPen( 0, 0 ) );

	    if ( y == r.top() + 1 )
		c = m_cells[ 5 + ( ( x - r.left() - 1 ) % 2 ) ];
	    else
		c = m_cells[ 9 + ( ( x - r.left() - 1 ) % 2 ) ];

	    if ( c )
		cell->setTopBorderPen( c->topBorderPen( 0, 0 ) );
            }
	}

    // Outer right border
    for( y = r.top(); y <= r.bottom(); ++y )
    {
	KSpreadCell* cell = m_view->activeTable()->nonDefaultCell( r.right(), y );
        if(!cell->isObscuringForced())
        {
	if ( y == r.top() )
        {
	    if ( m_cells[3] )
		cell->setRightBorderPen( m_cells[3]->leftBorderPen( 0, 0 ) );
	}
	else if ( y == r.right() )
        {
	    if ( m_cells[11] )
		cell->setRightBorderPen( m_cells[11]->leftBorderPen( 0, 0 ) );
	}
	else
        {
	    if ( m_cells[7] )
		cell->setRightBorderPen( m_cells[7]->leftBorderPen( 0, 0 ) );
	}
        }
    }

    // Outer bottom border
    for( x = r.left(); x <= r.right(); ++x )
    {
	KSpreadCell* cell = m_view->activeTable()->nonDefaultCell( x, r.bottom() );
        if(!cell->isObscuringForced())
        {
        if ( x == r.left() )
        {
	    if ( m_cells[12] )
		cell->setBottomBorderPen( m_cells[12]->topBorderPen( 0, 0 ) );
	}
	else if ( x == r.right() )
        {
	    if ( m_cells[14] )
		cell->setBottomBorderPen( m_cells[14]->topBorderPen( 0, 0 ) );
	}
	else
        {
	    if ( m_cells[13] )
		cell->setBottomBorderPen( m_cells[13]->topBorderPen( 0, 0 ) );
	}
        }
    }

    m_view->activeTable()->setSelection( QRect(), r.topLeft() );
    m_view->doc()->setModified( true ); 
    accept();
}

bool KSpreadFormatDlg::parseXML( const QDomDocument& doc )
{
    for( int i = 0; i < 16; ++i )
    {
	delete m_cells[ i ];
	m_cells[ i ] = 0;
    }

    QDomElement e = doc.documentElement().firstChild().toElement();
    for( ; !e.isNull(); e = e.nextSibling().toElement() )
    {
	if ( e.tagName() == "cell" )
        {
	    KSpreadTable* table = m_view->activeTable();
	    KSpreadLayout* cell = new KSpreadLayout( table );

	    if ( !cell->load( e.namedItem("format").toElement(),Normal ) )
		return false;

	    int row = e.attribute("row").toInt();
	    int column = e.attribute("column").toInt();
	    int i = (row-1)*4 + (column-1);
	    if ( i < 0 || i >= 16 )
		return false;
	    
	    m_cells[ i ] = cell;
	}
    }

    return TRUE;
}

#include "kspread_dlg_format.moc"
