/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KOMARKERDATA_H
#define KOMARKERDATA_H

#include <QtGlobal>

#include "flake_export.h"

class KoGenStyle;
class KoMarker;
class KoShapeLoadingContext;
class KoShapeSavingContext;

class FLAKE_EXPORT KoMarkerData
{
public:
    /// Property enum
    enum MarkerPosition {
        MarkerStart, ///< it is the marker where the Path starts
        MarkerEnd ///< it is the marker where the Path ends
    };

    KoMarkerData(KoMarker *marker, qreal width, MarkerPosition position, bool center);
    explicit KoMarkerData(MarkerPosition position);
    KoMarkerData(const KoMarkerData &other);
    ~KoMarkerData();

    /**
     * Get the marker
     *
     * @return the marker or 0 if no marker is set
     */
    KoMarker *marker() const;

    /**
     * Set the marker
     *
     * @param marker The marker that is set or 0 to remove the marker
     */
    void setMarker(KoMarker *marker);

    /**
     * Get the with of the marker according to the pen width
     */
    qreal width(qreal penWidth) const;

    /**
     * Set the width of the marker
     *
     * This calculates a base width for the marker so the width of the marker changes
     * with the width of the line.
     *
     * @param width The width of the marker
     * @param penWidth The width of the used pen
     */
    void setWidth(qreal width, qreal penWidth);

    /**
     * Get the position of the marker
     *
     * @return Position of the marker
     */
    MarkerPosition position() const;

    /**
     * Set the position of the marker
     *
     * @param position Position of the marker
     */
    void setPosition(MarkerPosition position);

    /**
     * Get the center property of the marker
     *
     * If the marker is centered at the start of the stroke the line will get longer.
     *
     * @return Returns true if the marker is centered at the start of the stroke.
     */
    bool center() const;

    /**
     * Set the center property of the marker
     *
     * @param center If set to true the marker should be centered at the start of the stroke.
     */
    void setCenter(bool center);

    /**
     * Compare the marker data
     */
    KoMarkerData &operator=(const KoMarkerData &other);

    /**
     * Load the marker data
     *
     * @param penWidth the used pen width of the line
     * @param context The shape loading context
     */
    bool loadOdf(qreal penWidth, KoShapeLoadingContext &context);

    /**
     * Save the marker data to the style
     *
     * @param style The style that we add the marker data to
     * @param penWidth the used pen width of the line
     * @param context The shape saving context
     */
    void saveStyle(KoGenStyle &style, qreal penWidth, KoShapeSavingContext &context) const;

private:
    // make private to be sure it is not used
    KoMarkerData();

    class Private;
    Private *const d;
};

#endif /* KOMARKERDATA_H */
