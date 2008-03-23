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
#ifndef KRTEXTDATA_H
#define KRTEXTDATA_H
#include "krobjectdata.h"
#include <QRect>
#include <QPainter>
#include <qdom.h>
#include "krpos.h"
#include "krsize.h"
#include <parsexmlutils.h>
/**
	@author 
*/
class KRTextData : public KRObjectData
{
	public:
		KRTextData(){createProperties();};
		KRTextData ( QDomNode & element);
		~KRTextData();
		virtual KRTextData * toText();
		virtual int type() const;
		
		void setTextFlags ( Qt::Alignment );
		Qt::Alignment textFlags() const;
		QFont font() const {return _font->value().value<QFont>();}
		ORDataData data() {return ORDataData("Data Source"/*_query->value().toString()*/, _controlSource->value().toString());}
		
		void setColumn ( const QString& );
		void setBottomPadding ( qreal bp );
		qreal bottomPadding() const;

		QString column() const;
		ORTextStyleData textStyle();
		ORLineStyleData lineStyle();
		
	protected:
		QRect _rect();
		KRPos _pos;
		KRSize _size;
		KoProperty::Property * _controlSource;
		KoProperty::Property* _hAlignment;
		KoProperty::Property* _vAlignment;
		KoProperty::Property* _font;
		KoProperty::Property* _fgColor;
		KoProperty::Property* _bgColor;
		KoProperty::Property* _bgOpacity;
		KoProperty::Property* _lncolor;
		KoProperty::Property* _lnweight;
		KoProperty::Property* _lnstyle;
		
		qreal bpad;
		
	private:
		virtual void createProperties();
		static int RTTI;
		
		friend class ORPreRenderPrivate;
};

#endif
