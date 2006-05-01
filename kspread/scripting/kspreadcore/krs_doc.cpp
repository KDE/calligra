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

#include "krs_doc.h"
#include "krs_sheet.h"

#include <kspread_doc.h>
#include <kspread_map.h>
#include <kspread_sheet.h>
#include <Q3PtrListIterator>
namespace Kross { namespace KSpreadCore {

Doc::Doc(KSpread::Doc* doc) : Kross::Api::Class<Doc>("KSpreadDocument", 0 ), m_doc(doc) {
    addFunction("currentSheet", &Doc::currentSheet);
    addFunction("sheetByName", &Doc::sheetByName);
    addFunction("sheetNames", &Doc::sheetNames);
}

Doc::~Doc() {

}

const QString Doc::getClassName() const {
    return "Kross::KSpreadCore::Doc";
}

Kross::Api::Object::Ptr Doc::currentSheet(Kross::Api::List::Ptr)
{
    return Kross::Api::Object::Ptr(new Sheet(m_doc->displaySheet(), m_doc));
}

Kross::Api::Object::Ptr Doc::sheetByName(Kross::Api::List::Ptr args)
{
    QString name = Kross::Api::Variant::toString(args->item(0));
    foreach ( KSpread::Sheet* sheet, m_doc->map()->sheetList() )
    {
        if(sheet->sheetName() == name)
            return Kross::Api::Object::Ptr(new Sheet(sheet, m_doc));
    }
    return Kross::Api::Object::Ptr();
}

Kross::Api::Object::Ptr Doc::sheetNames(Kross::Api::List::Ptr)
{
    Kross::Api::List* array = new Kross::Api::List;
    foreach ( KSpread::Sheet* sheet, m_doc->map()->sheetList() )
    {
        array->append(Kross::Api::Object::Ptr(new Kross::Api::Variant(sheet->sheetName())));
    }
    return Kross::Api::Object::Ptr(array);
}

}
}
