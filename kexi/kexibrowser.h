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
class KexiBrowserItem;
class KexiView;

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

	KexiBrowser(KexiView *project,QWidget *parent=0, Section s=SectionDB, const char *name=0);
	~KexiBrowser();
	
	Section			section();
	
private:
	KexiView *m_view;
protected:
	void			createForm();

	KexiBrowserItem*	m_tables;
	KexiBrowserItem*	m_queries;
	KexiBrowserItem*	m_forms;
	KexiBrowserItem*	m_reports;

	KIconLoader		*iconLoader;
	

	KexiBrowserItem*	m_database;

	Section			m_section;
	
protected slots:
	void		slotContextMenu(KListView*, QListViewItem *i, const QPoint &point);
	void		slotCreate(QListViewItem* i);
	void		slotDelete();
	void		slotEdit();
	
	void		slotCreateTable();
	void		slotAlterTable();
	void		slotDeleteTable();

	void		slotCreateNewForm();

	void		slotCreateQuery();

	void		slotShowReport();
};

#endif
