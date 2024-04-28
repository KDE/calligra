/*
    SPDX-FileCopyrightText: 2007 Sven Langkamp <sven.langkamp@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef KOSTOPGRADIENT_H
#define KOSTOPGRADIENT_H

#include <QGradient>
#include <QPair>

#include "KoAbstractGradient.h"
#include "KoColor.h"
#include "KoResource.h"
#include <pigment_export.h>

typedef QPair<qreal, KoColor> KoGradientStop;

/**
 * Resource for colorstop based gradients like Karbon gradients and SVG gradients
 */
class PIGMENTCMS_EXPORT KoStopGradient : public KoAbstractGradient
{
public:
    explicit KoStopGradient(const QString &filename);
    ~KoStopGradient() override;

    KoAbstractGradient *clone() const override;

    bool load() override;
    bool loadFromDevice(QIODevice *dev) override;
    bool save() override;
    bool saveToDevice(QIODevice *dev) const override;

    /// reimplemented
    QGradient *toQGradient() const override;

    /// reimplemented
    void colorAt(KoColor &, qreal t) const override;

    /// Creates KoStopGradient from a QGradient
    static KoStopGradient *fromQGradient(QGradient *gradient);

    /// Sets the gradient stops
    void setStops(QList<KoGradientStop> stops);
    QList<KoGradientStop> stops() const;

    /// reimplemented
    QString defaultFileExtension() const override;

protected:
    QList<KoGradientStop> m_stops;
    QPointF m_start;
    QPointF m_stop;
    QPointF m_focalPoint;

private:
    mutable KoColor buffer;

private:
    void loadKarbonGradient(QIODevice *file);
    void parseKarbonGradient(const QDomElement &element);

    void loadSvgGradient(QIODevice *file);
    void parseSvgGradient(const QDomElement &element);
    void parseSvgColor(QColor &color, const QString &s);
};

#endif // KOSTOPGRADIENT_H
