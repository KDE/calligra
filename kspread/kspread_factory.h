#ifndef KSPREAD_FACTORY_H
#define KSPREAD_FACTORY_H

#include <klibloader.h>

class KInstance;
class KAboutData;

class KSpreadFactory : public KLibFactory
{
    Q_OBJECT
public:
    KSpreadFactory( QObject* parent = 0, const char* name = 0 );
    ~KSpreadFactory();

    virtual QObject* create( QObject* parent = 0, const char* oname = 0, const char* name = "QObject", const QStringList &args = QStringList() );

    static KInstance* global();

    // _Creates_ a KAboutData but doesn't keep ownership
    static KAboutData* aboutData();

private:
    static KInstance* s_global;
};

#endif
