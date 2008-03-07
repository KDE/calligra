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
#include "krlabeldata.h"
#include <koproperty/property.h>
#include <koproperty/set.h>
#include <KoGlobal.h>
#include <kdebug.h>
#include <klocalizedstring.h>
#include <kglobalsettings.h>

KRLabelData::KRLabelData ( QDomNode & element )
{
	createProperties();
	QDomNodeList nl = element.childNodes();
	QString n;
	QDomNode node;
	for ( int i = 0; i < nl.count(); i++ )
	{
		node = nl.item ( i );
		n = node.nodeName();
		if ( n == "string" )
		{
			_text->setValue ( node.firstChild().nodeValue() );
		}
		else if ( n == "zvalue" )
		{
			Z = node.firstChild().nodeValue().toDouble();
		}
		else if ( n == "left" )
		{
			_hAlignment->setValue ( "Left" );
		}
		else if ( n == "hcenter" )
		{
			_hAlignment->setValue ( "Center" );
		}
		else if ( n == "right" )
		{
			_hAlignment->setValue ( "Right" );
		}
		else if ( n == "top" )
		{
			_vAlignment->setValue ( "Top" );
		}
		else if ( n == "vcenter" )
		{
			_vAlignment->setValue ( "Center" );
		}
		else if ( n == "bottom" )
		{
			_vAlignment->setValue ( "Bottom" );
		}
		else if ( n == "rect" )
		{
			QRectF r;
			parseReportRect(node.toElement(), r);
			_pos.setPointPos(r.topLeft());
			_size.setPointSize(r.size());
		}
		else if ( n == "textstyle" )
		{
			
			ORTextStyleData ts;
			if (parseReportTextStyleData(node.toElement(), ts))
			{
				_bgColor->setValue(ts.bgColor);
				_fgColor->setValue(ts.fgColor);
				_bgOpacity->setValue(ts.bgOpacity);
				_font->setValue(ts.font);
				
			}
		}
		else if (n == "linestyle")
		{
			ORLineStyleData ls;
			if (parseReportLineStyleData( node.toElement(), ls ))
			{
				_lnweight->setValue(ls.weight);
				_lncolor->setValue(ls.lnColor);
				_lnstyle->setValue(ls.style);
			}
		}
		else
		{
			kDebug() << "while parsing label element encountered unknow element: " << n << endl;
		}
	}
}

void KRLabelData::setTextFlags ( Qt::Alignment f )
{
	
}

QString KRLabelData::text() const
{
	return _text->value().toString();
}

void KRLabelData::setText ( const QString& t )
{
	_text->setValue ( t );
}

void KRLabelData::createProperties()
{
	_set = new KoProperty::Set ( 0, "Label" );

	_text = new KoProperty::Property ( "Caption", "Label", "Caption", "Label Caption" );
	QStringList keys, strings;

	keys << "Left" << "Center" << "Right";
	strings << i18n ( "Left" ) << i18n ( "Center" ) << i18n ( "Right" );
	_hAlignment = new KoProperty::Property ( "HAlign", keys, strings, "Left", "Horizontal Alignment" );

	keys.clear();
	strings.clear();
	keys << "Top" << "Center" << "Bottom";
	strings << i18n ( "Top" ) << i18n ( "Center" ) << i18n ( "Bottom" );
	_vAlignment = new KoProperty::Property ( "VAlign", keys, strings, "Center", "Vertical Alignment" );

	_font = new KoProperty::Property ( "Font", KGlobalSettings::generalFont(), "Font", "Label Font" );

	_bgColor = new KoProperty::Property ( "BackgroundColor", Qt::white, "Background Color", "Background Color" );
	_fgColor = new KoProperty::Property ( "ForegroundColor", Qt::black, "Foreground Color", "Foreground Color" );
	_bgOpacity = new KoProperty::Property ( "Opacity", 255, "Opacity", "Opacity" );
	_bgOpacity->setOption("max", 255);
	_bgOpacity->setOption("min", 0);
	
	_lnweight = new KoProperty::Property ( "Weight", 1, "Line Weight", "Line Weight" );
	_lncolor = new KoProperty::Property ( "LineColor", Qt::black, "Line Color", "Line Color" );
	_lnstyle = new KoProperty::Property ( "LineStyle", Qt::NoPen, "Line Style", "Line Style", KoProperty::LineStyle );
	
	_set->addProperty ( _text );
	_set->addProperty ( _hAlignment );
	_set->addProperty ( _vAlignment );
	_set->addProperty ( _pos.property() );
	_set->addProperty ( _size.property() );
	_set->addProperty ( _font );
	_set->addProperty ( _bgColor );
	_set->addProperty ( _fgColor );
	_set->addProperty ( _bgOpacity );
	_set->addProperty ( _lnweight );
	_set->addProperty ( _lncolor );
	_set->addProperty ( _lnstyle );
}

QRectF KRLabelData::_rect()
{
	QRectF r;
	r.setRect(_pos.toScene().x(),_pos.toScene().y(), _size.toScene().width(), _size.toScene().height());	
	return r;
}

Qt::Alignment KRLabelData::textFlags() const
{
	Qt::Alignment align;
	QString t;
	t = _hAlignment->value().toString();
	if ( t == "Center" )
		align = Qt::AlignHCenter;
	else if ( t == "Right" )
		align = Qt::AlignRight;
	else
		align = Qt::AlignLeft;

	t = _vAlignment->value().toString();
	if ( t == "Center" )
		align |= Qt::AlignVCenter;
	else if ( t == "Bottom" )
		align |= Qt::AlignBottom;
	else
		align |= Qt::AlignTop;

	return align;
}

ORTextStyleData KRLabelData::textStyle()
{
	ORTextStyleData d;
	d.bgColor = _bgColor->value().value<QColor>();
	d.fgColor = _fgColor->value().value<QColor>();
	d.font = _font->value().value<QFont>();
	d.bgOpacity = _bgOpacity->value().toInt();
	return d;
}

ORLineStyleData KRLabelData::lineStyle()
{
	ORLineStyleData ls;
	ls.weight = _lnweight->value().toInt();
	ls.lnColor = _lncolor->value().value<QColor>();
	ls.style = (Qt::PenStyle)_lnstyle->value().toInt();
	return ls;
}

// RTTI
int KRLabelData::type() const { return RTTI; }
int KRLabelData::RTTI = KRObjectData::EntityLabel;
KRLabelData * KRLabelData::toLabel() { return this; }