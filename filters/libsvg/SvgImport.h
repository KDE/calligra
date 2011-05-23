/* This file is part of the KDE project
 * Copyright (C) 2002-2003,2005 Rob Buis <buis@kde.org>
 * Copyright (C) 2005-2006 Tim Beaulen <tbscope@gmail.com>
 * Copyright (C) 2005,2007-2009 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SVGIMPORT_H
#define SVGIMPORT_H

#include <KoFilter.h>
#include <KoXmlReaderForward.h>
#include <QtCore/QVariant>

class KoShape;
class KarbonDocument;
class QDomElement;

class SvgImport : public KoFilter
{
    Q_OBJECT

public:
    SvgImport(QObject* parent, const QVariantList&);
    virtual ~SvgImport();

    virtual KoFilter::ConversionStatus convert(const QByteArray& from, const QByteArray& to);

protected:

    /// The main entry point for the conversion
    void convert(const KoXmlElement &rootElement);

    /// Builds the document from the given shapes list
    void buildDocument(const QList<KoShape*> &toplevelShapes, const QList<KoShape*> &shapes);

private:
    KarbonDocument * m_document;
};

#endif
