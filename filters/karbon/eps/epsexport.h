/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __EPSEXPORT_H__
#define __EPSEXPORT_H__

#include <qobject.h>

#include <koFilter.h>

#include "vvisitor.h"

class QTextStream;
class VDocument;
class VGroup;
class VLayer;
class VPath;
class VSegmentList;

class EpsExport : public KoFilter, private VVisitor
{
	Q_OBJECT

public:
	EpsExport( KoFilter* parent, const char* name, const QStringList& );
	virtual ~EpsExport() {}

	virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );

private:
	virtual void visitVDocument( VDocument& document );
	virtual void visitVGroup( VGroup& group );
	virtual void visitVLayer( VLayer& layer );
	virtual void visitVPath( VPath& path );
	virtual void visitVSegmentList( VSegmentList& segmentList );

	QTextStream* m_stream;
};

#endif

