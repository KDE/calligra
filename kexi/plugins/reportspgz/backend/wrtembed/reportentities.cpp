/*
 * OpenRPT report writer and rendering engine
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

#include "reportentities.h"

#include <parsexmlutils.h>

// qt
#include <qpainter.h>
#include <qstring.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qdom.h>
#include <qinputdialog.h>
#include <qslider.h>
#include <qdatastream.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qsettings.h>
#include <kdebug.h>

#include <koproperty/property.h>
#include <koproperty/set.h>
#include <koproperty/editor.h>
#include <KoGlobal.h>
#include <krobjectdata.h>

#include "reportentitylabel.h"
#include "reportentityfield.h"
#include "reportentitytext.h"
#include "reportentityline.h"
#include "reportentitybarcode.h"
#include "reportentityimage.h"

//
// ReportEntity
//
ReportEntity::ReportEntity ( ReportDesigner* r )
{
	_rd=r;
}

void ReportEntity::buildXML ( QGraphicsItem * item, QDomDocument & doc, QDomElement & parent )
{
	ReportEntity * re = NULL;
	switch ( dynamic_cast<KRObjectData*> ( item )->type() )
	{
		case KRObjectData::EntityLabel:
			re = ( ReportEntityLabel* ) item;
			break;
		case KRObjectData::EntityField:
			re = ( ReportEntityField* ) item;
			break;
		case KRObjectData::EntityText:
			re = ( ReportEntityText* ) item;
			break;
		case KRObjectData::EntityLine:
			re = ( ReportEntityLine* ) item;
			break;
		case KRObjectData::EntityBarcode:
			re = ( ReportEntityBarcode* ) item;
			break;
		case KRObjectData::EntityImage:
			re = ( ReportEntityImage* ) item;
			break;
			//case EntityGraph:
			//    re = (ReportEntityGraph*)item;
			//    break;
		default:
			kDebug() << "ReportEntity::buildXML(): unrecognized rtti type" << item->type() << endl;
	};

	if ( re != NULL )
	{
		re->buildXML ( doc,parent );
	}
}

void ReportEntity::buildXMLRect ( QDomDocument & doc, QDomElement & entity, QRectF rect )
{
	kDebug() << "Saving Rect " << rect << endl;
	QDomElement element = doc.createElement ( "rect" );

	QDomElement x = doc.createElement ( "x" );
	x.appendChild ( doc.createTextNode ( QString::number ( rect.x() ) ) );
	element.appendChild ( x );

	QDomElement y = doc.createElement ( "y" );
	y.appendChild ( doc.createTextNode ( QString::number ( rect.y() ) ) );
	element.appendChild ( y );

	QDomElement w = doc.createElement ( "width" );
	w.appendChild ( doc.createTextNode ( QString::number ( rect.width() ) ) );
	element.appendChild ( w );

	QDomElement h = doc.createElement ( "height" );
	h.appendChild ( doc.createTextNode ( QString::number ( rect.height() ) ) );
	element.appendChild ( h );

	entity.appendChild ( element );
}

void ReportEntity::buildXMLFont ( QDomDocument & doc, QDomElement & entity, QFont font )
{
	QDomElement element = doc.createElement ( "font" );
	element.appendChild ( doc.createTextNode ( font.toString() ) );
#if 0
	QDomElement face = doc.createElement ( "face" );
	face.appendChild ( doc.createTextNode ( font.family() ) );
	element.appendChild ( face );

	QDomElement size = doc.createElement ( "size" );
	size.appendChild ( doc.createTextNode ( QString::number ( font.pointSize() ) ) );
	element.appendChild ( size );

	QDomElement weight = doc.createElement ( "weight" );
	int w = font.weight();
	if ( w == QFont::Normal )
		weight.appendChild ( doc.createTextNode ( "normal" ) );
	else if ( w == QFont::Bold )
		weight.appendChild ( doc.createTextNode ( "bold" ) );
	else
		weight.appendChild ( doc.createTextNode ( QString::number ( w ) ) );
	element.appendChild ( weight );
#endif
	entity.appendChild ( element );
}

void ReportEntity::buildXMLTextStyle ( QDomDocument & doc, QDomElement & entity, ORTextStyleData ts )
{
	QDomElement element = doc.createElement ( "textstyle" );

	QDomElement bgcolor = doc.createElement ( "bgcolor" );
	bgcolor.appendChild ( doc.createTextNode ( ts.bgColor.name() ) );
	element.appendChild ( bgcolor );

	QDomElement fgcolor = doc.createElement ( "fgcolor" );
	fgcolor.appendChild ( doc.createTextNode ( ts.fgColor.name() ) );
	element.appendChild ( fgcolor );

	QDomElement bgopacity = doc.createElement ( "bgopacity" );
	bgopacity.appendChild ( doc.createTextNode ( QString::number(ts.bgOpacity) ) );
	element.appendChild ( bgopacity );
	
	buildXMLFont ( doc, element, ts.font );

	entity.appendChild ( element );
}

void ReportEntity::buildXMLLineStyle ( QDomDocument & doc, QDomElement & entity, ORLineStyleData ls )
{
	QDomElement element = doc.createElement ( "linestyle" );

	QDomElement color = doc.createElement ( "color" );
	color.appendChild ( doc.createTextNode ( ls.lnColor.name() ) );
	element.appendChild ( color );

	QDomElement weight = doc.createElement ( "weight" );
	weight.appendChild ( doc.createTextNode ( QString::number ( ls.weight ) ) );
	element.appendChild ( weight );

	QDomElement style = doc.createElement ( "style" );
	QString l;
	switch ( ls.style )
	{
		case Qt::NoPen:
			l = "nopen";
			break;
		case Qt::SolidLine:
			l = "solid";
			break;
		case Qt::DashLine:
			l = "dash";
			break;
		case Qt::DotLine:
			l = "dot";
			break;
		case Qt::DashDotLine:
			l = "dashdot";
			break;
		case Qt::DashDotDotLine:
			l = "dashdotdot";
			break;
		default:
			l = "solid";

	}
	style.appendChild ( doc.createTextNode ( l ) );
	element.appendChild ( style );

	entity.appendChild ( element );
}


