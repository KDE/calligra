//
// C++ Implementation: krscriptlabel
//
// Description: 
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "krscriptlabel.h"

KRScriptLabel::KRScriptLabel(KRLabelData *l)
{
	_label = l;
}


KRScriptLabel::~KRScriptLabel()
{
}

QString KRScriptLabel::caption()
{
	return _label->text();
}

void KRScriptLabel::setCaption(const QString& c)
{
	_label->setText(c);
}

