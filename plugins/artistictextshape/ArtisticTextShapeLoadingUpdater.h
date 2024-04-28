/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef ARTISTICTEXTSHAPELOADINGUPDATER_H
#define ARTISTICTEXTSHAPELOADINGUPDATER_H

#include <KoLoadingShapeUpdater.h>

class ArtisticTextShape;

class ArtisticTextShapeLoadingUpdater : public KoLoadingShapeUpdater
{
public:
    explicit ArtisticTextShapeLoadingUpdater(ArtisticTextShape *artisticTextShape);
    ~ArtisticTextShapeLoadingUpdater() override;
    void update(KoShape *shape) override;

private:
    ArtisticTextShape *m_artisticTextShape;
};

#endif // ARTISTICTEXTSHAPELOADINGUPDATER_H
