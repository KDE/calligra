/***************************************************************************
                          dataset.h  -  description                              
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


#ifndef KDB_DATASET_H
#define KDB_DATASET_H

#include <qobject.h>
#include <qlist.h>
#include <qprogressbar.h>

#include <kdb.h>

class kdbFieldSet;
class kdbDataBase;
class kdbDataField;

/**
 * This class contains a representation of a database
 * table, within the program.
 *
 * Each physical table, must have an abstract representation
 * within a program.  A representation that is not directly
 * related to any means, the datatable is represented within
 * the database.  This class strives to achieve this, by
 * fulfilling most needs a table will have, when retrieving
 * data within a program.
 *
 * Before any data can be taken from this dataset, it must
 * be put into a query or a criteria and the database itself
 * made to fill in the data, as specified by any criteria
 * for a database query.
 *
 *@author Ørn E. Hansen
 *@short Local representation of a database table
 */

class kdbDataSet : public QObject {
Q_OBJECT

private:
	kdbFieldSet   *_fields;
	QString        _table;
	kdbDataSet    *_join;
	QString        _joinField;
	QList<QString> _data;
	
	unsigned int   _record;
	int            _recid;
	bool           _showProgress;
	kdbDataBase   *_dataBase;
	QProgressBar  *_progress;

	void retrieve();
	void notifyAll(int);
	bool locate(int);

public:
	kdbDataSet(kdbDataSet&);
	kdbDataSet(kdbDataBase *,const QString&);
	~kdbDataSet();

	void setJoin(class kdbDataSet *, const QString&);
	void setRecId(int);
	
	void progressControl(uint);
	void setVisualProgress(bool s)       { _showProgress = s; };

	void runQuery(bool);
	
	bool hasRelation(const QString&)      { return false; };
	
  kdbDataBase *dataBase() const         { return _dataBase; };
	kdbFieldSet  *fieldSet();
	
	kdbDataField& field(const QString&);
	kdbDataField& field(uint);
	uint          fieldNo(const QString&);
	
	kdbDataSet    *join()                 { return _join;             };
	const QString& joinField()            { return _joinField;        };
	const QString& name()                 { return _table;            };
	unsigned int   records()              { return _data.count();     };
	unsigned int   record()               { return _record;           };
	int recid()                           { return _recid;            };

	bool gotoRec(unsigned int);
	bool gotoRec();
	bool nextRec();
	bool prevRec();

	bool insert()                         { return insert(-1);         };
	bool insert(int);
	bool update();
	bool remove();
	void clear();
	
	kdbDataSet& operator++ ()             { nextRec(); return (*this); };
	kdbDataSet& operator-- ()             { prevRec(); return (*this); };

	operator kdbFieldSet*()               { return fieldSet();         };
	
	kdbDataField& operator[] (const QString& f) { return field(f); };
	kdbFieldSet&  operator[] (unsigned int);

signals:
	void initData();
	void clearData();
	void preInsert(bool&);
	void postInsert();
	void preUpdate(bool&);
	void postUpdate();
	void preDelete(bool&);
	void postDelete();
	
private slots:
	void postLoad(kdbDataSet *);
	
};

#endif





























































