#ifndef ASCIIIMPORT_FACTORY_H
#define ASCIIIMPORT_FACTORY_H

#include <klibloader.h>

class KLibGlobal;

class ASCIIImportFactory : public KLibFactory
{
    Q_OBJECT
public:
    ASCIIImportFactory( QObject* parent = 0, const char* name = 0 );
    virtual ~ASCIIImportFactory();

    virtual QObject* create( QObject* parent = 0, const char* name = 0, const char* classname = "QObject" );

    static KLibGlobal* global();

private:
    static KLibGlobal* s_global;
};
#endif
