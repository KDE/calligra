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

#include <qlayout.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qregexp.h>
#include <qmetaobject.h>
#include <qstrlist.h>

#include <kpushbutton.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <klocale.h>

#include "kexitableview.h"
#include "kexitableviewdata.h"
#include "events.h"
#include "form.h"
#include "formmanager.h"
#include "objecttree.h"

#include "connectiondialog.h"

namespace KFormDesigner {

/////////////////////////////////////////////////////////////////////////////////
///////////// The dialog to edit or add/remove connections //////////////////////
/////////////////////////////////////////////////////////////////////////////////
ConnectionDialog::ConnectionDialog(QWidget *parent)
: KDialogBase(parent, "connections_dialog", true, i18n("Edit Form Connections"),
							Ok|Cancel|Details, Ok, false)
, m_buffer(0)
{
	QFrame *frame = makeMainWidget();
	QHBoxLayout *layout = new QHBoxLayout(frame, 0, 6);

	// Setup the details widget /////////
	QHBox *details = new QHBox(frame);
	setDetailsWidget(details);
	setDetails(true);

	m_pixmapLabel = new QLabel(details);
	m_pixmapLabel->setFixedWidth( int(IconSize(KIcon::Desktop) * 1.5) );
	m_pixmapLabel->setAlignment(AlignHCenter | AlignTop);

	m_textLabel = new QLabel(details);
	m_textLabel->setAlignment(AlignLeft | AlignTop);
	//setStatusOk();

	// And the KexiTableView ////////
	m_data = new KexiTableViewData();
	m_table = new KexiTableView(0, frame, "connections_tableview");
	m_table->setSpreadSheetMode();
	m_table->setInsertingEnabled(true);
	initTable();
	m_table->setData(m_data, false);
	m_table->adjustColumnWidthToContents(0);
	layout->addWidget(m_table);

	//// Setup the icon toolbar /////////////////
	QVBoxLayout *vlayout = new QVBoxLayout(layout, 3);
	KPushButton *newItem = new KPushButton(SmallIconSet("filenew"), i18n("&New Connection"), frame);
	vlayout->addWidget(newItem);
	m_buttons.insert(BAdd, newItem);
	connect(newItem, SIGNAL(clicked()), this, SLOT(newItem()));

	KPushButton *delItem = new KPushButton(SmallIconSet("editdelete"), i18n("&Remove Connection"), frame);
	vlayout->addWidget(delItem);
	m_buttons.insert(BRemove, delItem);
	connect(delItem, SIGNAL(clicked()), this, SLOT(removeItem()));

	vlayout->addStretch();

	setInitialSize(QSize(600, 300));
	//setWFlags(WDestructiveClose);

	connect(m_table,SIGNAL(cellSelected(int, int)), this, SLOT(slotCellSelected(int, int)));
	connect(m_table->data(), SIGNAL(rowInserted(KexiTableItem*,bool)), this, SLOT(slotRowInserted(KexiTableItem*,bool)));
	this->newItem();
}

void
ConnectionDialog::initTable()
{
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

	QValueList<int> c;
	c << 2 << 4;
	m_table->maximizeColumnsWidth(c);
	m_table->setColumnStretchEnabled( true, 4 );

	connect(m_data, SIGNAL(aboutToChangeCell(KexiTableItem*, int, QVariant&, KexiDB::ResultInfo*)),
	      this,SLOT(slotCellChanged(KexiTableItem*, int, QVariant, KexiDB::ResultInfo*)));
	connect(m_data, SIGNAL(rowUpdated(KexiTableItem*)), this, SLOT(checkConnection(KexiTableItem *)));
	connect(m_table, SIGNAL(itemSelected(KexiTableItem *)), this, SLOT(checkConnection(KexiTableItem *)));
}

void
ConnectionDialog::exec(Form *form)
{
	m_form = form;
	updateTableData();

	KDialogBase::exec();
}

void ConnectionDialog::slotCellSelected(int col, int row)
{
	m_buttons[BRemove]->setEnabled( row < m_table->rows() );
	KexiTableItem *item = m_table->itemAt(row);
	if (!item)
		return;
	if(col == 2) // signal col
		updateSignalList(item);
	else if(col == 4) // slot col
		updateSlotList(item);
}

void ConnectionDialog::slotRowInserted(KexiTableItem* item,bool)
{
	m_buffer->append(new Connection());
	checkConnection( item );
}

void
ConnectionDialog::slotOk()
{
	// First we update our buffer contents
	for(int i=0; i < m_table->rows(); i++)
	{
		KexiTableItem *item = m_table->itemAt(i);
		Connection *c = m_buffer->at(i);

		c->setSender( (*item)[1].toString() );
		c->setSignal( (*item)[2].toString() );
		c->setReceiver( (*item)[3].toString() );
		c->setSlot( (*item)[4].toString() );
	}

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
		KexiTableItem *item = m_widgetsColumnData->createItem(); //new KexiTableItem(2);
		(*item)[0] = it.current()->name();
		(*item)[1] = (*item)[0];
		m_widgetsColumnData->append(item);
	}
	delete dict;

	// Then we fill the columns with the form connections
	for(Connection *c = m_form->connectionBuffer()->first(); c ; c = m_form->connectionBuffer()->next())
	{
		KexiTableItem *item = m_table->data()->createItem(); //new KexiTableItem(5);
		(*item)[1] = c->sender();
		(*item)[2] = c->signal();
		(*item)[3] = c->receiver();
		(*item)[4] = c->slot();
		m_table->insertItem(item, m_table->rows());
	}

	m_buffer = new ConnectionBuffer(*(m_form->connectionBuffer()));
}

void
ConnectionDialog::setStatusOk(KexiTableItem *item)
{
	m_pixmapLabel->setPixmap( DesktopIcon("button_ok") );
	m_textLabel->setText("<qt><h2>The connection is OK.</h2></qt>");

	if (!item)
		item = m_table->selectedItem();
	if (m_table->currentRow() >= m_table->rows())
		item = 0;

	if (item)
		(*item)[0] = "button_ok";
	else {
		m_pixmapLabel->setPixmap( QPixmap() );
		m_textLabel->setText(QString::null);
	}
}

void
ConnectionDialog::setStatusError(const QString &msg, KexiTableItem *item)
{
	m_pixmapLabel->setPixmap( DesktopIcon("button_cancel") );
	m_textLabel->setText("<qt><h2>The connection is invalid.</h2></qt>" + msg);

	if (!item)
		item = m_table->selectedItem();
	if (m_table->currentRow() >= m_table->rows())
		item = 0;

	if (item)
		(*item)[0] = "button_cancel";
	else {
		m_pixmapLabel->setPixmap( QPixmap() );
		m_textLabel->setText(QString::null);
	}
}

void
ConnectionDialog::slotCellChanged(KexiTableItem *item, int col, QVariant&, KexiDB::ResultInfo*)
{
	switch(col)
	{
		// sender changed, we clear siganl and slot
		case 1:
			(*item)[2] = QString("");
		// signal or receiver changed, we clear the slot cell
		case 2:
		case 3:
		{
			(*item)[4] = QString("");
			break;
		}
		default:
			break;
	}
}

void
ConnectionDialog::updateSlotList(KexiTableItem *item)
{
	m_slotsColumnData->deleteAllRows();
	QString widget = (*item)[1].toString();
	QString signal = (*item)[2].toString();

	if((widget.isEmpty()) || signal.isEmpty())
		return;
	ObjectTreeItem *tree = m_form->objectTree()->lookup(widget);
	if(!tree || !tree->widget())
		return;

	QString signalArg(signal);
	signalArg = signalArg.remove( QRegExp(".*[(]|[)]") );

	QStrList slotList = tree->widget()->metaObject()->slotNames(true);
	QStrListIterator it(slotList);
	for(; it.current() != 0; ++it)
	{
		// we add the slot only if it is compatible with the signal
		QString slotArg(*it);
		slotArg = slotArg.remove( QRegExp(".*[(]|[)]") );

		if(!signalArg.startsWith(slotArg, true) && (!signal.isEmpty())) // args not compatible
			continue;

		KexiTableItem *item = m_slotsColumnData->createItem(); //new KexiTableItem(2);
		(*item)[0] = QString(*it);
		(*item)[1] = (*item)[0];
		m_slotsColumnData->append(item);
	}
}

void
ConnectionDialog::updateSignalList(KexiTableItem *item)
{
	ObjectTreeItem *tree = m_form->objectTree()->lookup((*item)[1].toString());
	if(!tree || !tree->widget())
		return;

	m_signalsColumnData->deleteAllRows();
	QStrList signalList = tree->widget()->metaObject()->signalNames(true);
	QStrListIterator it(signalList);
	for(; it.current() != 0; ++it)
	{
		KexiTableItem *item = m_signalsColumnData->createItem(); //new KexiTableItem(2);
		(*item)[0] = QString(*it);
		(*item)[1] = (*item)[0];
		m_signalsColumnData->append(item);
	}
}

void
ConnectionDialog::checkConnection(KexiTableItem *item)
{
	// First we check if one column is empty
	for(int i = 1; i < 5; i++)
	{
		if( !item || (*item)[i].toString().isEmpty())
		{
			setStatusError( i18n("<qt>You have not selected item: <b>%1</b>.</qt>").arg(m_data->column(i)->captionAliasOrName()),
				item);
			return;
		}
	}

	// Then we check if signal/slot args are compatible
	QString signal = (*item)[2].toString();
	signal = signal.remove( QRegExp(".*[(]|[)]") ); // just keep the args list
	QString slot = (*item)[4].toString();
	slot = slot.remove( QRegExp(".*[(]|[)]") );

	if(!signal.startsWith(slot, true))
	{
		setStatusError( i18n("The signal/slot arguments are not compatible."), item);
		return;
	}

	setStatusOk(item);
}

void
ConnectionDialog::newItem()
{
	m_table->acceptRowEdit();
	m_table->setCursorPosition(m_table->rows(), 1);
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

	Connection *c = new Connection(connection);
	KexiTableItem *item = m_table->data()->createItem(); //new KexiTableItem(5);
	(*item)[1] = c->sender();
	(*item)[2] = c->signal();
	(*item)[3] = c->receiver();
	(*item)[4] = c->slot();
	m_table->insertItem(item, m_table->rows());
	m_buffer->append(c);
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
	if(m_table->currentRow() == -1 || m_table->currentRow()>=m_table->rows())
		return;

	int confirm = KMessageBox::warningContinueCancel(parentWidget(),
		QString("<qt>")+i18n("Do you want to delete this connection ?")+"</qt>", QString::null, KGuiItem(i18n("&Delete Connection")),
		"dontAskBeforeDeleteConnection"/*config entry*/);
	if(confirm == KMessageBox::Cancel)
		return;

	m_buffer->remove(m_table->currentRow());
	m_table->deleteItem(m_table->selectedItem());
}

}

#include "connectiondialog.moc"
