/*
**
** Copyright (C) 2002 Robert JACOLIN
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
** Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
**
*/

#ifndef __KSPREAD_LATEX_COLUMN_H__
#define __KSPREAD_LATEX_COLUMN_H__

#include <QString>
//Added by qt3to4:
#include <QTextStream>

#include "config.h"
#include "format.h"
#include "xmlparser.h"

/***********************************************************************/
/* Class: Column                                                       */
/***********************************************************************/

/**
 * This class hold a column.
 */
class Column: public Format
{
	
	/* USEFULL DATA */
	long _col;
	double _width;

	public:
		/**
		 * Constructors
		 *
		 */

		/**
		 * Creates a new instance of Column.
		 */
		Column();

		/* 
		 * Destructor
		 *
		 * The destructor must remove the list of frames.
		 */

		virtual ~Column();

		/**
		 * getters
		 */

		long   getCol() const { return _col; }
		double getWidth() const { return _width; }

		/**
		 * setters
		 */
		void setCol(int c) { _col = c; }
		void setWidth(double w) { _width = w; }
	
		/**
		 * Helpful functions
		 */
		void     analyze (const QDomNode);
		void     generate  (QTextStream&);

	private:

};

#endif /* __KSPREAD_LATEX_COLUMN_H__ */

