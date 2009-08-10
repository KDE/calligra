/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "simplefield.h"


SimpleField::SimpleField() {
    pkey = false;
    notnull = false;
}

SimpleField::SimpleField ( KexiDB::QueryColumnInfo* col) {
name = col->aliasOrName();
type = col->field->typeName();
pkey = col->field->isPrimaryKey();
notnull = col->field->isNotNull();
}

void SimpleField::save(KoXmlWriter &writer)
{
    writer.startElement("relation:column");
    writer.addAttribute("name", name);
    writer.addAttribute("type", type);
    writer.addAttribute("primarykey", pkey);
    writer.addAttribute("notnull", notnull);
    writer.endElement();
}