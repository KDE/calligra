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

Cell::Cell(KSpread::Cell* cell, KSpread::Sheet* sheet, uint col, uint row) : Kross::Api::Class<Cell>("KSpreadCell", 0 ), m_cell(cell), m_sheet(sheet), m_col(col), m_row(row) {
    addFunction("value", &Cell::value);
    addFunction("text", &Cell::text);
    addFunction("setText", &Cell::setText, Kross::Api::ArgumentList() << Kross::Api::Argument("Kross::Api::Variant") );
    addFunction("setTextColor", &Cell::setTextColor, Kross::Api::ArgumentList() << Kross::Api::Argument("KSpreadColor") );
    addFunction("setBackgroundColor", &Cell::setBackgroundColor, Kross::Api::ArgumentList() << Kross::Api::Argument("KSpreadColor") );
}

Cell::~Cell() {
}

const QString Cell::getClassName() const {
    return "Kross::KSpreadCore::Cell";
}

Kross::Api::Object::Ptr Cell::value(Kross::Api::List::Ptr) {
    KSpread::Value value = m_cell->value();
    switch(value.type()) {
	case KSpread::Value::Empty:
            return new Kross::Api::Variant(QVariant(), "empty");
	case KSpread::Value::Boolean:
            return new Kross::Api::Variant(value.asBoolean());
	case KSpread::Value::Integer:
            return new Kross::Api::Variant(static_cast<Q_LLONG>(value.asInteger()));
	case KSpread::Value::Float:
            return new Kross::Api::Variant(value.asFloat());
	case KSpread::Value::String:
            return new Kross::Api::Variant(value.asString());
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
            return new Kross::Api::Variant(QVariant(), "cellrange");
	case KSpread::Value::Error:
            return new Kross::Api::Variant(QVariant(), "error");
    }
    return 0;
}

Kross::Api::Object::Ptr Cell::text(Kross::Api::List::Ptr) {
    return new Kross::Api::Variant(m_cell->text());
}

Kross::Api::Object::Ptr Cell::setText(Kross::Api::List::Ptr args) {
    QString text = Kross::Api::Variant::toString(args->item(0));
    bool asString = false;
    //FIXME: there is some problem with asString parameter, when it's set
    //to true KSpread says: ASSERT: "f" in dependencies.cc (621)
    //kspread: Cell at row 6, col 1 marked as formula, but formula is NULL
    if(args->count() > 1) {
    	asString= Kross::Api::Variant::toBool(args->item(1));
	kdDebug() << "asString" << endl;
    }

    KSpread::ProtectedCheck prot;
    prot.setSheet (m_sheet);
    prot.add (QPoint (m_col, m_row));
    if (prot.check())
        return new Kross::Api::Variant(1);

    KSpread::DataManipulator *dm = new KSpread::DataManipulator ();
    dm->setSheet (m_sheet);
    dm->setValue (text);
    dm->setParsing (!asString);
    dm->add (QPoint (m_col, m_row));
    dm->execute ();

    return 0;
}

Kross::Api::Object::Ptr Cell::setTextColor(Kross::Api::List::Ptr args) {
    Color* c = (Color*)args->item(0).data();
    m_cell->format()->setTextColor(c->toQColor());
    return 0;
}

Kross::Api::Object::Ptr Cell::setBackgroundColor(Kross::Api::List::Ptr args) {
    Color* c = (Color*)args->item(0).data();
    m_cell->format()->setBgColor(c->toQColor());
    return 0;
}

}
}
