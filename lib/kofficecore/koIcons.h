#ifndef KOICONS_H
#define KOICONS_H

#include <kiconloader.h>

#define KofficeBarIcon( x ) BarIcon( x, KoIcons::global() )

class KoIcons
{
public:
    static KLibGlobal* global();
    
private:
    static KLibGlobal* s_global;
};

#endif
