/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2009 Jarosław Staniek <staniek@kde.org>

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

#include <QLayout>
#include <QLabel>
#include <QRegExp>
#include <QMetaObject>

#include <kpushbutton.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <klocale.h>

#include <KoIcon.h>
#include "kexitableview.h"
#include "kexitableviewdata.h"

#include "events.h"
#include "form.h"
#include "objecttree.h"


using namespace KFormDesigner;

class ConnectionDialog::Private
{
public:
    Private(Form *f);
    ~Private();

    Form *form;
    ConnectionBuffer *buffer;
    KexiTableView  *table;
    KexiTableViewData  *data;
    KexiTableViewData *widgetsColumnData,
    *slotsColumnData, *signalsColumnData;
    QLabel  *pixmapLabel, *textLabel;
    KPushButton *addButton, *removeButton;
};

ConnectionDialog::Private::Private(Form *f)
    :form(f), buffer(0)
{

}

ConnectionDialog::Private::~Private()
{

}

/////////////////////////////////////////////////////////////////////////////////
///////////// The dialog to edit or add/remove connections //////////////////////
/////////////////////////////////////////////////////////////////////////////////
ConnectionDialog::ConnectionDialog(Form *form, QWidget *parent)
        : KDialog(parent)
        , d(new Private(form))
{
    setObjectName("connections_dialog");
    setModal(true);
    setWindowTitle(i18n("Edit Form Connections"));
    setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Details);
    setDefaultButton(KDialog::Ok);

    QFrame *frame = new QFrame(this);
    setMainWidget(frame);
    QHBoxLayout *layout = new QHBoxLayout(frame);

    // Setup the details widget /////////
    QWidget *details = new QWidget(frame);
    layout->addWidget(details);
    QHBoxLayout *detailsLyr = new QHBoxLayout(details);
    setDetailsWidget(details);
    setDetailsWidgetVisible(true);

    d->pixmapLabel = new QLabel(details);
    detailsLyr->addWidget(d->pixmapLabel);
    d->pixmapLabel->setFixedWidth(int(IconSize(KIconLoader::Desktop) * 1.5));
    d->pixmapLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    d->textLabel = new QLabel(details);
    detailsLyr->addWidget(d->textLabel);
    d->textLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    //setStatusOk();

    // And the KexiTableView ////////
    d->data = new KexiTableViewData();
    d->table = new KexiTableView(0, frame, "connections_tableview");
    d->table->setSpreadSheetMode();
    d->table->setInsertingEnabled(true);
    initTable();
    d->table->setData(d->data, false);
    d->table->adjustColumnWidthToContents(0);
    layout->addWidget(d->table);

    connect(d->table, SIGNAL(cellSelected(int, int)),
            this, SLOT(slotCellSelected(int, int)));
    connect(d->table->data(), SIGNAL(rowInserted(KexiDB::RecordData*, bool)),
            this, SLOT(slotRowInserted(KexiDB::RecordData*, bool)));

    //// Setup the icon toolbar /////////////////
    QVBoxLayout *vlayout = new QVBoxLayout(layout);
    d->addButton = new KPushButton(koIcon("document-new"), i18n("&New Connection"), frame);
    vlayout->addWidget(d->addButton);
    connect(d->addButton, SIGNAL(clicked()), this, SLOT(newItem()));

    d->removeButton = new KPushButton(koIcon("edit-delete"), i18n("&Remove Connection"), frame);
    vlayout->addWidget(d->removeButton);
    connect(d->removeButton, SIGNAL(clicked()), this, SLOT(removeItem()));

    vlayout->addStretch();

    setInitialSize(QSize(600, 300));
    //setWFlags(WDestructiveClose);

    this->newItem();
}

ConnectionDialog::~ConnectionDialog()
{
    delete d;
}

void
ConnectionDialog::initTable()
{
    KexiTableViewColumn *col0 = new KexiTableViewColumn(i18n("OK?"), KexiDB::Field::Text);
    col0->field()->setSubType("KIcon");
    col0->setReadOnly(true);
    col0->field()->setDescription(i18n("Connection correctness"));
    d->data->addColumn(col0);

    KexiTableViewColumn *col1 = new KexiTableViewColumn(i18n("Sender"), KexiDB::Field::Enum);
    d->widgetsColumnData = new KexiTableViewData(KexiDB::Field::Text, KexiDB::Field::Text);
    col1->setRelatedData(d->widgetsColumnData);
    d->data->addColumn(col1);

    KexiTableViewColumn *col2 = new KexiTableViewColumn(i18n("Signal"), KexiDB::Field::Enum);
    d->signalsColumnData = new KexiTableViewData(KexiDB::Field::Text, KexiDB::Field::Text);
    col2->setRelatedData(d->signalsColumnData);
    d->data->addColumn(col2);

    KexiTableViewColumn *col3 = new KexiTableViewColumn(i18n("Receiver"), KexiDB::Field::Enum);
    col3->setRelatedData(d->widgetsColumnData);
    d->data->addColumn(col3);

    KexiTableViewColumn *col4 = new KexiTableViewColumn(i18n("Slot"), KexiDB::Field::Enum);
    d->slotsColumnData = new KexiTableViewData(KexiDB::Field::Text, KexiDB::Field::Text);
    col4->setRelatedData(d->slotsColumnData);
    d->data->addColumn(col4);

    QList<int> c;
    c << 2 << 4;
    d->table->maximizeColumnsWidth(c);
    d->table->setColumnStretchEnabled(true, 4);

    connect(d->data, SIGNAL(aboutToChangeCell(KexiDB::RecordData*, int, QVariant&, KexiDB::ResultInfo*)),
            this, SLOT(slotCellChanged(KexiDB::RecordData*, int, QVariant, KexiDB::ResultInfo*)));
    connect(d->data, SIGNAL(rowUpdated(KexiDB::RecordData*)), this, SLOT(checkConnection(KexiDB::RecordData*)));
    connect(d->table, SIGNAL(itemSelected(KexiDB::RecordData*)), this, SLOT(checkConnection(KexiDB::RecordData*)));
}

void ConnectionDialog::exec()
{
    updateTableData();
    KDialog::exec();
}

void ConnectionDialog::slotCellSelected(int col, int row)
{
    d->removeButton->setEnabled(row < d->table->rows());
    KexiDB::RecordData *record = d->table->itemAt(row);
    if (!record)
        return;
    if (col == 2) // signal col
        updateSignalList(record);
    else if (col == 4) // slot col
        updateSlotList(record);
}

void ConnectionDialog::slotRowInserted(KexiDB::RecordData* item, bool)
{
    d->buffer->append(new Connection());
    checkConnection(item);
}

void
ConnectionDialog::slotOk()
{
    // First we update our buffer contents
    for (int i = 0; i < d->table->rows(); i++) {
        KexiDB::RecordData *record = d->table->itemAt(i);
        Connection *c = d->buffer->at(i);

        c->setSender((*record)[1].toString());
        c->setSignal((*record)[2].toString());
        c->setReceiver((*record)[3].toString());
        c->setSlot((*record)[4].toString());
    }

    // then me make it replace form's current one
    d->form->setConnectionBuffer(d->buffer);

    QDialog::accept();
}

void
ConnectionDialog::updateTableData()
{
    // First we update the columns data
    foreach (ObjectTreeItem *item, *d->form->objectTree()->hash()) {
        KexiDB::RecordData *record = d->widgetsColumnData->createItem();
        (*record)[0] = item->name();
        (*record)[1] = (*record)[0];
        d->widgetsColumnData->append(record);
    }

    // Then we fill the columns with the form connections
    foreach (Connection *c, *d->form->connectionBuffer()) {
        KexiDB::RecordData *record = d->table->data()->createItem();
        (*record)[1] = c->sender();
        (*record)[2] = c->signal();
        (*record)[3] = c->receiver();
        (*record)[4] = c->slot();
        d->table->insertItem(record, d->table->rows());
    }

    d->buffer = new ConnectionBuffer(*(d->form->connectionBuffer()));
}

void
ConnectionDialog::setStatusOk(KexiDB::RecordData *record)
{
    d->pixmapLabel->setPixmap(koDesktopIcon("dialog-ok"));
    d->textLabel->setText(i18n("<qt><h2>The connection is OK.</h2></qt>"));

    if (!record)
        record = d->table->selectedItem();
    if (d->table->currentRow() >= d->table->rows())
        record = 0;

    if (record)
        (*record)[0] = "dialog-ok";
    else {
        d->pixmapLabel->setPixmap(QPixmap());
        d->textLabel->setText(QString());
    }
}

void
ConnectionDialog::setStatusError(const QString &msg, KexiDB::RecordData *record)
{
    d->pixmapLabel->setPixmap(koDesktopIcon("dialog-cancel"));
    d->textLabel->setText(i18n("<qt><h2>The connection is invalid.</h2></qt>") + msg);

    if (!record)
        record = d->table->selectedItem();
    if (d->table->currentRow() >= d->table->rows())
        record = 0;

    if (record)
        (*record)[0] = "dialog-cancel";
    else {
        d->pixmapLabel->setPixmap(QPixmap());
        d->textLabel->setText(QString());
    }
}

void
ConnectionDialog::slotCellChanged(KexiDB::RecordData *record, int col, QVariant&, KexiDB::ResultInfo*)
{
    switch (col) {
        // sender changed, we clear siganl and slot
    case 1:
        (*record)[2] = QString("");
        // signal or receiver changed, we clear the slot cell
    case 2:
    case 3: {
        (*record)[4] = QString("");
        break;
    }
    default:
        break;
    }
}

void
ConnectionDialog::updateSlotList(KexiDB::RecordData *record)
{
    d->slotsColumnData->deleteAllRows();
    QString widget = (*record)[1].toString();
    QString signal = (*record)[2].toString();

    if ((widget.isEmpty()) || signal.isEmpty())
        return;
    ObjectTreeItem *tree = d->form->objectTree()->lookup(widget);
    if (!tree || !tree->widget())
        return;

    QString signalArg(signal);
    signalArg = signalArg.remove(QRegExp(".*[(]|[)]"));

    const QList<QMetaMethod> list(
        KexiUtils::methodsForMetaObjectWithParents(tree->widget()->metaObject(),
                QMetaMethod::Slot, QMetaMethod::Public));
    foreach(QMetaMethod method, list) {
        // we add the slot only if it is compatible with the signal
        QString slotArg(method.signature());
        slotArg = slotArg.remove(QRegExp(".*[(]|[)]"));
        if (!signalArg.startsWith(slotArg, Qt::CaseSensitive) && (!signal.isEmpty())) // args not compatible
            continue;

        KexiDB::RecordData *record = d->slotsColumnData->createItem();
        (*record)[0] = QString::fromLatin1(method.signature());
        (*record)[1] = (*record)[0];
        d->slotsColumnData->append(record);
    }
}

void
ConnectionDialog::updateSignalList(KexiDB::RecordData *record)
{
    ObjectTreeItem *tree = d->form->objectTree()->lookup((*record)[1].toString());
    if (!tree || !tree->widget())
        return;

    d->signalsColumnData->deleteAllRows();
    const QList<QMetaMethod> list(
        KexiUtils::methodsForMetaObjectWithParents(tree->widget()->metaObject(),
                QMetaMethod::Signal, QMetaMethod::Public));
    foreach(QMetaMethod method, list) {
        KexiDB::RecordData *record = d->signalsColumnData->createItem();
        (*record)[0] = QString::fromLatin1(method.signature());
        (*record)[1] = (*record)[0];
        d->signalsColumnData->append(record);
    }
}

void
ConnectionDialog::checkConnection(KexiDB::RecordData *record)
{
    // First we check if one column is empty
    for (int i = 1; i < 5; i++) {
        if (!record || (*record)[i].toString().isEmpty()) {
            setStatusError(i18n("<qt>You have not selected item: <b>%1</b>.</qt>",
                                d->data->column(i)->captionAliasOrName()), record);
            return;
        }
    }

    // Then we check if signal/slot args are compatible
    QString signal = (*record)[2].toString();
    signal = signal.remove(QRegExp(".*[(]|[)]"));   // just keep the args list
    QString slot = (*record)[4].toString();
    slot = slot.remove(QRegExp(".*[(]|[)]"));

    if (!signal.startsWith(slot, Qt::CaseSensitive)) {
        setStatusError(i18n("The signal/slot arguments are not compatible."), record);
        return;
    }

    setStatusOk(record);
}

void
ConnectionDialog::newItem()
{
    d->table->acceptRowEdit();
    d->table->setCursorPosition(d->table->rows(), 1);
}

void
ConnectionDialog::newItemByDragnDrop()
{
    d->form->enterConnectingState();
    connect(d->form, SIGNAL(connectionAborted(KFormDesigner::Form*)),
        this, SLOT(slotConnectionAborted(KFormDesigner::Form*)));
    connect(d->form, SIGNAL(connectionCreated(KFormDesigner::Form*, Connection&)),
        this, SLOT(slotConnectionCreated(KFormDesigner::Form*, Connection&)));

    hide();
}

void
ConnectionDialog::slotConnectionCreated(KFormDesigner::Form *form, Connection &connection)
{
    show();
    if (form != d->form)
        return;

    Connection *c = new Connection(connection);
    KexiDB::RecordData *record = d->table->data()->createItem();
    (*record)[1] = c->sender();
    (*record)[2] = c->signal();
    (*record)[3] = c->receiver();
    (*record)[4] = c->slot();
    d->table->insertItem(record, d->table->rows());
    d->buffer->append(c);
}

void
ConnectionDialog::slotConnectionAborted(KFormDesigner::Form *form)
{
    show();
    if (form != d->form)
        return;

    newItem();
}

void
ConnectionDialog::removeItem()
{
    if (d->table->currentRow() == -1 || d->table->currentRow() >= d->table->rows())
        return;

    const int confirm
        = KMessageBox::warningYesNo(parentWidget(),
              i18n("Do you want to delete this connection?"),
              QString(),
              KGuiItem(i18n("&Delete Connection")),
              KStandardGuiItem::no(),
              "dontAskBeforeDeleteConnection"/*config entry*/);
    if (confirm != KMessageBox::Yes)
        return;

    d->buffer->removeAt(d->table->currentRow());
    d->table->deleteItem(d->table->selectedItem());
}

#include "connectiondialog.moc"
