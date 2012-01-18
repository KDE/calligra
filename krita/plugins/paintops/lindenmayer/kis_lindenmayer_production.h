#ifndef KIS_LINDENMAYER_PRODUCTION_H
#define KIS_LINDENMAYER_PRODUCTION_H

#include <QList>

class KisLindenmayerLetter;

class KisLindenmayerProduction
{
public:
    KisLindenmayerProduction();

    // this can change the letter and return zero or more new letters.
    QList<KisLindenmayerLetter*> produce(KisLindenmayerLetter* letter);
};

#endif // KIS_LINDENMAYER_PRODUCTION_H
