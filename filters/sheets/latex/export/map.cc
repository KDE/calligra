/*
** A program to convert the XML rendered by Words into LATEX.
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

#include "map.h"

#include "LatexDebug.h"

#include <stdlib.h>  /* for atoi function    */

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
    debugLatex << "Destruction of a map.";
}

/*******************************************/
/* Analyze                                 */
/*******************************************/
void Map::analyze(const QDomNode node)
{
    /* Analysis of the parameters */
    debugLatex << "ANALYZE A MAP";

    /* Analysis of the child markups */
    for (int index = 0; index < getNbChild(node); index++) {
        // Only tables
        Table* table = new Table();
        table->analyze(getChild(node, index));
        _tables.append(table);
    }
    debugLatex << "END OF MAP";
}

/*******************************************/
/* Generate                                */
/*******************************************/
/* Generate each text zone with the parag. */
/* markup.                                 */
/*******************************************/
void Map::generate(QTextStream &out)
{
    debugLatex << "  MAP GENERATION";
    foreach(Table* table, _tables) {
        table->generate(out);
    }

    debugLatex << "MAP GENERATED";
}

