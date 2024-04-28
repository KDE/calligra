/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef PDF2ODGIMPORT_H
#define PDF2ODGIMPORT_H

#include <KoFilter.h>
#include <QObject>
#include <QVariantList>

class QFile;

class KarbonDocument;
class KoXmlElement;
class KoShape;

class Pdf2OdgImport : public KoFilter
{
    Q_OBJECT

public:
    Pdf2OdgImport(QObject *parent, const QVariantList &);
    ~Pdf2OdgImport() override;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;

    KoFilter::ConversionStatus convert(int pageNumber, QFile &in);
    void convert(const KoXmlElement &rootElement, int pageNumber);
    void buildDocument(const QList<KoShape *> &toplevelShapes, const QList<KoShape *> &shapes, int pageNumber);

private:
    KarbonDocument *m_document;
};

#endif
