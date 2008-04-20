//
// C++ Implementation: reportentitychart
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "reportentitychart.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include "reportentities.h"
#include "reportdesigner.h"

#include <qdom.h>
#include <qpainter.h>
#include <kdebug.h>
#include <klocalizedstring.h>

#include <koproperty/property.h>
#include <koproperty/set.h>
#include <koproperty/editor.h>



void ReportEntityChart::init ( QGraphicsScene* s, ReportDesigner *r )
{
	_rd = r;
	setPos ( 0,0 );

	setFlags ( ItemIsSelectable|ItemIsMovable );
	setAcceptsHoverEvents ( true );

	if ( s )
		s->addItem ( this );

	connect ( _set, SIGNAL ( propertyChanged ( KoProperty::Set &, KoProperty::Property & ) ), this, SLOT ( propertyChanged ( KoProperty::Set &, KoProperty::Property & ) ) );

	ReportRectEntity::init ( &_pos, &_size, _set );
	setZValue ( Z );

	setConnection ( _rd->theConn() );
}

ReportEntityChart::ReportEntityChart ( ReportDesigner * rd, QGraphicsScene* scene )
		: ReportRectEntity ( rd )
{
	init ( scene, rd );
	_size.setSceneSize ( QSizeF ( dpiX, dpiY ) );
	setSceneRect ( _pos.toScene(), _size.toScene() );

	_name->setValue ( _rd->suggestEntityName ( "Chart" ) );
}

ReportEntityChart::ReportEntityChart ( QDomNode & element, ReportDesigner * rd, QGraphicsScene* scene ) : ReportRectEntity ( rd ), KRChartData ( element )
{
	init ( scene, rd );
	setSceneRect ( _pos.toScene(), _size.toScene() );
}

ReportEntityChart::~ReportEntityChart()
{
}

void ReportEntityChart::paint ( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget *widget )
{
	// store any values we plan on changing so we can restore them
	QFont f = painter->font();
	QPen  p = painter->pen();
	QColor bg = Qt::white;

	painter->fillRect ( QGraphicsRectItem::rect(), bg );

	if ( _chartWidget )
	{
		_chartWidget->setFixedSize ( _size.toScene().toSize() );
		painter->drawImage ( rect().left(), rect().top(), QPixmap::grabWidget ( _chartWidget ), 0, 0, rect().width(), rect().height() );
	}
	bg.setAlpha ( 255 );

	painter->setBackground ( bg );
	painter->setPen ( Qt::black );

	painter->drawText ( rect(), 0, _dataSource->value().toString() +QObject::tr ( ":" ) +QObject::tr ( "chart" ) );


	if ( ( Qt::PenStyle ) _lnStyle->value().toInt() == Qt::NoPen || _lnWeight->value().toInt() <= 0 )
	{
		painter->setPen ( QPen ( QColor ( 224,224,224 ) ) );
	}
	else
	{
		painter->setPen ( QPen ( _lnColor->value().value<QColor>(), _lnWeight->value().toInt(), ( Qt::PenStyle ) _lnStyle->value().toInt() ) );
	}

	painter->drawRect ( rect() );

	painter->setBackgroundMode ( Qt::TransparentMode );

	drawHandles ( painter );

	// restore an values before we started just in case
	painter->setFont ( f );
	painter->setPen ( p );
}

ReportEntityChart* ReportEntityChart::clone()
{
	QDomDocument d;
	QDomElement e = d.createElement ( "clone" );;
	QDomNode n;
	buildXML ( d,e );
	n = e.firstChild();
	return new ReportEntityChart ( n, designer(), 0 );
}

void ReportEntityChart::buildXML ( QDomDocument & doc, QDomElement & parent )
{
	QDomElement entity = doc.createElement ( "chart" );

	buildXMLRect ( doc,entity, pointRect() );

	// name
	QDomElement n = doc.createElement ( "name" );
	n.appendChild ( doc.createTextNode ( entityName() ) );
	entity.appendChild ( n );

	// z
	QDomElement z = doc.createElement ( "zvalue" );
	z.appendChild ( doc.createTextNode ( QString::number ( zValue() ) ) );
	entity.appendChild ( z );

	// bounding rect
	buildXMLRect ( doc,entity,pointRect() );
	
	//Data source
	QDomElement data = doc.createElement ( "data" );
	QDomElement dcolumn = doc.createElement ( "datasource" );
	dcolumn.appendChild ( doc.createTextNode ( _dataSource->value().toString() ) );
	data.appendChild ( dcolumn );
	entity.appendChild ( data );
	//TODO Link child-master
	
	//type
	QDomElement type = doc.createElement ( "type" );
	type.appendChild ( doc.createTextNode ( _chartType->value().toString() ) );
	entity.appendChild ( type );
	
	//sub type
	QDomElement subtype = doc.createElement ( "subtype" );
	subtype.appendChild ( doc.createTextNode ( _chartSubType->value().toString() ));
	entity.appendChild ( subtype );
	
	//3d
	QDomElement d3 = doc.createElement ( "3d" );
	d3.appendChild ( doc.createTextNode ( _chartSubType->value().toBool() ? "true" : "false" ));
	entity.appendChild ( d3 );
	
	//color scheme
	QDomElement cs = doc.createElement ( "colorscheme" );
	cs.appendChild ( doc.createTextNode ( _colorScheme->value().toString() ));
	entity.appendChild ( cs );
	
	//aa
	QDomElement aa = doc.createElement ( "antialiased" );
	aa.appendChild ( doc.createTextNode ( _aa->value().toBool() ? "true" : "false" ));
	entity.appendChild ( aa );
	
	//Line Style
	buildXMLLineStyle(doc, entity, lineStyle());
	
	parent.appendChild ( entity );
}

void ReportEntityChart::propertyChanged ( KoProperty::Set &s, KoProperty::Property &p )
{
	kDebug() << s.typeName() << ":" << p.name() << ":" << p.value() << endl;

	//Handle Position
	if ( p.name() == "Position" )
	{
		_pos.setUnitPos ( p.value().value<QPointF>() );
	}
	else if ( p.name() == "Name" )
	{
		//For some reason p.oldValue returns an empty string
		if ( !_rd->isEntityNameUnique ( p.value().toString(), this ) )
		{
			p.setValue ( _oldName );
		}
		else
		{
			_oldName =p.value().toString();
		}
	}
	else if ( p.name() == "ThreeD" )
	{
		set3D ( p.value().toBool() );
	}
	else if ( p.name() == "Antialiased" )
	{
		setAA ( p.value().toBool() );
	}
	else if ( p.name() == "ColorScheme" )
	{
		setColorScheme ( p.value().toString() );
	}
	else if ( p.name() == "DataSource" )
	{
		populateData();
	}
	else if ( p.name() == "XAxis" ||   p.name() == "YAxis")
	{
		setAxis();
	}

	if ( _chartWidget )
	{
		_chartWidget->setType ( ( KDChart::Widget::ChartType ) _chartType->value().toInt() );
		_chartWidget->setSubType ( ( KDChart::Widget::SubType ) _chartSubType->value().toInt() );
		
		setAxis();
	}

	if ( _rd ) _rd->setModified ( true );

	if ( scene() ) scene()->update();

}

void ReportEntityChart::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
	_dataSource->setListData ( _rd->queryList(), _rd->queryList() );
	ReportRectEntity::mousePressEvent ( event );
}
