#ifndef OLEFILTER_FACTORY_H
#define OLEFILTER_FACTORY_H

#include <loader.h>

class KInstance;

class OLEFilterFactory : public Factory
{
    Q_OBJECT
public:
    OLEFilterFactory( QObject* parent = 0, const char* name = 0 );
    virtual ~OLEFilterFactory();

    virtual QObject* create( QObject* parent = 0, const char* name = 0 );

    static KInstance* global();

private:
    static KInstance* s_global;
};
#endif
