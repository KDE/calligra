#ifndef CSVFILTER_FACTORY_H
#define CSVFILTER_FACTORY_H

#include <klibloader.h>

class KLibGlobal;

class CSVFilterFactory : public KLibFactory
{
    Q_OBJECT
public:
    CSVFilterFactory( QObject* parent = 0, const char* name = 0 );
    virtual ~CSVFilterFactory();

    virtual QObject* create( QObject* parent = 0, const char* name = 0, const char* classname = "QObject" );

    static KLibGlobal* global();

private:
    static KLibGlobal* s_global;
};
#endif
