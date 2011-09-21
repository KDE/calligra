/* This file is part of the KDE project
   Copyright (C) 2004-2007 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and,or
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

#include "kexicelleditorfactory.h"

#include <QSet>
#include <QHash>
#include <KGlobal>

#include <kexidb/indexschema.h>
#include <kexidb/tableschema.h>
#include <widget/dataviewcommon/kexitableviewdata.h>
#include "kexidatetableedit.h"
#include "kexitimetableedit.h"
#include "kexidatetimetableedit.h"
#include "kexitableedit.h"
#include "kexiinputtableedit.h"
#include "kexicomboboxtableedit.h"
#include "kexiblobtableedit.h"
#include "kexibooltableedit.h"

//============= KexiCellEditorFactoryItem ============

KexiCellEditorFactoryItem::KexiCellEditorFactoryItem()
{
}

KexiCellEditorFactoryItem::~KexiCellEditorFactoryItem()
{
}

//============= KexiCellEditorFactoryPrivate ============

//! @internal
class KexiCellEditorFactoryPrivate
{
public:
    KexiCellEditorFactoryPrivate() {
        // Initialize standard editor cell editor factories
        registerItem(*new KexiBlobEditorFactoryItem(), KexiDB::Field::BLOB);
        registerItem( *new KexiDateEditorFactoryItem(), KexiDB::Field::Date);
        registerItem( *new KexiTimeEditorFactoryItem(), KexiDB::Field::Time);
        registerItem( *new KexiDateTimeEditorFactoryItem(), KexiDB::Field::DateTime);
        registerItem(*new KexiComboBoxEditorFactoryItem(), KexiDB::Field::Enum);
        registerItem(*new KexiBoolEditorFactoryItem(), KexiDB::Field::Boolean);
        registerItem(*new KexiKIconTableEditorFactoryItem(), KexiDB::Field::Text, "KIcon");
        //default type
        registerItem(*new KexiInputEditorFactoryItem(), KexiDB::Field::InvalidType);
    }
    ~KexiCellEditorFactoryPrivate() {
        qDeleteAll(items);
    }

    QString key(uint type, const QString& subType) const {
        QString key = QString::number(type);
        if (!subType.isEmpty())
            key += (QString(" ") + subType);
        return key;
    }

    void registerItem(KexiCellEditorFactoryItem& item, uint type, const QString& subType = QString()) {
        if (!items.contains(&item))
            items.insert(&item);

        items_by_type.insert(key(type, subType), &item);
    }

    KexiCellEditorFactoryItem *findItem(uint type, const QString& subType) {
        KexiCellEditorFactoryItem *item = items_by_type.value(key(type, subType));
        if (item)
            return item;
        item = items_by_type.value(key(type, QString()));
        if (item)
            return item;
        return items_by_type.value(key(KexiDB::Field::InvalidType, QString()));
    }

    QSet<KexiCellEditorFactoryItem*> items; //!< list of editor factory items (for later destroy)

    QHash<QString, KexiCellEditorFactoryItem*> items_by_type; //!< editor factory items accessed by a key
};

K_GLOBAL_STATIC(KexiCellEditorFactoryPrivate, KexiCellEditorFactory_static)

//============= KexiCellEditorFactory ============

KexiCellEditorFactory::KexiCellEditorFactory()
{
}

KexiCellEditorFactory::~KexiCellEditorFactory()
{
}

void KexiCellEditorFactory::registerItem(KexiCellEditorFactoryItem& item, uint type, const QString& subType)
{
    KexiCellEditorFactory_static->registerItem(item, type, subType);
}

static bool hasEnumType(const KexiTableViewColumn &column)
{
    /*not db-aware case*/
    if (column.relatedData())
        return true;
    /*db-aware case*/
    if (!column.field() || !column.field()->table())
        return false;
    KexiDB::LookupFieldSchema *lookupFieldSchema = column.field()->table()->lookupFieldSchema(*column.field());
    if (!lookupFieldSchema)
        return false;
    if (lookupFieldSchema->rowSource().name().isEmpty())
        return false;
    return true;
}

KexiTableEdit* KexiCellEditorFactory::createEditor(KexiTableViewColumn &column, QWidget* parent)
{
    KexiDB::Field *realField;
    if (column.visibleLookupColumnInfo()) {
        realField = column.visibleLookupColumnInfo()->field;
    } else {
        realField = column.field();
    }

    KexiCellEditorFactoryItem *item = 0;

    if (hasEnumType(column)) {
        //--we need to create combo box because of relationship:
        item = KexiCellEditorFactory::item(KexiDB::Field::Enum);
    } else {
        item = KexiCellEditorFactory::item(realField->type(), realField->subType());
    }

#if 0 //js: TODO LATER
    //--check if we need to create combo box because of relationship:
    //WARNING: it's assumed that indices are one-field long
    KexiDB::TableSchema *table = f.table();
    if (table) {
        //find index that contain this field
        KexiDB::IndexSchema::ListIterator it = table->indicesIterator();
        for (;it.current();++it) {
            KexiDB::IndexSchema *idx = it.current();
            if (idx->fields()->contains(&f)) {
                //find details-side rel. for this index
                KexiDB::Relationship *rel = idx->detailsRelationships()->first();
                if (rel) {

                }
            }
        }
    }
#endif

    return item->createEditor(column, parent);
}

KexiCellEditorFactoryItem* KexiCellEditorFactory::item(uint type, const QString& subType)
{
    return KexiCellEditorFactory_static->findItem(type, subType);
}

