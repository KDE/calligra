/*
    SPDX-FileCopyrightText: 2007 Sven Langkamp <sven.langkamp@gmail.com>
    SPDX-FileCopyrightText: 2004 Adrian Page <adrian@pagenet.plus.com>
    SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>

    SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "KoAbstractGradient.h"
#include "KoColorSpaceRegistry.h"

#include <KoColor.h>

#include <QBuffer>
#include <QByteArray>

#define PREVIEW_WIDTH 64
#define PREVIEW_HEIGHT 64

struct Q_DECL_HIDDEN KoAbstractGradient::Private {
    const KoColorSpace *colorSpace;
    QGradient::Spread spread;
    QGradient::Type type;
};

KoAbstractGradient::KoAbstractGradient(const QString &filename)
    : KoResource(filename)
    , d(new Private)
{
    d->colorSpace = KoColorSpaceRegistry::instance()->rgb8();
    d->spread = QGradient::PadSpread;
    d->type = QGradient::NoGradient;
}

KoAbstractGradient::~KoAbstractGradient()
{
    delete d;
}

KoAbstractGradient::KoAbstractGradient(const KoAbstractGradient &rhs)
    : KoResource(rhs)
    , d(new Private(*rhs.d))
{
}

void KoAbstractGradient::colorAt(KoColor &, qreal t) const
{
    Q_UNUSED(t);
}

void KoAbstractGradient::setColorSpace(KoColorSpace *colorSpace)
{
    d->colorSpace = colorSpace;
}

const KoColorSpace *KoAbstractGradient::colorSpace() const
{
    return d->colorSpace;
}

void KoAbstractGradient::setSpread(QGradient::Spread spreadMethod)
{
    d->spread = spreadMethod;
}

QGradient::Spread KoAbstractGradient::spread() const
{
    return d->spread;
}

void KoAbstractGradient::setType(QGradient::Type repeatType)
{
    d->type = repeatType;
}

QGradient::Type KoAbstractGradient::type() const
{
    return d->type;
}

QImage KoAbstractGradient::generatePreview(int width, int height) const
{
    QImage image(width, height, QImage::Format_RGB32);

    const int checkerSize = 4;
    const int checkerSize_2 = 2 * checkerSize;
    const int darkBackground = 128;
    const int lightBackground = 128 + 63;

    QRgb *lineA = reinterpret_cast<QRgb *>(image.scanLine(0));
    QRgb *lineB = reinterpret_cast<QRgb *>(image.scanLine(checkerSize));

    KoColor c;
    QColor color;
    // first create the two reference lines
    for (int x = 0; x < image.width(); ++x) {
        qreal t = static_cast<qreal>(x) / (image.width() - 1);
        colorAt(c, t);
        c.toQColor(&color);
        const qreal alpha = color.alphaF();

        int darkR = static_cast<int>((1 - alpha) * darkBackground + alpha * color.red() + 0.5);
        int darkG = static_cast<int>((1 - alpha) * darkBackground + alpha * color.green() + 0.5);
        int darkB = static_cast<int>((1 - alpha) * darkBackground + alpha * color.blue() + 0.5);

        int lightR = static_cast<int>((1 - alpha) * lightBackground + alpha * color.red() + 0.5);
        int lightG = static_cast<int>((1 - alpha) * lightBackground + alpha * color.green() + 0.5);
        int lightB = static_cast<int>((1 - alpha) * lightBackground + alpha * color.blue() + 0.5);

        bool defColor = (x % checkerSize_2) < checkerSize;

        if (lineA)
            lineA[x] = defColor ? qRgb(darkR, darkG, darkB) : qRgb(lightR, lightG, lightB);
        if (lineB)
            lineB[x] = defColor ? qRgb(lightR, lightG, lightB) : qRgb(darkR, darkG, darkB);
    }

    int bytesPerLine = image.bytesPerLine();

    // now copy lines accordingly
    for (int y = 0; y < image.height(); ++y) {
        bool firstLine = (y % checkerSize_2) < checkerSize;
        QRgb *line = reinterpret_cast<QRgb *>(image.scanLine(y));
        if (line == lineA || line == lineB)
            continue;

        memcpy(line, firstLine ? lineA : lineB, bytesPerLine);
    }

    return image;
}

void KoAbstractGradient::updatePreview()
{
    setImage(generatePreview(PREVIEW_WIDTH, PREVIEW_HEIGHT));
}
