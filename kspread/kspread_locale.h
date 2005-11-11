#ifndef KSPREAD_LOCALE_H
#define KSPREAD_LOCALE_H

#include <klocale.h>

class QDomElement;
class QDomDocument;

class Locale : public KLocale
{
public:
    Locale();
    
    void load( const QDomElement& element );
    QDomElement save( QDomDocument& doc ) const;
    void defaultSystemConfig();
};

#endif
