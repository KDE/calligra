/***************************************************************************
                          kdb.h  -  description                              
                             -------------------                                         
    begin                : Fri Jun 25 1999                                           
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


#ifndef KDATABASE_10_H
#define KDATABASE_10_H

#include <qstring.h>

class kdbDataSet;
class kdbDataBase;
class kdbRelation;

/**
  *@author Ørn E. Hansen
  */

class Kdb {
public:
	enum RelationType {
		Equal,
		Greater,
		Less,
		Like
	};
	enum Operator {
		And,
		Or,
		NAnd,
		NOr
	};
	enum Bases {
		Postgres,
		mySQL,
		miniSQL,
		ODBC,
		Oracle,
		None
	};
	
private:
	static Bases        _connectionType;
	static QString      _dateFormat;
	static kdbDataBase *_dataBase;

public:
	Kdb();
	~Kdb();
	
	static void setDateFormat(const QString& df) { _dateFormat = df;   };
	static const QString& dateFormat()           { return _dateFormat; };
	
	static bool isOpen();
	static bool hasDataSet(const QString&);
	static void Open(Bases, const QString&, const QString&);
	static void Close();
	
	static kdbDataSet  *dataSet(const QString&);
	static kdbRelation *relation(kdbDataSet *,const QString&);
	static kdbDataBase *dataBase();
};

#endif

























