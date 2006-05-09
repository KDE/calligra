#ifndef KFORMULA_FACTORY_H
#define KFORMULA_FACTORY_H

#include <KoFactory.h>

class KInstance;
class KInstance;
class KAboutData;

class KFormulaFactory : public KoFactory
{
    Q_OBJECT
public:
    KFormulaFactory( QObject* parent = 0 );
    ~KFormulaFactory();

    virtual KParts::Part *createPartObject( QWidget *parentWidget = 0, QObject *parent = 0, const char *classname = "KoDocument", const QStringList &args = QStringList() );

    static KInstance* global();

    static KAboutData* aboutData();

private:
    static KInstance* s_global;
    static KAboutData* s_aboutData;
};

#endif
