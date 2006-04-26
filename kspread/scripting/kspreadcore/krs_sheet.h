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

#ifndef KROSS_KSPREADCOREKRSSHEET_H
#define KROSS_KSPREADCOREKRSSHEET_H

#include <kspread_sheet.h>
#include <kspread_value.h>

#include <api/class.h>

namespace Kross { namespace KSpreadCore {

class Sheet : public Kross::Api::Class<Sheet>
{
    public:
        Sheet(KSpread::Sheet* sheet, KSpread::Doc* doc = 0);
        virtual ~Sheet();
        virtual const QString getClassName() const;
    private:
	/**
	 * Return the name of the sheet
	 */
	Kross::Api::Object::Ptr getName(Kross::Api::List::Ptr);
	/**
	 * Set the name of the sheet
	 */
	Kross::Api::Object::Ptr setName(Kross::Api::List::Ptr);
	/**
	 * Return a given cell
	 */
	Kross::Api::Object::Ptr cell(Kross::Api::List::Ptr);
	/**
	 * Add a new row
	 */
	Kross::Api::Object::Ptr insertRow(Kross::Api::List::Ptr);
	/**
	 * Add a new column
	 */
	Kross::Api::Object::Ptr insertColumn(Kross::Api::List::Ptr);
	/**
	 * Remove a row
	 */
	Kross::Api::Object::Ptr removeRow(Kross::Api::List::Ptr);
	/**
	 * Remove a column
	 */
	Kross::Api::Object::Ptr removeColumn(Kross::Api::List::Ptr);
    private:
	KSpread::Sheet* m_sheet;
	KSpread::Doc* m_doc;
};
}
}

#endif
