/* This file is part of the KDE project
   Copyright (C) 2004-2012 Jarosław Staniek <staniek@kde.org>

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

#include "kexidataawarepropertyset.h"
#include "kexitableviewdata.h"
#include "kexidataawareobjectiface.h"

#include <kexi_global.h>
#include <koproperty/Property.h>
#include <core/KexiView.h>

class KexiDataAwarePropertySet::Private
{
public:
    Private(KexiView *aview, KexiDataAwareObjectInterface *adataObject)
      : view(aview)
      , dataObject(adataObject)
      , row(-99)
    {
    }

    ~Private()
    {
        qDeleteAll(sets);
        sets.clear();
    }

    QVector<KoProperty::Set*> sets; //!< prop. sets vector

    QPointer<KexiView> view;
    KexiDataAwareObjectInterface* dataObject;
    QPointer<KexiTableViewData> currentTVData;

    int row; //!< used to know if a new row is selected in slotCellSelected()
};

KexiDataAwarePropertySet::KexiDataAwarePropertySet(KexiView *view,
        KexiDataAwareObjectInterface* dataObject)
        : QObject(view)
        , d(new Private(view, dataObject))
{
    setObjectName(view->objectName() + "_KexiDataAwarePropertySet");
    d->dataObject->connectDataSetSignal(this, SLOT(slotDataSet(KexiTableViewData*)));
    d->dataObject->connectCellSelectedSignal(this, SLOT(slotCellSelected(int, int)));
    slotDataSet(d->dataObject->data());
    const bool wasDirty = view->isDirty();
    clear();
    if (!wasDirty)
        view->setDirty(false);
}

KexiDataAwarePropertySet::~KexiDataAwarePropertySet()
{
    delete d;
}

void KexiDataAwarePropertySet::slotDataSet(KexiTableViewData *data)
{
    if (!d->currentTVData.isNull()) {
        d->currentTVData->disconnect(this);
        clear();
    }
    d->currentTVData = data;
    if (!d->currentTVData.isNull()) {
        connect(d->currentTVData, SIGNAL(rowDeleted()), this, SLOT(slotRowDeleted()));
        connect(d->currentTVData, SIGNAL(rowsDeleted(const QList<int> &)),
                this, SLOT(slotRowsDeleted(const QList<int> &)));
        connect(d->currentTVData, SIGNAL(rowInserted(KexiDB::RecordData*, uint, bool)),
                this, SLOT(slotRowInserted(KexiDB::RecordData*, uint, bool)));
        connect(d->currentTVData, SIGNAL(reloadRequested()),
                this, SLOT(slotReloadRequested()));
    }
}

void KexiDataAwarePropertySet::eraseCurrentPropertySet()
{
    eraseAt(d->dataObject->currentRow());
}

void KexiDataAwarePropertySet::eraseAt(uint row)
{
    KoProperty::Set *set = d->sets.value(row);
    if (!set) {
        kWarning() << "No row to erase:" << row;
        return;
    }
    d->sets[row] = 0;
    set->debug();
    delete set;
    d->view->setDirty();
    d->view->propertySetSwitched();
}

uint KexiDataAwarePropertySet::size() const
{
    return d->sets.size();
}

void KexiDataAwarePropertySet::clear()
{
    qDeleteAll(d->sets);
    d->sets.clear();
    d->sets.resize(1000);
    d->view->setDirty(true);
    d->view->propertySetSwitched();
}

void KexiDataAwarePropertySet::enlargeToFitRow(uint row)
{
    uint newSize = d->sets.size();
    if (row < newSize) {
        return;
    }
    while (newSize < (row + 1)) {
        newSize *= 2;
    }
    d->sets.resize(newSize);
}

void KexiDataAwarePropertySet::slotReloadRequested()
{
    clear();
}

void KexiDataAwarePropertySet::set(uint row, KoProperty::Set* set, bool newOne)
{
    if (!set) {
        Q_ASSERT_X(false, "KexiDataAwarePropertySet::set", "set == 0");
        kWarning() << "set == 0";
        return;
    }
    if (set->parent() && set->parent() != this) {
        const char *msg = "property set's parent must be NULL or this KexiDataAwarePropertySet";
        Q_ASSERT_X(false, "KexiDataAwarePropertySet::set", msg);
        kWarning() << msg;
        return;
    }
    enlargeToFitRow(row);

    d->sets[row] = set;

    connect(set, SIGNAL(propertyChanged(KoProperty::Set&, KoProperty::Property&)), d->view, SLOT(setDirty()));

    if (newOne) {
        //add a special property indicating that this is brand new set,
        //not just changed
        KoProperty::Property* prop = new KoProperty::Property("newrecord");
        prop->setVisible(false);
        set->addProperty(prop);
        d->view->setDirty();
    }
}

KoProperty::Set* KexiDataAwarePropertySet::currentPropertySet() const
{
    if (d->dataObject->currentRow() < 0) {
        return 0;
    }
    return d->sets.value(d->dataObject->currentRow());
}

uint KexiDataAwarePropertySet::currentRow() const
{
    return d->dataObject->currentRow();
}

KoProperty::Set* KexiDataAwarePropertySet::at(uint row) const
{
    return d->sets.value(row);
}


void KexiDataAwarePropertySet::slotRowDeleted()
{
    d->view->setDirty();
    enlargeToFitRow(d->dataObject->currentRow());
    d->sets.remove(d->dataObject->currentRow());

    d->view->propertySetSwitched();
    emit rowDeleted();
}

void KexiDataAwarePropertySet::slotRowsDeleted(const QList<int> &_rows)
{
    if (_rows.isEmpty()) {
        return;
    }
    //let's move most property sets up & delete unwanted
    const int orig_size = size();
    int prev_r = -1;
    int num_removed = 0;
    int cur_r = -1;
    QList<int> rows(_rows);
    qSort(rows);
    enlargeToFitRow(rows.last());
    for (QList<int>::ConstIterator r_it = rows.constBegin(); r_it != rows.constEnd() && *r_it <
         orig_size; ++r_it)
    {
        cur_r = *r_it;
        if (prev_r >= 0) {
//   kDebug() << "move " << prev_r+nud->removed-1 << ".." << cur_r-1 << " to " << prev_r+nud->removed-1 << ".." << cur_r-2;
            int i = prev_r;
            KoProperty::Set *set = d->sets.at(i + num_removed);
            d->sets.remove(i + num_removed);
            kDebug() << "property set " << i + num_removed << " deleted";
            delete set;
            num_removed++;
        }
        prev_r = cur_r - num_removed;
    }
    //finally: add empty rows
    d->sets.insert(size(), num_removed, 0);

    if (num_removed > 0)
        d->view->setDirty();
    d->view->propertySetSwitched();
}

void KexiDataAwarePropertySet::slotRowInserted(KexiDB::RecordData*, uint pos, bool /*repaint*/)
{
    d->view->setDirty();
    if (pos > 0) {
        enlargeToFitRow(pos - 1);
    }
    d->sets.insert(pos, 0);

    d->view->propertySetSwitched();
    emit rowInserted();
}

void KexiDataAwarePropertySet::slotCellSelected(int, int row)
{
    if (row == d->row)
        return;
    d->row = row;
    d->view->propertySetSwitched();
}

KoProperty::Set* KexiDataAwarePropertySet::findPropertySetForItem(const KexiDB::RecordData& record)
{
    if (d->currentTVData.isNull())
        return 0;
    int idx = d->currentTVData->indexOf(&record);
    if (idx < 0)
        return 0;
    return d->sets.at(idx);
}

int KexiDataAwarePropertySet::findRowForPropertyValue(
    const QByteArray& propertyName, const QVariant& value)
{
    const int size = d->sets.size();
    for (int i = 0; i < size; i++) {
        KoProperty::Set *set = d->sets.at(i);
        if (!set || !set->contains(propertyName))
            continue;
        if (set->propertyValue(propertyName) == value)
            return i;
    }
    return -1;
}

#include "kexidataawarepropertyset.moc"
