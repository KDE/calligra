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

#ifndef KEXICELLEDITORFACTORY_H
#define KEXICELLEDITORFACTORY_H

#include <qvariant.h>
#include <qscrollview.h>

#include <kexidb/field.h>

class KexiCellEditorFactoryItem;
class KexiTableEdit;
class KexiTableViewColumn;

class KexiCellEditorFactory
{
	public:
		KexiCellEditorFactory();
		virtual ~KexiCellEditorFactory();

		/*! Registers factory item for \a type. Passing KExiDB::Field::Invalid as type
		 will set default item, i.e. the one that will be used when no other item is 
		 defined for given data type. */
		static void registerItem( uint type, KexiCellEditorFactoryItem& item );

		//! \return item for \a type. It no item found, the default is tried. Eventually, may return NULL.
		static KexiCellEditorFactoryItem* item( uint type );

//		static KexiTableEdit* createEditor(KexiDB::Field &f, QScrollView* parent = 0);
		static KexiTableEdit* createEditor(KexiTableViewColumn &column, QScrollView* parent = 0);
};

class KexiCellEditorFactoryItem
{
	public:
		KexiCellEditorFactoryItem();
		virtual ~KexiCellEditorFactoryItem();

	protected:
//		virtual KexiTableEdit* createEditor(KexiDB::Field &f, QScrollView* parent = 0) = 0;
		virtual KexiTableEdit* createEditor(KexiTableViewColumn &column, QScrollView* parent = 0) = 0;
	
	friend class KexiCellEditorFactory;
};


#endif
