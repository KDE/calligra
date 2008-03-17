//
// C++ Implementation: krscriptsection
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "krscriptsection.h"
#include "krscriptlabel.h"
#include "krscriptfield.h"

namespace Scripting
{
	Section::Section ( KRSectionData* sec )
	{
		_section = sec;
	}


	Section::~Section()
	{
	}

	QColor Section::backgroundColor()
	{
		return _section->_bgColor->value().value<QColor>();
	}

	void   Section::setBackgroundColor ( const QString& c)
	{
		_section->_bgColor->setValue(QColor(c));
	}

	qreal Section::height()
	{
		return _section->_height->value().toDouble();
	}

	void Section::setHeight ( qreal h)
	{
		_section->_height->setValue(h);
	}

	QString Section::name()
	{
		return _section->_name;
	}

	QObject* Section::object(int i)
	{
		switch (_section->_objects[i]->type())
		{
			case KRObjectData::EntityLabel:
				return new Scripting::Label(_section->_objects[i]->toLabel());
				break;
			case KRObjectData::EntityField:
				return new Scripting::Field(_section->_objects[i]->toField());
				break;
			default:
				return new QObject();
		}
	
	}

	QObject* Section::object(const QString&)
	{
	
	}

}