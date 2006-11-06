/* This file is part of the KDE project
   Copyright (C) 2004, 2006 Jaroslaw Staniek <js@iidea.pl>

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

#include <qptrdict.h>
#include <qintdict.h>
#include <kstaticdeleter.h>

#include <kexidb/indexschema.h>
#include <kexidb/tableschema.h>
#include "kexitableviewdata.h"
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
		KexiCellEditorFactoryPrivate()
		 : items(101)
		 , items_by_type(101, false)
		{
			items.setAutoDelete( true );
			items_by_type.setAutoDelete( false );
		}
		~KexiCellEditorFactoryPrivate() {}

		QString key(uint type, const QString& subType) const
		{
			QString key = QString::number(type);
			if (!subType.isEmpty())
				key += (QString(" ") + subType);
			return key;
		}

		void registerItem( KexiCellEditorFactoryItem& item, uint type, const QString& subType = QString::null )
		{
			if (!items[ &item ])
				items.insert( &item, &item );

			items_by_type.insert( key(type, subType), &item );
		}
		
		KexiCellEditorFactoryItem *findItem(uint type, const QString& subType)
		{
			KexiCellEditorFactoryItem *item = items_by_type[ key(type, subType) ];
			if (item)
				return item;
			item = items_by_type[ key(type, QString::null) ];
			if (item)
				return item;
			return items_by_type[ key( KexiDB::Field::InvalidType, QString::null ) ];
		}

		QPtrDict<KexiCellEditorFactoryItem> items; //!< list of editor factory items (for later destroy)

		QDict<KexiCellEditorFactoryItem> items_by_type; //!< editor factory items accessed by a key
};

static KStaticDeleter<KexiCellEditorFactoryPrivate> KexiCellEditorFactory_deleter;
static KexiCellEditorFactoryPrivate *KexiCellEditorFactory_static = 0;

//============= KexiCellEditorFactory ============

KexiCellEditorFactory::KexiCellEditorFactory()
{
}

KexiCellEditorFactory::~KexiCellEditorFactory()
{
}


// Initializes standard editor cell editor factories
void KexiCellEditorFactory::init()
{
	if (KexiCellEditorFactory_static)
		return;
	KexiCellEditorFactory_deleter.setObject(KexiCellEditorFactory_static, new KexiCellEditorFactoryPrivate());

	KexiCellEditorFactory_static->registerItem( *new KexiBlobEditorFactoryItem(), KexiDB::Field::BLOB );
	KexiCellEditorFactory_static->registerItem( *new KexiDateEditorFactoryItem(), KexiDB::Field::Date );
	KexiCellEditorFactory_static->registerItem( *new KexiTimeEditorFactoryItem(), KexiDB::Field::Time );
	KexiCellEditorFactory_static->registerItem( *new KexiDateTimeEditorFactoryItem(), KexiDB::Field::DateTime );
	KexiCellEditorFactory_static->registerItem( *new KexiComboBoxEditorFactoryItem(), KexiDB::Field::Enum );
	KexiCellEditorFactory_static->registerItem( *new KexiBoolEditorFactoryItem(), KexiDB::Field::Boolean );
	KexiCellEditorFactory_static->registerItem( *new KexiKIconTableEditorFactoryItem(), KexiDB::Field::Text, "KIcon" );
	//default type
	KexiCellEditorFactory_static->registerItem( *new KexiInputEditorFactoryItem(), KexiDB::Field::InvalidType );
}

void KexiCellEditorFactory::registerItem( KexiCellEditorFactoryItem& item, uint type, const QString& subType )
{
	init();
	KexiCellEditorFactory_static->registerItem( item, type, subType );
}

KexiTableEdit* KexiCellEditorFactory::createEditor(KexiTableViewColumn &column, QWidget* parent)
{
	init();
	KexiDB::Field *realField;
	if (column.visibleLookupColumnInfo) {
		realField = column.visibleLookupColumnInfo->field;
	}
	else {
		realField = column.field();
	}

	KexiCellEditorFactoryItem *item = 0;
	if (/*not db-aware case*/column.relatedData() 
		|| (/*db-aware case*/column.field() && column.field()->table() && column.field()->table()->lookupFieldSchema( *column.field() )))
	{
		//--we need to create combo box because of relationship:
		item = KexiCellEditorFactory::item( KexiDB::Field::Enum );
	}
	else {
		item = KexiCellEditorFactory::item( realField->type(), realField->subType() );
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
			if (idx->fields()->findRef(&f)!=-1) {
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

KexiCellEditorFactoryItem* KexiCellEditorFactory::item( uint type, const QString& subType )
{
	init();
	return KexiCellEditorFactory_static->findItem(type, subType);
}

