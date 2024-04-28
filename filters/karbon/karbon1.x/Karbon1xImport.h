/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KARBON_IMPORT_H
#define KARBON_IMPORT_H

#include <KoFilter.h>
#include <KoXmlReader.h>
#include <QStack>
#include <QTransform>
#include <QVariant>

class KoXmlWriter;

class KarbonImport : public KoFilter
{
    Q_OBJECT

public:
    KarbonImport(QObject *parent, const QVariantList &);
    ~KarbonImport() override;

    KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to) override;

protected:
    bool parseRoot(QIODevice *io);

    bool convert(const KoXmlDocument &);
    bool loadXML(const KoXmlElement &doc);

    QString loadStyle(const KoXmlElement &element);
    void loadCommon(const KoXmlElement &element, bool ignoreTransform = false);
    QString loadStroke(const KoXmlElement &element);
    QString loadFill(const KoXmlElement &element);
    QColor loadColor(const KoXmlElement &element);
    QVector<qreal> loadDashes(const KoXmlElement &element);
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
