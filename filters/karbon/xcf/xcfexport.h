/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __XCFEXPORT_H__
#define __XCFEXPORT_H__


#include <koFilter.h>

#include "vvisitor.h"


class QDataStream;
class VDocument;
class VLayer;


class XcfExport : public KoFilter, private VVisitor
{
	Q_OBJECT

public:
	XcfExport( KoFilter* parent, const char* name, const QStringList& );
	virtual ~XcfExport() {}

	virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );
	
private:
	virtual void visitVDocument( VDocument& document );
	virtual void visitVLayer( VLayer& layer );
	
	QDataStream* m_stream;
};

#endif

