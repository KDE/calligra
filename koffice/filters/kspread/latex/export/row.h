/*
**
** Copyright (C) 2003 Robert JACOLIN
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

#ifndef __KSPREAD_LATEX_ROW_H__
#define __KSPREAD_LATEX_ROW_H__

#include <qstring.h>

#include "config.h"
#include "format.h"
#include "xmlparser.h"

/***********************************************************************/
/* Class: Row                                                          */
/***********************************************************************/

/**
 * This class hold a row.
 */
class Row: public Format
{
	
	/* USEFULL DATA */
	long _row;
	double _height;

	public:
		/**
		 * Constructors
		 *
		 */

		/**
		 * Creates a new instance of Row.
		 */
		Row();

		/* 
		 * Destructor
		 *
		 * The destructor must remove the list of frames.
		 */

		virtual ~Row();

		/**
		 * getters
		 */

		long   getRow() const { return _row; }
		double getHeight() const { return _height; }

		/**
		 * setters
		 */
		void setRow(int r) { _row = r; }
		void setHeight(double h) { _height = h; }
	
		/**
		 * Helpfull functions
		 */
		void     analyse (const QDomNode);
		void     generate  (QTextStream&);

	private:

};

#endif /* __KSPREAD_LATEX_ROW_H__ */

