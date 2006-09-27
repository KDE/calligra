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

#ifndef KSPREAD_KROSS_KRS_DOC_H_
#define KSPREAD_KROSS_KRS_DOC_H_

#include <kspread_doc.h>

#include <api/class.h>

#include <qstring.h>
#include <qstringlist.h>
#include <qdom.h>

namespace Kross { namespace KSpreadCore {

class Sheet;

/**
 * The KSpread document.
 *
 * Example (in Ruby) :
 * @code
 * doc = krosskspreadcore::get("KSpreadDocument")
 * @endcode
 */
class Doc : public Kross::Api::Class<Doc>
{
	public:
		explicit Doc(KSpread::Doc* doc);
		virtual ~Doc();
		virtual const QString getClassName() const;
	private:

		/**
		* This function returns the Sheet currently active in this
		* document.
		*
		* Example (in Ruby) :
		* @code
		* doc = krosskspreadcore::get("KSpreadDocument")
		* sheet = doc.currentSheet()
		* @endcode
		*/
		Sheet* currentSheet();

		/**
		* This function returns a Sheet by name.
		*
		* Example (in Ruby) :
		* @code
		* doc = krosskspreadcore::get("KSpreadDocument")
		* sheet = doc.sheetByName("foosheet")
		* @endcode
		*/
		Sheet* sheetByName(const QString& name);

		/**
		* This function returns an array with the sheet names
		*
		* Example (in Ruby) :
		* @code
		* doc = krosskspreadcore::get("KSpreadDocument")
		* sheetnames = doc.sheetNames()
		* sheet = doc.sheetByName( sheetnames[0] )
		* @endcode
		*/
		QStringList sheetNames();

		/**
		* Loads the native XML document.
		*/
		bool loadNativeXML(const QString& xml);
		/**
		* Save and return the to a native document saved XML.
		*/
		QString saveNativeXML();

#if 0
		bool loadOpenDocXML(const QString& xml);
		QString saveOpenDocXML();
#endif

		bool openUrl(const QString& url);
		bool saveUrl(const QString& url);
		bool import(const QString& url);
		bool exp0rt(const QString& url);

	private:
		KSpread::Doc* m_doc;
};
}
}


#endif
