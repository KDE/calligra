/***************************************************************************
                          kdbrecordview.h  -  description                              
                             -------------------                                         
    begin                : Mon Jun 28 1999                                           
    copyright            : (C) 1999 by Ørn E. Hansen                         
    email                : hanseno@mail.bip.net                                     
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/


#ifndef KDBRECORDVIEW_H
#define KDBRECORDVIEW_H

#include <qevent.h>
#include <qtimer.h>
#include <qlistview.h>
#include <qlineedit.h>
#include <qpopupmenu.h>

#include <kdbdataset.h>

#include "kdbrecordviewitem.h"

/**
  *@author Ørn E. Hansen
  */

class kdbRecordView : public QListView {
Q_OBJECT

protected:
	QList<QListViewItem> _dustBin;
	QPopupMenu *         _menu;
	QLineEdit  *         _edit;
	kdbDataSet *         _dataSet;
	uint                 _columnPos;

	void setEditItem(QListViewItem *);
	
public:
	kdbRecordView(kdbDataSet *, QWidget *, const char *);
	~kdbRecordView();
	
	uint columnPos()             { return _columnPos; };
	
	void insertItem(QListViewItem *);
	void takeItem(QListViewItem *);
	void removeItem(QListViewItem *);
	void clear();
	
	void commitDeletes();
	void commitEdits();
	
	void setTable(kdbDataSet *);
	
protected:
	void keyPressEvent(QKeyEvent*);
	void contentsMousePressEvent(QMouseEvent*);
	void focusInEvent(QFocusEvent*);
	void focusOutEvent(QFocusEvent*);
	
protected slots:
	void recordPicked(QListViewItem *);
	void textChanged(const QString&);
	void removeData(bool&);
	void insertData();
	void sizeChanged(int,int,int);
		
public slots:
	void buttonMenu(QListViewItem *,const QPoint&,int);
	void init();
	
};

#endif

















