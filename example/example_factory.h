#ifndef EXAMPLE_FACTORY_H
#define EXAMPLE_FACTORY_H

#include <koFactory.h>

class KInstance;
class KAboutData;

class ExampleFactory : public KoFactory
{
    Q_OBJECT
public:
    ExampleFactory( QObject* parent = 0, const char* name = 0 );
    ~ExampleFactory();

    virtual QObject* create( QObject* parent = 0, const char* name = 0, const char* classname = "QObject", const QStringList &args = QStringList() );

    static KInstance* global();

    // _Creates_ a KAboutData but doesn't keep ownership
    static KAboutData* aboutData();

private:
    static KInstance* s_global;
};

#endif
