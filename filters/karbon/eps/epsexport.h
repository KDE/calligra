/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __EPSEXPORT_H__
#define __EPSEXPORT_H__

#include <qobject.h>

#include <koFilter.h>

class QTextStream;
class VDocument;
class VGroup;
class VLayer;
class VPath;
class VSegmentList;

class EpsExport : public KoFilter
{
	Q_OBJECT

public:
	EpsExport( KoFilter* parent, const char* name, const QStringList& );
	virtual ~EpsExport() {}

	virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );

private:
	void exportDocument( const VDocument& document );
	void exportGroup( const VGroup& group );
	void exportLayer( const VLayer& layer );
	void exportPath( const VPath& path );
	void exportSegmentList( const VSegmentList& segmentList );

	QTextStream* m_stream;
};

#endif

