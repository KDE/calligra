/***************************************************************************
                          kexibrowser.h  -  description
                             -------------------
    begin                : Sun Jun 9 2002
    copyright            : (C) 2002 by lucijan busch
    email                : lucijan@gmx.at
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KEXIBROWSER_H
#define KEXIBROWSER_H

#include <klistview.h>

/**
  *@author lucijan busch
  */

class QListViewItem;

class KIconLoader;
class Kexi;
class KexiBrowserItem;

class KexiBrowser : public KListView {
   Q_OBJECT

public:
	KexiBrowser(QWidget *parent=0, const char *name=0);
	~KexiBrowser();
	
	void addTableItem(QString name);
	void clearView();
	
protected:
	void			createForm();
	void			generateView();
	
	KexiBrowserItem*	m_tables;
	KexiBrowserItem*	m_queries;
	KexiBrowserItem*	m_forms;
	KexiBrowserItem*	m_reports;

	KIconLoader		*iconLoader;
	
	QWidget*		m_parent;

	KexiBrowserItem*	m_database;
	
		
protected slots:
	void		slotContextMenu(KListView*, QListViewItem *i, const QPoint &point);
	void		slotCreate();
	void		slotDelete();
	void		slotEdit();
	
	void		slotCreateTable();
};

#endif
