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
#include <KexiIcon.h>
#include "kexitableview.h"
#include "events.h"
#include "form.h"
#include "objecttree.h"

#include <KDbTableViewData>

#include <KMessageBox>
#include <KLocalizedString>

#include <QLabel>
#include <QRegExp>
#include <QPushButton>

using namespace KFormDesigner;

class ConnectionDialog::Private
{
public:
    explicit Private(Form *f);
    ~Private();

    Form *form;
    ConnectionBuffer *buffer;
    KexiTableView  *table;
    KDbTableViewData  *data;
    KDbTableViewData *widgetsColumnData,
    *slotsColumnData, *signalsColumnData;
    QLabel  *pixmapLabel, *textLabel;
    QPushButton *addButton, *removeButton;
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
    setWindowTitle(xi18nc("@title:window", "Edit Form Connections"));
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
    d->data = new KDbTableViewData();
    d->table = new KexiTableView(0, frame, "connections_tableview");
    d->table->setSpreadSheetMode(true);
    d->table->setInsertingEnabled(true);
    initTable();
    d->table->setData(d->data, false);
    d->table->adjustColumnWidthToContents(0);
    layout->addWidget(d->table);

    connect(d->table, SIGNAL(cellSelected(int,int)),
            this, SLOT(slotCellSelected(int,int)));
    connect(d->table->data(), SIGNAL(recordInserted(KDbRecordData*,bool)),
            this, SLOT(slotRecordInserted(KDbRecordData*,bool)));

    //// Setup the icon toolbar /////////////////
    QVBoxLayout *vlayout = new QVBoxLayout(layout);
    d->addButton = new QPushButton(koIcon("document-new"), xi18n("&New Connection"), frame);
    vlayout->addWidget(d->addButton);
    connect(d->addButton, SIGNAL(clicked()), this, SLOT(newItem()));

    d->removeButton = new QPushButton(koIcon("edit-delete"), xi18n("&Remove Connection"), frame);
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
    KDbTableViewColumn *col0 = new KDbTableViewColumn(xi18n("OK?"), KDbField::Text);
    col0->field()->setSubType("QIcon");
    col0->setReadOnly(true);
    col0->field()->setDescription(xi18n("Connection correctness"));
    d->data->addColumn(col0);

    KDbTableViewColumn *col1 = new KDbTableViewColumn(xi18n("Sender"), KDbField::Enum);
    d->widgetsColumnData = new KDbTableViewData(KDbField::Text, KDbField::Text);
    col1->setRelatedData(d->widgetsColumnData);
    d->data->addColumn(col1);

    KDbTableViewColumn *col2 = new KDbTableViewColumn(xi18n("Signal"), KDbField::Enum);
    d->signalsColumnData = new KDbTableViewData(KDbField::Text, KDbField::Text);
    col2->setRelatedData(d->signalsColumnData);
    d->data->addColumn(col2);

    KDbTableViewColumn *col3 = new KDbTableViewColumn(xi18n("Receiver"), KDbField::Enum);
    col3->setRelatedData(d->widgetsColumnData);
    d->data->addColumn(col3);

    KDbTableViewColumn *col4 = new KDbTableViewColumn(xi18n("Slot"), KDbField::Enum);
    d->slotsColumnData = new KDbTableViewData(KDbField::Text, KDbField::Text);
    col4->setRelatedData(d->slotsColumnData);
    d->data->addColumn(col4);

    QList<int> c;
    c << 2 << 4;
    d->table->maximizeColumnsWidth(c);
    d->table->setColumnResizeEnabled(4, true);

    connect(d->data, SIGNAL(aboutToChangeCell(KDbRecordData*,int,QVariant&,KDbResultInfo*)),
            this, SLOT(slotCellChanged(KDbRecordData*,int,QVariant,KDbResultInfo*)));
    connect(d->data, SIGNAL(recordUpdated(KDbRecordData*)), this, SLOT(checkConnection(KDbRecordData*)));
    connect(d->table, SIGNAL(itemSelected(KDbRecordData*)), this, SLOT(checkConnection(KDbRecordData*)));
}

void ConnectionDialog::exec()
{
    updateTableData();
    KDialog::exec();
}

void ConnectionDialog::slotCellSelected(int row, int col)
{
    d->removeButton->setEnabled(row < d->table->rows());
    KDbRecordData *data = d->table->itemAt(row);
    if (!data)
        return;
    if (col == 2) // signal col
        updateSignalList(data);
    else if (col == 4) // slot col
        updateSlotList(data);
}

void ConnectionDialog::slotRecordInserted(KDbRecordData* item, bool)
{
    d->buffer->append(new Connection());
    checkConnection(item);
}

void
ConnectionDialog::slotOk()
{
    // First we update our buffer contents
    for (int i = 0; i < d->table->rows(); i++) {
        KDbRecordData *data = d->table->itemAt(i);
        Connection *c = d->buffer->at(i);

        c->setSender((*data)[1].toString());
        c->setSignal((*data)[2].toString());
        c->setReceiver((*data)[3].toString());
        c->setSlot((*data)[4].toString());
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
        KDbRecordData *data = d->widgetsColumnData->createItem();
        (*data)[0] = item->name();
        (*data)[1] = (*data)[0];
        d->widgetsColumnData->append(data);
    }

    // Then we fill the columns with the form connections
    foreach (Connection *c, *d->form->connectionBuffer()) {
        KDbRecordData *newData = d->table->data()->createItem();
        (*newData )[1] = c->sender();
        (*newData )[2] = c->signal();
        (*newData )[3] = c->receiver();
        (*newData )[4] = c->slot();
        d->table->insertItem(newData , d->table->rows());
    }

    d->buffer = new ConnectionBuffer(*(d->form->connectionBuffer()));
}

void
ConnectionDialog::setStatusOk(KDbRecordData *data)
{
    d->pixmapLabel->setPixmap(koDesktopIcon("dialog-ok"));
    d->textLabel->setText(QString("<qt><h2>%1</h2></qt>").arg(xi18n("The connection is OK.")));

    if (!data)
        data = d->table->selectedItem();
    if (d->table->currentRow() >= d->table->rows())
        data = 0;

    if (data)
        (*data)[0] = "dialog-ok";
    else {
        d->pixmapLabel->setPixmap(QPixmap());
        d->textLabel->setText(QString());
    }
}

void
ConnectionDialog::setStatusError(const QString &msg, KDbRecordData *data)
{
    d->pixmapLabel->setPixmap(koDesktopIcon("dialog-cancel"));
    d->textLabel->setText(QString("<qt><h2>%1</h2></qt>").arg(xi18n("The connection is invalid.")) + msg);

    if (!data)
        data = d->table->selectedItem();
    if (d->table->currentRow() >= d->table->rows())
        data = 0;

    if (data)
        (*data)[0] = "dialog-cancel";
    else {
        d->pixmapLabel->setPixmap(QPixmap());
        d->textLabel->setText(QString());
    }
}

void
ConnectionDialog::slotCellChanged(KDbRecordData *data, int col, QVariant&, KDbResultInfo*)
{
    switch (col) {
        // sender changed, we clear siganl and slot
    case 1:
        (*data)[2] = QString("");
        // signal or receiver changed, we clear the slot cell
    case 2:
    case 3: {
        (*data)[4] = QString("");
        break;
    }
    default:
        break;
    }
}

void
ConnectionDialog::updateSlotList(KDbRecordData *data)
{
    d->slotsColumnData->deleteAllRecords();
    QString widget = (*data)[1].toString();
    QString signal = (*data)[2].toString();

    if ((widget.isEmpty()) || signal.isEmpty())
        return;
    ObjectTreeItem *tree = d->form->objectTree()->lookup(widget);
    if (!tree || !tree->widget())
        return;

    QString signalArg(signal);
    signalArg.remove(QRegExp(".*[(]|[)]"));

    const QList<QMetaMethod> list(
        KexiUtils::methodsForMetaObjectWithParents(tree->widget()->metaObject(),
                QMetaMethod::Slot, QMetaMethod::Public));
    foreach(const QMetaMethod &method, list) {
        // we add the slot only if it is compatible with the signal
        QString slotArg(method.signature());
        slotArg.remove(QRegExp(".*[(]|[)]"));
        if (!signalArg.startsWith(slotArg, Qt::CaseSensitive) && (!signal.isEmpty())) // args not compatible
            continue;

        KDbRecordData *newData = d->slotsColumnData->createItem();
        (*newData)[0] = QString::fromLatin1(method.signature());
        (*newData)[1] = (*newData)[0];
        d->slotsColumnData->append(newData);
    }
}

void
ConnectionDialog::updateSignalList(KDbRecordData *data)
{
    ObjectTreeItem *tree = d->form->objectTree()->lookup((*data)[1].toString());
    if (!tree || !tree->widget())
        return;

    d->signalsColumnData->deleteAllRecords();
    const QList<QMetaMethod> list(
        KexiUtils::methodsForMetaObjectWithParents(tree->widget()->metaObject(),
                QMetaMethod::Signal, QMetaMethod::Public));
    foreach(const QMetaMethod &method, list) {
        KDbRecordData *newData = d->signalsColumnData->createItem();
        (*newData )[0] = QString::fromLatin1(method.signature());
        (*newData )[1] = (*newData )[0];
        d->signalsColumnData->append(newData );
    }
}

void
ConnectionDialog::checkConnection(KDbRecordData *data)
{
    // First we check if one column is empty
    for (int i = 1; i < 5; i++) {
        if (!data || (*data)[i].toString().isEmpty()) {
            setStatusError(xi18n("You have not selected item: <resource>%1</resource>.",
                                d->data->column(i)->captionAliasOrName()), data);
            return;
        }
    }

    // Then we check if signal/slot args are compatible
    QString signal = (*data)[2].toString();
    signal.remove(QRegExp(".*[(]|[)]"));   // just keep the args list
    QString slot = (*data)[4].toString();
    slot.remove(QRegExp(".*[(]|[)]"));

    if (!signal.startsWith(slot, Qt::CaseSensitive)) {
        setStatusError(xi18n("The signal/slot arguments are not compatible."), data);
        return;
    }

    setStatusOk(data);
}

void
ConnectionDialog::newItem()
{
    d->table->acceptRecordEditing();
    d->table->setCursorPosition(d->table->rows(), 1);
}

void
ConnectionDialog::newItemByDragnDrop()
{
    d->form->enterConnectingState();
    connect(d->form, SIGNAL(connectionAborted(KFormDesigner::Form*)),
        this, SLOT(slotConnectionAborted(KFormDesigner::Form*)));
    connect(d->form, SIGNAL(connectionCreated(KFormDesigner::Form*,Connection&)),
        this, SLOT(slotConnectionCreated(KFormDesigner::Form*,Connection&)));

    hide();
}

void
ConnectionDialog::slotConnectionCreated(KFormDesigner::Form *form, Connection &connection)
{
    show();
    if (form != d->form)
        return;

    Connection *c = new Connection(connection);
    KDbRecordData *newData = d->table->data()->createItem();
    (*newData)[1] = c->sender();
    (*newData)[2] = c->signal();
    (*newData)[3] = c->receiver();
    (*newData)[4] = c->slot();
    d->table->insertItem(newData, d->table->rows());
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
              xi18n("Do you want to delete this connection?"),
              QString(),
              KGuiItem(xi18n("&Delete Connection")),
              KStandardGuiItem::no(),
              "AskBeforeDeleteConnection"/*config entry*/);
    if (confirm != KMessageBox::Yes)
        return;

    d->buffer->removeAt(d->table->currentRow());
    d->table->deleteItem(d->table->selectedItem());
}

//! @todo KEXI3 noi18n # added to disable message extraction in Messages.sh
