#ifndef ASCIIEXPORT_FACTORY_H
#define ASCIIEXPORT_FACTORY_H

#include <klibloader.h>

class KInstance;

class ASCIIExportFactory : public KLibFactory
{
    Q_OBJECT
public:
    ASCIIExportFactory( QObject* parent = 0, const char* name = 0 );
    virtual ~ASCIIExportFactory();

    virtual QObject* create( QObject* parent = 0, const char* name = 0, const char* classname = "QObject" );

    static KInstance* global();

private:
    static KInstance* s_global;
};
#endif
