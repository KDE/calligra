/* This file is part of the KDE project
   Copyright (C) 2000 Robert JACOLIN <rjacolin@ifrance.com>

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

#ifndef __XSLTIMPORT_H__
#define __XSLTIMPORT_H__

#include <qstring.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qobject.h>

#include <KoFilter.h>
#include <KoStore.h>

class XSLTImport : public KoFilter
{

	Q_OBJECT

	public:
    	XSLTImport(KoFilter *parent, const char *name, const QStringList&);
		virtual ~XSLTImport() {}

		virtual KoFilter::ConversionStatus convert( const QCString& from,
							const QCString& to );
};

#endif /* __XSLTIMPORT_H__ */
