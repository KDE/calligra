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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __XAMLIMPORT_H__
#define __XAMLIMPORT_H__

#include <KoFilter.h>
#include <qdom.h>
#include <qmap.h>
#include <q3ptrstack.h>
//Added by qt3to4:
#include <Q3CString>
#include <core/vdocument.h>
#include <core/vgradient.h>
#include <core/vfill.h>
#include <core/vstroke.h>
#include <core/vfillrule.h>
#include "xamlgraphiccontext.h"

class VGroup;
class VPath;

class XAMLImport : public KoFilter
{
	Q_OBJECT

public:
	XAMLImport(KoFilter *parent, const char *name, const QStringList&);
	virtual ~XAMLImport();

	virtual KoFilter::ConversionStatus convert(const QByteArray& from, const QByteArray& to);

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
		QMatrix	gradientTransform;
	};

	void parseGroup( VGroup *, const QDomElement & );
	void parseStyle( VObject *, const QDomElement & );
	void parsePA( VObject *, XAMLGraphicsContext *, const QString &, const QString & );
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
	void createText( VGroup *, const QDomElement & );
	void parseFont( const QDomElement & );
	// find object with given id in document
	VObject* findObject( const QString &name );
	// find object with given id in given group
	VObject* findObject( const QString &name, VGroup * );

private:
	VDocument						m_document;
	Q3PtrStack<XAMLGraphicsContext>	m_gc;
	QMap<QString, GradientHelper>	m_gradients;
	QMap<QString, QDomElement>		m_paths;
	KoRect							m_outerRect;
};

#endif
