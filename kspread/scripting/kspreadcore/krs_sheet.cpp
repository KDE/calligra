/*
 *  Copyright (c) 2005 Cyrille Berger <cberger@cberger.net>
 *  Copyright (c) 2006 Isaac Clerencia <isaac@warp.es>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Library General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "krs_cell.h"
#include "krs_sheet.h"
#include "krs_doc.h"

#include <kspread_doc.h>
#include <kspread_sheet.h>
#include <kspread_cell.h>
#include <kspread_value.h>

namespace Kross { namespace KSpreadCore {

Sheet::Sheet(KSpread::Sheet* sheet, KSpread::Doc *doc) : Kross::Api::Class<Sheet>("KSpreadSheet"), m_sheet(sheet), m_doc(doc) {
    addFunction("getName", &Sheet::getName);
    addFunction("setName", &Sheet::setName);
    addFunction("cell", &Sheet::cell, Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant") << Kross::Api::Argument("Kross::Api::Variant") );
    addFunction("insertRow", &Sheet::insertRow, Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant") );
    addFunction("insertColumn", &Sheet::insertColumn, Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant") );
    addFunction("removeRow", &Sheet::removeRow, Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant") );
    addFunction("removeColumn", &Sheet::removeColumn, Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant") );
}

Sheet::~Sheet() {
}

const QString Sheet::getClassName() const {
    return "Kross::KSpreadCore::Sheet";
}

Kross::Api::Object::Ptr Sheet::getName(Kross::Api::List::Ptr)
{
    return Kross::Api::Object::Ptr(new Kross::Api::Variant(m_sheet->sheetName()));
}

Kross::Api::Object::Ptr Sheet::setName(Kross::Api::List::Ptr args)
{
    QString name = Kross::Api::Variant::toString(args->item(0));
    return Kross::Api::Object::Ptr(new Kross::Api::Variant(m_sheet->setSheetName(name)));
}

Kross::Api::Object::Ptr Sheet::cell(Kross::Api::List::Ptr args) {
    uint col = qMax(uint(1), Kross::Api::Variant::toUInt(args->item(0)));
    uint row = qMax(uint(1), Kross::Api::Variant::toUInt(args->item(1)));

    return Kross::Api::Object::Ptr(new Cell(m_sheet->cellAt(col,row),m_sheet,col,row));
}

Kross::Api::Object::Ptr Sheet::insertRow(Kross::Api::List::Ptr args) {
    uint row = Kross::Api::Variant::toUInt(args->item(0));
    return Kross::Api::Object::Ptr(new Kross::Api::Variant(m_sheet->insertRow(row)));
}

Kross::Api::Object::Ptr Sheet::insertColumn(Kross::Api::List::Ptr args) {
    uint col = Kross::Api::Variant::toUInt(args->item(0));
    return Kross::Api::Object::Ptr(new Kross::Api::Variant(m_sheet->insertColumn(col)));
}

Kross::Api::Object::Ptr Sheet::removeRow(Kross::Api::List::Ptr args) {
    uint row = qMax(uint(1), Kross::Api::Variant::toUInt(args->item(0)));
    m_sheet->removeRow(row);
    return Kross::Api::Object::Ptr();
}

Kross::Api::Object::Ptr Sheet::removeColumn(Kross::Api::List::Ptr args) {
    uint col = qMax(uint(1), Kross::Api::Variant::toUInt(args->item(0)));
    m_sheet->removeColumn(col);
    return Kross::Api::Object::Ptr();
}

}
}
