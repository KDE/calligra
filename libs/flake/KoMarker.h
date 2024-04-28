/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOMARKER_H
#define KOMARKER_H

#include <QMetaType>
#include <QSharedData>

#include "flake_export.h"

class KoXmlElement;
class KoShapeLoadingContext;
class KoShapeSavingContext;
class QString;
class QPainterPath;

class FLAKE_EXPORT KoMarker : public QSharedData
{
public:
    KoMarker();
    ~KoMarker();

    /**
     * Load the marker
     *
     * @param element The xml element containing the marker
     * @param context The shape loading context
     */
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context);

    /**
     * Save the marker
     *
     * @return The reference of the marker.
     */
    QString saveOdf(KoShapeSavingContext &context) const;

    /**
     * Display name of the marker
     *
     * @return Display name of the marker
     */
    QString name() const;

    /**
     * Get the path of the marker
     *
     * It calculates the offset depending on the line width
     *
     * @param The width of the line the marker is attached to.
     * @return the path of the marker
     */
    QPainterPath path(qreal width) const;

    bool operator==(const KoMarker &other) const;

private:
    class Private;
    Private *const d;
};

Q_DECLARE_METATYPE(KoMarker *)

#endif /* KOMARKER_H */
