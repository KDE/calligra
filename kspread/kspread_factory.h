#ifndef KSPREAD_FACTORY_H
#define KSPREAD_FACTORY_H

#include <klibloader.h>

class KLibGlobal;

class KSpreadFactory : public KLibFactory
{
    Q_OBJECT
public:
    KSpreadFactory( QObject* parent = 0, const char* name = 0 );
    ~KSpreadFactory();

    virtual QObject* create( QObject* parent = 0, const char* name = 0, const char* name = "QObject" );

    static KLibGlobal* global();

private:
    static KLibGlobal* s_global;
};

#endif
