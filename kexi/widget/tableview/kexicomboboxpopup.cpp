/* This file is part of the KDE project
   Copyright (C) 2004-2006 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "kexicomboboxpopup.h"

#include "kexidatatableview.h"
#include "kexitableview_p.h"
#include "kexitableitem.h"
#include "kexitableedit.h"

#include <kexidb/lookupfieldschema.h>

#include <kdebug.h>

#include <qlayout.h>
#include <qevent.h>

/*! @internal
 Helper for KexiComboBoxPopup. */
class KexiComboBoxPopup_KexiTableView : public KexiDataTableView
{
	public:
		KexiComboBoxPopup_KexiTableView(QWidget* parent=0)
//		 : KexiTableView(0, parent, "KexiComboBoxPopup_tv")
		 : KexiDataTableView(parent, "KexiComboBoxPopup_tv")
		{
			init();
		}
		void init()
		{
			setReadOnly( true );
			setLineWidth( 0 );
			d->moveCursorOnMouseRelease = true;
		//	setBackgroundAltering( false ); 
			KexiTableView::Appearance a(appearance());
			a.navigatorEnabled = false;
			a.backgroundAltering = false; //TODO add option??
			a.fullRowSelection = true;
			a.rowHighlightingEnabled = true;
			a.rowMouseOverHighlightingEnabled = true;
			a.persistentSelections = false;
			a.rowMouseOverHighlightingColor = colorGroup().highlight();
			a.rowMouseOverHighlightingTextColor = colorGroup().highlightedText();
			a.rowHighlightingTextColor = a.rowMouseOverHighlightingTextColor;
			a.gridEnabled = false;
			setAppearance(a);
			setInsertingEnabled( false );
			setSortingEnabled( false );
			setVerticalHeaderVisible( false );
			setHorizontalHeaderVisible( false );
//js later setColumnStretchEnabled( true, -1 );
			setContextMenuEnabled( false );
			setScrollbarToolTipsEnabled( false );
			installEventFilter(this);
			setBottomMarginInternal( - horizontalScrollBar()->sizeHint().height() );
		}
		virtual void setData( KexiTableViewData *data, bool owner = true )
			{ KexiTableView::setData( data, owner ); }
		bool setData(KexiDB::Cursor *cursor)
	{ return KexiDataTableView::setData( cursor ); }
};

//========================================

//! @internal
class KexiComboBoxPopupPrivate
{
	public:
		KexiComboBoxPopupPrivate() 
		 : int_f(0)
		{
			max_rows = KexiComboBoxPopup::defaultMaxRows;
		}
		~KexiComboBoxPopupPrivate() {
			delete int_f;
		}
		
		KexiComboBoxPopup_KexiTableView *tv;
		KexiDB::Field *int_f; //TODO: remove this -temporary
		int max_rows;
};

//========================================

const int KexiComboBoxPopup::defaultMaxRows = 8;


/*unused 
KexiComboBoxPopup::KexiComboBoxPopup(QWidget* parent, KexiDB::Field &f)
 : QFrame( parent, "KexiComboBoxPopup", WType_Popup )
{
	init();
	//setup tv data
	setData(f);
}*/

KexiComboBoxPopup::KexiComboBoxPopup(QWidget* parent, KexiTableViewColumn &column)
 : QFrame( parent, "KexiComboBoxPopup", WType_Popup )
{
	init();
	//setup tv data
	setData(&column, 0);
}

KexiComboBoxPopup::KexiComboBoxPopup(QWidget* parent, KexiDB::Field &field)
 : QFrame( parent, "KexiComboBoxPopup", WType_Popup )
{
	init();
	//setup tv data
	setData(0, &field);
}

KexiComboBoxPopup::~KexiComboBoxPopup()
{
	delete d;
}

void KexiComboBoxPopup::init()
{
	d = new KexiComboBoxPopupPrivate();
	setPaletteBackgroundColor(palette().color(QPalette::Active,QColorGroup::Base));
	setLineWidth( 1 );
	setFrameStyle( Box | Plain );
	
//	QVBoxLayout *lyr = new QVBoxLayout(this, 1);
	d->tv = new KexiComboBoxPopup_KexiTableView(this);
	installEventFilter(this);
	
	connect(d->tv, SIGNAL(itemReturnPressed(KexiTableItem*,int,int)),
		this, SLOT(slotTVItemAccepted(KexiTableItem*,int,int)));

	connect(d->tv, SIGNAL(itemMouseReleased(KexiTableItem*,int,int)),
		this, SLOT(slotTVItemAccepted(KexiTableItem*,int,int)));

	connect(d->tv, SIGNAL(itemDblClicked(KexiTableItem*,int,int)),
		this, SLOT(slotTVItemAccepted(KexiTableItem*,int,int)));

//	lyr->addWidget(d->tv);
}

void KexiComboBoxPopup::setData(KexiTableViewColumn *column, KexiDB::Field *field)
{
	if (column && !field)
		field = column->field();
	if (!field) {
		kexiwarn << "KexiComboBoxPopup::setData(): !field" << endl;
		return;
	}

	// case 1: simple related data
	if (column && column->relatedData()) {
		d->tv->setColumnStretchEnabled( true, -1 ); //only needed when using single column
		setDataInternal( column->relatedData(), false /*!owner*/ );
		return;
	}

	// case 2: lookup field
	KexiDB::LookupFieldSchema *lookupFieldSchema = 0;
	if (field->table())
		lookupFieldSchema = field->table()->lookupFieldSchema( *field );
	if (lookupFieldSchema) {
//! @todo support more RowSourceType's, not only table
		KexiDB::TableSchema *lookupTable 
			= field->table()->connection()->tableSchema( lookupFieldSchema->rowSource() );
		if (!lookupTable)
//! @todo errmsg
			return;
		KexiDB::Cursor *cursor = field->table()->connection()->prepareQuery( *lookupTable );
		if (!cursor)
//! @todo errmsg
			return;

		if (d->tv->data())
			d->tv->data()->disconnect( this );
		d->tv->setData( cursor );

		connect( d->tv, SIGNAL(dataRefreshed()), this, SLOT(slotDataReloadRequested()));
		updateSize();
		return;
	}
	
	kdWarning() << "KexiComboBoxPopup::setData(KexiTableViewColumn &): no column relatedData \n - moving to setData(KexiDB::Field &)" << endl;

	// case 3: enum hints
	d->tv->setColumnStretchEnabled( true, -1 ); //only needed when using single column

//j: TODO: THIS IS PRIMITIVE: we'd need to employ KexiDB::Reference here!
	d->int_f = new KexiDB::Field(field->name(), KexiDB::Field::Text);
	KexiTableViewData *data = new KexiTableViewData();
//	data->setReadOnly( true );
	data->addColumn( new KexiTableViewColumn( *d->int_f ) );
	QValueVector<QString> hints = field->enumHints();
	for(uint i=0; i < hints.size(); i++) {
		KexiTableItem *item = data->createItem();//new KexiTableItem(1);
		(*item)[0]=QVariant(hints[i]);
		kdDebug() << "added: '" << hints[i] <<"'"<<endl;
		data->append( item );
	}
	setDataInternal( data, true );
}

void KexiComboBoxPopup::setDataInternal( KexiTableViewData *data, bool owner )
{
	if (d->tv->data())
		d->tv->data()->disconnect( this );
	d->tv->setData( data, owner );
	connect( d->tv, SIGNAL(dataRefreshed()), this, SLOT(slotDataReloadRequested()));
//	connect( data, SIGNAL(refreshRequested()), this, SLOT(slotDataRefreshRequested()));

	updateSize();
}

void KexiComboBoxPopup::updateSize(int minWidth)
{
	//jsd->tv->setColumnStretchEnabled( true, -1 );
//	d->tv->adjustColumnWidthToContents( -1 ); //TODO: not only for column 0, if there are more columns!
//	                                         //TODO: check if the width is not too big
//js	d->tv->adjustHorizontalHeaderSize();
//	d->tv->adjustColumnWidthToContents( 0 ); //TODO: not only for column 0, if there are more columns!
//	                                         //TODO: check if the width is not too big

	const int rows = QMIN( d->max_rows, d->tv->rows() );

//	resize(400, d->tv->rowHeight() * rows +2);
	d->tv->adjustColumnWidthToContents(-1);
//	resize(500, d->tv->rowHeight() * rows +2);


	KexiTableEdit *te = dynamic_cast<KexiTableEdit*>(parentWidget());
	const int width = QMAX( d->tv->tableSize().width(), 
		(te ? te->totalSize().width() : parentWidget()->width()/*sanity*/) );
	kexidbg << size() << endl;
	resize( QMAX(minWidth, width)+1, d->tv->rowHeight() * rows +2 );
}

KexiTableView* KexiComboBoxPopup::tableView()
{
	return d->tv;
}

void KexiComboBoxPopup::resize( int w, int h )
{
	d->tv->horizontalScrollBar()->hide();
	d->tv->verticalScrollBar()->hide();
//	hide();
	d->tv->move(1,1);
	d->tv->resize( w-2, h-2 );
	QFrame::resize(w,h);
	update();
	updateGeometry();
//	show();
}

void KexiComboBoxPopup::setMaxRows(int r)
{
	d->max_rows = r;
}

/*! \return maximum number of rows for this popup. */
int KexiComboBoxPopup::maxRows() const
{
	return d->max_rows;
}

void KexiComboBoxPopup::slotTVItemAccepted(KexiTableItem *item, int row, int)
{
	hide();
	emit rowAccepted(item, row);
}

bool KexiComboBoxPopup::eventFilter( QObject *o, QEvent *e )
{
	if (o==this && e->type()==QEvent::Hide) {
		emit hidden();
	}
	else if (e->type()==QEvent::MouseButtonPress) {
		kdDebug() << "QEvent::MousePress" << endl;
	}
	else if (o==d->tv) {
		if (e->type()==QEvent::KeyPress) {
			QKeyEvent *ke = static_cast<QKeyEvent*>(e);
			const int k = ke->key();
			if ((ke->state()==NoButton && (k==Key_Escape || k==Key_F4))
				|| (ke->state()==AltButton && k==Key_Up))
			{
				hide();
				emit cancelled();
				return true;
			}
		}
	}
	return QFrame::eventFilter( o, e );
}

void KexiComboBoxPopup::slotDataReloadRequested()
{
	updateSize();
}


#include "kexicomboboxpopup.moc"

