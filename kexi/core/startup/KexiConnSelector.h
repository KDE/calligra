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

#include "KexiDBConnectionSet.h"

#include <kdialogbase.h>
#include <qwidgetstack.h>

class KexiNewFileDBWidget;
class KexiConnSelectorBase;
class KexiConnSelectorDialog;
class KexiConnSelectorWidgetPrivate;

/*! Widget that allows to select a database connection (without choosing database itself)
*/
class KEXICORE_EXPORT KexiConnSelectorWidget : public QWidgetStack
{
	Q_OBJECT

public:
	enum ConnType { FileBased=1, ServerBased=2 };
    
	KexiConnSelectorWidget( const KexiDBConnectionSet& conn_set, QWidget* parent = 0, const char* name = 0 );
    ~KexiConnSelectorWidget();
	
	/*! After accepting this dialog this method returns wherher user selected
	 file- or server- based connection. */
	int selectedConnectionType() const;

	/*! \return data of selected connection, if server-based connection was selected.
		Returns NULL if no selection has been made or file-based connection 
		has been selected.
	*/
	const KexiDB::ConnectionData* selectedConnectionData() const;
		
	//! Usable when we want to do other things for "back" button
	void disconnectShowSimpleConnButton();

	KexiConnSelectorBase *advancedPage() { return m_conn_sel; }

signals:
	void connectionItemDBLClicked(QListViewItem *item);

public slots:
	void showSimpleConn();
	void showAdvancedConn();

protected:
	KexiNewFileDBWidget *m_file_sel;
	KexiConnSelectorBase *m_conn_sel;
	const KexiDBConnectionSet *m_conn_set;
	
	KexiConnSelectorWidgetPrivate *d;
	
	friend class KexiConnSelectorDialog;
};

/*! Dialog that allows to select a database connection (without choosing database itself)
*/
class KEXICORE_EXPORT KexiConnSelectorDialog : public KDialogBase
{
	Q_OBJECT
public:
	KexiConnSelectorDialog(const KexiDBConnectionSet& conn_set, QWidget *parent = 0, 
		const char *name = 0 );
	~KexiConnSelectorDialog();

	/*! see: KexiConnSelector::selectedConnectionType() */
	int selectedConnectionType() const;

	/*! see: KexiConnSelector::selectedConnectionData() */
	const KexiDB::ConnectionData* selectedConnectionData() const;

protected slots:
	void connectionItemSelected();
	void connectionItemDBLClicked(QListViewItem *item);

protected:
	void updateDialogState();
	
	KexiConnSelectorWidget *m_sel;
};

#endif // KEXICONNSELECTOR_H

