/* This file is part of the KDE project
   Copyright (c) 2003 Rob Buis <buis@kde.org>

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

#ifndef OODRAW_IMPORT_H__
#define OODRAW_IMPORT_H__

#include <KoFilter.h>
#include <KoStore.h>

#include <qdom.h>
#include <q3dict.h>
#include <qcolor.h>
//Added by qt3to4:
#include <Q3CString>
#include <KoStyleStack.h>
#include <core/vdocument.h>
#include <core/vcomposite.h>
#include <core/vstroke.h>

class KZip;

class VGroup;

class OoDrawImport : public KoFilter
{
    Q_OBJECT
public:
    OoDrawImport( KoFilter *parent, const char *name, const QStringList & );
    virtual ~OoDrawImport();

    virtual KoFilter::ConversionStatus convert( QByteArray const & from, QByteArray const & to );

private:
	void createDocumentInfo( QDomDocument &docinfo );

	void createStyleMap( QDomDocument &docstyles );
	void insertStyles( const QDomElement& styles );
	void insertDraws( const QDomElement& styles );
	void fillStyleStack( const QDomElement& object );
	void addStyles( const QDomElement* style );
	void storeObjectStyles( const QDomElement& object );
	void appendPen( VObject &obj );
	void appendBrush( VObject &obj );
	void appendPoints(VPath &path, const QDomElement& object);
	void convert();
	void parseGroup( VGroup *parent, const QDomElement& object );
	void parseColor( VColor &color, const QString &s );
	double ymirror( double y );
	KoRect parseViewBox( const QDomElement& object );

	KoFilter::ConversionStatus openFile();
	KoFilter::ConversionStatus loadAndParse(const QString& filename, QDomDocument& doc);

	VDocument			m_document;
	QDomDocument			m_content;
	QDomDocument			m_meta;
	QDomDocument			m_settings;
	Q3Dict<QDomElement>		m_styles, m_draws;
	KoStyleStack			m_styleStack;
	KZip * m_zip;

};

#endif
