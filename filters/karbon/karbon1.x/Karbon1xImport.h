/* This file is part of the KDE project
 * Copyright (C) 2007,2011 Jan Hambrecht <jaham@gmx.net>
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

#ifndef KARBON_IMPORT_H
#define KARBON_IMPORT_H

#include <KoFilter.h>
#include <KoXmlReader.h>
#include <QVariant>
#include <QTransform>
#include <QStack>

class KoXmlWriter;

class KarbonImport : public KoFilter
{
    Q_OBJECT

public:
    KarbonImport(QObject* parent, const QVariantList&);
    ~KarbonImport() override;

    KoFilter::ConversionStatus convert(const QByteArray& from, const QByteArray& to) override;

protected:
    bool parseRoot(QIODevice* io);

    bool convert(const KoXmlDocument &);
    bool loadXML(const KoXmlElement& doc);

    QString loadStyle(const KoXmlElement &element);
    void loadCommon(const KoXmlElement &element, bool ignoreTransform = false);
    QString loadStroke(const KoXmlElement &element);
    QString loadFill(const KoXmlElement &element);
    QColor loadColor(const KoXmlElement &element);
    QVector<qreal> loadDashes(const KoXmlElement& element);
    QString loadGradient(const KoXmlElement &element);
    void loadColorStops(const KoXmlElement &element);
    QString loadPattern(const KoXmlElement &element);

    void loadGroup(const KoXmlElement &element);
    void loadPath(const KoXmlElement &element);
    void loadEllipse(const KoXmlElement &element);
    void loadRect(const KoXmlElement &element);
    void loadPolyline(const KoXmlElement &element);
    void loadPolygon(const KoXmlElement &element);
    void loadSinus(const KoXmlElement &element);
    void loadSpiral(const KoXmlElement &element);
    void loadStar(const KoXmlElement &element);
    void loadImage(const KoXmlElement &element);
    void loadText(const KoXmlElement &element);

    QString makeUnique(const QString &id);

private:
    QTransform m_mirrorMatrix;
    KoXmlWriter *m_svgWriter;
    QHash<QString, int> m_uniqueNames;
    QString m_lastId;
    QStack<QTransform> m_transformation;
};

#endif // KARBON_IMPORT_H
