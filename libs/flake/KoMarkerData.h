/* This file is part of the KDE project
   Copyright (C) 2011 Thorsten Zachmann <zachmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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
        MarkerBegin, ///< it is the marker where the Path begins
        MarkerEnd ///< it is the marker where the Path ends
    };

    KoMarkerData(KoMarker *marker, qreal width, MarkerPosition position, bool center);
    KoMarkerData(MarkerPosition position);
    ~KoMarkerData();

    KoMarker *marker() const;
    void setMarker(KoMarker *marker);

    qreal width() const;
    void setWidth(qreal width);

    MarkerPosition position() const;
    void setPosition(MarkerPosition position);

    bool center() const;
    void setCenter(bool center);

    KoMarkerData &operator=(const KoMarkerData &other);

    bool loadOdf(qreal penWidth, KoShapeLoadingContext &context);
    void saveStyle(KoGenStyle &style, qreal lineWidth, KoShapeSavingContext &context) const;

private:
    KoMarker *m_marker;
    qreal m_width;
    MarkerPosition m_position;
    bool m_center;
};

#endif /* KOMARKERDATA_H */
