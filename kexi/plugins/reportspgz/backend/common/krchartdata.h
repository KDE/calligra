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
#ifndef KRCHARTDATA_H
#define KRCHARTDATA_H

#include "krobjectdata.h"
#include <QRect>
#include <qdom.h>
#include "krsize.h"
#include "krpos.h"
#include <parsexmlutils.h>
#include <KDChartWidget>

namespace KexiDB
{
	class Connection;
	class Cursor;
}

/**
	@author Adam Pigg <adam@piggz.co.uk>
*/

namespace Scripting
{
	class Chart;
}

class KRChartData : public KRObjectData
{
	public:
		KRChartData ( ){_conn = 0; createProperties();}
		KRChartData ( QDomNode & element );
		~KRChartData();
		virtual KRChartData * toChart();
		virtual int type() const;
		KDChart::Widget *widget(){return _chartWidget;}
		void populateData();
		void setConnection(KexiDB::Connection*);
	protected:
		KRSize _size;	
		KoProperty::Property * _dataSource;
		KoProperty::Property * _font;
		KoProperty::Property * _chartType;
		KoProperty::Property * _chartSubType;
		KoProperty::Property * _threeD;
		KoProperty::Property * _colorScheme;
		KoProperty::Property * _aa;
		KoProperty::Property * _xTitle;
		KoProperty::Property * _yTitle;
		
		KoProperty::Property *_bgColor;
		KoProperty::Property* _displayLegend;
		
		//KoProperty::Property* _lnWeight;
		//KoProperty::Property* _lnStyle;
		
		//ORLineStyleData lineStyle();
		
		KDChart::Widget *_chartWidget;
		
		void set3D ( bool );
		void setAA ( bool );
		void setColorScheme ( const QString & );
		void setAxis( const QString&, const QString& );
		void setBackgroundColor(const QColor&);
		void setLegend ( bool );
			
		QStringList fieldNames(const QString &);
		QStringList fieldNamesHackUntilImprovedParser(const QString &);
		
		
	private:
		virtual void createProperties();
		static int RTTI;
		KexiDB::Connection* _conn;
		
		friend class ORPreRenderPrivate;
		friend class Scripting::Chart;
		
		KexiDB::Cursor *dataSet();

};

#endif
