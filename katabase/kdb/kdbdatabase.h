/***************************************************************************
                          database.h  -  description                              
                             -------------------                                         
    begin                : Sat Jun 5 1999                                           
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


#ifndef KDB_DATABASE_H
#define KDB_DATABASE_H

#include <qobject.h>
#include <qstring.h>

#include <kdb.h>

class kdbCriteria;
class kdbQuery;
class kdbDataSet;
class kdbFieldSet;

/** Abstract class, for physically communications with a database.
	*
	* The database class connects the program, physically with the
  * database.  It provides an abstract means, of basic functions
	* for which a database is communicated with.
	*
  *@author Ørn E. Hansen
  */

class kdbDataBase : public QObject {
Q_OBJECT

public:
	kdbDataBase(const char *);
	virtual ~kdbDataBase();

	virtual bool create(const char *,kdbFieldSet*) { return false; };
	virtual bool insert(const char *,kdbFieldSet*) { return false; };
	virtual bool insert(kdbDataSet*)               { return false; };
	virtual bool update(kdbDataSet*)               { return false; };
	virtual bool remove(kdbDataSet*)               { return false; };
	virtual bool remove(const QString&)            { return false; };
	virtual bool query(kdbDataSet*)                { return false; };
	virtual bool getFields(kdbDataSet*)            { return false; };

signals:
	void initBase();
	void destroyBase();
	void preInsert(kdbDataSet*);
	void postInsert(kdbDataSet*);
	void preDelete(kdbDataSet*);
	void postDelete(kdbDataSet*);
	void preUpdate(kdbDataSet*);
	void postUpdate(kdbDataSet*);
	void preLoad(kdbDataSet*);
	void postLoad(kdbDataSet*);
	void preDestroy(const QString&);
	void postDestroy(const QString&);
		
};

#endif

































