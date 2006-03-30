/* This file is part of the KDE project
   Copyright (C) 2002, 2003 The Karbon Developers

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __SVGEXPORT_H__
#define __SVGEXPORT_H__

#include <KoFilter.h>

#include "vvisitor.h"
#include "vgradient.h"

#include "svggraphiccontext.h"

#include <q3ptrstack.h>
//Added by qt3to4:
#include <QTextStream>
#include <Q3CString>

class QTextStream;
class VColor;
class VPath;
class VDocument;
class VFill;
class VGroup;
class VImage;
class VLayer;
class VSubpath;
class VStroke;
class VText;
class VTransformCmd;


class SvgExport : public KoFilter, private VVisitor
{
	Q_OBJECT

public:
	SvgExport( KoFilter* parent, const char* name, const QStringList& );
	virtual ~SvgExport() {}

	virtual KoFilter::ConversionStatus convert( const QByteArray& from, const QByteArray& to );

private:
	virtual void visitVPath( VPath& composite );
	virtual void visitVDocument( VDocument& document );
	virtual void visitVGroup( VGroup& group );
	virtual void visitVImage( VImage& image );
	virtual void visitVLayer( VLayer& layer );
	virtual void visitVSubpath( VSubpath& path );
	virtual void visitVText( VText& text );

	void getStroke( const VStroke& stroke, QTextStream *stream );
	void getColorStops( const Q3PtrVector<VColorStop> &colorStops );
	void getFill( const VFill& fill, QTextStream *stream );
	void getGradient( const VGradient& grad );
	void getPattern( const VPattern& patt );
	void getHexColor( QTextStream *, const VColor& color  );
	QString getID( VObject *obj );

	void writePathToStream( VPath &composite, const QString &id, QTextStream *stream, unsigned int indent );

	QTextStream* m_stream;
	QTextStream* m_defs;
	QTextStream* m_body;

	Q3PtrStack<SvgGraphicsContext>	m_gc;

	unsigned int m_indent;
	unsigned int m_indent2;

	VTransformCmd *m_trans;
};

#endif

