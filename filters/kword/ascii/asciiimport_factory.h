#ifndef ASCIIIMPORT_FACTORY_H
#define ASCIIIMPORT_FACTORY_H

#include <klibloader.h>

class KInstance;

class ASCIIImportFactory : public KLibFactory
{
    Q_OBJECT
public:
    ASCIIImportFactory( QObject* parent = 0, const char* name = 0 );
    virtual ~ASCIIImportFactory();

    virtual QObject* create( QObject* parent = 0, const char* name = 0, const char* classname = "QObject" );

    static KInstance* global();

private:
    static KInstance* s_global;
};
#endif
