/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003   Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003-2007 Jarosław Staniek <staniek@kde.org>

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

   Original Author:  Till Busch <till@bux.at>
   Original Project: buX (www.bux.at)
*/

#ifndef KEXITABLEVIEWDATA_H
#define KEXITABLEVIEWDATA_H

#include <QVariant>
#include <QVector>

#include "KexiTableViewColumn.h"
#include <kexiutils/utils.h>
#include <kexidb/RecordData.h>

namespace KexiDB
{
class RowEditBuffer;
class Cursor;
}

typedef KexiUtils::AutodeletedList<KexiDB::RecordData*> KexiTableViewDataBase;

/*! Reimplements QPtrList to allow configurable sorting and more.
  Original author: Till Busch.
  Reimplemented by Jarosław Staniek.
*/
class KEXIDATATABLE_EXPORT KexiTableViewData : public QObject, protected KexiTableViewDataBase
{
    Q_OBJECT

public:
    typedef KexiTableViewDataBase::ConstIterator Iterator;

    //! not db-aware version
    KexiTableViewData();

    //! db-aware version
    KexiTableViewData(KexiDB::Cursor *c);

//TODO: make this more generic: allow to add more columns!
    /*! Defines two-column table usually used with comboboxes.
     First column is invisible and contains key values.
     Second column and contains user-visible value.
     @param keys a list of keys
     @param values a list of text values (must be of the same length as keys list)
     @param keyType a type for keys
     @param valueType a type for values
    */
    KexiTableViewData(
        const QList<QVariant> &keys, const QList<QVariant> &values,
        KexiDB::Field::Type keyType = KexiDB::Field::Text,
        KexiDB::Field::Type valueType = KexiDB::Field::Text);

    /*! Like above constructor, but keys and values are not provided.
     You can do this later by calling append(KexiDB::RecordData*) method.
     (KexiDB::RecordData object must have exactly two columns)
    */
    KexiTableViewData(KexiDB::Field::Type keyType, KexiDB::Field::Type valueType);

    virtual ~KexiTableViewData();
//js void setSorting(int key, bool order=true, short type=1);

    /*! Preloads all rows provided by cursor (only for db-aware version). */
    bool preloadAllRows();

    /*! Sets sorting for \a column. If \a column is -1, sorting is disabled. */
    void setSorting(int column, bool ascending = true);

    /*! \return the column number by which the data is sorted,
     or -1 if sorting is disabled. In this case sortingOrder() will return 0.
     Initial sorted column number for data after instantiating object is -1. */
    int sortedColumn() const;

    /*! \return 1 if ascending sort order is set, -1 id descending sort order is set,
     or 0 if no sorting is set. This is independent of whether data is sorted now.
     Initial sorting for data after instantiating object is 0. */
    int sortingOrder() const;

    //! Sorts this data using previously set order.
    void sort();

    /*! Adds column \a col.
     Warning: \a col will be owned by this object, and deleted on its destruction. */
    void addColumn(KexiTableViewColumn* col);

    int globalColumnID(int visibleID) const;
    int visibleColumnID(int globalID) const;

    /*virtual?*/
    /*! \return true if this db-aware data set. */
    bool isDBAware() const;

    /*! For db-aware data set only: table name is returned;
     equivalent to cursor()->query()->parentTable()->name(). */
    QString dbTableName() const;

    KexiDB::Cursor* cursor() const;

    inline uint columnsCount() const {
        return m_columns.count();
    }

    inline KexiTableViewColumn* column(uint c) {
        return m_columns.at(c);
    }

    /*! \return columns information */
    inline KexiTableViewColumn::List& columns() {
        return m_columns;
    }

    /*! \return true if data is not editable. Can be set using setReadOnly()
     but it's still true if database cursor returned by cursor()
     is not 0 and has read-only connection. */
    virtual bool isReadOnly() const;

    /*! Sets readOnly flag for this data.
     If \a set is true, insertingEnabled flag will be cleared automatically.
     \sa isInsertingEnabled() */
    virtual void setReadOnly(bool set);

    /*! \return true if data inserting is enabled (the default). */
    virtual bool isInsertingEnabled() const;

    /*! Sets insertingEnabled flag. If true, empty row is available
     If \a set is true, read-only flag will be cleared automatically.
     \sa setReadOnly() */
    virtual void setInsertingEnabled(bool set);

    /*! Clears and initializes internal row edit buffer for incoming editing.
     Creates buffer using KexiDB::RowEditBuffer(false) (false means not db-aware type)
     if our data is not db-aware,
     or db-aware buffer if data is db-aware (isDBAware()==true).
     \sa KexiDB::RowEditBuffer
    */
    void clearRowEditBuffer();

    /*! Updates internal row edit buffer: currently edited column \a col (number \a colnum)
     has now assigned new value of \a newval.
     Uses column's caption to address the column in buffer
     if the buffer is of simple type, or db-aware buffer if (isDBAware()==true).
     (then fields are addressed with KexiDB::Field, instead of caption strings).
     If \a allowSignals is true (the default), aboutToChangeCell() signal is emitted.
     \a visibleValueForLookupField allows to pass visible value (usually a text)
     for a lookup field (only reasonable if col->visibleLookupColumnInfo != 0).
     Note that \a newval may be changed in aboutToChangeCell() signal handler.
     \sa KexiDB::RowEditBuffer */
    bool updateRowEditBufferRef(KexiDB::RecordData *record,
                                int colnum, KexiTableViewColumn* col, QVariant& newval, bool allowSignals = true,
                                QVariant *visibleValueForLookupField = 0);

    /*! Added for convenience. Like above but \a newval is passed by value. */
    inline bool updateRowEditBuffer(KexiDB::RecordData *record, int colnum, KexiTableViewColumn* col,
                                    QVariant newval, bool allowSignals = true) {
        QVariant newv(newval);
        return updateRowEditBufferRef(record, colnum, col, newv, allowSignals);
    }

    /*! Added for convenience. Like above but it's assumed that \a record record's columns are ordered
     like in table view, not like in form view. Don't use this with form views. */
    inline bool updateRowEditBuffer(KexiDB::RecordData *record, int colnum,
                                    QVariant newval, bool allowSignals = true) {
        KexiTableViewColumn* col = m_columns.at(colnum);
        return col ? updateRowEditBufferRef(record, colnum, col, newval, allowSignals) : false;
    }

    //! \return row edit buffer for currently edited record. Can be 0 or empty.
    KexiDB::RowEditBuffer* rowEditBuffer() const;

    /*! \return last operation's result information (always not null). */
    const KexiDB::ResultInfo& result() const;

    bool saveRowChanges(KexiDB::RecordData& record, bool repaint = false);

    bool saveNewRow(KexiDB::RecordData& record, bool repaint = false);

    bool deleteRow(KexiDB::RecordData& record, bool repaint = false);

    /*! Deletes rows (by number) passed with \a rowsToDelete.
     Currently, this method is only for non data-aware tables. */
    void deleteRows(const QList<int> &rowsToDelete, bool repaint = false);

    /*! Deletes all rows. Works either for db-aware and non db-aware tables.
     Column's definition is not changed.
     For db-aware version, all rows are removed from a database.
     Row-edit buffer is cleared.

     If \a repaint is true, reloadRequested() signal
     is emitted after deleting (if at least one row was deleted),
     so presenters can repaint their contents.

     \return true on success. */
    virtual bool deleteAllRows(bool repaint = false);

    /*! @internal method, used mostly by specialized classes like KexiTableView.
     Clears internal row structures. Row-edit buffer is cleared.
     Does not touch data @ database backend.
     Use deleteAllRows() to safely delete all rows. */
    virtual void clearInternal();

    /*! Inserts new \a record at index \a index.
     \a record will be owned by this data object.
     Note: Reasonable only for not not-db-aware version. */
    void insertRow(KexiDB::RecordData& record, uint index, bool repaint = false);

    /*TODO: add this as well?
      void insertRow(KexiDB::RecordData& record, KexiDB::RecordData& aboveRecord); */

    //! \return index of autoincremented column. The result is cached.
//! \todo what about multiple autoinc columns?
//! \todo what about changing column order?
    int autoIncrementedColumn();

    //! Emits reloadRequested() signal to reload presenters.
    void reload() {
        emit reloadRequested();
    }

    inline KexiDB::RecordData* at(uint index) {
        return KexiTableViewDataBase::at(index);
    }
    inline virtual uint count() const {
        return KexiTableViewDataBase::count();
    }
    inline bool isEmpty() const {
        return KexiTableViewDataBase::isEmpty();
    }
    inline KexiDB::RecordData* first() {
        return KexiTableViewDataBase::first();
    }
    inline KexiDB::RecordData* last() {
        return KexiTableViewDataBase::last();
    }
//Qt 4 inline int findRef( const KexiDB::RecordData* record ) { return KexiTableViewDataBase::findRef(record); }
    inline int indexOf(KexiDB::RecordData* record, int from = 0) const {
        return KexiTableViewDataBase::indexOf(record, from);
    }
    inline void removeFirst() {
        KexiTableViewDataBase::removeFirst();
    }
    inline void removeLast() {
        KexiTableViewDataBase::removeLast();
    }
    inline void append(KexiDB::RecordData* record) {
        KexiTableViewDataBase::append(record);
    }
    inline void prepend(KexiDB::RecordData* record) {
        KexiTableViewDataBase::prepend(record);
    }
// inline Iterator iterator() { return Iterator(*this); }
// inline Iterator* createIterator() { return new Iterator(*this); }
    inline KexiTableViewData::Iterator constBegin() const {
        return KexiTableViewDataBase::constBegin();
    }
    inline KexiTableViewData::Iterator constEnd() const {
        return KexiTableViewDataBase::constEnd();
    }

    /*! \return true if ROWID information is stored within every row.
     Only reasonable for db-aware version. ROWID information is available
     if DriverBehaviour::ROW_ID_FIELD_RETURNS_LAST_AUTOINCREMENTED_VALUE == false
     for a KexiDB database driver and a table has no primary key defined.
     Phisically, ROWID information is stored after last KexiDB::RecordData's element,
     so every KexiDB::RecordData's length is expanded by one. */
    bool containsROWIDInfo() const;

    inline KexiDB::RecordData* createItem() const {
        return new KexiDB::RecordData(m_itemSize);
    }

public slots:
    //! @internal The same as QObject::deleteLater() but also sets smart pointer m_cursor to 0 to avoid crashes...
    void deleteLater();

signals:
    void destroying();

    /*! Emitted before change of the single, currently edited cell.
     Connect this signal to your slot and set \a result->success to false
     to disallow this change. You can also change \a newValue to other value,
     or change other columns in \a record. */
    void aboutToChangeCell(KexiDB::RecordData *record, int colnum, QVariant& newValue,
                           KexiDB::ResultInfo* result);

    /*! Emitted before inserting of a new, current row.
     Connect this signal to your slot and set \a result->success to false
     to disallow this inserting. You can also change columns in \a record. */
    void aboutToInsertRow(KexiDB::RecordData *record, KexiDB::ResultInfo* result, bool repaint);

    /*! Emitted before changing of an edited, current row.
     Connect this signal to your slot and set \a result->success to false
     to disallow this change. You can also change columns in \a record. */
    void aboutToUpdateRow(KexiDB::RecordData *record, KexiDB::RowEditBuffer* buffer,
                          KexiDB::ResultInfo* result);

    void rowUpdated(KexiDB::RecordData*); //!< Current row has been updated

    void rowInserted(KexiDB::RecordData*, bool repaint); //!< A row has been inserted

    //! A row has been inserted at \a index position (not db-aware data only)
    void rowInserted(KexiDB::RecordData*, uint index, bool repaint);

    /*! Emitted before deleting of a current row.
     Connect this signal to your slot and set \a result->success to false
     to disallow this deleting. */
    void aboutToDeleteRow(KexiDB::RecordData& record, KexiDB::ResultInfo* result, bool repaint);

    //! Current row has been deleted
    void rowDeleted();

    //! Rows have been deleted
    void rowsDeleted(const QList<int> &rowsToDelete);

    //! Displayed data needs to be reloaded in all presenters.
    void reloadRequested();

    void rowRepaintRequested(KexiDB::RecordData&);

private:
    void init();
    void init(
        const QList<QVariant> &keys, const QList<QVariant> &values,
        KexiDB::Field::Type keyType, KexiDB::Field::Type valueType);

    /* Qt4
      virtual int compareItems(Item item1, Item item2);
      int cmpStr(Item item1, Item item2);
      int cmpInt(Item item1, Item item2);
      int cmpUInt(Item item1, Item item2);
      int cmpLongLong(Item item1, Item item2);
      int cmpULongLong(Item item1, Item item2);
      int cmpDouble(Item item1, Item item2);
      int cmpDate(Item item1, Item item2);
      int cmpDateTime(Item item1, Item item2);
      int cmpTime(Item item1, Item item2);

      //! Compare function for BLOB data (QByteArray). Uses size as the weight.
      int cmpBLOB(Item item1, Item item2);*/

    //! @internal for saveRowChanges() and saveNewRow()
    bool saveRow(KexiDB::RecordData& record, bool insert, bool repaint);

    //! Number of physical columns
    int m_itemSize;

    /*! Columns information */
    KexiTableViewColumn::List m_columns;

    //! Temporary, used in compare functions like cmpInt(), cmpString()
    //! to avoid memory allocations.
//moved QVariant m_leftTmp, m_rightTmp;

    class Private;
    Private * const d;
};

#endif
