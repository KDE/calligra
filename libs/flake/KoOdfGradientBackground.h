/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2011 Lukáš Tvrdý <lukas.tvrdy@ixonos.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOODFGRADIENTBACKGROUND_H
#define KOODFGRADIENTBACKGROUND_H

#include "KoShapeBackground.h"
#include "flake_export.h"

class QImage;

class KoOdfGradientBackgroundPrivate;
class KoXmlElement;
class KoGenStyles;
class KoGenStyle;

/// Gradients from odf that are not native to Qt
class FLAKE_EXPORT KoOdfGradientBackground : public KoShapeBackground
{
public:
    // constructor
    KoOdfGradientBackground();
    // destructor
    ~KoOdfGradientBackground() override;

    /// reimplemented from KoShapeBackground
    void fillStyle(KoGenStyle &style, KoShapeSavingContext &context) override;
    /// reimplemented from KoShapeBackground
    bool loadStyle(KoOdfLoadingContext &context, const QSizeF &shapeSize) override;
    /// reimplemented from KoShapeBackground
    void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &context, const QPainterPath &fillPath) const override;

private:
    bool loadOdf(const KoXmlElement &element);
    void saveOdf(KoGenStyle &styleFill, KoGenStyles &mainStyles) const;

    void renderSquareGradient(QImage &buffer) const;
    void renderRectangleGradient(QImage &buffer) const;

private:
    void debug() const;

private:
    Q_DECLARE_PRIVATE(KoOdfGradientBackground)
    Q_DISABLE_COPY(KoOdfGradientBackground)
};

#endif
