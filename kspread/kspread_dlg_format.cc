#include "kspread_dlg_format.h"
#include "kspread_view.h"
#include "kspread_table.h"
#include "kspread_factory.h"

#include <kinstance.h>
#include <kstddirs.h>
#include <ksimpleconfig.h>
#include <kbuttonbox.h>
#include <klocale.h>

#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qstringlist.h>
#include <qdom.h>
#include <qfile.h>
#include <qmessagebox.h>

KSpreadFormatDlg::KSpreadFormatDlg( KSpreadView* view, const char* name )
    : QDialog( view, name, TRUE )
{
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

void KSpreadFormatDlg::slotActivated( int index )
{
    QString img = KSpreadFactory::global()->dirs()->findResource( "table-styles", m_entries[ index ].image );
    if ( img.isEmpty() )
    {
	QString str( i18n( "Could not find image %1" ) );
	str = str.arg( m_entries[ index ].image );
	QMessageBox::critical( this, i18n("KSpread Error"), str );
	return;
    }

    QPixmap pix( img );
    if ( pix.isNull() )
    {
	QString str( i18n( "Could not load image %1" ) );
	str = str.arg( img );
	QMessageBox::critical( this, i18n("KSpread Error"), str );
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
	QMessageBox::critical( this, i18n("KSpread Error"), str );
	return;
    }

    QFile file( xml );
    file.open( IO_ReadOnly );
    QDomDocument doc( &file );
    file.close();

    if ( !parseXML( doc ) )
    {
	QString str( i18n( "Parsing error in table-style XML file %1" ) );
	str = str.arg( m_entries[ m_combo->currentItem() ].xml );
	QMessageBox::critical( this, i18n("KSpread Error"), str );
	return;
    }
	
    QRect r = m_view->activeTable()->selectionRect();

    //
    // Set colors, borders etc.
    //
    KSpreadCell* cell = m_view->activeTable()->nonDefaultCell( r.left(), r.top() );
    cell->setBgColor( m_cells[0].bgColor );
    cell->setAlign( m_cells[0].align );
    cell->setFloatFormat( m_cells[0].floatFormat );
    cell->setFloatColor( m_cells[0].floatColor );
    cell->setTextPen( m_cells[0].pen );
    cell->setTextFont( m_cells[0].font );
    cell->setLeftBorderPen( m_cells[0].leftPen );
    cell->setTopBorderPen( m_cells[0].topPen );
	
    // Top column
    int x, y;
    for( x = r.left() + 1; x <= r.right(); ++x )
    {
	int pos = 1 + ( ( x - r.left() - 1 ) % 2 );
	KSpreadCell* cell = m_view->activeTable()->nonDefaultCell( x, r.top() );
	cell->setBgColor( m_cells[ pos ].bgColor );
	cell->setAlign( m_cells[pos].align );
	cell->setFloatFormat( m_cells[pos].floatFormat );
	cell->setFloatColor( m_cells[pos].floatColor );
	cell->setTextPen( m_cells[pos].pen );
	cell->setTextFont( m_cells[pos].font );
	if ( x == r.right() )
	    cell->setTopBorderPen( m_cells[2].topPen );
	else
	    cell->setTopBorderPen( m_cells[1].topPen );
	if ( x == r.left() + 1 )
	    cell->setLeftBorderPen( m_cells[1].leftPen );
	else
	    cell->setLeftBorderPen( m_cells[2].leftPen );
    }
    cell = m_view->activeTable()->nonDefaultCell( r.right() + 1, r.top() );
    cell->setLeftBorderPen( m_cells[3].leftPen );

    // Left row
    for( y = r.top() + 1; y <= r.bottom(); ++y )
    {
	int pos = 4 + ( ( y - r.top() - 1 ) % 2 ) * 4;
	KSpreadCell* cell = m_view->activeTable()->nonDefaultCell( r.left(), y );
	cell->setBgColor( m_cells[ pos ].bgColor );
	cell->setAlign( m_cells[pos].align );
	cell->setFloatFormat( m_cells[pos].floatFormat );
	cell->setFloatColor( m_cells[pos].floatColor );
	cell->setTextPen( m_cells[pos].pen );
	cell->setTextFont( m_cells[pos].font );
	if ( y == r.bottom() )
	    cell->setLeftBorderPen( m_cells[8].leftPen );
	else
	    cell->setLeftBorderPen( m_cells[4].leftPen );
	if ( y == r.top() + 1 )
	    cell->setTopBorderPen( m_cells[4].topPen );
	else
	    cell->setTopBorderPen( m_cells[8].topPen );
    }

    // Body
    for( x = r.left() + 1; x <= r.right(); ++x )
	for( y = r.top() + 1; y <= r.bottom(); ++y )
        {
	    int pos = 5 + ( ( y - r.top() - 1 ) % 2 ) * 4 + ( ( x - r.left() - 1 ) % 2 );
	    KSpreadCell* cell = m_view->activeTable()->nonDefaultCell( x, y );
	    cell->setBgColor( m_cells[ pos ].bgColor );
	    cell->setAlign( m_cells[pos].align );
	    cell->setFloatFormat( m_cells[pos].floatFormat );
	    cell->setFloatColor( m_cells[pos].floatColor );
	    cell->setTextPen( m_cells[pos].pen );
	    cell->setTextFont( m_cells[pos].font );
	    if ( x == r.left() + 1 )
		cell->setLeftBorderPen( m_cells[ 5 + ( ( y - r.top() - 1 ) % 2 ) * 4 ].leftPen );
	    else
		cell->setLeftBorderPen( m_cells[ 6 + ( ( y - r.top() - 1 ) % 2 ) * 4 ].leftPen );
	    if ( y == r.top() + 1 )
		cell->setTopBorderPen( m_cells[ 5 + ( ( x - r.left() - 1 ) % 2 ) ].topPen );
	    else
		cell->setTopBorderPen( m_cells[ 9 + ( ( x - r.left() - 1 ) % 2 ) ].topPen );
	}

    // Outer right border
    for( y = r.top(); y <= r.bottom(); ++y )
    {
	KSpreadCell* cell = m_view->activeTable()->nonDefaultCell( r.right() + 1, y );
	if ( y == r.top() )
	    cell->setLeftBorderPen( m_cells[3].leftPen );
	else if ( y == r.right() )
	    cell->setLeftBorderPen( m_cells[11].leftPen );
	else
	    cell->setLeftBorderPen( m_cells[7].leftPen );
    }

    // Outer bottom border
    for( x = r.left(); x <= r.right(); ++x )
    {
	KSpreadCell* cell = m_view->activeTable()->nonDefaultCell( x, r.bottom() + 1 );
	if ( x == r.left() )
	    cell->setTopBorderPen( m_cells[12].topPen );
	else if ( x == r.right() )
	    cell->setTopBorderPen( m_cells[14].topPen );
	else
	    cell->setTopBorderPen( m_cells[13].topPen );
    }

    m_view->activeTable()->setSelection( QRect( 0, 0, 0, 0 ) );

    accept();
}

bool KSpreadFormatDlg::parseXML( const QDomDocument& doc )
{
    m_cells.clear();
    for( int i = 0; i < 15; ++i )
    {
	Cell cell;
	m_cells.append( cell );
    }

    QDomElement e = doc.documentElement().firstChild().toElement();
    for( ; !e.isNull(); e = e.nextSibling().toElement() )
    {
	if ( e.tagName() == "cell" )
        {
	    bool ok;
	    int row = e.attribute( "row" ).toInt( &ok );
	    if ( !ok )
		return false;
	    int column = e.attribute( "column" ).toInt( &ok );
	    if ( !ok )
		return false;

	    Cell cell;
	    cell.align = KSpreadCell::Undefined;
	    cell.floatFormat = KSpreadCell::OnlyNegSigned;
	    cell.floatColor = KSpreadCell::AllBlack;
	
	    QDomElement f = e.namedItem( "format" ).toElement();
	    if ( !f.isNull() )
	    {
		if ( f.hasAttribute( "align" ) )
	        {
		    cell.align = (KSpreadCell::Align)f.attribute("align").toInt( &ok );
		    if ( !ok )
			return FALSE;
		}
		if ( f.hasAttribute( "bgcolor" ) )
		    cell.bgColor = QColor( f.attribute( "bgcolor" ) );
		if ( f.hasAttribute( "float" ) )
	        {
		    cell.floatFormat = (KSpreadCell::FloatFormat)f.attribute("float").toInt( &ok );
		    if ( !ok )
			return FALSE;
		}
		if ( f.hasAttribute( "floatcolor" ) )
	        {
		    cell.floatColor = (KSpreadCell::FloatColor)f.attribute("floatcolor").toInt( &ok );
		    if ( !ok )
			return FALSE;
		}

		QDomElement pen = f.namedItem( "pen" ).toElement();
		if ( !pen.isNull() )
		    cell.pen = pen.toPen();

		QDomElement font = f.namedItem( "font" ).toElement();
		if ( !font.isNull() )
		    cell.font = font.toFont();

		QDomElement left = f.namedItem( "left-border" ).toElement();
		if ( !left.isNull() )
	        {
		    QDomElement pen = left.namedItem( "pen" ).toElement();
		    if ( !pen.isNull() )
			cell.leftPen = pen.toPen();
		}

		QDomElement top = f.namedItem( "top-border" ).toElement();
		if ( !top.isNull() )
	        {
		    QDomElement pen = top.namedItem( "pen" ).toElement();
		    if ( !pen.isNull() )
			cell.topPen = pen.toPen();
		}
	    }
	
	    m_cells[ (row-1)*4 + (column-1) ] = cell;
	}
    }

    return TRUE;
}
