//
// C++ Implementation: krscriptfield
//
// Description: 
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "krscriptfield.h"

KRScriptField::KRScriptField(KRFieldData *f)
{
	_field = f;
}


KRScriptField::~KRScriptField()
{
}

QString KRScriptField::source()
{
	return _field->column();
}

void KRScriptField::setSource(const QString& s)
{
	_field->setColumn(s);
}
		
void KRScriptField::setBackgroundOpacity(int o)
{
	_field->_bgOpacity->setValue(o);
}
