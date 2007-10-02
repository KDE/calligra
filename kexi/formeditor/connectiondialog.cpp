/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#include "connectiondialog.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qregexp.h>
#include <qmetaobject.h>

#include <kpushbutton.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <klocale.h>

#ifdef __GNUC__
#warning "Port Kexi Table View!"
#endif
#ifndef KEXI_NO_TABLEVIEW
#include "kexitableview.h"
#include "kexitableviewdata.h"
#endif

#include "events.h"
#include "form.h"
#include "formmanager.h"
#include "objecttree.h"


using namespace KFormDesigner;

/////////////////////////////////////////////////////////////////////////////////
///////////// The dialog to edit or add/remove connections //////////////////////
/////////////////////////////////////////////////////////////////////////////////
ConnectionDialog::ConnectionDialog(QWidget *parent)
: KDialog(parent)
, m_buffer(0)
{
	setObjectName("connections_dialog");
	setModal(true);
	setCaption(i18n("Edit Form Connections"));
	setButtons( KDialog::Ok | KDialog::Cancel | KDialog::Details );
	setDefaultButton( KDialog::Ok );

	QFrame *frame = new QFrame(this);
	setMainWidget(frame);
	QHBoxLayout *layout = new QHBoxLayout(frame, 0, 6);

	// Setup the details widget /////////
	QWidget *details = new QWidget(frame);
	layout->addWidget(details);
	QHBoxLayout *detailsLyr = new QHBoxLayout(details);
	setDetailsWidget(details);
	setDetailsWidgetVisible(true);

	m_pixmapLabel = new QLabel(details);
	detailsLyr->addWidget(m_pixmapLabel);
	m_pixmapLabel->setFixedWidth( int(IconSize(KIconLoader::Desktop) * 1.5) );
	m_pixmapLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

	m_textLabel = new QLabel(details);
	detailsLyr->addWidget(m_textLabel);
	m_textLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	//setStatusOk();

#ifndef KEXI_NO_TABLEVIEW
#ifdef __GNUC__
#warning "Port Kexi Table View!"
#endif
	// And the KexiTableView ////////
	m_data = new KexiTableViewData();
	m_table = new KexiTableView(0, frame, "connections_tableview");
	m_table->setSpreadSheetMode();
	m_table->setInsertingEnabled(true);
	initTable();
	m_table->setData(m_data, false);
	m_table->adjustColumnWidthToContents(0);
	layout->addWidget(m_table);
	
	connect(m_table,SIGNAL(cellSelected(int, int)),
		this, SLOT(slotCellSelected(int, int)));
	connect(m_table->data(), SIGNAL(rowInserted(KexiDB::RecordData*,bool)),
		this, SLOT(slotRowInserted(KexiDB::RecordData*,bool)));
#endif

	//// Setup the icon toolbar /////////////////
	QVBoxLayout *vlayout = new QVBoxLayout(layout, 3);
	m_addButton = new KPushButton(KIcon("document-new"), i18n("&New Connection"), frame);
	vlayout->addWidget(m_addButton);
	connect(m_addButton, SIGNAL(clicked()), this, SLOT(newItem()));

	m_removeButton = new KPushButton(KIcon("edit-delete"), i18n("&Remove Connection"), frame);
	vlayout->addWidget(m_removeButton);
	connect(m_removeButton, SIGNAL(clicked()), this, SLOT(removeItem()));

	vlayout->addStretch();

	setInitialSize(QSize(600, 300));
	//setWFlags(WDestructiveClose);

	this->newItem();
}

void
ConnectionDialog::initTable()
{
#ifdef __GNUC__
#warning "Port Kexi Table View!"
#endif
#ifndef KEXI_NO_TABLEVIEW
	KexiTableViewColumn *col0 = new KexiTableViewColumn(i18n("OK?"), KexiDB::Field::Text);
	col0->field()->setSubType("KIcon");
	col0->setReadOnly(true);
	col0->field()->setDescription(i18n("Connection correctness"));
	m_data->addColumn(col0);

	KexiTableViewColumn *col1 = new KexiTableViewColumn(i18n("Sender"), KexiDB::Field::Enum);
	m_widgetsColumnData = new KexiTableViewData(KexiDB::Field::Text, KexiDB::Field::Text);
	col1->setRelatedData( m_widgetsColumnData );
	m_data->addColumn(col1);

	KexiTableViewColumn *col2 = new KexiTableViewColumn(i18n("Signal"), KexiDB::Field::Enum);
	m_signalsColumnData = new KexiTableViewData(KexiDB::Field::Text, KexiDB::Field::Text);
	col2->setRelatedData( m_signalsColumnData );
	m_data->addColumn(col2);

	KexiTableViewColumn *col3 = new KexiTableViewColumn(i18n("Receiver"), KexiDB::Field::Enum);
	col3->setRelatedData( m_widgetsColumnData );
	m_data->addColumn(col3);

	KexiTableViewColumn *col4 = new KexiTableViewColumn(i18n("Slot"), KexiDB::Field::Enum);
	m_slotsColumnData = new KexiTableViewData(KexiDB::Field::Text, KexiDB::Field::Text);
	col4->setRelatedData( m_slotsColumnData );
	m_data->addColumn(col4);

	QList<int> c;
	c << 2 << 4;
	m_table->maximizeColumnsWidth(c);
	m_table->setColumnStretchEnabled( true, 4 );

	connect(m_data, SIGNAL(aboutToChangeCell(KexiDB::RecordData*, int, QVariant&, KexiDB::ResultInfo*)),
	      this,SLOT(slotCellChanged(KexiDB::RecordData*, int, QVariant, KexiDB::ResultInfo*)));
	connect(m_data, SIGNAL(rowUpdated(KexiDB::RecordData*)), this, SLOT(checkConnection(KexiDB::RecordData*)));
	connect(m_table, SIGNAL(itemSelected(KexiDB::RecordData*)), this, SLOT(checkConnection(KexiDB::RecordData*)));
#endif
}

void
ConnectionDialog::exec(Form *form)
{
	m_form = form;
	updateTableData();
	KDialog::exec();
}

void ConnectionDialog::slotCellSelected(int col, int row)
{
#ifdef __GNUC__
#warning "Port Kexi Table View!"
#endif
#ifndef KEXI_NO_TABLEVIEW
	m_removeButton->setEnabled( row < m_table->rows() );
	KexiDB::RecordData *record = m_table->itemAt(row);
	if (!record)
		return;
	if(col == 2) // signal col
		updateSignalList(record);
	else if(col == 4) // slot col
		updateSlotList(record);
#endif
}

void ConnectionDialog::slotRowInserted(KexiDB::RecordData* item,bool)
{
	m_buffer->append(new Connection());
	checkConnection( item );
}

void
ConnectionDialog::slotOk()
{
#ifdef __GNUC__
#warning "Port Kexi Table View!"
#endif
#ifndef KEXI_NO_TABLEVIEW
	// First we update our buffer contents
	for(int i=0; i < m_table->rows(); i++)
	{
		KexiDB::RecordData *record = m_table->itemAt(i);
		Connection *c = m_buffer->at(i);

		c->setSender( (*record)[1].toString() );
		c->setSignal( (*record)[2].toString() );
		c->setReceiver( (*record)[3].toString() );
		c->setSlot( (*record)[4].toString() );
	}
#endif

	// then me make it replace form's current one
	delete m_form->connectionBuffer();
	m_form->setConnectionBuffer(m_buffer);

	QDialog::accept();
}

void
ConnectionDialog::updateTableData()
{
	// First we update the columns data
	ObjectTreeDict *dict = new ObjectTreeDict( *(m_form->objectTree()->dict()) );
	ObjectTreeDictIterator it(*dict);
	for(; it.current(); ++it)
	{
#ifdef __GNUC__
#warning "Port Kexi Table View!"
#endif
#ifndef KEXI_NO_TABLEVIEW
		KexiDB::RecordData *record = m_widgetsColumnData->createItem();
		(*record)[0] = it.current()->name();
		(*record)[1] = (*record)[0];
		m_widgetsColumnData->append(item);
#endif
	}
	delete dict;

	// Then we fill the columns with the form connections
	for(Connection *c = m_form->connectionBuffer()->first(); c ; c = m_form->connectionBuffer()->next())
	{
#ifdef __GNUC__
#warning "Port Kexi Table View!"
#endif
#ifndef KEXI_NO_TABLEVIEW
		KexiDB::RecordData *record = m_table->data()->createItem();
		(*record)[1] = c->sender();
		(*record)[2] = c->signal();
		(*record)[3] = c->receiver();
		(*record)[4] = c->slot();
		m_table->insertItem(record, m_table->rows());
#endif
	}

	m_buffer = new ConnectionBuffer(*(m_form->connectionBuffer()));
}

void
ConnectionDialog::setStatusOk(KexiDB::RecordData *record)
{
	m_pixmapLabel->setPixmap( DesktopIcon("dialog-ok") );
	m_textLabel->setText("<qt><h2>The connection is OK.</h2></qt>");

#ifdef __GNUC__
#warning "Port Kexi Table View!"
#endif
#ifndef KEXI_NO_TABLEVIEW
	if (!record)
		record = m_table->selectedItem();
	if (m_table->currentRow() >= m_table->rows())
		record = 0;

	if (record)
		(*record)[0] = "dialog-ok";
	else {
		m_pixmapLabel->setPixmap( QPixmap() );
		m_textLabel->setText(QString());
	}
#endif
}

void
ConnectionDialog::setStatusError(const QString &msg, KexiDB::RecordData *record)
{
	m_pixmapLabel->setPixmap( DesktopIcon("dialog-cancel") );
	m_textLabel->setText("<qt><h2>The connection is invalid.</h2></qt>" + msg);

#ifdef __GNUC__
#warning "Port Kexi Table View!"
#endif
#ifndef KEXI_NO_TABLEVIEW
	if (!record)
		record = m_table->selectedItem();
	if (m_table->currentRow() >= m_table->rows())
		record = 0;

	if (record)
		(*record)[0] = "dialog-cancel";
	else {
		m_pixmapLabel->setPixmap( QPixmap() );
		m_textLabel->setText(QString());
	}
#endif
}

void
ConnectionDialog::slotCellChanged(KexiDB::RecordData *record, int col, QVariant&, KexiDB::ResultInfo*)
{
#ifdef __GNUC__
#warning "Port Kexi Table View!"
#endif
#ifndef KEXI_NO_TABLEVIEW
	switch(col)
	{
		// sender changed, we clear siganl and slot
		case 1:
			(*record)[2] = QString("");
		// signal or receiver changed, we clear the slot cell
		case 2:
		case 3:
		{
			(*record)[4] = QString("");
			break;
		}
		default:
			break;
	}
#endif
}

void
ConnectionDialog::updateSlotList(KexiDB::RecordData *record)
{
#ifdef __GNUC__
#warning "Port Kexi Table View!"
#endif
#ifndef KEXI_NO_TABLEVIEW
	m_slotsColumnData->deleteAllRows();
	QString widget = (*record)[1].toString();
	QString signal = (*record)[2].toString();

	if((widget.isEmpty()) || signal.isEmpty())
		return;
	ObjectTreeItem *tree = m_form->objectTree()->lookup(widget);
	if(!tree || !tree->widget())
		return;

	QString signalArg(signal);
	signalArg = signalArg.remove( QRegExp(".*[(]|[)]") );

	const QList<QMetaMethod> list( 
		KexiUtils::methodsForMetaObjectWithParents(tree->widget()->metaObject(), 
		QMetaMethod::Slot, QMetaMethod::Public) );
	foreach (QMetaMethod method, list) {
		// we add the slot only if it is compatible with the signal
		QString slotArg(method.signature());
		slotArg = slotArg.remove( QRegExp(".*[(]|[)]") );
		if(!signalArg.startsWith(slotArg, true) && (!signal.isEmpty())) // args not compatible
			continue;

		KexiDB::RecordData *record = m_slotsColumnData->createItem();
		(*record)[0] = QString::fromLatin1(method.signature());
		(*record)[1] = (*record)[0];
		m_slotsColumnData->append(record);
	}
#endif
}

void
ConnectionDialog::updateSignalList(KexiDB::RecordData *record)
{
#ifdef __GNUC__
#warning "Port Kexi Table View!"
#endif
#ifndef KEXI_NO_TABLEVIEW
ObjectTreeItem *tree = m_form->objectTree()->lookup((*record)[1].toString());
	if(!tree || !tree->widget())
		return;

	m_signalsColumnData->deleteAllRows();
	const QList<QMetaMethod> list( 
		KexiUtils::methodsForMetaObjectWithParents(tree->widget()->metaObject(), 
		QMetaMethod::Signal, QMetaMethod::Public) );
	foreach (QMetaMethod method, list) {
		KexiDB::RecordData *record = m_signalsColumnData->createItem();
		(*record)[0] = QString::fromLatin1(method.signature());
		(*record)[1] = (*record)[0];
		m_signalsColumnData->append(record);
	}
#endif
}

void
ConnectionDialog::checkConnection(KexiDB::RecordData *record)
{
#ifdef __GNUC__
#warning "Port Kexi Table View!"
#endif
#ifndef KEXI_NO_TABLEVIEW
	// First we check if one column is empty
	for(int i = 1; i < 5; i++)
	{
		if( !record || (*record)[i].toString().isEmpty())
		{
			setStatusError( i18n("<qt>You have not selected item: <b>%1</b>.</qt>",
				m_data->column(i)->captionAliasOrName()), record);
			return;
		}
	}

	// Then we check if signal/slot args are compatible
	QString signal = (*record)[2].toString();
	signal = signal.remove( QRegExp(".*[(]|[)]") ); // just keep the args list
	QString slot = (*record)[4].toString();
	slot = slot.remove( QRegExp(".*[(]|[)]") );

	if(!signal.startsWith(slot, true))
	{
		setStatusError( i18n("The signal/slot arguments are not compatible."), record);
		return;
	}

	setStatusOk(record);
#endif
}

void
ConnectionDialog::newItem()
{
#ifdef __GNUC__
#warning "Port Kexi Table View!"
#endif
#ifndef KEXI_NO_TABLEVIEW
	m_table->acceptRowEdit();
	m_table->setCursorPosition(m_table->rows(), 1);
#endif
}

void
ConnectionDialog::newItemByDragnDrop()
{
	KFormDesigner::FormManager::self()->startCreatingConnection();
	connect(KFormDesigner::FormManager::self(), SIGNAL(connectionAborted(KFormDesigner::Form*)), this, SLOT(slotConnectionAborted(KFormDesigner::Form*)));
	connect(KFormDesigner::FormManager::self(), SIGNAL(connectionCreated(KFormDesigner::Form*, Connection&)), this, SLOT(slotConnectionCreated(KFormDesigner::Form*, Connection&)) );

	hide();
}

void
ConnectionDialog::slotConnectionCreated(KFormDesigner::Form *form, Connection &connection)
{
	show();
	if(form != m_form)
		return;

#ifdef __GNUC__
#warning "Port Kexi Table View!"
#endif
#ifndef KEXI_NO_TABLEVIEW
	Connection *c = new Connection(connection);
	KexiDB::RecordData *record = m_table->data()->createItem();
	(*record)[1] = c->sender();
	(*record)[2] = c->signal();
	(*record)[3] = c->receiver();
	(*record)[4] = c->slot();
	m_table->insertItem(record, m_table->rows());
	m_buffer->append(c);
#endif
}

void
ConnectionDialog::slotConnectionAborted(KFormDesigner::Form *form)
{
	show();
	if(form != m_form)
		return;

	newItem();
}

void
ConnectionDialog::removeItem()
{
#ifdef __GNUC__
#warning "Port Kexi Table View!"
#endif
#ifndef KEXI_NO_TABLEVIEW
	if(m_table->currentRow() == -1 || m_table->currentRow()>=m_table->rows())
		return;

	int confirm = KMessageBox::warningContinueCancel(parentWidget(),
		QString("<qt>")+i18n("Do you want to delete this connection ?")+"</qt>", 
		QString(), KGuiItem(i18n("&Delete Connection")),
		"dontAskBeforeDeleteConnection"/*config entry*/);
	if(confirm == KMessageBox::Cancel)
		return;

	m_buffer->remove(m_table->currentRow());
	m_table->deleteItem(m_table->selectedItem());
#endif
}

#include "connectiondialog.moc"
