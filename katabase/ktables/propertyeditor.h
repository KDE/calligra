/***************************************************************************
                          propertyeditor.h  -  description                              
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


#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include <qdialog.h>
#include <qstringlist.h>

class PropertyEditor;
class PropertyItem;
class QScrollView;
class QGridLayout;
class QLabel;
class QListViewItem;

/**
  *@author Ørn E. Hansen
  */

class PropertyEditor : public QDialog  {
  Q_OBJECT

private:
	QGridLayout   *_grid;
	QScrollView   *_items;
	uint           _count;
	QListViewItem *_item;
	
	QLabel *makeLabel(const QString&, QWidget *);
		
public: 
	PropertyEditor(QWidget *,const char *);
	~PropertyEditor();
	
	void set(PropertyItem *);
	void set(const QString&, const QString&);
	void set(const QString&, const QStringList&);
	void set(const QString&, bool);
	QString setting(const QString&);

signals:
	void exitted(PropertyEditor *, QListViewItem *);
	
protected slots:
	void done();
	void canceled();
	
};

#endif





