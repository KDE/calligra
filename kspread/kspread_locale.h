#ifndef KSPREAD_LOCALE_H
#define KSPREAD_LOCALE_H

#include <klocale.h>

class QDomElement;
class QDomDocument;

class KSpreadLocale : public KLocale
{
public:
    KSpreadLocale();
    
    void read( const QDomElement& element );
    QDomElement write( QDomDocument& doc ) const;
};

#endif
