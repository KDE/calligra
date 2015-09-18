/* This file is part of the KDE project
   Copyright (C) 2005-2006 Jarosław Staniek <staniek@kde.org>

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

#include "kexidataprovider.h"
#include <kexiutils/utils.h>
#include <kexi_global.h>

#include <KDbTableViewData>
#include <KDbQuerySchema>

#include <KLocalizedString>

#include <QWidget>
#include <QDebug>

KexiFormDataProvider::KexiFormDataProvider()
        : KexiDataItemChangesListener()
        , m_mainWidget(0)
        , m_duplicatedItems(0)
        , m_disableFillDuplicatedDataItems(false)
{
}

KexiFormDataProvider::~KexiFormDataProvider()
{
    delete m_duplicatedItems;
}

void KexiFormDataProvider::setMainDataSourceWidget(QWidget* mainWidget)
{
    m_mainWidget = mainWidget;
    m_dataItems.clear();
    m_usedDataSources.clear();
    m_fieldNumbersForDataItems.clear();
    if (!m_mainWidget)
        return;

    //find widgets whose will work as data items
    const QList<QWidget*> widgets(m_mainWidget->findChildren<QWidget*>());
    QSet<QString> tmpSources;
    foreach(QWidget *widget, widgets) {
        KexiFormDataItemInterface* const formDataItem = dynamic_cast<KexiFormDataItemInterface*>(widget);
        if (!formDataItem)
            continue;
        if (formDataItem->parentDataItemInterface()) //item with parent interface: collect parent instead...
            continue;
        QString dataSource(formDataItem->dataSource().toLower());
        if (dataSource.isEmpty())
            continue;
        qDebug() << widget->objectName();
        m_dataItems.append(formDataItem);
        formDataItem->installListener(this);
        tmpSources.insert(dataSource);
    }
    //now we've got a set (unique list) of field names in tmpSources
    //remember it in m_usedDataSources
    foreach(const QString& source, tmpSources) {
        m_usedDataSources += source;
    }
}

void KexiFormDataProvider::fillDataItems(KDbRecordData *data, bool cursorAtNewRecord)
{
    Q_ASSERT(data);
    qDebug() << "record.count=" << data->count()
             << "\nRECORD=" << *data;
    for (KexiFormDataItemInterfaceToIntMap::ConstIterator it
            = m_fieldNumbersForDataItems.constBegin();
            it != m_fieldNumbersForDataItems.constEnd(); ++it) {
        KexiFormDataItemInterface *itemIface = it.key();
        if (!itemIface->columnInfo()) {
            qDebug() << "itemIface->columnInfo() == 0";
            continue;
        }
        //1. Is this a value with a combo box (lookup)?
        int indexForVisibleLookupValue = itemIface->columnInfo()->indexForVisibleLookupValue();
        if (indexForVisibleLookupValue<0 && indexForVisibleLookupValue >= data->count()) //sanity
            indexForVisibleLookupValue = -1; //no
        const QVariant value(data->at(it.value()));
        QVariant visibleLookupValue;
        if (indexForVisibleLookupValue != -1 && (int)data->count() > indexForVisibleLookupValue)
            visibleLookupValue = data->at(indexForVisibleLookupValue);
            qDebug() << "fill data of '" << itemIface->dataSource() <<  "' at idx=" << it.value()
            << " data=" << value
            << (indexForVisibleLookupValue != -1
                 ? QString(" SPECIAL: indexForVisibleLookupValue=%1 visibleValue=%2")
                 .arg(indexForVisibleLookupValue).arg(visibleLookupValue.toString())
                 : QString());
        const bool displayDefaultValue = cursorAtNewRecord && (value.isNull() && visibleLookupValue.isNull())
                                         && !itemIface->columnInfo()->field->defaultValue().isNull()
                                         && !itemIface->columnInfo()->field->isAutoIncrement(); //no value to set but there is default value defined
        itemIface->setValue(
            displayDefaultValue ? itemIface->columnInfo()->field->defaultValue() : value,
            QVariant(), /*add*/
            /*!remove old*/false,
            indexForVisibleLookupValue == -1 ? 0 : &visibleLookupValue //pass visible value if available
        );
        // now disable/enable "display default value" if needed (do it after setValue(), before setValue() turns it off)
        if (itemIface->hasDisplayedDefaultValue() != displayDefaultValue)
            itemIface->setDisplayDefaultValue(dynamic_cast<QWidget*>(itemIface), displayDefaultValue);
    }
}

void KexiFormDataProvider::fillDuplicatedDataItems(
    KexiFormDataItemInterface* item, const QVariant& value)
{
    if (m_disableFillDuplicatedDataItems)
        return;
    if (!m_duplicatedItems) {
        //build (once) a set of duplicated data items (having the same fields assigned)
        //so we can later check if an item is duplicated with a cost of o(1)
        QHash<KDbField*, int> tmpDuplicatedItems;
        QHash<KDbField*, int>::const_iterator it_dup;
        foreach(KexiFormDataItemInterface *dataItemIface, m_dataItems) {
            if (!dataItemIface->columnInfo() || !dataItemIface->columnInfo()->field)
                continue;
            qDebug() << " ** " << dataItemIface->columnInfo()->field->name();
            it_dup = tmpDuplicatedItems.constFind(dataItemIface->columnInfo()->field);
            int count;
            if (it_dup == tmpDuplicatedItems.constEnd())
                count = 0;
            else
                count = it_dup.value();
            tmpDuplicatedItems.insert(dataItemIface->columnInfo()->field, ++count);
        }
        m_duplicatedItems = new QSet<KDbField*>();
        for (it_dup = tmpDuplicatedItems.constBegin(); it_dup != tmpDuplicatedItems.constEnd(); ++it_dup) {
            if (it_dup.value() > 1) {
                m_duplicatedItems->insert(it_dup.key());
                qDebug() << "duplicated item: " << static_cast<KDbField*>(it_dup.key())->name()
                    << " (" << it_dup.value() << " times)";
            }
        }
    }
    if (item->columnInfo() && m_duplicatedItems->contains(item->columnInfo()->field)) {
        foreach(KexiFormDataItemInterface *dataItemIface, m_dataItems) {
            if (dataItemIface != item && item->columnInfo()->field == dataItemIface->columnInfo()->field) {
                qDebug() << "- setting a copy of value for item '"
                    << dynamic_cast<QObject*>(dataItemIface)->objectName() << "' == " << value;
                dataItemIface->setValue(value);
            }
        }
    }
}

void KexiFormDataProvider::valueChanged(KexiDataItemInterface* item)
{
    Q_UNUSED(item);
}

bool KexiFormDataProvider::cursorAtNewRecord() const
{
    return false;
}

void KexiFormDataProvider::invalidateDataSources(const QSet<QString>& invalidSources,
        KDbQuerySchema* query)
{
    //fill m_fieldNumbersForDataItems mapping from data item to field number
    //(needed for fillDataItems)
    KDbQueryColumnInfo::Vector fieldsExpanded;
// int dataFieldsCount; // == fieldsExpanded.count() if query is available or else == m_dataItems.count()

    if (query) {
        fieldsExpanded = query->fieldsExpanded(KDbQuerySchema::WithInternalFields);
//  dataFieldsCount = fieldsExpanded.count();
        QHash<KDbQueryColumnInfo*, int> columnsOrder(query->columnsOrder());
        for (QHash<KDbQueryColumnInfo*, int>::const_iterator it
                = columnsOrder.constBegin(); it != columnsOrder.constEnd(); ++it) {
            qDebug() << "query->columnsOrder()[ " << it.key()->field->name() << " ] = "
                << it.value();
        }
        foreach(KexiFormDataItemInterface *item, m_dataItems) {
            KDbQueryColumnInfo* ci = query->columnInfo(item->dataSource());
            int index = ci ? columnsOrder[ ci ] : -1;
            qDebug() << "query->columnsOrder()[ " << (ci ? ci->field->name() : QString()) << " ] = " << index
                << " (dataSource: " << item->dataSource() << ", name="
                << dynamic_cast<QObject*>(item)->objectName() << ")";
            if (index != -1 && !m_fieldNumbersForDataItems[ item ])
                m_fieldNumbersForDataItems.insert(item, index);
            //! @todo
            //WRONG: not only used data sources can be fetched!
            //   m_fieldNumbersForDataItems.insert( item,
            //    m_usedDataSources.findIndex(item->dataSource().toLower()) );
        }
    }

    //update data sources set (some of them may be removed)
    QSet<QString> tmpUsedDataSources;

    if (query) {
        qDebug() << *query;
    }
    m_disableFillDuplicatedDataItems = true; // temporary disable fillDuplicatedDataItems()
                                             // because setColumnInfo() can activate it
    for (QList<KexiFormDataItemInterface*>::iterator it(m_dataItems.begin());
            it != m_dataItems.end();) {
        KexiFormDataItemInterface *item = *it;
        Q_ASSERT(item);

        if (invalidSources.contains(item->dataSource().toLower())) {
            item->setInvalidState(QString::fromLatin1("#%1?").arg(xi18n("NAME")));
            it = m_dataItems.erase(it);
            continue;
        }
        int fieldNumber = m_fieldNumbersForDataItems[ item ];
        if (query) {
            KDbQueryColumnInfo *ci = fieldsExpanded[fieldNumber];
            item->setColumnInfo(ci);
            qDebug() << "- item=" << dynamic_cast<QObject*>(item)->objectName()
                << " dataSource=" << item->dataSource()
                << " field=" << ci->field->name();
            const int indexForVisibleLookupValue = ci->indexForVisibleLookupValue();
            if (-1 != indexForVisibleLookupValue && indexForVisibleLookupValue < (int)fieldsExpanded.count()) {
                //there's lookup column defined: set visible column as well
                KDbQueryColumnInfo *visibleColumnInfo = fieldsExpanded[ indexForVisibleLookupValue ];
                if (visibleColumnInfo) {
                    item->setVisibleColumnInfo(visibleColumnInfo);

                    if (item->isComboBox() && m_mainWidget && item->internalEditor()) {
                        // m_mainWidget (dbform) should filter the (just created using setVisibleColumnInfo())
                        // combo box' internal editor (actually, only if the combo is in 'editable' mode)
                        item->internalEditor()->installEventFilter(m_mainWidget);
                    }

                    qDebug() << "ALSO SET visibleColumn=" << *visibleColumnInfo
                        << "\n at position " << indexForVisibleLookupValue;
                }
            }
        }
        tmpUsedDataSources.insert(item->dataSource().toLower());
        ++it;
    }
    m_disableFillDuplicatedDataItems = false;
    m_usedDataSources.clear();
    foreach(const QString& source, tmpUsedDataSources) {
        m_usedDataSources += source;
    }
}
