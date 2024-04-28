/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOCOLORBACKGROUND_H
#define KOCOLORBACKGROUND_H

#include "KoShapeBackground.h"
#include "flake_export.h"
#include <Qt>

class KoColorBackgroundPrivate;
class QColor;

/// A simple solid color shape background
class FLAKE_EXPORT KoColorBackground : public KoShapeBackground
{
public:
    KoColorBackground();

    /// Creates background from given color and style
    explicit KoColorBackground(const QColor &color, Qt::BrushStyle style = Qt::SolidPattern);

    ~KoColorBackground() override;

    /// Returns the background color
    QColor color() const;

    /// Sets the background color
    void setColor(const QColor &color);

    /// Returns the background style
    Qt::BrushStyle style() const;

    // reimplemented from KoShapeBackground
    void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &context, const QPainterPath &fillPath) const override;
    // reimplemented from KoShapeBackground
    void fillStyle(KoGenStyle &style, KoShapeSavingContext &context) override;
    // reimplemented from KoShapeBackground
    bool loadStyle(KoOdfLoadingContext &context, const QSizeF &shapeSize) override;

protected:
    KoColorBackground(KoShapeBackgroundPrivate &dd);

private:
    Q_DECLARE_PRIVATE(KoColorBackground)
    Q_DISABLE_COPY(KoColorBackground)
};

#endif // KOCOLORBACKGROUND_H
