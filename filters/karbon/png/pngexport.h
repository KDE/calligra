/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __PNGEXPORT_H__
#define __PNGEXPORT_H__

#include <koFilter.h>

#include "vvisitor.h"

class PngExport : public KoFilter, private VVisitor
{
	Q_OBJECT

public:
	PngExport( KoFilter* parent, const char* name, const QStringList& );
	virtual ~PngExport() {}

	virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );
};

#endif

