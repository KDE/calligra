#ifndef KIMAGESHOP_FACTORY_H
#define KIMAGESHOP_FACTORY_H

#include <klibloader.h>

class KInstance;

class KImageShopFactory : public KLibFactory
{
    Q_OBJECT
public:
    KImageShopFactory( QObject* parent = 0, const char* name = 0 );
    ~KImageShopFactory();

    virtual QObject* create( QObject* parent = 0, const char* name = 0, const char* classname = "QObject", const QStringList &args = QStringList() );

    static KInstance* global();

private:
    static KInstance* s_global;
};

#endif
