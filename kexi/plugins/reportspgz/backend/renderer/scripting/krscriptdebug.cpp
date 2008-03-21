//
// C++ Implementation: krscriptdebug
//
// Description: 
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "krscriptdebug.h"
#include <kdebug.h>

KRScriptDebug::KRScriptDebug(QObject *parent)
 : QObject(parent)
{
}


KRScriptDebug::~KRScriptDebug()
{
}

void KRScriptDebug::print(const QString& s)
{
	kDebug() << s << endl;
}


