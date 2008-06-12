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

#include "reportentities.h"
#include "reportentityshape.h"
#include "reportdesigner.h"

#include <qdom.h>
#include <qpainter.h>
#include <kdebug.h>
#include <klocalizedstring.h>
#include <koproperty/editor.h>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <KoShapeRegistry.h>


//
// class ReportEntityLabel
//

void ReportEntityShape::init(QGraphicsScene * scene)
{
	setFlags(ItemIsSelectable|ItemIsMovable);
	
	if (scene) 
		scene->addItem ( this );	
	
	ReportRectEntity::init(&_pos, &_size, _set);
	
	connect ( properties(), SIGNAL ( propertyChanged ( KoProperty::Set &, KoProperty::Property & ) ), this, SLOT ( propertyChanged ( KoProperty::Set &, KoProperty::Property & ) ) );
	
	setZValue(Z);
}

// methods (constructors)
ReportEntityShape::ReportEntityShape ( ReportDesigner* d, QGraphicsScene * scene )
	: ReportRectEntity(d)
{
	
    
    init(scene);	
setSceneRect(QPointF(0,0),QSizeF(100,100));    
_name->setValue(_rd->suggestEntityName("Shape"));
	
}

ReportEntityShape::ReportEntityShape ( QDomNode & element, ReportDesigner * d, QGraphicsScene * s )
	: ReportRectEntity(d), KRShapeData(element)
{
	init(s);
	setSceneRect(_pos.toScene(), _size.toScene());
}

ReportEntityShape* ReportEntityShape::clone()
{
	QDomDocument d;
	QDomElement e = d.createElement("clone");;
	QDomNode n;
	buildXML(d,e);
	n = e.firstChild();
	return new ReportEntityShape(n, designer(), 0);
}

// methods (deconstructor)
ReportEntityShape::~ReportEntityShape()
{}

void ReportEntityShape::paint ( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	QList<KoShape*> shapes;
	painter->setRenderHint(QPainter::Antialiasing);

	mShape = ( KoShape* )( KoShapeRegistry::instance()->value(_shapeType->value().toString()) )->createDefaultShape( 0 );
	mShape->setSize(_size.toScene());
	shapes << mShape;
	
	sp.setShapes(shapes);
      
	sp.paintShapes(*painter, z);
	//mShape->paint(*painter, z);

	drawHandles(painter);
}

void ReportEntityShape::buildXML ( QDomDocument & doc, QDomElement & parent )
{
	kDebug() << endl;
	//kdDebug() << "ReportEntityLabel::buildXML()");
	QDomElement entity = doc.createElement ( "shape" );

	// bounding rect
	buildXMLRect ( doc,entity, pointRect() );
	
	// name
	QDomElement n = doc.createElement ( "name" );
	n.appendChild ( doc.createTextNode ( entityName() ) );
	entity.appendChild ( n );
	
	// z
	QDomElement z = doc.createElement ( "zvalue" );
	z.appendChild ( doc.createTextNode ( QString::number(zValue()) ) );
	entity.appendChild ( z );

	parent.appendChild ( entity );
}

void ReportEntityShape::propertyChanged ( KoProperty::Set &s, KoProperty::Property &p )
{
	kDebug() << endl;
	//TODO KoProperty needs QPointF and QSizeF and need to sync property with actual size/pos
	if ( p.name() == "Position" )
	{
		//_pos.setUnitPos(p.value().value<QPointF>(), false);
	}
	else if ( p.name() == "Size" )
	{
		//_size.setUnitSize(p.value().value<QSizeF>());
	}
	else if (p.name() == "Name")
	{
		//For some reason p.oldValue returns an empty string
		if (!_rd->isEntityNameUnique(p.value().toString(), this))
		{
			p.setValue(_oldName);
		}
		else
		{
			_oldName =p.value().toString();	
		}
	}
		
	if ( _rd ) _rd->setModified ( true );
	if (scene()) scene()->update();
}




