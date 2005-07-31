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

#ifndef __SVGIMPORT_H__
#define __SVGIMPORT_H__

#include <koFilter.h>
#include <qdom.h>
#include <qmap.h>
#include <qptrstack.h>
#include <core/vdocument.h>
#include <core/vgradient.h>
#include <core/vfill.h>
#include <core/vstroke.h>
#include <core/vfillrule.h>
#include "svggraphiccontext.h"

class VGroup;
class VPath;

class SvgImport : public KoFilter
{
	Q_OBJECT

public:
	SvgImport(KoFilter *parent, const char *name, const QStringList&);
	virtual ~SvgImport();

	virtual KoFilter::ConversionStatus convert(const QCString& from, const QCString& to);

protected:
	class GradientHelper
	{
	public:
		GradientHelper()
		{
			bbox = true;
		}
		VGradient	gradient;
		bool		bbox;
		QWMatrix	gradientTransform;
	};

	void parseGroup( VGroup *, const QDomElement & );
	void parseStyle( VObject *, const QDomElement & );
	void parsePA( VObject *, SvgGraphicsContext *, const QString &, const QString & );
	void parseGradient( const QDomElement & );
	void parseColorStops( VGradient *, const QDomElement & );
	double parseUnit( const QString &, bool horiz = false, bool vert = false, KoRect bbox = KoRect() );
	void parseColor( VColor &, const QString & );
	QColor parseColor( const QString & );
	double toPercentage( QString );
	double fromPercentage( QString );
	void setupTransform( const QDomElement & );
	void addGraphicContext();
	QDomDocument inpdoc;
	QDomDocument outdoc;
	void convert();
	VObject* createObject( const QDomElement & );

private:
	VDocument						m_document;
	QPtrStack<SvgGraphicsContext>	m_gc;
	QMap<QString, GradientHelper>	m_gradients;
	QMap<QString, QDomElement>		m_paths;
	KoRect							m_outerRect;
};

#endif
