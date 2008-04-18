//
// C++ Interface: reportentitychart
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef REPORTENTITYCHART_H
#define REPORTENTITYCHART_H

#include <QObject>
#include <krchartdata.h>
#include "reportrectentity.h"
/**
	@author Adam Pigg <adam@piggz.co.uk>
*/
class ReportEntityChart : public QObject, public ReportRectEntity, public KRChartData
{
		Q_OBJECT
	public:
		ReportEntityChart ( ReportDesigner *, QGraphicsScene* scene );
		ReportEntityChart ( QDomNode & element, ReportDesigner *, QGraphicsScene* scene );
		
		virtual ~ReportEntityChart();
		virtual void buildXML ( QDomDocument & doc, QDomElement & parent );
		virtual void paint ( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget=0 );
		virtual ReportEntityChart* clone();
	
	protected:
		virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
		
	private:
		void init(QGraphicsScene*, ReportDesigner *r);
	private slots:
		void propertyChanged ( KoProperty::Set &, KoProperty::Property & );	

};

#endif
