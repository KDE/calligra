/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2002-2003, 2005 Rob Buis <buis@kde.org>
 * SPDX-FileCopyrightText: 2005-2006 Tim Beaulen <tbscope@gmail.com>
 * SPDX-FileCopyrightText: 2005, 2007-2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SVGIMPORT_H
#define SVGIMPORT_H

#include <KoFilter.h>
#include <KoXmlReaderForward.h>
#include <QVariant>

class KoShape;
class KarbonDocument;

class SvgImport : public KoFilter
{
    Q_OBJECT

public:
    SvgImport(QObject *parent, const QVariantList &);
    ~SvgImport() override;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;

protected:
    /// The main entry point for the conversion
    void convert(const KoXmlElement &rootElement);

    /// Builds the document from the given shapes list
    void buildDocument(const QList<KoShape *> &toplevelShapes, const QList<KoShape *> &shapes);

private:
    KarbonDocument *m_document;
};

#endif
