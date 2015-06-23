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

#ifndef KEXIDATAAWAREPROPERTYSET_H
#define KEXIDATAAWAREPROPERTYSET_H

#include "kexidataviewcommon_export.h"

#include <KPropertySet>

#include <KDbRecordData>
#include <KDbTableViewData>

#include <QList>
#include <QByteArray>

class KDbTableViewData;
class KexiView;
class KexiDataAwareObjectInterface;

/*! This helper class handles data changes of a single
 object implementing KexiDataAwareObjectInterface (e.g. KexiTableView) inside
 a KexiView container.

 It is currently used in KexiAlterTableDialog and KexiQueryDesignerGuiEditor,
 and may be used for similar purposes, when each KexiDataAwareObjectInterface's
 record can be associated with single KPropertySet object, and given
 KexiDataAwareObjectInterface object has to inform the world about currently
 selected record/property set.

 Following functionality is built-in:
 - auto-initializing after resetting of table view's data
 - destroying single property set that is associated with deleted record
 - inserting single property set that and associating it with new record
 - all property sets are cleared when view's data is cleared (using clear())
 - setting view's 'dirty' flag when needed
 - signalling via KexiView::propertySetSwitched() that current property
   set has changed (e.g. on moving to other record)
*/
class KEXIDATAVIEWCOMMON_EXPORT KexiDataAwarePropertySet : public QObject
{
    Q_OBJECT

public:
    /*! You can instantiate KexiDataAwarePropertySet object
     for existing \a tableView and \a view. \a tableView can have data assigned
     (KexiDataAwareObjectInterface::setData()) now but it can be done later as well
     (but assigning data is needed for proper functionality).
     Any changed reassignments of table view's data will be handled automatically. */
    KexiDataAwarePropertySet(KexiView *view, KexiDataAwareObjectInterface* dataObject);

    virtual ~KexiDataAwarePropertySet();

    uint size() const;

    KPropertySet* currentPropertySet() const;

    uint currentRecord() const;
    KPropertySet* at(uint record) const;

    /*! \return a pointer to property set assigned for \a record or null if \a item has no
     property set assigned or it's not owned by assigned table view or
     if assigned table view has no data set. */
    KPropertySet* findPropertySetForItem(const KDbRecordData& record);

    /*! \return number of the first record containing \a propertyName property equal to \a value.
     This is used e.g. in the Table Designer to find a record by field name.
     If no such record has been found, -1 is returned. */
    int findRecordForPropertyValue(const QByteArray& propertyName, const QVariant& value);

Q_SIGNALS:
    /*! Emitted when a single record is deleted.
     KexiDataAwareObjectInterface::recordDeleted() signal is usually used but when you're using
     KexiDataAwarePropertySet, you never know if currentPropertySet() is updated.
     So use this signal instead. */
    void recordDeleted();

    /*! Emitted when record is inserted.
     Purpose of this signal is similar to recordDeleted() signal. */
    void recordInserted();

    /*! Emitted when the value of @a property is changed in @a set.*/
    void propertyChanged(KPropertySet& set, KProperty& property);

public Q_SLOTS:
    void eraseCurrentPropertySet();

    void clear();

    /*! Inserts \a set property set at \a record position.
     If there was a buffer at this position before, it will be destroyed.
     If \a newOne is true, the property set will be marked as newly created,
     simply by adding "newrecord" property.

     The property set \a set will be owned by this object, so you should not
     delete this property set by hand but call removeCurrentPropertySet()
     or remove(uint) instead.
     Note that property set's parent (QObject::parent()) must be null
     or equal to this KexiDataAwarePropertySet object, otherwise this method
     will fail with a warning.
    */
    void set(uint record, KPropertySet* set, bool newOne = false);

    /*! Deletes a property set at \a record position without removing the record. */
    void eraseAt(uint record);

protected Q_SLOTS:
    /*! Handles table view's data source changes. */
    void slotDataSet(KDbTableViewData *data);

    //! Called on record delete in a tableview.
    void slotRecordDeleted();

    //! Called on multiple records delete in a tableview.
    void slotRecordsDeleted(const QList<int> &records);

    //! Called on \a record insertion in a tableview.
    void slotRecordInserted(KDbRecordData* record, uint pos, bool repaint);

    //! Called on selecting another cell in a tableview.
    void slotCellSelected(int record, int column);

    //! Called on clearing tableview's data: just clears all property sets.
    void slotReloadRequested();

protected:
    void enlargeToFitRecord(uint record);

private:
    class Private;
    Private * const d;
};

#endif

