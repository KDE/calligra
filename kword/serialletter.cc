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

#include <klocale.h>
#include <kbuttonbox.h>

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

    if ( num == -1 )
	return name;

    if ( db.contains( name ) ) {
	QStringList lst = db[ name ];
	if ( num < (int)lst.count() )
	    return lst[ num ];
	return QString::null;
    }

    return QString::null;
}

/*================================================================*/
void KWSerialLetterDataBase::setValue( const QString &name, const QString &value, int record )
{
    int num = record;
    if ( num == -1 )
	num = doc->getSerialLetterRecord();

    if ( num == -1 )
	return;

    if ( db.contains( name ) ) {
	QStringList lst = db[ name ];
	if ( num < (int)lst.count() ) {
	    lst[ num ] = value;
	    qDebug( "KWSerialLetterDataBase::setValue worked!" );
	}
    }
}

/*================================================================*/
void KWSerialLetterDataBase::appendRecord()
{
    QMap< QString, QStringList >::Iterator it = db.begin();
    for ( ; it != db.end(); ++it )
	( *it ) << i18n( "No Value" );
}

/*================================================================*/
void KWSerialLetterDataBase::addEntry( const QString &name )
{
    if ( db.isEmpty() ) {
	QStringList lst;
	lst << i18n( "No Value" );
	db[ name ] = lst;
    } else {
	int records = ( *db.begin() ).count();
	QStringList lst;
	for ( int i = 0; i < records; ++i )
	    lst << i18n( "No Value" );
	db[ name ] = lst;
    }
}

/*================================================================*/
void KWSerialLetterDataBase::removeEntry( const QString &name )
{
    db.remove( name );
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

    QMap< QString, QStringList >::ConstIterator it = db->database().begin();
    for ( ; it != db->database().end(); ++it )
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
    return names->currentText();
}

/*================================================================*/
void KWSerialLetterVariableInsertDia::resizeEvent( QResizeEvent *e )
{
    QDialog::resizeEvent( e );
    back->resize( size() );
}
