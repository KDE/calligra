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
#include "kexidatetableedit.h"
#include "kexitimetableedit.h"
#include "kexidatetimetableedit.h"
#include "kexitableedit.h"
#include "kexiinputtableedit.h"
#include "kexicomboboxtableedit.h"
#include "kexiblobtableedit.h"
#include "kexibooltableedit.h"

#include <KDbIndexSchema>
#include <KDbTableSchema>
#include <KDbTableViewData>

#include <QSet>
#include <QHash>

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
        registerItem(*new KexiBlobEditorFactoryItem(), KDbField::BLOB);
        registerItem( *new KexiDateEditorFactoryItem(), KDbField::Date);
        registerItem( *new KexiTimeEditorFactoryItem(), KDbField::Time);
        registerItem( *new KexiDateTimeEditorFactoryItem(), KDbField::DateTime);
        registerItem(*new KexiComboBoxEditorFactoryItem(), KDbField::Enum);
        registerItem(*new KexiBoolEditorFactoryItem(), KDbField::Boolean);
        registerItem(*new KexiKIconTableEditorFactoryItem(), KDbField::Text, "QIcon");
        //default type
        registerItem(*new KexiInputEditorFactoryItem(), KDbField::InvalidType);
    }
    ~KexiCellEditorFactoryPrivate() {
        qDeleteAll(items);
    }

    QString key(int type, const QString& subType) const {
        QString key = QString::number(type);
        if (!subType.isEmpty())
            key += (QString(" ") + subType);
        return key;
    }

    void registerItem(KexiCellEditorFactoryItem& item, int type, const QString& subType = QString()) {
        if (!items.contains(&item))
            items.insert(&item);

        items_by_type.insert(key(type, subType), &item);
    }

    KexiCellEditorFactoryItem *findItem(int type, const QString& subType) {
        KexiCellEditorFactoryItem *item = items_by_type.value(key(type, subType));
        if (item)
            return item;
        item = items_by_type.value(key(type, QString()));
        if (item)
            return item;
        return items_by_type.value(key(KDbField::InvalidType, QString()));
    }

    QSet<KexiCellEditorFactoryItem*> items; //!< list of editor factory items (for later destroy)

    QHash<QString, KexiCellEditorFactoryItem*> items_by_type; //!< editor factory items accessed by a key
};

Q_GLOBAL_STATIC(KexiCellEditorFactoryPrivate, KexiCellEditorFactory_static)

//============= KexiCellEditorFactory ============

KexiCellEditorFactory::KexiCellEditorFactory()
{
}

KexiCellEditorFactory::~KexiCellEditorFactory()
{
}

void KexiCellEditorFactory::registerItem(KexiCellEditorFactoryItem& item, int type, const QString& subType)
{
    KexiCellEditorFactory_static->registerItem(item, type, subType);
}

static bool hasEnumType(const KDbTableViewColumn &column)
{
    /*not db-aware case*/
    if (column.relatedData())
        return true;
    /*db-aware case*/
    if (!column.field() || !column.field()->table())
        return false;
    KDbLookupFieldSchema *lookupFieldSchema = column.field()->table()->lookupFieldSchema(*column.field());
    if (!lookupFieldSchema)
        return false;
    if (lookupFieldSchema->recordSource().name().isEmpty())
        return false;
    return true;
}

KexiTableEdit* KexiCellEditorFactory::createEditor(KDbTableViewColumn &column, QWidget* parent)
{
    KDbField *realField;
    if (column.visibleLookupColumnInfo()) {
        realField = column.visibleLookupColumnInfo()->field;
    } else {
        realField = column.field();
    }

    KexiCellEditorFactoryItem *item = 0;

    if (hasEnumType(column)) {
        //--we need to create combo box because of relationship:
        item = KexiCellEditorFactory::item(KDbField::Enum);
    } else {
        item = KexiCellEditorFactory::item(realField->type(), realField->subType());
    }

//! @todo later
#if 0
    //--check if we need to create combo box because of relationship:
    //WARNING: it's assumed that indices are one-field long
    KDbTableSchema *table = f.table();
    if (table) {
        //find index that contain this field
        KDbIndexSchema::ListIterator it = table->indicesIterator();
        for (;it.current();++it) {
            KDbIndexSchema *idx = it.current();
            if (idx->fields()->contains(&f)) {
                //find details-side rel. for this index
                KDbRelationship *rel = idx->detailsRelationships()->first();
                if (rel) {

                }
            }
        }
    }
#endif
    return item->createEditor(column, parent);
}

KexiCellEditorFactoryItem* KexiCellEditorFactory::item(int type, const QString& subType)
{
    return KexiCellEditorFactory_static->findItem(type, subType);
}
