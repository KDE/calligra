/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __SVGEXPORT_H__
#define __SVGEXPORT_H__

#include <koFilter.h>

#include "vvisitor.h"


class QTextStream;
class VColor;
class VDocument;
class VFill;
class VGroup;
class VLayer;
class VPath;
class VSegmentList;
class VStroke;
class VText;


class SvgExport : public KoFilter, private VVisitor
{
	Q_OBJECT

public:
	SvgExport( KoFilter* parent, const char* name, const QStringList& );
	virtual ~SvgExport() {}

	virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );

private:
	virtual void visitVDocument( VDocument& document );
	virtual void visitVPath( VPath& path );
	virtual void visitVSegmentList( VSegmentList& segmentList );
	virtual void visitVText( VText& text );

	void getStroke( const VStroke& stroke );
	void getFill( const VFill& fill  );
	void getHexColor( const VColor& color  );

	QTextStream* m_stream;
};

#endif

