/*
    SPDX-FileCopyrightText: 2007 Sven Langkamp <sven.langkamp@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef KOABSTRACTGRADIENT_H
#define KOABSTRACTGRADIENT_H

#include <QGradient>
#include <QMetaType>

#include "KoColorSpace.h"
#include "KoResource.h"
#include <pigment_export.h>

class KoColor;

/**
 * KoAbstractGradient is the base class of all gradient resources
 */
class PIGMENTCMS_EXPORT KoAbstractGradient : public KoResource
{
public:
    explicit KoAbstractGradient(const QString &filename);
    ~KoAbstractGradient() override;

    virtual KoAbstractGradient *clone() const = 0;

    bool load() override
    {
        return false;
    }

    bool loadFromDevice(QIODevice *) override
    {
        return false;
    }

    bool save() override
    {
        return false;
    }

    bool saveToDevice(QIODevice *) const override
    {
        return false;
    }

    /**
     * Creates a QGradient from the gradient.
     * The resulting QGradient might differ from original gradient
     */
    virtual QGradient *toQGradient() const
    {
        return new QGradient();
    }

    /// gets the color at position 0 <= t <= 1
    virtual void colorAt(KoColor &, qreal t) const;

    void setColorSpace(KoColorSpace *colorSpace);
    const KoColorSpace *colorSpace() const;

    void setSpread(QGradient::Spread spreadMethod);
    QGradient::Spread spread() const;

    void setType(QGradient::Type repeatType);
    QGradient::Type type() const;

    void updatePreview();

    QImage generatePreview(int width, int height) const;

    KoAbstractGradient(const KoAbstractGradient &rhs);

private:
    struct Private;
    Private *const d;
};

Q_DECLARE_METATYPE(KoAbstractGradient *)

#endif // KOABSTRACTGRADIENT_H
