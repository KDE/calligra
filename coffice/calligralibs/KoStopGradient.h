#ifndef CALLIGRALIBS_KoStopGradient_H
#define CALLIGRALIBS_KoStopGradient_H

#include <KoAbstractGradient.h>
#include <QPair>

typedef QPair<qreal, KoColor> KoGradientStop;

class KoStopGradient : public KoAbstractGradient
{
public:
    KoStopGradient(const QString &filename) : KoAbstractGradient(filename) {}
    
    static KoStopGradient * fromQGradient(QGradient * gradient) { Q_ASSERT(false); return 0; }
    void setStops(QList<KoGradientStop> stops) {}

};

#endif
