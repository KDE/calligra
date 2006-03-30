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

#ifndef KEXIDATASOURCECOMBOBOX_H
#define KEXIDATASOURCECOMBOBOX_H

#include <kcombobox.h>
//Added by qt3to4:
#include <Q3CString>

class KexiProject;
namespace KexiPart {
	class Item;
}

/**
 * A combo box listing availabe data sources (tables and queries)
 * with icons. "Define query..." item can be also prepended.
 */
class KEXIEXTWIDGETS_EXPORT KexiDataSourceComboBox : public KComboBox
{
	Q_OBJECT

	public:
		KexiDataSourceComboBox(QWidget *parent, const char *name=0);
		~KexiDataSourceComboBox();

		KexiProject* project() const;

		Q3CString selectedMimeType() const;
		Q3CString selectedName() const;
		/*! \return index of item of mime type \a mimeType and name \a name.
		 Returs -1 of no such item exists. */
		int findItem(const Q3CString& mimeType, const Q3CString& name);

	public slots:
		void setProject(KexiProject *prj);

		/*! Selects item for data source described by \a mimeType and \a name.
		 If \a mimeType is empty, either "kexi/table" and "kexi/query" are tried. */
		void setDataSource(const Q3CString& mimeType, const Q3CString& name);

	signals:
		void dataSourceSelected();

	protected slots:
		void slotNewItemStored(KexiPart::Item& item);
		void slotItemRemoved(const KexiPart::Item& item);
		void slotItemRenamed(const KexiPart::Item& item, const Q3CString& oldName);
		void slotActivated( int index );

	protected:
		class Private;
		Private *d;
};

#endif
