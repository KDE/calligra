/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __SVGEXPORT_H__
#define __SVGEXPORT_H__

#include <qobject.h>

#include <koFilter.h>

class QDomElement;
class QTextStream;

class SvgExport : public KoFilter
{
	Q_OBJECT

public:
	SvgExport( KoFilter* parent, const char* name, const QStringList& );
	virtual ~SvgExport() {}

	virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );

private:
	void exportDocument( QTextStream& s, const QDomElement& node );
	void exportLayer( QTextStream& s, const QDomElement& node );
	void exportPath( QTextStream& s, const QDomElement& node );
	void exportSegments( QTextStream& s, const QDomElement& node );
	void exportStroke( QTextStream& s, const QDomElement& node );
	void exportFill( QTextStream& s, const QDomElement& node );
	void getHexColor( QTextStream& s, const QDomElement& node );
	void exportText( QTextStream& s, const QDomElement& node );
	int fill_rule;
};

#endif

