#ifndef KOMARKERDATA_H
#define KOMARKERDATA_H

#include <QtGlobal>

#include "flake_export.h"

class KoMarker;

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

private:
    KoMarker *m_marker;
    qreal m_width;
    MarkerPosition m_position;
    bool m_center;
};

#endif /* KOMARKERDATA_H */
