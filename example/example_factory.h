#ifndef EXAMPLE_FACTORY_H
#define EXAMPLE_FACTORY_H

#include <klibloader.h>

class KLibGlobal;

class ExampleFactory : public KLibFactory
{
    Q_OBJECT
public:
    ExampleFactory( QObject* parent = 0, const char* name = 0 );
    ~ExampleFactory();

    virtual QObject* create( QObject* parent = 0, const char* name = 0, const char* classname = "QObject" );

    static KLibGlobal* global();

private:
    static KLibGlobal* s_global;
};

#endif
