/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __EPSIMPORT_H__
#define __EPSIMPORT_H__

#include <qobject.h>

#include <koFilter.h>

class QDomElement;
class QTextStream;

class EpsImport : public KoFilter
{
	Q_OBJECT

public:
	EpsImport( KoFilter* parent, const char* name, const QStringList& );
	virtual ~EpsImport() {}

	virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );
};

#endif

