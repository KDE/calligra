/***************************************************************************
                          kdbrelation.h  -  description                              
                             -------------------                                         
    begin                : Sun Jun 27 1999                                           
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


#ifndef KDBRELATION_H
#define KDBRELATION_H

#include <qobject.h>
#include <qstring.h>
#include <qlistbox.h>

#include <kdb.h>

class kdbDataSet;

/**
  *@author Ørn E. Hansen
  */

class kdbRelation : public QObject {
Q_OBJECT

private:
	QString           _field;
	QString           _value;
	Kdb::RelationType _kind;
	QString           _refTable;
	QString           _refField;
	QString           _listField;
	QListBox         *_listBox;
	kdbDataSet       *_set;
	
public:
	kdbRelation(kdbDataSet *,const QString&,const QString&,const QString&);
	~kdbRelation();

	QListBox *listBox();

	void  runQuery();
			
	const QString& field()              { return _field;     };
	const QString& value()              { return _value;     };
	const QString& refTable()           { return _refTable;  };
	const QString& refField()           { return _refField;  };
	const QString& listField()          { return _listField; };
	Kdb::RelationType kind() const      { return _kind;      };
	
	void setValue(const QString& v)     { _value = v;        };
	void setKind(Kdb::RelationType t)   { _kind = t;         };
	void setTable(const QString& t)     { _refTable = t;     };
	void setField(const QString& f)     { _refField = f;     };
	void setListField(const QString& f) { _listField = f;    };
	
	QString lookup(const QString&);
	int index(const QString&);
	
signals:
	void postLoad();
	
protected slots:
	void verifyText(bool&,QString&);
	void selected(int);
	
};

#endif

















