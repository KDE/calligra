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

class Kexi;
class KListView;
class KListViewItem;

class KexiBrowser : public QWidget  {
   Q_OBJECT

public:
	KexiBrowser(Kexi *mainWin=0, QWidget *parent=0, const char *name=0);
	~KexiBrowser();

protected:
	void		createForm();
	
	Kexi*		m_mainWin;
	QWidget*	m_parent;

	KListView*	m_list;

	KListViewItem*	m_database;
	KListViewItem*	m_tables;
	KListViewItem*	m_queries;
	KListViewItem*	m_forms;
	KListViewItem*	m_reports;
	
protected slots:
//	void		slotContextMenu(QListViewItem *i, const QPoit p);
	void		slotCreate();
};

#endif
