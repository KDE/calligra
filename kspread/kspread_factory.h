#ifndef KSPREAD_FACTORY_H
#define KSPREAD_FACTORY_H

#include <koFactory.h>

class KInstance;
class KAboutData;
class DCOPObject;

class KSpreadFactory : public KoFactory
{
    Q_OBJECT
public:
    KSpreadFactory( QObject* parent = 0, const char* name = 0 );
    ~KSpreadFactory();

    virtual KParts::Part *createPart( QWidget *parentWidget = 0, const char *widgetName = 0, QObject *parent = 0, const char *name = 0, const char *classname = "KoDocument", const QStringList &args = QStringList() );

    static KInstance* global();

    // _Creates_ a KAboutData but doesn't keep ownership
    static KAboutData* aboutData();

    static DCOPObject* dcopObject();

private:
    static KInstance* s_global;
    static DCOPObject* s_dcopObject;
    static KAboutData* s_aboutData;
};

#endif
