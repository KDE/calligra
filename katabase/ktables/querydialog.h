/***************************************************************************
                          querydialog.h  -  description                              
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


#ifndef QUERYDIALOG_H
#define QUERYDIALOG_H

#include <qdialog.h>
#include "queryselect.h"

/**
  *@author Ørn E. Hansen
  */

class QueryDialog : public QDialog  {
	Q_OBJECT
	
private:
	QuerySelect *_tree;
	
public: 
	QueryDialog(QWidget *,const char *);
	~QueryDialog();
	
	void clear()                       { _tree->clear();        };
	void addTable(const QString& s)    { _tree->addTable(s);    };
	void removeTable(const QString& s) { _tree->removeTable(s); };
	
	void buildQuery();
	
private slots:
	void ok_pushed();
	void cancel_pushed();
};

#endif



