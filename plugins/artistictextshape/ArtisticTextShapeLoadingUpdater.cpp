/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ArtisticTextShapeLoadingUpdater.h"

#include "ArtisticTextShape.h"
#include <KoPathShape.h>

ArtisticTextShapeLoadingUpdater::ArtisticTextShapeLoadingUpdater(ArtisticTextShape *artisticTextShape)
    : m_artisticTextShape(artisticTextShape)
{
}

ArtisticTextShapeLoadingUpdater::~ArtisticTextShapeLoadingUpdater() = default;

void ArtisticTextShapeLoadingUpdater::update(KoShape *shape)
{
    // we have already loaded the correct transformation, so save it here
    // and apply after putting us on the path shape
    QTransform matrix = m_artisticTextShape->transformation();

    // putting us on the path shape resulting in a changed transformation
    m_artisticTextShape->putOnPath(dynamic_cast<KoPathShape *>(shape));

    // resetting the transformation to the former state
    m_artisticTextShape->setTransformation(matrix);
}
