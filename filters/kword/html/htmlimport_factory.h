#ifndef HTMLIMPORT_FACTORY_H
#define HTMLIMPORT_FACTORY_H

#include <klibloader.h>

class KInstance;

class HTMLImportFactory : public KLibFactory
{
    Q_OBJECT
public:
    HTMLImportFactory( QObject* parent = 0, const char* name = 0 );
    virtual ~HTMLImportFactory();

    virtual QObject* create( QObject* parent = 0, const char* name = 0, const char* classname = "QObject" );

    static KInstance* global();

private:
    static KInstance* s_global;
};
#endif
