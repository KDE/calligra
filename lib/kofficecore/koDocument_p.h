#ifndef __koDocument_p_h__
#define __koDocument_p_h__

#include <kparts/browserextension.h>

class KoDocument;

// Used in singleViewMode, when embedded into a browser
class KoBrowserExtension : public KParts::BrowserExtension
{
    Q_OBJECT
public:
    KoBrowserExtension( KoDocument * doc, const char * name = 0 );

public slots:
    // Automatically detected by konqueror
    void print();
};

#endif
