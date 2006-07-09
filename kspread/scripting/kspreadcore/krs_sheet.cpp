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

#include <Doc.h>
#include <Sheet.h>
#include <Cell.h>
#include <Value.h>

namespace Kross { namespace KSpreadCore {

Sheet::Sheet(KSpread::Sheet* sheet, KSpread::Doc *doc) : Kross::Api::Class<Sheet>("KSpreadSheet"), m_sheet(sheet), m_doc(doc) {

    this->addFunction0< Kross::Api::Variant >("getName", this, &Sheet::getName);
    this->addFunction1< void, Kross::Api::Variant >("setName", this, &Sheet::setName);

    this->addFunction2< Cell, Kross::Api::Variant, Kross::Api::Variant >("cell", this, &Sheet::cell);

    this->addFunction1< Kross::Api::Variant, Kross::Api::Variant >("insertRow", this, &Sheet::insertRow);
    this->addFunction1< Kross::Api::Variant, Kross::Api::Variant >("insertColumn", this, &Sheet::insertColumn);

    this->addFunction1< void, Kross::Api::Variant >("removeRow", this, &Sheet::removeRow);
    this->addFunction1< void, Kross::Api::Variant >("removeColumn", this, &Sheet::removeColumn);
}

Sheet::~Sheet() {
}

const QString Sheet::getClassName() const {
    return "Kross::KSpreadCore::Sheet";
}

const QString Sheet::getName() const
{
    return m_sheet->sheetName();
}

void Sheet::setName(const QString& name)
{
    m_sheet->setSheetName(name);
}

Cell* Sheet::cell(uint col, uint row) {
    uint c = qMax(uint(1), col);
    uint r = qMax(uint(1), row);
    return new Cell(m_sheet->cellAt(c,r),m_sheet,c,r);
}

bool Sheet::insertRow(uint row) {
    return m_sheet->insertRow(row);
}

bool Sheet::insertColumn(uint col) {
    return m_sheet->insertColumn(col);
}

void Sheet::removeRow(uint row) {
    m_sheet->removeRow( qMax(uint(1), row) );
}

void Sheet::removeColumn(uint col) {
    m_sheet->removeColumn( qMax(uint(1), col) );
}

}
}
