/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
 */

#ifndef KEXICONNSELECTOR_H
#define KEXICONNSELECTOR_H

#include "kexidbconnectionset.h"
#include "KexiStartupFileDialog.h"
#include <kexidb/driver.h>

#include <kdialogbase.h>
#include <klistview.h>

#include <qwidgetstack.h>
#include <qguardedptr.h>

class KexiConnSelectorBase;

//! helper class
class ConnectionDataLVItem : public QListViewItem
{
public:
	ConnectionDataLVItem(KexiDB::ConnectionData *d, 
		const KexiDB::Driver::Info& info, QListView *list);
	~ConnectionDataLVItem();
	
	KexiDB::ConnectionData *data;
};


class KexiOpenExistingFile;
class KexiConnSelectorBase;
class KexiConnSelectorWidgetPrivate;

/*! Widget that allows to select a database connection (without choosing database itself)
*/
class KEXIMAIN_EXPORT KexiConnSelectorWidget : public QWidgetStack
{
	Q_OBJECT

public:
	enum ConnType { FileBased=1, ServerBased=2 };
    
	/*! Constructs a KexiConnSelector which contain \a conn_set as connection set. */
	KexiConnSelectorWidget( const KexiDBConnectionSet& conn_set, QWidget* parent = 0, 
		const char* name = 0 );
    ~KexiConnSelectorWidget();
	
	/*! After accepting this dialog this method returns wherher user selected
	 file- or server- based connection (ConnType enum). */
	int selectedConnectionType() const;

	/*! \return data of selected connection, if server-based connection was selected.
	 Returns NULL if no selection has been made or file-based connection 
	 has been selected. 
	 @see selectedConnectionType()
	*/
	KexiDB::ConnectionData* selectedConnectionData() const;

	/*! \return the name of database file, if server-based connection was selected.
	 Returns null string if no selection has been made or server-based connection 
	 has been selected.
	 @see selectedConnectionType()
	*/
	QString selectedFileName();

	//! Usable when we want to do other things for "back" button
	void disconnectShowSimpleConnButton();

	QListView* connectionsList() const;
	
	KexiConnSelectorBase *m_remote;
	KexiOpenExistingFile *m_file;
	KexiStartupFileDialog *m_fileDlg;

	/*! If true, user will be asked to accept overwriting existing project. 
	 This is true by default. */
	void setConfirmOverwrites(bool set);

	bool confirmOverwrites() const;

signals:
	void connectionItemExecuted( ConnectionDataLVItem *item );

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

protected slots:
	void slotConnectionItemExecuted(QListViewItem *item);

protected:
	QGuardedPtr<const KexiDBConnectionSet> m_conn_set;
	
	KexiConnSelectorWidgetPrivate *d;
	
};


#endif // KEXICONNSELECTOR_H

