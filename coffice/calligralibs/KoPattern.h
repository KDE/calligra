#ifndef CALLIGRALIBS_KOPATTERN_H
#define CALLIGRALIBS_KOPATTERN_H

#include <KoResource.h>

class KoPattern : public KoResource
{
public:
    KoPattern(const QString &filename) : KoResource(filename) {}
};

#endif
