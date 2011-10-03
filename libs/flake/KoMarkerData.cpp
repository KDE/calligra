#include "KoMarkerData.h"

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
