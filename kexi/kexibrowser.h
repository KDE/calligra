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
	enum Section
	{
		SectionDB,
		SectionTable,
		SectionQuery,
		SectionForm,
		SectionReport
	};

	KexiBrowser(QWidget *parent=0, Section s=SectionDB, const char *name=0);
	~KexiBrowser();
	
	void			clearView();
	void			generateView();

	Section			section();
	
protected:
	void			createForm();
	void			addTables(KexiBrowserItem *parent);
	KexiBrowserItem*	m_tables;
	KexiBrowserItem*	m_queries;
	KexiBrowserItem*	m_forms;
	KexiBrowserItem*	m_reports;

	KIconLoader		*iconLoader;
	
	QWidget*		m_parent;

	KexiBrowserItem*	m_database;

	Section			m_section;
	
		
protected slots:
	void		slotContextMenu(KListView*, QListViewItem *i, const QPoint &point);
	void		slotCreate(QListViewItem* i);
	void		slotDelete();
	void		slotEdit();
	
	void		slotCreateTable();
	void		slotCreateNewForm();
};

#endif
