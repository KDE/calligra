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

#include "manipulator.h"
#include "manipulator_data.h"

namespace Kross { namespace KSpreadCore {

Cell::Cell(KSpread::Cell* cell, KSpread::Sheet* sheet, uint col, uint row)
	: Kross::Api::Class<Cell>("KSpreadCell"), m_cell(cell), m_sheet(sheet), m_col(col), m_row(row)
{
	this->addFunction0< Kross::Api::Variant >("value", this, &Cell::value);
	this->addFunction1< Kross::Api::Variant, Kross::Api::Variant >("setValue", this, &Cell::setValue);

	this->addFunction0< Kross::Api::Variant >("column", this, &Cell::column);
	this->addFunction0< Kross::Api::Variant >("row", this, &Cell::row);

	this->addFunction0< Cell >("previousCell", this, &Cell::previousCell);
	this->addFunction0< Cell >("nextCell", this, &Cell::nextCell);
	this->addFunction1< void, Cell >("setPreviousCell", this, &Cell::setPreviousCell);
	this->addFunction1< void, Cell >("setNextCell", this, &Cell::setNextCell);

	this->addFunction0< Kross::Api::Variant >("name", this, &Cell::name);
	this->addFunction0< Kross::Api::Variant >("fullName", this, &Cell::fullName);

	this->addFunction0< Kross::Api::Variant >("comment", this, &Cell::comment);
	this->addFunction1< void, Kross::Api::Variant >("setComment", this, &Cell::setComment);

	this->addFunction0< Kross::Api::Variant >("getFormatString", this, &Cell::getFormatString);
	this->addFunction1< void, Kross::Api::Variant >("setFormatString", this, &Cell::setFormatString);

	this->addFunction0< Kross::Api::Variant >("text", this, &Cell::text);
	this->addFunction1< void, Kross::Api::Variant >("setText", this, &Cell::setText);

	this->addFunction0< Kross::Api::Variant >("textColor", this, &Cell::textColor);
	this->addFunction1< void, Kross::Api::Variant >("setTextColor", this, &Cell::setTextColor);
	this->addFunction0< Kross::Api::Variant >("backgroundColor", this, &Cell::backgroundColor);
	this->addFunction1< void, Kross::Api::Variant >("setBackgroundColor", this, &Cell::setBackgroundColor);
}

Cell::~Cell() {
}

const QString Cell::getClassName() const {
    return "Kross::KSpreadCore::KSpreadCell";
}

QVariant Cell::toVariant(const KSpread::Value& value) const
{
	//Should we use following value-format enums here?
	//fmt_None, fmt_Boolean, fmt_Number, fmt_Percent, fmt_Money, fmt_DateTime, fmt_Date, fmt_Time, fmt_String

	switch(value.type()) {
		case KSpread::Value::Empty:
			return QVariant();
		case KSpread::Value::Boolean:
			return QVariant( value.asBoolean() );
		case KSpread::Value::Integer:
			return static_cast<Q_LLONG>(value.asInteger());
		case KSpread::Value::Float:
			return (float)value.asFloat();
		case KSpread::Value::String:
			return value.asString();
		case KSpread::Value::Array: {
			QValueList<QVariant> colarray;
			for(uint j = 0; j < value.rows(); j++) {
				QValueList<QVariant> rowarray;
				for( uint i = 0; i < value.columns(); i++) {
					KSpread::Value v = value.element(i,j);
					rowarray.append( toVariant(v) );
				}
				colarray.append(rowarray);
			}
			return colarray;
		} break;
		case KSpread::Value::CellRange:
			//FIXME: not yet used
			return QVariant();
		case KSpread::Value::Error:
			return QVariant();
	}
	return QVariant();
}

QVariant Cell::value() const {
	return toVariant( m_cell->value() );
}

bool Cell::setValue(const QVariant& value) {
	KSpread::Value v = m_cell->value();
	switch(value.type()) {
		case QVariant::Bool: v.setValue( value.toBool() ); break;
		case QVariant::ULongLong: v.setValue( (long)value.toLongLong() ); break;
		case QVariant::Int: v.setValue( value.toInt() ); break;
		case QVariant::Double: v.setValue( value.toDouble() ); break;
		case QVariant::String: v.setValue( value.toString() ); break;
		case QVariant::Date: v.setValue( value.toDate() ); break;
		case QVariant::Time: v.setValue( value.toTime() ); break;
		case QVariant::DateTime: v.setValue( value.toDateTime() ); break;
		default: return false;
	}
	return true;
}

int Cell::column() const {
	return m_cell->column();
}

int Cell::row() const  {
	return m_cell->row();
}

Cell* Cell::previousCell() const {
	KSpread::Cell* c = m_cell->previousCell();
	return c ? new Cell(c,c->sheet(),c->column(),c->row()) : 0;
}

Cell* Cell::nextCell() const {
	KSpread::Cell* c = m_cell->nextCell();
	return c ? new Cell(c,c->sheet(),c->column(),c->row()) : 0;
}

void Cell::setPreviousCell(Cell* c) {
	return m_cell->setPreviousCell(c->m_cell);
}

void Cell::setNextCell(Cell* c) {
	return m_cell->setNextCell(c->m_cell);
}

const QString Cell::name() const {
	return m_cell->name();
}

const QString Cell::fullName() const {
	return m_cell->fullName();
}

const QString Cell::comment() const {
	return m_cell->format()->comment(m_col, m_row);
}

void Cell::setComment(const QString& c) {
	return m_cell->format()->setComment(c);
}

const QString Cell::getFormatString() const {
	return m_cell->format()->getFormatString(m_col, m_row);
}

void Cell::setFormatString(const QString& format) {
	m_cell->format()->setFormatString(format);
}

const QString Cell::text() const {
	return m_cell->text();
}

bool Cell::setText(const QString& text, bool asString) {

	//FIXME: there is some problem with asString parameter, when it's set
	//to true KSpread says: ASSERT: "f" in Dependencies.cpp (621)
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

const QString Cell::textColor() {
	return m_cell->format()->textColor(m_col, m_row).name();
}

void Cell::setTextColor(const QString& textcolor) {
	m_cell->format()->setTextColor( QColor(textcolor) );
}

const QString Cell::backgroundColor() {
	return m_cell->format()->bgColor(m_col, m_row).name();
}

void Cell::setBackgroundColor(const QString& backgroundcolor) {
	m_cell->format()->setBgColor( QColor(backgroundcolor) );
}

}
}
