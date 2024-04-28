/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2012 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOHATCHBACKGROUND_H
#define KOHATCHBACKGROUND_H

#include "KoColorBackground.h"

class KoHatchBackgroundPrivate;

/**
 * A hatch shape background
 */
class FLAKE_EXPORT KoHatchBackground : public KoColorBackground
{
public:
    enum HatchStyle { Single, Double, Triple };

    KoHatchBackground();

    // reimplemented
    void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &context, const QPainterPath &fillPath) const override;

    // reimplemented
    void fillStyle(KoGenStyle &style, KoShapeSavingContext &context) override;

    // reimplemented
    bool loadStyle(KoOdfLoadingContext &context, const QSizeF &shapeSize) override;

private:
    QString saveHatchStyle(KoShapeSavingContext &context) const;

    Q_DECLARE_PRIVATE(KoHatchBackground)
    Q_DISABLE_COPY(KoHatchBackground)
};

#endif /* KOHATCHBACKGROUND_H */
