/***************************************************************************
                          propertyitem.h  -  description                              
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


#ifndef PROPERTYITEM_H
#define PROPERTYITEM_H

#include <qlistview.h>
#include <qstringlist.h>
#include <qlist.h>

class QuerySelect;
class PropertyEditor;

/**
  *@author Ørn E. Hansen
  */

class PropertyItem : public QListViewItem  {
private:
	PropertyEditor *_ped;
	
protected:
	void buildProperty(const QString&, const QString&);
	
public: 
	PropertyItem(QuerySelect *, const QString&, const QString&);
	PropertyItem(PropertyItem *, const QString&, const QString&);
	~PropertyItem();

	void set(const QString&, const QString&);
	void set(const QString&, const QStringList&);
	void set(const QString&, bool);	
	void edit();
	
	QString getProperty(const QString&);
	QString text(int) const;
};

#endif





