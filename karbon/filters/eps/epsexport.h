/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __EPSEXPORT_H__
#define __EPSEXPORT_H__

#include <qobject.h>

#include <koFilter.h>
#include <koStore.h>

class EpsExport : public KoFilter
{
	Q_OBJECT

public:
	EpsExport( KoFilter* parent, const char* name, const QStringList& );
	virtual ~EpsExport() {}

	virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );
};

#endif

