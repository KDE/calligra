#ifndef HTMLEXPORT_FACTORY_H
#define HTMLEXPORT_FACTORY_H

#include <klibloader.h>

class KInstance;

class HTMLExportFactory : public KLibFactory
{
    Q_OBJECT
public:
    HTMLExportFactory( QObject* parent = 0, const char* name = 0 );
    virtual ~HTMLExportFactory();

    virtual QObject* create( QObject* parent = 0, const char* name = 0, const char* classname = "QObject" );

    static KInstance* global();

private:
    static KInstance* s_global;
};
#endif
