#ifndef KSPREAD_FACTORY_H
#define KSPREAD_FACTORY_H

#include <klibloader.h>

class KInstance;

class KSpreadFactory : public KLibFactory
{
    Q_OBJECT
public:
    KSpreadFactory( QObject* parent = 0, const char* name = 0 );
    ~KSpreadFactory();

    virtual QObject* create( QObject* parent = 0, const char* name = 0, const char* name = "QObject" );

    static KInstance* global();

private:
    static KInstance* s_global;
};

#endif
