#ifndef EXAMPLE_FACTORY_H
#define EXAMPLE_FACTORY_H

#include <klibloader.h>

class KInstance;

class ExampleFactory : public KLibFactory
{
    Q_OBJECT
public:
    ExampleFactory( QObject* parent = 0, const char* name = 0 );
    ~ExampleFactory();

    virtual QObject* create( QObject* parent = 0, const char* name = 0, const char* classname = "QObject", const QStringList &args = QStringList() );

    static KInstance* global();

private:
    static KInstance* s_global;
};

#endif
