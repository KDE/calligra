#ifndef ASCIIIMPORT_FACTORY_H
#define ASCIIIMPORT_FACTORY_H

#include <loader.h>

class KLibGlobal;

class ASCIIImportFactory : public Factory
{
    Q_OBJECT
public:
    ASCIIImportFactory( QObject* parent = 0, const char* name = 0 );
    virtual ~ASCIIImportFactory();

    virtual QObject* create( QObject* parent = 0, const char* name = 0 );

    static KLibGlobal* global();

private:
    static KLibGlobal* s_global;
};
#endif
