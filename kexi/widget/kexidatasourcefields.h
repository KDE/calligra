/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIDATASOURCEFIELDS_H
#define KEXIDATASOURCEFIELDS_H

#include <qwidget.h>
#include <kexipartitem.h>

class QListBox;
class KexiProject;
namespace KexiDB
{
	class FieldList;
}

namespace KexiPart
{
	class DataSource;
}

/**
 * this visual class provides two ListViews
 * one with all available fields of a DataSource and one
 * where users chosen are displayed.
 * the fields can be made visible by << < > >> buttons
 */
class KEXIEXTWIDGETS_EXPORT KexiDataSourceFields : public QWidget
{
	Q_OBJECT

	public:
		KexiDataSourceFields(KexiProject *project, QWidget *parent, const char *name = 0);
		~KexiDataSourceFields();

		/**
		 * @returns a FieldList of fields selected to be displayed
		 * @note you can savely delete the list after it isn't used anymore
		 */
		KexiDB::FieldList *usedFields();

	public slots:
		/**
		 * regenerates the list of available fields and sets datasource to @arg ds
		 */
		void setDataSource(KexiPart::DataSource *ds, const KexiPart::Item &it);

	protected slots:
		/**
		 * adds selected item to displayed items
		 */
		void addSelected();

		/**
		 * removes selected item from displayed items and adds it to available items
		 */
		void removeSelected();

		/**
		 * adds all available items to displayed items
		 */
		void addAll();

		/**
		 * removes all displayed fields and adds them to availabe list again
		 */
		void removeAll();

	signals:
		/**
		 * this singal is executed everytime the list of displayed fields changes
		 */
		void listChanged();

	private:
		QListBox *m_avail;
		QListBox *m_used;

		KexiProject *m_project;
		KexiPart::DataSource *m_ds;
		KexiPart::Item m_item;
		KexiDB::FieldList *m_fields;
};

#endif

