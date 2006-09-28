/*
** A program to convert the XML rendered by KWord into LATEX.
**
** Copyright (C) 2000, 2001, 2002 Robert JACOLIN
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

#include <stdlib.h>		/* for atoi function    */
#include <kdebug.h>		/* for kDebug() stream */
#include "map.h"
//Added by qt3to4:
#include <QTextStream>

/*******************************************/
/* Constructor                             */
/*******************************************/
Map::Map()
{
}

/*******************************************/
/* Destructor                              */
/*******************************************/
Map::~Map()
{
	kDebug(30522) << "Destruction of a map." << endl;
}

/*******************************************/
/* Analyze                                 */
/*******************************************/
void Map::analyze(const QDomNode balise)
{
	/* Analysis of the parameters */
	kDebug(30522) << "ANALYZE A MAP" << endl;

	/* Analysis of the child markups */
	for(int index = 0; index < getNbChild(balise); index++)
	{
		// Only tables
		Table* table = new Table();
		table->analyze(getChild(balise, index));
		_tables.append(table);
	}
	kDebug(30522) << "END OF MAP" << endl;
}

/*******************************************/
/* Generate                                */
/*******************************************/
/* Generate each text zone with the parag. */
/* markup.                                 */
/*******************************************/
void Map::generate(QTextStream &out)
{
	Table *table = NULL;
	kDebug(30522) << "  MAP GENERATION" << endl;
	Q3PtrListIterator<Table> it(_tables);
	while ( (table = it.current()) != 0 )
	{
		++it;
		table->generate(out);
	}

	kDebug(30522) << "MAP GENERATED" << endl;
}

