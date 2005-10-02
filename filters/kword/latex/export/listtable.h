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
** Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
**
*/

#ifndef __KWORD_LISTTABLE_H__
#define __KWORD_LISTTABLE_H__

#include "table.h"

/**
 * This class hold a list of tables. It just a wrapper for \verbatim QPtrList<Table> \endverbatim
 * to add a frame in a table if the table exists else to create a new table.
 */
class ListTable: public QPtrList<Table>
{
	public:
		/**
		 * Constructors
		 *
		 * Creates a new instance of ListTable.
		 */
		ListTable();

		/* 
		 * Destructor
		 */
		virtual ~ListTable();

		/**
		 * Accessors
		 */

		Table* isNewTable(QString);

		/**
		 * Modifiers
		 */

		void add(Element*);

	private:
};

#endif /* __KWORD_LISTTABLE_H__ */

