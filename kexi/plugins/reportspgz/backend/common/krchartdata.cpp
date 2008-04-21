//
// C++ Implementation: krchartdata
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "krchartdata.h"
#include <KoGlobal.h>
#include <kglobalsettings.h>
#include <klocalizedstring.h>

#include <KDChartBarDiagram>
#include <KDChartThreeDBarAttributes>
#include <KDChartLineDiagram>
#include <KDChartThreeDLineAttributes>
#include <KDChartPieDiagram>
#include <KDChartThreeDPieAttributes>
#include <KDChartLegend>
#include <KDChartCartesianAxis>

#include <kexidb/connection.h>
#include <kexidb/cursor.h>
#include <kexidb/parser/parser.h>
#include <koproperty/property.h>

#include <kdebug.h>
typedef QVector<qreal> datalist;

KRChartData::KRChartData ( QDomNode & element )
{
	_conn = 0;
	createProperties();
	
	QDomNodeList nl = element.childNodes();

	QString n;
	QDomNode node;

	for ( int i = 0; i < nl.count(); i++ )
	{
		node = nl.item ( i );
		n = node.nodeName();
		
		kDebug() << node.nodeName() << node.firstChild().nodeValue();
		if ( n == "data" )
		{
			QDomNodeList dnl = node.childNodes();
			for ( int di = 0; di < dnl.count(); di++ )
			{
				//TODO link child, master
				node = dnl.item ( di );
				n = node.nodeName();
				if ( n == "datasource" )
				{
					_dataSource->setValue ( node.firstChild().nodeValue() );
				}
				else
				{
					kDebug() << "while parsing field data encountered and unknown element: " << n << endl;
				}
			}
		}
		else if ( n == "name" )
		{
			_name->setValue(node.firstChild().nodeValue());
		}
		else if ( n == "zvalue" )
		{
			Z = node.firstChild().nodeValue().toDouble();
		}
		else if ( n == "type" )
		{
			_chartType->setValue(node.firstChild().nodeValue().toInt());
		}
		else if ( n == "subtype" )
		{
			_chartSubType->setValue(node.firstChild().nodeValue().toInt());
		}
		else if ( n == "threed" )
		{
			_threeD->setValue(node.firstChild().nodeValue() == "true" ? true : false);
		}
		else if ( n == "colorscheme" )
		{
			_colorScheme->setValue(node.firstChild().nodeValue());
		}
		else if ( n == "antialiased" )
		{
			_aa->setValue(node.firstChild().nodeValue() == "true" ? true : false);
		}
		else if ( n == "xtitle" )
		{
			_xTitle->setValue(node.firstChild().nodeValue());
		}
		else if ( n == "ytitle" )
		{
			_yTitle->setValue(node.firstChild().nodeValue());
		}
		else if ( n == "rect" )
		{
			QRectF r;
			parseReportRect(node.toElement(), r);
			_pos.setPointPos(r.topLeft());
			_size.setPointSize(r.size());
		}
		
		else if (n == "linestyle")
		{
			ORLineStyleData ls;
			if (parseReportLineStyleData( node.toElement(), ls ))
			{
				_lnWeight->setValue(ls.weight);
				_lnColor->setValue(ls.lnColor);
				_lnStyle->setValue(ls.style);
			}
		}
		else
		{
			kDebug() << "while parsing field element encountered unknow element: " << n << endl;
		}
	}

}


KRChartData::~KRChartData()
{
}

void KRChartData::createProperties()
{
	_chartWidget = 0;
	_set = new KoProperty::Set ( 0, "Chart" );

	QStringList strings;
	QList<QVariant> keys;
	QStringList stringkeys;

	_dataSource = new KoProperty::Property ( "DataSource", QStringList(), QStringList(),"", "Data Source" );

	_dataSource->setOption ( "extraValueAllowed", "true" );

	_font = new KoProperty::Property ( "Font", KGlobalSettings::generalFont(), "Font", "Field Font" );

	keys << 1 << 2 << 3 << 4 << 5;
	strings << i18n ( "Bar" ) << i18n ( "Line" ) << i18n ( "Pie" ) << i18n ( "Ring" ) << i18n ( "Polar" );
	KoProperty::Property::ListData *typeData = new KoProperty::Property::ListData ( keys, strings );
	_chartType = new KoProperty::Property ( "Type", typeData, 1, "Chart Type" );

	keys.clear();
	strings.clear();
	keys << 0 << 1 << 2 << 3;
	strings << i18n ( "Normal" ) << i18n ( "Stacked" ) << i18n ( "Percent" ) << i18n ( "Rows" );

	KoProperty::Property::ListData *subData = new KoProperty::Property::ListData ( keys, strings );

	_chartSubType = new KoProperty::Property ( "Sub Type", subData, 0, "Chart Sub Type" );

	keys.clear();
	strings.clear();
	stringkeys << "default" << "rainbow" << "subdued";
	strings << i18n ( "Default" ) << i18n ( "Rainbow" ) << i18n ( "Subdued" );
	_colorScheme = new KoProperty::Property ( "ColorScheme", stringkeys, strings, "default", i18n ( "Color Scheme" ) );

	_threeD = new KoProperty::Property ( "ThreeD", QVariant ( false, 0 ), "3D", "3D" );
	_aa = new KoProperty::Property ( "Antialiased", QVariant ( false, 0 ), "Antialiased", "Antialiased" );

	_xTitle = new KoProperty::Property ( "XTitle", "", "X Axis Title", "X Axis Title" );
	_yTitle = new KoProperty::Property ( "YTitle", "", "Y Axis Title", "Y Axis Title" );

	_lnWeight = new KoProperty::Property ( "Weight", 1, "Line Weight", "Line Weight" );
	_lnColor = new KoProperty::Property ( "LineColor", Qt::black, "Line Color", "Line Color" );
	_lnStyle = new KoProperty::Property ( "LineStyle", Qt::NoPen, "Line Style", "Line Style", KoProperty::LineStyle );

	_set->addProperty ( _name );
	_set->addProperty ( _dataSource );

	_set->addProperty ( _pos.property() );
	_set->addProperty ( _size.property() );
	_set->addProperty ( _chartType );
	_set->addProperty ( _chartSubType );
	_set->addProperty ( _font );
	_set->addProperty ( _colorScheme );
	_set->addProperty ( _threeD );
	_set->addProperty ( _aa );
	_set->addProperty ( _xTitle );
	_set->addProperty ( _yTitle );

	_set->addProperty ( _lnWeight );
	_set->addProperty ( _lnColor );
	_set->addProperty ( _lnStyle );

	set3D ( false );
	setAA ( false );
	setColorScheme ( "default" );
}

void KRChartData::set3D ( bool td )
{
	if ( _chartWidget && _chartWidget->barDiagram() )
	{
		KDChart::BarDiagram *bar = _chartWidget->barDiagram();
		bar->setPen ( QPen ( Qt::black ) );

		KDChart::ThreeDBarAttributes threed = bar->threeDBarAttributes();
		threed.setEnabled ( td );
		threed.setDepth ( 10 );
		threed.setAngle ( 15 );
		threed.setUseShadowColors ( true );
		bar->setThreeDBarAttributes ( threed );
	}

}
void KRChartData::setAA ( bool aa )
{
	if ( _chartWidget && _chartWidget->diagram() )
	{
		_chartWidget->diagram()->setAntiAliasing ( aa );
	}
}

void KRChartData::setColorScheme ( const QString &cs )
{
	if ( _chartWidget && _chartWidget->diagram() )
	{
		if ( cs == "rainbow" )
		{
			_chartWidget->diagram()->useRainbowColors();
		}
		else if ( cs == "subdued" )
		{
			_chartWidget->diagram()->useSubduedColors();
		}
		else
		{
			_chartWidget->diagram()->useDefaultColors();
		}
	}
}

void KRChartData::setConnection ( KexiDB::Connection *c )
{
	_conn = c;
}

void KRChartData::populateData()
{
	KexiDB::Cursor* curs = 0;
	QVector<datalist> data;
	QStringList labels;

	QStringList fn;

	if ( _conn )
	{
		curs = _conn->executeQuery ( _dataSource->value().toString() );

		if ( curs )
		{
			fn = fieldNamesHackUntilImprovedParser ( _dataSource->value().toString() );
			
			//resize the data lists to match the number of columns
			int cols = fn.count() -1;
			data.resize(cols);
			
			
			if ( _chartWidget )
				delete _chartWidget;
			_chartWidget = new KDChart::Widget();
			_chartWidget->setType ( ( KDChart::Widget::ChartType ) _chartType->value().toInt() );
			_chartWidget->setSubType ( ( KDChart::Widget::SubType ) _chartSubType->value().toInt() );
			set3D ( _threeD->value().toBool() );
			setAA ( _aa->value().toBool() );
			setColorScheme ( _colorScheme->value().toString() );

			curs->moveFirst();
			while ( !curs->eof() )
			{
				labels << curs->value ( 0 ).toString();
				
				for (int i = 1; i <= cols; ++i)
				{
					data[i - 1] << curs->value ( i ).toDouble();
				}
				curs->moveNext();
			}
			kDebug() << labels;

			for (int i = 1; i <= cols; ++i)
			{
				_chartWidget->setDataset ( i - 1, data[i - 1], fn[i] );
			}
			

			//Add the legend
			_chartWidget->addLegend ( KDChart::Position::East );
			_chartWidget->legend()->setOrientation ( Qt::Horizontal );
			_chartWidget->legend()->setTitleText ( "Legend" );
			for ( unsigned int i = 1; i < fn.count(); ++i )
			{
				_chartWidget->legend()->setText ( i - 1, fn[i] );
			}

			_chartWidget->legend()->setShowLines ( true );

			//Add the axis
			setAxis();

			//Add the bottom labels
			if ( _chartWidget->barDiagram() || _chartWidget->lineDiagram() )
			{
				KDChart::AbstractCartesianDiagram *dia = dynamic_cast<KDChart::AbstractCartesianDiagram*> ( _chartWidget->diagram() );

				foreach ( KDChart::CartesianAxis* axis, dia->axes() )
				{
					if ( axis->position() == KDChart::CartesianAxis::Bottom )
					{
						axis->setLabels ( labels );
					}
				}
			}
		}
		else
		{
			kDebug() << "Cursor was invalid: " << _dataSource->value().toString() << endl;
		}
	}
	else
	{
		kDebug() << "No connection!" << endl;
	}
}

QStringList KRChartData::fieldNames ( const QString &stmt )
{
	KexiDB::Parser *pars;

	pars = new KexiDB::Parser ( _conn );

	pars->setOperation ( KexiDB::Parser::OP_Select );
	pars->parse ( stmt );

	KexiDB::QuerySchema *qs = pars->select();

	if ( qs )
	{
		kDebug() << "Parsed OK " << qs->fieldCount();
		KexiDB::Field::List fl = qs->fields();
		QStringList fn;

		for ( unsigned int i = 0; i < qs->fieldCount(); ++i )
		{
			fn << qs->field ( i )->name();
		}
		return fn;
	}
	else
	{
		kDebug() << "Unable to parse statement";
		return QStringList();
	}

}

QStringList KRChartData::fieldNamesHackUntilImprovedParser ( const QString &stmt )
{
	QStringList fn;
	QString s = stmt.mid ( 6, stmt.indexOf ( "from", 0, Qt::CaseInsensitive ) - 6 ).simplified();
	kDebug() << s;

	fn = s.split ( "," );
	return fn;
}


void KRChartData::setAxis()
{
	if ( _chartWidget->barDiagram() || _chartWidget->lineDiagram() )
	{
		KDChart::AbstractCartesianDiagram *dia = dynamic_cast<KDChart::AbstractCartesianDiagram*> ( _chartWidget->diagram() );
		KDChart::CartesianAxis *xAxis = 0;
		KDChart::CartesianAxis *yAxis = 0;

		//delete existing axis
		foreach ( KDChart::CartesianAxis* axis, dia->axes() )
		{
			if ( axis->position() == KDChart::CartesianAxis::Bottom )
			{
				xAxis = axis;
			}
			if ( axis->position() == KDChart::CartesianAxis::Left )
			{
				yAxis = axis;
			}
		}



		if ( !xAxis )
		{
			xAxis =  new KDChart::CartesianAxis ( dynamic_cast<KDChart::AbstractCartesianDiagram*> ( _chartWidget->diagram() ) );
			xAxis->setPosition ( KDChart::CartesianAxis::Bottom );
			dia->addAxis ( xAxis );
		}

		if ( !yAxis )
		{
			yAxis = new KDChart::CartesianAxis ( dynamic_cast<KDChart::AbstractCartesianDiagram*> ( _chartWidget->diagram() ) );
			yAxis->setPosition ( KDChart::CartesianAxis::Left );
			dia->addAxis ( yAxis );
		}

		xAxis->setTitleText ( _xTitle->value().toString() );
		yAxis->setTitleText ( _yTitle->value().toString() );
	}
}

ORLineStyleData KRChartData::lineStyle()
{
	ORLineStyleData ls;
	ls.weight = _lnWeight->value().toInt();
	ls.lnColor = _lnColor->value().value<QColor>();
	ls.style = (Qt::PenStyle)_lnStyle->value().toInt();
	return ls;
}

// RTTI
int KRChartData::type() const { return RTTI; }
int KRChartData::RTTI = KRObjectData::EntityChart;
KRChartData * KRChartData::toChart() { return this; }
