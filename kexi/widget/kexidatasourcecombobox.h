/* This file is part of the KDE project
   Copyright (C) 2005-2006 Jaroslaw Staniek <js@iidea.pl>

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

		//! \return global project that is used to retrieve schema informationm for this combo box.
		KexiProject* project() const;

		//! \return name of selected table or query. Can return null string.
		//! You should use isSelectionValid() to check validity of the input.
		QString selectedMimeType() const;

		//! \return name of selected table or query. Can return null string or nonexisting name,
		//! so you should use isSelectionValid() to check validity of the input.
		QString selectedName() const;

		//! \return true if current selection is valid
		bool isSelectionValid() const;

		/*! \return index of item of mime type \a mimeType and name \a name.
		 Returs -1 of no such item exists. */
		int findItem(const QString& mimeType, const QString& name);

	public slots:
		//! Sets global project that is used to retrieve schema informationm for this combo box.
		//! Tables visibility can be set using \a showTables queries visibility using \a showQueries.
		void setProject(KexiProject *prj, bool showTables = true, bool showQueries = true);

		/*! Sets item for data source described by \a mimeType and \a name.
		 If \a mimeType is empty, either "kexi/table" and "kexi/query" are tried. */
		void setDataSource(const QString& mimeType, const QString& name);

	signals:
		//! Emitted whenever data source changes. 
		//! Even setting invalid data source or clearing it will emit this signal.
		void dataSourceChanged();

	protected slots:
		void slotNewItemStored(KexiPart::Item& item);
		void slotItemRemoved(const KexiPart::Item& item);
		void slotItemRenamed(const KexiPart::Item& item, const Q3CString& oldName);
		void slotActivated( int index );
		void slotReturnPressed(const QString & text);

	protected:
		virtual void focusOutEvent( QFocusEvent *e );

		class Private;
		Private *d;
};

#endif
