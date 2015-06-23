/* This file is part of the KDE project
   Copyright (C) 2004-2014 Jarosław Staniek <staniek@kde.org>

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
#include "kexidataawareobjectiface.h"
#include <kexi_global.h>
#include <core/KexiView.h>

#include <KProperty>

#include <KDbTableViewData>

#include <QDebug>

class KexiDataAwarePropertySet::Private
{
public:
    Private(KexiView *aview, KexiDataAwareObjectInterface *adataObject)
      : view(aview)
      , dataObject(adataObject)
      , record(-99)
    {
    }

    ~Private()
    {
        qDeleteAll(sets);
        sets.clear();
    }

    QVector<KPropertySet*> sets; //!< prop. sets vector

    QPointer<KexiView> view;
    KexiDataAwareObjectInterface* dataObject;
    QPointer<KDbTableViewData> currentTVData;

    int record; //!< used to know if a new record is selected in slotCellSelected()
};

KexiDataAwarePropertySet::KexiDataAwarePropertySet(KexiView *view,
        KexiDataAwareObjectInterface* dataObject)
        : QObject(view)
        , d(new Private(view, dataObject))
{
    setObjectName(view->objectName() + "_KexiDataAwarePropertySet");
    d->dataObject->connectDataSetSignal(this, SLOT(slotDataSet(KDbTableViewData*)));
    d->dataObject->connectCellSelectedSignal(this, SLOT(slotCellSelected(int,int)));
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

void KexiDataAwarePropertySet::slotDataSet(KDbTableViewData *data)
{
    if (!d->currentTVData.isNull()) {
        d->currentTVData->disconnect(this);
        clear();
    }
    d->currentTVData = data;
    if (!d->currentTVData.isNull()) {
        connect(d->currentTVData, SIGNAL(recordDeleted()), this, SLOT(slotRecordDeleted()));
        connect(d->currentTVData, SIGNAL(recordsDeleted(QList<int>)),
                this, SLOT(slotRecordsDeleted(QList<int>)));
        connect(d->currentTVData, SIGNAL(recordInserted(KDbRecordData*,uint,bool)),
                this, SLOT(slotRecordInserted(KDbRecordData*,uint,bool)));
        connect(d->currentTVData, SIGNAL(reloadRequested()),
                this, SLOT(slotReloadRequested()));
    }
}

void KexiDataAwarePropertySet::eraseCurrentPropertySet()
{
    eraseAt(d->dataObject->currentRecord());
}

void KexiDataAwarePropertySet::eraseAt(uint record)
{
    KPropertySet *set = d->sets.value(record);
    if (!set) {
        qWarning() << "No record to erase:" << record;
        return;
    }
    d->sets[record] = 0;
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

void KexiDataAwarePropertySet::enlargeToFitRecord(uint record)
{
    uint newSize = d->sets.size();
    if (record < newSize) {
        return;
    }
    while (newSize < (record + 1)) {
        newSize *= 2;
    }
    d->sets.resize(newSize);
}

void KexiDataAwarePropertySet::slotReloadRequested()
{
    clear();
}

void KexiDataAwarePropertySet::set(uint record, KPropertySet* set, bool newOne)
{
    if (!set) {
        Q_ASSERT_X(false, "KexiDataAwarePropertySet::set", "set == 0");
        qWarning() << "set == 0";
        return;
    }
    if (set->parent() && set->parent() != this) {
        const char *msg = "property set's parent must be NULL or this KexiDataAwarePropertySet";
        Q_ASSERT_X(false, "KexiDataAwarePropertySet::set", msg);
        qWarning() << msg;
        return;
    }
    enlargeToFitRecord(record);

    d->sets[record] = set;

    connect(set, SIGNAL(propertyChanged(KPropertySet&,KProperty&)), d->view, SLOT(setDirty()));
    connect(set, SIGNAL(propertyChanged(KPropertySet&,KProperty&)),
            this, SIGNAL(propertyChanged(KPropertySet&,KProperty&)));

    if (newOne) {
        //add a special property indicating that this is brand new set,
        //not just changed
        KProperty* prop = new KProperty("newrecord");
        prop->setVisible(false);
        set->addProperty(prop);
        d->view->setDirty();
    }
}

KPropertySet* KexiDataAwarePropertySet::currentPropertySet() const
{
    if (d->dataObject->currentRecord() < 0) {
        return 0;
    }
    return d->sets.value(d->dataObject->currentRecord());
}

uint KexiDataAwarePropertySet::currentRecord() const
{
    return d->dataObject->currentRecord();
}

KPropertySet* KexiDataAwarePropertySet::at(uint record) const
{
    return d->sets.value(record);
}


void KexiDataAwarePropertySet::slotRecordDeleted()
{
    d->view->setDirty();
    enlargeToFitRecord(d->dataObject->currentRecord());
    d->sets.remove(d->dataObject->currentRecord());

    d->view->propertySetSwitched();
    emit recordDeleted();
}

void KexiDataAwarePropertySet::slotRecordsDeleted(const QList<int> &_record)
{
    if (_record.isEmpty()) {
        return;
    }
    //let's move most property sets up & delete unwanted
    const int orig_size = size();
    int prev_r = -1;
    int num_removed = 0;
    int cur_r = -1;
    QList<int> record(_record);
    qSort(record);
    enlargeToFitRecord(record.last());
    for (QList<int>::ConstIterator r_it = record.constBegin(); r_it != record.constEnd() && *r_it <
         orig_size; ++r_it)
    {
        cur_r = *r_it;
        if (prev_r >= 0) {
//   qDebug() << "move " << prev_r+nud->removed-1 << ".." << cur_r-1 << " to " << prev_r+nud->removed-1 << ".." << cur_r-2;
            int i = prev_r;
            KPropertySet *set = d->sets.at(i + num_removed);
            d->sets.remove(i + num_removed);
            qDebug() << "property set " << i + num_removed << " deleted";
            delete set;
            num_removed++;
        }
        prev_r = cur_r - num_removed;
    }
    //finally: add empty records
    d->sets.insert(size(), num_removed, 0);

    if (num_removed > 0)
        d->view->setDirty();
    d->view->propertySetSwitched();
}

void KexiDataAwarePropertySet::slotRecordInserted(KDbRecordData*, uint pos, bool /*repaint*/)
{
    d->view->setDirty();
    if (pos > 0) {
        enlargeToFitRecord(pos - 1);
    }
    d->sets.insert(pos, 0);

    d->view->propertySetSwitched();
    emit recordInserted();
}

void KexiDataAwarePropertySet::slotCellSelected(int record, int column)
{
    Q_UNUSED(column);
    if (record == d->record)
        return;
    d->record = record;
    d->view->propertySetSwitched();
}

KPropertySet* KexiDataAwarePropertySet::findPropertySetForItem(const KDbRecordData& record)
{
    if (d->currentTVData.isNull())
        return 0;
    int idx = d->currentTVData->indexOf(&record);
    if (idx < 0)
        return 0;
    return d->sets.at(idx);
}

int KexiDataAwarePropertySet::findRecordForPropertyValue(
    const QByteArray& propertyName, const QVariant& value)
{
    const int size = d->sets.size();
    for (int i = 0; i < size; i++) {
        KPropertySet *set = d->sets.at(i);
        if (!set || !set->contains(propertyName))
            continue;
        if (set->propertyValue(propertyName) == value)
            return i;
    }
    return -1;
}

