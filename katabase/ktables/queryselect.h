/***************************************************************************
                          queryselect.h  -  description                              
                             -------------------                                         
    begin                : Sun Jul 11 1999                                           
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


#ifndef QUERYSELECT_H
#define QUERYSELECT_H

#include <qlistview.h>

/**
  *@author Ørn E. Hansen
  */

class QuerySelect : public QListView  {
	Q_OBJECT
	
public: 
	QuerySelect(QWidget *,const char *);
	~QuerySelect();
	
	void addTable(const QString&);
	void removeTable(const QString&);
	
protected slots:
	void popupMenu(QListViewItem *, const QPoint&, int);
	
};

#endif

