#ifndef CALLIGRALIBS_KoSegmentGradient_H
#define CALLIGRALIBS_KoSegmentGradient_H

#include <KoAbstractGradient.h>

class KoSegmentGradient : public KoAbstractGradient
{
public:
    KoSegmentGradient(const QString &filename) : KoAbstractGradient(filename) {}
};

#endif
