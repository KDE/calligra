/***************************************************************************
                          criteria.h  -  description                              
                             -------------------                                         
    begin                : Sun Jun 6 1999                                           
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


#ifndef KDB_CRITERIA_H
#define KDB_CRITERIA_H

#include <qstring.h>
#include <qobject.h>

#include <kdb.h>

class kdbDataSet;

/** Give a criteria for a query.
  *
  * Each query needs a criteria, to which to search for data.  The
  * criteria is a dataset, a field, a value and the relation between
  * the field and the value.  Data is selected according to this
  * criteria, and put into the dataset under the query.
  *
  *@author Ørn E. Hansen
  */

class kdbCriteria : public QObject {
	Q_OBJECT
	
private:
	typedef Kdb::RelationType Rel;
	typedef Kdb::Operator     Oper;
	
  /** The dataset, this criteria refers to. */
	kdbDataSet *_set;
	/** A field, within the dataset for which the criteria is to be
    * checked for.  An empty field, denotes no criteria and all
    * records will fullfill it.
		*/
	QString _field;
  /** The value itself, which must be found within the field according
    * to the relation.
    */
	QString _value;
  /** A relation to be checked for, between the field and value. */
	Rel     _relation;
	Oper    _operation;
	
public:
	/** Make a clone of another criteria */
	kdbCriteria(kdbCriteria&);
  /** A criteria takes two parameters, a pointer to a dataSet and
    * a name of the field, which the criteria is to be based upon. An
    * empty field is no criteria.
    *
    * Initially the relation is greater_ and the value is empty, which
    * will take all records.
		*/
	kdbCriteria(kdbDataSet*, const char *);
	/** Destroy the criteria and free all memory it occupies. */
	~kdbCriteria();

	/** Return the name of the field. */
	const QString& field()              { return _field;                 };
	/** Return the value. */
	const QString& value()              { return _value;                 };

	/** To set or change the value, the criteria is simply assigned a
    * string value.
		*/
	kdbCriteria& operator= (const QString& s) { _value = s; return (*this);    };
	/** To set or change the relation, the criteria is simply assigned
		* a value of type __rel.
		*/
	kdbCriteria& operator= (Rel v)      { _relation = v; return (*this); };
	kdbCriteria& operator= (Oper v)     { _operation =v; return (*this); };
	
	/** A method to retrieve the value. */
	operator const char *()             { return _value.data();          };
	/** A method to retrieve the relation value. */
	operator Kdb::RelationType ()       { return _relation;              };
	operator Kdb::Operator ()           { return _operation;             };
};

#endif






























