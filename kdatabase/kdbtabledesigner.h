/***************************************************************************
                          clstbldesigner2.h  -  description
                             -------------------
    begin                : Mon Apr 29 2002
    copyright            : (C) 2002 by root
    email                : root@machlinux
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CLSTBLDESIGNER2_H
#define CLSTBLDESIGNER2_H

#include <kdialog.h>
#include <qstringlist.h>
//#include <qtable.h>

/**
  *@author root
  */

class QTable;
class QStringList;

class KDBTableDesigner : public KDialog  {
         Q_OBJECT
public: 

	enum DataType
	{
		t_int,
		t_char,
		t_vchar,
		t_float,
		t_boolen
	};
		 
	KDBTableDesigner();
	~KDBTableDesigner();
	bool populateTblDesigner(QString tblName);
	void addRow(bool primary_key=false, QString name="", DataType type=t_int, int size=10, QString default_v="", bool allow_null=false);

private:
	QTable		*m_table;
	QStringList	m_comboEntries;

protected:
	int		m_rows;
};

#endif
