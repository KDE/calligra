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

#include <qwidget.h>

/**
  *@author lucijan busch
  */

class QListViewItem;

class KIconLoader;
class Kexi;
class KListView;
class KListViewItem;

class KexiBrowser : public QWidget  {
   Q_OBJECT

public:
	KexiBrowser(Kexi *mainWin=0, QWidget *parent=0, const char *name=0);
	~KexiBrowser();
	
	void addTableItem(QString name);
	
protected:
	void		createForm();
	
	KListViewItem*	m_tables;
	KListViewItem*	m_queries;
	KListViewItem*	m_forms;
	KListViewItem*	m_reports;

	KIconLoader	*iconLoader;
	
	Kexi*		m_mainWin;
	QWidget*	m_parent;

	KListView*	m_list;

	KListViewItem*	m_database;
	
		
protected slots:
	void		slotContextMenu(KListView*, QListViewItem *i, const QPoint &point);
	void		slotCreate();
	void		slotDelete();
	void		slotEdit();
};

#endif
