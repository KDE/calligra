#ifndef KOICONS_H
#define KOICONS_H

class KInstance;

#include <kiconloader.h>

#define KofficeBarIcon( x ) BarIcon( x, KoIcons::global() )

class KoIcons
{
public:
    static KInstance* global();
    
private:
    static KInstance* s_global;
};

#endif
