//
// C++ Interface: krchartdata
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
		
		KRPos _pos;
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
		KoProperty::Property* _lnColor;
		KoProperty::Property* _lnWeight;
		KoProperty::Property* _lnStyle;
		
		ORLineStyleData lineStyle();
		
		KDChart::Widget *_chartWidget;
		
		void set3D ( bool );
		void setAA ( bool );
		void setColorScheme ( const QString & );
		void setAxis();
		
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
