#ifndef CSVFILTER_FACTORY_H
#define CSVFILTER_FACTORY_H

#include <loader.h>

class KLibGlobal;

class CSVFilterFactory : public Factory
{
    Q_OBJECT
public:
    CSVFilterFactory( QObject* parent = 0, const char* name = 0 );
    virtual ~CSVFilterFactory();

    virtual QObject* create( QObject* parent = 0, const char* name = 0 );

    static KLibGlobal* global();

private:
    static KLibGlobal* s_global;
};
#endif
