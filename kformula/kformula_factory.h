#ifndef KFORMULA_FACTORY_H
#define KFORMULA_FACTORY_H

#include <klibloader.h>
#include <kaboutdata.h>

class KInstance;

class KFormulaFactory : public KLibFactory
{
    Q_OBJECT
public:
    KFormulaFactory( QObject* parent = 0, const char* name = 0 );
    ~KFormulaFactory();

    virtual QObject* create( QObject* parent = 0, const char* name = 0, const char* classname = "QObject", const QStringList &args = QStringList() );

    static KInstance* global();

    static KAboutData* aboutData();


private:
    static KInstance* s_global;
};

#endif
