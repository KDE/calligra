#ifndef OLEFILTER_FACTORY_H
#define OLEFILTER_FACTORY_H

#include <loader.h>

class KLibGlobal;

class OLEFilterFactory : public Factory
{
    Q_OBJECT
public:
    OLEFilterFactory( QObject* parent = 0, const char* name = 0 );
    virtual ~OLEFilterFactory();

    virtual QObject* create( QObject* parent = 0, const char* name = 0 );

    static KLibGlobal* global();

private:
    static KLibGlobal* s_global;
};
#endif
