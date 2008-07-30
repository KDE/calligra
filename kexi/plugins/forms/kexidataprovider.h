/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIFORMDATAPROVIDER_H
#define KEXIFORMDATAPROVIDER_H

#include "kexiformdataiteminterface.h"
#include <QSet>
#include <QList>

namespace KexiDB {
  class QuerySchema;
  class RecordData;
}

//! @short The KexiFormDataProvider class is a data provider for Kexi Forms
/*! This provider collects data-aware widgets using setMainWidget().
 Then, usedDataSources() unique list of required field names is available.
 On every call of fillDataItems() method, the provider will fill data items 
 with appropriate data from a database cursor.

 Field names are collected effectively, so eg. having widgets using data sources:
 ("name", "surname", "surname", "name") - "name" and "surname" repeated - will only 
 return ("name", "surname") list, so the cursor's query can be simplified 
 and thus more effective.
*/
class KEXIFORMUTILS_EXPORT KexiFormDataProvider : public KexiDataItemChangesListener
{
  public:
    KexiFormDataProvider();
    virtual ~KexiFormDataProvider();

    /*! sets \a mainWidget to be a main widget for this data provider.
     Also find widgets whose will work as data items 
     (all of them must implement KexiFormDataItemInterface), so these could be 
     filled with data on demand. */
    void setMainDataSourceWidget(QWidget* mainWidget);

    QStringList usedDataSources() const { return m_usedDataSources; }

    //unused Q3PtrList<KexiFormDataItemInterface>& dataItems() { return m_dataItems; }

    /*! Fills data items with appropriate data fetched from \a cursor. 
     \a newRowEditing == true means that we are at new (not yet inserted) database row. */
    void fillDataItems(KexiDB::RecordData& record, bool cursorAtNewRow);

    /*! Implementation for KexiDataItemChangesListener. 
     Reaction for change of \a item. Does nothing here. */
    virtual void valueChanged(KexiDataItemInterface* item);

    /*! Implementation for KexiDataItemChangesListener. 
     Implement this to return information whether we're currently at new row or now.
     This can be used e.g. by data-aware widgets to determine if "(autonumber)" 
     label should be displayed. Returns false here. */
    virtual bool cursorAtNewRow() const;

    /*! Invalidates data sources collected by this provided.
     \a invalidSources is the set of data sources that should 
     be omitted for fillDataItems(). 
     Used by KexiFormView::initDataSource(). */
    void invalidateDataSources( const QSet<QString>& invalidSources, 
      KexiDB::QuerySchema* query = 0 );

    /*! Fills the same data provided by \a value to every data item (other than \a item) 
     having the same data source as \a item. This method is called immediately when 
     \a value is changed, so duplicated data items are quickly updated. */
    void fillDuplicatedDataItems(KexiFormDataItemInterface* item, const QVariant& value);

  protected:
    QWidget *m_mainWidget;
    QSet<KexiDB::Field*> *m_duplicatedItems;
    typedef QMap<KexiFormDataItemInterface*,uint> KexiFormDataItemInterfaceToIntMap;
    QList<KexiFormDataItemInterface*> m_dataItems;
    QStringList m_usedDataSources;
    KexiFormDataItemInterfaceToIntMap m_fieldNumbersForDataItems;
    bool m_disableFillDuplicatedDataItems : 1;
};

#endif
