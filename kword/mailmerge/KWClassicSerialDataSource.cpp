/* This file is part of the KDE project
   Original file (mailmerge.cc): Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2001 Joseph Wenninger <jowenn@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KWClassicSerialDataSource.h"
#include "KWClassicSerialDataSource.moc"
#include "KWMailMergeDataBase.h"
#include <kinputdialog.h>
#include <QLabel>

#include <QLayout>

#include <QPushButton>
#include <QLineEdit>
#include <qvalidator.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <Q3VBoxLayout>
#include <klocale.h>
#include <kdebug.h>
#include <qtoolbutton.h>
#include <QToolTip>
#include <kiconloader.h>
#include <q3header.h>
#include <kvbox.h>
#include "KWDocument.h"
#include "defs.h"

#define KWSLCPBarIcon( x ) BarIcon( x, db->KWInstance() )

/******************************************************************
 *
 * Class: KWClassicSerialDataSource
 *
 ******************************************************************/

KWClassicSerialDataSource::KWClassicSerialDataSource(KInstance *inst,QObject *parent)
	: KWMailMergeDataSource(inst,parent)
//    : doc( doc_ )
{
}

KWClassicSerialDataSource::~KWClassicSerialDataSource()
{
}

QString KWClassicSerialDataSource::getValue( const QString &name, int record ) const
{
    int num = record;
/*    if ( num == -1 )
        num = doc->getMailMergeRecord(); Is this really needed ?*/

    if ( num < 0 || num > (int)db.count() )
        return name;

    return db[ num ][ name ];
}

void KWClassicSerialDataSource::setValue( const QString &name, const QString &value, int record )
{
    int num = record;
/*    if ( num == -1 )
        num = doc->getMailMergeRecord(); Is this really needed?*/

    if ( num < 0 || num > (int)db.count() )
        return;

    db[ num ][ name ] = value;
}

void KWClassicSerialDataSource::appendRecord()
{
    DbRecord record( sampleRecord );
    db.append( record );
}

void KWClassicSerialDataSource::addEntry( const QString &name )
{
    sampleRecord[ name ] = i18n( "No Value" );
    Db::Iterator it = db.begin();
    for ( ; it != db.end(); ++it )
        ( *it )[ name ] = sampleRecord[ name ];
}

void KWClassicSerialDataSource::removeEntry( const QString &name )
{
    sampleRecord.remove( name );
    Db::Iterator it = db.begin();
    for ( ; it != db.end(); ++it )
        ( *it ).remove( name );
}

void KWClassicSerialDataSource::removeRecord( int i )
{
    if ( (i < 0) || (i > (int)db.count() - 1) )
        return;

	kDebug()<<QString("Removing record %1").arg(i)<<endl;

    Db::Iterator it = db.at( i);
    db.remove( it );
}

void KWClassicSerialDataSource::save( QDomDocument &doc, QDomElement &parent)
{
	QDomElement def=doc.createElement(QString::fromLatin1("DEFINITION"));
	parent.appendChild(def);
	for (DbRecord::Iterator it=sampleRecord.begin();it!=sampleRecord.end();++it)
	{
		QDomElement defEnt=doc.createElement(QString::fromLatin1("FIELD"));
		defEnt.setAttribute(QString::fromLatin1("name"),it.key());
		def.appendChild(defEnt);
	}
	QDomElement cont=doc.createElement(QString::fromLatin1("CONTENT"));
	parent.appendChild(cont);
	for (Db::Iterator dbI=db.begin();dbI!=db.end();++dbI)
	{
		QDomElement rec=doc.createElement(QString::fromLatin1("RECORD"));
		cont.appendChild(rec);
		for (DbRecord::Iterator it=sampleRecord.begin();it!=sampleRecord.end();++it)
		{
			QDomElement recEnt=doc.createElement(QString::fromLatin1("ITEM"));
			recEnt.setAttribute(QString::fromLatin1("name"),it.key());
			recEnt.setAttribute(QString::fromLatin1("data"),(*dbI)[it.key()]);
			rec.appendChild(recEnt);
		}
	}
}

void KWClassicSerialDataSource::load( QDomElement& parentElem )
{
	db.clear();
	sampleRecord.clear();
	QDomNode defNd=parentElem.namedItem("DEFINITION");
	if (defNd.isNull()) return;
	QDomElement def=defNd.toElement();
	for (QDomElement defEnt=def.firstChild().toElement();!defEnt.isNull();defEnt=defEnt.nextSibling().toElement())
	{
		sampleRecord[defEnt.attribute(QString::fromLatin1("name"))]=i18n( "No Value" );
	}
	QDomNode contNd=parentElem.namedItem("CONTENT");
	if (contNd.isNull()) return;
	for (QDomNode rec=contNd.firstChild();!rec.isNull();rec=rec.nextSibling())
	{
		appendRecord();
		for (QDomElement recEnt=rec.firstChild().toElement();!recEnt.isNull();recEnt=recEnt.nextSibling().toElement())
		{
			setValue(recEnt.attribute(QString::fromLatin1("name")),
				recEnt.attribute(QString::fromLatin1("data")),db.count()-1);
		}
	}
}

bool KWClassicSerialDataSource::showConfigDialog(QWidget *par,int action)
{
   if (action==KWSLCreate)
   {
   	db.clear();
   	sampleRecord.clear();
   }
   KWClassicMailMergeEditor *dia=new KWClassicMailMergeEditor( par, this );
   bool ret=(dia->exec()==QDialog::Accepted);
   delete dia;
   return ret;
}


/******************************************************************
 *
 * Class: KWClassicMailMergeEditorListItem
 *
 ******************************************************************/

KWClassicMailMergeEditorListItem::KWClassicMailMergeEditorListItem( Q3ListView *parent )
    : Q3ListViewItem( parent )
{
    editWidget = new QLineEdit( listView()->viewport() );
    listView()->addChild( editWidget );
}

KWClassicMailMergeEditorListItem::KWClassicMailMergeEditorListItem( Q3ListView *parent, Q3ListViewItem *after )
    : Q3ListViewItem( parent, after )
{
    editWidget = new QLineEdit( listView()->viewport() );
    listView()->addChild( editWidget );
}

KWClassicMailMergeEditorListItem::~KWClassicMailMergeEditorListItem()
{
    delete editWidget;
}

void KWClassicMailMergeEditorListItem::setText( int i, const QString &text )
{
    Q3ListViewItem::setText( i, text );
    if ( i == 1 )
        editWidget->setText( text );
}

QString KWClassicMailMergeEditorListItem::text( int i ) const
{
    if ( i == 1 )
        return editWidget->text();
    return Q3ListViewItem::text( i );
}

void KWClassicMailMergeEditorListItem::setup()
{
    setHeight( qMax( listView()->fontMetrics().height(),
                     editWidget->sizeHint().height() ) );
    if ( listView()->columnWidth( 1 ) < editWidget->sizeHint().width() )
        listView()->setColumnWidth( 1, editWidget->sizeHint().width() );
}

void KWClassicMailMergeEditorListItem::update()
{
    editWidget->resize( listView()->header()->cellSize( 1 ), height() );
    listView()->moveChild( editWidget, listView()->header()->cellPos( 1 ),
                           listView()->itemPos( this ) + listView()->contentsY() );
    editWidget->show();
}

/******************************************************************
 *
 * Class: KWClassicMailMergeEditorList
 *
 ******************************************************************/

KWClassicMailMergeEditorList::KWClassicMailMergeEditorList( QWidget *parent, KWClassicSerialDataSource *db_ )
    : Q3ListView( parent ), db( db_ )
{
    setSorting( -1 );
    addColumn( i18n( "Name" ) );
    addColumn( i18n( "Value" ) );
    header()->setMovingEnabled( FALSE );
    connect( header(), SIGNAL( sizeChange( int, int, int ) ),
             this, SLOT( columnSizeChange( int, int, int ) ) );
//     connect( header(), SIGNAL( sectionClicked( int ) ),
//           this, SLOT( sectionClicked( int ) ) );
    disconnect( header(), SIGNAL( sectionClicked( int ) ),
                this, SLOT( changeSortColumn( int ) ) );

    currentRecord = -1;
}

void KWClassicMailMergeEditorList::invalidateCurrentRecord()
{
	currentRecord=-1;
}

KWClassicMailMergeEditorList::~KWClassicMailMergeEditorList()
{
    if ( currentRecord == -1 )
        return;

    Q3ListViewItemIterator lit( this );
    QMap< QString, QString >::ConstIterator it = db->getRecordEntries().begin();
    for ( ; it != db->getRecordEntries().end(); ++it ) {
        Q3ListViewItem *item = 0;
        item = lit.current();
        ++lit;
        if ( currentRecord != -1 && item )
            db->setValue( it.key(), item->text( 1 ), currentRecord );
    }
}

void KWClassicMailMergeEditorList::columnSizeChange( int c, int, int )
{
    if ( c == 0 || c == 1 )
        updateItems();
}

void KWClassicMailMergeEditorList::sectionClicked( int )
{
    updateItems();
}

void KWClassicMailMergeEditorList::updateItems()
{
    Q3ListViewItemIterator it( this );
    for ( ; it.current(); ++it )
        ( (KWClassicMailMergeEditorListItem*)it.current() )->update();
}

void KWClassicMailMergeEditorList::displayRecord( int i )
{
    if ( i < 0 || i >= db->getNumRecords() )
        return;

    bool create = !firstChild();
    Q3ListViewItemIterator lit( this );
    QMap< QString, QString >::ConstIterator it = db->getRecordEntries().begin();
    Q3ListViewItem *after = 0;
    for ( ; it != db->getRecordEntries().end(); ++it ) {
        Q3ListViewItem *item = 0;
        if ( create ) {
            item = new KWClassicMailMergeEditorListItem( this, after );
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
 * Class: KWClassicMailMergeEditor
 *
 ******************************************************************/

KWClassicMailMergeEditor::KWClassicMailMergeEditor( QWidget *parent, KWClassicSerialDataSource *db_ )
    : KDialogBase( Plain, i18n( "Mail Merge - Editor" ), Ok | Cancel, Ok, parent, "", true ), db( db_ )
{
    back = plainPage();

    Q3VBoxLayout *l = new Q3VBoxLayout( back );
    l->setAutoAdd(true);
    l->setSpacing( KDialog::spacingHint() );

    KHBox *toolbar = new KHBox( back );

    first = new QToolButton( toolbar );
    first->setIconSet( SmallIconSet( "start" ) );
    first->setFixedSize( first->sizeHint() );
    connect(first, SIGNAL(clicked()), this, SLOT(firstRecord()));

    back_ = new QToolButton( toolbar );
    back_->setIconSet( SmallIconSet( "back" ) );
    back_->setFixedSize( back_->sizeHint() );
    connect(back_, SIGNAL(clicked()), this, SLOT(prevRecord()));

    records = new QSpinBox( 1, db->getNumRecords(), 1, toolbar );
    records->setMaximumHeight( records->sizeHint().height() );
    connect( records, SIGNAL( valueChanged( int ) ),
             this, SLOT( changeRecord( int ) ) );

    forward = new QToolButton( toolbar );
    forward->setIconSet( SmallIconSet( "forward" ) );
    forward->setFixedSize( forward->sizeHint() );
    connect(forward, SIGNAL(clicked()), this, SLOT(nextRecord()));

    finish = new QToolButton( toolbar );
    finish->setIconSet( SmallIconSet( "finish" ) );
    finish->setFixedSize( finish->sizeHint() );
    connect(finish, SIGNAL(clicked()), this, SLOT(lastRecord()));

    QWidget *sep = new QWidget( toolbar );
    sep->setMaximumWidth( 10 );

    newRecord = new QToolButton( toolbar );
    newRecord->setIconSet( SmallIconSet( "sl_addrecord" ) );
    newRecord->setFixedSize( newRecord->sizeHint() );
    connect( newRecord, SIGNAL( clicked() ),
             this, SLOT( addRecord() ) );
    newRecord->setToolTip( i18n( "Add record" ) );

    newEntry = new QToolButton( toolbar );
    newEntry->setIconSet( SmallIconSet( "sl_addentry" ) );
    newEntry->setFixedSize( newEntry->sizeHint() );
    connect( newEntry, SIGNAL( clicked() ),
             this, SLOT( addEntry() ) );
    newEntry->setToolTip( i18n( "Add entry" ) );

    deleteRecord = new QToolButton( toolbar );
    deleteRecord->setIconSet( SmallIconSet( "sl_delrecord" ) );
    deleteRecord->setFixedSize( deleteRecord->sizeHint() );
    connect( deleteRecord, SIGNAL( clicked() ),
             this, SLOT( removeRecord() ) );
    deleteRecord->setToolTip( i18n( "Remove record" ) );

    deleteEntry = new QToolButton( toolbar );
    deleteEntry->setIconSet( SmallIconSet( "sl_delentry" ) );
    deleteEntry->setFixedSize( deleteEntry->sizeHint() );
    connect( deleteEntry, SIGNAL( clicked() ),
             this, SLOT( removeEntry() ) );
    deleteEntry->setToolTip( i18n( "Remove entry" ) );

    dbList = new KWClassicMailMergeEditorList( back, db );

    if ( db->getNumRecords() > 0 ) {
        records->setValue( 1 );
	dbList->displayRecord(0);
//        dbList->updateItems();
    } else {
        first->setEnabled(false);
        back_->setEnabled(false);
        forward->setEnabled(false);
        finish->setEnabled(false);
        newRecord->setEnabled(false);
        deleteEntry->setEnabled(false);
        deleteRecord->setEnabled(false);
        records->setEnabled(true);
    }
    setInitialSize( QSize( 600, 400 ) );
    updateButton();
}

void KWClassicMailMergeEditor::firstRecord()
{
    records->setValue(1);
    updateButton();
}

void KWClassicMailMergeEditor::prevRecord()
{
    records->setValue(records->value()-1);
    updateButton();
}

void KWClassicMailMergeEditor::nextRecord()
{
    records->setValue(records->value()+1);
    updateButton();
}

void KWClassicMailMergeEditor::lastRecord()
{
    records->setValue(records->maxValue());
    updateButton();
}


void KWClassicMailMergeEditor::updateButton()
{
    int val = records->value();
    first->setEnabled( val > 1);
    back_->setEnabled( val> 1 );
    forward->setEnabled( val < records->maxValue() );
    finish->setEnabled( val < records->maxValue());

}

void KWClassicMailMergeEditor::resizeEvent( QResizeEvent *e )
{
    KDialogBase::resizeEvent( e );
//    back->resize( size() );
}

void KWClassicMailMergeEditor::changeRecord( int i )
{
    dbList->displayRecord( i - 1 );
}

void KWClassicMailMergeEditor::addEntry()
{
    bool ok;
    QString value=KInputDialog::getText( i18n("Add Entry"),
					 i18n("Enter entry name:"),
					 QString::null,
					 &ok,
					 this);
    if ((ok) && !value.isEmpty())
    {
        if ( db->getNumRecords() == 0 )
        {
            first->setEnabled(true);
            back_->setEnabled(true);
            forward->setEnabled(true);
            finish->setEnabled(true);
            newRecord->setEnabled(true);
            deleteEntry->setEnabled(true);
            deleteRecord->setEnabled(true);
            records->setEnabled(true);
            addRecord();
        }
        dbList->clear();
        db->addEntry( value );
        changeRecord( records->value() );
        dbList->updateItems();
    }
    updateButton();
}

void KWClassicMailMergeEditor::addRecord()
{
    db->appendRecord();
    records->setRange( records->minValue(), records->maxValue() + 1 );
    records->setValue( db->getNumRecords() );
    updateButton();

}

void KWClassicMailMergeEditor::removeEntry()
{
    Q3ListViewItem * item = dbList->selectedItem ();
    if ( item )
    {
        db->removeEntry( item->text(0) );
        dbList->clear();
        changeRecord( records->value() );
        dbList->updateItems();
        updateButton();
    }
#if defined( Q_CC_GNU )
#warning reimplement
#endif
/*
    if ( db->getNumRecords() == 0 )
        return;

    KWMailMergeVariableInsertDia
        *dia = new KWMailMergeVariableInsertDia( this, db );
    if ( dia->exec() == QDialog::Accepted ) {
        dbList->clear();
        db->removeEntry( dia->getName() );
        changeRecord( records->value() + 1 );
        dbList->updateItems();
    }
    delete dia;
*/
}

void KWClassicMailMergeEditor::removeRecord()
{
    if ( db->getNumRecords() == 0 )
        return;

    db->removeRecord( records->value() - 1 );
    dbList->invalidateCurrentRecord();
    if ( db->getNumRecords() > 0 ) {
        records->setRange( records->minValue(), records->maxValue() - 1 );
        records->setValue( 1 );
        dbList->clear();
        dbList->updateItems();
    } else
    {
        dbList->clear();
        records->setEnabled( FALSE );
    }

    if ( db->getNumRecords() == 0 )
    {
        newRecord->setEnabled(false);
        deleteEntry->setEnabled(false);
        deleteRecord->setEnabled(false);
        records->setEnabled(true);
    }

    updateButton();
}

extern "C" {
	KWORD_MAILMERGE_EXPORT KWMailMergeDataSource *create_kwmailmerge_classic(KInstance *inst,QObject *parent)
	{
		return new KWClassicSerialDataSource(inst,parent);
	}

}
