/***************************************************************************
                          kdbrecordviewitem.h  -  description                              
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


#ifndef KDBRECORDVIEWITEM_H
#define KDBRECORDVIEWITEM_H

#include <qlistview.h>
#include <qtimer.h>

#include <kdbdataset.h>

/**
  *@author Ørn E. Hansen
  */

class kdbRecordViewItem : public QListViewItem  {
private:
	kdbDataSet *_set;
	uint        _record;

	void init(kdbDataSet *);
		
public: 
	kdbRecordViewItem(int,kdbDataSet *,QListView *);
	kdbRecordViewItem(int,kdbDataSet *,QListViewItem *);
	kdbRecordViewItem(int,kdbDataSet *,QListView *, QListViewItem *);
	kdbRecordViewItem(int,kdbDataSet *,QListViewItem *, QListViewItem *);
	~kdbRecordViewItem();
	
	QString text(int) const;
	uint record() const          { return _record; };

	void setText(int,const QString&);
	
	virtual void paintCell(QPainter*,const QColorGroup&,int,int,int);
	
};

#endif








