/***************************************************************************
                          kdbBaseType.h  -  description                              
                             -------------------                                         
    begin                : Wed Jun 30 1999                                           
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

#ifndef KDB_BASETYPE_H
#define KDB_BASETYPE_H

#include <qstring.h>

class kdbBaseType {
private:
	QString     _label;
	QString     _descr;

protected:
	QString     _value;
	QString     _type;
	uint        _len;
	
public:
	kdbBaseType(kdbBaseType&);
	kdbBaseType(const QString&);
	virtual ~kdbBaseType();
	
	const QString& label()          { return _label;       };
	const QString& descr()          { return _descr;       };
	
	void setLabel(const QString& l) { _label = l; };
	void setDescr(const QString& d) { _descr = d; };
	
	virtual const QString& text();
	virtual const QString& type();
	virtual uint length() const;
	
	virtual void setText(const QString&);
	virtual void setType(const QString&);
	virtual void setLength(uint);
};

#endif




