#ifndef ASCIIEXPORT_FACTORY_H
#define ASCIIEXPORT_FACTORY_H

#include <loader.h>

class KLibGlobal;

class ASCIIExportFactory : public Factory
{
    Q_OBJECT
public:
    ASCIIExportFactory( QObject* parent = 0, const char* name = 0 );
    virtual ~ASCIIExportFactory();

    virtual QObject* create( QObject* parent = 0, const char* name = 0 );

    static KLibGlobal* global();

private:
    static KLibGlobal* s_global;
};
#endif
