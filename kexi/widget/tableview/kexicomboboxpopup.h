/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
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

#ifndef KEXICOMBOBOXPOPUP_H
#define KEXICOMBOBOXPOPUP_H

#include <q3frame.h>
//Added by qt3to4:
#include <QEvent>

class KexiComboBoxPopupPrivate;
class KexiTableView;
class KexiTableViewData;
class KexiTableViewColumn;
class KexiTableItem;
namespace KexiDB {
	class Field;
}

//! Internal class for displaying popup table view 
class KexiComboBoxPopup : public Q3Frame
{
	Q_OBJECT
	public:
//js TODO: more ctors!
		/*! Constructor for creating simple one-column enum-defined popup
		 using definition from \a f. */
		KexiComboBoxPopup(QWidget* parent, KexiDB::Field &f);
		KexiComboBoxPopup(QWidget* parent, KexiTableViewColumn &column);
		virtual ~KexiComboBoxPopup();

		KexiTableView* tableView();

		/*! Sets maximum number of rows for this popup. */
		void setMaxRows(int r);

		/*! \return maximum number of rows for this popup. */
		int maxRows() const;

		/*! Default maximum number of rows for KexiComboBoxPopup objects. */
		static const int defaultMaxRows;

		virtual bool eventFilter( QObject *o, QEvent *e );

	signals:
		void rowAccepted(KexiTableItem *item, int row);
		void cancelled();
		void hidden();

	public slots:
		virtual void resize( int w, int h );
		void updateSize(int minWidth = 0);

	protected slots:
		void slotTVItemAccepted(KexiTableItem *item, int row, int col);
		void slotDataReloadRequested();

	protected:
		void init();
		void setData(KexiDB::Field &f);
		void setData(KexiTableViewColumn &column);
		void setDataInternal( KexiTableViewData *data, bool owner = true ); //!< helper

		KexiComboBoxPopupPrivate *d;

		friend class KexiComboBoxTableEdit;
};

#endif

