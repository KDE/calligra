/*
 * Kexi report writer and rendering engine
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

//
// KRSectionData is used to store the information about a specific
// section. 
// A section has a name and optionally extra data. `name'
// rpthead, rptfoot, pghead, pgfoot, grphead, grpfoot or detail.
// In the case of pghead and pgfoot extra would contain the page
// designation (firstpage, odd, even or lastpage).
//
#ifndef KRSECTIONDATA_H
#define KRSECTIONDATA_H

#include <QObject>
#include <qdom.h>

class KRObjectData;

#include <koproperty/property.h>
#include <koproperty/set.h>
#include <QColor>

namespace Scripting
{
	class Section;
}
class KRSectionData : public QObject
{
	Q_OBJECT
	public:
		enum Section
		{
		  None = 0,
		  PageHeadFirst = 1,
		  PageHeadOdd,
		  PageHeadEven,
		  PageHeadLast,
		  PageHeadAny,
		  ReportHead,
		  ReportFoot,
		  PageFootFirst,
		  PageFootOdd,
		  PageFootEven,
		  PageFootLast,
		  PageFootAny,
		  GroupHead,
		  GroupFoot,
		  Detail
		};
	  
		KRSectionData();
		KRSectionData(const QDomElement &);
		~KRSectionData();
		KoProperty::Set* properties(){return _set;}
	
		bool isValid() const {return _valid;}
		
		/**
		set the 'extra' info for the section.  This is used
		to uniquely identity the section, eg, odd/even footers,
		or multiple group heads
		*/
		void setExtra(const QString &e){_extra = e;}
		
		QString extra() const {return _extra;}
		qreal height() const {return _height->value().toDouble();}
		QList<KRObjectData*> objects() const {return _objects;};
		QString name() const;
		QColor bgColor() const { return _bgColor->value().value<QColor>();}
		Section type() const {return _type;}
		
	protected:
		KoProperty::Set *_set;
		KoProperty::Property *_height;
		KoProperty::Property *_bgColor;
		
	public slots:
		KoProperty::Set& propertySet(){return *_set;}
		
	private:
		void createProperties();
		
		QList<KRObjectData*> _objects;
		//QList<ORDataData> trackTotal;
		
		QString _name;
		QString _extra;
		Section _type;
		
		static bool zLessThan(KRObjectData* s1, KRObjectData* s2);
		
		bool _valid;
		
	friend class Scripting::Section;
	friend class ReportSection;
};

#endif
