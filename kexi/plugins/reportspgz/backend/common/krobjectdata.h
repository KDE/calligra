/*
 * Kexi Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)                  
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */
#ifndef KROBJECTDATA_H
#define KROBJECTDATA_H
#include <koproperty/property.h>
class KRLineData;
class KRLabelData;
class KRFieldData;
class KRTextData;
class KRBarcodeData;
class KRImageData;
class KRGraphData;

namespace KoProperty
{
	class Set;
	class Property;
}
/**
	@author 
*/
class KRObjectData
{
	public:
		enum EntityTypes
		{
			EntityLine  = 65537,
			EntityLabel = 65550,
			EntityField = 65551,
			EntityText  = 65552,
			EntityBarcode = 65553,
			EntityImage = 65554,
			EntityGraph = 65555
		};
		
		KRObjectData();
		virtual ~KRObjectData();

		virtual int type() const = 0;
		virtual KRLineData * toLine();
		virtual KRLabelData * toLabel();
		virtual KRFieldData * toField();
		virtual KRTextData * toText();
		virtual KRBarcodeData * toBarcode();
		virtual KRImageData * toImage();
		//virtual ORGraphData * toGraph();
		
		KoProperty::Set* properties(){return _set;}
		virtual void createProperties() =0;
		
		qreal Z;
		
		QString entityName(){return _name->value().toString();}
	protected:
		KoProperty::Set *_set;
		KoProperty::Property *_name;
		
		QString _oldName;
		
};

#endif
