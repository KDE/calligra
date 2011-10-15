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

#include "KoMarkerData.h"

#include <kdebug.h>
#include <KoStyleStack.h>
#include <KoXmlNS.h>
#include <KoUnit.h>
#include <KoOdfLoadingContext.h>
#include <KoGenStyle.h>
#include "KoShapeLoadingContext.h"
#include "KoMarkerSharedLoadingData.h"

static const struct {
    const char * m_markerPositionLoad;
    const char * m_markerWidthLoad;
    const char * m_markerCenterLoad;
    const char * m_markerPositionSave;
    const char * m_markerWidthSave;
    const char * m_markerCenterSave;
} markerOdfData[] = {
    { "marker-start", "marker-start-width", "marker-start-center", "draw:marker-start", "draw:marker-start-width", "draw:marker-start-center" },
    { "marker-end"  , "marker-end-width",   "marker-end-center", "draw:marker-end"  , "draw:marker-end-width",   "draw:marker-end-center" }
};

KoMarkerData::KoMarkerData(KoMarker *marker, qreal width, MarkerPosition position, bool center)
: m_marker(marker)
, m_width(width)
, m_position(position)
, m_center(center)
{
}

KoMarkerData::KoMarkerData(MarkerPosition position)
: m_marker(0)
, m_width(0)
, m_position(position)
, m_center(false)
{
}

KoMarkerData::~KoMarkerData()
{
}

KoMarker *KoMarkerData::marker() const
{
    return m_marker;
}

void KoMarkerData::setMarker(KoMarker *marker)
{
    m_marker = marker;
}

qreal KoMarkerData::width() const
{
    return m_width;
}

void KoMarkerData::setWidth(qreal width)
{
    m_width = width;
}

KoMarkerData::MarkerPosition KoMarkerData::position() const
{
    return m_position;
}

void KoMarkerData::setPosition(MarkerPosition position)
{
    m_position = position;
}

bool KoMarkerData::center() const
{
    return m_center;
}

void KoMarkerData::setCenter(bool center)
{
    m_center = center;
}

KoMarkerData &KoMarkerData::operator=(const KoMarkerData &other)
{
    if (this != &other) {
        m_marker = other.m_marker;
        m_width = other.m_width;
        m_position = other.m_position;
        m_center = other.m_center;
    }
    return (*this);
}

bool KoMarkerData::loadOdf(qreal penWidth, KoShapeLoadingContext &context)
{
    KoMarkerSharedLoadingData *markerShared = dynamic_cast<KoMarkerSharedLoadingData*>(context.sharedData(MARKER_SHARED_LOADING_ID));
    if (markerShared) {
        KoStyleStack &styleStack = context.odfLoadingContext().styleStack();
        // draw:marker-end="Arrow" draw:marker-end-width="0.686cm" draw:marker-end-center="true"
        const QString markerStart(styleStack.property(KoXmlNS::draw, markerOdfData[m_position].m_markerPositionLoad));
        const QString markerStartWidth(styleStack.property(KoXmlNS::draw, markerOdfData[m_position].m_markerWidthLoad));
        if (!markerStart.isEmpty() && !markerStartWidth.isEmpty()) {
            KoMarker *marker = markerShared->marker(markerStart);
            if (marker) {
                setMarker(marker);
                qreal markerWidth = KoUnit::parseValue(markerStartWidth);
                qreal markerBaseWidth = qMax(qreal(0.0), markerWidth - penWidth * 1.5);
                kDebug(30006) << markerWidth << markerBaseWidth << penWidth * 1.5;
                setWidth(markerBaseWidth);
                setCenter(styleStack.property(KoXmlNS::draw, markerOdfData[m_position].m_markerCenterLoad) == "true");
            }
        }
    }
    return true;
}

void KoMarkerData::saveStyle(KoGenStyle &style, qreal lineWidth, KoShapeSavingContext &context) const
{
    if (m_marker) {
        style.addProperty(markerOdfData[m_position].m_markerPositionSave, "foo", KoGenStyle::GraphicType);
        style.addPropertyPt(markerOdfData[m_position].m_markerWidthSave, m_width + lineWidth * 1.5, KoGenStyle::GraphicType);
        style.addProperty(markerOdfData[m_position].m_markerCenterSave, m_center, KoGenStyle::GraphicType);
    }
}
