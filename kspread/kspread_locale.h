#ifndef KSPREAD_LOCALE_H
#define KSPREAD_LOCALE_H

#include <klocale.h>

class QDomElement;
class QDomDocument;

class KSpreadLocale : public KLocale
{
public:
    KSpreadLocale();
    
    void load( const QDomElement& element );
    QDomElement save( QDomDocument& doc ) const;
};

#endif
