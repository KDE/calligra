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

KRScriptSection::KRScriptSection ( KRSectionData* sec )
{
	_section = sec;
}


KRScriptSection::~KRScriptSection()
{
}

QColor KRScriptSection::backgroundColor()
{
	return _section->_bgColor->value().value<QColor>();
}

void   KRScriptSection::setBackgroundColor ( const QString& c)
{
	_section->_bgColor->setValue(QColor(c));
}

qreal KRScriptSection::height()
{
	return _section->_height->value().toDouble();
}

void KRScriptSection::setHeight ( qreal h)
{
	_section->_height->setValue(h);
}

QString KRScriptSection::name()
{
	return _section->_name;
}

QObject* KRScriptSection::object(int i)
{
	switch (_section->_objects[i]->type())
	{
		case KRObjectData::EntityLabel:
			return new KRScriptLabel(_section->_objects[i]->toLabel());
			break;
		case KRObjectData::EntityField:
			return new KRScriptField(_section->_objects[i]->toField());
			break;
		default:
			return new QObject();
	}
	
}

QObject* KRScriptSection::object(const QString&)
{
	
}