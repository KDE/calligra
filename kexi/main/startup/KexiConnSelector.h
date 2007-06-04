/* This file is part of the KDE project
   Copyright (C) 2003,2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXICONNSELECTOR_H
#define KEXICONNSELECTOR_H

#include <kexidbconnectionset.h>
#include <kexidb/driver.h>
#include "KexiStartupFileDialog.h"
#include "ui_KexiConnSelector.h"

#include <KDialog>
#include <k3listview.h>

#include <QPointer>

class KexiConnSelectorBase : public QWidget, public Ui_KexiConnSelector
{
	public:
		KexiConnSelectorBase(QWidget* parent);
		~KexiConnSelectorBase();
};

//! helper class
class ConnectionDataLVItem : public Q3ListViewItem
{
	public:
		ConnectionDataLVItem(KexiDB::ConnectionData *data, 
			const KexiDB::Driver::Info& info, Q3ListView *list);
		~ConnectionDataLVItem();

		void update(const KexiDB::Driver::Info& info);
		KexiDB::ConnectionData *data() const { return m_data; }
	
	protected:
		KexiDB::ConnectionData *m_data;
};


class KexiConnSelectorWidgetPrivate;

/*! Widget that allows to select a database connection (without choosing database itself)
*/
class KEXIMAIN_EXPORT KexiConnSelectorWidget : public QWidget
{
	Q_OBJECT

	public:
		//! Defines connection type
		enum ConnType {
			FileBased=1, //!< the widget displays file-based connection
			ServerBased=2 //!< the widget displays server-based connection
		};

		/*! Constructs a KexiConnSelector which contain \a conn_set as connection set. 
		 \a conn_set can be altered, because Add/Edit/Remove buttons are available 
		 to users. \a startDirOrVariable can be provided to specify a start dir for file browser
		 (it can also contain a configuration variable name with ":" prefix as described 
		 in KRecentDirs documentation). */
		KexiConnSelectorWidget( KexiDBConnectionSet& conn_set, 
			const QString& startDirOrVariable, QWidget* parent = 0 );

		virtual ~KexiConnSelectorWidget();
		
		/*! After accepting this dialog this method returns wherher user selected
		 file- or server- based connection (ConnType enum). */
		int selectedConnectionType() const;

		/*! \return data of selected connection, if server-based connection was selected.
		 Returns NULL if no selection has been made or file-based connection 
		 has been selected. 
		 @see selectedConnectionType()
		*/
		KexiDB::ConnectionData* selectedConnectionData() const;

		/*! \return the name of database file, if file-based connection was selected.
		 Returns null string if no selection has been made or server-based connection 
		 has been selected.
		 @see selectedConnectionType()
		*/
		QString selectedFileName();

		/*! Sets selected filename to \a fileName. 
		 Only works when selectedConnectionType()==FileBased. */
		void setSelectedFileName(const QString& fileName);

//		//! Usable when we want to do other things for "back" button
//		void disconnectShowSimpleConnButton();

		Q3ListView* connectionsList() const;
		
		KexiConnSelectorBase *m_remote;
//		KexiOpenExistingFile *m_file;
		KexiStartupFileDialog *m_fileDlg;

		/*! If true, user will be asked to accept overwriting existing project. 
		 This is true by default. */
		void setConfirmOverwrites(bool set);

		bool confirmOverwrites() const;

	signals:
		void connectionItemExecuted(ConnectionDataLVItem *item);
		void connectionItemHighlighted(ConnectionDataLVItem *item);

	public slots:
		void showSimpleConn();
		void showAdvancedConn();
		virtual void setFocus();
		
		/*! Hides helpers on the server based connection page
		  (sometimes it's convenient not to have these):
		- "Select existing database server's connection..." (label at the top)
		- "Click "Back" button" (label at the bottom)
		- "Back" button itself */
		void hideHelpers();
		void hideConnectonIcon();

	protected slots:
		void slotConnectionItemExecuted(Q3ListViewItem *item);
		void slotRemoteAddBtnClicked();
		void slotRemoteEditBtnClicked();
		void slotRemoteRemoveBtnClicked();
		void slotConnectionSelectionChanged();
		void slotPrjTypeSelected(int id);

	private:
		ConnectionDataLVItem* addConnectionData( KexiDB::ConnectionData* data );
		ConnectionDataLVItem* selectedConnectionDataItem() const;
		
		class Private;
		Private * const d;
};

#endif // KEXICONNSELECTOR_H
