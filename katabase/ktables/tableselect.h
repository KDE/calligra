/***************************************************************************
                          tableselect.h  -  description                              
                             -------------------                                         
    begin                : Thu Jul 8 1999                                           
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


#ifndef TABLESELECT_H
#define TABLESELECT_H

//Generated area. DO NOT EDIT!!!(begin)
#include <qwidget.h>
#include <qlistbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
//Generated area. DO NOT EDIT!!!(end)

#include <qdialog.h>

/**
  *@author Ørn E. Hansen
  */

class TableSelect : public QDialog  {
   Q_OBJECT
public: 
	TableSelect(QWidget *parent=0, const char *name=0);
	~TableSelect();

	void init();
	
	int tables() const;
	QString tableName(int);
	
protected: 
	void initDialog();
	//Generated area. DO NOT EDIT!!!(begin)
	QListBox *_available;
	QListBox *_selected;
	QLabel *QLabel_1;
	QLabel *QLabel_2;
	QPushButton *QPushButton_1;
	QPushButton *QPushButton_2;
	QPushButton *QPushButton_3;
	QPushButton *QPushButton_4;
	//Generated area. DO NOT EDIT!!!(end)

private:

signals:
	void signalMsg(const char *);
	void tableSelectionOk();

protected slots:
	void acceptItem();
	void acceptItem(int);
	void removeItem();
	void removeItem(int);
	void accept();
	void reject();
		
};

#endif

