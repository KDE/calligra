/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __EPSEXPORT_H__
#define __EPSEXPORT_H__

#include <qobject.h>

#include <koFilter.h>

class QDomElement;
class QTextStream;

class EpsExport : public KoFilter
{
	Q_OBJECT

public:
	EpsExport( KoFilter* parent, const char* name, const QStringList& );
	virtual ~EpsExport() {}

	virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );

private:
	void exportDocument( QTextStream& s, const QDomElement& node );
	void exportLayer( QTextStream& s, const QDomElement& node );
	void exportPath( QTextStream& s, const QDomElement& node );
	void exportSegments( QTextStream& s, const QDomElement& node );
};

#endif

