/***************************************************************************
                          fieldset.h  -  description                              
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


#ifndef KDB_FIELDSET_H
#define KDB_FIELDSET_H

#include <qlist.h>
#include <kdbdatafield.h>

class kdbDataSet;

/** Contains a set of data fields.
	*
	* Each table within a database, has several fields.  This class
	* provides a container for these fields.	And methods to access
	* and change these fields at need.
	*
  *@author Ørn E. Hansen
  */

class kdbFieldSet {
private:
	kdbDataSet *_parent;
	QList<class kdbDataField> _fields;

	int locate(const QString&);
	
public:
	kdbFieldSet(kdbFieldSet&);
	kdbFieldSet(kdbDataSet*);
	~kdbFieldSet();

	void clear()                       { _fields.clear();        };
	
	kdbDataSet* parent() const         { return _parent;         };

	uint fieldNo(const QString&);
	
	void addField(const QString&);
	void addField(kdbDataField *);
	void removeField(const QString&);

	bool has(const QString&);

	uint count() const                 { return _fields.count(); };

	QString fieldList(const QChar&);
	QString fieldList(const QString&,const QChar&);
	QString valueList(const QChar&,const QChar&);
	
	kdbDataField& field(const QString&);
	kdbDataField& field(uint);
	
	kdbDataField& operator[] (const QString& p)  { return field(p); };
	kdbDataField& operator[] (uint i)            { return field(i); }

};

#endif































