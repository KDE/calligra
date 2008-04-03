/*
 * Kexi report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC
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


#include "krsectiondata.h"
#include <koproperty/property.h>
#include <koproperty/set.h>
#include <KoGlobal.h>
#include <kdebug.h>
#include <QColor>

#include "krlinedata.h"
#include "krfielddata.h"
#include "krtextdata.h"
#include "krbarcodedata.h"
#include "krimagedata.h"
#include "krlabeldata.h"

KRSectionData::KRSectionData()
{
	createProperties();
}

KRSectionData::KRSectionData ( const QDomElement & elemSource)
{
	createProperties();
	_name = elemSource.tagName();
	setObjectName ( _name );
	
	if ( _name != "rpthead" && _name != "rptfoot" &&
		    _name != "pghead" && _name != "pgfoot" &&
		    _name != "grouphead" && _name != "groupfoot" &&
		    _name != "head" && _name != "foot" &&
		    _name != "detail" )
	{
		_valid = false;
		return;
	}
	_height->setValue(1);

	QDomNodeList section = elemSource.childNodes();
	for ( int nodeCounter = 0; nodeCounter < section.count(); nodeCounter++ )
	{
		QDomElement elemThis = section.item ( nodeCounter ).toElement();
		if ( elemThis.tagName() == "height" )
		{
			bool valid;
			qreal height = elemThis.text().toDouble ( &valid );
			if ( valid )
				_height->setValue( height );
		}
		else if ( elemThis.tagName() == "bgcolor" )
		{
			_bgColor->setValue( QColor ( elemThis.text() ) );
		}
		else if ( elemThis.tagName() == "firstpage" )
		{
			if ( _name == "pghead" || _name == "pgfoot" )
				_extra = elemThis.tagName();
		}
		else if ( elemThis.tagName() == "odd" )
		{
			if ( _name == "pghead" || _name == "pgfoot" )
				_extra = elemThis.tagName();
		}
		else if ( elemThis.tagName() == "even" )
		{
			if ( _name == "pghead" || _name == "pgfoot" )
				_extra = elemThis.tagName();
		}
		else if ( elemThis.tagName() == "lastpage" )
		{
			if ( _name == "pghead" || _name == "pgfoot" )
				_extra = elemThis.tagName();
		}
		else if ( elemThis.tagName() == "label" )
		{
			KRLabelData * label = new KRLabelData ( elemThis );
			_objects.append ( label );
			//else
			//  delete label;
		}
		else if ( elemThis.tagName() == "field" )
		{
			KRFieldData * field = new KRFieldData ( elemThis );
			//if(parseReportField(elemThis, *field) == TRUE)
			//{
			_objects.append ( field );
			//TODO Totals
			//  if(field->trackTotal)
//          sectionTarget.trackTotal.append(field->data);
			//}
			//else
			//  delete field;
		}
		else if ( elemThis.tagName() == "text" )
		{
			KRTextData * text = new KRTextData ( elemThis );
			_objects.append ( text );
		}
		else if ( elemThis.tagName() == "line" )
		{
			KRLineData * line = new KRLineData ( elemThis );
			_objects.append ( line );
		}
		else if ( elemThis.tagName() == "barcode" )
		{
			KRBarcodeData * bc = new KRBarcodeData ( elemThis );
			_objects.append ( bc );
			
		}
		else if ( elemThis.tagName() == "image" )
		{
			KRImageData * img = new KRImageData ( elemThis );
			_objects.append ( img );
		}
		//TODO Graph
		//else if(elemThis.tagName() == "graph")
		//{
		//  ORGraphData * graph = new ORGraphData();
		//  if(parseReportGraphData(elemThis, *graph) == TRUE)
		//   sectionTarget.objects.append(graph);
		//  else
		//    delete graph;
		//}
		else
			kDebug() << "While parsing section encountered an unknown element: " << elemThis.tagName() << endl;
	}
	qSort (_objects.begin(),_objects.end(), zLessThan );
	_valid = true;
}

KRSectionData::~KRSectionData()
{

}

bool KRSectionData::zLessThan(KRObjectData* s1, KRObjectData* s2)
{
	return s1->Z < s2->Z;
}

void KRSectionData::createProperties()
{
	_set = new KoProperty::Set ( 0, "Section" );
	
	_height = new KoProperty::Property ( "Height", 1.0, "Height", "Height");
	_bgColor = new KoProperty::Property ( "BackgroundColor", Qt::white, "Background Color", "Background Color" );
	
	_set->addProperty ( _height );
	_set->addProperty ( _bgColor );
}

QString KRSectionData::name() const
{
	return (_extra.isEmpty() ? _name : _name + "_" + _extra);
}