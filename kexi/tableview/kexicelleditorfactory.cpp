/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kexicelleditorfactory.h"

#include <qptrdict.h>
#include <qintdict.h>

//============= KexiCellEditorFactoryItem ============

KexiCellEditorFactoryItem::KexiCellEditorFactoryItem()
{
}

KexiCellEditorFactoryItem::~KexiCellEditorFactoryItem()
{
}

int KexiCellEditorFactoryItem::widthForValue( QVariant &val, QFontMetrics &fm )
{
	return fm.width( val.toString() );
}

//============= KexiCellEditorFactoryPrivate ============

class KexiCellEditorFactoryPrivate
{
	public:
		KexiCellEditorFactoryPrivate()
		{
			items.setAutoDelete( true );
			items_by_type.setAutoDelete( false );
		}
		~KexiCellEditorFactoryPrivate() {}

		void registerItem( uint type, KexiCellEditorFactoryItem& item )
		{
			if (!items[ &item ])
				items.insert( &item, &item );

			items_by_type.insert( type, &item );
		}

		QPtrDict<KexiCellEditorFactoryItem> items; //!< list of editor factory items (for later destroy)

		QIntDict<KexiCellEditorFactoryItem> items_by_type; //!< editor factory items accessed by a type (from KexiDB::Field::Type)
};

KexiCellEditorFactoryPrivate static_KexiCellEditorFactory;


//============= KexiCellEditorFactory ============

KexiCellEditorFactory::KexiCellEditorFactory()
{
}

KexiCellEditorFactory::~KexiCellEditorFactory()
{
}

void KexiCellEditorFactory::registerItem( uint type, KexiCellEditorFactoryItem& item )
{
	static_KexiCellEditorFactory.registerItem( type, item );
}

KexiTableEdit* KexiCellEditorFactory::createEditor(KexiDB::Field &f, QWidget* parent)
{
	KexiCellEditorFactoryItem *item = KexiCellEditorFactory::item( f.type() );
	return item->createEditor(f, parent);
}

KexiCellEditorFactoryItem* KexiCellEditorFactory::item( uint type )
{
	KexiCellEditorFactoryItem *item = static_KexiCellEditorFactory.items_by_type[ type ];
	if (!item) { //try the default
		item = static_KexiCellEditorFactory.items_by_type[ KexiDB::Field::InvalidType ];
		if (!item)
			return 0;
	}
	return item;
}

