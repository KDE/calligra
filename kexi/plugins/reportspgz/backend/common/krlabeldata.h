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
#ifndef KRLABEL_H
#define KRLABEL_H
#include "krobjectdata.h"
#include "parsexmlutils.h"
#include "../../pgzkexireportpart2_export.h"
#include <QRect>
#include <QPainter>
#include <qdom.h>
#include "krpos.h"
#include "krsize.h"

/**
	@author 
*/
class PGZKEXIREPORTPART2_LIB_EXPORT KRLabelData : public KRObjectData
{
	public:
		KRLabelData(){createProperties();};
		KRLabelData ( QDomNode & element );
		~KRLabelData(){};
		virtual int type() const;
		virtual KRLabelData * toLabel();
	
		QString text() const;
		QFont font() const{return _font->value().value<QFont>();}
		Qt::Alignment textFlags() const;
		void setTextFlags ( Qt::Alignment );
		void setText ( const QString& );
		ORTextStyleData textStyle();
		ORLineStyleData lineStyle();
		
	protected:
		QRectF _rect();
				
		KRPos _pos;
		KRSize _size;
		KoProperty::Property *_text;
		KoProperty::Property* _hAlignment;
		KoProperty::Property* _vAlignment;
		KoProperty::Property* _font;
		KoProperty::Property* _fgColor;
		KoProperty::Property* _bgColor;
		KoProperty::Property* _bgOpacity;
		KoProperty::Property* _lncolor;
		KoProperty::Property* _lnweight;
		KoProperty::Property* _lnstyle;

	private:
		virtual void createProperties();
		static int RTTI;
		
		friend class ORPreRenderPrivate;
};
#endif
