#ifndef KILLUSTRATOR_FACTORY_H
#define KILLUSTRATOR_FACTORY_H

#include <koFactory.h>

class KInstance;
class KAboutData;

class KIllustratorFactory : public KoFactory
{
    Q_OBJECT
public:
    KIllustratorFactory( QObject* parent = 0, const char* name = 0 );
    ~KIllustratorFactory();

    virtual KParts::Part *createPart( QWidget *parentWidget = 0, const char *widgetName = 0, QObject *parent = 0, const char *name = 0, const char *classname = "KoDocument", const QStringList &args = QStringList() );

    static KInstance* global();
    static KAboutData* aboutData();

private:
    static KInstance* s_global;
    static KAboutData* s_aboutData;
};

#endif
