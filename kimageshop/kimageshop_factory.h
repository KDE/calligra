#ifndef KIMAGESHOP_FACTORY_H
#define KIMAGESHOP_FACTORY_H

#include <klibloader.h>

class KLibGlobal;

class KImageShopFactory : public KLibFactory
{
    Q_OBJECT
public:
    KImageShopFactory( QObject* parent = 0, const char* name = 0 );
    ~KImageShopFactory();

    virtual QObject* create( QObject* parent = 0, const char* name = 0, const char* classname = "QObject" );

    static KLibGlobal* global();

private:
    static KLibGlobal* s_global;
};

#endif
