
/*
** Header file for inclusion with kword_xml2latex.c
**
** Copyright (C) 2000 Robert JACOLIN
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** To receive a copy of the GNU Library General Public License, write to the
** Free Software Foundation, Inc., 59 Temple Place - Suite 330,
** Boston, MA  02111-1307, USA.
**
*/

#ifndef __KWORD_TABLE_H__
#define __KWORD_TABLE_H__

#include <qstring.h>
#include <qlist.h>
#include "element.h"		/* child class */
#include "layout.h"		/* Cell flow */

class Table: public QList<Element>
{
	int _maxRow, _maxCol;
	QString _name;
	QString _grpMgr;

	public:
		Table();
		Table(QString);

		virtual ~Table();

		QString getName  () const { return _name;   }
		QString getGrpMgr() const { return _grpMgr; }
		int     getMaxRow() const { return _maxRow; }
		int     getMaxCol() const { return _maxCol; }
		EEnv    getCellFlow(int);

		void setMaxRow(int r) { _maxRow = r; }
		void setMaxCol(int c) { _maxCol = c; }

		Element* searchCell(int, int);
		void append(Element*);

		void generate(QTextStream&);

	private:
		void generateCell(QTextStream&, int, int);
		void generateTableHeader(QTextStream&);
};

#endif /* __KWORD_TABLE_H__ */

