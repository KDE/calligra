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

    virtual KParts::Part *createPart( QWidget *parentWidget = 0, const char *widgetName = 0, QObject *parent = 0, const char *name = 0, const char *classname = "KoDocument", const QStringList &args = QStringList() );

    static KInstance* global();

    // _Creates_ a KAboutData but doesn't keep ownership
    static KAboutData* aboutData();

private:
    static KInstance* s_global;
};

#endif
