/***************************************************************************
                          datafield.h  -  description                              
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


#ifndef KDB_DATAFIELD_H
#define KDB_DATAFIELD_H

#include <qobject.h>
#include <qstring.h>
#include <qdatetime.h>

#include <kdbBaseType.h>

/** Class containing database fields, and contained values.
	*
	* This classs realizes a field within a table.  A field can
	* contain any number of datatypes.  Like, date, string... to
	* mention just a few.  Each database has different methods
	* to deal with each type, and each database has different
	* types of fields.
	*
	* Abstract form of this class, only deals with the world
	* known dataformats, such as a string, integer and a double
	* value.  All other handling of field values, are left to
	* subclasses.
	*
  *@author Ørn E. Hansen
  */

class kdbDataField : public QObject, public kdbBaseType {
Q_OBJECT

public:
	kdbDataField(kdbDataField&);
	kdbDataField(QObject *,const QString&,const QString& p_typ = "char");
	virtual ~kdbDataField();

	virtual void set(int);
	virtual void set(double);
	virtual void set(const QString&);
	virtual void set(const kdbDataField&);
	virtual void set(const QDate&);
		
signals:
	void preChange(bool&);
	void postChange(const QString&);
	void validateText(bool&,QString&);
	
public slots:
	void setText(const QString&);
	void setDate(const QDate&);
	
};

#endif










































