#ifndef KOSCRIPT_LOCALE_H
#define KOSCRIPT_LOCALE_H

#include <klocale.h>

/**
 * This locale is used when executing external KoScript scripts.
 * This locale says, that it assumes numbers to be formatted like
 * in C,C++: 12345.6789
 * Dates are formatted like in the US: 10/31/99
 * Time is formatted like this: 12:43:56
 */
class KSLocale : public KLocale
{
public:
    KSLocale();
};

#endif
