/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Serial Letters                                         */
/******************************************************************/

#include <qlistbox.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qheader.h>
#include <qtoolbutton.h>
#include <qspinbox.h>
#include <qpopupmenu.h>

#include <klocale.h>
#include <kbuttonbox.h>
#include <kiconloader.h>

#include "kword_doc.h"
#include "serialletter.h"
#include "serialletter.moc"

/******************************************************************
 *
 * Class: KWSerialLetterDataBase
 *
 ******************************************************************/

/*================================================================*/
KWSerialLetterDataBase::KWSerialLetterDataBase( KWordDocument *doc_ )
    : doc( doc_ )
{
}

/*================================================================*/
QString KWSerialLetterDataBase::getValue( const QString &name, int record ) const
{
    int num = record;
    if ( num == -1 )
	num = doc->getSerialLetterRecord();

    if ( num == -1 || num > (int)db.count() )
	return name;

    return db[ num ][ name ];
}

/*================================================================*/
void KWSerialLetterDataBase::setValue( const QString &name, const QString &value, int record )
{
    int num = record;
    if ( num == -1 )
	num = doc->getSerialLetterRecord();

    if ( num < 0 || num > (int)db.count() )
	return;

    db[ num ][ name ] = value;
}

/*================================================================*/
void KWSerialLetterDataBase::appendRecord()
{
    DbRecord record( sampleRecord );
    db.append( record );
}

/*================================================================*/
void KWSerialLetterDataBase::addEntry( const QString &name )
{
    sampleRecord[ name ] = i18n( "No Value" );
    Db::Iterator it = db.begin();
    for ( ; it != db.end(); ++it )
	( *it )[ name ] = sampleRecord[ name ];
}

/*================================================================*/
void KWSerialLetterDataBase::removeEntry( const QString &name )
{
    sampleRecord.remove( name );
    Db::Iterator it = db.begin();
    for ( ; it != db.end(); ++it )
	( *it ).remove( name );
}

/******************************************************************
 *
 * Class: KWSerialLetterVariableInsertDia
 *
 ******************************************************************/

/*================================================================*/
KWSerialLetterVariableInsertDia::KWSerialLetterVariableInsertDia( QWidget *parent, KWSerialLetterDataBase *db )
    : QDialog( parent, "", TRUE )
{
    setCaption( i18n( "Serial Letter - Variable Name" ) );

    back = new QVBox( this );
    back->setSpacing( 5 );
    back->setMargin( 5 );

    QVBox *row1 = new QVBox( back );
    row1->setSpacing( 5 );

    QLabel *l = new QLabel( i18n( "Name:" ), row1 );
    l->setMaximumHeight( l->sizeHint().height() );
    names = new QListBox( row1 );

    QMap< QString, QString >::ConstIterator it = db->getRecordEntries().begin();
    for ( ; it != db->getRecordEntries().end(); ++it )
	names->insertItem( it.key(), -1 );

    KButtonBox *bb = new KButtonBox( back );
    bb->addStretch();
    QPushButton *ok = bb->addButton( i18n( "&OK"  ) );
    ok->setDefault( TRUE );
    if ( names->count() == 0 )
	ok->setEnabled( FALSE );
    QPushButton *cancel = bb->addButton( i18n( "&Cancel"  ) );
    bb->layout();

    connect( ok, SIGNAL( clicked() ),
	     this, SLOT( accept() ) );
    connect( cancel, SIGNAL( clicked() ),
	     this, SLOT( reject() ) );

    resize( 350, 400 );
}

/*================================================================*/
QString KWSerialLetterVariableInsertDia::getName() const
{
    return names->text( names->currentItem() );
}

/*================================================================*/
void KWSerialLetterVariableInsertDia::resizeEvent( QResizeEvent *e )
{
    QDialog::resizeEvent( e );
    back->resize( size() );
}

/******************************************************************
 *
 * Class: KWSerialLetterEditorListItem
 *
 ******************************************************************/

/*================================================================*/
KWSerialLetterEditorListItem::KWSerialLetterEditorListItem( QListView *parent )
    : QListViewItem( parent )
{
    editWidget = new QLineEdit( listView()->viewport() );
    listView()->addChild( editWidget );
}

/*================================================================*/
KWSerialLetterEditorListItem::KWSerialLetterEditorListItem( QListView *parent, QListViewItem *after )
    : QListViewItem( parent, after )
{
    editWidget = new QLineEdit( listView()->viewport() );
    listView()->addChild( editWidget );
}

/*================================================================*/
KWSerialLetterEditorListItem::~KWSerialLetterEditorListItem()
{
    delete editWidget;
}

/*================================================================*/
void KWSerialLetterEditorListItem::setText( int i, const QString &text )
{
    QListViewItem::setText( i, text );
    if ( i == 1 )
	editWidget->setText( text );
}

/*================================================================*/
QString KWSerialLetterEditorListItem::text( int i ) const
{
    if ( i == 1 )
	return editWidget->text();
    return QListViewItem::text( i );
}

/*================================================================*/
void KWSerialLetterEditorListItem::setup()
{
    setHeight( QMAX( listView()->fontMetrics().height(),
		     editWidget->sizeHint().height() ) );
    if ( listView()->columnWidth( 1 ) < editWidget->sizeHint().width() )
	listView()->setColumnWidth( 1, editWidget->sizeHint().width() );
}

/*================================================================*/
void KWSerialLetterEditorListItem::update()
{
    editWidget->resize( listView()->header()->cellSize( 1 ), height() );
    listView()->moveChild( editWidget, listView()->header()->cellPos( 1 ),
			   listView()->itemPos( this ) + listView()->contentsY() );
    editWidget->show();
}

/******************************************************************
 *
 * Class: KWSerialLetterEditorList
 *
 ******************************************************************/

/*================================================================*/
KWSerialLetterEditorList::KWSerialLetterEditorList( QWidget *parent, KWSerialLetterDataBase *db_ )
    : QListView( parent ), db( db_ )
{
    setSorting( -1 );
    addColumn( i18n( "Name" ) );
    addColumn( i18n( "Value" ) );
    header()->setMovingEnabled( FALSE );
    connect( header(), SIGNAL( sizeChange( int, int, int ) ),
	     this, SLOT( columnSizeChange( int, int, int ) ) );
//     connect( header(), SIGNAL( sectionClicked( int ) ),
// 	     this, SLOT( sectionClicked( int ) ) );
    disconnect( header(), SIGNAL( sectionClicked( int ) ),
		this, SLOT( changeSortColumn( int ) ) );

    currentRecord = -1;
}

/*================================================================*/
KWSerialLetterEditorList::~KWSerialLetterEditorList()
{
    if ( currentRecord == -1 )
	return;
    
    QListViewItemIterator lit( this );
    QMap< QString, QString >::ConstIterator it = db->getRecordEntries().begin();
    for ( ; it != db->getRecordEntries().end(); ++it ) {
	QListViewItem *item = 0;
	item = lit.current();
	++lit;
	if ( currentRecord != -1 && item )
	    db->setValue( it.key(), item->text( 1 ), currentRecord );
    }	
}   

/*================================================================*/
void KWSerialLetterEditorList::columnSizeChange( int c, int, int )
{
    if ( c == 0 || c == 1 )
	updateItems();
}

/*================================================================*/
void KWSerialLetterEditorList::sectionClicked( int )
{
    updateItems();
}

/*================================================================*/
void KWSerialLetterEditorList::updateItems()
{
    QListViewItemIterator it( this );
    for ( ; it.current(); ++it )
	( (KWSerialLetterEditorListItem*)it.current() )->update();
}

/*================================================================*/
void KWSerialLetterEditorList::displayRecord( int i )
{
    if ( i < 0 || i >= db->getNumRecords() )
	return;
    
    bool create = !firstChild();
    QListViewItemIterator lit( this );
    QMap< QString, QString >::ConstIterator it = db->getRecordEntries().begin();
    QListViewItem *after = 0;
    for ( ; it != db->getRecordEntries().end(); ++it ) {
	QListViewItem *item = 0;
	if ( create ) {
	    item = new KWSerialLetterEditorListItem( this, after );
	    item->setText( 0, it.key() );
	    after = item;
	} else {
	    item = lit.current();
	    ++lit;
	    if ( currentRecord != -1 && item )
		db->setValue( it.key(), item->text( 1 ), currentRecord );
	}
	
	if ( item )
	    item->setText( 1, db->getValue( it.key(), i ) );
    }    
    updateItems();
    currentRecord = i;
}

/******************************************************************
 *
 * Class: KWSerialLetterEditor
 *
 ******************************************************************/

/*================================================================*/
KWSerialLetterEditor::KWSerialLetterEditor( QWidget *parent, KWSerialLetterDataBase *db_ )
    : QDialog( parent, "", TRUE ), db( db_ )
{
    setCaption( i18n( "Serial Letter - Editor" ) );

    back = new QVBox( this );
    back->setSpacing( 5 );
    back->setMargin( 5 );

    QHBox *toolbar = new QHBox( back );
    
    QToolButton *first = new QToolButton( toolbar );
    first->setPixmap( BarIcon( "start" ) );
    first->setFixedSize( first->sizeHint() );
    
    QToolButton *back_ = new QToolButton( toolbar );
    back_->setPixmap( BarIcon( "back" ) );
    back_->setFixedSize( back_->sizeHint() );
    
    records = new QSpinBox( 1, db->getNumRecords(), 1, toolbar );
    records->setMaximumHeight( records->sizeHint().height() );
    connect( records, SIGNAL( valueChanged( int ) ),
	     this, SLOT( changeRecord( int ) ) );
    
    QToolButton *forward = new QToolButton( toolbar );
    forward->setPixmap( BarIcon( "forward" ) );
    forward->setFixedSize( forward->sizeHint() );
    
    QToolButton *finish = new QToolButton( toolbar );
    finish->setPixmap( BarIcon( "finish" ) );
    finish->setFixedSize( finish->sizeHint() );

    QWidget *sep = new QWidget( toolbar );
    sep->setMaximumWidth( 10 );
    
    QToolButton *newRecord = new QToolButton( toolbar );
    newRecord->setPixmap( BarIcon( "filenew" ) );
    newRecord->setFixedSize( newRecord->sizeHint() );
    connect( newRecord, SIGNAL( clicked() ),
	     this, SLOT( addRecord() ) );
    
    QToolButton *newEntry = new QToolButton( toolbar );
    newEntry->setPixmap( BarIcon( "filenew" ) );
    newEntry->setFixedSize( newEntry->sizeHint() );
    connect( newEntry, SIGNAL( clicked() ),
	     this, SLOT( addEntry() ) );

    QToolButton *deleteRecord = new QToolButton( toolbar );
    deleteRecord->setPixmap( BarIcon( "delete" ) );
    deleteRecord->setFixedSize( deleteRecord->sizeHint() );
    connect( deleteRecord, SIGNAL( clicked() ),
	     this, SLOT( removeRecord() ) );
    
    QToolButton *deleteEntry = new QToolButton( toolbar );
    deleteEntry->setPixmap( BarIcon( "delete" ) );
    deleteEntry->setFixedSize( deleteEntry->sizeHint() );
    connect( deleteEntry, SIGNAL( clicked() ),
	     this, SLOT( removeEntry() ) );

    dbList = new KWSerialLetterEditorList( back, db );

    if ( db->getNumRecords() > 0 ) {
	changeRecord( 1 );
	dbList->updateItems();
    } else
	records->setEnabled( FALSE );
    
    resize( 600, 400 );
}

/*================================================================*/
void KWSerialLetterEditor::resizeEvent( QResizeEvent *e )
{
    QDialog::resizeEvent( e );
    back->resize( size() );
}

/*================================================================*/
void KWSerialLetterEditor::changeRecord( int i )
{
    dbList->displayRecord( i - 1 );
}

/*================================================================*/
void KWSerialLetterEditor::addEntry()
{
}

/*================================================================*/
void KWSerialLetterEditor::addRecord()
{
    db->appendRecord();
    records->setRange( records->minValue(), records->maxValue() + 1 );
    records->setValue( db->getNumRecords() );
}

/*================================================================*/
void KWSerialLetterEditor::removeEntry()
{
}

/*================================================================*/
void KWSerialLetterEditor::removeRecord()
{
}
