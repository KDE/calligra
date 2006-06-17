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

#include "krs_color.h"
#include "krs_cell.h"

#include <kspread_sheet.h>
#include <kspread_cell.h>
#include <kspread_value.h>

#include "manipulator.h"
#include "manipulator_data.h"

namespace Kross { namespace KSpreadCore {

Cell::Cell(KSpread::Cell* cell, KSpread::Sheet* sheet, uint col, uint row) : Kross::Api::Class<Cell>("KSpreadCell"), m_cell(cell), m_sheet(sheet), m_col(col), m_row(row) {

    this->addFunction0< Kross::Api::Variant >("value", this, &Cell::value);
    this->addFunction0< Kross::Api::Variant >("text", this, &Cell::text);
    this->addFunction1< void, Kross::Api::Variant >("setText", this, &Cell::setText);

    //TODO
    //this->addFunction0< void, Kross::Api::Variant >("setTextColor", this, &Cell::setTextColor);
    //this->addFunction0< void, Kross::Api::Variant >("setBackgroundColor", this, &Cell::setBackgroundColor);
}

Cell::~Cell() {
}

const QString Cell::getClassName() const {
    return "Kross::KSpreadCore::Cell";
}

QVariant Cell::value() {
    KSpread::Value value = m_cell->value();
    switch(value.type()) {
	case KSpread::Value::Empty:
            return QVariant();
	case KSpread::Value::Boolean:
            return QVariant(value.asBoolean(),0);
	case KSpread::Value::Integer:
            return static_cast<qint64>(value.asInteger());
	case KSpread::Value::Float:
            return (float)value.asFloat();
	case KSpread::Value::String:
            return value.asString();
	case KSpread::Value::Array:
	    //FIXME
	    /*
            Kross::Api::List* array = new Kross::Api::List;
            for( uint j = 0; j < value.rows(); j++)
            {
                        Kross::Api::List* row = new Kross::Api::List;
                        for( uint i = 0; i < value.columns(); i++)
                        {
                                    KSpread::Value v = value.element(i,j);
                                    row->append(convertToKrossApiVariant(v));
                        }
                        array->append(row);
            }
            return array;
	    */
	case KSpread::Value::CellRange:
	    //FIXME: not yet used
            return QVariant();
	case KSpread::Value::Error:
            return QVariant();
    }
    return QVariant();
}

const QString Cell::text() const {
    return m_cell->text();
}

bool Cell::setText(const QString& text, bool asString) {

    //FIXME: there is some problem with asString parameter, when it's set
    //to true KSpread says: ASSERT: "f" in dependencies.cc (621)
    //kspread: Cell at row 6, col 1 marked as formula, but formula is NULL

    KSpread::ProtectedCheck prot;
    prot.setSheet (m_sheet);
    prot.add (QPoint (m_col, m_row));
    if (prot.check())
        return false;

    KSpread::DataManipulator *dm = new KSpread::DataManipulator ();
    dm->setSheet (m_sheet);
    dm->setValue (text);
    dm->setParsing (!asString);
    dm->add (QPoint (m_col, m_row));
    dm->execute ();

    return true;
}

#if 0
Kross::Api::Object::Ptr Cell::setTextColor(Kross::Api::List::Ptr args) {
    Color* c = (Color*)args->item(0).data();
    m_cell->format()->setTextColor(c->toQColor());
    return Kross::Api::Object::Ptr();
}

Kross::Api::Object::Ptr Cell::setBackgroundColor(Kross::Api::List::Ptr args) {
    Color* c = (Color*)args->item(0).data();
    m_cell->format()->setBgColor(c->toQColor());
    return Kross::Api::Object::Ptr();
}
#endif

}
}
