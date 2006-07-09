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

#include <Doc.h>
#include <Map.h>
#include <Sheet.h>

namespace Kross { namespace KSpreadCore {

Doc::Doc(KSpread::Doc* doc)
	: Kross::Api::Class<Doc>("KSpreadDocument"), m_doc(doc)
{
	this->addFunction0< Sheet >("getName", this, &Doc::currentSheet);

	this->addFunction1< Sheet, Kross::Api::Variant >("sheetByName", this, &Doc::sheetByName);
	this->addFunction0< Kross::Api::Variant >("sheetNames", this, &Doc::sheetNames);

	this->addFunction1< Kross::Api::Variant, Kross::Api::Variant >("loadNativeXML", this, &Doc::loadNativeXML);
	this->addFunction0< Kross::Api::Variant >("saveNativeXML", this, &Doc::saveNativeXML);
}

Doc::~Doc() {

}

const QString Doc::getClassName() const {
	return "Kross::KSpreadCore::Doc";
}

Sheet* Doc::currentSheet()
{
	return new Sheet(m_doc->displaySheet(), m_doc);
}

Sheet* Doc::sheetByName(const QString& name)
{
	foreach ( KSpread::Sheet* sheet, m_doc->map()->sheetList() )
		if(sheet->sheetName() == name)
			return new Sheet(sheet, m_doc);
	return 0;
}

QStringList Doc::sheetNames()
{
	QStringList names;
	foreach ( KSpread::Sheet* sheet, m_doc->map()->sheetList() )
		names.append( sheet->sheetName() );
	return names;
}

bool Doc::loadNativeXML(const QString& xml) {
	QDomDocument doc;
	if(! doc.setContent(xml, true))
		return false;
	return m_doc->loadXML(0, doc);
}

QString Doc::saveNativeXML() {
	return m_doc->saveXML().toString(2);
}

}
}
